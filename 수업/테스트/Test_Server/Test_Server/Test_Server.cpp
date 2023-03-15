#include <iostream>
#include <WS2tcpip.h>
#pragma comment(lib, "WS2_32.lib")
using namespace std;
constexpr int PORT_NUM = 9000;
constexpr int BUF_SIZE = 200;
SOCKET client;
WSAOVERLAPPED c_over;
WSABUF c_wsabuf[1];
CHAR c_mess[BUF_SIZE];


void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
void do_recv();
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
void error_display(const char* msg, int err_no);


int main()
{
	std::wcout.imbue(std::locale("korean"));


	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	SOCKET server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT_NUM);
	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	ret = bind(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	ret = listen(server, SOMAXCONN);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	SOCKADDR_IN cl_addr;
	int addr_size = sizeof(cl_addr);
	while (true)
	{
		client = WSAAccept(server, reinterpret_cast<sockaddr*>(&cl_addr), &addr_size, NULL, NULL);
		do_recv();
		SleepEx(100, true);
	}
	closesocket(server);
	WSACleanup();
}

void do_recv()
{
	c_wsabuf[0].buf = c_mess;
	c_wsabuf[0].len = BUF_SIZE;
	DWORD recv_flag = 0;
	memset(&c_over, 0, sizeof(c_over));
	int ret = WSARecv(client, c_wsabuf, 1, 0, &recv_flag, &c_over, recv_callback);
	
}
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	do_recv();
}
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	if (0 == num_bytes) return;
	cout << "Client sent: " << c_mess << endl;
	c_wsabuf[0].len = num_bytes;
	memset(&c_over, 0, sizeof(c_over));
	int ret = WSASend(client, c_wsabuf, 1, 0, 0, &c_over, send_callback);
	
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