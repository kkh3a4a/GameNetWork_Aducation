#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <windows.h>
#include <iostream>
#include <unordered_map>
#include <chrono>
using namespace std;
using namespace chrono;

//#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "libprotobuf.lib")

#include "../../SimpleIOCPServer/SimpleIOCPServer/PROTOCOL/protocol.pb.h"

sf::TcpSocket g_socket;

constexpr int MAX_ID_LEN = 50;
constexpr int MAX_STR_LEN = 255;

#define WORLD_WIDTH		400
#define WORLD_HEIGHT	400

#define NPC_ID_START	20000

#define SERVER_PORT		9000
#define NUM_NPC			100

#define MAX_BUFFER		1024

#define PKID_SC_LOGIN_OK 1
#define PKID_SC_POS	2
#define PKID_SC_PUT_OBJECT 3
#define PKID_SC_REMOVE_OBJECT 4
#define PKID_CS_LOGIN 5
#define PKID_CS_MOVE 6

constexpr unsigned char D_UP = 0;
constexpr unsigned char D_DOWN = 1;
constexpr unsigned char D_LEFT = 2;
constexpr unsigned char D_RIGHT = 3;

constexpr auto SCREEN_WIDTH = 19;
constexpr auto SCREEN_HEIGHT = 19;

constexpr auto TILE_WIDTH = 65;
constexpr auto WINDOW_WIDTH = TILE_WIDTH * SCREEN_WIDTH + 10;   // size of window
constexpr auto WINDOW_HEIGHT = TILE_WIDTH * SCREEN_WIDTH + 10;
constexpr auto BUF_SIZE = 200;
constexpr auto MAX_USER = 10;

int g_left_x;
int g_top_y;
int g_myid;

sf::RenderWindow* g_window;
sf::Font g_font;

class OBJECT {
private:
	bool m_showing;
	sf::Sprite m_sprite;
	char m_mess[MAX_STR_LEN];
	high_resolution_clock::time_point m_time_out;
	sf::Text m_text;

public:
	int m_x, m_y;
	OBJECT(sf::Texture& t, int x, int y, int x2, int y2) {
		m_showing = false;
		m_sprite.setTexture(t);
		m_sprite.setTextureRect(sf::IntRect(x, y, x2, y2));
		m_time_out = high_resolution_clock::now();
	}
	OBJECT() {
		m_showing = false;
		m_time_out = high_resolution_clock::now();
	}
	void show()
	{
		m_showing = true;
	}
	void hide()
	{
		m_showing = false;
	}

	void a_move(int x, int y) {
		m_sprite.setPosition((float)x, (float)y);
	}

	void a_draw() {
		g_window->draw(m_sprite);
	}

	void move(int x, int y) {
		m_x = x;
		m_y = y;
	}
	void draw() {
		if (false == m_showing) return;
		float rx = (m_x - g_left_x) * 65.0f + 8;
		float ry = (m_y - g_top_y) * 65.0f + 8;
		m_sprite.setPosition(rx, ry);
		g_window->draw(m_sprite);
		if (high_resolution_clock::now() < m_time_out) {
			m_text.setPosition(rx - 10, ry - 10);
			g_window->draw(m_text);
		}
	}
	void add_chat(char chat[]) {
		m_text.setFont(g_font);
		m_text.setString(chat);
		m_time_out = high_resolution_clock::now() + 1s;
	}
};

OBJECT avatar;
unordered_map <int, OBJECT> npcs;

OBJECT white_tile;
OBJECT black_tile;

sf::Texture* board;
sf::Texture* pieces;

void client_initialize()
{
	board = new sf::Texture;
	pieces = new sf::Texture;
	if (false == g_font.loadFromFile("cour.ttf")) {
		cout << "Font Loading Error!\n";
		while (true);
	}
	board->loadFromFile("chessmap.bmp");
	pieces->loadFromFile("chess2.png");
	white_tile = OBJECT{ *board, 5, 5, TILE_WIDTH, TILE_WIDTH };
	black_tile = OBJECT{ *board, 69, 5, TILE_WIDTH, TILE_WIDTH };
	avatar = OBJECT{ *pieces, 128, 0, 64, 64 };
	avatar.move(4, 4);
}

void client_finish()
{
	delete board;
	delete pieces;
}



void ProcessPacket(char* pb, int pb_type, int pb_size)
{
	switch (pb_type)
	{
	case PKID_SC_LOGIN_OK:
	{
		SC_LOGIN_OK p;
		p.ParseFromArray(pb, pb_size);
		g_myid = p.id();
		avatar.move(p.x(), p.y());
		g_left_x = p.x() - (SCREEN_WIDTH / 2);
		g_top_y = p.y() - (SCREEN_HEIGHT / 2);
		avatar.show();
	}

	case PKID_SC_PUT_OBJECT:
	{
		SC_PUT_OBJECT p;
		p.ParseFromArray(pb, pb_size);

		int id = p.id();

		if (id == g_myid) {
			avatar.move(p.x(), p.y());
			g_left_x = p.x() - (SCREEN_WIDTH / 2);
			g_top_y = p.y() - (SCREEN_HEIGHT / 2);
			avatar.show();
		}
		else {
			if (id < NPC_ID_START)
				npcs[id] = OBJECT{ *pieces, 64, 0, 64, 64 };
			else
				npcs[id] = OBJECT{ *pieces, 0, 0, 64, 64 };
			npcs[id].move(p.x(), p.y());
			npcs[id].show();
		}
		break;
	}
	case PKID_SC_POS:
	{
		SC_POS p;
		p.ParseFromArray(pb, pb_size);

		int other_id = p.id();
		if (other_id == g_myid) {
			avatar.move(p.x(), p.y());
			g_left_x = p.x() - (SCREEN_WIDTH / 2);
			g_top_y = p.y() - (SCREEN_HEIGHT / 2);
		}
		else {
			if (0 != npcs.count(other_id))
				npcs[other_id].move(p.x(), p.y());
		}
		break;
	}

	case PKID_SC_REMOVE_OBJECT:
	{
		SC_REMOVE_OBJECT p;
		p.ParseFromArray(pb, pb_size);
	
		int other_id = p.id();
		if (other_id == g_myid) {
			avatar.hide();
		}
		else {
			if (0 != npcs.count(other_id))
				npcs[other_id].hide();
		}
		break;
	}
	default:
		printf("Unknown PACKET type [%d]\n", pb_type);
	}
}

