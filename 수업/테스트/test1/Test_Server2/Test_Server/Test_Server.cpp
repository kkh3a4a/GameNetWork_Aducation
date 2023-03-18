#include <iostream>
#include <WS2tcpip.h>
#include <unordered_map>
using namespace std;
#pragma comment (lib, "WS2_32.LIB")
const short SERVER_PORT = 9000;
const int BUFSIZE = 256;

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag);
void error_display(const char* msg, int err_no);


class EXP_OVER {
public:
	WSAOVERLAPPED _wsa_over;
	int _s_id;
	WSABUF _wsabuf;
	char _send_msg[BUFSIZE];

public:
	EXP_OVER(char s_id, char num_bytes, char* mess) : _s_id(s_id)
	{
		ZeroMemory(&_wsa_over, sizeof(WSAOVERLAPPED));
		_wsabuf.buf = _send_msg;
		_wsabuf.len = num_bytes + 2;
		_s_id = s_id;


		memcpy(_send_msg + 2, mess, num_bytes);
		_send_msg[0] = num_bytes + 2;
		_send_msg[1] = s_id;	// 256넘는 byte 보낼수 없음( 사이즈 검사 필요 )
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
	char _recv_buf[BUFSIZE];
	SESSION() {
		cout << "Unexpected Constructor Call Error!\n";
		exit(-1);
	}
	SESSION(int id, SOCKET s) : _id(id), _socket(s) {
		_recv_wsabuf.buf = _recv_buf; _recv_wsabuf.len = BUFSIZE;
		_send_wsabuf.buf = _recv_buf; _send_wsabuf.len = 0;
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
			if (errorcode != WSA_IO_PENDING)
				error_display("WSARecv : ", errorcode);
		}
	}
	void do_send(int sender_id, int num_bytes, char * mess) 
	{
		EXP_OVER* ex_over = new EXP_OVER(sender_id, num_bytes, mess);
		int ret = WSASend(_socket, &ex_over->_wsabuf, 1, 0, 0, &ex_over->_wsa_over, send_callback);

		if (ret != 0)
		{
			int errorcode = WSAGetLastError();
			if (errorcode != WSA_IO_PENDING)
				error_display("WSASend : ", errorcode);
		}
	}
};
unordered_map <int, SESSION> clients;
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED send_over, DWORD recv_flag)
{
	EXP_OVER* ex_over = reinterpret_cast<EXP_OVER*>(send_over);
	delete send_over;
}
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED recv_over, DWORD recv_flag)
{
	int s_id = reinterpret_cast<int>(recv_over->hEvent);
	cout << s_id <<" : Client Sent [" << num_bytes << "bytes] : " << clients[s_id]._recv_buf << endl;

	for (auto& cl : clients)
	{
		cl.second.do_send(s_id, num_bytes, clients[s_id]._recv_buf);
	}

	clients[s_id].do_recv();
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
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	ret = bind(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSABind : ", errorcode);
	}
	ret = listen(s_socket, SOMAXCONN);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAListen : ", errorcode);
	}
	INT addr_size = sizeof(server_addr);
	for (int i = 1; ; ++i) {
		SOCKET c_socket = WSAAccept(s_socket, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, 0, 0);
		int tcp_option = 1;
		setsockopt(c_socket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&tcp_option), sizeof(tcp_option));

		clients.try_emplace(i, i, c_socket);
		clients[i].do_recv();
	}
	clients.clear();
	closesocket(s_socket);
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

































