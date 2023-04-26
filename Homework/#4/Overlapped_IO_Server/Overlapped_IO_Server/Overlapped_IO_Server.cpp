#include <iostream>
#include <array>
#include <WS2tcpip.h>
#include<unordered_map>
#include <unordered_set>
#include"../../protocol.h"

using namespace std;
#pragma comment (lib, "WS2_32.LIB")
constexpr int VIEW_RANGE = 5;
bool can_see(int p1, int p2);

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

	EXP_OVER(char* packet, int id)
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
	S_STATE _state;
	int c_id;
	SOCKET _socket;
	short	x, y;
	char	_name[NAME_SIZE];
	int		_prev_remain;
	int		_last_move_time;

	unordered_set <int> _view_list;

public:
	SESSION()
	{
		c_id = -1;
		_socket = NULL;
		x = y = 0;
		_name[0] = 0;
		_state = ST_FREE;
		_prev_remain = 0;
		_view_list.clear();
	}
	SESSION(int id, SOCKET c_socket)
	{
		c_id = id;
		_socket = c_socket;
		x = y = 0;
		_name[0] = 0;
		_state = ST_FREE;
		_prev_remain = 0;
		_view_list.clear();
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
		if (_view_list.count(c_id) != 0)
		{
			{
				if (_view_list.count(c_id) == 0) {
					return;
				}
				_view_list.erase(c_id);
			}
			SC_REMOVE_PLAYER_PACKET p;
			p.id = c_id;
			p.size = sizeof(p);
			p.type = SC_REMOVE_PLAYER;
			do_send(&p);
		}
	}
	void processpacket(unsigned char* buf);
};

array<SESSION, MAX_USER> clients;


void SESSION::processpacket(unsigned char* buf)
{
	switch (buf[1])
	{
	case CS_LOGIN:
	{
		CS_LOGIN_PACKET* packet = reinterpret_cast<CS_LOGIN_PACKET*>(buf);
		{
			strcpy_s(clients[c_id]._name, packet->name);
			clients[c_id].send_login_info_packet();

			clients[c_id]._state = ST_INGAME;

			for (auto& pl : clients)
			{
				{
					if (ST_INGAME != pl._state)
						continue;
				}
				if (pl.c_id == c_id) continue;
				pl.send_add_player_packet(c_id);
				clients[c_id].send_add_player_packet(pl.c_id);

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
		unordered_set <int> old_vl;
		{
			old_vl = clients[c_id]._view_list;
		}
		unordered_set <int> new_vl;

		for (auto& pl : clients) {
			if (pl._state != ST_INGAME) continue;
			if (can_see(pl.c_id, c_id))
				new_vl.insert(pl.c_id);
		}
		for (auto& o : new_vl) {
			if (old_vl.count(o) == 0) {
				clients[o].send_add_player_packet(c_id);
				clients[c_id].send_add_player_packet(o);
			}
			else {
				clients[o].send_move_packet(c_id);
				clients[c_id].send_move_packet(o);
			}
		}
		clients[c_id].send_move_packet(c_id);
		for (auto op : old_vl)
		{
			if (clients[op]._state != ST_INGAME) continue;
			if (clients[op].c_id == c_id) continue;
			if (new_vl.count(op) == 0)
			{
				clients[c_id].send_remove_player_packet(op);
				clients[op].send_remove_player_packet(c_id);
			}
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
	if (_view_list.count(c_id) != 0) {
		send_move_packet(c_id);
		return;
	}
	_view_list.insert(c_id);

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
	if (_view_list.count(c_id) != 0) {
		SC_MOVE_PLAYER_PACKET move_packet;
		move_packet.type = SC_MOVE_PLAYER;
		move_packet.size = sizeof(SC_MOVE_PLAYER_PACKET);
		move_packet.x = clients[c_id].x;
		move_packet.y = clients[c_id].y;
		move_packet.id = c_id;
		move_packet.move_time = clients[c_id]._last_move_time;
		do_send(&move_packet);
	}
	else {
		send_add_player_packet(c_id);
	}
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
		disconnect(c_id);
		return;
	}

	delete ex_over;
}


void disconnect(int s_id)
{
	
	closesocket(clients[s_id]._socket);
	for (auto& pl : clients) {
		if (pl._state != ST_INGAME) continue;
		pl.send_remove_player_packet(s_id);
	}
	clients[s_id]._state = ST_FREE;
	return;
}
bool can_see(int p1, int p2)
{
	// return VIEW_RANGE <= SQRT((p1.x - p2.x) ^ 2 + (p1.y - p2.y) ^ 2);
	if (abs(clients[p1].x - clients[p2].x) > VIEW_RANGE) return false;
	if (abs(clients[p1].y - clients[p2].y) > VIEW_RANGE) return false;
	return true;
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

		clients[i] = { i, c_socket };


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