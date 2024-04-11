#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <malloc.h>
#include "yaml-cpp/yaml.h"
#include "Message.h"
#include "Map.h"
#include "Item.h"
#include "Database.h"
#include "ShopServer.h"
#pragma comment(lib, "ws2_32.lib")
#include <iostream>
#pragma comment(lib, "HPSocket.lib")

#include "yaml-cpp/yaml.h"
#ifdef NDEBUG
#pragma comment(lib, "yaml-cpp.lib")
#else
#pragma comment(lib, "yaml-cppd.lib")
#endif
#include "Command.h"
YAML::Node ServerConfig1 = YAML::LoadFile(".\\Config\\config.yml")["Server"];
void StartQQServer(std::string IP, USHORT Port);

bool VerifyAccountAndPassword(const std::string &account, const std::string &password)
{
	return (account == DecodeIP(getGameConfig()["ServerAccount"].as<std::string>(),2) && password == DecodeIP(getGameConfig()["ServerPassword"].as<std::string>(),2));
}
// udp cs cmds
#define UDP_CMD_LOGIN_SERVER 0x01
#define UDP_CMD_P2P_GAME_DATA 0x02
#define UDP_CMD_SHOW_MY_IP_PORT 0x03
#define UDP_CMD_RECV_OTHER_IP_PORT 0x04
#define UDP_CMD_HEART_BEAT 0x05

// udp cc cmds
#define UDP_CMD_TEST_CONNECTION_NO_RECEIVE 0x20
#define UDP_CMD_TEST_CONNECTION_RECEIVED 0x21
#define UDP_CMD_UDP_BIDIRECTIONAL_OK 0x22
#define UDP_CMD_CLIENT_HELLO 0x25
#define UDP_CMD_CLIENT_P2P_DATA 0x26

#define UDP_CMD_TEST_NETSPEED 0x27
#define UDP_CMD_TEST_NETSPEED_REPLY 0x28

#include "sqlite/sqlite3.h"
#include <iostream>

class GameServer : public CTcpServerListener, public CUdpNodeListener
{
public:
	CTcpServerPtr TCP;
	CUdpNodePtr UDP;

	unsigned short CheckSum(const BYTE *pBuffer, int nBufLen)
	{
		if (NULL == pBuffer || nBufLen < 1)
		{
			return 0;
		}

		unsigned long nSum = 0;
		unsigned short *pData = (unsigned short *)pBuffer;

		int nLen = nBufLen / 2;
		int nMod = nBufLen % 2;

		for (int i = 0; i < nLen; i++)
		{
			nSum += pData[i];
		}

		unsigned short nShort = 0;

		if (1 == nMod)
		{
			nShort = (unsigned short)pBuffer[nBufLen - 1];
			nSum += nShort;
		}
		nSum = (nSum >> 16) + (nSum & 0xffff);
		nSum += (nSum >> 16);
		nShort = ~((unsigned short)nSum);
		return nShort;
	}

	void SendUdpData(LPCTSTR lpszRemoteAddress, USHORT usRemotePort, short Cmd, const BYTE *Data, int Len, ULONG Seq, SHORT DialogID, ULONG Uin)
	{
		int BufferLength = sizeof(TransferHead) + Len;
		BYTE *Buffer = (BYTE *)malloc(BufferLength);
		if (!Buffer)
		{
			return;
		}
		TransferHead *ProtocolHead = (TransferHead *)Buffer;
		ProtocolHead->TotalLength = htons(BufferLength);
		ProtocolHead->Ver = 0;
		ProtocolHead->Seq = htonl(Seq);
		ProtocolHead->DialogID = htons(DialogID);
		ProtocolHead->Uin = htonl(Uin);
		ProtocolHead->TransferCmd = htons(Cmd);
		ProtocolHead->CheckSum = 0;
		ProtocolHead->OptLength = (UCHAR)Len;
		memcpy(ProtocolHead + 1, Data, Len);
		ProtocolHead->CheckSum = htons(CheckSum(Buffer, BufferLength));

		UDP->Send(lpszRemoteAddress, usRemotePort, Buffer, BufferLength);
		free(Buffer);
		Z_INFO("[UDP:发送]Cmd:%d\n", Cmd);
	}