//#include <iostream>
//#include <WS2tcpip.h>
//#include<unordered_map>
//#pragma comment(lib, "WS2_32.lib")
//using namespace std;
//constexpr int PORT_NUM = 9000;
//constexpr int BUF_SIZE = 200;
//SOCKET client;
////WSAOVERLAPPED c_over;
////WSABUF c_wsabuf[1];
////CHAR c_mess[BUF_SIZE];
//class SESSION {
//private:
//	int _id;
//	WSABUF _recv_wsabuf;
//	WSABUF _send_wsabuf;
//	WSAOVERLAPPED _recv_over;
//	SOCKET _socket;
//public:
//	char _recv_buf[BUF_SIZE];
//	SESSION() {
//		cout << "Unexpected Constructor Call Error!\n";
//		exit(-1);
//	}
//	SESSION(int id, SOCKET s) : _id(id), _socket(s) {
//		_recv_wsabuf.buf = _recv_buf; _recv_wsabuf.len = BUFSIZE;
//		_send_wsabuf.buf = _recv_buf; _send_wsabuf.len = 0;
//	}
//	~SESSION() {
//		closesocket(_socket);
//	}
//	void do_recv() {
//		DWORD recv_flag = 0;
//		ZeroMemory(&_recv_over, sizeof(_recv_over));
//		_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
//		WSARecv(_socket, &_recv_wsabuf, 1, 0, &recv_flag, &_recv_over, recv_callback);
//	}
//	void do_send(int num_bytes) {
//		ZeroMemory(&_recv_over, sizeof(_recv_over));
//		_recv_over.hEvent = reinterpret_cast<HANDLE>(_id);
//		_send_wsabuf.len = num_bytes;
//		WSASend(_socket, &_send_wsabuf, 1, 0, 0, &_recv_over, send_callback);
//	}
//};
//
//unordered_map<SOCKET, SESSION> Client_list;
//
//void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags, SOCKET& c_client);
//void do_recv(SOCKET& c_client);
//void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags, SOCKET& c_client);
//void error_display(const char* msg, int err_no);
//
//
//int main()
//{
//	std::wcout.imbue(std::locale("korean"));
//
//
//	WSADATA WSAData;
//	int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
//	if (ret != 0)
//	{
//		int errorcode = WSAGetLastError();
//		error_display("WSAStartup : ", errorcode);
//	}
//	SOCKET server = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
//	SOCKADDR_IN server_addr;
//	memset(&server_addr, 0, sizeof(server_addr));
//	server_addr.sin_family = AF_INET;
//	server_addr.sin_port = htons(PORT_NUM);
//	server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
//	ret = bind(server, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
//	if (ret != 0)
//	{
//		int errorcode = WSAGetLastError();
//		error_display("WSAStartup : ", errorcode);
//	}
//	ret = listen(server, SOMAXCONN);
//	if (ret != 0)
//	{
//		int errorcode = WSAGetLastError();
//		error_display("WSAStartup : ", errorcode);
//	}
//	SOCKADDR_IN cl_addr;
//	int addr_size = sizeof(cl_addr);
//	while (true)
//	{
//		client = WSAAccept(server, reinterpret_cast<sockaddr*>(&cl_addr), &addr_size, NULL, NULL);
//		SESSION session;
//		ZeroMemory(&session, sizeof(SESSION));
//		Client_list.insert({ client, session});
//		do_recv(client);
//		SleepEx(100, true);
//	}
//	closesocket(server);
//	WSACleanup();
//}
//
//void do_recv(SOCKET& c_client)
//{
//	Client_list[c_client].c_wsabuf[0].buf = Client_list[c_client].c_mess;
//	Client_list[c_client].c_wsabuf[0].len = BUF_SIZE;
//	DWORD recv_flag = 0;
//	memset(&Client_list[c_client].c_over, 0, sizeof(Client_list[c_client].c_over));
//	int ret = WSARecv(client, Client_list[c_client].c_wsabuf, 1, 0, &recv_flag, &Client_list[c_client].c_over, recv_callback);
//	if (ret != 0)
//	{
//		int errorcode = WSAGetLastError();
//		if(errorcode != WSA_IO_PENDING)
//			error_display("WSAStartup : ", errorcode);
//	}
//	
//}
//void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
//{
//	do_recv();
//}
//void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags)
//{
//	if (0 == num_bytes) return;
//	cout << "Client sent: " << c_mess << endl;
//	c_wsabuf[0].len = num_bytes;
//	memset(&c_over, 0, sizeof(c_over));
//	int ret = WSASend(client, c_wsabuf, 1, 0, 0, &c_over, send_callback);
//	if (ret != 0)
//	{
//		int errorcode = WSAGetLastError();
//		if (errorcode != WSA_IO_PENDING)
//			error_display("WSAStartup : ", errorcode);
//	}
//	
//}
//
//void error_display(const char* msg, int err_no)
//{
//	WCHAR* lpMsgBuf;
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER |
//		FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL, err_no,
//		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//		(LPTSTR)&lpMsgBuf, 0, NULL);
//	cout << msg;
//	wcout << L"에러 " << lpMsgBuf << endl;
//	while (true);
//	LocalFree(lpMsgBuf);
//}