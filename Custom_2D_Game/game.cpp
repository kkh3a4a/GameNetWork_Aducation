#define _CRT_SECURE_NO_WARNINGS
#include "game.h"
#include <Windows.h>


using namespace std;

void Game::clearRenderer()
{
	// Clear renderer
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
}
void Game::updateRenderer()
{
	// Update screen
	SDL_RenderPresent(renderer);
}

void Game::initVariables()
{
	
}
void Game::initWindow()
{
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		cout << "Could not initialize SDL!" << SDL_GetError() << endl;
	}
	if (TTF_Init() < 0) {
		cout << "Could not initialize SDL!" << SDL_GetError() << endl;
	}


	this->iMonitorWidth = GetSystemMetrics(SM_CXSCREEN);    // ����� ���� �ػ�
	this->iMonitorHeight = GetSystemMetrics(SM_CYSCREEN); //����  �ػ�

	//Create Window
	this->window = SDL_CreateWindow("", iMonitorWidth / 2 - WIDTH / 2, iMonitorHeight / 2 - HEIGHT / 2, WIDTH, HEIGHT, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		cout << "Could not create window!" << SDL_GetError() << endl;;
	}

	// Create renderer
	this->renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
	if (renderer == NULL) {
		cout << "Could not create renderer!" << SDL_GetError() << endl;
	}
}

SDL_Texture* Game::loadImage(const char* directory)
{
	SDL_Texture* Tex;
	tmpSurface = IMG_Load(directory);
	if (!tmpSurface) {
		cout << "Image not loaded" << directory << endl;
	}
	Tex = SDL_CreateTextureFromSurface(renderer, tmpSurface);
	SDL_FreeSurface(tmpSurface);
	return Tex;
}
void Game::loadWavs()
{
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
	gunsound = Mix_LoadWAV("Sounds/gunsound.wav");
}
void Game::loadFont()
{
	font = TTF_OpenFont("arial.ttf", 17);
	if (!font) {
		printf("Could not open font! (%s)\n", TTF_GetError());
	}
}

double Game::calcAngleFromPoints(TF first_point, TF second_point)
{
	float fAngle;
	float fdX = first_point.x - second_point.x;
	float fdY = first_point.y - second_point.y;
	float dRad = atan2(fdY, fdX);
	return fAngle = (dRad * 180) / 3.14159265;
}

int Game::Timer(int start_time, int delay)
{
	int curr_time = clock();
	if (curr_time - start_time > delay && start_time != 0) {
		//cout << curr_time << " " << start_time << endl;
		return 1;
	}
	else
		return 0;
}

void Game::collide_check(float a, float b, float c, float d, float e, float f, float g, float h)//x,y x,y a,b a,b
{
	if (a < g && c > e && b < h && d > f) {
		cout << "collide" << endl;
	}
}

void Game::mouseEvent_menu()
{
	//get mouse coordinates 
	SDL_GetGlobalMouseState(&mouse_X, &mouse_Y);

	mouse_point.x = mouse_X;
	mouse_point.y = mouse_Y;

	//����� �ػ� ��ǥ�� ���� ���� â ��ǥ �������� ��ȯ��Ŵ
	SDL_GetWindowPosition(window, &window_moved_x, &window_moved_y);
	mouse_point.x -= window_moved_x;
	mouse_point.y -= window_moved_y;
}

