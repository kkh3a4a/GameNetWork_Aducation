#include<iostream>
#include"include/lua.hpp"

#pragma comment (lib, "lua54.lib")



using namespace std;
int addnum_c(lua_State* L)
{
	int a = (int)lua_tonumber(L, -2);
	int b = (int)lua_tonumber(L, -1);
	lua_pop(L, 3);
	lua_pushnumber(L,a + b);
	lua_pushnumber(L, a - b);
	return 2;			//return 파라미터 개수 알려주는 값
}

int main() {
	int result = 0, result2 = 0;

	lua_State* L = luaL_newstate(); //루아를연다.
	luaL_openlibs(L); //루아표준라이브러리를연다.

	luaL_loadfile(L, "TEST.lua");	//파일 열기
	lua_pcall(L, 0, 0, 0);
	lua_register(L, "c_addnum", addnum_c);	//c_add_two 랑 add_two_c랑 연동 
												// lua 에서 c_add_two를 호출하면, c함수인 add_two_c를 호출

	lua_getglobal(L, "addnum");		//add_two 얻어오기
	lua_pushnumber(L, 123);			//add_two 인자 넣기
	lua_pushnumber(L, 456);			//add_two 인자 넣기2
	lua_pcall(L, 2, 2, 0);			// 실행
	result = lua_tonumber(L, -2);	//return값은 2개
	result2 = lua_tonumber(L, -1);	
	lua_pop(L, 2);

	printf("result %d, result2 %d\n", result, result2);


	lua_close(L);
	return 0;
}