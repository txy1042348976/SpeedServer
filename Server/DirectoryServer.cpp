#include "Message.h"
#include "DirectoryServer.h"
#include <stdio.h>

void InitDirServer(const char *Name, UINT IP, USHORT TcpPort, USHORT UdpPort)
{
	{
		g_WorldBaseInfo.WorldID = 1;

		strcpy_s(g_WorldBaseInfo.Name, Name);

		g_WorldBaseInfo.DirNum = 1;
		DirServerInfo *pDir = new DirServerInfo;
		g_WorldBaseInfo.Dir = pDir;
		{
			pDir->ServerID = 2;
			pDir->LocationID = 1;
			pDir->ServerIP = IP;
			pDir->ServerPort[0] = TcpPort;
			pDir->ServerPort[1] = TcpPort;
			pDir->ServerPort[2] = TcpPort;
			pDir->MultiIPInfo = nullptr;
		}
	}

	{
		g_CountryInfo.CountryID = 1;

		// 在 ZingSpeed 中必须确保有所有频道有相同的 LocationID, 否则无法登录!
		g_CountryInfo.ChannelNum = 5;
		ChannelInfo *pChannel = new ChannelInfo[5];
		g_CountryInfo.Channel = pChannel;
		{
			pChannel->ChannelID = 1; // 新手
			pChannel->IDCNum = 1;
			IDCServerInfo *pIDC = new IDCServerInfo;
			pChannel->IDC = pIDC;
			{
				pIDC->LocationID = 1;
				pIDC->GameServerNum = 1;
				GameServerInfo *pGameServer = new GameServerInfo;
				pIDC->GameServer = pGameServer;
				{
					pGameServer->ServerID = 110;
					pGameServer->ServerIP = IP;
					pGameServer->TcpPort = TcpPort;
					pGameServer->UdpPort = UdpPort;
					pGameServer->MaxPlayerNum = 10000;
					pGameServer->CurrentPlayerNum = 1;
					pGameServer->RandomKey = 0;
					pGameServer->IsDisabledLogin = 0;
					pGameServer->MultiIPInfo = nullptr;
				}
			}
			pChannel++;
		}
		{
			pChannel->ChannelID = 2; // 初级
			pChannel->IDCNum = 1;
			IDCServerInfo *pIDC = new IDCServerInfo;
			pChannel->IDC = pIDC;
			{
				pIDC->LocationID = 1;
				pIDC->GameServerNum = 1;
				GameServerInfo *pGameServer = new GameServerInfo;
				pIDC->GameServer = pGameServer;
				{
					pGameServer->ServerID = 111;
					pGameServer->ServerIP = IP;
					pGameServer->TcpPort = TcpPort;
					pGameServer->UdpPort = UdpPort;
					pGameServer->MaxPlayerNum = 10000;
					pGameServer->CurrentPlayerNum = 1;
					pGameServer->RandomKey = 0;
					pGameServer->IsDisabledLogin = 0;
					pGameServer->MultiIPInfo = nullptr;
				}
			}
			pChannel++;
		}
		{
			pChannel->ChannelID = 3; // 中级
			pChannel->IDCNum = 1;
			IDCServerInfo *pIDC = new IDCServerInfo;
			pChannel->IDC = pIDC;
			{
				pIDC->LocationID = 1;
				pIDC->GameServerNum = 1;
				GameServerInfo *pGameServer = new GameServerInfo;
				pIDC->GameServer = pGameServer;
				{
					pGameServer->ServerID = 112;
					pGameServer->ServerIP = IP;
					pGameServer->TcpPort = TcpPort;
					pGameServer->UdpPort = UdpPort;
					pGameServer->MaxPlayerNum = 10000;
					pGameServer->CurrentPlayerNum = 1;
					pGameServer->RandomKey = 0;
					pGameServer->IsDisabledLogin = 0;
					pGameServer->MultiIPInfo = nullptr;
				}
			}
			pChannel++;
		}
		{
			pChannel->ChannelID = 4; // 高级
			pChannel->IDCNum = 1;
			IDCServerInfo *pIDC = new IDCServerInfo;
			pChannel->IDC = pIDC;
			{
				pIDC->LocationID = 1;
				pIDC->GameServerNum = 1;
				GameServerInfo *pGameServer = new GameServerInfo;
				pIDC->GameServer = pGameServer;
				{
					pGameServer->ServerID = 113;
					pGameServer->ServerIP = IP;
					pGameServer->TcpPort = TcpPort;
					pGameServer->UdpPort = UdpPort;
					pGameServer->MaxPlayerNum = 10000;
					pGameServer->CurrentPlayerNum = 1;
					pGameServer->RandomKey = 0;
					pGameServer->IsDisabledLogin = 0;
					pGameServer->MultiIPInfo = nullptr;
				}
			}
			pChannel++;
		}
		{
			pChannel->ChannelID = 5; // 高级
			pChannel->IDCNum = 1;
			IDCServerInfo *pIDC = new IDCServerInfo;
			pChannel->IDC = pIDC;
			{
				pIDC->LocationID = 1;
				pIDC->GameServerNum = 1;
				GameServerInfo *pGameServer = new GameServerInfo;
				pIDC->GameServer = pGameServer;
				{
					pGameServer->ServerID = 114;
					pGameServer->ServerIP = IP;
					pGameServer->TcpPort = TcpPort;
					pGameServer->UdpPort = UdpPort;
					pGameServer->MaxPlayerNum = 10000;
					pGameServer->CurrentPlayerNum = 1;
					pGameServer->RandomKey = 0;
					pGameServer->IsDisabledLogin = 0;
					pGameServer->MultiIPInfo = nullptr;
				}
			}
			pChannel++;
		}
		{
			pChannel->ChannelID = 6; // 交友
			pChannel->IDCNum = 1;
			IDCServerInfo *pIDC = new IDCServerInfo;
			pChannel->IDC = pIDC;
			{
				pIDC->LocationID = 1;
				pIDC->GameServerNum = 1;
				GameServerInfo *pGameServer = new GameServerInfo;
				pIDC->GameServer = pGameServer;
				{
					pGameServer->ServerID = 115;
					pGameServer->ServerIP = IP;
					pGameServer->TcpPort = TcpPort;
					pGameServer->UdpPort = UdpPort;
					pGameServer->MaxPlayerNum = 10000;
					pGameServer->CurrentPlayerNum = 1;
					pGameServer->RandomKey = 0;
					pGameServer->IsDisabledLogin = 0;
					pGameServer->MultiIPInfo = nullptr;
				}
			}
			pChannel++;
		}
	}

	{
		g_ShopServerInfo.ServerID = 116;
		g_ShopServerInfo.LocationID = 0;
		g_ShopServerInfo.ServerIP = IP;
		g_ShopServerInfo.ServerPort = TcpPort;
	}

	{
		g_BorderServerInfo.ServerInfo.ServerID = 200;
		g_BorderServerInfo.ServerInfo.ServerIP = IP;
		g_BorderServerInfo.ServerInfo.TcpPort = TcpPort;
		g_BorderServerInfo.ServerInfo.UdpPort = UdpPort;
		g_BorderServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_BorderServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_BorderServerInfo.ServerInfo.RandomKey = 0;
		g_BorderServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_BorderServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_BorderServerInfo.LocationID = 1;
	}

	{
		g_MatchServerInfo.ServerInfo.ServerID = 300;
		g_MatchServerInfo.ServerInfo.ServerIP = IP;
		g_MatchServerInfo.ServerInfo.TcpPort = TcpPort;
		g_MatchServerInfo.ServerInfo.UdpPort = UdpPort;
		g_MatchServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_MatchServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_MatchServerInfo.ServerInfo.RandomKey = 0;
		g_MatchServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_MatchServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_MatchServerInfo.LocationID = 1;
	}

	{
		g_RelaxServerInfo.ServerInfo.ServerID = 400;
		g_RelaxServerInfo.ServerInfo.ServerIP = IP;
		g_RelaxServerInfo.ServerInfo.TcpPort = TcpPort;
		g_RelaxServerInfo.ServerInfo.UdpPort = UdpPort;
		g_RelaxServerInfo.ServerInfo.MaxPlayerNum = 100;
		g_RelaxServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_RelaxServerInfo.ServerInfo.RandomKey = 0;
		g_RelaxServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_RelaxServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_RelaxServerInfo.LocationID = 1;
	}

	{
		g_TeamServerInfo.ServerInfo.ServerID = 700;
		g_TeamServerInfo.ServerInfo.ServerIP = IP;
		g_TeamServerInfo.ServerInfo.TcpPort = TcpPort;
		g_TeamServerInfo.ServerInfo.UdpPort = UdpPort;
		g_TeamServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_TeamServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_TeamServerInfo.ServerInfo.RandomKey = 0;
		g_TeamServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_TeamServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_TeamServerInfo.LocationID = 1;
	}

	{
		g_DanceServerInfo.ServerInfo.ServerID = 800;
		g_DanceServerInfo.ServerInfo.ServerIP = IP;
		g_DanceServerInfo.ServerInfo.TcpPort = TcpPort;
		g_DanceServerInfo.ServerInfo.UdpPort = UdpPort;
		g_DanceServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_DanceServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_DanceServerInfo.ServerInfo.RandomKey = 0;
		g_DanceServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_DanceServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_DanceServerInfo.LocationID = 1;
	}

	{
		g_SlideServerInfo.ServerInfo.ServerID = 900;
		g_SlideServerInfo.ServerInfo.ServerIP = IP;
		g_SlideServerInfo.ServerInfo.TcpPort = TcpPort;
		g_SlideServerInfo.ServerInfo.UdpPort = UdpPort;
		g_SlideServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_SlideServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_SlideServerInfo.ServerInfo.RandomKey = 0;
		g_SlideServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_SlideServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_SlideServerInfo.LocationID = 1;
	}
	{
		g_SkatingServerInfo.ServerInfo.ServerID = 1000;
		g_SkatingServerInfo.ServerInfo.ServerIP = IP;
		g_SkatingServerInfo.ServerInfo.TcpPort = TcpPort;
		g_SkatingServerInfo.ServerInfo.UdpPort = UdpPort;
		g_SkatingServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_SkatingServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_SkatingServerInfo.ServerInfo.RandomKey = 0;
		g_SkatingServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_SkatingServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_SkatingServerInfo.LocationID = 1;
	}
	{
		g_BugReportServerInfo.ServerInfo.ServerID = 1100;
		g_BugReportServerInfo.ServerInfo.ServerIP = IP;
		g_BugReportServerInfo.ServerInfo.TcpPort = TcpPort;
		g_BugReportServerInfo.ServerInfo.UdpPort = UdpPort;
		g_BugReportServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_BugReportServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_BugReportServerInfo.ServerInfo.RandomKey = 0;
		g_BugReportServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_BugReportServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_BugReportServerInfo.LocationID = 1;
	}
	{
		g_FBServerInfo.ServerInfo.ServerID = 1200;
		g_FBServerInfo.ServerInfo.ServerIP = IP;
		g_FBServerInfo.ServerInfo.TcpPort = TcpPort;
		g_FBServerInfo.ServerInfo.UdpPort = UdpPort;
		g_FBServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_FBServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_FBServerInfo.ServerInfo.RandomKey = 0;
		g_FBServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_FBServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_FBServerInfo.LocationID = 1;
	}
	{
		g_MRServerInfo.ServerInfo.ServerID = 1300;
		g_MRServerInfo.ServerInfo.ServerIP = IP;
		g_MRServerInfo.ServerInfo.TcpPort = TcpPort;
		g_MRServerInfo.ServerInfo.UdpPort = UdpPort;
		g_MRServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_MRServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_MRServerInfo.ServerInfo.RandomKey = 0;
		g_MRServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_MRServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_MRServerInfo.LocationID = 1;
	}
	{
		g_WlMatchSvrInfo.ServerInfo.ServerID = 1400;
		g_WlMatchSvrInfo.ServerInfo.ServerIP = IP;
		g_WlMatchSvrInfo.ServerInfo.TcpPort = TcpPort;
		g_WlMatchSvrInfo.ServerInfo.UdpPort = UdpPort;
		g_WlMatchSvrInfo.ServerInfo.MaxPlayerNum = 10000;
		g_WlMatchSvrInfo.ServerInfo.CurrentPlayerNum = 1;
		g_WlMatchSvrInfo.ServerInfo.RandomKey = 0;
		g_WlMatchSvrInfo.ServerInfo.IsDisabledLogin = 0;
		g_WlMatchSvrInfo.ServerInfo.MultiIPInfo = nullptr;
		g_WlMatchSvrInfo.LocationID = 1;
	}
	{
		g_NpcServerInfo.ServerInfo.ServerID = 1500;
		g_NpcServerInfo.ServerInfo.ServerIP = IP;
		g_NpcServerInfo.ServerInfo.TcpPort = TcpPort;
		g_NpcServerInfo.ServerInfo.UdpPort = UdpPort;
		g_NpcServerInfo.ServerInfo.MaxPlayerNum = 10000;
		g_NpcServerInfo.ServerInfo.CurrentPlayerNum = 1;
		g_NpcServerInfo.ServerInfo.RandomKey = 0;
		g_NpcServerInfo.ServerInfo.IsDisabledLogin = 0;
		g_NpcServerInfo.ServerInfo.MultiIPInfo = nullptr;
		g_NpcServerInfo.LocationID = 1;
	}
}

