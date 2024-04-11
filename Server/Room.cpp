#include "Room.h"
#include "Map.h"
#include "Database.h"
#include <stdio.h>
#include <time.h>
#include <map>
#include <mutex>
#include <random>
#include "Item.h"
#include "GameServer.h"
#include "yaml-cpp/yaml.h"
#include "sqlite/sqlite3.h"
#include "z_debug.h"
UINT GlobalBorderRoomID = 0;
BYTE chewangkaoyan[100] = { 0 };
BYTE BorderOnePlayerInfo[1305];
BYTE shumiaoyunshu[100] = { 0 };
BYTE baofengtiaozhan[2048] = { 0 };
HANDLE TimerQueue = CreateTimerQueue();
void WINAPI OnDanceTimer(RoomNode *Room);
extern sqlite3 *PlayerDB;
std::map<long, RoomNode *> Rooms;
std::mutex Mutex;
long SeqRoomID = 0;
RoomNode* NewRoomSingle()
{
	RoomNode *Room = new RoomNode;
	Room->ID = 9999;
	memset(Room->Name, 0, sizeof(Room->Name));
	Room->RoomOwnerID = 0;
	Room->ServerID = 0;
	Room->BaseGameMode = 0;
	Room->SubGameMode = 0;
	Room->SeatNum = 0;
	Room->startStatus = 1;
	Room->CurrentPlayerNum = 0;
	Room->Flag = 0;
	memset(Room->Password, 0, sizeof(Room->Password));
	for (size_t i = 0; i < 6; i++)
	{
		Room->Player[i] = NULL;
	}
	Room->Timer = NULL;
	Room->Callback = NULL;

	Room->MapID = 0;
	Room->STDynamicInfo.Season = -1;
	Room->STDynamicInfo.Timeslot = -1;
	Room->STDynamicInfo.Weather = -1;

	Room->SceneID = 0;
	Room->MusicID = 0;
	return Room;
}
RoomNode* NewRoom()
{
	RoomNode *Room = new RoomNode;
	Room->ID = InterlockedIncrement(&SeqRoomID) + 1000;
	memset(Room->Name, 0, sizeof(Room->Name));
	Room->RoomOwnerID = 0;
	Room->ServerID = 0;
	Room->startStatus = 1;
	Room->BaseGameMode = 0;
	Room->SubGameMode = 0;
	Room->SeatNum = 0;
	Room->CurrentPlayerNum = 0;
	Room->Flag = 0;
	memset(Room->Password, 0, sizeof(Room->Password));
	for (size_t i = 0; i < 6; i++)
	{
		Room->Player[i] = NULL;
	}
	for (size_t i = 0; i < 6; i++)
	{
		Room->NPCPlayer[i] = NULL;
	}
	Room->Timer = NULL;
	Room->Callback = NULL;

	Room->MapID = 0;
	Room->STDynamicInfo.Season = -1;
	Room->STDynamicInfo.Timeslot = -1;
	Room->STDynamicInfo.Weather = -1;

	Room->SceneID = 0;
	Room->MusicID = 0;

	Mutex.lock();
	Rooms[Room->ID] = Room;
	Mutex.unlock();
	return Room;
}
RoomNode* GetRoom(long RoomID)
{
	RoomNode* Room = NULL;
	Mutex.lock();
	auto iter = Rooms.find(RoomID);
	if (iter != Rooms.end())
	{
		Room = iter->second;
	}
	/*else
	{
		printf("无效的房间ID:%d\n", RoomID);
	}*/
	Mutex.unlock();
	return Room;
}
void PrintRoom()
{
	std::map<long, RoomNode *>::iterator iter = Rooms.begin();
	while (iter != Rooms.end())
	{
		RoomNode *Room = iter->second;
		if (Room)
		{
			Z_INFO("房间号:%d, ", Room->ID);
			Z_INFO("基础游戏模式:%d, ", Room->BaseGameMode);
			Z_INFO("子游戏模式:%d, ", Room->SubGameMode);
			Z_INFO("服务ID:%d, ", Room->ServerID);
			Z_INFO("房主ID:%d, ", Room->RoomOwnerID);
			Z_INFO("房间名字:%s\n", Room->Name);
			Z_INFO("标志:%d, ", Room->Flag);
			Z_INFO("密码:%s\n", Room->Password);
			for (char i = 0; i < 6; i++)
			{
				ClientNode *RoomClient = Room->Player[i];
				if (RoomClient)
				{
					Z_INFO("\t玩家ID:%d, 座位ID:%d\n", RoomClient->ConnID, i);
				}
			}
		}
		iter++;
	}
}

VOID WINAPI OnTimer(PVOID p, BOOLEAN b)
{
	RoomNode *Room = (RoomNode *)p;
	if (DeleteTimerQueueTimer(TimerQueue, Room->Timer, NULL))
	{
		Room->Timer = NULL;
	}
	Room->Timer = NULL;
	Room->Callback(Room);
}

void CreateRoomTimer(RoomNode *Room, int Timeout, RoomOnTimer Callback)
{
	Room->Callback = Callback;
	CreateTimerQueueTimer(&Room->Timer, TimerQueue, OnTimer, Room, Timeout, 0, WT_EXECUTEINTIMERTHREAD);
}

void RequestGetRoomList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{

	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT StartRoomIdx = Read16(Body);
	USHORT GetRoomNum = Read16(Body);
	USHORT BaseGameMode = Read16(Body);
	USHORT SubGameMode = Read16(Body);
	BYTE FBLev = Read8(Body);
	BYTE FBSeason = Read8(Body);
	BYTE RoomFlag = Read8(Body); // 0传统 1 3d

	ResponseGetRoomList(Client, BaseGameMode, SubGameMode, StartRoomIdx, GetRoomNum, RoomFlag);
}
void ResponseGetRoomList(ClientNode *Client, USHORT BaseGameMode, USHORT SubGameMode, USHORT StartRoomIdx, USHORT GetRoomNum, BYTE RoomFlag)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write16(p, (WORD)Rooms.size()); // CurTotalRoomNum
	Write16(p, StartRoomIdx);		// StartIdx

	BYTE *pRoomNum = p;
	Write16(p, GetRoomNum); // RoomNum
	{
		int RoomNum = 0;
		Mutex.lock();
		std::map<long, RoomNode *>::iterator iter;
		for (iter = Rooms.begin(); iter != Rooms.end(); iter++)
		{
			RoomNode *Room = iter->second;
			UINT CurrentPlayerNum = 0;
			if (BaseGameMode > 0 && SubGameMode > 0)
			{ // 根据模式查询
				if (!(Room->BaseGameMode == BaseGameMode && Room->SubGameMode == SubGameMode))
				{
					continue;
				}
			}
			if (Room->ID == 9999)
			{ // 不查询9999号房间
				continue;
			}
			if (Room->ShowFlag == 1) {//匹配模式列表不显示
				continue;
			}
			if (Room->ServerID != Client->ServerID)
			{ // 只查询当前频道房间
				continue;
			}

			if (RoomNum == GetRoomNum)
			{ // 只查出一页的房间 减少多余数据
				break;
			}
			if (Room)
			{ // 检测房间内是否有人

				for (size_t i = 0; i < 6; i++)
				{
					ClientNode *RoomClient = Room->Player[i];
					if (RoomClient)
					{
						if (RoomClient->RoomID == Room->ID)
						{
							CurrentPlayerNum++;
						}
						else
						{
							Room->Player[i] = NULL;
						}
					}
				}
				if (CurrentPlayerNum == 0)
				{
					continue;
				}
			}

			RoomNum++;
			if (RoomNum < StartRoomIdx)
			{ // 根据索引查询房间
				continue;
			}

			{ // Rooms
				BYTE *pRooms = p;
				Write16(pRooms, 0); // len

				Write16(pRooms, Room->ID);				// RoomID
				Write16(pRooms, Room->BaseGameMode);	// BaseGameMode
				Write16(pRooms, Room->SubGameMode);		// SubGameMode
				Write8(pRooms, Room->CurrentPlayerNum); // CurrentPlayerNum
				Write8(pRooms, Room->SeatNum);			// TotalSeatNum
				Write8(pRooms, Room->startStatus);		// Status
				Write32(pRooms, Room->MapID);			// MapID
#ifdef ZingSpeed
				Write8(pRooms, (BYTE)Room->Flag); // Flag
#else
				Write16(pRooms, Room->Flag); // Flag
#endif

				memcpy(pRooms, Room->Name, MaxRoomName); // RoomName
				pRooms += MaxRoomName;

				Write8(pRooms, 0); // HasFBInfo
				for (size_t i = 0; i < 0; i++)
				{ // FBInfo
					BYTE *pFBInfo = pRooms;
					Write16(pFBInfo, 0); // len

					Write8(pFBInfo, 0);	 // AvailableLev
					Write8(pFBInfo, 0);	 // Lev
					Write8(pFBInfo, 0);	 // SubLev
					Write32(pFBInfo, 0); // BossID
					Write8(pFBInfo, 0);	 // FBSeason

					len = pFBInfo - pRooms;
					Set16(pRooms, (WORD)len);
					pRooms += len;
				}

				Write8(pRooms, 10);					  // SpeFlag
				Write32(pRooms, Room->ID);			  // RoomNo
				Write16(pRooms, 0);					  // SvrId
				Write16(pRooms, Room->SceneID);		  // SceneID
				Write8(pRooms, 0);					  // CurrentOBNum
				Write8(pRooms, Room->PresetOBNum);	  // TotalOBNum
				Write8(pRooms, 0);					  // SpecialActivityStatus
				Write8(pRooms, Room->AllowAutoStart); // AllowAutoStart
				Write32(pRooms, 0);					  // Gender
				Write32(pRooms, 0);					  // LocaleCode
				Write8(pRooms, 0);					  // TalkRoomBGID
				Write8(pRooms, 0);					  // SpecialActivityIDNum
				Write8(pRooms, 0);					  // SpecialActivityInfoNum
				for (size_t i = 0; i < 0; i++)
				{ // SpecialActivityInfo
					BYTE *pSpecialActivityInfo = pRooms;
					Write16(pSpecialActivityInfo, 0); // len

					Write8(pSpecialActivityInfo, 0); // ProfessionLicenseInfoNum
					for (size_t i = 0; i < 0; i++)
					{ // ProfessionLicenseInfo
						BYTE *pProfessionLicenseInfo = pSpecialActivityInfo;
						Write16(pProfessionLicenseInfo, 0); // len

						Write8(pProfessionLicenseInfo, 6); // BigLevel
						Write8(pProfessionLicenseInfo, 5); // SmallLevel

						len = pProfessionLicenseInfo - pSpecialActivityInfo;
						Set16(pSpecialActivityInfo, (WORD)len);
						pSpecialActivityInfo += len;
					}

					len = pSpecialActivityInfo - pRooms;
					Set16(pRooms, (WORD)len);
					pRooms += len;
				}
				Write8(pRooms, 0);	// ValueNum
				Write32(pRooms, 0); // EnterRoomValue[]
				Write8(pRooms, 0);	// ParaNum
				Write32(pRooms, 0); // ParaList[]
				Write32(pRooms, 0); // GSvrRoomNo
				Write8(pRooms, 0);	// OwnerName[]

				len = pRooms - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
		Mutex.unlock();

		Set16(pRoomNum, RoomNum);
	}

	Write8(p, 0); // ReasonLen

	Write16(p, BaseGameMode); // BaseGameMode
	Write16(p, SubGameMode);  // SubGameMode
	Write8(p, 0);			  // FBAvailableLev
	Write8(p, 0);			  // FBLev
	Write8(p, 0);			  // FBSeason

	Write16(p, 0); // ChannelStatus
	Write8(p, 0);  // RoomFlag

	len = p - buf;
	SendToClient(Client, 102, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Response);
}
void RequestCreateRoomMatch(ClientNode *Client)
{
	UINT Uin = Client->Uin;
	UINT Time = 0;
	// 46疑似龙晶
	USHORT BaseGameMode = Client->BaseGameMode;
	USHORT SubGameMode = Client->SubGameMode;
	USHORT Flag = 0;
	const char *RoomName = "pp";

	BYTE SeatNum = 6;

	// char* Password;
	/*memcpy(Password, Body, 16);
	Body += 16;*/

	BYTE IsNewerOpen = 1;
	BYTE FBSeason = 5;
	BYTE Contex = 0;
	BYTE FBLev = 0;

	BYTE SpeFlag = 10;
	BYTE MoreInfoNum = 0;
	for (BYTE i = 0; i < 0; i++)
	{
		// m_astMoreInfos[].m_iTaskID
		// Body += Get16(Body);
	}

	USHORT CreateRoomType = 0;
	USHORT ServerMatchType = 1;
	USHORT TeamMatchType = 0;
	BYTE ChatRoomType = 0;
	USHORT SceneID = 0;
	BYTE PresetOBNum = 0;
	BYTE AllowAutoStart = 1;
	BYTE TalkRoomBGID = 0;
	BYTE ValueNum = 0;
	for (BYTE i = 0; i < ValueNum; i++)
	{
		// m_aiEnterRoomValue[]
		// Read32(Body);
	}

	RoomNode *Room = NewRoom();
	Room->BaseGameMode = BaseGameMode;
	Room->SubGameMode = SubGameMode;
	strcpy_s(Room->Name, RoomName);
	Room->ServerID = Client->ServerID;
	Room->RoomOwnerID = Client->ConnID;
	Client->RoomID = Room->ID;
	Room->SeatNum = SeatNum;
	Room->CurrentPlayerNum = 1;
	Room->Flag = Flag;
	// memcpy(Room->Password, Password, 16);
	Room->Player[0] = Client;

	Room->SceneID = SceneID;
	Room->CreateRoomType = CreateRoomType;
	Room->ServerMatchType = ServerMatchType;
	Room->TeamMatchType = TeamMatchType;
	Room->ChatRoomType = ChatRoomType;

	Room->PresetOBNum = PresetOBNum;
	Room->AllowAutoStart = AllowAutoStart;
	Room->TalkRoomBGID = TalkRoomBGID;

	ResponseCreateRoom(Client, Room);
}
void RequestCreateRoom(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT BaseGameMode = Read16(Body);
	USHORT SubGameMode = Read16(Body);
	USHORT Flag = Read16(Body);
	Flag += 4;//皇族
	Flag += 8;//车队贵族
	Flag += 256;//魅力大人
	Flag += 128;//情侣房间
	char *RoomName = (char *)Body;
	Body += MaxRoomName;

	BYTE SeatNum = Read8(Body);

	char *Password;
	Password = ReadString(Body, 16);
	/*memcpy(Password, Body, 16);
	Body += 16;*/

	BYTE IsNewerOpen = Read8(Body);
	BYTE FBSeason = Read8(Body);
	BYTE Contex = Read8(Body);
	BYTE FBLev = Read8(Body);

	BYTE SpeFlag = Read8(Body);
	BYTE MoreInfoNum = Read8(Body);
	for (BYTE i = 0; i < MoreInfoNum; i++)
	{
		// m_astMoreInfos[].m_iTaskID
		Body += Get16(Body);
	}

	USHORT CreateRoomType = Read16(Body);
	USHORT ServerMatchType = Read16(Body);
	USHORT TeamMatchType = Read16(Body);
	BYTE ChatRoomType = Read8(Body);
	USHORT SceneID = Read16(Body);
	BYTE PresetOBNum = Read8(Body);
	BYTE AllowAutoStart = Read8(Body);
	BYTE TalkRoomBGID = Read8(Body);
	BYTE ValueNum = Read8(Body);
	for (BYTE i = 0; i < ValueNum; i++)
	{
		// m_aiEnterRoomValue[]
		Read32(Body);
	}
	RoomNode *Room;
	Room = NewRoom();

	Room->BaseGameMode = BaseGameMode;
	Room->SubGameMode = SubGameMode;
	strcpy_s(Room->Name, RoomName);
	Room->ServerID = Client->ServerID;
	Room->RoomOwnerID = Client->ConnID;
	Client->RoomID = Room->ID;
	Room->SeatNum = SeatNum;
	Room->CurrentPlayerNum = 1;
	Room->Flag = Flag;
	memcpy(Room->Password, Password, 16);
	Room->Player[0] = Client;

	Room->SceneID = SceneID;
	Room->CreateRoomType = CreateRoomType;
	Room->ServerMatchType = ServerMatchType;
	Room->TeamMatchType = TeamMatchType;
	Room->ChatRoomType = ChatRoomType;

	Room->PresetOBNum = PresetOBNum;
	Room->AllowAutoStart = AllowAutoStart;
	Room->TalkRoomBGID = TalkRoomBGID;
	if (BaseGameMode == 0x2e)//龙晶大闯关
	{
		const char* buf1 = "\x00\x00\x00\x01\x00\x2e\x00\x01\x06\x00\xb4\xf3\xbc\xd2\xd2\xbb\xc6\xf0\xcc\xf4\xd5\xbd\xbc\xab\xcf\xde\xa3\xa1\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x64\x70\x64\x1c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x08\x47\x7e\x1d\x00\x00\x00\x00\x01\x00\x11\x00\x00\x00\x00\x00\x04\x01\x01\x00\x08\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x0f\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x0f\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x17\x31\x2d\x31\x38\x37\x32\x34\x37\x33\x37\x36\x37\x2d\x31\x36\x38\x35\x30\x38\x37\x32\x36\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x72\x69";
		SendToClient(Client, 103, (BYTE*)buf1, 207, Client->ServerID, FE_GAMESVRD, Client->ConnID, Response);
	}
	else {
		ResponseCreateRoom(Client, Room);
		if (Room->SceneID == 400)
		{
			RequestEnterOutdoor2(Client, Room->ID, 4);
		}
		if (Room->SceneID == 401)
		{
			RequestEnterOutdoor2(Client, Room->ID, 14);
		}
	}
}
void ResponseCreateRoom(ClientNode *Client, RoomNode *Room)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write16(p, Room->ID);			// RoomID
	Write16(p, Room->BaseGameMode); // BaseGameMode
	Write16(p, Room->SubGameMode);	// SubGameMode
	Write16(p, Room->Flag);			// Flag

	memcpy(p, Room->Name, MaxRoomName); // RoomName
	p += MaxRoomName;

	Write8(p, Room->SeatNum); // SeatNum

	Write8(p, 0); // SeatID
	Write8(p, 0); // TeamID

	Write8(p, 0); // ReasonLen

	Write8(p, 0); // LoverRaceOrPro

	Write32(p, Room->MapID);		  // MapID
	Write8(p, 0);					  // DebutOrX5
	Write32(p, (DWORD)time(nullptr)); // ServerTime
	Write32(p, 0);					  // PreKnowMapID
	Write32(p, 0);					  // PreKnownMusicID

	Write8(p, 0); // HasFBInfo
	/*
m_astFBInfo[%d].m_bAvailableLev
m_astFBInfo[%d].m_bLev
m_astFBInfo[%d].m_bSubLev
m_astFBInfo[%d].m_iBossID
m_astFBInfo[%d].m_bFBSeason
	*/

	Write32(p, 0);		  // HasFBInfoGuildScoreInNextGame
	Write8(p, 0);		  // HasFBInfoContex
	Write16(p, 0);		  // GuildScoreAlreadyAward
	Write16(p, 0);		  // GuildScoreTotalAward
	Write8(p, 10);		  // HasFBInfoSpeFlag
	Write32(p, Room->ID); // RoomNo

	Write8(p, 0); // MoreInfoNum
	/*
m_astMoreInfos[%d].m_iTaskID
	*/

	Write16(p, 0); // RaceCardNums
	/*
m_astRaceCardAddValueInfo[%d].m_uiRaceCardID
m_astRaceCardAddValueInfo[%d].m_uiAddValue
	*/
	Write16(p, Room->CreateRoomType);  // CreateRoomType
	Write16(p, Room->ServerMatchType); // ServerMatchType
	Write16(p, Room->TeamMatchType);   // TeamMatchType
	Write8(p, Room->ChatRoomType);	   // ChatRoomType
	Write16(p, Room->SceneID);		   // SceneID

	Write8(p, Room->PresetOBNum);	 // PresetOBNum
	Write8(p, Room->AllowAutoStart); // AllowAutoStart
	Write8(p, Room->TalkRoomBGID);	 // TalkRoomBGID
	Write16(p, 0);					 // Role

	Write8(p, 0); // ParaNum
	// ParaList[]

	Write8(p, 0); // ValueNum
	// EnterRoomValue[]

	Write32(p, 0); // PlayerEquipKartID
	Write32(p, 0); // RankedMatchTestDriveKartID
	Write8(p, 0); // GVoiceRoomNameLen
	Write8(p, 0); // GVoiceReasonLen
	Write8(p, 0); // TDCExcluItemNum
	// TDCExclusiveItemID[]

	Write8(p, 0);  // TestKartGameType
	Write32(p, 0); // GameTestKartID

	Write8(p, 0); // HasStarsFightingInfo
	/*
m_stStarsFightingInfo[%d].m_iStar
m_stStarsFightingInfo[%d].m_iStarVal
m_stStarsFightingInfo[%d].m_iStarsFightingStarLevel
	*/

	Write8(p, 0); // HasYuLeJiaNianHuaInfo
	/*
m_stYuLeJiaNianHuaInfo[%d].m_uiMode
m_stYuLeJiaNianHuaInfo[%d].m_uiScore
m_stYuLeJiaNianHuaInfo[%d].m_uchRankRangNum
m_stYuLeJiaNianHuaInfo[%d].m_auiRankRange[%d]
m_stYuLeJiaNianHuaInfo[%d].m_uchRankScoreNum
m_stYuLeJiaNianHuaInfo[%d].m_auiRankScore[%d]
m_stYuLeJiaNianHuaInfo[%d].m_uchInRecommend
m_stYuLeJiaNianHuaInfo[%d].m_uchInDoubleScore
	*/

	len = p - buf;
	SendToClient(Client, 103, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Response);
}

