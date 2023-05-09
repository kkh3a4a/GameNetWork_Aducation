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
	return 2;			//return �Ķ���� ���� �˷��ִ� ��
}

int main() {
	int result = 0, result2 = 0;

	lua_State* L = luaL_newstate(); //��Ƹ�����.
	luaL_openlibs(L); //���ǥ�ض��̺귯��������.

	luaL_loadfile(L, "TEST.lua");	//���� ����
	lua_pcall(L, 0, 0, 0);
	lua_register(L, "c_addnum", addnum_c);	//c_add_two �� add_two_c�� ���� 
												// lua ���� c_add_two�� ȣ���ϸ�, c�Լ��� add_two_c�� ȣ��

	lua_getglobal(L, "addnum");		//add_two ������
	lua_pushnumber(L, 123);			//add_two ���� �ֱ�
	lua_pushnumber(L, 456);			//add_two ���� �ֱ�2
	lua_pcall(L, 2, 2, 0);			// ����
	result = lua_tonumber(L, -2);	//return���� 2��
	result2 = lua_tonumber(L, -1);	
	lua_pop(L, 2);

	printf("result %d, result2 %d\n", result, result2);


	lua_close(L);
	return 0;
}