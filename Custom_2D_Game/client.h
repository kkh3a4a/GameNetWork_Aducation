#include <winsock2.h> // ����2 ���� ���
#include <ws2tcpip.h> // ����2 Ȯ�� ���
#include <iostream>
#include<map>
#include "game.h"

WSADATA wsa;
Game game;
//SOCKET sock;
int retval;
#define SERVERPORT 9000
char* SERVERIP = (char*)"127.0.0.1";
#define NAMESIZE 20

// ���� �Լ� ���� ��� �� ����
void err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	MessageBoxA(NULL, (const char*)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}
// ���� �Լ� ���� ���
void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[%s] %s\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}
// ���� �Լ� ���� ���
void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	printf("[����] %s\n", (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


void send_name(SOCKET sock, Game* game)	// �÷��̾� �̸� ������
{
	char name_buf[NAMESIZE]{};
	strncpy(name_buf, game->Name, strlen(game->Name));
	cout << name_buf << endl;
	retval = send(sock, name_buf, NAMESIZE, 0);
	if (retval == SOCKET_ERROR) {
		err_display("send_name()");
	}
}
//
//void gun_change(SOCKET sock, int gun)	// �� ���� ������
//{
//	char cgun = gun + '0';
//	cout << "�� Ÿ���� " << cgun << endl;
//	retval = send(sock, (char*)&cgun, sizeof(char), 0);
//	if (retval == SOCKET_ERROR) {
//		err_display("gun_change()");
//	}
//	gun_type = gun;
//}

void recv_created_object(SOCKET sock)
{
	retval = recv(sock, (char*)&game.created_objects, sizeof(game.created_objects), MSG_WAITALL);
	cout << "�޴� byte : " << sizeof(game.created_objects) << endl;
	if (retval == SOCKET_ERROR) {
		err_display("recv()");
		//����ó��
	}
	for (int i = 0; i < MAXITEM; ++i) {
		cout << "���� created_objects[" << i << "] : " << game.created_objects[i].object_position.x << " / " << game.created_objects[i].object_position.y << endl;
	}

	cout << "created object ���� �Ϸ�" << endl;
}

// Ŭ���̾�Ʈ �ΰ��ӿ����� �Է�
int send_event(SOCKET sock)
{
	// key input for player move
	retval = send(sock, (char*)&game.input, sizeof(CI), 0);
	
	return retval;
}

// �������� �ٸ� �÷��̾� �̺�Ʈ recv
int recv_event(SOCKET sock)
{
	//�÷��̾� ���� �ޱ�
	retval = recv(sock, (char*)&game.player_list, sizeof(PS) * 3, MSG_WAITALL);
	
	return retval;
}

SOCKET socket_init(SOCKET sock)
{
	game.waiting_match = false;
	game.connect_server = false;
	game.server_connected = false;
	closesocket(sock);
	// ���� �ʱ�ȭ
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
		err_quit("WSAStartup()");
	}
	// ���� ����
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");
	return sock;
}
