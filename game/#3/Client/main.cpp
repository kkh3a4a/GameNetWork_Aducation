/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings

#include "Extra_Libs\SDL2-2.24.0\include\SDL.h"
#include "Extra_Libs\SDL2_image-2.6.2\include\SDL_image.h"
#include "Extra_Libs\SDL2_mixer-2.6.2\include\SDL_mixer.h"
#include "Extra_Libs\SDL2_ttf-2.0.15\include\SDL_ttf.h"

#include<iostream>
#include<vector>
#include<string>
#include<unordered_map>
#include <WS2tcpip.h>
#include"global.h"

#pragma comment (lib, "WS2_32.LIB")

using namespace std;
//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//ip check
bool IsValidIPAddress(const string& str);

//Loads individual image as texture
SDL_Texture* loadTexture(string path);
SDL_Texture* playerTex;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;
SDL_Event event;

unordered_map<int, SDL_Rect> MyPlayer;
unordered_map<int, Player_Info> players_list;
SDL_Point center;

bool quit = false;
bool isKeyPress = true;

void error_display(const char* msg, int err_no);
void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);
void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED over, DWORD flags);


bool init();
bool loadMedia();
void close();



SEND_BUF* _send_buf = new SEND_BUF;
RECV_BUF* _recv_buf = new RECV_BUF;


SDL_Texture* loadTexture(string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}
class Games {
private:
	

	int success = 0;
	

public:
	WSAOVERLAPPED _s_over;
	WSAOVERLAPPED _r_over;

	WSABUF _send_wsabuf[1]{};
	WSABUF _recv_wsabuf[1]{};

	Games()
	{
		ZeroMemory(&_s_over, sizeof(WSAOVERLAPPED));
		ZeroMemory(&_r_over, sizeof(WSAOVERLAPPED));
		ZeroMemory(&_send_wsabuf, sizeof(WSABUF));
		
		_send_wsabuf[0].buf = reinterpret_cast<CHAR*>(_send_buf);
		_send_wsabuf[0].len = sizeof(SEND_BUF);

		_recv_wsabuf[0].buf = reinterpret_cast<CHAR*>(_recv_buf);
		_recv_wsabuf[0].len = sizeof(RECV_BUF);
	}

	void KeyEvent()
	{
		SDL_PollEvent(&event);
		//Fix keyboard status when key pressed
		if (event.type == SDL_QUIT)
		{
			quit = true;
		}
		if (isKeyPress)
		{
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
					_send_buf->key_input.w = true;
					isKeyPress = false;
				}
				if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
					_send_buf->key_input.a = true;
					isKeyPress = false;
				}
				if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
					_send_buf->key_input.s = true;
					isKeyPress = false;
				}
				if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
					_send_buf->key_input.d = true;
					isKeyPress = false;
				}
				do_send();
			}
		}
		else
		{
			_send_buf->key_input.w = false;
			_send_buf->key_input.a = false;
			_send_buf->key_input.s = false;
			_send_buf->key_input.d = false;
		}

		if (event.type == SDL_KEYUP) {
			if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
				_send_buf->key_input.w = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
				_send_buf->key_input.a = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
				_send_buf->key_input.s = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
				_send_buf->key_input.d = false;
				isKeyPress = true;
			}
		}

		
	}

	void UpdateRender()
	{

		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//Draw horizontal line

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		for (int i = 0; i <= 8; ++i)
			SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH * i / 8, 0, SCREEN_WIDTH * i / 8, SCREEN_HEIGHT);
		for (int i = 0; i <= 8; ++i)
			SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT * i / 8, SCREEN_WIDTH, SCREEN_HEIGHT * i / 8);
		for (auto& a : MyPlayer)
		{
			SDL_RenderCopyEx(gRenderer, playerTex, NULL, &a.second, 0, &center, SDL_FLIP_NONE);
		}


		//Update screen
		SDL_RenderPresent(gRenderer);
	}


	void Ip_Input()
	{
		while(true)
		{
			string ip = "";
			cout << "IP : ";
			cin >> ip;
			if (IsValidIPAddress(ip))
			{
				strcpy(SERVER_ADDR, ip.c_str());
				break;
			}
			else
				cout << "error IP" << endl;

		}
		return;
	}


	//체스판 생성 , player 이미지 불러오고 생성

	void Gameinit()
	{
		//Main loop flag


			//Event handler
		SDL_Surface* tmpSurface;

		tmpSurface = IMG_Load("player.bmp");
		if (!tmpSurface) {
			cout << "Image not loaded" << endl;
		}
		playerTex = SDL_CreateTextureFromSurface(gRenderer, tmpSurface);
		SDL_FreeSurface(tmpSurface);
		int player_size = 40;
		SDL_Rect* temped = new SDL_Rect;
		MyPlayer.insert({ 1,*temped });
		MyPlayer[1].w = player_size;
		MyPlayer[1].h = player_size;
		MyPlayer[1].x = player_size / 2;
		MyPlayer[1].y = player_size / 2;

		center.x = player_size / 2;
		center.y = player_size / 2;
		SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(gRenderer);

		//Draw blue horizontal line

		SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
		for (int i = 0; i <= 8; ++i)
			SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH * i / 8, 0, SCREEN_WIDTH * i / 8, SCREEN_HEIGHT);
		for (int i = 0; i <= 8; ++i)
			SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT * i / 8, SCREEN_WIDTH, SCREEN_HEIGHT * i / 8);
	
		SDL_RenderCopyEx(gRenderer, playerTex, NULL, &MyPlayer[1], 0, &center, SDL_FLIP_NONE);
		SDL_RenderPresent(gRenderer);
	}
	void do_send() 
	{
		memset(&_s_over, 0, sizeof(_s_over));

		int ret = WSASend(s_socket, _send_wsabuf, 1, 0, 0, &_s_over, send_callback);
		if (ret != 0)
		{
			int errorcode = WSAGetLastError();
			if (errorcode != WSA_IO_PENDING)
				error_display("WSASend : ", errorcode);
		}
	}
	void do_recv()
	{
		DWORD r_flag = 0;
		memset(&_r_over, 0, sizeof(_r_over));
		int ret = WSARecv(s_socket, _recv_wsabuf, 1, 0, &r_flag, &_r_over, recv_callback);
		if (ret != 0)
		{
			int errorcode = WSAGetLastError();
			if (errorcode != WSA_IO_PENDING)
				error_display("WSARecv : ", errorcode);
		}
	}

};
Games ingame;