void RequestEnterRoomMatch(ClientNode* Client)
{
	Client->IsReady = 1;
	UINT Uin = Client->Uin;
	UINT Time = 0;

	USHORT RoomID = Client->RoomID;

	// Body += 16; //Password
	char pwd[16] = { 0 };
	BYTE InviteType = 0;
	UINT InviterUin = 0;

	RoomNode* Room = GetRoom(RoomID);
	if (!Room)
	{
		ResponseStartGame(Client, "对不起,您要加入的房间已经不存在,请选择加入其他房间!");
		return;
	}
	if (!Room->startStatus)
	{
		ResponseStartGame(Client, "对不起,您要加入的房间正在游戏中!");
		return;
	}
	if (Room->CurrentPlayerNum >= Room->SeatNum)
	{
		ResponseStartGame(Client, "你要进入的房间已经满人了");
		return;
	}
	int cmppw = strcmp(pwd, Room->Password);
	if (cmppw != 0 && Room->Flag - 12 == 1&&Client->Uin!=10001)
	{
		ResponseStartGame(Client, "对不起,房间密码输入错误!");
		return;
	}
	LeaveRoom(Client, 0);

	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient == NULL)
		{
			Client->RoomID = RoomID;
			ResponseEnterRoom(Client, Room, i);
			Room->Player[i] = Client;
			Room->CurrentPlayerNum++;
			// 通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode* RoomClient = Room->Player[i2];

				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterRoom(RoomClient, Room, Client, i);
					NotifyEnterRoom(Client, Room, RoomClient, i2);
				}
			}

			break;
		}
	}
}
void RequestEnterRoom(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	Client->IsReady = 1;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT RoomID = Read16(Body);

	//Body += 16; //Password
	char pwd[16] = { 0 };
	memcpy(pwd, Body, 16);
	BYTE InviteType = Read8(Body);
	UINT InviterUin = Read32(Body);


	RoomNode* Room = GetRoom(RoomID);
	if (!Room)
	{
		ResponseStartGame(Client, "对不起,您要加入的房间已经不存在,请选择加入其他房间!");
		return;
	}
	if (!Room->startStatus) {
		ResponseStartGame(Client, "对不起,您要加入的房间正在游戏中!");
		return;
	}
	if (Room->CurrentPlayerNum >= Room->SeatNum) {
		ResponseStartGame(Client, "你要进入的房间已经满人了");
		return;
	}
	int cmppw = strcmp(pwd, Room->Password);
	if (cmppw != 0 && Room->Flag - 4 - 8 - 256 - 128 == 1)
	{
		ResponseStartGame(Client, "对不起,房间密码输入错误!");
		return;
	}
	LeaveRoom(Client, 0);

	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient == NULL)
		{
			Client->RoomID = RoomID;
			ResponseEnterRoom(Client, Room, i);
			Room->Player[i] = Client;
			Room->CurrentPlayerNum++;
			//通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode* RoomClient = Room->Player[i2];

				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterRoom(RoomClient, Room, Client, i);
					NotifyEnterRoom(Client, Room, RoomClient, i2);
				}

			}

			break;
		}
	}

}
void RequestQuestEnterRoom(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	INT roomNum = Rooms.size();
	if (roomNum == 0)
	{
		ResponseStartGame(Client, "暂时没有房间");
	}
	INT roomId[100]; // 最多房间随机数
	int count = -1;
	int randomID = 0;
	std::map<long, RoomNode*>::iterator iter;
	for (iter = Rooms.begin(); iter != Rooms.end(); iter++)
	{
		RoomNode* Room = iter->second;
		roomId[count + 1] = Room->ID;
		count++;
	}
	if (count > 0)
	{
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<int> dis(0, count);
		randomID = dis(eng);
	}

	Client->IsReady = 1;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	USHORT RoomID = roomId[randomID];

	Body += 16; // Password

	BYTE InviteType = Read8(Body);
	UINT InviterUin = Read32(Body);

	RoomNode* Room = GetRoom(RoomID);
	if (!Room)
	{
		return;
	}
	LeaveRoom(Client, 0);

	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient == NULL)
		{
			Client->RoomID = RoomID;
			ResponseEnterRoom(Client, Room, i);
			Room->Player[i] = Client;
			Room->CurrentPlayerNum++;

			// 通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode* RoomClient = Room->Player[i2];
				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterRoom(RoomClient, Room, Client, i);
					//NotifyEnterRoom(Client, Room, RoomClient, i2);
				}
			}
			break;
		}
	}
}
void ResponseEnterRoom(ClientNode* Client, RoomNode* Room, char SeatID)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write16(p, 0); //ResultID

	Write16(p, Room->ID); //RoomID
	Write16(p, Room->BaseGameMode); //BaseGameMode
	Write16(p, Room->SubGameMode); //SubGameMode


	memcpy(p, Room->Name, MaxRoomName); //RoomName
	p += MaxRoomName;

	Write8(p, Room->SeatNum); //SeatNum
#ifdef ZingSpeed
	Write8(p, (BYTE)Room->Flag); //Flag
#else
	Write16(p, Room->Flag); //Flag
