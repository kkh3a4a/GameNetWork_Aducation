#include <iostream>
#include <WS2tcpip.h>
#include"global.h"
#include<map>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const short SERVER_PORT = 9000;
const int BUFSIZE = 256;

map<SOCKET, Player_Info> players_list;
void error_display(const char* msg, int err_no);
void UpdatePlayer(map<SOCKET, Player_Info>&, KeyInput*, SOCKET);

int main()
{
	KeyInput* keyinput = new KeyInput;

	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 0), &WSAData);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	ret = listen(s_socket, SOMAXCONN);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	INT addr_size = sizeof(server_addr);
	SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
	Player_Info player_info;
	players_list.insert({ c_socket, player_info });
	Player_Location player;
	WSABUF mybuf_s[1];
	mybuf_s[0].buf = reinterpret_cast<CHAR*>(players_list[c_socket].MyPlayerLocation);
	mybuf_s[0].len = sizeof(Player_Location);
	for (;;) {
		char recv_buf[BUFSIZE];
		WSABUF mybuf;
		mybuf.buf = recv_buf; 
		mybuf.len = BUFSIZE;
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(c_socket, &mybuf, 1, &recv_byte, &recv_flag, 0, 0);
		
		keyinput = reinterpret_cast<KeyInput*>(mybuf.buf);
		UpdatePlayer(players_list, keyinput, c_socket);

		DWORD sent_byte;
		if (keyinput->w || keyinput->a|| keyinput->s|| keyinput->d)
			cout << players_list[c_socket].MyPlayerLocation->x << "," << players_list[c_socket].MyPlayerLocation->y << endl;
		WSASend(c_socket, mybuf_s, 1, &sent_byte, 0, 0, 0);


	}
	WSACleanup();
}





void UpdatePlayer(map<SOCKET, Player_Info>& players_list, KeyInput* keyinput, SOCKET c_socket)
{
	if (keyinput->w)
	{
		if (players_list[c_socket].MyPlayerLocation->y > 0)
		{
			players_list[c_socket].MyPlayerLocation->y -= 1;
		}
		
	}
	if (keyinput->a)
	{
		if (players_list[c_socket].MyPlayerLocation->x > 0)
		{
			players_list[c_socket].MyPlayerLocation->x -= 1;
		}

	}
	if (keyinput->s)
	{
		if (players_list[c_socket].MyPlayerLocation->y < 7)
		{
			players_list[c_socket].MyPlayerLocation->y += 1;
		}

	}
	if (keyinput->d)
	{
		if (players_list[c_socket].MyPlayerLocation->x < 7)
		{
			players_list[c_socket].MyPlayerLocation->x += 1;
		}
	}
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
	wcout << L"¿¡·¯ " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}