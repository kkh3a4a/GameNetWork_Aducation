/*This source code copyrighted by Lazy Foo' Productions (2004-2022)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings

#include "Extra_Libs\SDL2-2.24.0\include\SDL.h"
#include "Extra_Libs\SDL2_image-2.6.2\include\SDL_image.h"
#include "Extra_Libs\SDL2_mixer-2.6.2\include\SDL_mixer.h"
#include "Extra_Libs\SDL2_ttf-2.0.15\include\SDL_ttf.h"
#include <stdio.h>
#include <string>
#include <cmath>
#include<iostream>
#include<WS2tcpip.h>
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

//Loads individual image as texture
SDL_Texture* loadTexture(std::string path);
SDL_Texture* playerTex;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;
SDL_Event event;
SDL_Rect destR;
SDL_Point center;
int x = 1;
int y = 1;
bool quit = false;
bool isKeyPress = true;

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

SDL_Texture* loadTexture(std::string path)
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
class Events {
private:
	bool w_Pressed = false;
	bool a_Pressed = false;
	bool s_Pressed = false;
	bool d_Pressed = false;

	int success = 0;


public:
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
				if (event.key.keysym.sym == SDLK_UP) {
					w_Pressed = true;
				}
				if (event.key.keysym.sym == SDLK_LEFT) {
					a_Pressed = true;
				}
				if (event.key.keysym.sym == SDLK_DOWN) {
					s_Pressed = true;
				}
				if (event.key.keysym.sym == SDLK_RIGHT) {
					d_Pressed = true;
				}
			}
		}
		if (event.type == SDL_KEYUP) {
			if (event.key.keysym.sym == SDLK_UP) {
				w_Pressed = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_LEFT) {
				a_Pressed = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_DOWN) {
				s_Pressed = false;
				isKeyPress = true;
			}
			if (event.key.keysym.sym == SDLK_RIGHT) {
				d_Pressed = false;
				isKeyPress = true;
			}
		}
		if (isKeyPress) {
			if (w_Pressed) {
				if (0 < destR.y - 80)
				{
					destR.y -= 80;
					isKeyPress = false;
					cout << destR.x << ", " << destR.y << endl;;
				}
			}
			if (s_Pressed) {
				if (SCREEN_WIDTH > destR.y + 80)
				{
					destR.y += 80;
					isKeyPress = false;
					cout << destR.x << ", " << destR.y << endl;;
				}
			}
			if (a_Pressed) {
				if (0 < destR.x - 80) {
					destR.x -= 80;
					isKeyPress = false;
					cout << destR.x << ", " << destR.y << endl;;
				}
			}
			if (d_Pressed) {
				if (SCREEN_WIDTH > destR.x + 80) {
					destR.x += 80;
					isKeyPress = false;
					cout << destR.x << ", " << destR.y << endl;;
				}
			}
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);

			//Draw blue horizontal line

			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			for (int i = 0; i <= 8; ++i)
				SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH * i / 8, 0, SCREEN_WIDTH * i / 8, SCREEN_HEIGHT);
			for (int i = 0; i <= 8; ++i)
				SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT * i / 8, SCREEN_WIDTH, SCREEN_HEIGHT * i / 8);
			SDL_RenderCopyEx(gRenderer, playerTex, NULL, &destR, 0, &center, SDL_FLIP_NONE);


			//Update screen
			SDL_RenderPresent(gRenderer);
		}
		//cout << destR.x << ", " << destR.y << endl;;
	}

};
Events events;


int SDL_main(int argc, char* args[])
{
	
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag


			//Event handler
			SDL_Surface* tmpSurface;

			tmpSurface = IMG_Load("player.bmp");
			if (!tmpSurface) {
				cout << "Image not loaded" << endl;
				return 0;
			}
			playerTex = SDL_CreateTextureFromSurface(gRenderer, tmpSurface);
			SDL_FreeSurface(tmpSurface);
			int player_size = 40;

			destR.w = player_size;
			destR.h = player_size;
			destR.x = player_size / 2;
			destR.y = SCREEN_HEIGHT - (player_size * 1.5);

			center.x = player_size / 2;
			center.y = player_size / 2;
			//Clear screen

			//While application is running
			SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
			SDL_RenderClear(gRenderer);

			//Draw blue horizontal line

			SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
			for (int i = 0; i <= 8; ++i)
				SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH * i / 8, 0, SCREEN_WIDTH * i / 8, SCREEN_HEIGHT);
			for (int i = 0; i <= 8; ++i)
				SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT * i / 8, SCREEN_WIDTH, SCREEN_HEIGHT * i / 8);
			SDL_RenderCopyEx(gRenderer, playerTex, NULL, &destR, 0, &center, SDL_FLIP_NONE);
			SDL_RenderPresent(gRenderer);
			while (!quit)
			{
				//Handle events on queue


				events.KeyEvent();

			}
		}
	}
	//Free resources and close SDL
	close();

	return 0;
}