void Game::drawBackground()
{
	destR.w = background_size + 1000;
	destR.h = background_size;
	destR.x = WIDTH / 2 - (background_size + 1000) / 2;
	destR.y = HEIGHT / 2 - background_size / 2;

	//cout << MyCharPos.x << " " << MyCharPos.y << endl;

	//Draw Background

	SDL_RenderCopy(renderer, backTex, NULL, &destR);
}
void Game::drawGround()
{
	destR.w = ground_size;
	destR.h = ground_size;
	destR.x = WIDTH / 2 - ground_size / 2 - MyCharPos.x;
	destR.y = HEIGHT / 2 - ground_size / 2 - MyCharPos.y;

	//Draw Background
	SDL_RenderCopy(renderer, groundTex, NULL, &destR);
}
void Game::drawCharacter(int playing, SDL_Texture* tex, TF pos, float rot, bool dead)
{
	if (playing != 3) {
		return;
	}
	destR.w = player_size;
	destR.h = player_size;
	center.x = player_size / 2;
	center.y = player_size / 2;

	destR.x = WIDTH / 2 - player_size / 2 + pos.x - MyCharPos.x;
	destR.y = HEIGHT / 2 - player_size / 2 + pos.y - MyCharPos.y;

	if (dead){
		tex = blood_Tex;
		rot = -90;
		destR.w = 50;
		destR.h = 50;
	}
	SDL_RenderCopyEx(renderer, tex, NULL, &destR, rot + 90, &center, SDL_FLIP_NONE);
}
void Game::drawHealthbar(int playing, int health, TF pos)
{
	if (playing != 3) {
		return;
	}
	destR.w = (float)health / 2.5f;
	destR.h = 10;
	destR.x = WIDTH / 2 - 20 + pos.x - MyCharPos.x;
	destR.y = HEIGHT / 2 - 35 + pos.y - MyCharPos.y;
	if (health > 50) {
		SDL_RenderCopy(renderer, greenTex, NULL, &destR);
	}
	else if (health > 25) {
		SDL_RenderCopy(renderer, yellowTex, NULL, &destR);
	}
	else {
		SDL_RenderCopy(renderer, redTex, NULL, &destR);
	}
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

	if (health > 0) {
		SDL_Rect r = { WIDTH / 2 - 40 / 2 + pos.x - MyCharPos.x, HEIGHT / 2 - 70 / 2 + pos.y - MyCharPos.y, 40, 10 };
		SDL_RenderDrawRect(renderer, &r);
	}
}
void Game::drawBullet(int i,float char_angle, TF pos)
{
	player_fire[i].bullet_angle = 3.14159265 * 2 * player_fire[i].fired_angle / 360;
	//�Ѿ��� ���� ��
	if (player_fire[i].show_bullet) {
		player_fire[i].bulletVelo.x = cos(player_fire[i].bullet_angle) * 30;
		player_fire[i].bulletVelo.y = sin(player_fire[i].bullet_angle) * 30;
		player_fire[i].bulletPos.x += (player_fire[i].bulletVelo.x);// +MyVelo.x);
		player_fire[i].bulletPos.y += (player_fire[i].bulletVelo.y);// +MyVelo.y);
		
		destR.w = bullet_size;
		destR.h = bullet_size;
		destR.x = (player_fire[i].bulletPos.x - bullet_size / 2) - MyCharPos.x + player_fire[i].fired_pos.x;
		destR.y = (player_fire[i].bulletPos.y - bullet_size / 2) - MyCharPos.y + player_fire[i].fired_pos.y;
		center.x = bullet_size / 2;
		center.y = bullet_size / 2;
		cout << "������ �Ѿ� x : " << player_fire[i].bulletPos.x << "\ty : " << player_fire[i].bulletPos.y << endl;
		cout << "Ŭ�� ���콺 rotation : " << input.clicked_mouse_rotation << endl;
		//SDL_RenderCopy(renderer, bulletTex, NULL, &destR);
		SDL_RenderCopyEx(renderer, bulletTex, NULL, &destR, player_fire[i].fired_angle, &center, SDL_FLIP_NONE);

		if (player_fire[i].bulletPos.x > WIDTH / 2 + ground_size / 2 - MyCharPos.x ||
			player_fire[i].bulletPos.x <  WIDTH / 2 - ground_size / 2 - MyCharPos.x || 
			player_fire[i].bulletPos.y > HEIGHT / 2 + ground_size / 2 - MyCharPos.y ||
			player_fire[i].bulletPos.y < HEIGHT / 2 - ground_size / 2 - MyCharPos.y) 
		{
			player_fire[i].show_bullet = false;
			//cout << player_fire[i].bulletPos.x << " " << player_fire[i].bulletPos.y << endl;
		}
		else if (player_list[i].collide == true)
		{
			player_fire[i].show_bullet = false;
		}
	}
	//�Ѿ��� ������ ���� ��
	else {
		player_list[i].collide = false;
		player_fire[i].bulletPos.x = WIDTH / 2 + cos(player_fire[i].bullet_angle) * 75 + pos.x - MyCharPos.x;
		player_fire[i].bulletPos.y = HEIGHT / 2 + sin(player_fire[i].bullet_angle) * 75 + pos.y - MyCharPos.y;
		player_fire[i].fired_pos.x = MyCharPos.x;
		player_fire[i].fired_pos.y = MyCharPos.y;
		player_fire[i].unconditional_fired_pos.x = cos(player_fire[i].bullet_angle) * 75 + pos.x;
		player_fire[i].unconditional_fired_pos.y = sin(player_fire[i].bullet_angle) * 75 + pos.y;
		//cout << i << "�� �Ѿ� ���� ��ġ x: " << player_list[i].uncounditional_fired_pos.x << "\ty: " << player_list[i].uncounditional_fired_pos.y << endl;
		//cout << player_fire[i].bulletPos.x << " " << player_fire[i].bulletPos.y << endl;

		//ĳ���Ͱ� �߻��� ������ ������ �Ҵ�
		player_fire[i].fired_angle = char_angle;
	}
}
void Game::drawFlash(int i, float char_angle, TF pos)
{
	//draw flash
	flash_size.x = 150;
	flash_size.y = 80;

	if (player_fire[i].gun_flash) {
		srcR.w = flash_sprite_w / 4;
		srcR.h = flash_sprite_h / 3;

		player_fire[i].flash_i += 1;
		if (player_fire[i].flash_i % 2 == 0) {
			srcR.x += srcR.w;
			srcR.y = 320;
		}

		if (srcR.x >= flash_sprite_w - flash_sprite_w % 4) {
			srcR.x = 0;

			player_fire[i].flash_i = 0;
			player_fire[i].gun_flash = false;
		}

		destR.w = flash_size.x;
		destR.h = flash_size.y;
		destR.x = WIDTH / 2 - flash_size.x / 2 + cos(player_fire[i].bullet_angle) * 75 + pos.x - MyCharPos.x;
		destR.y = HEIGHT / 2 - flash_size.y / 2 + sin(player_fire[i].bullet_angle) * 75 + pos.y - MyCharPos.y;

		center.x = flash_size.x / 2;
		center.y = flash_size.y / 2;

		SDL_RenderCopyEx(renderer, flashTex, &srcR, &destR, player_fire[i].fired_angle, &center, SDL_FLIP_NONE);
	}
	else if(!player_fire[i].gun_flash) {
		player_fire[i].flash_angle = char_angle;
	}
}
void Game::drawLog() 
{
	SDL_Color color = { 0, 0, 0 };
	char state[30]{};
	for (int i = 0; i < 3; i++){
		if (player_list[i].game_state != 3) {
			strcpy(state, "UNKNOWN : NOT EXSIST");
		}
		else if (player_list[i].hp <= 0) {
			strcpy(state, " : DEAD");
		}
		else {
			strcpy(state, " : ALIVE");
		}
		drawText(10, i * 20 + 10, player_info.name[i], color);
		drawText(10 + 13 * strlen(player_info.name[i]), i * 20 + 10, state, color);
	}
}
void Game::mouseEvent_ingame()
{
	//get mouse coordinates 
	SDL_GetGlobalMouseState(&mouse_X, &mouse_Y);

	mouse_point.x = mouse_X;
	mouse_point.y = mouse_Y;

	//����� �ػ� ��ǥ�� ���� ���� â ��ǥ �������� ��ȯ��Ŵ
	SDL_GetWindowPosition(window, &window_moved_x, &window_moved_y);
	mouse_point.x -= window_moved_x;
	mouse_point.y -= window_moved_y;

	//Get Character Angle by Mouse's Coordinates
	my_char_angle = calcAngleFromPoints(mouse_point, middle_pos);
	input.mouse_rotation = my_char_angle;
	//input.unconditional_fired_pos_input = player_fire[0].uncounditional_fired_pos;

	//���콺 �� ��ư ������ �߻�

	if (weapon_type == 0 || player_list[0].bullet[weapon_type] > 0)
	{
		if (event.type == SDL_MOUSEBUTTONDOWN && player_list[0].hp > 0) {
			//������ ������ struct ������ ���콺 ��ǥ ����, clicked
			input.unconditional_fired_pos_input = player_fire[0].unconditional_fired_pos;
			input.clicked_mouse_rotation = my_char_angle;
			if (event.button.button == SDL_BUTTON_LEFT && player_fire[0].gun_fired == false) {
				input.clicked = true;
				cout << "fire" << endl;
				input.unconditional_fired_pos_input = player_fire[0].unconditional_fired_pos;
				input.clicked_mouse_rotation = my_char_angle;
				cout << "Ŭ�� ���콺 rotation : " << input.clicked_mouse_rotation << endl;
				cout << "�÷��̾� ��ġ x : " << player_list[0].player_position.x << "\ty : " << player_list[0].player_position.y << endl;
				cout << "�Ѿ� ������ġ x : " << input.unconditional_fired_pos_input.x << "\ty : " << input.unconditional_fired_pos_input.y << endl;
				cout << "ȭ����� ��ġ x : " << player_fire[0].bulletPos.x << "\ty : " << player_fire[0].bulletPos.y << endl;
				player_fire[0].gun_fired = true;
				fired_time = clock();

				player_fire[0].show_bullet = true;
				player_fire[0].gun_flash = true;

				Mix_PlayChannel(-1, gunsound, 0);
			}
		}

		//���� �߻�ǰ� Ÿ�̸� �۵����Ѽ� 1�� �� �ٽ� �߻� ����
		if (player_fire[0].gun_fired) {
			if (player_list[0].gun_type == 0)
			{
				if (Timer(fired_time, 2000) == 1) {
					input.clicked = false;
					player_fire[0].gun_fired = false;
					fired_time = 0;
				}
			}
			else if (player_list[0].gun_type == 1)
			{
				if (Timer(fired_time, 1000) == 1) {
					input.clicked = false;
					player_fire[0].gun_fired = false;
					fired_time = 0;
				}
			}
			else if (player_list[0].gun_type == 2)
			{
				if (Timer(fired_time, 3000) == 1) {
					input.clicked = false;
					player_fire[0].gun_fired = false;
					fired_time = 0;
				}
			}
			else {

			}
		}
	}
}