void process_data(char* net_buf, size_t io_byte)
{
	char* ptr = net_buf;
	static size_t in_packet_size = 0;
	static size_t saved_packet_size = 0;
	static char packet_buffer[BUF_SIZE];

	while (0 != io_byte) {
		if (0 == in_packet_size) in_packet_size = ptr[0];
		if (io_byte + saved_packet_size >= in_packet_size) {
			memcpy(packet_buffer + saved_packet_size, ptr, in_packet_size - saved_packet_size);
			ProcessPacket(&packet_buffer[2], packet_buffer[1], in_packet_size - 2);
			ptr += in_packet_size - saved_packet_size;
			io_byte -= in_packet_size - saved_packet_size;
			in_packet_size = 0;
			saved_packet_size = 0;
		}
		else {
			memcpy(packet_buffer + saved_packet_size, ptr, io_byte);
			saved_packet_size += io_byte;
			io_byte = 0;
		}
	}
}

void client_main()
{
	char net_buf[BUF_SIZE];
	size_t	received;

	auto recv_result = g_socket.receive(net_buf, BUF_SIZE, received);
	if (recv_result == sf::Socket::Error)
	{
		wcout << L"Recv ����!";
		while (true);
	}

	if (recv_result == sf::Socket::Disconnected)
	{
		wcout << L"���� ���� ����.";
		g_window->close();
	}

	if (recv_result != sf::Socket::NotReady)
		if (received > 0) process_data(net_buf, received);

	for (int i = 0; i < SCREEN_WIDTH; ++i)
		for (int j = 0; j < SCREEN_HEIGHT; ++j)
		{
			int tile_x = i + g_left_x;
			int tile_y = j + g_top_y;
			if ((tile_x < 0) || (tile_y < 0)) continue;
			if (((tile_x / 3 + tile_y / 3) % 2) == 0) {
				white_tile.a_move(TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
				white_tile.a_draw();
			}
			else
			{
				black_tile.a_move(TILE_WIDTH * i + 7, TILE_WIDTH * j + 7);
				black_tile.a_draw();
			}
		}
	avatar.draw();
	//	for (auto &pl : players) pl.draw();
	for (auto& npc : npcs) npc.second.draw();
	sf::Text text;
	text.setFont(g_font);
	char buf[100];
	sprintf_s(buf, "(%d, %d)", avatar.m_x, avatar.m_y);
	text.setString(buf);
	g_window->draw(text);

}


void send_packet(void* packet)
{
	char* p = reinterpret_cast<char*>(packet);
	size_t sent;
	g_socket.send(p, p[0], sent);
}

void send_move_packet(unsigned char dir)
{
	CS_MOVE packet;
	switch (dir) {
	case 0: packet.set_dir(Direction::UP); break;
	case 1: packet.set_dir(Direction::DOWN); break;
	case 2: packet.set_dir(Direction::LEFT); break;
	case 3: packet.set_dir(Direction::RIGHT); break;
	}
	packet.set_move_time(static_cast<unsigned>(duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count()));


	char buf[MAX_BUFFER];
	buf[0] = packet.ByteSizeLong() + 2;
	buf[1] = PKID_CS_MOVE;
	
	packet.SerializeToArray(&buf[2], MAX_BUFFER - 2);

	send_packet(&buf);
}


int main()
{
	wcout.imbue(locale("korean"));
	sf::Socket::Status status = g_socket.connect("127.0.0.1", SERVER_PORT);
	g_socket.setBlocking(false);

	if (status != sf::Socket::Done) {
		wcout << L"������ ������ �� �����ϴ�.\n";
		while (true);
	}

	client_initialize();

	char user_name[MAX_ID_LEN];
	int t_id = GetCurrentProcessId();
	sprintf_s(user_name, "P%d", t_id);

	CS_LOGIN packet;
	packet.set_user_name(user_name);
	char buf[MAX_BUFFER];
	buf[0] = packet.ByteSizeLong() + 2;
	buf[1] = PKID_CS_LOGIN;

	packet.SerializeToArray(&buf[2], MAX_BUFFER - 2);

	send_packet(&buf);

	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2), "2D CLIENT");
	g_window = &window;

	sf::View view = g_window->getView();
	view.zoom(2.0f);
	view.move(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 4);
	g_window->setView(view);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed) {
				int p_type = -1;
				switch (event.key.code) {
				case sf::Keyboard::Left:
					send_move_packet(D_LEFT);
					break;
				case sf::Keyboard::Right:
					send_move_packet(D_RIGHT);
					break;
				case sf::Keyboard::Up:
					send_move_packet(D_UP);
					break;
				case sf::Keyboard::Down:
					send_move_packet(D_DOWN);
					break;
				case sf::Keyboard::Escape:
					window.close();
					break;
				}
			}
		}

		window.clear();
		client_main();
		window.display();
	}
	client_finish();

	return 0;
}