#endif

	Write8(p, SeatID); //SeatID
	Write8(p, 0); //TeamID
	Write32(p, Room->MapID); //MapID
	Write16(p, Room->RoomOwnerID); //RoomOwnerID

	//SeatStatus[6]
	for (size_t i = 0; i < 6; i++)
	{
		Write8(p, 2);
	}

	BYTE* pPlayerCount = p;
	Write8(p, 0); //PlayerCount
	int PlayerCount = 0;
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			PlayerCount++;

			WriteVisbleInfo(p, RoomClient->Uin, i, RoomClient->ConnID); //PlayerRoomVisibleInfo
		}
	}
	Set8(pPlayerCount, PlayerCount);

	Write8(p, 0); //ReasonLen
	Write8(p, 0); //PlayerCount2
	/*
m_astPlayerRelationInfo[].m_uiSrcUin
m_astPlayerRelationInfo[].m_iRelationFlag
m_astPlayerRelationInfo[].m_uiRelationUin
m_astPlayerRelationInfo[].m_szRelationNickname[]:
m_astPlayerRelationInfo[].m_u32EngageTime
m_astPlayerRelationInfo[].m_uiNextCanBookingTimeLimit
m_astPlayerRelationInfo[].m_uiBeginHoneyMoonTime
m_astPlayerRelationInfo[].m_uiEndHoneyMoonTime
m_astPlayerRelationInfo[].m_bEngageFlag

	for (size_t i = 0; i < n; i++)
	{ //PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len

		Write32(pPlayerRelationInfo, 0); //SrcUin
		Write32(pPlayerRelationInfo, 0); //RelationFlag
		Write32(pPlayerRelationInfo, 0); //RelationUin
		WriteString(pPlayerRelationInfo, 0); //RelationNickName[]
		Write32(pPlayerRelationInfo, 0); //EngageTime
		Write32(pPlayerRelationInfo, 0); //NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); //BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); //EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0); //EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	//SSSSSSSSSSSSSSSSSSS
	size_t Karti = 0;
	BYTE* pKartNum = p;
	Write16(p, 0); //KartNum  PlayerCount
	for (size_t i = 0; i < 6; i++)
	{ //KartRefitInfo
		ClientNode* RoomClient = Room->Player[i];
		if (!RoomClient) {
			continue;
		}
		Karti++;
		BYTE* pKartRefitInfo = p;
		Write16(pKartRefitInfo, 0); //len

		/*UINT Uin = RoomClient->Uin;
		UINT KartID = RoomClient->KartID;*/

		const char* sql = nullptr;
		sqlite3_stmt* stmt = nullptr;
		int RefitCout = 0;
		short MaxFlags = 0;
		short WWeight = 0;
		short SpeedWeight = 0;
		short JetWeight = 0;
		short SJetWeight = 0;
		short AccuWeight = 0;
		int ShapeRefitCount = 0;
		int KartHeadRefitItemID = 0;
		int KartTailRefitItemID = 0;
		int KartFlankRefitItemID = 0;
		int KartTireRefitItemID = 0;
		int AntiCollisionWeight = 0;//防护
		int SecondRefitCount = 0;//进阶改装次数
		int Speed2Weight = 0;//推进系统
		int DriftVecWeight = 0;//离合系统
		int AdditionalZSpeedWeight = 0;//平衡系统
		int ShapeSuitID = 0;
		int LegendSuitLevel = 0;
		int LegendSuitLevelChoice = 0;
		int ShapeLegendSuitID = 0;
		//ResponseEnterRoom 这里闪退修复
		sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,AntiCollisionWeight,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin=? AND KartID=?;";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, RoomClient->Uin);
			sqlite3_bind_int(stmt, 2, RoomClient->KartID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				RefitCout = sqlite3_column_int(stmt, 0);
				MaxFlags = sqlite3_column_int(stmt, 1);
				WWeight = sqlite3_column_int(stmt, 2);
				SpeedWeight = sqlite3_column_int(stmt, 3);
				JetWeight = sqlite3_column_int(stmt, 4);
				SJetWeight = sqlite3_column_int(stmt, 5);
				AccuWeight = sqlite3_column_int(stmt, 6);

				ShapeRefitCount = sqlite3_column_int(stmt, 7);
				KartHeadRefitItemID = sqlite3_column_int(stmt, 8);
				KartTailRefitItemID = sqlite3_column_int(stmt, 9);
				KartFlankRefitItemID = sqlite3_column_int(stmt, 10);
				KartTireRefitItemID = sqlite3_column_int(stmt, 11);
				AntiCollisionWeight = sqlite3_column_int(stmt, 12);

				SecondRefitCount = sqlite3_column_int(stmt, 13);
				Speed2Weight = sqlite3_column_int(stmt, 14);
				DriftVecWeight = sqlite3_column_int(stmt, 15);
				AdditionalZSpeedWeight = sqlite3_column_int(stmt, 16);

				ShapeSuitID = sqlite3_column_int(stmt, 17);
				LegendSuitLevel = sqlite3_column_int(stmt, 18);
				LegendSuitLevelChoice = sqlite3_column_int(stmt, 19);
				ShapeLegendSuitID = sqlite3_column_int(stmt, 20);

			}
		}
		else
		{
			printf("NotifyEnterRoom 001 %s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;

		Write32(pKartRefitInfo, RoomClient->Uin); //Uin
		Write32(pKartRefitInfo, RoomClient->KartID); //KartId
		Write32(pKartRefitInfo, RefitCout); //RefitCout
		Write16(pKartRefitInfo, MaxFlags); //MaxFlags
		Write16(pKartRefitInfo, WWeight); //WWeight
		Write16(pKartRefitInfo, SpeedWeight); //SpeedWeight
		Write16(pKartRefitInfo, JetWeight); //JetWeight
		Write16(pKartRefitInfo, SJetWeight); //SJetWeight
		Write16(pKartRefitInfo, AccuWeight); //AccuWeight
		Write32(pKartRefitInfo, ShapeRefitCount); //ShapeRefitCount
		Write32(pKartRefitInfo, KartHeadRefitItemID); //KartHeadRefitItemID
		Write32(pKartRefitInfo, KartTailRefitItemID); //KartTailRefitItemID
		Write32(pKartRefitInfo, KartFlankRefitItemID); //KartFlankRefitItemID
		Write32(pKartRefitInfo, KartTireRefitItemID); //KartTireRefitItemID
		Write32(pKartRefitInfo, SecondRefitCount); //SecondRefitCount
		Write16(pKartRefitInfo, Speed2Weight); //Speed2Weight
		Write16(pKartRefitInfo, DriftVecWeight); //DriftVecWeight
		Write16(pKartRefitInfo, AdditionalZSpeedWeight); //AdditionalZSpeedWeight
		Write16(pKartRefitInfo, AntiCollisionWeight); //AntiCollisionWeight
		Write16(pKartRefitInfo, 0); //LuckyValue
		Write16(pKartRefitInfo, 0); //RefitLuckyValueMaxWeight
		Write32(pKartRefitInfo, ShapeSuitID); //ShapeSuitID
		Write8(pKartRefitInfo, LegendSuitLevel); //LegendSuitLevel
		Write32(pKartRefitInfo, LegendSuitLevelChoice); //LegendSuitLevelChoice
		Write32(pKartRefitInfo, ShapeLegendSuitID); //ShapeLegendSuitID

		len = pKartRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Set16(pKartNum, (WORD)Karti);
	/*
m_astKartRefitInfo[].m_uiUin
m_astKartRefitInfo[].m_iKartId
m_astKartRefitInfo[].m_iRefitCout
m_astKartRefitInfo[].m_shMaxFlags
m_astKartRefitInfo[].m_shWWeight
m_astKartRefitInfo[].m_shSpeedWeight
m_astKartRefitInfo[].m_shJetWeight
m_astKartRefitInfo[].m_shSJetWeight
m_astKartRefitInfo[].m_shAccuWeight
m_astKartRefitInfo[].m_iShapeRefitCount
m_astKartRefitInfo[].m_iKartHeadRefitItemID
m_astKartRefitInfo[].m_iKartTailRefitItemID
m_astKartRefitInfo[].m_iKartFlankRefitItemID
m_astKartRefitInfo[].m_iKartTireRefitItemID
	*/

	Write8(p, 0); //LoverRaceOrPro

	{ //STDynamicInfo
		BYTE* pSTDynamicInfo = p;
		Write16(pSTDynamicInfo, 0); //len

		Write8(pSTDynamicInfo, Room->STDynamicInfo.Season);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Timeslot);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Weather);

		len = pSTDynamicInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); //PreKnowMapID
	Write8(p, 0); //HasMoreInfo
	Write32(p, 0); //MapSubID

	Write8(p, 0); //HasFBInfo
	/*
m_astFBInfo[].m_bAvailableLev
m_astFBInfo[].m_bLev
m_astFBInfo[].m_bSubLev
m_astFBInfo[].m_iBossID
m_astFBInfo[].m_bFBSeason
	*/
	Write32(p, 0); //GuildScoreInNextGame
	Write16(p, 0); //GuildScoreAlreadyAward
	Write16(p, 0); //GuildScoreTotalAward


	len = p - buf;
	SendToClient(Client, 104, buf, len, Room->ID, FE_ROOM, Client->ConnID, Response);
}

void NotifyEnterRoom(ClientNode* Client, RoomNode* Room, ClientNode* RoomClient, char SeatID)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;


	Write16(p, Room->ID); //RoomID

	WriteVisbleInfo(p, RoomClient->Uin, SeatID, RoomClient->ConnID); //NewPlayerInfo
	{ //PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); //len


		Write32(pPlayerRelationInfo, RoomClient->Uin); //SrcUin
		Write32(pPlayerRelationInfo, 0); //RelationFlag
		Write32(pPlayerRelationInfo, 0); //RelationUin

		char RelationNickName[MaxNickName] = "";
		memcpy(pPlayerRelationInfo, RelationNickName, MaxNickName);
		pPlayerRelationInfo += MaxNickName;

		Write32(pPlayerRelationInfo, 0); //EngageTime
		Write32(pPlayerRelationInfo, 0); //NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); //BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); //EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0); //EngageFlag


		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 1); //KartNum
	/*
m_astKartRefitInfo[].m_uiUin
m_astKartRefitInfo[].m_iKartId
m_astKartRefitInfo[].m_iRefitCout
m_astKartRefitInfo[].m_shMaxFlags
m_astKartRefitInfo[].m_shWWeight
m_astKartRefitInfo[].m_shSpeedWeight
m_astKartRefitInfo[].m_shJetWeight
m_astKartRefitInfo[].m_shSJetWeight
m_astKartRefitInfo[].m_shAccuWeight
m_astKartRefitInfo[].m_iShapeRefitCount
m_astKartRefitInfo[].m_iKartHeadRefitItemID
m_astKartRefitInfo[].m_iKartTailRefitItemID
m_astKartRefitInfo[].m_iKartFlankRefitItemID
m_astKartRefitInfo[].m_iKartTireRefitItemID
	*/
	for (size_t i = 0; i < 1; i++)
	{ //KartRefitInfo
		BYTE* pKartRefitInfo = p;
		Write16(pKartRefitInfo, 0); //len


		const char* sql = nullptr;
		sqlite3_stmt* stmt = nullptr;
		int RefitCout = 0;
		short MaxFlags = 0;
		short WWeight = 0;
		short SpeedWeight = 0;
		short JetWeight = 0;
		short SJetWeight = 0;
		short AccuWeight = 0;
		int ShapeRefitCount = 0;
		int KartHeadRefitItemID = 0;
		int KartTailRefitItemID = 0;
		int KartFlankRefitItemID = 0;
		int KartTireRefitItemID = 0;
		int AntiCollisionWeight = 0;//防护
		int SecondRefitCount = 0;//进阶改装次数
		int Speed2Weight = 0;//推进系统
		int DriftVecWeight = 0;//离合系统
		int AdditionalZSpeedWeight = 0;//平衡系统
		int ShapeSuitID = 0;
		int LegendSuitLevel = 0;
		int LegendSuitLevelChoice = 0;
		int ShapeLegendSuitID = 0;

		sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,AntiCollisionWeight,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin=? AND KartID=?;";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL) == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, RoomClient->Uin);
			sqlite3_bind_int(stmt, 2, RoomClient->KartID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				RefitCout = sqlite3_column_int(stmt, 0);
				MaxFlags = sqlite3_column_int(stmt, 1);
				WWeight = sqlite3_column_int(stmt, 2);
				SpeedWeight = sqlite3_column_int(stmt, 3);
				JetWeight = sqlite3_column_int(stmt, 4);
				SJetWeight = sqlite3_column_int(stmt, 5);
				AccuWeight = sqlite3_column_int(stmt, 6);

				ShapeRefitCount = sqlite3_column_int(stmt, 7);
				KartHeadRefitItemID = sqlite3_column_int(stmt, 8);
				KartTailRefitItemID = sqlite3_column_int(stmt, 9);
				KartFlankRefitItemID = sqlite3_column_int(stmt, 10);
				KartTireRefitItemID = sqlite3_column_int(stmt, 11);
				AntiCollisionWeight = sqlite3_column_int(stmt, 12);

				SecondRefitCount = sqlite3_column_int(stmt, 13);
				Speed2Weight = sqlite3_column_int(stmt, 14);
				DriftVecWeight = sqlite3_column_int(stmt, 15);
				AdditionalZSpeedWeight = sqlite3_column_int(stmt, 16);

				ShapeSuitID = sqlite3_column_int(stmt, 17);
				LegendSuitLevel = sqlite3_column_int(stmt, 18);
				LegendSuitLevelChoice = sqlite3_column_int(stmt, 19);
				ShapeLegendSuitID = sqlite3_column_int(stmt, 20);

			}
		}
		else
		{
			printf("NotifyEnterRoom 001 %s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;

		Write32(pKartRefitInfo, RoomClient->Uin); //Uin
		Write32(pKartRefitInfo, RoomClient->KartID); //KartId
		Write32(pKartRefitInfo, RefitCout); //RefitCout
		Write16(pKartRefitInfo, MaxFlags); //MaxFlags
		Write16(pKartRefitInfo, WWeight); //WWeight
		Write16(pKartRefitInfo, SpeedWeight); //SpeedWeight
		Write16(pKartRefitInfo, JetWeight); //JetWeight
		Write16(pKartRefitInfo, SJetWeight); //SJetWeight
		Write16(pKartRefitInfo, AccuWeight); //AccuWeight
		Write32(pKartRefitInfo, ShapeRefitCount); //ShapeRefitCount
		Write32(pKartRefitInfo, KartHeadRefitItemID); //KartHeadRefitItemID
		Write32(pKartRefitInfo, KartTailRefitItemID); //KartTailRefitItemID
		Write32(pKartRefitInfo, KartFlankRefitItemID); //KartFlankRefitItemID
		Write32(pKartRefitInfo, KartTireRefitItemID); //KartTireRefitItemID
		Write32(pKartRefitInfo, SecondRefitCount); //SecondRefitCount
		Write16(pKartRefitInfo, Speed2Weight); //Speed2Weight
		Write16(pKartRefitInfo, DriftVecWeight); //DriftVecWeight
		Write16(pKartRefitInfo, AdditionalZSpeedWeight); //AdditionalZSpeedWeight
		Write16(pKartRefitInfo, AntiCollisionWeight); //AntiCollisionWeight
		Write16(pKartRefitInfo, 0); //LuckyValue
		Write16(pKartRefitInfo, 0); //RefitLuckyValueMaxWeight
		Write32(pKartRefitInfo, ShapeSuitID); //ShapeSuitID          皮肤ID
		Write8(pKartRefitInfo, LegendSuitLevel); //LegendSuitLevel   皮肤等级
		Write32(pKartRefitInfo, LegendSuitLevelChoice); //LegendSuitLevelChoice  传奇皮肤等级
		Write32(pKartRefitInfo, ShapeLegendSuitID); //ShapeLegendSuitID			 传奇皮肤ID	

		len = pKartRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //RoomInfoChange
		BYTE* pRoomInfoChange = p;
		Write16(pRoomInfoChange, 0); //len

		Write32(pRoomInfoChange, 0); //TeamJumpLevel
		Write32(pRoomInfoChange, 0); //RoomBGID

		len = pRoomInfoChange - p;
		Set16(p, (WORD)len);
		p += len;
	}


	len = p - buf;
	SendToClient(Client, 500, buf, len, Room->ID, FE_ROOM, Client->ConnID, Notify);
}
void RequestLeaveRoom(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short LeaveType = 0;
#ifndef ZingSpeed
	LeaveType = Read16(Body);
	BYTE LeaveRoomReason = Read8(Body);
#endif

	LeaveRoom(Client, LeaveType);
}
void LeaveRoom(ClientNode *Client, short LeaveType)
{
	if (!Client) return;
	if (Client->RoomID == 0)
	{
		return;
	}
	RoomNode *Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient == Client)
		{
			ResponseLeaveRoom(Client, LeaveType);
			Room->Player[i] = NULL;
			Client->RoomID = 0;
			Room->CurrentPlayerNum--;

			BOOL IsRoomOwner = FALSE;
			short NewRoomOwnerID = Room->RoomOwnerID;
			if (NewRoomOwnerID == Client->ConnID)
			{
				IsRoomOwner = TRUE;
				for (char i2 = 0; i2 < 6; i2++)
				{
					RoomClient = Room->Player[i2];
					if (RoomClient)
					{
						NewRoomOwnerID = RoomClient->ConnID; // 转让房主
						break;
					}
				}
			}
			Room->RoomOwnerID = NewRoomOwnerID;

			// 通知其它玩家
			int n = 0;
			for (char i2 = 0; i2 < 6; i2++)
			{
				RoomClient = Room->Player[i2];
				if (RoomClient)
				{
					NotifyLeaveRoom(RoomClient, Client->Uin, i, IsRoomOwner, NewRoomOwnerID, LeaveType);
					n++;
				}
			}

			if (n == 0)
			{
				if (Room->Timer)
				{
					if (DeleteTimerQueueTimer(TimerQueue, Room->Timer, INVALID_HANDLE_VALUE))
					{
						Room->Timer = NULL;
					}
				}
				Mutex.lock();
				Rooms.erase(Room->ID);
				Mutex.unlock();
				delete Room;
			}
			break;
		}
	}
}
void ResponseLeaveRoom(ClientNode *Client, short LeaveType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write8(p, 0); // ReasonLen

#ifndef ZingSpeed
	Write16(p, LeaveType); // LeaveType
	Write16(p, 0);		   // BaseGameMode
	Write16(p, 0);		   // SubGameMode
	Write8(p, 0);		   // IsLeaveGVoice
#endif

	len = p - buf;
	SendToClient(Client, 105, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}
void NotifyLeaveRoom(ClientNode *Client, UINT Uin, char SeatID, bool IsChangeRoomOwner, short NewRoomOwnerID, short LeaveType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, Client->RoomID);	  // RoomID
	Write32(p, Uin);			  // Uin
	Write8(p, SeatID);			  // SeatID
	Write8(p, IsChangeRoomOwner); // IsChangeRoomOwner
	Write16(p, NewRoomOwnerID);	  // NewRoomOwnerID

#ifdef ZingSpeed
	Write8(p, 0); // Flag
#else
	Write16(p, 0);							 // Flag
#endif

	Write8(p, 0); // HasFBInfo
	/*
m_astFBInfo[].m_bAvailableLev
m_astFBInfo[].m_bLev
m_astFBInfo[].m_bSubLev
m_astFBInfo[].m_iBossID
m_astFBInfo[].m_bFBSeason
	*/

	len = p - buf;
	SendToClient(Client, 501, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}
void NotifyClientUpdatePlayerID(ClientNode* Client, RoomNode* Room)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	Write16(p, Room->CurrentPlayerNum); //PlayerNum
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];

		if (RoomClient)
		{
			{ //PlayerIDInfo
				BYTE* pPlayerIDInfo = p;
				Write16(pPlayerIDInfo, 0); //len

				Write32(pPlayerIDInfo, RoomClient->Uin); //Uin
				Write16(pPlayerIDInfo, RoomClient->ConnID); //PlayerID

				len = pPlayerIDInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
	}

	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write16(p, 0);
	len = p - buf;
	SendToClient(Client, 24390, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestStartGame(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);


	// Client->PassedCheckPointPropS= { {0, 0} };
	RoomNode *Room = GetRoom(Client->RoomID);
	
	if (!Room)
	{
		ResponseLeaveRoom(Client, 0);
		ResponseStartGame(Client, "房间状态异常");
		return;
	}
	Room->TotalPlayerNum = Room->CurrentPlayerNum;
	Room->isFinsh = false;
	{ // 加入检测房间内玩家是否准备
		for (char i = 0; i < Room->CurrentPlayerNum; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient)
			{
				InitClientGameArrt(RoomClient);
				if (RoomClient->Uin != Client->Uin)
				{
					int count = RoomClient->IsReady;
					if (count == 1)
					{
						ResponseStartGame(Client, "有玩家未准备。");
						return;
					}
				}
			}
		}
	}
	Room->startStatus = 2;
	if (Room->BaseGameMode == 4) // 舞蹈
	{
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient)
			{
				RoomClient->InitForDance();
				NotifyDanceGameBegin(RoomClient);
			}
		}
		CreateRoomTimer(Room, DanceMusicDuration(Room->MusicID), OnDanceTimer);
	}
	else
	{
		UINT preMapID = Room->MapID;
		if (Room->BaseGameMode == 18&& preMapID==0) {
			preMapID = 20;
		}
		auto MapID = RoomMapID2GameMapID(preMapID);
		
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			
			if (RoomClient)
			{
				RoomClient->matchQueueType = 0;
				//Room->PlayerReal[i] = getBakClient(RoomClient);
				RoomClient->GameID = 2;
				RoomClient->InitForRace(MapID);
				

				NotifyClientUpdatePlayerID(RoomClient,Room);
				NotifyGameBegin(RoomClient, Room->Player);
				NotifyGameNPCInfo(RoomClient);
				NotifyKartPhysParam(RoomClient, Room->Player);
				NotifyOtherKartStoneInfo(RoomClient);
			}
		}
	}
	
}

