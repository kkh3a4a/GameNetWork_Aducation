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
//	//�ʱ�ȭ 
//	const char lua_program[] = "print \"Hello\" \n";
//	//lua����ӽ� ����
//	lua_State* L = luaL_newstate();
//	luaL_openlibs(L);
//
//	//lua�� print
//	luaL_loadbuffer(L, lua_program, strlen(lua_program), "line");
//	
//	//lua ����
//	int error = lua_pcall(L, 0, 0, 0);
//
//	if (error) {
//		cout << "Error:" << lua_tostring(L, -1);
//		lua_pop(L, 1);
//	}
//
//
//	//lua����ӽ� ����
//	lua_close(L);
//}