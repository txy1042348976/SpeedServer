#pragma once
constexpr int MAX_DIR_LISTEN_PORT_NUM = 3;

struct DirServerInfo
{
	int ServerID;
	USHORT LocationID;
	UINT ServerIP;
	USHORT ServerPort[MAX_DIR_LISTEN_PORT_NUM];
	void *MultiIPInfo;
};
struct WorldBaseInfo
{
	BYTE WorldID;
	char Name[256];
	BYTE DirNum;
	DirServerInfo *Dir;
};
WorldBaseInfo g_WorldBaseInfo;

struct GameServerInfo
{
	int ServerID;
	UINT ServerIP;
	USHORT TcpPort;
	USHORT UdpPort;
	USHORT MaxPlayerNum;
	USHORT CurrentPlayerNum;
	UINT RandomKey;
	BYTE IsDisabledLogin;
	void *MultiIPInfo;
};
struct IDCServerInfo
{
	USHORT LocationID;
	BYTE GameServerNum;
	GameServerInfo *GameServer;
};
struct ChannelInfo
{
	BYTE ChannelID;
	BYTE IDCNum;
	IDCServerInfo *IDC;
};
struct CountryInfo
{
	BYTE CountryID;
	BYTE ChannelNum;
	ChannelInfo *Channel;
};
CountryInfo g_CountryInfo;

struct ShopServerInfo
{
	int ServerID;
	USHORT LocationID;
	UINT ServerIP;
	USHORT ServerPort;
};
ShopServerInfo g_ShopServerInfo;

struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_BorderServerInfo;

struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_MatchServerInfo;

struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_RelaxServerInfo;

struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_TeamServerInfo;

struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_DanceServerInfo;

struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_SlideServerInfo;
struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_SkatingServerInfo;
struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_BugReportServerInfo;
struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_FBServerInfo;
struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_MRServerInfo;
struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_WlMatchSvrInfo;
struct
{
	GameServerInfo ServerInfo;
	USHORT LocationID;
} g_NpcServerInfo;
