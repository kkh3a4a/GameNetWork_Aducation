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


struct RECV_BUF {
public:
	KeyInput key_input;
};

struct SEND_BUF {
public:
	char id{};
	char num_byte{};
	char player_state{};	//0 : 霉 立加, 1 : 立加 惑怕, -1 : 立加秦力
	Player_Location player_location{};
};


void error_display(const char* msg, int err_no);