void Game::draw_enemy_fire(bool* shoot_check, int i)
{
	if (player_list[i].gun_fired == true && *shoot_check == true) {
		cout << "shoot check : " << *shoot_check << endl;
		Mix_PlayChannel(-1, gunsound, 0);

		// �Ʒ� �� �� ����� �� ���� �� �׸��� - game.player_list[1].player_rotation
		cout << player_list[i].player_rotation << endl;

		player_fire[i].show_bullet = true;
		player_fire[i].gun_flash = true;
		
		*shoot_check = false;
	}

	if (player_list[i].gun_fired == false && *shoot_check == false) {
		*shoot_check = true;
	}
}
void Game::drawCrosshair()
{
	//���콺 ��ǥ ���̱�
	destR.w = crosshair_size;
	destR.h = crosshair_size;
	destR.x = mouse_point.x - crosshair_size / 2;
	destR.y = mouse_point.y - crosshair_size / 2;

	//Draw Target
	SDL_RenderCopy(renderer, targetTex, NULL, &destR);
	//SDL_RenderCopyEx(renderer, targetTex, NULL, &destR, my_char_angle, &center, SDL_FLIP_NONE);
}
void Game::drawText(int x, int y, char text[], SDL_Color color)
{
	if (!font) {
		printf("Could not open font! (%s)\n", TTF_GetError());
		return;
	}
	SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
	if (!surface) {
		//cout << "no surface" << endl;
		return;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);

	SDL_Rect r = { x, y, surface->w, surface->h };
	SDL_RenderCopy(renderer, texture, NULL, &r);
	
	SDL_DestroyTexture(texture);
}
void Game::drawWeaponList()
{
	destR.w = 100;
	destR.h = 100;
	if (weapon_type == 0) {
		destR.x = WIDTH / 2 - 151;
	}
	else if (weapon_type == 1) {
		destR.x = WIDTH / 2 - 50;
	}
	else if (weapon_type == 2) {
		destR.x = WIDTH / 2 + 51;
	}
	destR.y = HEIGHT - 100;
	SDL_RenderCopy(renderer, whiteTex, NULL, &destR);
	
	
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_Rect r = { WIDTH / 2 - 151, HEIGHT - 100, 100, 100 };
	SDL_RenderDrawRect(renderer, &r);
	r = { WIDTH / 2 - 50, HEIGHT - 100, 100, 100 }; 
	SDL_RenderDrawRect(renderer, &r);
	r = { WIDTH / 2 + 51, HEIGHT - 100, 100, 100 };
	SDL_RenderDrawRect(renderer, &r);

	destR.x = WIDTH / 2 - 151;
	destR.y = HEIGHT - 100;
	SDL_RenderCopy(renderer, pistolTex, NULL, &destR);
	destR.x = WIDTH / 2 - 50;
	destR.y = HEIGHT - 100;
	SDL_RenderCopy(renderer, rifleTex, NULL, &destR);
	destR.x = WIDTH / 2 + 51;
	destR.y = HEIGHT - 100;
	SDL_RenderCopy(renderer, sniperTex, NULL, &destR);
	
	
	destR.w = 20;
	destR.h = 20;
	destR.x = WIDTH / 2 - 140;
	destR.y = HEIGHT - 100;
	SDL_RenderCopy(renderer, infinity_Tex, NULL, &destR);
	
	SDL_Color color = { 0, 0, 0 };
	string tmp = to_string(player_list[0].bullet[1]);
	char const* num_char = tmp.c_str();
	drawText(WIDTH / 2 - 40, HEIGHT - 100, (char*)num_char, color);
	tmp = to_string(player_list[0].bullet[2]);
	num_char = tmp.c_str();
	drawText(WIDTH / 2 + 60, HEIGHT - 100, (char*)num_char, color);
}
void Game::drawObstacle()
{
	for (int i = 0; i < MAXITEM; i++) {
		destR.x = WIDTH / 2 - 100 / 2 - MyCharPos.x + created_objects[i].object_position.x;
		destR.y = HEIGHT / 2 - 100 / 2 - MyCharPos.y + created_objects[i].object_position.y;
		
		//cout << created_objects[i].object_position.x << " | " << created_objects[i].object_position.y << endl;
		//cout << MyCharPos.x << " / " << MyCharPos.y << endl;
		if (created_objects[i].object_type == 0) {		//rock
			destR.w = 74;
			destR.h = 74;
			SDL_RenderCopy(renderer, rock_Tex, NULL, &destR);
		}
		else if (created_objects[i].object_type == 1) {		//wall 1
			destR.w = 30;
			destR.h = 100;			
			SDL_RenderCopy(renderer, wall1_Tex, NULL, &destR);
		}
		else if (created_objects[i].object_type == 2){		//wall 2
			destR.w = 100;
			destR.h = 30;
			SDL_RenderCopy(renderer, wall2_Tex, NULL, &destR);
		}
		else if (created_objects[i].object_type == 3) {		//rifle
			destR.w = 40;
			destR.h = 40;
			SDL_RenderCopy(renderer, rifle_ammoTex, NULL, &destR);
		}
		else if (created_objects[i].object_type == 4) {		//sniper
			destR.w = 40;
			destR.h = 40;
			SDL_RenderCopy(renderer, sniper_ammoTex, NULL, &destR);
		}
	}
}