void RequestStartGameMatch(ClientNode *Client)
{
	UINT Uin = Client->Uin;
	UINT Time = 0;
	InitClientGameArrt(Client);
	// Client->PassedCheckPointPropS= { {0, 0} };
	RoomNode *Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	Room->startStatus = 2;
	if (Room->BaseGameMode == 4) // 舞蹈
	{
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient)
			{
				RoomClient->InitForDance();
				NotifyDanceGameBegin(RoomClient);
			}
		}
		CreateRoomTimer(Room, DanceMusicDuration(Room->MusicID), OnDanceTimer);
	}
	else
	{
		auto MapID = RoomMapID2GameMapID(Room->MapID);
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient)
			{
				RoomClient->GameID = 2;
				RoomClient->InitForRace(MapID);

				NotifyGameBegin(RoomClient, Room->Player);
				NotifyKartPhysParam(RoomClient, Room->Player);
				NotifyOtherKartStoneInfo(RoomClient);
				// RequestPrepareReadyMath(Client);
				delay(8);
				NotifyRaceBegin(RoomClient);
			}
		}
	}
}
void WINAPI OnBegin1(RoomNode *Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient)
		{
			NotifyRaceBegin(RoomClient);
		}
	}
}
void RequestStartSingleTask(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	Z_INFO("[RequestStartSingleTask] 开始单机游戏\n");
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int TaskID = Read32(p);
	int MapID = Read32(p);
	UINT Level = Read32(p);
	InitClientGameArrt(Client);
	if (TaskID != 1701)
	{ // 剧情
		/*Room = NewRoom();
		Room->ID = 9999;
		Room->BaseGameMode = 1;
		Room->SubGameMode = 1;
		Room->SeatNum = 6;
		Room->ServerID = 113;
		Room->CurrentPlayerNum = 6;
		Room->Player[0] = Client;*/
		Client->RoomID = 9999;
		Client->taskID = TaskID;
		TaskInfo *pTaskInfo = GetTaskInfo(TaskID);
		if (pTaskInfo)
		{
			Client->MapID = pTaskInfo->MapID;
		}
	}
	else
	{
		RoomNode* Room = GetRoom(Client->RoomID);
		Client->MapID = Room->MapID;
		Client->taskID = TaskID;
	}
	Client->TaskID = TaskID;
	MapID = Client->MapID;
	MapID = RoomMapID2GameMapID(Client->MapID); 
	ClientNode *RoomClient = Client;
	if (RoomClient)
	{
		RoomClient->GameID = 254;
		RoomClient->InitForRace(MapID);
		if (TaskID == 1701) // 紫钻
		{
			NotifySingleGameBeginTask1701(RoomClient);
			NotifyKartPhysParam(RoomClient, NULL);
			NotifySingleRaceBeginDiy(Client, 0);
		}
		else if (TaskID == 110)
		{
			NotifySingleGameBeginTask110(RoomClient);
			NotifyKartPhysParam(RoomClient, NULL);
			NotifySingleRaceBegin(Client);
		}
		else // 其他
		{
			TaskInfo *taskInfo = GetTaskInfo(TaskID);
			NotifySingleGameBegin(RoomClient, taskInfo->EHexToByte, taskInfo->EHexToByteLen);
			NotifyKartPhysParam(RoomClient, NULL);
			NotifySingleRaceBegin(Client);
		}
	}
}

void ResponseStartSingleTask(ClientNode *Client, int taskId, int mapId)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, 73);			 // TaskID
	Write32(p, 108);		 // MapID
	Write8(p, 0);			 // ReasonLen

	len = p - buf;
	SendToClient(Client, 138, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseStartGame(ClientNode *Client, const char *Reason)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, -1); // ResultID

	len = strlen(Reason);
	Write8(p, (BYTE)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;

	len = p - buf;
	SendToClient(Client, 110, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}

void RequestChangeMap(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT NewMapID = Read32(Body);
	/*
m_iMapSubID
	*/
	char Season = -1;
	char Timeslot = -1;
	char Weather = -1;
	{
		BYTE *pSTDynamicInfo = Body;
		USHORT len = Read16(pSTDynamicInfo);

		Season = Read8(pSTDynamicInfo);
		Timeslot = Read8(pSTDynamicInfo);
		Weather = Read8(pSTDynamicInfo);

		Body += len;
	}

	RoomNode *Room = GetRoom(Client->RoomID);
	Room->MapID = NewMapID;
	Room->STDynamicInfo.Season = Season;
	Room->STDynamicInfo.Timeslot = Timeslot;
	Room->STDynamicInfo.Weather = Weather;

	ResponseChangeMap(Client, Room);

	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient && RoomClient != Client)
		{
			NotifyChangeMap(RoomClient, Client->Uin, Room);
		}
	}
}
void ResponseChangeMap(ClientNode *Client, RoomNode *Room)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Room->MapID); // NewMapID
	Write8(p, 0);			 // ReasonLen
	Write8(p, 0);			 // UnLockMapCondDescNum
	// m_astUnLockMapCondDesc[].m_bDescLen

	{ // STDynamicInfo
		BYTE *pSTDynamicInfo = p;
		Write16(pSTDynamicInfo, 0); // len

		Write8(pSTDynamicInfo, Room->STDynamicInfo.Season);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Timeslot);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Weather);

		len = pSTDynamicInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // PreKnowMapID
	Write32(p, 0); // MapSubID
#ifndef ZingSpeed
	Write8(p, 0); // FirstlyTipMap
#endif

	len = p - buf;
	SendToClient(Client, 121, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Response);
}
void NotifyChangeMap(ClientNode *Client, UINT Uin, RoomNode *Room)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin);		 // Uin
	Write32(p, Room->MapID); // NewMapID

	{ // STDynamicInfo
		BYTE *pSTDynamicInfo = p;
		Write16(pSTDynamicInfo, 0); // len

		Write8(pSTDynamicInfo, Room->STDynamicInfo.Season);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Timeslot);
		Write8(pSTDynamicInfo, Room->STDynamicInfo.Weather);

		len = pSTDynamicInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); // PreKnowMapID
	Write32(p, 0); // MapSubID
	Write8(p, 0);  // FirstlyTipMap

	len = p - buf;
	SendToClient(Client, 520, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}

void RequestChangeSeatState(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short RoomID = Read16(Body);
	BYTE SeatID = Read8(Body);
	BYTE SeatState = Read8(Body);
	RoomNode *Room = GetRoom(RoomID);
	if (SeatState == 0)
	{
		Room->SeatNum -= 1;
	}
	else
	{
		Room->SeatNum += 1;
	}

	ResponseChangeSeatState(Client, SeatID, SeatState);
}
void ResponseChangeSeatState(ClientNode *Client, BYTE SeatID, BYTE SeatState)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write8(p, SeatID);
	Write8(p, SeatState);

	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 107, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}

void RequestKickFromRoom(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT DstUin = Read32(Body);
	BYTE KickReason = Read8(Body);
	{ // 非房主踢人检测
		RoomNode *Room = GetRoom(Client->RoomID);
		if (Room->RoomOwnerID != Client->ConnID)
		{
			ResponseStartGame(Client, "没有踢人的权利");
			return;
		}
		// 把人请出去
		for (char i = 0; i < Room->CurrentPlayerNum; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient && RoomClient->Uin == DstUin)
			{
				RequestLeaveRoom(RoomClient, Body, BodyLen);
				ResponseStartGame(RoomClient, "你已被房主请出房间");
			}
		}
	}

	ResponseKickFromRoom(Client, DstUin);
}
void ResponseKickFromRoom(ClientNode *Client, UINT DstUin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write32(p, DstUin);

	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 108, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}

void NotifyChangeReadyState(ClientNode *Client, UINT Uin, bool State)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin);
	Write8(p, State);
	// 加入改变准备状态代码
	Client->IsReady = State;
	len = p - buf;
	SendToClient(Client, 504, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}
void RequestChangeReadyState(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	bool ReadyState = Read8(Body);
	ResponseChangeReadyState(Client, ReadyState);
	RoomNode *Room = GetRoom(Client->RoomID);

	if (!Room)
	{
		return;
	}
	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient && RoomClient != Client)
		{
			NotifyChangeReadyState(RoomClient, Client->Uin, ReadyState);
		}
	}
}
void ResponseChangeReadyState(ClientNode *Client, bool ReadyState)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write8(p, ReadyState);

	Write8(p, 0); // ReasonLen
	// 加入准备代码
	Client->IsReady = ReadyState;
	len = p - buf;
	SendToClient(Client, 109, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}
