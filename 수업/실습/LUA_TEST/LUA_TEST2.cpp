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
//	int rows =0, cols;
//
//	lua_State* L = luaL_newstate(); //��Ƹ�����.
//	luaL_openlibs(L); //���ǥ�ض��̺귯��������.
//
//	luaL_loadfile(L, "TEST.lua");	//���� ����
//	lua_pcall(L, 0, 0, 0);
//
//	lua_getglobal(L, "rows");
//	lua_getglobal(L, "cols");
//	lua_setglobal(L, "rows");
//
//	rows = (int)lua_tonumber(L, -2);
//	cols = (int)lua_tonumber(L, -1);
//
//	printf("Rows %d, Cols %d\n", rows, cols);
//
//	lua_pop(L, 2);
//	lua_close(L);
//	return 0;
//}