int GetServerType(int ServerID)
{
	if (ServerID == g_RelaxServerInfo.ServerInfo.ServerID)
	{
		return Relax;
	}
	if (ServerID == g_DanceServerInfo.ServerInfo.ServerID)
	{
		return Dance;
	}
	if (ServerID == 2000)
	{
		return Engage;
	}
	return Game;
}

void RequestGetWorldInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT ClientVersion = Read32(Body);
	UINT MasterVersion = Read32(Body);
	UINT SlaveVersion = Read32(Body);
	BYTE WorldID = Read8(Body);
	// BYTE FileHash[32];
	Body += 32;

	BYTE ClientCnt = Read8(Body);
	BYTE IsSimplifyVersion = Read8(Body);
#ifndef ZingSpeed
	UINT PlayerFlag = Read32(Body);

	BYTE FileNum = Read8(Body);
	// UINT ClientFileCRC[FileNum];
	Body += FileNum * sizeof(UINT);
#endif
	// printf("[RequestGetWorldInfo]ClientVersion:%d, MasterVersion:%d, SlaveVersion:%d\n", ClientVersion, MasterVersion, SlaveVersion);
	ResponseGetWorldInfo(Client, ClientVersion, MasterVersion, SlaveVersion);
	// Client->Server->Disconnect(Client->ConnID);
}

