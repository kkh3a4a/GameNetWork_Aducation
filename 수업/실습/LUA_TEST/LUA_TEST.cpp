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
//	//초기화 
//	const char lua_program[] = "print \"Hello\" \n";
//	//lua가상머신 생성
//	lua_State* L = luaL_newstate();
//	luaL_openlibs(L);
//
//	//lua에 print
//	luaL_loadbuffer(L, lua_program, strlen(lua_program), "line");
//	
//	//lua 실행
//	int error = lua_pcall(L, 0, 0, 0);
//
//	if (error) {
//		cout << "Error:" << lua_tostring(L, -1);
//		lua_pop(L, 1);
//	}
//
//
//	//lua가상머신 종료
//	lua_close(L);
//}