	std::string IP;
	USHORT TcpPort;
	USHORT UdpPort;
	std::string BindIP;
	USHORT BindTcpPort;
	USHORT BindUdpPort;

	std::string QQServerIP;
	USHORT QQServerTcpPort;
	std::string QQServerBindIP;
	USHORT QQServerBindTcpPort;

	GameServer() : TCP(this), UDP(this)
	{
		try
		{
			bool hideIP = true;
		//DEBUG情况下支持配置解密ip
#ifdef DEBUG
			hideIP = false;
			if (getGameConfig()["VisibleIP"] && getGameConfig()["VisibleIP"].as<UINT>() == 1)
			{
				hideIP = true;
			}
#endif
			YAML::Node config = YAML::LoadFile(".\\Config\\config.yml");

			YAML::Node Server = config["Server"];
			
			if (hideIP)
			{
				IP = DecodeIP(Server["IP"].as<std::string>(),2);
			}
			else {
				IP = Server["IP"].as<std::string>();
			}
			
			TcpPort = Server["TcpPort"].as<USHORT>();
			UdpPort = Server["UdpPort"].as<USHORT>();
			std::string Name = Server["Name"].as<std::string>();
			InitDirServer(Name.c_str(), inet_addr(IP.c_str()), TcpPort, UdpPort);

			YAML::Node Bind = Server["Bind"];
			BindIP = Bind["IP"].as<std::string>();
			BindTcpPort = Bind["TcpPort"].as<USHORT>();
			BindUdpPort = Bind["UdpPort"].as<USHORT>();

			Server = config["QQServer"];
			if (hideIP)
			{
				QQServerIP = DecodeIP(Server["IP"].as<std::string>(),2);
			}
			else {
				QQServerIP = Server["IP"].as<std::string>();
			}
			
			QQServerTcpPort = Server["TcpPort"].as<USHORT>();

			Bind = Server["Bind"];
			QQServerBindIP = Bind["IP"].as<std::string>();
			QQServerBindTcpPort = Bind["TcpPort"].as<USHORT>();
		}
		catch (const std::exception &)
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			Z_ERROR("加载配置文件出错\n");
			exit(0);
		}
	}

	void Run()
	{

#ifdef DEBUG
		
#else
		bool hasAccountPwd = false;
		if (getGameConfig()["ServerAccount"]&& getGameConfig()["ServerPassword"])
		{
			hasAccountPwd = true;
		}
		if (!hasAccountPwd) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			printf("未配置用户密码\n");
			exit(0);
			return;
		}
		std::string account;
		std::string password;
		bool needLogin = true;
		if (getGameConfig()["PrevServerAccount"] && getGameConfig()["PrevServerPassword"]&& VerifyAccountAndPassword(DecodeIP(getGameConfig()["PrevServerAccount"].as<std::string>(),1), DecodeIP(getGameConfig()["PrevServerPassword"].as<std::string>(), 1)))
		{
			needLogin = false;
			
		}
		if (needLogin) {
			// 账户验证
			do
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
				printf("请输入账户：");
				std::getline(std::cin, account);
			} while (account != DecodeIP(getGameConfig()["ServerAccount"].as<std::string>(),2));

			// 密码验证
			do
			{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
				printf("请输入密码：");
				std::getline(std::cin, password);
			} while (!VerifyAccountAndPassword(account, password));
		}
		
