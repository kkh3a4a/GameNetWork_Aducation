#include<iostream>
#include"include/lua.hpp"

#pragma comment (lib, "lua54.lib")



using namespace std;
int add_two_c(lua_State* L)
{
	int a = (int)lua_tonumber(L, -2);
	int b = (int)lua_tonumber(L, -1);
	lua_pop(L, 3);
	lua_pushnumber(L,a + b);
	return 1;
}

int main() {
	int result = 0;

	lua_State* L = luaL_newstate(); //루아를연다.
	luaL_openlibs(L); //루아표준라이브러리를연다.

	luaL_loadfile(L, "TEST.lua");	//파일 열기
	lua_pcall(L, 0, 0, 0);
	lua_register(L, "c_add_two", add_two_c);

	lua_getglobal(L, "add_two");	//plustwo 얻어오기
	lua_pushnumber(L, 100);			//plustwo 인자 넣기
	lua_pushnumber(L, 200);
	lua_pcall(L, 2, 1, 0);			// 실행
	result = lua_tonumber(L, -1);
	lua_pop(L, 1);

	printf("result %d\n", result);


	lua_close(L);
	return 0;
}