void CALLBACK recv_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED r_over, DWORD flags)
{
	if (_recv_buf->player_state == 0)
	{
		cout << "create Avatar" << endl;
		Player_Info player_info;
		SDL_Rect sdl_rect{};
		players_list[_recv_buf->id] = player_info;
		MyPlayer[_recv_buf->id] = sdl_rect;
		MyPlayer[_recv_buf->id].w = 40;
		MyPlayer[_recv_buf->id].h = 40;
		MyPlayer[_recv_buf->id].x = 40 / 2;
		MyPlayer[_recv_buf->id].y = 40 / 2;
		players_list[_recv_buf->id].MyPlayerLocation->x = _recv_buf->player_location.x;
		players_list[_recv_buf->id].MyPlayerLocation->y = _recv_buf->player_location.y;
		MyPlayer[_recv_buf->id].x = players_list[_recv_buf->id].MyPlayerLocation->x * 80 + 20;
		MyPlayer[_recv_buf->id].y = players_list[_recv_buf->id].MyPlayerLocation->y * 80 + 20;
		memset(r_over, 0, sizeof(*r_over));

	}
	if(_recv_buf->player_state == 1)
	{
		players_list[_recv_buf->id].MyPlayerLocation->x = _recv_buf->player_location.x;
		players_list[_recv_buf->id].MyPlayerLocation->y = _recv_buf->player_location.y;
		MyPlayer[_recv_buf->id].x = players_list[_recv_buf->id].MyPlayerLocation->x * 80 + 20;
		MyPlayer[_recv_buf->id].y = players_list[_recv_buf->id].MyPlayerLocation->y * 80 + 20;
		memset(r_over, 0, sizeof(*r_over));
	}
	if (_recv_buf->player_state == -1)
	{
		cout << "Player [ " << (int)_recv_buf->id << " ] disconnect" << endl;
		players_list.erase(_recv_buf->id);
	}

	//cout << (int)_recv_buf->id << " : " << players_list[_recv_buf->id].MyPlayerLocation->x << ", " << players_list[_recv_buf->id].MyPlayerLocation->y << endl;
	ingame.do_recv();
}

void CALLBACK send_callback(DWORD err, DWORD num_bytes, LPWSAOVERLAPPED s_over, DWORD flags)
{
	memset(s_over, 0, sizeof(*s_over));
}















int SDL_main(int argc, char* args[])
{

	wcout.imbue(locale("korean"));
	//cout << SERVER_ADDR << endl;
	//ingame.Ip_Input();
	cout << SERVER_ADDR << endl;
	//Start up SDL and create window
	
	//NetWork
	WSADATA WSAData;
	int ret = WSAStartup(MAKEWORD(2, 2), &WSAData);
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("WSAStartup : ", errorcode);
	}
	s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, WSA_FLAG_OVERLAPPED);
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
	ret = connect(s_socket, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	if (ret != 0)
	{
		int errorcode = WSAGetLastError();
		error_display("Connect : ", errorcode);
	}
	cout << "connecting" << endl;
	Player_Info player_info;

	players_list.insert({ 0, player_info});
	

	if (!init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}
	//Load media
	if (!loadMedia())
	{
		printf("Failed to load media!\n");
		return 0;
	}
	ingame.Gameinit();
	ingame.do_recv();
	DWORD recv_byte;
	DWORD recv_flag = 0;
	while (!quit)
	{
		DWORD sent_byte;
		ingame.KeyEvent();
		ingame.UpdateRender();
		SleepEx(1,true);
	}
	close();

	return 0;
}
































bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}


bool IsValidIPAddress(const string& str) {
	// 문자열을 "."으로 구분하여 숫자로 분리
	vector<int> parts;
	string::size_type start = 0, end = 0;
	while ((end = str.find(".", start)) != string::npos) {
		string part_str = str.substr(start, end - start);
		int part = stoi(part_str);
		if (part < 0 || part > 255) {
			return false;  // 유효한 범위를 벗어난 숫자
		}
		parts.push_back(part);
		start = end + 1;
	}
	string part_str = str.substr(start);
	int part = stoi(part_str);
	if (part < 0 || part > 255) {
		return false;  // 유효한 범위를 벗어난 숫자
	}
	parts.push_back(part);

	// 숫자가 4개인지 확인
	if (parts.size() != 4) {
		return false;
	}

	// 모든 숫자가 유효한 범위에 있다면 유효한 IP 주소로 판단
	return true;
}