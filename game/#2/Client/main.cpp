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
#include<map>
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

SDL_Rect MyPlayer;
map<int, Player_Info> players_list;
SDL_Point center;
int x = 1;
int y = 1;
bool quit = false;
bool isKeyPress = true;


void error_display(const char* msg, int err_no);
bool init();
bool loadMedia();
void close();

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

	KeyInput* keyinput = new KeyInput;


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
					keyinput->w = true;
					isKeyPress = false;
				}
				if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
					keyinput->a = true;
					isKeyPress = false;
				}
				if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
					keyinput->s = true;
					isKeyPress = false;
				}
				if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
					keyinput->d = true;
					isKeyPress = false;
				}
			}
		}
		else
		{
			keyinput->w = false;
			keyinput->a = false;
			keyinput->s = false;
			keyinput->d = false;
		}

		if (event.type == SDL_KEYUP) {
			if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_w) {
				keyinput->w = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_LEFT || event.key.keysym.sym == SDLK_a) {
				keyinput->a = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_DOWN || event.key.keysym.sym == SDLK_s) {
				keyinput->s = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_RIGHT || event.key.keysym.sym == SDLK_d) {
				keyinput->d = false;
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
		SDL_RenderCopyEx(gRenderer, playerTex, NULL, &MyPlayer, 0, &center, SDL_FLIP_NONE);


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

		MyPlayer.w = player_size;
		MyPlayer.h = player_size;
		MyPlayer.x = player_size / 2;
		MyPlayer.y = player_size / 2;

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
		SDL_RenderCopyEx(gRenderer, playerTex, NULL, &MyPlayer, 0, &center, SDL_FLIP_NONE);
		SDL_RenderPresent(gRenderer);
	}
};
Games ingame;


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
	SOCKET s_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, 0, 0, 0);
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
	int player_count = 0;
	players_list.insert({ player_count, player_info});
	

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
	WSABUF mybuf[1];
	mybuf[0].buf = reinterpret_cast<CHAR*>(ingame.keyinput);
	mybuf[0].len = sizeof(KeyInput);
	while (!quit)
	{
		DWORD sent_byte;
		ingame.KeyEvent();
		WSASend(s_socket, mybuf, 1, &sent_byte, 0, 0, 0);

		char recv_buf[BUFSIZE];
		WSABUF mybuf_r[1];
		mybuf_r[0].buf = recv_buf;
		mybuf_r[0].len = BUFSIZE;
		DWORD recv_byte;
		DWORD recv_flag = 0;
		WSARecv(s_socket, mybuf_r, 1, &recv_byte, &recv_flag, 0, 0);
		players_list[0].MyPlayerLocation = reinterpret_cast<Player_Location*>(recv_buf);

		MyPlayer.x = players_list[0].MyPlayerLocation->x * 80 + 20;
		MyPlayer.y = players_list[0].MyPlayerLocation->y * 80 + 20;

		ingame.UpdateRender();
	}
	close();

	return 0;
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