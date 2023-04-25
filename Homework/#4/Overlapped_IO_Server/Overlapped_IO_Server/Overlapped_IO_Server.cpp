#include <iostream>
#include <WS2tcpip.h>
#include<unordered_map>
#include"../../protocol.h"
#include<shared_mutex>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")


void error_display(const char* msg, int err_no);

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag);

void disconnect(int id);


class EXP_OVER {
public:
	WSAOVERLAPPED _wsa_over;
	WSABUF _wsabuf;
	char _send_buf[BUF_SIZE];
	int c_id;
public:
	EXP_OVER()
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
	}
	EXP_OVER(int id)
	{
		_wsabuf.len = BUF_SIZE;
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		c_id = id;
	}

	EXP_OVER(char* packet ,int id)
	{
		_wsabuf.len = packet[0];
		_wsabuf.buf = _send_buf;
		ZeroMemory(&_wsa_over, sizeof(_wsa_over));
		memcpy(_send_buf, packet, packet[0]);
		SC_MOVE_PLAYER_PACKET* p = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(packet);
		p->type;
		SC_MOVE_PLAYER_PACKET* p2 = reinterpret_cast<SC_MOVE_PLAYER_PACKET*>(_send_buf);
		p2->type;

		c_id = id;
	}
};

enum S_STATE { ST_FREE, ST_ALLOC, ST_INGAME };
class SESSION {
	EXP_OVER _recv_over;

public:
	shared_mutex _s_lock;
	S_STATE _state;
	int c_id;
	SOCKET _socket;
	short	x, y;
	char	_name[NAME_SIZE];
	int		_prev_remain;
	int		_last_move_time;
public:
	SESSION()
	{
		DebugBreak();
		exit(-1);
	}
	SESSION(int id, SOCKET c_socket)
	{
		c_id = id;
		_socket = c_socket;
		x = y = 0;
		_name[0] = 0;
		_state = ST_FREE;
		_prev_remain = 0;
	}

	~SESSION() {}

	void do_recv()
	{
		DWORD recv_flag = 0;
		memset(&_recv_over._wsa_over, 0, sizeof(_recv_over._wsa_over));
		_recv_over._wsabuf.len = BUF_SIZE - _prev_remain;
		_recv_over._wsabuf.buf = _recv_over._send_buf + _prev_remain;
		_recv_over.c_id = c_id;
		WSARecv(_socket, &_recv_over._wsabuf, 1, 0, &recv_flag, &_recv_over._wsa_over, recv_callback);
	}

	void do_send(void* packet)
	{
		EXP_OVER* sdata = new EXP_OVER{ reinterpret_cast<char*>(packet), c_id };
		WSASend(_socket, &sdata->_wsabuf, 1, 0, 0, &sdata->_wsa_over, send_callback);
	}
	void send_login_info_packet()
	{
		SC_LOGIN_INFO_PACKET p;
		p.id = c_id;
		p.size = sizeof(SC_LOGIN_INFO_PACKET);
		p.type = SC_LOGIN_INFO;
		p.x = x = rand() % W_WIDTH;
		p.y = y = rand() % W_HEIGHT;
		do_send(&p);
	}
	void send_move_packet(int c_id);
	void send_add_player_packet(int c_id);
	void send_remove_player_packet(int c_id)
	{
		SC_REMOVE_PLAYER_PACKET p;
		p.id = c_id;
		p.size = sizeof(p);
		p.type = SC_REMOVE_PLAYER;
		do_send(&p);
	}
	void processpacket(unsigned char* buf);
};


unordered_map<int, SESSION> clients;
shared_mutex cl_lock;

