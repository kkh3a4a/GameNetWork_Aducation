
char SERVER_ADDR[20] = "127.0.0.1";
const short SERVER_PORT = 9000;
const int BUFSIZE = 256;


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

