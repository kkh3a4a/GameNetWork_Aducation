#include <iostream>
#include <WS2tcpip.h>
#include"global.h"
#include<unordered_map>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const short SERVER_PORT = 9000;
const int BUFSIZE = 256;

void error_display(const char* msg, int err_no);

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag);
void CALLBACK connect_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD send_flag);
void disconnect(int id);
class EXP_OVER {
public:
	WSAOVERLAPPED _wsa_over;
	WSABUF _wsabuf;

public:
	EXP_OVER(char s_id, char num_bytes,char player_state, SEND_BUF* send_buf)
	{
		ZeroMemory(&_wsa_over, sizeof(WSAOVERLAPPED));
		_wsa_over.hEvent = reinterpret_cast<HANDLE>(s_id);
		_wsabuf.buf = reinterpret_cast<CHAR*>(send_buf);
		_wsabuf.len = sizeof(SEND_BUF);
		send_buf->id = s_id;
		send_buf->num_byte = num_bytes;
		send_buf->player_state = player_state;
		// 256넘는 byte 보낼수 없음( 사이즈 검사 필요 )
		// 최대 접속 100명 안됨, 1byte라서 
	}

	~EXP_OVER()
	{

	}
};

class SESSION {
private:
	int _id;
	WSABUF _recv_wsabuf{};
	WSABUF _send_wsabuf{};
	WSAOVERLAPPED _recv_over;
	SOCKET _socket;
	

public:
	int player_state;
	RECV_BUF* _recv_buf{};
	SEND_BUF* _send_buf{};

	SESSION() {
		cout << "Unexpected Constructor Call Error!\n";
		exit(-1);
	}
	SESSION(int id, SOCKET s) : _id(id), _socket(s) {
		player_state = 0;
		_recv_buf = new RECV_BUF;
		_send_buf = new SEND_BUF;
		_recv_buf->key_input = { 0 };
		_send_buf->player_location = { 0 };
		_recv_wsabuf.buf = reinterpret_cast<CHAR*>(_recv_buf); _recv_wsabuf.len = sizeof(RECV_BUF);
		_send_wsabuf.buf = reinterpret_cast<CHAR*>(_send_buf); _send_wsabuf.len = sizeof(SEND_BUF);
		
	}
	~SESSION() {
		cout << "사라짐" << endl;
		closesocket(_socket);
	}
	void do_recv() {
		DWORD recv_flag = 0;
		ZeroMemory(&_recv_over, sizeof(_recv_over));
		_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
		int ret = WSARecv(_socket, &_recv_wsabuf, 1, 0, &recv_flag, &_recv_over, recv_callback);
		if (ret != 0)
		{
			int errorcode = WSAGetLastError();

			if (errorcode == 10054)
			{
				player_state = -1;
				return;
			}
			else if (errorcode != WSA_IO_PENDING)
				error_display("WSARecv : ", errorcode);
		}
	}
	void do_send(int sender_id, int num_bytes, SEND_BUF* send_buf, int player_state, unordered_map<int, SESSION>& players_list)
	{
		EXP_OVER* ex_over = new EXP_OVER(sender_id, num_bytes, player_state, send_buf);
		int ret = WSASend(_socket, &ex_over->_wsabuf, 1, 0, 0, &ex_over->_wsa_over, send_callback);
		if (ret != 0)
		{
			int errorcode = WSAGetLastError();
			if (errorcode == 10054)
			{
				player_state = -1;
				
				for (auto& pl : players_list)
				{
					if (pl.first != _id)
					{
						pl.second.do_send(_id, num_bytes, send_buf, player_state, players_list);
						
					}
				}
			}
			else if (errorcode != WSA_IO_PENDING)
				error_display("WSASend : ", errorcode);
		}
		
	}
	void first_send(unordered_map<int, SESSION>& players_list)
	{
		player_state = 0;
		for (auto& pl : players_list)
		{
			EXP_OVER* ex_over = new EXP_OVER(pl.first, 0, player_state, pl.second._send_buf);
			int ret = WSASend(_socket, &ex_over->_wsabuf, 1, 0, 0, &ex_over->_wsa_over, connect_callback);
			if (ret != 0)
			{
				int errorcode = WSAGetLastError();
				error_display("WSASend : ", errorcode);
			}
			EXP_OVER* ex_over2 = new EXP_OVER(_id, 0, player_state, _send_buf);
			ret = WSASend(pl.second._socket, &ex_over2->_wsabuf, 1, 0, 0, &ex_over2->_wsa_over, connect_callback);
			if (ret != 0)
			{
				int errorcode = WSAGetLastError();
				error_display("WSASend : ", errorcode);
			}
		}
	}


	void UpdatePlayer()
	{

	}
};

unordered_map<int, SESSION> players_list;
void CALLBACK connect_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD send_flag)
{
	int s_id = reinterpret_cast<int>(send_over->hEvent);
	EXP_OVER* ex_over = reinterpret_cast<EXP_OVER*>(send_over);
	delete send_over;

	players_list[s_id].do_recv();
}

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	int s_id = reinterpret_cast<int>(recv_over->hEvent);
	if (err != 0)
	{
		return;
	}
	if (players_list[s_id].player_state == -1)
	{
		players_list.erase(s_id);
		return;
	}
	

	if (players_list[s_id]._recv_buf->key_input.w && players_list[s_id]._send_buf->player_location.y > 0)
	{
		players_list[s_id]._send_buf->player_location.y -= 1;
	}
	if (players_list[s_id]._recv_buf->key_input.a && players_list[s_id]._send_buf->player_location.x > 0)
		players_list[s_id]._send_buf->player_location.x -= 1;
	if (players_list[s_id]._recv_buf->key_input.s && players_list[s_id]._send_buf->player_location.y < 7)
		players_list[s_id]._send_buf->player_location.y += 1;
	if (players_list[s_id]._recv_buf->key_input.d && players_list[s_id]._send_buf->player_location.x < 7)
		players_list[s_id]._send_buf->player_location.x += 1;

	for (auto& pl : players_list)
	{
		pl.second.do_send(s_id, num_bytes, players_list[s_id]._send_buf, 1, players_list);
	}
	
	players_list[s_id].do_recv();


}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	EXP_OVER* ex_over = reinterpret_cast<EXP_OVER*>(send_over);
	delete send_over;
}


void disconnect(int s_id)
{
	players_list.erase(s_id);
	return;
}

int main()
{
	KeyInput* keyinput = new KeyInput;

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
	server_addr.sin_port = htons(SERVER_PORT);
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


	
	Player_Location player;
	WSABUF mybuf_s[1];
	//mybuf_s[0].buf = reinterpret_cast<CHAR*>(players_list[c_socket]);
	mybuf_s[0].len = sizeof(Player_Location);
	
	for (int i = 1; ; ++i) {
		SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
		players_list.try_emplace(i, i, c_socket);
		int tcp_option = 1;
		setsockopt(c_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));
		players_list[i].first_send(players_list);
		//players_list[i].do_recv();
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