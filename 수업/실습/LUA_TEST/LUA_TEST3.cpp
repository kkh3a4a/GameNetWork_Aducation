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
//	int result = 0, result2;
//
//	lua_State* L = luaL_newstate(); //��Ƹ�����.
//	luaL_openlibs(L); //���ǥ�ض��̺귯��������.
//
//	luaL_loadfile(L, "TEST.lua");	//���� ����
//	lua_pcall(L, 0, 0, 0);
//
//	lua_getglobal(L, "plustwo");	//plustwo ������
//	lua_pushnumber(L, 100);			//plustwo ���� �ֱ�
//	lua_pcall(L, 1, 1, 0);			// ����
//	result = lua_tonumber(L, -1);
//	lua_pop(L, 1);
//
//	printf("result %d\n", result);
//
//
//	lua_getglobal(L, "plustwo_subtwo");	//plustwo ������
//	lua_pushnumber(L, 100);			//plustwo ���� �ֱ�
//	lua_pcall(L, 1, 2, 0);			// ����
//	result = lua_tonumber(L, -2);
//	result2 = lua_tonumber(L, -1);
//	lua_pop(L, 2);
//
//	printf("result %d, result2 %d\n", result, result2);
//	
//	lua_close(L);
//	return 0;
//}