void Game::drawMenu()
{
	SDL_Color color = { 255, 255, 255 };
	drawBackground();

	if (!connect_server) {
		//Press enter to return
		if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) {
			if (text_in_height == 100) {
				text_in_height += 100;
			}
			else if (text_in_height == 200) {
				text_in_height += 100;
			}
			else if (text_in_height == 300) {
				text_in_height = 100;
			}
			text_in = "";
		}
		//Press button to add text
		else if (event.type == SDL_TEXTINPUT && strlen(text_in.c_str()) < 10) {
			text_in += event.text.text;
			if (text_in_height == 100) {
				strcpy(IPAdress, text_in.c_str());
			}
			else if (text_in_height == 200) {
				strcpy(Port, text_in.c_str());
			}
			else if (text_in_height == 300) {
				if(strlen(Name) < 10)
					strcpy(Name, text_in.c_str());
			}
		}
		//Press backspace to erase
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_BACKSPACE && text_in.size()) {
			text_in.pop_back();
			if (text_in_height == 100) {
				strcpy(IPAdress, text_in.c_str());
			}
			else if (text_in_height == 200) {
				strcpy(Port, text_in.c_str());
			}
			else if (text_in_height == 300) {
				strcpy(Name, text_in.c_str());
			}
		}
		else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {

			connect_server = true;
		}
		//Exit event
		else if (event.type == SDL_QUIT) {
			done = 1;
		}

		//Draw subject
		drawText(100, 100, (char*)"IP Adress", color);
		drawText(100, 200, (char*)"Port", color);
		drawText(100, 300, (char*)"Name", color);

		// Draw input rect
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
		SDL_Rect r = { 200, text_in_height, 200, 20 };
		SDL_RenderDrawRect(renderer, &r);

		drawText(200, 100, (char*)IPAdress, color);
		drawText(200, 200, (char*)Port, color);
		drawText(200, 300, (char*)Name, color);
		
		drawText(170, 400, (char*)"Enter to connect", color);
	}
	else {
		if (!waiting_match) {
			drawText(200, 100, (char*)Name, color);
			drawText(200, 200, (char*)"Press enter to find match", color);

			if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
				find_match = true;
				waiting_match = true;
				//curr_state = 1;
			}
		}
	}
	if (waiting_match) {
		drawText(200, 200, (char*)"Finding match...", color);
	}
}