void ResponseGetWorldInfo(ClientNode *Client, UINT ClientVersion, UINT MasterVersion, UINT SlaveVersion)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	USHORT ResultID = 0;
	const char *Reason = "";
	if (!Client->IsLogin)
	{
		ResultID = 1;
		Reason = "the password is incorrect";
	}

	Write16(p, ResultID);
	Write32(p, ClientVersion); // LastClientVersion
	Write32(p, MasterVersion); // LastMasterVersion
	Write32(p, 4);			   // LastSlaveVersion

	Write8(p, 0); // OldDownloadServerNum

	const char RootDirection[] = "qqkart/test";
	len = strlen(RootDirection);
	Write8(p, (BYTE)len); // RootDirectionLen
	memcpy(p, RootDirection, len);
	p += len;

	Write32(p, 25); // MasterFullVersion
	Write32(p, 0);	// SlaveFullVersion
	Write16(p, 0);	// P2POption

	Write8(p, 1); // WorldCount
	{			  // WorldBaseInfo
		BYTE *pWorldBaseInfo = p;
		Write16(pWorldBaseInfo, 0); // len

		Write8(pWorldBaseInfo, g_WorldBaseInfo.WorldID);

		len = strlen(g_WorldBaseInfo.Name);
		Write8(pWorldBaseInfo, (BYTE)len);
		memcpy(pWorldBaseInfo, g_WorldBaseInfo.Name, len);
		pWorldBaseInfo += len;

		Write8(pWorldBaseInfo, g_WorldBaseInfo.DirNum);
		DirServerInfo *pDir = g_WorldBaseInfo.Dir;
		for (size_t i = 0; i < g_WorldBaseInfo.DirNum; i++)
		{ // DirServerInfo
			BYTE *pDirServerInfo = pWorldBaseInfo;
			Write16(pDirServerInfo, 0); // len

			Write32(pDirServerInfo, pDir->ServerID);
			Write16(pDirServerInfo, pDir->LocationID);
			Write32(pDirServerInfo, pDir->ServerIP);
			for (size_t i = 0; i < MAX_DIR_LISTEN_PORT_NUM; i++)
			{
				Write16(pDirServerInfo, pDir->ServerPort[i]);
			}
#ifndef ZingSpeed
			Write8(pDirServerInfo, pDir->MultiIPInfo ? 1 : 0); // HasMultiIPInfo

#endif
			pDir++;

			len = pDirServerInfo - pWorldBaseInfo;
			Set16(pWorldBaseInfo, (WORD)len);
			pWorldBaseInfo += len;
		}

		len = pWorldBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	if (Client->IsLogin)
	{
		Write8(p, 1); // CountryCount
		{			  // CountryInfo
			BYTE *pCountryInfo = p;
			Write16(pCountryInfo, 0); // len

			Write8(pCountryInfo, g_CountryInfo.CountryID);
			Write8(pCountryInfo, g_CountryInfo.ChannelNum);
			ChannelInfo *pChannel = g_CountryInfo.Channel;
			for (size_t i = 0; i < g_CountryInfo.ChannelNum; i++)
			{
				BYTE *pChannelInfo = pCountryInfo;
				Write16(pChannelInfo, 0); // len

				Write8(pChannelInfo, pChannel->ChannelID);
				Write8(pChannelInfo, pChannel->IDCNum);
				IDCServerInfo *pIDC = pChannel->IDC;
				for (size_t i = 0; i < pChannel->IDCNum; i++)
				{
					BYTE *pIDCServerInfo = pChannelInfo;
					Write16(pIDCServerInfo, 0); // len

					Write16(pIDCServerInfo, pIDC->LocationID);
					Write8(pIDCServerInfo, pIDC->GameServerNum);
					GameServerInfo *pGameServer = pIDC->GameServer;
					for (size_t i = 0; i < pIDC->GameServerNum; i++)
					{
						BYTE *pGameServerInfo = pIDCServerInfo;
						Write16(pGameServerInfo, 0); // len

						Write32(pGameServerInfo, pGameServer->ServerID);
						Write32(pGameServerInfo, pGameServer->ServerIP);
						Write16(pGameServerInfo, pGameServer->TcpPort);
						Write16(pGameServerInfo, pGameServer->UdpPort);
						Write16(pGameServerInfo, pGameServer->MaxPlayerNum);
						Write16(pGameServerInfo, pGameServer->CurrentPlayerNum);
						Write32(pGameServerInfo, pGameServer->RandomKey);
#ifndef ZingSpeed
						Write8(pGameServerInfo, pGameServer->IsDisabledLogin);
						Write8(pGameServerInfo, pGameServer->MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif

						pGameServer++;

						len = pGameServerInfo - pIDCServerInfo;
						Set16(pIDCServerInfo, (WORD)len);
						pIDCServerInfo += len;
					}

					pIDC++;

					len = pIDCServerInfo - pChannelInfo;
					Set16(pChannelInfo, (WORD)len);
					pChannelInfo += len;
				}

				pChannel++;

				len = pChannelInfo - pCountryInfo;
				Set16(pCountryInfo, (WORD)len);
				pCountryInfo += len;
			}

			len = pCountryInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	else
	{
		Write8(p, 0); // CountryCount
	}

	Write8(p, 1); // ShopServerCount
	{			  // ShopServerInfo
		BYTE *pShopServerInfo = p;
		Write16(pShopServerInfo, 0); // len

		Write32(pShopServerInfo, g_ShopServerInfo.ServerID);
		Write16(pShopServerInfo, g_ShopServerInfo.LocationID);
		Write32(pShopServerInfo, g_ShopServerInfo.ServerIP);
		Write16(pShopServerInfo, g_ShopServerInfo.ServerPort);

		len = pShopServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 1); // BorderServerCount
	{			  // BorderServerInfo
		BYTE *pBorderServerInfo = p;
		Write16(pBorderServerInfo, 0); // len

		{
			BYTE *pServerInfo = pBorderServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_BorderServerInfo.ServerInfo.ServerID);
			Write32(pServerInfo, g_BorderServerInfo.ServerInfo.ServerIP);
			Write16(pServerInfo, g_BorderServerInfo.ServerInfo.TcpPort);
			Write16(pServerInfo, g_BorderServerInfo.ServerInfo.UdpPort);
			Write16(pServerInfo, g_BorderServerInfo.ServerInfo.MaxPlayerNum);
			Write16(pServerInfo, g_BorderServerInfo.ServerInfo.CurrentPlayerNum);
			Write32(pServerInfo, g_BorderServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
			Write8(pServerInfo, g_BorderServerInfo.ServerInfo.IsDisabledLogin);
			Write8(pServerInfo, g_BorderServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
			len = pServerInfo - pBorderServerInfo;
			Set16(pBorderServerInfo, (WORD)len);
			pBorderServerInfo += len;
		}
		Write16(pBorderServerInfo, g_BorderServerInfo.LocationID);

		len = pBorderServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 1); // MatchServerCount
	{			  // MatchServerInfo
		BYTE *pMatchServerInfo = p;
		Write16(pMatchServerInfo, 0); // len

		{
			BYTE *pServerInfo = pMatchServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_MatchServerInfo.ServerInfo.ServerID);
			Write32(pServerInfo, g_MatchServerInfo.ServerInfo.ServerIP);
			Write16(pServerInfo, g_MatchServerInfo.ServerInfo.TcpPort);
			Write16(pServerInfo, g_MatchServerInfo.ServerInfo.UdpPort);
			Write16(pServerInfo, g_MatchServerInfo.ServerInfo.MaxPlayerNum);
			Write16(pServerInfo, g_MatchServerInfo.ServerInfo.CurrentPlayerNum);
			Write32(pServerInfo, g_MatchServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
			Write8(pServerInfo, g_MatchServerInfo.ServerInfo.IsDisabledLogin);
			Write8(pServerInfo, g_MatchServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
			len = pServerInfo - pMatchServerInfo;
			Set16(pMatchServerInfo, (WORD)len);
			pMatchServerInfo += len;
		}
		Write16(pMatchServerInfo, g_MatchServerInfo.LocationID);

		len = pMatchServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 1); // RelaxServerCount
	{			  // RelaxServerInfo
		BYTE *pRelaxServerInfo = p;
		Write16(pRelaxServerInfo, 0); // len

		{
			BYTE *pServerInfo = pRelaxServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_RelaxServerInfo.ServerInfo.ServerID);
			Write32(pServerInfo, g_RelaxServerInfo.ServerInfo.ServerIP);
			Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.TcpPort);
			Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.UdpPort);
			Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.MaxPlayerNum);
			Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.CurrentPlayerNum);
			Write32(pServerInfo, g_RelaxServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
			Write8(pServerInfo, g_RelaxServerInfo.ServerInfo.IsDisabledLogin);
			Write8(pServerInfo, g_RelaxServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
			len = pServerInfo - pRelaxServerInfo;
			Set16(pRelaxServerInfo, (WORD)len);
			pRelaxServerInfo += len;
		}
		Write16(pRelaxServerInfo, g_RelaxServerInfo.LocationID);

		len = pRelaxServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // NetAdminServerCount

	len = strlen(Reason);
	Write16(p, (WORD)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;

	Write8(p, 0); // DownloadServerNum

	Write8(p, 1); // TeamServerCount
	{			  // TeamServerInfo
		BYTE *pTeamServerInfo = p;
		Write16(pTeamServerInfo, 0); // len

		{
			BYTE *pServerInfo = pTeamServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_TeamServerInfo.ServerInfo.ServerID);
			Write32(pServerInfo, g_TeamServerInfo.ServerInfo.ServerIP);
			Write16(pServerInfo, g_TeamServerInfo.ServerInfo.TcpPort);
			Write16(pServerInfo, g_TeamServerInfo.ServerInfo.UdpPort);
			Write16(pServerInfo, g_TeamServerInfo.ServerInfo.MaxPlayerNum);
			Write16(pServerInfo, g_TeamServerInfo.ServerInfo.CurrentPlayerNum);
			Write32(pServerInfo, g_TeamServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
			Write8(pServerInfo, g_TeamServerInfo.ServerInfo.IsDisabledLogin);
			Write8(pServerInfo, g_TeamServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
			len = pServerInfo - pTeamServerInfo;
			Set16(pTeamServerInfo, (WORD)len);
			pTeamServerInfo += len;
		}
		Write16(pTeamServerInfo, g_TeamServerInfo.LocationID);

		len = pTeamServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 1); // DanceServerCount
	{			  // DanceServerInfo
		BYTE *pDanceServerInfo = p;
		Write16(pDanceServerInfo, 0); // len

		{
			BYTE *pServerInfo = pDanceServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_DanceServerInfo.ServerInfo.ServerID);
			Write32(pServerInfo, g_DanceServerInfo.ServerInfo.ServerIP);
			Write16(pServerInfo, g_DanceServerInfo.ServerInfo.TcpPort);
			Write16(pServerInfo, g_DanceServerInfo.ServerInfo.UdpPort);
			Write16(pServerInfo, g_DanceServerInfo.ServerInfo.MaxPlayerNum);
			Write16(pServerInfo, g_DanceServerInfo.ServerInfo.CurrentPlayerNum);
			Write32(pServerInfo, g_DanceServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
			Write8(pServerInfo, g_DanceServerInfo.ServerInfo.IsDisabledLogin);
			Write8(pServerInfo, g_DanceServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
			len = pServerInfo - pDanceServerInfo;
			Set16(pDanceServerInfo, (WORD)len);
			pDanceServerInfo += len;
		}
		Write16(pDanceServerInfo, g_DanceServerInfo.LocationID);

		len = pDanceServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 1); // SlideServerCount
	{			  // SlideServerInfo
		BYTE *pSlideServerInfo = p;
		Write16(pSlideServerInfo, 0); // len

		{
			BYTE *pServerInfo = pSlideServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_SlideServerInfo.ServerInfo.ServerID);
			Write32(pServerInfo, g_SlideServerInfo.ServerInfo.ServerIP);
			Write16(pServerInfo, g_SlideServerInfo.ServerInfo.TcpPort);
			Write16(pServerInfo, g_SlideServerInfo.ServerInfo.UdpPort);
			Write16(pServerInfo, g_SlideServerInfo.ServerInfo.MaxPlayerNum);
			Write16(pServerInfo, g_SlideServerInfo.ServerInfo.CurrentPlayerNum);
			Write32(pServerInfo, g_SlideServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
			Write8(pServerInfo, g_SlideServerInfo.ServerInfo.IsDisabledLogin);
			Write8(pServerInfo, g_SlideServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
			len = pServerInfo - pSlideServerInfo;
			Set16(pSlideServerInfo, (WORD)len);
			pSlideServerInfo += len;
		}
		Write16(pSlideServerInfo, g_SlideServerInfo.LocationID);

		len = pSlideServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); // UpdateType
	Write8(p, 0);  // OpenOtherIDCFlag
	Write16(p, 0); // OtherIDCID

	Write8(p, 1); // BugReportServerCount
	for (size_t i = 0; i < 1; i++)
	{ // BugReportServerInfo
		BYTE *pBugReportServerInfo = p;
		Write16(pBugReportServerInfo, 0); // len

		Write32(pBugReportServerInfo, g_BugReportServerInfo.ServerInfo.ServerID); // ServerID
		Write16(pBugReportServerInfo, g_BugReportServerInfo.LocationID);		  // LocationID
		Write32(pBugReportServerInfo, g_BugReportServerInfo.ServerInfo.ServerID); // ServerIP
		Write16(pBugReportServerInfo, g_BugReportServerInfo.ServerInfo.TcpPort);  // ServerPort
		Write8(pBugReportServerInfo, 0);										  // HasMultiIPInfo

		len = pBugReportServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // IsDynamicDownloadPermitted
	Write8(p, 0); // DynamicDownloadResourceIdNum
	// DynamicDownloadResourceIds[]

	Write8(p, 0); // EnableRandMapMusicPreKnown

	Write16(p, 0); // MinPlayerNumForRand
	Write16(p, 0); // PlayerNumDeltaForRand

	Write8(p, 1); // FBServerCount
	for (size_t i = 0; i < 1; i++)
	{ // FBServerInfo
		BYTE *pFBServerInfo = p;
		Write16(pFBServerInfo, 0); // len

		{ // ServerInfo
			BYTE *pServerInfo = pFBServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_FBServerInfo.ServerInfo.ServerID);		  // ServerID
			Write32(pServerInfo, g_FBServerInfo.ServerInfo.ServerIP);		  // ServerIP
			Write16(pServerInfo, g_FBServerInfo.ServerInfo.TcpPort);		  // TcpPort
			Write16(pServerInfo, g_FBServerInfo.ServerInfo.UdpPort);		  // UdpPort
			Write16(pServerInfo, g_FBServerInfo.ServerInfo.MaxPlayerNum);	  // MaxPlayerNum
			Write16(pServerInfo, g_FBServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
			Write32(pServerInfo, g_FBServerInfo.ServerInfo.RandomKey);		  // RandomKey
			Write8(pServerInfo, g_FBServerInfo.ServerInfo.IsDisabledLogin);	  // IsDisabledLogin
			Write8(pServerInfo, 0);											  // HasMultiIPInfo

			len = pServerInfo - pFBServerInfo;
			Set16(pFBServerInfo, (WORD)len);
			pFBServerInfo += len;
		}
		Write16(pFBServerInfo, g_FBServerInfo.LocationID); // LocationID
		Write32(pFBServerInfo, 0);						   // ServerRoomType

		len = pFBServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	const char WebCGIDomain[] = "localhost";
	len = strlen(WebCGIDomain);
	Write8(p, (BYTE)len); // WebCGIDomainLen
	memcpy(p, WebCGIDomain, len);
	p += len;

	Write16(p, 8000); // WebCGIServerPort

	Write8(p, 1); // WlMatchSvrCount
	for (size_t i = 0; i < 1; i++)
	{ // WlMatchSvrInfo
		BYTE *pWlMatchSvrInfo = p;
		Write16(pWlMatchSvrInfo, 0); // len

		{ // ServerInfo
			BYTE *pServerInfo = pWlMatchSvrInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_WlMatchSvrInfo.ServerInfo.ServerID);			// ServerID
			Write32(pServerInfo, g_WlMatchSvrInfo.ServerInfo.ServerIP);			// ServerIP
			Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.TcpPort);			// TcpPort
			Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.UdpPort);			// UdpPort
			Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.MaxPlayerNum);		// MaxPlayerNum
			Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
			Write32(pServerInfo, g_WlMatchSvrInfo.ServerInfo.RandomKey);		// RandomKey
			Write8(pServerInfo, g_WlMatchSvrInfo.ServerInfo.IsDisabledLogin);	// IsDisabledLogin
			Write8(pServerInfo, 0);												// HasMultiIPInfo
			len = pServerInfo - pWlMatchSvrInfo;
			Set16(pWlMatchSvrInfo, (WORD)len);
			pWlMatchSvrInfo += len;
		}
		Write16(pWlMatchSvrInfo, g_WlMatchSvrInfo.LocationID); // LocationID
		Write32(pWlMatchSvrInfo, 0);						   // ServerRoomType

		len = pWlMatchSvrInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); // SkateServerCount
	for (size_t i = 0; i < 1; i++)
	{ // SkateServerInfo
		BYTE *pSkateServerInfo = p;
		Write16(pSkateServerInfo, 0); // len

		{ // CommServerInfo
			BYTE *pCommServerInfo = pSkateServerInfo;
			Write16(pCommServerInfo, 0); // len

			{ // ServerInfo
				BYTE *pServerInfo = pCommServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_SkatingServerInfo.ServerInfo.ServerID);		   // ServerID
				Write32(pServerInfo, g_SkatingServerInfo.ServerInfo.ServerIP);		   // ServerIP
				Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.TcpPort);		   // TcpPort
				Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.UdpPort);		   // UdpPort
				Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.MaxPlayerNum);	   // MaxPlayerNum
				Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
				Write32(pServerInfo, g_SkatingServerInfo.ServerInfo.RandomKey);		   // RandomKey
				Write8(pServerInfo, g_SkatingServerInfo.ServerInfo.IsDisabledLogin);   // IsDisabledLogin
				Write8(pServerInfo, 0);												   // HasMultiIPInfo

				len = pServerInfo - pCommServerInfo;
				Set16(pCommServerInfo, (WORD)len);
				pCommServerInfo += len;
			}
			Write16(pCommServerInfo, g_SkatingServerInfo.LocationID); // LocationID
			Write32(pCommServerInfo, 0);							  // ServerRoomType

			len = pCommServerInfo - pSkateServerInfo;
			Set16(pSkateServerInfo, (WORD)len);
			pSkateServerInfo += len;
		}
		Write32(pSkateServerInfo, 0); // ChannelId

		len = pSkateServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
#ifndef ZingSpeed
	Write8(p, 0); // VersionCount
	// VersionList[]

	Write32(p, 1); // IsEnableBETA
	Write32(p, 0); // AreaID

	Write8(p, 0); // PreVersionCount
	// PreVersionList[]

	Write8(p, 1); // IsEnableMapTester
	Write8(p, 0); // DisableRandomDanceSvr
	Write8(p, 0); // DisableRandomSkateSvr

	// m_abyClientSwitch[16]
	memset(p, 0, 16);
	p += 16;

	Write16(p, 0); // PreResoveDomainNum
	// for (size_t i = 0; i < 1; i++)
	//{ //DomainList
	//	BYTE* pDomainList = p;
	//	Write16(pDomainList, 0); //len

	//	Write8(pDomainList, 0); //IPCount
	//	Write32(pDomainList, 0); //ServerIP[]
	//	Write8(pDomainList, 0); //DomainLen

	//	len = pDomainList - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	Write8(p, 1); // MRServerCount
	for (size_t i = 0; i < 1; i++)
	{ // MRServerInfo
		BYTE *pMRServerInfo = p;
		Write16(pMRServerInfo, 0); // len

		{ // ServerInfo
			BYTE *pServerInfo = pMRServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_MRServerInfo.ServerInfo.ServerID);		  // ServerID
			Write32(pServerInfo, g_MRServerInfo.ServerInfo.ServerIP);		  // ServerIP
			Write16(pServerInfo, g_MRServerInfo.ServerInfo.TcpPort);		  // TcpPort
			Write16(pServerInfo, g_MRServerInfo.ServerInfo.UdpPort);		  // UdpPort
			Write16(pServerInfo, g_MRServerInfo.ServerInfo.MaxPlayerNum);	  // MaxPlayerNum
			Write16(pServerInfo, g_MRServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
			Write32(pServerInfo, g_MRServerInfo.ServerInfo.RandomKey);		  // RandomKey
			Write8(pServerInfo, g_MRServerInfo.ServerInfo.IsDisabledLogin);	  // IsDisabledLogin
			Write8(pServerInfo, 0);											  // HasMultiIPInfo

			len = pServerInfo - pMRServerInfo;
			Set16(pMRServerInfo, (WORD)len);
			pMRServerInfo += len;
		}
		Write16(pMRServerInfo, g_MRServerInfo.LocationID); // LocationID
		Write32(pMRServerInfo, 0);						   // ServerRoomType

		len = pMRServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); // MinPlayerNumForRandForGsvrd6

	Write8(p, 1); // NpcServerCount
	for (size_t i = 0; i < 1; i++)
	{ // NpcServerInfo
		BYTE *pNpcServerInfo = p;
		Write16(pNpcServerInfo, 0); // len

		{ // ServerInfo
			BYTE *pServerInfo = pNpcServerInfo;
			Write16(pServerInfo, 0); // len

			Write32(pServerInfo, g_NpcServerInfo.ServerInfo.ServerID);		   // ServerID
			Write32(pServerInfo, g_NpcServerInfo.ServerInfo.ServerIP);		   // ServerIP
			Write16(pServerInfo, g_NpcServerInfo.ServerInfo.TcpPort);		   // TcpPort
			Write16(pServerInfo, g_NpcServerInfo.ServerInfo.UdpPort);		   // UdpPort
			Write16(pServerInfo, g_NpcServerInfo.ServerInfo.MaxPlayerNum);	   // MaxPlayerNum
			Write16(pServerInfo, g_NpcServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
			Write32(pServerInfo, g_NpcServerInfo.ServerInfo.RandomKey);		   // RandomKey
			Write8(pServerInfo, g_NpcServerInfo.ServerInfo.IsDisabledLogin);   // IsDisabledLogin
			Write8(pServerInfo, 0);											   // HasMultiIPInfo

			len = pServerInfo - pNpcServerInfo;
			Set16(pNpcServerInfo, (WORD)len);
			pNpcServerInfo += len;
		}
		Write16(pNpcServerInfo, g_NpcServerInfo.LocationID); // LocationID
		Write32(pNpcServerInfo, 0);							 // ServerRoomType

		len = pNpcServerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // PatchCount
	// for (size_t i = 0; i < 1; i++)
	//{ //PatchMD5List
	//	BYTE* pPatchMD5List = p;
	//	Write16(pPatchMD5List, 0); //len

	//	Write16(pPatchMD5List, 0); //BeginVer
	//	Write16(pPatchMD5List, 0); //EndVer
	//	Write8(pPatchMD5List, 0); //MD5Len

	//	len = pPatchMD5List - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	Write8(p, 0); // UrlCount
	// for (size_t i = 0; i < 1; i++)
	//{ //PreLoadList
	//	BYTE* pPreLoadList = p;
	//	Write16(pPreLoadList, 0); //len

	//	Write16(pPreLoadList, 0); //Width
	//	Write16(pPreLoadList, 0); //Height
	//	Write8(pPreLoadList, 0); //WithSig
	//	Write8(pPreLoadList, 0); //URLLen

	//	len = pPreLoadList - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	Write16(p, 0); // LoginBuffLen

	Write16(p, 0); // WebURLLen

#endif

	len = p - buf;
	SendToClient(Client, 90, buf, len, 0, FE_DIRSVRD, 0, Response);
}

void ResponseGetChatRoomList(ClientNode *Client, USHORT GetRoomType, USHORT GetRoomNum, USHORT StartRoomIdx);
void RequestGetChatRoomList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT StartRoomIdx = Read16(Body);
	USHORT GetRoomNum = Read16(Body);
	USHORT GetRoomType = Read16(Body);
#ifdef DEBUG
	Z_INFO("获取房间类型:%d\n", GetRoomType);
#endif
	ResponseGetChatRoomList(Client, GetRoomType, GetRoomNum, StartRoomIdx);
}

void ResponseGetChatRoomList(ClientNode *Client, USHORT GetRoomType, USHORT GetRoomNum, USHORT StartRoomIdx)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	Write16(p, 0);			  // ResultID
	Write8(p, 0);			  // ReasonLen
	Write16(p, ChatRoomNum);  // CurTotalRoomNum
	Write16(p, StartRoomIdx); // StartRoomIdx
	BYTE *pPetNum = p;
	Write16(p, 0); // ChatRoomNum
	size_t count = 0;
	for (size_t i = StartRoomIdx; i < ChatRoomNum; i++)
	{ // ChatRoomInfo
		if (count >= 34)
		{
			break;
		}
		else
		{
			count++;
		}
		BYTE *pChatRoomInfo = p;
		Write16(pChatRoomInfo, 0);									   // len
		Write32(pChatRoomInfo, g_RelaxServerInfo.ServerInfo.ServerID); // ServerID
		Write32(pChatRoomInfo, ChatRooms[i].ID);					   // ChatRoomID

		len = strlen(ChatRooms[i].Name);
		Write8(pChatRoomInfo, (BYTE)len); // RoomNameLen
		memcpy(pChatRoomInfo, ChatRooms[i].Name, len);
		pChatRoomInfo += len;

		Write16(pChatRoomInfo, ChatRooms[i].CurrPlayerNum); // CurrPlayerNum
		Write16(pChatRoomInfo, ChatRooms[i].MaxPlayerNum);	// MaxPlayerNum

		Write32(pChatRoomInfo, 0);					  // RoomType
		Write16(pChatRoomInfo, ChatRooms[i].SceneID); // SceneID

		Write16(pChatRoomInfo, 0); // CurrMaleNum
		Write16(pChatRoomInfo, 0); // ChatAreaId

		len = pChatRoomInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Set16(pPetNum, count);
	Write16(p, GetRoomType); // GetRoomType

	len = p - buf;

	SendToClient(Client, 91, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void ResponseWorldOnlineInfo(ClientNode *Client)
{
	int n = 0;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, 1); // UpdateOnlineInfoInterval
	{			  // Info
		BYTE *pInfo = p;
		Write16(pInfo, 0); // len

		if (Client->IsLogin)
		{
			Write8(p, 1); // CountryCount
			{			  // CountryInfo
				BYTE *pCountryInfo = p;
				Write16(pCountryInfo, 0); // len

				Write8(pCountryInfo, g_CountryInfo.CountryID);
				Write8(pCountryInfo, g_CountryInfo.ChannelNum);
				ChannelInfo *pChannel = g_CountryInfo.Channel;
				for (size_t i = 0; i < g_CountryInfo.ChannelNum; i++)
				{
					BYTE *pChannelInfo = pCountryInfo;
					Write16(pChannelInfo, 0); // len

					Write8(pChannelInfo, pChannel->ChannelID);
					Write8(pChannelInfo, pChannel->IDCNum);
					IDCServerInfo *pIDC = pChannel->IDC;
					for (size_t i = 0; i < pChannel->IDCNum; i++)
					{
						BYTE *pIDCServerInfo = pChannelInfo;
						Write16(pIDCServerInfo, 0); // len

						Write16(pIDCServerInfo, pIDC->LocationID);
						Write8(pIDCServerInfo, pIDC->GameServerNum);
						GameServerInfo *pGameServer = pIDC->GameServer;
						for (size_t i = 0; i < pIDC->GameServerNum; i++)
						{
							BYTE *pGameServerInfo = pIDCServerInfo;
							Write16(pGameServerInfo, 0); // len

							Write32(pGameServerInfo, pGameServer->ServerID);
							Write32(pGameServerInfo, pGameServer->ServerIP);
							Write16(pGameServerInfo, pGameServer->TcpPort);
							Write16(pGameServerInfo, pGameServer->UdpPort);
							Write16(pGameServerInfo, pGameServer->MaxPlayerNum);
							Write16(pGameServerInfo, pGameServer->CurrentPlayerNum);
							Write32(pGameServerInfo, pGameServer->RandomKey);
#ifndef ZingSpeed
							Write8(pGameServerInfo, pGameServer->IsDisabledLogin);
							Write8(pGameServerInfo, pGameServer->MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif

							pGameServer++;

							len = pGameServerInfo - pIDCServerInfo;
							Set16(pIDCServerInfo, (WORD)len);
							pIDCServerInfo += len;
						}

						pIDC++;

						len = pIDCServerInfo - pChannelInfo;
						Set16(pChannelInfo, (WORD)len);
						pChannelInfo += len;
					}

					pChannel++;

					len = pChannelInfo - pCountryInfo;
					Set16(pCountryInfo, (WORD)len);
					pCountryInfo += len;
				}

				len = pCountryInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
		else
		{
			Write8(p, 0); // CountryCount
		}
		Write8(p, 1); // BorderServerCount
		{			  // BorderServerInfo
			BYTE *pBorderServerInfo = p;
			Write16(pBorderServerInfo, 0); // len

			{
				BYTE *pServerInfo = pBorderServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_BorderServerInfo.ServerInfo.ServerID);
				Write32(pServerInfo, g_BorderServerInfo.ServerInfo.ServerIP);
				Write16(pServerInfo, g_BorderServerInfo.ServerInfo.TcpPort);
				Write16(pServerInfo, g_BorderServerInfo.ServerInfo.UdpPort);
				Write16(pServerInfo, g_BorderServerInfo.ServerInfo.MaxPlayerNum);
				Write16(pServerInfo, g_BorderServerInfo.ServerInfo.CurrentPlayerNum);
				Write32(pServerInfo, g_BorderServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
				Write8(pServerInfo, g_BorderServerInfo.ServerInfo.IsDisabledLogin);
				Write8(pServerInfo, g_BorderServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
				len = pServerInfo - pBorderServerInfo;
				Set16(pBorderServerInfo, (WORD)len);
				pBorderServerInfo += len;
			}
			Write16(pBorderServerInfo, g_BorderServerInfo.LocationID);

			len = pBorderServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

		Write8(p, 1); // MatchServerCount
		{			  // MatchServerInfo
			BYTE *pMatchServerInfo = p;
			Write16(pMatchServerInfo, 0); // len

			{
				BYTE *pServerInfo = pMatchServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_MatchServerInfo.ServerInfo.ServerID);
				Write32(pServerInfo, g_MatchServerInfo.ServerInfo.ServerIP);
				Write16(pServerInfo, g_MatchServerInfo.ServerInfo.TcpPort);
				Write16(pServerInfo, g_MatchServerInfo.ServerInfo.UdpPort);
				Write16(pServerInfo, g_MatchServerInfo.ServerInfo.MaxPlayerNum);
				Write16(pServerInfo, g_MatchServerInfo.ServerInfo.CurrentPlayerNum);
				Write32(pServerInfo, g_MatchServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
				Write8(pServerInfo, g_MatchServerInfo.ServerInfo.IsDisabledLogin);
				Write8(pServerInfo, g_MatchServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
				len = pServerInfo - pMatchServerInfo;
				Set16(pMatchServerInfo, (WORD)len);
				pMatchServerInfo += len;
			}
			Write16(pMatchServerInfo, g_MatchServerInfo.LocationID);

			len = pMatchServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

		Write8(p, 1); // RelaxServerCount
		{			  // RelaxServerInfo
			BYTE *pRelaxServerInfo = p;
			Write16(pRelaxServerInfo, 0); // len

			{
				BYTE *pServerInfo = pRelaxServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_RelaxServerInfo.ServerInfo.ServerID);
				Write32(pServerInfo, g_RelaxServerInfo.ServerInfo.ServerIP);
				Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.TcpPort);
				Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.UdpPort);
				Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.MaxPlayerNum);
				Write16(pServerInfo, g_RelaxServerInfo.ServerInfo.CurrentPlayerNum);
				Write32(pServerInfo, g_RelaxServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
				Write8(pServerInfo, g_RelaxServerInfo.ServerInfo.IsDisabledLogin);
				Write8(pServerInfo, g_RelaxServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
				len = pServerInfo - pRelaxServerInfo;
				Set16(pRelaxServerInfo, (WORD)len);
				pRelaxServerInfo += len;
			}
			Write16(pRelaxServerInfo, g_RelaxServerInfo.LocationID);

			len = pRelaxServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 1); // TeamServerCount
		{			  // TeamServerInfo
			BYTE *pTeamServerInfo = p;
			Write16(pTeamServerInfo, 0); // len

			{
				BYTE *pServerInfo = pTeamServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_TeamServerInfo.ServerInfo.ServerID);
				Write32(pServerInfo, g_TeamServerInfo.ServerInfo.ServerIP);
				Write16(pServerInfo, g_TeamServerInfo.ServerInfo.TcpPort);
				Write16(pServerInfo, g_TeamServerInfo.ServerInfo.UdpPort);
				Write16(pServerInfo, g_TeamServerInfo.ServerInfo.MaxPlayerNum);
				Write16(pServerInfo, g_TeamServerInfo.ServerInfo.CurrentPlayerNum);
				Write32(pServerInfo, g_TeamServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
				Write8(pServerInfo, g_TeamServerInfo.ServerInfo.IsDisabledLogin);
				Write8(pServerInfo, g_TeamServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
				len = pServerInfo - pTeamServerInfo;
				Set16(pTeamServerInfo, (WORD)len);
				pTeamServerInfo += len;
			}
			Write16(pTeamServerInfo, g_TeamServerInfo.LocationID);

			len = pTeamServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

		Write8(p, 1); // DanceServerCount
		{			  // DanceServerInfo
			BYTE *pDanceServerInfo = p;
			Write16(pDanceServerInfo, 0); // len

			{
				BYTE *pServerInfo = pDanceServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_DanceServerInfo.ServerInfo.ServerID);
				Write32(pServerInfo, g_DanceServerInfo.ServerInfo.ServerIP);
				Write16(pServerInfo, g_DanceServerInfo.ServerInfo.TcpPort);
				Write16(pServerInfo, g_DanceServerInfo.ServerInfo.UdpPort);
				Write16(pServerInfo, g_DanceServerInfo.ServerInfo.MaxPlayerNum);
				Write16(pServerInfo, g_DanceServerInfo.ServerInfo.CurrentPlayerNum);
				Write32(pServerInfo, g_DanceServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
				Write8(pServerInfo, g_DanceServerInfo.ServerInfo.IsDisabledLogin);
				Write8(pServerInfo, g_DanceServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
				len = pServerInfo - pDanceServerInfo;
				Set16(pDanceServerInfo, (WORD)len);
				pDanceServerInfo += len;
			}
			Write16(pDanceServerInfo, g_DanceServerInfo.LocationID);

			len = pDanceServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

		Write8(p, 1); // SlideServerCount
		{			  // SlideServerInfo
			BYTE *pSlideServerInfo = p;
			Write16(pSlideServerInfo, 0); // len

			{
				BYTE *pServerInfo = pSlideServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_SlideServerInfo.ServerInfo.ServerID);
				Write32(pServerInfo, g_SlideServerInfo.ServerInfo.ServerIP);
				Write16(pServerInfo, g_SlideServerInfo.ServerInfo.TcpPort);
				Write16(pServerInfo, g_SlideServerInfo.ServerInfo.UdpPort);
				Write16(pServerInfo, g_SlideServerInfo.ServerInfo.MaxPlayerNum);
				Write16(pServerInfo, g_SlideServerInfo.ServerInfo.CurrentPlayerNum);
				Write32(pServerInfo, g_SlideServerInfo.ServerInfo.RandomKey);
#ifndef ZingSpeed
				Write8(pServerInfo, g_SlideServerInfo.ServerInfo.IsDisabledLogin);
				Write8(pServerInfo, g_SlideServerInfo.ServerInfo.MultiIPInfo ? 1 : 0); // HasMultiIPInfo
#endif
				len = pServerInfo - pSlideServerInfo;
				Set16(pSlideServerInfo, (WORD)len);
				pSlideServerInfo += len;
			}
			Write16(pSlideServerInfo, g_SlideServerInfo.LocationID);

			len = pSlideServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 1); // FBServerCount
		for (size_t i = 0; i < 1; i++)
		{ // FBServerInfo
			BYTE *pFBServerInfo = p;
			Write16(pFBServerInfo, 0); // len

			{ // ServerInfo
				BYTE *pServerInfo = pFBServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_FBServerInfo.ServerInfo.ServerID);		  // ServerID
				Write32(pServerInfo, g_FBServerInfo.ServerInfo.ServerIP);		  // ServerIP
				Write16(pServerInfo, g_FBServerInfo.ServerInfo.TcpPort);		  // TcpPort
				Write16(pServerInfo, g_FBServerInfo.ServerInfo.UdpPort);		  // UdpPort
				Write16(pServerInfo, g_FBServerInfo.ServerInfo.MaxPlayerNum);	  // MaxPlayerNum
				Write16(pServerInfo, g_FBServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
				Write32(pServerInfo, g_FBServerInfo.ServerInfo.RandomKey);		  // RandomKey
				Write8(pServerInfo, g_FBServerInfo.ServerInfo.IsDisabledLogin);	  // IsDisabledLogin
				Write8(pServerInfo, 0);											  // HasMultiIPInfo

				len = pServerInfo - pFBServerInfo;
				Set16(pFBServerInfo, (WORD)len);
				pFBServerInfo += len;
			}
			Write16(pFBServerInfo, g_FBServerInfo.LocationID); // LocationID
			Write32(pFBServerInfo, 0);						   // ServerRoomType

			len = pFBServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(pInfo, 1); // WlMatchSvrCount
		for (size_t i = 0; i < 1; i++)
		{ // WlMatchSvrInfo
			BYTE *pWlMatchSvrInfo = p;
			Write16(pWlMatchSvrInfo, 0); // len

			{ // ServerInfo
				BYTE *pServerInfo = pWlMatchSvrInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_WlMatchSvrInfo.ServerInfo.ServerID);			// ServerID
				Write32(pServerInfo, g_WlMatchSvrInfo.ServerInfo.ServerIP);			// ServerIP
				Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.TcpPort);			// TcpPort
				Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.UdpPort);			// UdpPort
				Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.MaxPlayerNum);		// MaxPlayerNum
				Write16(pServerInfo, g_WlMatchSvrInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
				Write32(pServerInfo, g_WlMatchSvrInfo.ServerInfo.RandomKey);		// RandomKey
				Write8(pServerInfo, g_WlMatchSvrInfo.ServerInfo.IsDisabledLogin);	// IsDisabledLogin
				Write8(pServerInfo, 0);												// HasMultiIPInfo
				len = pServerInfo - pWlMatchSvrInfo;
				Set16(pWlMatchSvrInfo, (WORD)len);
				pWlMatchSvrInfo += len;
			}
			Write16(pWlMatchSvrInfo, g_WlMatchSvrInfo.LocationID); // LocationID
			Write32(pWlMatchSvrInfo, 0);						   // ServerRoomType

			len = pWlMatchSvrInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 1); // SkateServerCount
		for (size_t i = 0; i < 1; i++)
		{ // SkateServerInfo
			BYTE *pSkateServerInfo = p;
			Write16(pSkateServerInfo, 0); // len

			{ // CommServerInfo
				BYTE *pCommServerInfo = pSkateServerInfo;
				Write16(pCommServerInfo, 0); // len

				{ // ServerInfo
					BYTE *pServerInfo = pCommServerInfo;
					Write16(pServerInfo, 0); // len

					Write32(pServerInfo, g_SkatingServerInfo.ServerInfo.ServerID);		   // ServerID
					Write32(pServerInfo, g_SkatingServerInfo.ServerInfo.ServerIP);		   // ServerIP
					Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.TcpPort);		   // TcpPort
					Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.UdpPort);		   // UdpPort
					Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.MaxPlayerNum);	   // MaxPlayerNum
					Write16(pServerInfo, g_SkatingServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
					Write32(pServerInfo, g_SkatingServerInfo.ServerInfo.RandomKey);		   // RandomKey
					Write8(pServerInfo, g_SkatingServerInfo.ServerInfo.IsDisabledLogin);   // IsDisabledLogin
					Write8(pServerInfo, 0);												   // HasMultiIPInfo

					len = pServerInfo - pCommServerInfo;
					Set16(pCommServerInfo, (WORD)len);
					pCommServerInfo += len;
				}
				Write16(pCommServerInfo, g_SkatingServerInfo.LocationID); // LocationID
				Write32(pCommServerInfo, 0);							  // ServerRoomType

				len = pCommServerInfo - pSkateServerInfo;
				Set16(pSkateServerInfo, (WORD)len);
				pSkateServerInfo += len;
			}
			Write32(pSkateServerInfo, 0); // ChannelId

			len = pSkateServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 1); // MRServerCount
		for (size_t i = 0; i < 1; i++)
		{ // MRServerInfo
			BYTE *pMRServerInfo = p;
			Write16(pMRServerInfo, 0); // len

			{ // ServerInfo
				BYTE *pServerInfo = pMRServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_MRServerInfo.ServerInfo.ServerID);		  // ServerID
				Write32(pServerInfo, g_MRServerInfo.ServerInfo.ServerIP);		  // ServerIP
				Write16(pServerInfo, g_MRServerInfo.ServerInfo.TcpPort);		  // TcpPort
				Write16(pServerInfo, g_MRServerInfo.ServerInfo.UdpPort);		  // UdpPort
				Write16(pServerInfo, g_MRServerInfo.ServerInfo.MaxPlayerNum);	  // MaxPlayerNum
				Write16(pServerInfo, g_MRServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
				Write32(pServerInfo, g_MRServerInfo.ServerInfo.RandomKey);		  // RandomKey
				Write8(pServerInfo, g_MRServerInfo.ServerInfo.IsDisabledLogin);	  // IsDisabledLogin
				Write8(pServerInfo, 0);											  // HasMultiIPInfo

				len = pServerInfo - pMRServerInfo;
				Set16(pMRServerInfo, (WORD)len);
				pMRServerInfo += len;
			}
			Write16(pMRServerInfo, g_MRServerInfo.LocationID); // LocationID
			Write32(pMRServerInfo, 0);						   // ServerRoomType

			len = pMRServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(pInfo, 1); // NpcServerCount
		for (size_t i = 0; i < 1; i++)
		{ // NpcServerInfo
			BYTE *pNpcServerInfo = p;
			Write16(pNpcServerInfo, 0); // len

			{ // ServerInfo
				BYTE *pServerInfo = pNpcServerInfo;
				Write16(pServerInfo, 0); // len

				Write32(pServerInfo, g_NpcServerInfo.ServerInfo.ServerID);		   // ServerID
				Write32(pServerInfo, g_NpcServerInfo.ServerInfo.ServerIP);		   // ServerIP
				Write16(pServerInfo, g_NpcServerInfo.ServerInfo.TcpPort);		   // TcpPort
				Write16(pServerInfo, g_NpcServerInfo.ServerInfo.UdpPort);		   // UdpPort
				Write16(pServerInfo, g_NpcServerInfo.ServerInfo.MaxPlayerNum);	   // MaxPlayerNum
				Write16(pServerInfo, g_NpcServerInfo.ServerInfo.CurrentPlayerNum); // CurrentPlayerNum
				Write32(pServerInfo, g_NpcServerInfo.ServerInfo.RandomKey);		   // RandomKey
				Write8(pServerInfo, g_NpcServerInfo.ServerInfo.IsDisabledLogin);   // IsDisabledLogin
				Write8(pServerInfo, 0);											   // HasMultiIPInfo

				len = pServerInfo - pNpcServerInfo;
				Set16(pNpcServerInfo, (WORD)len);
				pNpcServerInfo += len;
			}
			Write16(pNpcServerInfo, g_NpcServerInfo.LocationID); // LocationID
			Write32(pNpcServerInfo, 0);							 // ServerRoomType

			len = pNpcServerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

		len = pInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 1400, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}