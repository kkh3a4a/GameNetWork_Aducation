#include <iostream>
#include <WS2tcpip.h>
using namespace std;
#pragma comment(lib, "WS2_32.LIB")
constexpr short SERVER_PORT = 9000;
constexpr int BUF_SIZE = 200;
WSAOVERLAPPED s_over;
SOCKET s_socket;
WSABUF s_wsabuf[1];
char s_buf[BUF_SIZE];

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
void do_send();
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
	s_socket = WSASocket(AF_INET, SOCK_STREAM, 0, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN svr_addr;
	memset(&svr_addr, 0, sizeof(svr_addr));
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, "127.0.0.1", &svr_addr.sin_addr);
	ret = WSAConnect(s_socket, reinterpret_cast<sockaddr*>(&svr_addr), sizeof(svr_addr), 0, 0, 0, 0);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAConnect : ", errorcode);
	}
	do_send();
	while (true) SleepEx(100, true);
	closesocket(s_socket);
	WSACleanup();
}


void do_send()
{
	cout << "Enter Messsage: ";
	cin.getline(s_buf, BUF_SIZE - 1);
	s_wsabuf[0].buf = s_buf;
	s_wsabuf[0].len = static_cast<int>(strlen(s_buf)) + 1;
	memset(&s_over, 0, sizeof(s_over));
	int ret = WSASend(s_socket, s_wsabuf, 1, 0, 0, &s_over, send_callback);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		if (errorcode != WSA_IO_PENDING)
			error_display("WSAStartup : ", errorcode);
	}

}

void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	cout << "Server Sent: " << s_buf << endl;
	do_send();
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
{
	s_wsabuf[0].len = BUF_SIZE;
	DWORD r_flag = 0;
	memset(over, 0, sizeof(*over));
	int ret = WSARecv(s_socket, s_wsabuf, 1, 0, &r_flag, over, recv_callback);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		if (errorcode != WSA_IO_PENDING)
			error_display("WSAStartup : ", errorcode);
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
	wcout << L"���� " << lpMsgBuf << endl;
	while (true);
	LocalFree(lpMsgBuf);
}