void Game::drawIngame()
{
	drawBackground();
	drawGround();
	drawObstacle();

	// test��
	draw_enemy_fire(&p1_shoot_check, 1);
	draw_enemy_fire(&p2_shoot_check, 2);

	drawBullet(0, my_char_angle, MyCharPos);
	drawBullet(1, player_list[1].player_rotation, player_list[1].player_position);
	drawBullet(2, player_list[2].player_rotation, player_list[2].player_position);

	drawFlash(0, my_char_angle, MyCharPos);
	drawFlash(1, player_list[1].player_rotation, player_list[1].player_position);
	drawFlash(2, player_list[2].player_rotation, player_list[2].player_position);
	
	drawCharacter(player_list[0].game_state, black_playerTex, MyCharPos, my_char_angle, player_list[0].hp <= 0);
	drawCharacter(player_list[1].game_state, red_playerTex, player_list[1].player_position, player_list[1].player_rotation, player_list[1].hp <= 0);
	drawCharacter(player_list[2].game_state, blue_playerTex, player_list[2].player_position, player_list[2].player_rotation, player_list[2].hp <= 0);

	drawHealthbar(player_list[0].game_state, player_list[0].hp, player_list[0].player_position);
	drawHealthbar(player_list[1].game_state, player_list[1].hp, player_list[1].player_position);
	drawHealthbar(player_list[2].game_state, player_list[2].hp, player_list[2].player_position);

	drawCrosshair();
	drawWeaponList();
	drawLog();
}