void SESSION::processpacket(unsigned char* buf)
{
	switch (buf[1])
	{
	case CS_LOGIN:
	{
		CS_LOGIN_PACKET* packet = reinterpret_cast<CS_LOGIN_PACKET*>(buf);
		std::unique_lock<std::shared_mutex> lock(cl_lock);
		{
			strcpy_s(clients[c_id]._name, packet->name);
			clients[c_id].send_login_info_packet();
		}
		lock.unlock();

		{
			std::unique_lock<std::shared_mutex> {clients[c_id]._s_lock };
			clients[c_id]._state = ST_INGAME;
		}

		{
			std::shared_lock<std::shared_mutex> lock(cl_lock);
			for (auto& pl : clients)
			{
				{
					std::shared_lock<std::shared_mutex> clock(pl.second._s_lock);
					if (ST_INGAME != pl.second._state) 
						continue;
				}
				if (pl.first == c_id) continue;
				pl.second.send_add_player_packet(c_id);
				clients[c_id].send_add_player_packet(pl.second.c_id);

			}
		}
		break;
	}
	case CS_MOVE: {
		CS_MOVE_PACKET* p = reinterpret_cast<CS_MOVE_PACKET*>(buf);
		clients[c_id]._last_move_time = p->move_time;
		short x = clients[c_id].x;
		short y = clients[c_id].y;
		switch (p->direction) {
		case 0: if (y > 0) y--; break;
		case 1: if (y < W_HEIGHT - 1) y++; break;
		case 2: if (x > 0) x--; break;
		case 3: if (x < W_WIDTH - 1) x++; break;
		}
		clients[c_id].x = x;
		clients[c_id].y = y;

		for (auto& pl : clients) {
			if (pl.second._state != ST_INGAME) continue;
			pl.second.send_move_packet(c_id);
		}
		break;
	}
	default:
	{
		cout << "error packet type" << endl;
		while (1);
		break;
	}

	}
}
void SESSION::send_add_player_packet(int c_id)
{
	SC_ADD_PLAYER_PACKET add_packet;
	add_packet.id = c_id;
	strcpy_s(add_packet.name, clients[c_id]._name);
	add_packet.size = sizeof(add_packet);
	add_packet.type = SC_ADD_PLAYER;
	add_packet.x = clients[c_id].x;
	add_packet.y = clients[c_id].y;
	do_send(&add_packet);
}

void SESSION::send_move_packet(int c_id) 
{
	SC_MOVE_PLAYER_PACKET move_packet;
	move_packet.type = SC_MOVE_PLAYER;
	move_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
	move_packet.x = clients[c_id].x;
	move_packet.y = clients[c_id].y;
	move_packet.id = c_id;
	move_packet.move_time = clients[c_id]._last_move_time;
	do_send(&move_packet);
}


void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	EXP_OVER* ex_over = reinterpret_cast<EXP_OVER*>(recv_over);
	int c_id = ex_over->c_id;
	if (err != 0)
	{
		disconnect(c_id);
		return;
	}
	
	//여기서 패킷 재조립을 해준다.
	char* packet_start = ex_over->_send_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static unsigned char packet_buffer[BUF_SIZE];
	

	while (0 != num_bytes) {
		if (0 == in_packet_size) in_packet_size = packet_start[0];
		if (num_bytes + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, packet_start, in_packet_size - saved_packet_size);

			clients[c_id].processpacket(packet_buffer);
			packet_start += in_packet_size - saved_packet_size;
			num_bytes -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, packet_start, num_bytes);
			saved_packet_size += num_bytes;
			num_bytes = 0;
		}
	}
	clients[c_id].do_recv();
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	EXP_OVER* ex_over = reinterpret_cast<EXP_OVER*>(send_over);
	int c_id = ex_over->c_id;
	if (err != 0)
	{
		clients[c_id]._state = ST_FREE;
		disconnect(c_id);
		return;
	}

	delete ex_over;
}


void disconnect(int s_id)
{
	{
		std::unique_lock<std::shared_mutex> lock(cl_lock);
		if (clients.count(s_id) != 0)
		{
			closesocket(clients[s_id]._socket);
			clients.erase(s_id);
		}
		lock.unlock();
	}
	for (auto& pl : clients) {
		if (pl.second._state != ST_INGAME) continue;
			pl.second.send_remove_player_packet(s_id);
	}
	return;
}

int main()
{

	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSA_bind : ", errorcode);
	}
	ret = listen(s_socket, SOMAXCONN);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSA_listen : ", errorcode);
	}
	INT addr_size = sizeof(server_addr);




	for (int i = 1; ; ++i) {
		SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
		{
			std::unique_lock<std::shared_mutex> lock(cl_lock);
			clients.try_emplace(i, i, c_socket);
		}

		int tcp_option = 1;
		setsockopt(c_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));
		clients[i].do_recv();
		//clients[i].do_recv();
	}
	WSACleanup();
}






void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	cout << msg;
	wcout << L"에러 " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}