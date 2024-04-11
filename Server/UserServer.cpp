#include "Message.h"
#include "Database.h"

#include <stdio.h>

void MyResponseGetUin(ClientNode *Client, UINT Uin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);
	Write32(p, -1);
	Write32(p, Uin);

	

	len = p - buf;
	SendToClient(Client, 1, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Response);
	/*ResponseGetWorldInfo(Client, 18883, 18883, 18883);
	RequestLogin2(Client,nullptr,0);*/
}

void MyRequestGetUin(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	size_t len;
	len = Read8(Body);
	if (len == 0 || len == 205) {
		MyResponseGetUin(Client, 0);
		return;
	}
	char* GameKey = ReadString(Body, len);
	YAML::Node ServerConfig1 = YAML::LoadFile(".\\Config\\config.yml")["Server"];
	std::string key = ServerConfig1["Key"].as<std::string>();
	if (strcmp(DecodeIP(GameKey, 1).c_str(), key.data()) != 0)
	{
		MyResponseGetUin(Client, 0);
		return;
	}
	len = Read8(Body);
	char *UserName = new char[len + 1];
	memcpy(UserName, Body, len);
	Body += len;
	UserName[len] = 0;
	len = 0;
	len = Read8(Body);
	if (len ==0|| len == 205) {
		MyResponseGetUin(Client, 0);
		return;
	}
	char* GameVersion = ReadString(Body, len);
	if (strcmp(DecodeIP(GameVersion, 1).c_str(), UserName) != 0 )
	{
		MyResponseGetUin(Client, 0);
		return;
	}
	Z_INFO("用户名:%s\n", UserName);

	DWORD Uin = UserDB_GetUin(UserName);

	delete[] UserName;
	MyResponseGetUin(Client, Uin);
}

void MyResponseRegister(ClientNode *Client, USHORT Result)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	Write32(p, -1);
	Write16(p, Result);

	len = p - buf;
	SendToClient(Client, 2, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Response);
}

void MyRequestRegister(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	size_t len;
	len = Read8(Body);
	if (len == 0 || len == 205) {
		MyResponseGetUin(Client, 0);
		return;
	}
	char* GameKey = ReadString(Body, len);
	YAML::Node ServerConfig1 = YAML::LoadFile(".\\Config\\config.yml")["Server"];
	std::string key = ServerConfig1["Key"].as<std::string>();
	if (strcmp(DecodeIP(GameKey, 1).c_str(), key.data()) != 0)
	{
		MyResponseGetUin(Client, 0);
		return;
	}

	len = Read8(Body);
	char *UserName = new char[len + 1];
	memcpy(UserName, Body, len);
	Body += len;
	UserName[len] = 0;

	len = Read8(Body);
	char *Password = new char[len + 1];
	memcpy(Password, Body, len);
	Body += len;
	Password[len] = 0;

	len = 0;
	len = Read8(Body);
	if (len == 0 || len == 205) {
		MyResponseRegister(Client, 0);
		return;
	}
	char* GameVersion = ReadString(Body, len);
	if (strcmp(DecodeIP(GameVersion, 1).c_str(), UserName) != 0)
	{
		MyResponseRegister(Client, 0);
		return;
	}
	Z_INFO("用户名:%s, 密码:%s\n", UserName, Password);
	USHORT Result = UserDB_Register(UserName, Password) ? 0 : 1;

	delete[] UserName;
	delete[] Password;
	MyResponseRegister(Client, Result);
}