void Game::drawEndgame()
{
	SDL_Color color = { 255, 255, 255 };
	int shown_player{ 0 };
	for (int i = 0; i < 3; i++) {
		if (player_list[i].game_state == 5) {
			drawText(200, 200, (char*)"Winner :  ", color);
			drawText(200, 200 + 30, player_info.name[i], color);
		}
		else if(player_list[i].game_state == 4) {
			drawText(200, 300, (char*)"Loser :  ", color);
			drawText(200, 330 + 30 * shown_player, player_info.name[i], color);
			shown_player++;
		}
	}		
	
	if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
		curr_state = 0;
	}
}

void Game::update()
{
	SDL_PollEvent(&event);
	if (event.type == SDL_QUIT) {
		done = 1;
	}
	clearRenderer();
	
	if (curr_state == 0) {
		drawMenu();
	}
	else if (curr_state == 1) {
		mouseEvent_ingame();
		keyEvent_ingame();
		drawIngame();
	}
	else if (curr_state == 2) {
		drawEndgame();
	}

	updateRenderer();
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

}

Game::Game()
{
	this->initVariables();
	this->initWindow();

	black_playerTex = this->loadImage("Images/Player_Black.png");
	red_playerTex = this->loadImage("Images/Player_Red.png");
	blue_playerTex = this->loadImage("Images/Player_Blue.png");
	
	blood_Tex = this->loadImage("Images/Blood.png");
	
	targetTex = this->loadImage("Images/Target.png");
	backTex = this->loadImage("Images/Space.jpg");
	groundTex = this->loadImage("Images/Ground.png");
	bulletTex = this->loadImage("Images/Bullet.png");
	flashTex = this->loadImage("Images/Flash.png");
	
	pistolTex = this->loadImage("Images/Pistol.png");
	rifleTex = this->loadImage("Images/Rifle.png");
	sniperTex = this->loadImage("Images/Sniper.png");
	
	whiteTex = this->loadImage("Images/White.png");
	greenTex = this->loadImage("Images/Green.png");
	yellowTex = this->loadImage("Images/Yellow.png");
	redTex = this->loadImage("Images/Red.png");
	
	pistol_ammoTex = this->loadImage("Images/Pistol_ammo.png");
	rifle_ammoTex = this->loadImage("Images/Rifle_ammo.png");
	sniper_ammoTex = this->loadImage("Images/Sniper_ammo.png");

	infinity_Tex = this->loadImage("Images/Infinity.png");

	rock_Tex = this->loadImage("Images/Rock.png");
	wall1_Tex = this->loadImage("Images/Wall1.png");
	wall2_Tex = this->loadImage("Images/Wall2.png");
	
	this->loadWavs();
	this->loadFont();

	//Initialize player's position to middle of window
	MyCharPos.x = 0;
	MyCharPos.y = 0;

	middle_pos.x = WIDTH / 2;
	middle_pos.y = HEIGHT / 2;
}
Game::~Game()
{
	Mix_FreeChunk(gunsound);
	TTF_CloseFont(font);

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
}

void Game::keyEvent_ingame()
{
	//123���� weapon_type ����
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_1) {
			weapon_type = 0;
		}
		else if (event.key.keysym.sym == SDLK_2) {
			weapon_type = 1;
		}
		else if (event.key.keysym.sym == SDLK_3) {
			weapon_type = 2;
		}
	}
	input.gun_type = weapon_type;

	//Fix keyboard status when key pressed
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_w) {
			input.w_Pressed = true;
		}
		if (event.key.keysym.sym == SDLK_a) {
			input.a_Pressed = true;
		}
		if (event.key.keysym.sym == SDLK_s) {
			input.s_Pressed = true;
		}
		if (event.key.keysym.sym == SDLK_d) {
			input.d_Pressed = true;
		}
	}
	if (event.type == SDL_KEYUP) {
		if (event.key.keysym.sym == SDLK_w) {
			input.w_Pressed = false;
		}
		if (event.key.keysym.sym == SDLK_a) {
			input.a_Pressed = false;
		}
		if (event.key.keysym.sym == SDLK_s) {
			input.s_Pressed = false;
		}
		if (event.key.keysym.sym == SDLK_d) {
			input.d_Pressed = false;
		}
	}

}