char *U2GRoom(const char *utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char *str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr)
		delete[] wstr;
	return str;
}
void RequestTalk(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE Type = Read8(Body);
	BYTE ClientUseFlag = Read8(Body);
	UINT DestPlayerUin = Read32(Body);

	USHORT ContentLength = Read16(Body);
	char *Content = (char *)Body;
	Body += ContentLength;
	BYTE BugletType = Read8(Body);
	BYTE ReserveFlag = Read8(Body);
	int c;
	c = atof(Content);
	ItemInfo Items[3000];
	
	if (strcmp(Content, "kick") == 0 && DestPlayerUin != 10001 && GetClient(DestPlayerUin) && Client->Uin == 10001)
	{
		NotifyKickPlayer(GetClient(DestPlayerUin), "你已经被管理员踢下线", 3);
		GetClient(DestPlayerUin)->IsExit = true;
	}
	else if (GetClient(DestPlayerUin))
	{
		NotifyTalk(Client, Client->Uin, Type, ClientUseFlag, DestPlayerUin, ContentLength, Content);
		NotifyTalk(GetClient(DestPlayerUin), Client->Uin, Type, ClientUseFlag, DestPlayerUin, ContentLength, Content);
	}
	if (Type == 1)
	{
		if (strcmp(Content, "g") == 0 || strcmp(Content, "G") == 0)
		{
			Client->matchQueueType = 0;
			RequestLeaveGame2(Client);
			Client->MapID = 0;
		}
		if ((strcmp(Content, "f") == 0 || strcmp(Content, "f") == 0) && (Client->Uin == 10001))
		{
			Client->IsFinish = true;
		}
		if ((strcmp(Content, "cls") == 0 || strcmp(Content, "clear") == 0) && (Client->Uin == 10001))
		{
			system("cls");
		}
		if (strcmp(Content, "go") == 0)
		{
			NotifyRaceBegin(Client);
		}
		if (strcmp(Content, "npc") == 0)
		{
			RoomNode* Room = GetRoom(Client->RoomID);
			if (Room&&Room->RoomOwnerID==Client->ConnID)
			{
				if (Room->OpenNpc==true) {
					Room->OpenNpc = false;
					Room->NPCNum = 0;
					for (size_t i = 0; i < 6; i++) {
						Room->NPCPlayer[i] = NULL;
					}
					ResponseStartGame(Client, "ai补全关闭");
				}
				else {
					if (Room->CurrentPlayerNum < 6) {
						Room->OpenNpc = true;
						Room->NPCNum = 6 - Room->CurrentPlayerNum;
						ResponseStartGame(Client, "ai补全开启");
					}
				}
				
			}
		}
		
		if (strcmp(Content, "ob") == 0 || strcmp(Content, "OB") == 0) {
			ResponseChangeOBState(Client, 1); //134
			
			RoomNode* Room = GetRoom(Client->RoomID);
			if (Room)
			{

				for (char i = 0; i < 6; i++)
				{
					ClientNode* RoomClient = Room->Player[i];
					if (RoomClient&& RoomClient!=Client)
					{
						NotifyChangeOBState(RoomClient, 1, Client->Uin); //609
					}
				}
			}
			
		}
		/// ResponseG2DMagicCardGetCardAward(Client, c);
	}
	if (Type == 3)
	{
		NotifyBugletTalk(Client, Client, ContentLength, Content, BugletType);
		NotifyBugletTalkALL(Client, ContentLength, Content, BugletType);
		// NotifyBroadcast(Client);
	}
	ResponseTalk(Client, Type, ClientUseFlag, DestPlayerUin);

	Z_INFO("类型:%d, 客户端使用标志:%d. 目标玩家编号:%d,测试:%d,喇叭类型:%d\n", Type, ClientUseFlag, DestPlayerUin, c, BugletType);

	RoomNode *Room = GetRoom(Client->RoomID);
	if (Room)
	{

		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient)
			{
				NotifyTalk(RoomClient, Client->Uin, Type, ClientUseFlag, DestPlayerUin, ContentLength, Content);
			}
		}
	}
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (ChatRoom)
	{
		for (char i = 0; i < 99; i++)
		{
			ClientNode *RoomClient = ChatRoom->Player[i];
			if (RoomClient)
			{
				NotifyTalk(RoomClient, Client->Uin, Type, ClientUseFlag, DestPlayerUin, ContentLength, Content);
			}
		}
	}
}
void ResponseTalk(ClientNode *Client, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write8(p, Type);
	Write8(p, ClientUseFlag);
	Write32(p, DestPlayerUin);

	Write8(p, 0); // ReasonLen
	Write8(p, 0); // BugletType
	Write8(p, 0); // ReserveFlag
	Write8(p, 0); // ReserveData
#ifndef ZingSpeed
	Write8(p, 0);  // TalkSpecialType
	Write32(p, 0); // Seq
#endif

	len = p - buf;
	SendToClient(Client, 111, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}
void NotifyTalk(ClientNode *Client, UINT SrcUin, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin, short ContentLength, const char *Content)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, SrcUin);

	WriteString(p, GetClient(SrcUin)->NickName, 16);
	// memset(p, 0, MaxNickName); //SrcNickName[]
	// p += MaxNickName;

	Write8(p, GetClient(SrcUin)->Gender); // Gender

	Write8(p, Type);
	Write8(p, ClientUseFlag);
	Write32(p, DestPlayerUin);

	Write16(p, ContentLength);
	memcpy(p, Content, ContentLength);
	p += ContentLength;

	Write8(p, 0);  // BugletType
	Write16(p, 0); // Identify
	Write16(p, 0); // VipFlag
	Write8(p, 0);  // CanReport

	{ // TalkerInfo
		BYTE *pTalkerInfo = p;
		Write16(pTalkerInfo, 0); // len

		Write32(pTalkerInfo, 0); // Exp

		{ // NobleInfo
			BYTE *pNobleInfo = pTalkerInfo;
			Write16(pNobleInfo, 0); // len

			Write32(pNobleInfo, Client->NobleID); // NobleID
			Write8(pNobleInfo, 5);			  // NobleLevel
			Write32(pNobleInfo, 1);			  // NoblePoint
			Write32(pNobleInfo, 30);		  // NobleLeftDays

			len = pNobleInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
#ifndef ZingSpeed
		{ // GuildVipBaseInfo
			BYTE *pGuildVipBaseInfo = pTalkerInfo;
			Write16(pGuildVipBaseInfo, 0); // len

			Write8(pGuildVipBaseInfo, 5);  // GuildVipLevel
			Write32(pGuildVipBaseInfo, 1); // GuildVipPoint

			len = pGuildVipBaseInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
		Write32(pTalkerInfo, 0); // GuildId

		Write8(pTalkerInfo, 0); // HasLoverVip
		/*
		for (size_t i = 0; i < n; i++)
		{ //LoverVipInfo
			BYTE* pLoverVipInfo = pTalkerInfo;
			Write16(pLoverVipInfo, 0); //len

			Write8(pLoverVipInfo, 0); //LoverVipLevel
			Write32(pLoverVipInfo, 0); //LoverVipPoint
			Write8(pLoverVipInfo, 0); //GrowRate

			len = pLoverVipInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
		*/
		Write8(pTalkerInfo, 0); // GardenLevel
		Write8(pTalkerInfo, 0); // ConsumeVipLevel
		{						// EmperorInfo
			BYTE *pEmperorInfo = pTalkerInfo;
			Write16(pEmperorInfo, 0); // len

			Write8(pEmperorInfo, 0);  // EmperorLevel
			Write32(pEmperorInfo, 0); // EmperorPoint
			Write32(pEmperorInfo, 0); // EmperorLeftDays
			Write8(pEmperorInfo, 0);  // EmperorGrowRate

			len = pEmperorInfo - pTalkerInfo;
			Set16(pTalkerInfo, (WORD)len);
			pTalkerInfo += len;
		}
		Write32(pTalkerInfo, 0); // HelperLev
#endif

		len = pTalkerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0);  // ReserveFlag
	Write32(p, 0); // ReserveData
#ifndef ZingSpeed
	memset(p, 0, 18); // Contentid[]
	p += 18;
#endif
	Write8(p, 0);  // transBufType
	Write16(p, 0); // transBufLen
#ifndef ZingSpeed
	Write8(p, 0);  // MsgSource
	Write16(p, 0); // DstNickNameLength
	Write32(p, 0); // Seq
#endif

	len = p - buf;
	SendToClient(Client, 505, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}
void ResponseGetInviteList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_SPEEDMEMBER | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG;

	Write16(p, 0);			 // ResultID
	Write32(p, Uin);		 // Uin
	Write16(p, ChatRoomNum); // PlayerNum

	for (size_t i = 0; i < ChatRoomNum; i++)
	{ // InvitePlayerBaseInfo
		BYTE *pInvitePlayerBaseInfo = p;
		Write16(pInvitePlayerBaseInfo, 0); // len

		Write32(pInvitePlayerBaseInfo, i + 1); // Uin
		// WriteString(pInvitePlayerBaseInfo, 0); //NickName[]
		memcpy(pInvitePlayerBaseInfo, ChatRooms[i].Name, MaxRoomName); // RoomName
		pInvitePlayerBaseInfo += MaxRoomName;

		Write8(pInvitePlayerBaseInfo, 37);
		Write8(pInvitePlayerBaseInfo, 0);
		Write8(pInvitePlayerBaseInfo, 0);
		Write8(pInvitePlayerBaseInfo, 0);

		Write32(pInvitePlayerBaseInfo, 0);
		Write32(pInvitePlayerBaseInfo, 77777); // Experience 荣耀
		Write32(pInvitePlayerBaseInfo, 4);	   // License 驾驶等级
		Write32(pInvitePlayerBaseInfo, 888);   // TotalHonor 总荣誉
		Write32(pInvitePlayerBaseInfo, 777);   // Charm
		Write32(pInvitePlayerBaseInfo, 666);   // DanceExp	舞蹈经验
		Write32(pInvitePlayerBaseInfo, 107);   // Admiration		钦佩 着装度
		Write32(pInvitePlayerBaseInfo, 555);   // TeamWorkExp   团队工作经验

		len = pInvitePlayerBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // ReasonLen
	Write8(p, 0); // Type

	len = p - buf;
	SendToClient(Client, 255, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestQuickEnterRoom(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short BaseGameMode = Read16(p);
	short SubGameMode = Read16(p);
	bool IsNewerOpen = Read8(p);
	bool FBSeason = Read8(p);
	bool FBLev = Read8(p);
	char HasChangedChannel = Read8(p);
	bool RoomFlag = Read8(p);
	if (BaseGameMode == 1 && SubGameMode == 1)
	{
		RequestQuestEnterRoom(Client, Body, BodyLen);
	}
	else
	{
	}
}

void ResponseQuickEnterRoom(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write16(p, 0); // RoomID
	Write16(p, 0); // BaseGameMode
	Write16(p, 0); // SubGameMode
	// WriteString(p, 0); //RoomName[]
	Write8(p, 0);  // SeatNum
	Write8(p, 0);  // Flag
	Write8(p, 0);  // SeatID
	Write8(p, 0);  // TeamID
	Write32(p, 0); // MapID
	Write16(p, 0); // RoomOwnerID
	Write8(p, 0);  // SeatStatus[]
	Write8(p, 0);  // PlayerCount
	INT n = 0;
	for (size_t i = 0; i < n; i++)
	{ // PlayerRoomVisibleInfo
		BYTE *pPlayerRoomVisibleInfo = p;
		Write16(pPlayerRoomVisibleInfo, 0); // len

		Write32(pPlayerRoomVisibleInfo, 0); // Uin
		Write32(pPlayerRoomVisibleInfo, 0); // Identity
		Write8(pPlayerRoomVisibleInfo, 0);	// SeatID
		Write8(pPlayerRoomVisibleInfo, 0);	// TeamID
		Write16(pPlayerRoomVisibleInfo, 0); // PlayerID
		Write8(pPlayerRoomVisibleInfo, 0);	// Status
		{									// PlayerDBBaseInfo
			BYTE *pPlayerDBBaseInfo = pPlayerRoomVisibleInfo;
			Write16(pPlayerDBBaseInfo, 0); // len

			// WriteString(pPlayerDBBaseInfo, 0); //NickName[]
			Write8(pPlayerDBBaseInfo, 0);  // Gender
			Write8(pPlayerDBBaseInfo, 0);  // Country
			Write32(pPlayerDBBaseInfo, 0); // License
			Write32(pPlayerDBBaseInfo, 0); // Experience
			Write32(pPlayerDBBaseInfo, 0); // SuperMoney
			Write32(pPlayerDBBaseInfo, 0); // Money
			Write32(pPlayerDBBaseInfo, 0); // WinNum
			Write32(pPlayerDBBaseInfo, 0); // SecondNum
			Write32(pPlayerDBBaseInfo, 0); // ThirdNum
			Write32(pPlayerDBBaseInfo, 0); // TotalNum
			Write32(pPlayerDBBaseInfo, 0); // CurHonor
			Write32(pPlayerDBBaseInfo, 0); // TotalHonor
			Write32(pPlayerDBBaseInfo, 0); // TodayHonor
			Write32(pPlayerDBBaseInfo, 0); // RelaxTime
			Write32(pPlayerDBBaseInfo, 0); // MonthDurationBefore
			Write32(pPlayerDBBaseInfo, 0); // MonthDurationCur
			Write32(pPlayerDBBaseInfo, 0); // Charm
			Write32(pPlayerDBBaseInfo, 0); // DurationGame
			Write32(pPlayerDBBaseInfo, 0); // DanceExp
			Write32(pPlayerDBBaseInfo, 0); // Coupons
			Write32(pPlayerDBBaseInfo, 0); // Admiration
			Write32(pPlayerDBBaseInfo, 0); // LuckMoney
			Write32(pPlayerDBBaseInfo, 0); // TeamWorkExp
			Write32(pPlayerDBBaseInfo, 0); // AchievePoint
			Write32(pPlayerDBBaseInfo, 0); // RegTime
			// WriteString(pPlayerDBBaseInfo, 0); //Signature[]
			Write32(pPlayerDBBaseInfo, 0); // LuckyMatchPoint
			Write32(pPlayerDBBaseInfo, 0); // LuckyMatchNum
			Write16(pPlayerDBBaseInfo, 0); // LuckyMatchFirstNum
			Write8(pPlayerDBBaseInfo, 0);  // LuckyMatchBestRand
			Write32(pPlayerDBBaseInfo, 0); // LuckyMatchWinNum
			Write32(pPlayerDBBaseInfo, 0); // FizzPointTotal
			Write16(pPlayerDBBaseInfo, 0); // FizzPointDaily
			Write16(pPlayerDBBaseInfo, 0); // FizzPointWeekly
			Write16(pPlayerDBBaseInfo, 0); // FizzPointLastWeek
			Write8(pPlayerDBBaseInfo, 0);  // FizzLotteryStatus
			Write32(pPlayerDBBaseInfo, 0); // FizzLastUpdateTime
			Write32(pPlayerDBBaseInfo, 0); // SNSLocaleCode
			Write32(pPlayerDBBaseInfo, 0); // AuctionRightUnlockTime
			Write8(pPlayerDBBaseInfo, 0);  // ReserveFlag
			Write32(pPlayerDBBaseInfo, 0); // CurConsumeScore
			Write32(pPlayerDBBaseInfo, 0); // HistoryConsumeScore
			Write8(pPlayerDBBaseInfo, 0);  // CrashModeSponsorID
			Write32(pPlayerDBBaseInfo, 0); // Popularity
			Write32(pPlayerDBBaseInfo, 0); // LadderMatchAchievePoint
			Write32(pPlayerDBBaseInfo, 0); // Cash
			Write32(pPlayerDBBaseInfo, 0); // PlayerGPMIdentity
			Write32(pPlayerDBBaseInfo, 0); // PointTotalScore
			Write32(pPlayerDBBaseInfo, 0); // totalMapMedalNum
			Write32(pPlayerDBBaseInfo, 0); // lastMapMedalNum
			Write32(pPlayerDBBaseInfo, 0); // curMapMedalNum
			Write32(pPlayerDBBaseInfo, 0); // curMapMedalSeasonId
			Write32(pPlayerDBBaseInfo, 0); // lastClearMapMedalTime
			Write32(pPlayerDBBaseInfo, 0); // max2048Score
			Write32(pPlayerDBBaseInfo, 0); // curPlay2048LeftNum
			Write32(pPlayerDBBaseInfo, 0); // CheerValue
			Write16(pPlayerDBBaseInfo, 0); // SeasonID
			Write32(pPlayerDBBaseInfo, 0); // LastCheerValue
			Write32(pPlayerDBBaseInfo, 0); // SpeedBean
			Write32(pPlayerDBBaseInfo, 0); // SpeedCoin

			len = pPlayerDBBaseInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ // PlayerGuildInfo
			BYTE *pPlayerGuildInfo = pPlayerRoomVisibleInfo;
			Write16(pPlayerGuildInfo, 0); // len

			// WriteString(pPlayerGuildInfo, 0); //GuildName[]
			Write16(pPlayerGuildInfo, 0); // Duty
			Write32(pPlayerGuildInfo, 0); // GuildID
			Write32(pPlayerGuildInfo, 0); // Right
			Write32(pPlayerGuildInfo, 0); // Medal
			Write32(pPlayerGuildInfo, 0); // Contribute
			// WriteString(pPlayerGuildInfo, 0); //DutyTitle[]

			len = pPlayerGuildInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write16(pPlayerRoomVisibleInfo, 0); // EquipedItemNum
		for (size_t i = 0; i < n; i++)
		{ // PlayerItemInfo
			BYTE *pPlayerItemInfo = pPlayerRoomVisibleInfo;
			Write16(pPlayerItemInfo, 0); // len

			Write32(pPlayerItemInfo, 0); // ItemID
			Write32(pPlayerItemInfo, 0); // ItemNum
			Write32(pPlayerItemInfo, 0); // AvailPeriod
			Write8(pPlayerItemInfo, 0);	 // Status
			Write32(pPlayerItemInfo, 0); // ObtainTime
			Write32(pPlayerItemInfo, 0); // OtherAttribute
			Write16(pPlayerItemInfo, 0); // ItemType

			len = pPlayerItemInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write16(pPlayerRoomVisibleInfo, 0); // PetNum
		for (size_t i = 0; i < n; i++)
		{ // PetInfo
			BYTE *pPetInfo = pPlayerRoomVisibleInfo;
			Write16(pPetInfo, 0); // len

			Write32(pPetInfo, 0); // Uin
			Write32(pPetInfo, 0); // PetId
			// WriteString(pPetInfo, 0); //PetName[]
			Write8(pPetInfo, 0);  // IsProper
			Write16(pPetInfo, 0); // Level
			Write8(pPetInfo, 0);  // Status
			Write16(pPetInfo, 0); // PetSkillNum
			for (size_t i = 0; i < n; i++)
			{ // PetSkillList
				BYTE *pPetSkillList = pPetInfo;
				Write16(pPetSkillList, 0); // len

				Write16(pPetSkillList, 0); // SkillID
				Write8(pPetSkillList, 0);  // Status
				Write8(pPetSkillList, 0);  // Active
				Write32(pPetSkillList, 0); // Value

				len = pPetSkillList - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write32(pPetInfo, 0); // StrengLevel
			Write8(pPetInfo, 0);  // TopStatus
			{					  // CommonInfo
				BYTE *pCommonInfo = pPetInfo;
				Write16(pCommonInfo, 0); // len

				Write32(pCommonInfo, 0); // HuanHuaLevel
				Write8(pCommonInfo, 0);	 // CanHuanHua

				len = pCommonInfo - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write16(pPetInfo, 0); // PKStar
			Write32(pPetInfo, 0); // PetSkinID

			len = pPetInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ // PlayerGuildMatchInfo
			BYTE *pPlayerGuildMatchInfo = pPlayerRoomVisibleInfo;
			Write16(pPlayerGuildMatchInfo, 0); // len

			Write32(pPlayerGuildMatchInfo, 0); // SelfPoint
			Write32(pPlayerGuildMatchInfo, 0); // WinNum
			Write32(pPlayerGuildMatchInfo, 0); // TotalNum

			len = pPlayerGuildMatchInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0);	// IsInTopList
		Write8(pPlayerRoomVisibleInfo, 0);	// LoverRaceOrPro
		Write8(pPlayerRoomVisibleInfo, 0);	// TmpEffectNum
		Write32(pPlayerRoomVisibleInfo, 0); // TmpEffectItems[]
		Write8(pPlayerRoomVisibleInfo, 0);	// OBState
		Write8(pPlayerRoomVisibleInfo, 0);	// DebutOrX5
		Write8(pPlayerRoomVisibleInfo, 0);	// RandKeyFlag
		Write32(pPlayerRoomVisibleInfo, 0); // VipFlag
		Write8(pPlayerRoomVisibleInfo, 0);	// HaveAppellation
		for (size_t i = 0; i < n; i++)
		{ // CurAppellation
			BYTE *pCurAppellation = pPlayerRoomVisibleInfo;
			Write16(pCurAppellation, 0); // len

			Write8(pCurAppellation, 0);	 // Type
			Write8(pCurAppellation, 0);	 // Level
			Write8(pCurAppellation, 0);	 // Status
			Write8(pCurAppellation, 0);	 // Difficulty
			Write32(pCurAppellation, 0); // MapId
			Write32(pCurAppellation, 0); // Value

			len = pCurAppellation - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ // NobleInfo
			BYTE *pNobleInfo = pPlayerRoomVisibleInfo;
			Write16(pNobleInfo, 0); // len

			Write32(pNobleInfo, 0); // NobleID
			Write8(pNobleInfo, 0);	// NobleLevel
			Write32(pNobleInfo, 0); // NoblePoint
			Write32(pNobleInfo, 0); // NobleLeftDays

			len = pNobleInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HasCarryWizard
		for (size_t i = 0; i < n; i++)
		{ // CarryWizardInfo
			BYTE *pCarryWizardInfo = pPlayerRoomVisibleInfo;
			Write16(pCarryWizardInfo, 0); // len

			Write16(pCarryWizardInfo, 0); // WizardID
			// WriteString(pCarryWizardInfo, 0); //NickName[]
			Write16(pCarryWizardInfo, 0); // WizardType
			Write8(pCarryWizardInfo, 0);  // Order
			Write8(pCarryWizardInfo, 0);  // EvolutionBranch
			Write8(pCarryWizardInfo, 0);  // IsHuanHua

			len = pCarryWizardInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ // GuildVipBaseInfo
			BYTE *pGuildVipBaseInfo = pPlayerRoomVisibleInfo;
			Write16(pGuildVipBaseInfo, 0); // len

			Write8(pGuildVipBaseInfo, 0);  // GuildVipLevel
			Write32(pGuildVipBaseInfo, 0); // GuildVipPoint

			len = pGuildVipBaseInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write32(pPlayerRoomVisibleInfo, 0); // ExFlag
		Write8(pPlayerRoomVisibleInfo, 0);	// HaveLDMBaseInfo
		for (size_t i = 0; i < n; i++)
		{ // PlayerLDMBaseInfo
			BYTE *pPlayerLDMBaseInfo = pPlayerRoomVisibleInfo;
			Write16(pPlayerLDMBaseInfo, 0); // len

			Write8(pPlayerLDMBaseInfo, 0);	// Grade
			Write32(pPlayerLDMBaseInfo, 0); // Score
			Write32(pPlayerLDMBaseInfo, 0); // MaxScore
			Write32(pPlayerLDMBaseInfo, 0); // MaxGrade

			len = pPlayerLDMBaseInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HasWl
		for (size_t i = 0; i < n; i++)
		{ // PlayerWlVisibleInfo
			BYTE *pPlayerWlVisibleInfo = pPlayerRoomVisibleInfo;
			Write16(pPlayerWlVisibleInfo, 0); // len

			Write32(pPlayerWlVisibleInfo, 0); // WonderLandID
			// WriteString(pPlayerWlVisibleInfo, 0); //WonderLandName[]
			Write16(pPlayerWlVisibleInfo, 0); // WonderLandDuty
			Write8(pPlayerWlVisibleInfo, 0);  // SubType
			Write8(pPlayerWlVisibleInfo, 0);  // GuildGrade

			len = pPlayerWlVisibleInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HasLoverVip
		for (size_t i = 0; i < n; i++)
		{ // LoverVipInfo
			BYTE *pLoverVipInfo = pPlayerRoomVisibleInfo;
			Write16(pLoverVipInfo, 0); // len

			Write8(pLoverVipInfo, 0);  // LoverVipLevel
			Write32(pLoverVipInfo, 0); // LoverVipPoint
			Write8(pLoverVipInfo, 0);  // GrowRate

			len = pLoverVipInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HasBattleModeSkillInfoList
		for (size_t i = 0; i < n; i++)
		{ // BattleModeSkillInfoList
			BYTE *pBattleModeSkillInfoList = pPlayerRoomVisibleInfo;
			Write16(pBattleModeSkillInfoList, 0); // len

			Write8(pBattleModeSkillInfoList, 0); // SkillNum
			for (size_t i = 0; i < n; i++)
			{ // SkillInfo
				BYTE *pSkillInfo = pBattleModeSkillInfoList;
				Write16(pSkillInfo, 0); // len

				Write16(pSkillInfo, 0); // SkillId
				Write8(pSkillInfo, 0);	// SkillLevel

				len = pSkillInfo - pBattleModeSkillInfoList;
				Set16(pBattleModeSkillInfoList, (WORD)len);
				pBattleModeSkillInfoList += len;
			}

			len = pBattleModeSkillInfoList - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HasGarden
		for (size_t i = 0; i < n; i++)
		{ // PersonalGardenBaseInfo
			BYTE *pPersonalGardenBaseInfo = pPlayerRoomVisibleInfo;
			Write16(pPersonalGardenBaseInfo, 0); // len

			Write8(pPersonalGardenBaseInfo, 0);	 // GardenLevel
			Write32(pPersonalGardenBaseInfo, 0); // GardenPoint
			Write8(pPersonalGardenBaseInfo, 0);	 // GrowRate
			Write32(pPersonalGardenBaseInfo, 0); // GardenExpireTime
			{									 // SimpleInfo
				BYTE *pSimpleInfo = pPersonalGardenBaseInfo;
				Write16(pSimpleInfo, 0); // len

				Write32(pSimpleInfo, 0); // WeekPopularity
				Write32(pSimpleInfo, 0); // TotalPopularity
				Write32(pSimpleInfo, 0); // LastUpdatePopularityTime
				Write8(pSimpleInfo, 0);	 // PrivateType

				len = pSimpleInfo - pPersonalGardenBaseInfo;
				Set16(pPersonalGardenBaseInfo, (WORD)len);
				pPersonalGardenBaseInfo += len;
			}

			len = pPersonalGardenBaseInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ // ConsumeVipInfo
			BYTE *pConsumeVipInfo = pPlayerRoomVisibleInfo;
			Write16(pConsumeVipInfo, 0); // len

			Write32(pConsumeVipInfo, 0); // VipLevel
			Write32(pConsumeVipInfo, 0); // CharmValueOfMonth
			Write32(pConsumeVipInfo, 0); // SearchTreasureNums
			Write32(pConsumeVipInfo, 0); // GetTreasureNums

			len = pConsumeVipInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HaveBaby
		{								   // LoverBabyInfo
			BYTE *pLoverBabyInfo = pPlayerRoomVisibleInfo;
			Write16(pLoverBabyInfo, 0); // len

			Write32(pLoverBabyInfo, 0); // LoverBabyID
			Write32(pLoverBabyInfo, 0); // ItemID
			Write32(pLoverBabyInfo, 0); // GrowValue
			Write32(pLoverBabyInfo, 0); // MentalValue
			Write32(pLoverBabyInfo, 0); // BabyProfileLevel
			Write32(pLoverBabyInfo, 0); // StrengthLevel
			Write32(pLoverBabyInfo, 0); // GrowLevel
			// WriteString(pLoverBabyInfo, 0); //LoveBabyNickName[]
			Write16(pLoverBabyInfo, 0); // LoverBabyStat
			Write16(pLoverBabyInfo, 0); // LoverBabyEquipStat
			Write16(pLoverBabyInfo, 0); // LoverBabyTransferdStatus

			len = pLoverBabyInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ // EmperorInfo
			BYTE *pEmperorInfo = pPlayerRoomVisibleInfo;
			Write16(pEmperorInfo, 0); // len

			Write8(pEmperorInfo, 0);  // EmperorLevel
			Write32(pEmperorInfo, 0); // EmperorPoint
			Write32(pEmperorInfo, 0); // EmperorLeftDays
			Write8(pEmperorInfo, 0);  // EmperorGrowRate

			len = pEmperorInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write16(pPlayerRoomVisibleInfo, 0); // RaceCardNums
		for (size_t i = 0; i < n; i++)
		{ // RaceCardAddValueInfo
			BYTE *pRaceCardAddValueInfo = pPlayerRoomVisibleInfo;
			Write16(pRaceCardAddValueInfo, 0); // len

			Write32(pRaceCardAddValueInfo, 0); // RaceCardID
			Write32(pRaceCardAddValueInfo, 0); // AddValue

			len = pRaceCardAddValueInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		{ // GansterScoreInfo
			BYTE *pGansterScoreInfo = pPlayerRoomVisibleInfo;
			Write16(pGansterScoreInfo, 0); // len

			Write32(pGansterScoreInfo, 0); // GansterSeasonID
			Write32(pGansterScoreInfo, 0); // GansterScore
			Write32(pGansterScoreInfo, 0); // PoliceScore
			Write32(pGansterScoreInfo, 0); // TotalGansterScore

			len = pGansterScoreInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write32(pPlayerRoomVisibleInfo, 0); // OlympicId
		Write32(pPlayerRoomVisibleInfo, 0); // NPCEliminateWinTimes
		Write8(pPlayerRoomVisibleInfo, 0);	// OriginTeamID
		Write8(pPlayerRoomVisibleInfo, 0);	// NxNpc
		Write16(pPlayerRoomVisibleInfo, 0); // NxNpcAILevel
		{									// EquippedActiveKartInfo
			BYTE *pEquippedActiveKartInfo = pPlayerRoomVisibleInfo;
			Write16(pEquippedActiveKartInfo, 0); // len

			Write8(pEquippedActiveKartInfo, 0);	 // HaveActiveInfo
			Write32(pEquippedActiveKartInfo, 0); // KartID
			Write32(pEquippedActiveKartInfo, 0); // ActiveLevel

			len = pEquippedActiveKartInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // SpecialActivityStatus
		Write8(pPlayerRoomVisibleInfo, 0); // AreaID
		Write8(pPlayerRoomVisibleInfo, 0); // HaveRaiseSecondBaby
		{								   // SecondLoverBabyInfo
			BYTE *pSecondLoverBabyInfo = pPlayerRoomVisibleInfo;
			Write16(pSecondLoverBabyInfo, 0); // len

			Write32(pSecondLoverBabyInfo, 0); // LoverBabyID
			Write32(pSecondLoverBabyInfo, 0); // ItemID
			Write32(pSecondLoverBabyInfo, 0); // GrowValue
			Write32(pSecondLoverBabyInfo, 0); // MentalValue
			Write32(pSecondLoverBabyInfo, 0); // BabyProfileLevel
			Write32(pSecondLoverBabyInfo, 0); // StrengthLevel
			Write32(pSecondLoverBabyInfo, 0); // GrowLevel
			// WriteString(pSecondLoverBabyInfo, 0); //LoveBabyNickName[]
			Write16(pSecondLoverBabyInfo, 0); // LoverBabyStat
			Write16(pSecondLoverBabyInfo, 0); // LoverBabyEquipStat
			Write16(pSecondLoverBabyInfo, 0); // LoverBabyTransferdStatus

			len = pSecondLoverBabyInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HasRankedMatchInfo
		for (size_t i = 0; i < n; i++)
		{ // RankedMatchGradeInfo
			BYTE *pRankedMatchGradeInfo = pPlayerRoomVisibleInfo;
			Write16(pRankedMatchGradeInfo, 0); // len

			Write32(pRankedMatchGradeInfo, 0); // GradeLevel
			Write32(pRankedMatchGradeInfo, 0); // ChildLevel
			Write32(pRankedMatchGradeInfo, 0); // GradeScore
			Write32(pRankedMatchGradeInfo, 0); // MaxGradeScore
			Write32(pRankedMatchGradeInfo, 0); // TotalScore

			len = pRankedMatchGradeInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // HasHuanLingChangeInfo
		for (size_t i = 0; i < n; i++)
		{ // HuanLingSuitInfo
			BYTE *pHuanLingSuitInfo = pPlayerRoomVisibleInfo;
			Write16(pHuanLingSuitInfo, 0); // len

			Write32(pHuanLingSuitInfo, 0); // SuitID
			Write8(pHuanLingSuitInfo, 0);  // ChangeLevel

			len = pHuanLingSuitInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0);	// EquipSealType
		Write16(pPlayerRoomVisibleInfo, 0); // Role
		Write8(pPlayerRoomVisibleInfo, 0);	// SpecialActivityIDNum
		Write8(pPlayerRoomVisibleInfo, 0);	// CrewID
		Write8(pPlayerRoomVisibleInfo, 0);	// SpecialActivityInfoNum
		for (size_t i = 0; i < n; i++)
		{ // SpecialActivityInfo
			BYTE *pSpecialActivityInfo = pPlayerRoomVisibleInfo;
			Write16(pSpecialActivityInfo, 0); // len

			Write8(pSpecialActivityInfo, 0); // ProfessionLicenseInfoNum
			for (size_t i = 0; i < n; i++)
			{ // ProfessionLicenseInfo
				BYTE *pProfessionLicenseInfo = pSpecialActivityInfo;
				Write16(pProfessionLicenseInfo, 0); // len

				Write8(pProfessionLicenseInfo, 6); // BigLevel
				Write8(pProfessionLicenseInfo, 5); // SmallLevel

				len = pProfessionLicenseInfo - pSpecialActivityInfo;
				Set16(pSpecialActivityInfo, (WORD)len);
				pSpecialActivityInfo += len;
			}

			len = pSpecialActivityInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0);	// ParaNum
		Write32(pPlayerRoomVisibleInfo, 0); // ParaList[]
		Write8(pPlayerRoomVisibleInfo, 0);	// ItemNum
		for (size_t i = 0; i < n; i++)
		{ // ItemOrder
			BYTE *pItemOrder = pPlayerRoomVisibleInfo;
			Write16(pItemOrder, 0); // len

			Write32(pItemOrder, 0); // KartID
			Write32(pItemOrder, 0); // Order
			Write32(pItemOrder, 0); // BackGroundID

			len = pItemOrder - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0);	// TDCExcluItemNum
		Write32(pPlayerRoomVisibleInfo, 0); // TDCExclusiveItemID[]
		Write8(pPlayerRoomVisibleInfo, 0);	// EquipNiChangItemNum
		for (size_t i = 0; i < n; i++)
		{ // NiChangPavilionEquipItemInfo
			BYTE *pNiChangPavilionEquipItemInfo = pPlayerRoomVisibleInfo;
			Write16(pNiChangPavilionEquipItemInfo, 0); // len

			Write32(pNiChangPavilionEquipItemInfo, 0); // ItemId
			Write16(pNiChangPavilionEquipItemInfo, 0); // CurrentEquipColorationPlanId

			len = pNiChangPavilionEquipItemInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0);	// TestKartGameType
		Write32(pPlayerRoomVisibleInfo, 0); // GameTestKartID
		Write8(pPlayerRoomVisibleInfo, 0);	// HasStarsFightingInfo
		for (size_t i = 0; i < n; i++)
		{ // StarsFightingInfo
			BYTE *pStarsFightingInfo = pPlayerRoomVisibleInfo;
			Write16(pStarsFightingInfo, 0); // len

			Write32(pStarsFightingInfo, 0); // Star
			Write32(pStarsFightingInfo, 0); // StarVal
			Write32(pStarsFightingInfo, 0); // StarsFightingStarLevel

			len = pStarsFightingInfo - pPlayerRoomVisibleInfo;
			Set16(pPlayerRoomVisibleInfo, (WORD)len);
			pPlayerRoomVisibleInfo += len;
		}
		Write8(pPlayerRoomVisibleInfo, 0); // RoomBGSetting

		len = pPlayerRoomVisibleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // ReasonLen
	Write8(p, 0); // PlayerCount2
	for (size_t i = 0; i < n; i++)
	{ // PlayerRelationInfo
		BYTE *pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); // len

		Write32(pPlayerRelationInfo, 0); // SrcUin
		Write32(pPlayerRelationInfo, 0); // RelationFlag
		Write32(pPlayerRelationInfo, 0); // RelationUin
		// WriteString(pPlayerRelationInfo, 0); //RelationNickName[]
		Write32(pPlayerRelationInfo, 0); // EngageTime
		Write32(pPlayerRelationInfo, 0); // NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); // BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); // EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0);	 // EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // KartNum
	for (size_t i = 0; i < n; i++)
	{ // KartRefitInfo
		BYTE *pKartRefitInfo = p;
		Write16(pKartRefitInfo, 0); // len

		Write32(pKartRefitInfo, 0); // Uin
		Write32(pKartRefitInfo, 0); // KartId
		Write32(pKartRefitInfo, 0); // RefitCout
		Write16(pKartRefitInfo, 0); // MaxFlags
		Write16(pKartRefitInfo, 0); // WWeight
		Write16(pKartRefitInfo, 0); // SpeedWeight
		Write16(pKartRefitInfo, 0); // JetWeight
		Write16(pKartRefitInfo, 0); // SJetWeight
		Write16(pKartRefitInfo, 0); // AccuWeight
		Write32(pKartRefitInfo, 0); // ShapeRefitCount
		Write32(pKartRefitInfo, 0); // KartHeadRefitItemID
		Write32(pKartRefitInfo, 0); // KartTailRefitItemID
		Write32(pKartRefitInfo, 0); // KartFlankRefitItemID
		Write32(pKartRefitInfo, 0); // KartTireRefitItemID
		Write32(pKartRefitInfo, 0); // SecondRefitCount
		Write16(pKartRefitInfo, 0); // Speed2Weight
		Write16(pKartRefitInfo, 0); // DriftVecWeight
		Write16(pKartRefitInfo, 0); // AdditionalZSpeedWeight
		Write16(pKartRefitInfo, 0); // AntiCollisionWeight
		Write16(pKartRefitInfo, 0); // LuckyValue
		Write16(pKartRefitInfo, 0); // RefitLuckyValueMaxWeight
		Write32(pKartRefitInfo, 0); // ShapeSuitID
		Write8(pKartRefitInfo, 0);	// LegendSuitLevel
		Write32(pKartRefitInfo, 0); // LegendSuitLevelChoice
		Write32(pKartRefitInfo, 0); // ShapeLegendSuitID

		len = pKartRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // LoverRaceOrPro
	{			  // STDynamicInfo
		BYTE *pSTDynamicInfo = p;
		Write16(pSTDynamicInfo, 0); // len

		Write8(pSTDynamicInfo, 0); // Season
		Write8(pSTDynamicInfo, 0); // Timeslot
		Write8(pSTDynamicInfo, 0); // Weather

		len = pSTDynamicInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // PreKnowMapID
	Write8(p, 0);  // HasMoreInfo
	Write32(p, 0); // MapSubID
	Write8(p, 0);  // HasFBInfo
	for (size_t i = 0; i < n; i++)
	{ // FBInfo
		BYTE *pFBInfo = p;
		Write16(pFBInfo, 0); // len

		Write8(pFBInfo, 0);	 // AvailableLev
		Write8(pFBInfo, 0);	 // Lev
		Write8(pFBInfo, 0);	 // SubLev
		Write32(pFBInfo, 0); // BossID
		Write8(pFBInfo, 0);	 // FBSeason

		len = pFBInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // GuildScoreInNextGame
	Write16(p, 0); // GuildScoreAlreadyAward
	Write16(p, 0); // GuildScoreTotalAward
	Write8(p, 0);  // SpeFlag
	Write32(p, 0); // RoomNo
	Write16(p, 0); // SvrId
	Write8(p, 0);  // RoomFlag
	Write8(p, 0);  // MaxOBNum
	Write8(p, 0);  // AllowAutoStart
	Write8(p, 0);  // TalkRoomBGID

	len = p - buf;
	SendToClient(Client, 106, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestReportCurrentOpt(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short OptType = Read16(p);
	int OptID = Read32(p);
	int SpecialValue = Read32(p);
	if (OptType == 4)
	{
		NotifySingleGameOver(Client);
		NotifySingleRaceFinish(Client);
	}
	ResponseReportCurrentOpt(Client, OptType, OptID, SpecialValue);
}

void ResponseReportCurrentOpt(ClientNode *Client, short OptType, int OptID, int SpecialValue)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write16(p, OptType);	 // OptType
	Write32(p, OptID);		 // OptNum

	len = p - buf;
	SendToClient(Client, 139, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyCustomMapList(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	{			   // CustomMapList
		BYTE *pCustomMapList = p;
		Write16(pCustomMapList, 0); // len

		Write8(pCustomMapList, 0);	// RandType
		Write8(pCustomMapList, 0);	// MapNum
		Write32(pCustomMapList, 0); // MapList[]
		Write8(pCustomMapList, 0);	// MapType

		len = pCustomMapList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 631, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestUpdateCustomMapList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	{ // CustomMapList
		BYTE *pCustomMapList = p;
		len = Read16(pCustomMapList);

		UINT RandType = Read8(pCustomMapList);
		UINT MapNum = Read8(pCustomMapList);
		for (int i = 0; i < MapNum; i++)
		{
			Read32(pCustomMapList);
		}
		UINT MapType = Read8(pCustomMapList);

		p += len;
		ResponseUpdateCustomMapList(Client);
	}
}

void ResponseUpdateCustomMapList(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID

	len = p - buf;
	SendToClient(Client, 1651, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyBroadcast(ClientNode *Client)
{
	const static unsigned char *pMsg444 = EHexToByte("006B41000100552F433030CDE6BCD22F433034A1BED3E8B3BD4D61A1BF2F433030C8CBC6B7B1ACB7A2A3ACD4DA2F433033A1B0B4BABDDABDF0B1A6CFE4A1B12F433030D6D0BBF1B5C3B3E8CEEF2F433033A1BEB7C9BBA2CDF5A1BF000000000008000000000000000000000000");

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	{ // BroadcastInfo
		BYTE *pBroadcastInfo = p;
		Write16(pBroadcastInfo, 0); // len

		Write8(pBroadcastInfo, 0);	// BCFlag
		Write16(pBroadcastInfo, 0); // ConTime
		Write16(pBroadcastInfo, 0); // ContentLen
		Write32(pBroadcastInfo, 0); // BroadcastType
		Write8(pBroadcastInfo, 0);	// DisplayFlag
		Write32(pBroadcastInfo, 0); // ID1
		Write32(pBroadcastInfo, 0); // ID2
		Write16(pBroadcastInfo, 0); // AttachLen

		len = pBroadcastInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 541, pMsg444, 109, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetWizardHomeBaseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetWizardHomeBaseInfo(Client);
}

void ResponseGetWizardHomeBaseInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write32(p, Client->Uin); // Uin
	{			   // PlayerDBWizardHomeInfo
		BYTE *pPlayerDBWizardHomeInfo = p;
		Write16(pPlayerDBWizardHomeInfo, 0); // len

		{ // WizardHomeInfo
			BYTE *pWizardHomeInfo = pPlayerDBWizardHomeInfo;
			Write16(pWizardHomeInfo, 0); // len

			Write32(pWizardHomeInfo, 99999); // ProsperityDegree
			Write8(pWizardHomeInfo, 10);	 // FarmLevel
			Write8(pWizardHomeInfo, 10);	 // CollegeLevel
			Write8(pWizardHomeInfo, 10);	 // WorkShopLevel
			Write32(pWizardHomeInfo, 0); // PrayerTreeExp
			Write8(pWizardHomeInfo, 10);	 // PrayerTreeLevel

			len = pWizardHomeInfo - pPlayerDBWizardHomeInfo;
			Set16(pPlayerDBWizardHomeInfo, (WORD)len);
			pPlayerDBWizardHomeInfo += len;
		}
		{ // FriendWateringInfoList
			BYTE *pFriendWateringInfoList = pPlayerDBWizardHomeInfo;
			Write16(pFriendWateringInfoList, 0); // len

			Write8(pFriendWateringInfoList, 0); // FriendNum
			for (size_t i = 0; i < 0; i++)
			{ // FriendWateringInfo
				BYTE *pFriendWateringInfo = pFriendWateringInfoList;
				Write16(pFriendWateringInfo, 0); // len

				Write32(pFriendWateringInfo, 0); // Uin
				Write32(pFriendWateringInfo, 0); // LastFriendWateringTime
				Write8(pFriendWateringInfo, 0);	 // FriendWateringCountOneDay

				len = pFriendWateringInfo - pFriendWateringInfoList;
				Set16(pFriendWateringInfoList, (WORD)len);
				pFriendWateringInfoList += len;
			}

			len = pFriendWateringInfoList - pPlayerDBWizardHomeInfo;
			Set16(pPlayerDBWizardHomeInfo, (WORD)len);
			pPlayerDBWizardHomeInfo += len;
		}
		Write8(pPlayerDBWizardHomeInfo, 0);	 // JobCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastJobTime
		Write16(pPlayerDBWizardHomeInfo, 0); // PrayerTreeWateringCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastPrayerTreeWateringTime
		Write8(pPlayerDBWizardHomeInfo, 0);	 // PrayerTreeFriendWateringCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastPrayerTreeFriendWateringTime
		Write16(pPlayerDBWizardHomeInfo, 0); // CliffordCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastCliffordTime
		Write16(pPlayerDBWizardHomeInfo, 0); // FeedCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastFeedTime
		Write16(pPlayerDBWizardHomeInfo, 0); // AdventureCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastAdventureTime
		Write16(pPlayerDBWizardHomeInfo, 0); // ChallengeCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastChallengeTime
		Write8(pPlayerDBWizardHomeInfo, 0);	 // FarmStatus
		Write32(pPlayerDBWizardHomeInfo, 0); // FarmUpgradeEndTime
		Write8(pPlayerDBWizardHomeInfo, 0);	 // CollegeStatus
		Write32(pPlayerDBWizardHomeInfo, 0); // CollegeUpgradeEndTime
		Write8(pPlayerDBWizardHomeInfo, 0);	 // WorkShopStatus
		Write32(pPlayerDBWizardHomeInfo, 0); // WorkShopUpgradeEndTime
		Write16(pPlayerDBWizardHomeInfo, 0); // PrayerTreeBeWateredByFriendCount
		Write32(pPlayerDBWizardHomeInfo, 0); // LastPrayerTreeBeWateredByFriendTime
		Write8(pPlayerDBWizardHomeInfo, 0);	 // LuckyFruitLevel
		Write32(pPlayerDBWizardHomeInfo, 0); // JobProsperityScore
		{									 // DressInfo
			BYTE *pDressInfo = pPlayerDBWizardHomeInfo;
			Write16(pDressInfo, 0); // len

			Write8(pDressInfo, 0); // ColorSetLen
			Write8(pDressInfo, 0); // DoorPlateLen

			len = pDressInfo - pPlayerDBWizardHomeInfo;
			Set16(pPlayerDBWizardHomeInfo, (WORD)len);
			pPlayerDBWizardHomeInfo += len;
		}

		len = pPlayerDBWizardHomeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 4); // WizardNum
	for (size_t i = 0; i < 4; i++)
	{ // PlayerDBWizardBaseInfo
		BYTE *pPlayerDBWizardBaseInfo = p;
		Write16(pPlayerDBWizardBaseInfo, 0); // len

		{ // WizardBaseInfo
			BYTE *pWizardBaseInfo = pPlayerDBWizardBaseInfo;
			Write16(pWizardBaseInfo, 0); // len

			Write16(pWizardBaseInfo, 54644);	 // WizardID
			Write16(pWizardBaseInfo, i);	 // WizardType
			WriteString(pWizardBaseInfo, "小东西",16); // NickName[]
			Write32(pWizardBaseInfo, 99999);	 // Experience
			Write8(pWizardBaseInfo, 100);		 // Level
			Write8(pWizardBaseInfo, 10);		 // Order
			Write32(pWizardBaseInfo, 0);	 // Health
			Write8(pWizardBaseInfo, 1);		 // IsRare
			Write8(pWizardBaseInfo, 10000);		 // Intimacy
			Write8(pWizardBaseInfo, 0);		 // IsHuanHua
			Write8(pWizardBaseInfo, 1);		 // IsShow
			Write8(pWizardBaseInfo, 1);		 // IsCarry
			Write8(pWizardBaseInfo, 1);		 // MaxSkillNum
			{								 // WizardSkillInfoList
				BYTE *pWizardSkillInfoList = pWizardBaseInfo;
				Write16(pWizardSkillInfoList, 0); // len

				Write8(pWizardSkillInfoList, 1); // SkillNum
				for (size_t i = 0; i < 1; i++)
				{ // SkillInfo
					BYTE *pSkillInfo = pWizardSkillInfoList;
					Write16(pSkillInfo, 0); // len

					Write8(pSkillInfo, 1); // SkillId
					Write8(pSkillInfo, 1); // SkillLevel

					len = pSkillInfo - pWizardSkillInfoList;
					Set16(pWizardSkillInfoList, (WORD)len);
					pWizardSkillInfoList += len;
				}

				len = pWizardSkillInfoList - pWizardBaseInfo;
				Set16(pWizardBaseInfo, (WORD)len);
				pWizardBaseInfo += len;
			}
			Write8(pWizardBaseInfo, 0);	 // Status
			Write32(pWizardBaseInfo, 0); // EndTime
			Write32(pWizardBaseInfo, 0); // LeftTime
			Write8(pWizardBaseInfo, 0);	 // EvolutionBranch
			Write16(pWizardBaseInfo, 0); // CurAdventureMapID
			Write8(pWizardBaseInfo, 0);	 // LastunLockMapWizardLev
			Write16(pWizardBaseInfo, 0); // Flag
			Write8(pWizardBaseInfo, 0);	 // JobType

			len = pWizardBaseInfo - pPlayerDBWizardBaseInfo;
			Set16(pPlayerDBWizardBaseInfo, (WORD)len);
			pPlayerDBWizardBaseInfo += len;
		}
		Write32(pPlayerDBWizardBaseInfo, 0); // LastActionTime
		Write32(pPlayerDBWizardBaseInfo, 0); // LastCareTime
		Write16(pPlayerDBWizardBaseInfo, 0); // CareCount
		Write32(pPlayerDBWizardBaseInfo, 0); // LastFeedTime
		Write8(pPlayerDBWizardBaseInfo, 0);	 // FeedCount
		Write32(pPlayerDBWizardBaseInfo, 0); // LastFeedFruitTime
		Write16(pPlayerDBWizardBaseInfo, 0); // FeedFruitCount
		Write8(pPlayerDBWizardBaseInfo, 0);	 // ComputeType

		len = pPlayerDBWizardBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // FarmStatus
	Write32(p, 0); // FarmUpgradeLeftTime
	Write8(p, 0);  // CollegeStatus
	Write32(p, 0); // CollegeUpgradeLeftTime
	Write8(p, 0);  // WorkShopStatus
	Write32(p, 0); // WorkShopUpgradeLeftTime
	Write8(p, 0);  // HasLuckyFruitToReceive
	Write32(p, 0); // CurServerTime

	len = p - buf;
	SendToClient(Client, 11474, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyBorderPlayerInfo(ClientNode* Client)
{
	RoomNode* BorderRoom = GetRoom(Client->BorderRoomID);
	if (!BorderRoom)
	{
		return;
	}
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	Write32(p, BorderRoom->BaseGameMode); //BaseMode
	Write32(p, BorderRoom->SubGameMode); //SubMode
	Write8(p, BorderRoom->CurrentPlayerNum); //PlayerCount
	for (size_t i = 0; i < BorderRoom->CurrentPlayerNum; i++)
	{ //PlayerRoomVisibleInfo
		BYTE* pPlayerRoomVisibleInfo = BorderOnePlayerInfo;
		pPlayerRoomVisibleInfo += 2;
		Write32(pPlayerRoomVisibleInfo, BorderRoom->Player[i]->Uin); //Uin
		BYTE* pBack = pPlayerRoomVisibleInfo;
		pPlayerRoomVisibleInfo += 9;
		WritePlayerDBBaseInfo(pPlayerRoomVisibleInfo, BorderRoom->Player[i]->Uin);
		pPlayerRoomVisibleInfo = pBack;
		pPlayerRoomVisibleInfo += 350;
		//装备车辆
		if (BorderRoom->Player[i]->KartID != 0)
			Write32(pPlayerRoomVisibleInfo, BorderRoom->Player[i]->KartID);
		else
			Write32(pPlayerRoomVisibleInfo, BorderRoom->Player[i]->KartID);
		memcpy(p, BorderOnePlayerInfo, 805);
		p += 805;
	}
	Write16(p, BorderRoom->CurrentPlayerNum); //KartNum车辆显示信息
	for (size_t i = 0; i < BorderRoom->CurrentPlayerNum; i++)
	{ //KartRefitInfo
		BYTE* pKartRefitInfo = p;
		Write16(pKartRefitInfo, 0); //len

		Write32(pKartRefitInfo, BorderRoom->Player[i]->Uin); //Uin
		if (BorderRoom->Player[i]->KartID != 0)
			Write32(pKartRefitInfo, BorderRoom->Player[i]->KartID); //KartId
		else
			Write32(pKartRefitInfo, BorderRoom->Player[i]->KartID); //KartId
		Write32(pKartRefitInfo, 0); //RefitCout
		Write16(pKartRefitInfo, 0); //MaxFlags
		Write16(pKartRefitInfo, 0); //WWeight
		Write16(pKartRefitInfo, 0); //SpeedWeight
		Write16(pKartRefitInfo, 0); //JetWeight
		Write16(pKartRefitInfo, 0); //SJetWeight
		Write16(pKartRefitInfo, 0); //AccuWeight
		Write32(pKartRefitInfo, 0); //ShapeRefitCount
		Write32(pKartRefitInfo, 0); //KartHeadRefitItemID
		Write32(pKartRefitInfo, 0); //KartTailRefitItemID
		Write32(pKartRefitInfo, 0); //KartFlankRefitItemID
		Write32(pKartRefitInfo, 0); //KartTireRefitItemID
		Write32(pKartRefitInfo, 0); //SecondRefitCount
		Write16(pKartRefitInfo, 0); //Speed2Weight
		Write16(pKartRefitInfo, 0); //DriftVecWeight
		Write16(pKartRefitInfo, 0); //AdditionalZSpeedWeight
		Write16(pKartRefitInfo, 0); //AntiCollisionWeight
		Write16(pKartRefitInfo, 0); //LuckyValue
		Write16(pKartRefitInfo, 0); //RefitLuckyValueMaxWeight
		Write32(pKartRefitInfo, 0); //ShapeSuitID
		Write8(pKartRefitInfo, 0); //LegendSuitLevel
		Write32(pKartRefitInfo, 0); //LegendSuitLevelChoice
		Write32(pKartRefitInfo, 0); //ShapeLegendSuitID

		len = pKartRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	if (Client->TaskID == 0xFFFD)
	{
		memcpy(p, "\x00\x00\x00\x00\x00", 5);
		p += 5;
	}
	else if (Client->TaskID == 0xFFFC)
	{
		memcpy(p, "\x00\x00\x01\x00\x1e\x00\x01\x98\x99\x00\x01\x98\xa3\x00\x01\x98\x99\x00\x01\x98\xa3\x00\x00\x00\x00\x00\x00\x27\x3d\x00\x00\x27\x30\x00\x00", 35);
		p += 35;
	}

	len = p - buf;

	SendToClient(Client, 548, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);

}
void RequestRankedMatchDoBanMap(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT MapID = Read32(p);
	UCHAR Type = Read8(p);
	UCHAR CurrentRound = Read8(p);
	/*RoomNode* BorderRoom = GetRoom(Client->BorderRoomID);
	BorderRoom->MapID = MapID;*/
	ResponseRankedMatchDoBanMap(Client,Type);
}
void ResponseRankedMatchDoBanMap(ClientNode* Client,UCHAR Type)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write8(p, Type); //Type
	Write8(p, 0); //BuffNum
	for (size_t i = 0; i < 0; i++)
	{ //BuffIDListInfo
		BYTE* pBuffIDListInfo = p;
		Write16(pBuffIDListInfo, 0); //len

		Write32(pBuffIDListInfo, 0); //BuffID
		Write8(pBuffIDListInfo, 0); //Lev

		len = pBuffIDListInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 25010, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyRankedMatchBanMapResult(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //MapNum
	for (size_t i = 0; i < 0; i++)
	{ //MapInfo
		BYTE* pMapInfo = p;
		Write16(pMapInfo, 0); //len

		Write32(pMapInfo, 0); //MapID
		Write32(pMapInfo, 0); //Status
		Write8(pMapInfo, 0); //ThemeMapOrNot
		Write8(pMapInfo, 0); //SeasonNewMapOrNot
		Write8(pMapInfo, 0); //PinnacleBattleMapOrNot
		Write16(pMapInfo, 0); //TotalBeBannedNums
		Write16(pMapInfo, 0); //FinalRandBanMapType

		len = pMapInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 25011, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyRankedMatchBanMapInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	{ //BanMapCfg
		BYTE* pBanMapCfg = p;
		Write16(pBanMapCfg, 0); //len

		Write32(pBanMapCfg, 0); //GradeLevelLimit
		Write32(pBanMapCfg, 8); //ChooseTime
		Write32(pBanMapCfg, 8); //ShowTime
		Write32(pBanMapCfg, 8); //TotalMapNum
		Write32(pBanMapCfg, 8); //ChooseMapNum
		Write32(pBanMapCfg, 0); //BonusScore
		Write32(pBanMapCfg, 0); //PinnacleBattleTotalMapNum
		Write32(pBanMapCfg, 0); //PinnacleBattleChooseMapNum

		len = pBanMapCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}		
	Write32(p, 8); //MapNum
	for (size_t i = 0; i < 8; i++)
	{ //MapInfo
		BYTE* pMapInfo = p;
		Write16(pMapInfo, 0); //len
		
		UINT MID = RoomMapID2GameMapID(16);
		if (i == 7) {
			Write32(pMapInfo, 0); //MapID
		}
		else {
			Write32(pMapInfo, MID); //MapID
		}
		
		Write32(pMapInfo, 0); //Status
		Write8(pMapInfo, 0); //ThemeMapOrNot
		Write8(pMapInfo, 0); //SeasonNewMapOrNot
		Write8(pMapInfo, 0); //PinnacleBattleMapOrNot
		Write16(pMapInfo, 0); //TotalBeBannedNums
		Write16(pMapInfo, 0); //FinalRandBanMapType

		len = pMapInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //Type

	len = p - buf;
	SendToClient(Client, 25009, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void WINAPI OnBorderRace(RoomNode* Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			NotifyRaceBegin(RoomClient);
		}
	}
}
void WINAPI OnBorderGame(RoomNode* Room)
{
	if (Room->CurrentPlayerNum < 6) {
		Room->OpenNpc = true;
		Room->NPCNum = 6- Room->CurrentPlayerNum;
	}
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			NotifyGameBegin(RoomClient, Room->Player);
			NotifyGameNPCInfo(RoomClient);
			NotifyKartPhysParam(RoomClient, NULL);
			if (RoomClient->matchQueueType == 1) {
				NotifyOtherKartStoneInfo(RoomClient);
			}
			
		}
	}
	//跳舞并同步加载
	CreateRoomTimer(Room, 9000, OnBorderRace);
}
void WINAPI OnNotifyPlayerInfo(RoomNode* Room)
{
	int time = 4000;
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			NotifyBorderPlayerInfo(RoomClient);
			if (RoomClient->TaskID == 0xFFFC)
			{
				time = 8000;
				NotifyRankedMatchBanMapInfo(RoomClient);//排位禁图

			}
		}
	}
	//开始游戏
	CreateRoomTimer(Room, time, OnBorderGame);
}
void WINAPI OnMatchResult(RoomNode* Room)
{
	Room->Flag = 0x1;//标志该房间不能进入
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			NotifyMatchResult(RoomClient, Room->matchQueueType);
		}
	}
	//等待EnterMatchRoom 并发送NotifyBorderPlayerInfo
	CreateRoomTimer(Room, 3000, OnNotifyPlayerInfo);
}
void ResponseStartBorderGame(ClientNode* Client, USHORT matchQueueType)
{
	InitClientGameArrt(Client);
	ResponseAddToMatchQueue(Client, matchQueueType);
	RoomNode* BorderRoom = GetRoom(GlobalBorderRoomID);
	if (!BorderRoom || BorderRoom->CurrentPlayerNum == 6 || BorderRoom->Flag != 0 || BorderRoom->matchQueueType != matchQueueType)//如果房间不存在或者人数满了或者开始了游戏就新建否则直接加入房间
	{
		//printf("NewBorderRoom\n");
		BorderRoom = NewRoom();
		BorderRoom->ShowFlag = 1;
		GlobalBorderRoomID = BorderRoom->ID;//新建一个边境房间 并记录新的可用房间号
		if (matchQueueType == 0)
		{
			BorderRoom->BaseGameMode = BorderRoom->SubGameMode = BorderRoom->BorderFlag = 1;
		}
		else if (matchQueueType == 0xa)
		{
			BorderRoom->BaseGameMode = 0x28;
			BorderRoom->SubGameMode = BorderRoom->BorderFlag = 1;
		}
		else {
			BorderRoom->BaseGameMode = BorderRoom->SubGameMode = BorderRoom->BorderFlag = 1;
		}

		//随机地图
		BorderRoom->matchQueueType = matchQueueType;
		if (Client->MapID > 100) {
			BorderRoom->MapID = RoomMapID2GameMapID(Client->MapID);
		}
		else {
			BorderRoom->MapID = RoomMapID2GameMapID(0);
		}
		

		//建立成功后 开始计时等待其他用户加入并 通知匹配结果
		CreateRoomTimer(BorderRoom, 6000, OnMatchResult);
	}

	//填充客户端地图信息
	Map* pMap = GetMap(BorderRoom->MapID);
	if (matchQueueType == 0)
		Client->TaskID = 0xFFFD;
	else if (matchQueueType == 0xa)
		Client->TaskID = 0xFFFC;
	Client->GameID = 2;
	Client->IsReady = TRUE;
	if (pMap)
	{
		Client->MapCompatibilityMode = true;
		Client->EndCheckPoint = pMap->End;
		Client->TotalRound = pMap->Round;
		Client->Loop = pMap->Loop;
	}
	else
	{
		Client->MapCompatibilityMode = true;
		Client->EndCheckPoint = 0;
		Client->TotalRound = 1;
		Client->Loop = false;
	}
	Client->MapID = BorderRoom->MapID;
	Client->FinishTime = 0;
	Client->Round = 0;
	Client->PassedCheckPoint = 0;
	Client->BorderRoomID = BorderRoom->ID;
	//加入房间
	for (char i = 0; i < 6; i++)
	{
		if (!BorderRoom->Player[i])//空位
		{

			BorderRoom->Player[i] = Client;
			BorderRoom->CurrentPlayerNum++;
			break;
		}
	}


}
void RequestChangeOBState(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	bool OBState = Read8(p);
	ResponseChangeOBState(Client,OBState);
}

void ResponseChangeOBState(ClientNode* Client, UINT OBState)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write8(p, OBState); //OBState
	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 134, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyChangeOBState(ClientNode* Client, UINT OBState,UINT Uin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	WriteUn(p, Uin); //Uin
	Write8(p, OBState); //OBState

	len = p - buf;
	SendToClient(Client, 609, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetQuickRaceTimeRandMap(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetQuickRaceTimeRandMap(Client);
}

void ResponseGetQuickRaceTimeRandMap(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	WriteUn(p, Client->Uin); //Uin
	WriteUn(p, 0); //Time
	Write8(p, 3); //QuickRaceRandMapNum
	Write32(p, RoomMapID2GameMapID(0)); //QuickRaceRandMap[]
	Write32(p, RoomMapID2GameMapID(0)); //QuickRaceRandMap[]
	Write32(p, RoomMapID2GameMapID(0)); //QuickRaceRandMap[]
	Write8(p, 10000); //QuickRaceRandMapRefreshTime
	Write16(p, 0); //SeasonMapNums
	for (size_t i = 0; i < 0; i++)
	{ //MapInfos
		BYTE* pMapInfos = p;
		Write16(pMapInfos, 0); //len

		Write16(pMapInfos, 1); //GradeMapNums
		for (size_t i = 0; i < 1; i++)
		{ //MapInfo
			BYTE* pMapInfo = pMapInfos;
			Write16(pMapInfo, 0); //len

			Write16(pMapInfo, 27); //GradeLevel
			Write16(pMapInfo, 1); //MapNums
			Write32(pMapInfo, 200); //MapArray[]
			Write16(pMapInfo, 1); //SuperRaceMapNums
			Write32(pMapInfo, 200); //SuperRaceMapArray[]
			Write16(pMapInfo, 1); //CarFactoryMapNums
			Write32(pMapInfo, 200); //CarFactoryMapArray[]
			Write16(pMapInfo, 1); //NewMapNums
			Write32(pMapInfo, 200); //NewMapArray[]

			len = pMapInfo - pMapInfos;
			Set16(pMapInfos, (WORD)len);
			pMapInfos += len;
		}

		len = pMapInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 28379, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyChangeRoomOwner(ClientNode* Client,UINT RoomID, UINT NewRoomOwnerID,UINT ChangeType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, RoomID); //RoomID
	Write16(p, NewRoomOwnerID); //NewRoomOwnerID
	Write8(p, 0); //HasFBInfo
	for (size_t i = 0; i < 0; i++)
	{ //FBInfo
		BYTE* pFBInfo = p;
		Write16(pFBInfo, 0); //len

		Write8(pFBInfo, 0); //AvailableLev
		Write8(pFBInfo, 0); //Lev
		Write8(pFBInfo, 0); //SubLev
		Write32(pFBInfo, 0); //BossID
		Write8(pFBInfo, 0); //FBSeason

		len = pFBInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, NewRoomOwnerID); //OriRoomOwnerID
	Write8(p, ChangeType); //ChangeType
	Write16(p, 0); //RoomType

	len = p - buf;
	SendToClient(Client, 524, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestChangeRoomOwner(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT NewOwnerUIN = Read32(p);
	ResponseChangeRoomOwner(Client, Uin, NewOwnerUIN);
	RoomNode* Room = GetRoom(Client->RoomID);
	ClientNode* NewOwnerNode = GetClient(NewOwnerUIN);
	if (!NewOwnerNode) {
		ResponseStartGame(Client,"该玩家不在了");
		return;
	}
	Room->RoomOwnerID = NewOwnerNode->ConnID;
	Client->IsReady = 1;
	NotifyChangeRoomOwner(Client, Room->ID, NewOwnerNode->ConnID, 0);
	for (size_t i = 0; i < 6; i++) {
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient&& RoomClient!=Client) {
			NotifyChangeRoomOwner(RoomClient, Room->ID, NewOwnerNode->ConnID,1);
		}
	}
}

void ResponseChangeRoomOwner(ClientNode* Client, UINT Uin, UINT NewOwner)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Uin); //Uin
	Write32(p, NewOwner); //NewOwner
	Write16(p, 0); //ResultID
	WriteString(p, "转让成功");

	len = p - buf;
	SendToClient(Client, 713, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}