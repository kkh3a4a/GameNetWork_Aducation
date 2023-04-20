#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include <WS2tcpip.h>

char SERVER_ADDR[20] = "127.0.0.1";
const short SERVER_PORT = 9000;
const int BUFSIZE = 256;
SOCKET s_socket;

struct KeyInput {
public:
	bool w = false;
	bool a = false;
	bool s = false;
	bool d = false;
};

struct Player_Location
{
public:
	int x = 0, y = 0;
	int w = 40, h = 40;
};


struct Player_Info {
	Player_Location* MyPlayerLocation = new Player_Location;
};

struct SEND_BUF {
public:
	KeyInput key_input;
};

struct RECV_BUF {
public:
	char id{};
	char num_byte{};
	char player_state;	//0 : 霉 立加, 1 : 立加 惑怕, -1 : 立加秦力
	Player_Location player_location;
};



void error_display(const char* msg, int err_no)
{
	WCHAR* lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	std::cout << msg;
	std::wcout << L"俊矾 " << lpMsgBuf << std::endl;
	while (true);
	LocalFree(lpMsgBuf);
}
