//#include<iostream>
//#include"include/lua.hpp"
//
//#pragma comment (lib, "lua54.lib")
//
//
//
//using namespace std;
//
//int main() {
//	int pos_x = 0, pos_y, pos_z;
//
//	lua_State* L = luaL_newstate(); //루아를연다.
//	luaL_openlibs(L); //루아표준라이브러리를연다.
//
//	luaL_loadfile(L, "TEST.lua");	//파일 열기
//	int error = lua_pcall(L, 0, 0, 0);
//	if (error) 
//	{
//		cout << "Error:" << lua_tostring(L, -1);
//		lua_pop(L, 1);
//	}
//
//	lua_getglobal(L, "pos_x");
//	lua_getglobal(L, "pos_y");
//
//	pos_x = (int)lua_tonumber(L, -2);
//	pos_y = (int)lua_tonumber(L, -1);
//	lua_pop(L, 2);
//
//	printf("pos_x %d, pos_y %d\n", pos_x, pos_y);
//
//	lua_getglobal(L, "pos_z");
//	pos_z = (int)lua_tonumber(L, -1);
//	lua_pop(L, 1);
//
//	printf("pos_x %d, pos_y %d, pos_z %d\n", pos_x, pos_y, pos_z);
//	lua_close(L);
//	return 0;
//}