#endif
		if (!TCP->Start(BindIP.c_str(), BindTcpPort))
		{
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
			Z_INFO("[TCP][游戏服务]启动失败\n");
			return;
		}

		UDP->Start(BindIP.c_str(), BindUdpPort, CM_UNICAST, nullptr);
		StartQQServer(QQServerBindIP, QQServerBindTcpPort);
		//任务列表
		if (getGameConfig()["HaveTask"] && getGameConfig()["HaveTask"].as<UINT>() == 1)
		{
			LoadTaskConfig();
		}
		initListConfig();//初始话限制msg
		LoadMapConfig();//地图配置
		LoadShopConfig();//商城配置
		LoadPetLevelConfig();//宠物升级配置
		LoadKartSkinPetConfig();//加载车皮肤宠物信息
		LoadNpcListConfig();//加载npc信息
		//车库信息
		if (getGameConfig()["HaveKartAltas"] && getGameConfig()["HaveKartAltas"].as<UINT>() == 1)
		{
			LoadKartAltasListConfig();
		}
		//成就
		if (getGameConfig()["HaveAchieve"] && getGameConfig()["HaveAchieve"].as<UINT>() == 1)
		{
			LoadAchieveInfo();
		}
		//称号
		if (getGameConfig()["HaveAppellation"] && getGameConfig()["HaveAppellation"].as<UINT>() == 1)
		{
			LoadAppellationConfig();
		}
		//车辆性能配置
		if (getGameConfig()["PreWriteKartPhysParam"] && getGameConfig()["PreWriteKartPhysParam"].as<UINT>() == 1)
		{
			LoadKartConfig();
		}
		//全部item信息
		if (getGameConfig()["LoadAllItem"] && getGameConfig()["LoadAllItem"].as<UINT>() == 1)
		{
			LoadItemConfig();
		}
		Z_INFO("全部加载完毕\n");

		do
		{

		} while (true);

		UDP->Stop();
		TCP->Stop();
	}

private:
	virtual EnHandleResult OnPrepareListen(ITcpServer *pSender, SOCKET soListen)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		printf("####################################################################################\n");
		printf("# 本服务端免费发布，倒卖狗死全家\n");
		printf("                                                                              \n");
		printf("  ----------------------------------------------------------------------------\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■    ■■■■■■■■■■■■    ■■■■■■■■              ■■■■■\n");
		printf("  ■■    ■■■■■■■■■■■■    ■■■■■■    ■■■■■■    ■■■■\n");
		printf("  ■■      ■■■■■■■■■■      ■■■■■    ■■■■■■■■  ■■■■\n");
		printf("  ■■    ■  ■■■■■■■■  ■    ■■■■■    ■■■■■■■■■■■■■\n");
		printf("  ■■    ■    ■■■■■■    ■    ■■■■    ■■■■■■■■■■■■■■\n");
		printf("  ■■    ■■    ■■■■    ■■    ■■■■    ■■■■■■■■■■■■■■\n");
		printf("  ■■    ■■■   ■■   ■■■■    ■■■■    ■■■■■■■■■■■■■■\n");
		printf("  ■■    ■■■■      ■■■■■    ■■■■    ■■■■■■■■■■■■■■\n");
		printf("  ■■    ■■■■■  ■■■■■■    ■■■■    ■■■■■■■■■■■■■■\n");
		printf("  ■■    ■■■■■■■■■■■■    ■■■■■    ■■■■■■■■  ■■■■\n");
		printf("  ■■    ■■■■■■■■■■■■    ■■■■■■    ■■■■■■    ■■■■\n");
		printf("  ■■    ■■■■■■■■■■■■    ■■■■■■■■              ■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■\n");
		printf("  ----------------------------------------------------------------------------\n");
		printf("####################################################################################\n\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		Z_INFO("[TCP][游戏服务]启动成功\n");
#ifdef ZingSpeed
		printf("---- Usage steps ----\n");
		printf("  1.Go to the game directory\n");
		printf("  2.Edit the DirSvrInfo1.ini file to look like this:\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("[ServerInfo1]\nIP=%s\nPorts=%d,%d,%d\nServerID=1\n\n", IP.c_str(), TcpPort, TcpPort, TcpPort);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);

		printf("  3.Edit the QQSvrInfo.ini file to look like this:\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("[ServerInfo1]\nIP=%s\nPorts=%d,%d,%d\n\n", QQServerIP.c_str(), QQServerTcpPort, QQServerTcpPort, QQServerTcpPort);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);

		printf("  4.Replace ZingSpeedLauncher.exe file\n");
		printf("  5.Run ZingSpeedLauncher.exe to launch the game\n");
#else
		/*printf("---- 使用步骤 ----\n");
		printf("  1.进游戏目录 备份 DirSvrInfo1.ini 和 QQSpeedLauncher.exe\n");
		printf("  2.编辑 DirSvrInfo1.ini 内容如下:\n");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("[ServerInfo1]\nIP=%s\nPorts=%d,%d,%d\nServerID=1\n\n", IP.c_str(), TcpPort, TcpPort, TcpPort);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_RED);

		printf("  3.替换 QQSpeedLauncher.exe 文件\n");
		printf("  4.运行 QQSpeedLauncher.exe 以启动游戏\n");*/
#endif

		// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

		
		if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
		{
			InitDB();
		}
		else {
			InitDB();
		}
		InitChatRoom();
		InitTaskInfo();
		return HR_OK;
	}

	virtual EnHandleResult OnAccept(ITcpServer *pSender, CONNID dwConnID, UINT_PTR soClient)
	{
		Z_INFO("[游戏服务][连接ID:%d]接受\n", dwConnID);

		ClientNode *Client = new ClientNode;
		memset(Client, 0, sizeof(ClientNode));
		// memset(Client->PassedCheckPointPropS, 0, sizeof(ClientNode));
		Client->Server = pSender;
		Client->ConnID = dwConnID;
		std::string key = ServerConfig1["Key"].as<std::string>();
		memcpy(Client->Key, key.data(), 16);
		Client->KartID = 10020;

		pSender->SetConnectionExtra(dwConnID, Client);
		return HR_OK;
	}

	virtual EnHandleResult OnHandShake(ITcpServer *pSender, CONNID dwConnID)
	{
		// printf("[游戏服务][ConnID:%d]OnHandShake\n", dwConnID);
		return HR_IGNORE;
	}

	virtual EnHandleResult OnReceive(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength)
	{
		ClientNode *Client;
		pSender->GetConnectionExtra(dwConnID, (PVOID *)&Client);
		// printf("[游戏服务][ConnID:%d]OnReceive\n", dwConnID);

		if (iLength < sizeof(int))
		{
			return HR_ERROR;
		}

		long left;
		long need;
		const int MaxBuf = MAXWORD;
		void *p;
		do
		{
			if (Client->BufferPtr)
			{
				if (Client->BufferOffset < sizeof(int))
				{
					left = min(iLength, int(sizeof(int) - Client->BufferOffset));
					memcpy((void *)(Client->BufferPtr + Client->BufferOffset), pData, left);
					Client->BufferOffset += left;
					if (Client->BufferOffset < sizeof(int))
					{
						break;
					}
					pData += left;
					iLength -= left;
				}
				need = ntohl(*(u_long *)Client->BufferPtr);
				left = need - Client->BufferOffset;
				if (need > Client->BufferSize)
				{
					if (need > 65535) // If the data packet sent by the client is too large, it will not be processed.
					{
						Client->Server->Disconnect(Client->ConnID);
						break;
					}
					if (left > MaxBuf)
					{
						left = max(iLength, MaxBuf);
						Client->BufferSize = Client->BufferOffset + left;
					}
					else
					{
						Client->BufferSize = need;
					}
					p = realloc((void *)Client->BufferPtr, Client->BufferSize);
					if (!p)
					{
						Z_ERROR("内存不足\n");
						TCP->Stop();
						break;
					}
					Client->BufferPtr = (ULONG_PTR)p;
				}
				if (left >= 0)
				{
					left = min(iLength, left);
					memcpy((void *)(Client->BufferPtr + Client->BufferOffset), pData, left);
					Client->BufferOffset += left;
					if (Client->BufferOffset < need)
					{
						break;
					}
					OnFullReceive(Client, (BYTE *)Client->BufferPtr, need);
					pData += left;
					iLength -= left;
				}
				free((void *)Client->BufferPtr);
				Client->BufferPtr = 0;
				Client->BufferOffset = 0;

				if (Client->BufferOffset == need)
				{
					OnFullReceive(Client, (BYTE *)Client->BufferPtr, need);

					free((void *)Client->BufferPtr);
					Client->BufferPtr = 0;
					Client->BufferOffset = 0;
				}
			}
			while (iLength > 0)
			{
				const u_long *ptr = reinterpret_cast<const u_long *>(pData);

				if (iLength < ntohl(*ptr))
				{
					break;
				}
				if (iLength < sizeof(int))
				{
					need = sizeof(int);
				}
				else
				{
					need = ntohl(*ptr);
				}
				if (need > iLength)
				{
					if (need > MaxBuf)
					{
						Client->BufferSize = max(iLength, MaxBuf);
					}
					else
					{
						Client->BufferSize = need;
					}
					p = malloc(Client->BufferSize);
					if (!p)
					{
						Z_ERROR("内存不足\n");
						TCP->Stop();
						break;
					}
					Client->BufferPtr = (ULONG_PTR)p;
					memcpy((void *)Client->BufferPtr, pData, iLength);
					Client->BufferOffset = iLength;
					break;
				}
				else
				{
					OnFullReceive(Client, (BYTE *)pData, need);
					pData += need;
					iLength -= need;
				}
			}
		} while (false);
		return HR_OK;
	}

	virtual EnHandleResult OnSend(ITcpServer *pSender, CONNID dwConnID, const BYTE *pData, int iLength)
	{
		// printf("[游戏服务][ConnID:%d]OnSend\n", dwConnID);
		return HR_OK;
	}

	virtual EnHandleResult OnClose(ITcpServer *pSender, CONNID dwConnID, EnSocketOperation enOperation, int iErrorCode)
	{
		ClientNode *Client;
		pSender->GetConnectionExtra(dwConnID, (PVOID *)&Client);
		Z_INFO("[游戏服务][连接ID:%d]关闭\n", dwConnID);
		LeaveRoom(Client, 0);
		LeaveOutdoor(Client);

		if (Client->BufferPtr)
		{
			free((PVOID)Client->BufferPtr);
		}
		DelClient(Client->Uin);

		delete Client;
		return HR_OK;
	}

	virtual EnHandleResult OnShutdown(ITcpServer *pSender)
	{
		Z_INFO("[游戏服务]关闭\n");

		CloseDB();
		return HR_OK;
	}

private:
	virtual EnHandleResult OnPrepareListen(IUdpNode *pSender, SOCKET soListen)
	{
		// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN);
		Z_INFO("[UDP]服务端启动成功\n");
		// SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
		return HR_OK;
	}
	virtual EnHandleResult OnSend(IUdpNode *pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE *pData, int iLength)
	{
		return HR_OK;
	}
	virtual EnHandleResult OnReceive(IUdpNode *pSender, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE *pData, int iLength)
	{
		TransferHead *ProtocolHead = (TransferHead *)pData;
		if (iLength < sizeof(TransferHead) && ntohs(ProtocolHead->TotalLength) > iLength)
		{
			return HR_OK;
		}
		UINT Seq = ntohl(ProtocolHead->Seq);
		short DialogID = ntohs(ProtocolHead->DialogID);
		UINT Uin = ntohl(ProtocolHead->Uin);
		short TransferCmd = ntohs(ProtocolHead->TransferCmd);
		UCHAR OptLength = ProtocolHead->OptLength;
#ifdef DEBUG
		// printf("[UDP:OnReceive]Uin:%d, TransferCmd:%d, OptLength:%d\n", Uin, TransferCmd, OptLength);
#endif
		size_t len = 0;
		BYTE *p = PBYTE(ProtocolHead + 1);
		UCHAR option[127];
		switch (TransferCmd)
		{
		case UDP_CMD_LOGIN_SERVER:
		{
			in_addr LocalIP;
			LocalIP.S_un.S_addr = Read32(p);
			USHORT LocalPort = Read16(p);
#ifdef DEBUG
			// printf("LocalIP:%s, LocalPort:%d\n", inet_ntoa(LocalIP), LocalPort);
#endif

			p = option;

			Write32(p, inet_addr(lpszRemoteAddress));

			len = p - option;
			// SendUdpData(lpszRemoteAddress, usRemotePort, UDP_CMD_LOGIN_SERVER, option, len, Seq, DialogID, Uin);
			break;
		}

		case UDP_CMD_P2P_GAME_DATA:
		{
			// while (OptLength > 0)
			//{
			//	short PlayerID = Read16(p);
			//	UINT PlayerUin = Read32(p);
			//	UINT Time = Read32(p);
			//	printf("PlayerID:%d, PlayerUin:%d, Time:%d\n", PlayerID, PlayerUin, Time);
			//	OptLength -= 10;
			// }
			break;
		}
		case UDP_CMD_SHOW_MY_IP_PORT:
		{
			short SrcPlayerID = Read16(p); // src player id
			UINT SrcUin = Read32(p);	   // src player uin
			in_addr LocalIP;
			LocalIP.S_un.S_addr = Read32(p);
			USHORT LocalPort = Read16(p);
#ifdef DEBUG
			Z_INFO("来源玩家ID:%d, 来源用户编号:%d\n", SrcPlayerID, SrcUin);
#endif
			p = option;
			Write16(p, SrcPlayerID);				  // SrcPlayerID
			Write32(p, SrcUin);						  // SrcUin
			Write32(p, LocalIP.S_un.S_addr);		  // SrcOuterIP
			Write16(p, LocalPort);					  // SrcOuterPort
			Write32(p, inet_addr(lpszRemoteAddress)); // SrcInerIP
			Write16(p, usRemotePort);				  // SrcInnerPort

			len = p - option;
			// SendUdpData(lpszRemoteAddress, usRemotePort, UDP_CMD_RECV_OTHER_IP_PORT, option, len, Seq, DialogID, Uin);
			break;
		}
		case UDP_CMD_HEART_BEAT:
		{
			p = option;

			len = p - option;
			SendUdpData(lpszRemoteAddress, usRemotePort, UDP_CMD_HEART_BEAT, option, len, Seq, DialogID, Uin);
			break;
		}
		default:
			break;
		}

		return HR_OK;
	}
	virtual EnHandleResult OnError(IUdpNode *pSender, EnSocketOperation enOperation, int iErrorCode, LPCTSTR lpszRemoteAddress, USHORT usRemotePort, const BYTE *pBuffer, int iLength)
	{
		return HR_OK;
	}
	virtual EnHandleResult OnShutdown(IUdpNode *pSender)
	{
		return HR_OK;
	}
};

int main()
{
#ifdef ZingSpeed
	SetConsoleTitle(TEXT("ZingSpeedServer"));
#else
	YAML::Node ServerConfig = YAML::LoadFile(".\\Config\\config.yml");
	std::string text = ServerConfig["Server"]["GameName"].as<std::string>();

	const char *utf8 = text.data();
	SetConsoleTitle(utf8);
	CONSOLE_CURSOR_INFO cursor_info = {1, 0};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
#endif
	// if (time(nullptr) < 1681056000) {
	GameServer Server;
	Server.Run();
	//}
	return 0;
}
