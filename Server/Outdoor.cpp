#include "Outdoor.h"

#include <stdio.h>
#include <string>
#include <time.h>
#include "Database.h"

constexpr int MAX_CHAIR_NUM = 200;
constexpr int MAX_NUM_IN_EACHCHAIR = 2;

ChatRoomNode ChatRooms[ChatRoomNum];
void InitChatRoom()
{

	ChatRooms[0].ID = 0;
	ChatRooms[0].Name = "琳琅湾";
	ChatRooms[0].CurrPlayerNum = 0;
	ChatRooms[0].MaxPlayerNum = 99;
	ChatRooms[0].RoomType = 1;
	ChatRooms[0].SceneID = 109;
	ChatRooms[0].x = 0;
	ChatRooms[0].y = -50000;
	ChatRooms[0].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[0].Player[i] = NULL;
	}

	ChatRooms[1].ID = 1;
	ChatRooms[1].Name = "琳琅镇";
	ChatRooms[1].CurrPlayerNum = 0;
	ChatRooms[1].MaxPlayerNum = 99;
	ChatRooms[1].RoomType = 2;
	ChatRooms[1].SceneID = 25;
	ChatRooms[1].x = 0;
	ChatRooms[1].y = 0;
	ChatRooms[1].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[1].Player[i] = NULL;
	}

	ChatRooms[2].ID = 2;
	ChatRooms[2].Name = "单身派对";
	ChatRooms[2].CurrPlayerNum = 0;
	ChatRooms[2].MaxPlayerNum = 99;
	ChatRooms[2].RoomType = 3;
	ChatRooms[2].SceneID = 109;
	ChatRooms[2].x = 0;
	ChatRooms[2].y = -40000;
	ChatRooms[2].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[2].Player[i] = NULL;
	}

	ChatRooms[3].ID = 3;
	ChatRooms[3].Name = "皇族休闲区";
	ChatRooms[3].CurrPlayerNum = 0;
	ChatRooms[3].MaxPlayerNum = 99;
	ChatRooms[3].RoomType = 4;
	ChatRooms[3].SceneID = 136;
	ChatRooms[3].x = 10000;
	ChatRooms[3].y = 0;
	ChatRooms[3].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[3].Player[i] = NULL;
	}

	ChatRooms[4].ID = 4;
	ChatRooms[4].Name = "3d房间";
	ChatRooms[4].CurrPlayerNum = 0;
	ChatRooms[4].MaxPlayerNum = 99;
	ChatRooms[4].RoomType = 3;
	ChatRooms[4].SceneID = 400;
	ChatRooms[4].x = 0;
	ChatRooms[4].y = 0;
	ChatRooms[4].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[4].Player[i] = NULL;
	}
	ChatRooms[5].ID = 5;
	ChatRooms[5].Name = "赛车场";
	ChatRooms[5].CurrPlayerNum = 0;
	ChatRooms[5].MaxPlayerNum = 99;
	ChatRooms[5].RoomType = 3;
	ChatRooms[5].SceneID = 17;
	ChatRooms[5].x = 0;
	ChatRooms[5].y = 0;
	ChatRooms[5].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[5].Player[i] = NULL;
	}
	ChatRooms[6].ID = 6;
	ChatRooms[6].Name = "赛车训练场";
	ChatRooms[6].CurrPlayerNum = 0;
	ChatRooms[6].MaxPlayerNum = 99;
	ChatRooms[6].RoomType = 3;
	ChatRooms[6].SceneID = 9;
	ChatRooms[6].x = 0;
	ChatRooms[6].y = 0;
	ChatRooms[6].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[6].Player[i] = NULL;
	}
	ChatRooms[7].ID = 7;
	ChatRooms[7].Name = "风车岛";
	ChatRooms[7].CurrPlayerNum = 0;
	ChatRooms[7].MaxPlayerNum = 99;
	ChatRooms[7].RoomType = 3;
	ChatRooms[7].SceneID = 110;
	ChatRooms[7].x = 0;
	ChatRooms[7].y = -50000;
	ChatRooms[7].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[7].Player[i] = NULL;
	}
	ChatRooms[8].ID = 8;
	ChatRooms[8].Name = "结婚礼堂";
	ChatRooms[8].CurrPlayerNum = 0;
	ChatRooms[8].MaxPlayerNum = 99;
	ChatRooms[8].RoomType = 3;
	ChatRooms[8].SceneID = 123;
	ChatRooms[8].x = 10000;
	ChatRooms[8].y = 0;
	ChatRooms[8].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[8].Player[i] = NULL;
	}
	ChatRooms[9].ID = 9;
	ChatRooms[9].Name = "天空城";
	ChatRooms[9].CurrPlayerNum = 0;
	ChatRooms[9].MaxPlayerNum = 99;
	ChatRooms[9].RoomType = 3;
	ChatRooms[9].SceneID = 152;
	ChatRooms[9].x = 0;
	ChatRooms[9].y = 0;
	ChatRooms[9].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[9].Player[i] = NULL;
	}
	ChatRooms[10].ID = 10;
	ChatRooms[10].Name = "后花园";
	ChatRooms[10].CurrPlayerNum = 0;
	ChatRooms[10].MaxPlayerNum = 99;
	ChatRooms[10].RoomType = 3;
	ChatRooms[10].SceneID = 193;
	ChatRooms[10].x = 0;
	ChatRooms[10].y = 0;
	ChatRooms[10].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[10].Player[i] = NULL;
	}
	ChatRooms[11].ID = 11;
	ChatRooms[11].Name = "比赛场地";
	ChatRooms[11].CurrPlayerNum = 0;
	ChatRooms[11].MaxPlayerNum = 99;
	ChatRooms[11].RoomType = 3;
	ChatRooms[11].SceneID = 213;
	ChatRooms[11].x = 0;
	ChatRooms[11].y = 0;
	ChatRooms[11].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[11].Player[i] = NULL;
	}
	ChatRooms[12].ID = 12;
	ChatRooms[12].Name = "花园";
	ChatRooms[12].CurrPlayerNum = 0;
	ChatRooms[12].MaxPlayerNum = 99;
	ChatRooms[12].RoomType = 3;
	ChatRooms[12].SceneID = 236;
	ChatRooms[12].x = 0;
	ChatRooms[12].y = 0;
	ChatRooms[12].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[12].Player[i] = NULL;
	}
	ChatRooms[13].ID = 13;
	ChatRooms[13].Name = "城邦";
	ChatRooms[13].CurrPlayerNum = 0;
	ChatRooms[13].MaxPlayerNum = 99;
	ChatRooms[13].RoomType = 3;
	ChatRooms[13].SceneID = 237;
	ChatRooms[13].x = -50000;
	ChatRooms[13].y = -50000;
	ChatRooms[13].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[13].Player[i] = NULL;
	}
	ChatRooms[14].ID = 14;
	ChatRooms[14].Name = "3D舞蹈房间";
	ChatRooms[14].CurrPlayerNum = 0;
	ChatRooms[14].MaxPlayerNum = 99;
	ChatRooms[14].RoomType = 3;
	ChatRooms[14].SceneID = 401;
	ChatRooms[14].x = 0;
	ChatRooms[14].y = 0;
	ChatRooms[14].z = 0;
	for (size_t i = 0; i < 99; i++)
	{
		ChatRooms[14].Player[i] = NULL;
	}
}

void NotifyNPCInfo(ClientNode *Client);
void NotifyChairInfo(ClientNode *Client);
void ResponseEnterOutdoor(ClientNode *Client, short SceneID, int x, int y, int z);
void NotifyGuildRoomMoreInfo(ClientNode *Client);
void ResponseChangeChairState(ClientNode *Client, BYTE ChangeState, int ChairIndx, BYTE SeatIndx);
void NotifyEnterOutdoor(ClientNode *Client, ChatRoomNode *ChatRoom, ClientNode *RoomClient, char SeatID);

ChatRoomNode *GetChatRoom(short ChatRoomID)
{
	ChatRoomNode *ChatRoom = NULL;
	for (size_t i = 0; i < ChatRoomNum; i++)
	{
		if (ChatRooms[i].ID == ChatRoomID)
		{
			ChatRoom = &ChatRooms[i];
			break;
		}
	}
	return ChatRoom;
}
void RequestEnterOutdoor(ClientNode *Client, BYTE *Body, size_t BodyLen, int RoomID)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short SceneID = Read16(Body);
	short ChatRoomID = Read16(Body);
	short NPCCfgVer = Read16(Body);
	// char[] Password[] = ReadString(p);
	char Password[16];
	memcpy(Password, Body, 16);
	Body += 16;
	BYTE InviteType = Read8(Body);
	UINT InviterUin = Read32(Body);
	UINT ui3DRoomRoleType = Read32(Body);
	SceneID = 0;
	RoomNode *Room = GetRoom(ChatRoomID);
	if (ChatRoomID > 1000)
	{
		Client->RoomID = ChatRoomID;
		Client->ChatRoomID = ChatRoomID;
		ChatRoomID = 4;
	}
	// ChatRoomID = RoomID;
	/*if (RoomID > ChatRoomNum - 1) {
		ChatRoomID = 5;
	}*/
	if (Client->RoomID > 1000)
	{
		LeaveRoom(Client, 0);

		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient == NULL)
			{
				Client->RoomID = RoomID;
				ResponseEnterRoom(Client, Room, i);
				Room->Player[i] = Client;
				Room->CurrentPlayerNum++;
				// 通知其它玩家
				for (char i2 = 0; i2 < 6; i2++)
				{
					ClientNode *RoomClient = Room->Player[i2];

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
	LeaveOutdoor(Client);
	Z_INFO("场景ID:%d, 聊天室ID:%d\n", SceneID, ChatRoomID);

	ChatRoomNode *ChatRoom = GetChatRoom(ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (!ChatRoom->Player[i])
		{
			Client->ChatRoomID = ChatRoomID;
			ChatRoom->Player[i] = Client;
			ChatRoom->CurrPlayerNum++;
			/*if (RoomID > ChatRoomNum - 1) {
				SceneID = RoomID;
			}*/
			/*else {

			}*/
			SceneID = ChatRoom->SceneID;

			int x = ChatRoom->x;
			int y = ChatRoom->y;
			int z = ChatRoom->z;
			Z_INFO("场景ID:%d\n", SceneID);

			NotifyNPCInfo(Client);
			ResponseEnterOutdoor(Client, SceneID, x, y, z);
			NotifyGuildRoomMoreInfo(Client);
			NotifyChairInfo(Client);

			// 通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode *RoomClient = ChatRoom->Player[i2];
				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterOutdoor(RoomClient, ChatRoom, Client, i);
				}
			}

			break;
		}
	}
}
void RequestEnterOutdoor2(ClientNode *Client, int RoomID, int chatRoomID)
{

	UINT Uin = Client->Uin;

	short SceneID = 400;
	short ChatRoomID = Client->RoomID;
	short NPCCfgVer = 0;
	SceneID = 0;

	if (ChatRoomID > 1000)
	{
		Client->RoomID = ChatRoomID;
		ChatRoomID = chatRoomID;
	}

	// ChatRoomID = RoomID;
	/*if (RoomID > ChatRoomNum - 1) {
		ChatRoomID = 5;
	}*/
	LeaveOutdoor(Client);
	Z_INFO("场景ID:%d, 聊天室ID:%d\n", SceneID, ChatRoomID);

	ChatRoomNode *ChatRoom = GetChatRoom(ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (!ChatRoom->Player[i])
		{
			Client->ChatRoomID = ChatRoomID;
			ChatRoom->Player[i] = Client;
			ChatRoom->CurrPlayerNum++;
			/*if (RoomID > ChatRoomNum - 1) {
				SceneID = RoomID;
			}*/
			/*else {

			}*/
			SceneID = ChatRoom->SceneID;

			int x = ChatRoom->x;
			int y = ChatRoom->y;
			int z = ChatRoom->z;
			Z_INFO("场景ID:%d\n", SceneID);

			NotifyNPCInfo(Client);
			ResponseEnterOutdoor(Client, SceneID, x, y, z);
			NotifyGuildRoomMoreInfo(Client);
			NotifyChairInfo(Client);

			// 通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode *RoomClient = ChatRoom->Player[i2];
				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterOutdoor(RoomClient, ChatRoom, Client, i);
				}
			}

			break;
		}
	}
}
void ResponseEnterOutdoor(ClientNode *Client, short SceneID, int x, int y, int z)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, Client->ChatRoomID); // ChatRoomID
	Write16(p, SceneID);			// SceneID

	{ // HeroMoveInfo
		BYTE *pHeroMoveInfo = p;
		Write16(pHeroMoveInfo, 0); // len

		Write32(pHeroMoveInfo, Client->Uin); // Uin
		Write8(pHeroMoveInfo, 0);			 // WalkStatue

		Write32(pHeroMoveInfo, x); // Posion[3]
		Write32(pHeroMoveInfo, y);
		Write32(pHeroMoveInfo, z);

		Write32(pHeroMoveInfo, 1000); // Ori[9]
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 1000);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 0);
		Write32(pHeroMoveInfo, 1000);

		Write32(pHeroMoveInfo, 0); // AttachUin
		Write32(pHeroMoveInfo, 0); // WalkExtState

		Write8(pHeroMoveInfo, 0);				 // Nums
		Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
		Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
		Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
		Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
		Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
		Write32(pHeroMoveInfo, Client->Pet->ID); // EquipedPetID
		Write32(pHeroMoveInfo, 3);				 // EquipedPetStatus
		Write32(pHeroMoveInfo, 0);				 // PetPKResultStatus
		Write16(pHeroMoveInfo, 0);				 // HeroMajorState
		Write16(pHeroMoveInfo, 0);				 // HorsePetStatus

		len = pHeroMoveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); // ResultID
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	// const char RoomName[] = "休闲区房间";
	len = strlen(ChatRoom->Name);
	Write8(p, (BYTE)len); // ReasonLen
	memcpy(p, ChatRoom->Name, len);
	p += len;

	BYTE *pHeroCount = p;
	Write16(p, 0); // HeroCount

	short HeroCount = 0;
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{ // HeroInfo
			HeroCount++;

			BYTE *pHeroInfo = p;
			Write16(pHeroInfo, 0); // len

			{ // HeroMoveInfo
				BYTE *pHeroMoveInfo = pHeroInfo;
				Write16(pHeroMoveInfo, 0); // len

				Write32(pHeroMoveInfo, ChatRoom->Player[i]->Uin); // Uin
				Write8(pHeroMoveInfo, 0);						  // WalkStatue

				Write32(pHeroMoveInfo, ChatRoom->x); // Posion[3]
				Write32(pHeroMoveInfo, ChatRoom->y);
				Write32(pHeroMoveInfo, ChatRoom->z);

				Write32(pHeroMoveInfo, 1000); // Ori[9]
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 1000);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 1000);

				Write32(pHeroMoveInfo, 0);							  // AttachUin
				Write32(pHeroMoveInfo, 0);							  // WalkExtState
				Write8(pHeroMoveInfo, 0);							  // Nums
				Write32(pHeroMoveInfo, 0);							  // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);							  // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);							  // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);							  // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);							  // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, ChatRoom->Player[i]->Pet->ID); // EquipedPetID
				Write32(pHeroMoveInfo, 3);							  // EquipedPetStatus
				Write32(pHeroMoveInfo, 0);							  // PetPKResultStatus
				Write16(pHeroMoveInfo, 0);							  // HeroMajorState
				Write16(pHeroMoveInfo, 0);							  // HorsePetStatus
				len = pHeroMoveInfo - pHeroInfo;
				Set16(pHeroInfo, (WORD)len);
				pHeroInfo += len;
			}

			Write32(pHeroInfo, 0); // NextPoint
			Write16(pHeroInfo, 0); // PathLen
			// m_astHeroInfo[].m_aiPath[]

			WriteVisbleInfo(pHeroInfo, ChatRoom->Player[i]->Uin, i, ChatRoom->Player[i]->ConnID); // HeroVisbleInfo

			Write16(pHeroInfo, 0); // HeroEnterDelayTime

			len = pHeroInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	Set16(pHeroCount, HeroCount);

	Write16(p, 0); // RelationHeroCount
	for (size_t i = 0; i < 0; i++)
	{ // PlayerRelationInfo
		BYTE *pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); // len

		Write32(pPlayerRelationInfo, 0);	 // SrcUin
		Write32(pPlayerRelationInfo, 0);	 // RelationFlag
		Write32(pPlayerRelationInfo, 0);	 // RelationUin
		WriteString(pPlayerRelationInfo, 0); // RelationNickName[]
		Write32(pPlayerRelationInfo, 0);	 // EngageTime
		Write32(pPlayerRelationInfo, 0);	 // NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0);	 // BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0);	 // EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0);		 // EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // KartNum
	for (size_t i = 0; i < 0; i++)
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
	Write16(p, 0); // HammerCount
	for (size_t i = 0; i < 0; i++)
	{ // EquipedHammer
		BYTE *pEquipedHammer = p;
		Write16(pEquipedHammer, 0); // len

		Write32(pEquipedHammer, 0); // EquipedHammerID

		len = pEquipedHammer - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // StealPigCount
	for (size_t i = 0; i < 0; i++)
	{ // StealPig
		BYTE *pStealPig = p;
		Write16(pStealPig, 0); // len

		Write8(pStealPig, 0);  // Status
		Write8(pStealPig, 0);  // PigPos
		Write32(pStealPig, 0); // ItemID
		Write8(pStealPig, 0);  // PigNum

		len = pStealPig - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // MaxHeroNumInRoom
	{			   // PersonalGardenInfo
		BYTE *pPersonalGardenInfo = p;
		Write16(pPersonalGardenInfo, 0); // len

		Write32(pPersonalGardenInfo, Client->Uin);						 // OwnerUin
		WriteString(pPersonalGardenInfo, Client->NickName, MaxNickName); // NickName[]
		WriteString(pPersonalGardenInfo, Client->NickName, MaxNickName); // GardenName[]
		{																 // BaseInfo
			BYTE *pBaseInfo = pPersonalGardenInfo;
			Write16(pBaseInfo, 0); // len

			Write8(pBaseInfo, 5);	 // GardenLevel
			Write32(pBaseInfo, 888); // GardenPoint
			Write8(pBaseInfo, 888);	 // GrowRate
			Write32(pBaseInfo, 888); // GardenExpireTime
			{						 // SimpleInfo
				BYTE *pSimpleInfo = pBaseInfo;
				Write16(pSimpleInfo, 0); // len

				Write32(pSimpleInfo, 888); // WeekPopularity
				Write32(pSimpleInfo, 888); // TotalPopularity
				Write32(pSimpleInfo, 888); // LastUpdatePopularityTime
				Write8(pSimpleInfo, 0);	   // PrivateType

				len = pSimpleInfo - pBaseInfo;
				Set16(pBaseInfo, (WORD)len);
				pBaseInfo += len;
			}

			len = pBaseInfo - pPersonalGardenInfo;
			Set16(pPersonalGardenInfo, (WORD)len);
			pPersonalGardenInfo += len;
		}
		Write8(pPersonalGardenInfo, 1);	 // BuildingLevel
		Write8(pPersonalGardenInfo, 0);	 // PrivateType
		Write32(pPersonalGardenInfo, 0); // CreateTime
		{								 // KartExhibition
			BYTE *pKartExhibition = pPersonalGardenInfo;
			Write16(pKartExhibition, 0); // len

			Write16(pKartExhibition, 0); // TNum
			Write16(pKartExhibition, 0); // SNum
			Write16(pKartExhibition, 0); // ANum
			Write8(pKartExhibition, 0);	 // KartAltasForbiddenFlag
			{							 // GardenParkingList
				BYTE *pGardenParkingList = pKartExhibition;
				Write16(pGardenParkingList, 0); // len

				Write8(pGardenParkingList, 0); // ParkingNum
				for (size_t i = 0; i < 0; i++)
				{ // ParkingList
					BYTE *pParkingList = pGardenParkingList;
					Write16(pParkingList, 0); // len

					Write8(pParkingList, 0);	  // Index
					Write8(pParkingList, 0);	  // Type
					Write32(pParkingList, 0);	  // KartID
					Write32(pParkingList, 0);	  // Uin
					WriteString(pParkingList, 0); // NickName[]
					Write32(pParkingList, 0);	  // ParkingTime
					Write32(pParkingList, 0);	  // RefitCount

					len = pParkingList - pGardenParkingList;
					Set16(pGardenParkingList, (WORD)len);
					pGardenParkingList += len;
				}

				len = pGardenParkingList - pKartExhibition;
				Set16(pKartExhibition, (WORD)len);
				pKartExhibition += len;
			}
			Write32(pKartExhibition, 0); // LastReceiveAwardTime

			len = pKartExhibition - pPersonalGardenInfo;
			Set16(pPersonalGardenInfo, (WORD)len);
			pPersonalGardenInfo += len;
		}
		{ // PetExhibition
			BYTE *pPetExhibition = pPersonalGardenInfo;
			Write16(pPetExhibition, 0); // len

			Write8(pPetExhibition, 0); // ShowNum
			for (size_t i = 0; i < 0; i++)
			{ // ShowList
				BYTE *pShowList = pPetExhibition;
				Write16(pShowList, 0); // len

				Write8(pShowList, 0);  // Index
				Write8(pShowList, 0);  // Type
				Write32(pShowList, 0); // PetID
				Write32(pShowList, 0); // ShowTime
				Write16(pShowList, 0); // PetLevel
				Write8(pShowList, 0);  // Status

				len = pShowList - pPetExhibition;
				Set16(pPetExhibition, (WORD)len);
				pPetExhibition += len;
			}

			len = pPetExhibition - pPersonalGardenInfo;
			Set16(pPersonalGardenInfo, (WORD)len);
			pPersonalGardenInfo += len;
		}

		len = pPersonalGardenInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); // ServerTime
	Write8(p, 0);  // AwardGardenPopularity
	Write16(p, 0); // SuperShoeCount
	for (size_t i = 0; i < 0; i++)
	{ // EquipedSuperShoe
		BYTE *pEquipedSuperShoe = p;
		Write16(pEquipedSuperShoe, 0); // len

		Write32(pEquipedSuperShoe, 0); // EquipedSuperShoeID

		len = pEquipedSuperShoe - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HasSailTreasureInfo
	for (size_t i = 0; i < 0; i++)
	{ // SailTreasureInfo
		BYTE *pSailTreasureInfo = p;
		Write16(pSailTreasureInfo, 0); // len

		Write32(pSailTreasureInfo, 0); // SailCountWeekly
		Write32(pSailTreasureInfo, 0); // SailCountDaily_Common
		Write32(pSailTreasureInfo, 0); // WeekAwardReceiveStatus
		Write32(pSailTreasureInfo, 0); // GetTreasureBoxDaily
		Write32(pSailTreasureInfo, 0); // WorshipCountDaily

		len = pSailTreasureInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // ChatAreaId
	Write32(p, 0); // EmojiNum
	Write32(p, 0); // EmojiID[]
	Write8(p, 0);  // AdNum
	for (size_t i = 0; i < 0; i++)
	{ // Ad
		BYTE *pAd = p;
		Write16(pAd, 0); // len

		Write8(pAd, 0); // AdLen

		len = pAd - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // EnableAutoPrepare
	Write8(p, 0);  // EnableAutoStart
	WriteUn(p, 0); // aui3DRoomBoxID[]
	for (size_t i = 0; i < 0; i++)
	{ // ast3DRoomBox
		BYTE *past3DRoomBox = p;
		Write16(past3DRoomBox, 0); // len

		Write8(past3DRoomBox, 0); // AwardNum
		for (size_t i = 0; i < 0; i++)
		{ // ast3DRoomBoxAward
			BYTE *past3DRoomBoxAward = past3DRoomBox;
			Write16(past3DRoomBoxAward, 0); // len

			Write8(past3DRoomBoxAward, 0); // IsBest
			{							   // AwardToClient
				BYTE *pAwardToClient = past3DRoomBoxAward;
				Write16(pAwardToClient, 0); // len

				Write32(pAwardToClient, 0); // Exp
				Write32(pAwardToClient, 0); // Money
				Write32(pAwardToClient, 0); // SuperMoney
				Write16(pAwardToClient, 0); // ItemNum
				for (size_t i = 0; i < 0; i++)
				{ // ItemInfo
					BYTE *pItemInfo = pAwardToClient;
					Write16(pItemInfo, 0); // len

					Write32(pItemInfo, 0); // ItemID
					Write32(pItemInfo, 0); // ItemNum
					Write32(pItemInfo, 0); // AvailPeriod
					Write8(pItemInfo, 0);  // Status
					WriteUn(pItemInfo, 0); // ObtainTime
					Write32(pItemInfo, 0); // OtherAttribute
					Write16(pItemInfo, 0); // ItemType

					len = pItemInfo - pAwardToClient;
					Set16(pAwardToClient, (WORD)len);
					pAwardToClient += len;
				}
				Write32(pAwardToClient, 0); // Coupons
				Write32(pAwardToClient, 0); // GuildPoint
				Write32(pAwardToClient, 0); // LuckMoney
				Write8(pAwardToClient, 0);	// ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ // ExtendInfoAward
					BYTE *pExtendInfoAward = pAwardToClient;
					Write16(pExtendInfoAward, 0); // len

					Write16(pExtendInfoAward, 0); // Key
					Write32(pExtendInfoAward, 0); // AddValue
					Write32(pExtendInfoAward, 0); // TotalValue
					Write8(pExtendInfoAward, 0);  // ShowPriority
					Write16(pExtendInfoAward, 0); // BuyNeedScore
					Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

					len = pExtendInfoAward - pAwardToClient;
					Set16(pAwardToClient, (WORD)len);
					pAwardToClient += len;
				}
				Write32(pAwardToClient, 0); // SpeedCoin

				len = pAwardToClient - past3DRoomBoxAward;
				Set16(past3DRoomBoxAward, (WORD)len);
				past3DRoomBoxAward += len;
			}

			len = past3DRoomBoxAward - past3DRoomBox;
			Set16(past3DRoomBox, (WORD)len);
			past3DRoomBox += len;
		}

		len = past3DRoomBox - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // ChallengeTaskID
	Write8(p, 0);  // ChallengeTaskFlag
	Write8(p, 0);  // RoomType
	Write8(p, 0);  // SyncType
	Write16(p, 0); // ReportPosSpan
	Write16(p, 0); // BroadcastPosSpan
	Write8(p, 0);  // UseNewPathFind
	Write16(p, 0); // RandomGenSceneItemNum
	Write32(p, 0); // SceneItemList[]

	len = p - buf;
	// SendToClient(Client, 11019, buf, len, Client->ChatRoomID, FE_OUTDOORSVRD, Client->ConnID, Notify);
	// SendToClient(Client, 352, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	SendToClient(Client, 352, buf, len, Client->ChatRoomID, FE_OUTDOORSVRD, Client->ConnID, Response);
}

void NotifyEnterOutdoor(ClientNode *Client, ChatRoomNode *ChatRoom, ClientNode *RoomClient, char SeatID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, ChatRoom->ID);		  // ChatRoomID
	Write16(p, ChatRoom->SceneID);	  // SceneID
	Write32(p, (DWORD)time(nullptr)); // Time

	{ // HeroInfo

		BYTE *pHeroInfo = p;
		Write16(pHeroInfo, 0); // len

		{ // HeroMoveInfo
			BYTE *pHeroMoveInfo = pHeroInfo;
			Write16(pHeroMoveInfo, 0); // len

			Write32(pHeroMoveInfo, RoomClient->Uin); // Uin
			Write8(pHeroMoveInfo, 0);				 // WalkStatue

			Write32(pHeroMoveInfo, ChatRoom->x); // Posion[3]
			Write32(pHeroMoveInfo, ChatRoom->y);
			Write32(pHeroMoveInfo, ChatRoom->z);

			Write32(pHeroMoveInfo, 1000); // Ori[9]
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 1000);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 0);
			Write32(pHeroMoveInfo, 1000);

			Write32(pHeroMoveInfo, 0);					 // AttachUin
			Write32(pHeroMoveInfo, 0);					 // WalkExtState
			Write8(pHeroMoveInfo, 0);					 // Nums
			Write32(pHeroMoveInfo, 0);					 // PermitSitHorsePetUins[]
			Write32(pHeroMoveInfo, 0);					 // PermitSitHorsePetUins[]
			Write32(pHeroMoveInfo, 0);					 // PermitSitHorsePetUins[]
			Write32(pHeroMoveInfo, 0);					 // PermitSitHorsePetUins[]
			Write32(pHeroMoveInfo, 0);					 // PermitSitHorsePetUins[]
			Write32(pHeroMoveInfo, RoomClient->Pet->ID); // EquipedPetID
			Write32(pHeroMoveInfo, 3);					 // EquipedPetStatus
			Write32(pHeroMoveInfo, 0);					 // PetPKResultStatus
			Write16(pHeroMoveInfo, 0);					 // HeroMajorState
			Write16(pHeroMoveInfo, 0);					 // HorsePetStatus
			len = pHeroMoveInfo - pHeroInfo;
			Set16(pHeroInfo, (WORD)len);
			pHeroInfo += len;
		}

		Write32(pHeroInfo, 0); // NextPoint
		Write16(pHeroInfo, 0); // PathLen
		// m_astHeroInfo[].m_aiPath[]

		WriteVisbleInfo(pHeroInfo, RoomClient->Uin, 0, RoomClient->ConnID); // HeroVisbleInfo

		Write16(pHeroInfo, 0); // HeroEnterDelayTime

		len = pHeroInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	NotifyEngageResult4(RoomClient,p);
	//{ // PlayerRelationInfo
	//	BYTE *pPlayerRelationInfo = p;
	//	Write16(pPlayerRelationInfo, 0); // len

	//	Write32(pPlayerRelationInfo, RoomClient->Uin);		   // SrcUin
	//	Write32(pPlayerRelationInfo, 3);					   // RelationFlag
	//	Write32(pPlayerRelationInfo, RoomClient->Engage->UIN); // RelationUin
	//	WriteString(pPlayerRelationInfo, Client->Engage->NickName, 16);	 // NickName

	//	Write32(pPlayerRelationInfo, 0); // EngageTime
	//	Write32(pPlayerRelationInfo, 0); // NextCanBookingTimeLimit
	//	Write32(pPlayerRelationInfo, 0); // BeginHoneyMoonTime
	//	Write32(pPlayerRelationInfo, 0); // EndHoneyMoonTime

	//	Write8(pPlayerRelationInfo, 4); // EngageFlag

	//	len = pPlayerRelationInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}

	Write16(p, 0); // KartNum
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

	{ // StealPig
		BYTE *pStealPig = p;
		Write16(pStealPig, 0); // len

		Write8(pStealPig, 0);  // Status
		Write8(pStealPig, 0);  // PigPos
		Write32(pStealPig, 0); // ItemID

		len = pStealPig - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 852, buf, len, ChatRoom->ID, FE_OUTDOORSVRD, Client->ConnID, Notify);
}

void NotifyNPCInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 4); // NPCCfgVer
	Write8(p, 0);  // NPCNum

	len = p - buf;
	SendToClient(Client, 863, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyGuildRoomMoreInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // GuildID
	Write8(p, 0);  // GuildRoomID

	BYTE *pHeroCount = p;
	Write16(p, 0); // HeroCount
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	short HeroCount = 0;
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{ // HeroInfo
			HeroCount++;

			BYTE *pHeroInfo = p;
			Write16(pHeroInfo, 0); // len

			{ // HeroMoveInfo
				BYTE *pHeroMoveInfo = pHeroInfo;
				Write16(pHeroMoveInfo, 0); // len

				Write32(pHeroMoveInfo, ChatRoom->Player[i]->Uin); // Uin
				Write8(pHeroMoveInfo, 0);						  // WalkStatue

				Write32(pHeroMoveInfo, ChatRoom->x); // Posion[3]
				Write32(pHeroMoveInfo, ChatRoom->y);
				Write32(pHeroMoveInfo, ChatRoom->z);

				Write32(pHeroMoveInfo, 1000); // Ori[9]
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 1000);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 0);
				Write32(pHeroMoveInfo, 1000);

				Write32(pHeroMoveInfo, 0);				 // AttachUin
				Write32(pHeroMoveInfo, 0);				 // WalkExtState
				Write8(pHeroMoveInfo, 0);				 // Nums
				Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, 0);				 // PermitSitHorsePetUins[]
				Write32(pHeroMoveInfo, Client->Pet->ID); // EquipedPetID
				Write32(pHeroMoveInfo, 3);				 // EquipedPetStatus
				Write32(pHeroMoveInfo, 0);				 // PetPKResultStatus
				Write16(pHeroMoveInfo, 0);				 // HeroMajorState
				Write16(pHeroMoveInfo, 0);				 // HorsePetStatus
				len = pHeroMoveInfo - pHeroInfo;
				Set16(pHeroInfo, (WORD)len);
				pHeroInfo += len;
			}

			Write32(pHeroInfo, 0); // NextPoint
			Write16(pHeroInfo, 0); // PathLen
			// m_astHeroInfo[].m_aiPath[]

			WriteVisbleInfo(pHeroInfo, ChatRoom->Player[i]->Uin, i, ChatRoom->Player[i]->ConnID); // HeroVisbleInfo

			Write16(pHeroInfo, 0); // HeroEnterDelayTime

			len = pHeroInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	Set16(pHeroCount, HeroCount);

	Write16(p, 0); // RelationHeroCount
	for (size_t i = 0; i < 0; i++)
	{ // PlayerRelationInfo
		BYTE *pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); // len

		Write32(pPlayerRelationInfo, 0);	 // SrcUin
		Write32(pPlayerRelationInfo, 0);	 // RelationFlag
		Write32(pPlayerRelationInfo, 0);	 // RelationUin
		WriteString(pPlayerRelationInfo, 0); // RelationNickName[]
		Write32(pPlayerRelationInfo, 0);	 // EngageTime
		Write32(pPlayerRelationInfo, 0);	 // NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0);	 // BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0);	 // EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0);		 // EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // KartNum
	for (size_t i = 0; i < 0; i++)
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
	Write16(p, 0); // HammerCount
	for (size_t i = 0; i < 0; i++)
	{ // EquipedHammer
		BYTE *pEquipedHammer = p;
		Write16(pEquipedHammer, 0); // len

		Write32(pEquipedHammer, 0); // EquipedHammerID

		len = pEquipedHammer - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // StealPigCount
	for (size_t i = 0; i < 0; i++)
	{ // StealPig
		BYTE *pStealPig = p;
		Write16(pStealPig, 0); // len

		Write8(pStealPig, 0);  // Status
		Write8(pStealPig, 0);  // PigPos
		Write32(pStealPig, 0); // ItemID
		Write8(pStealPig, 0);  // PigNum

		len = pStealPig - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HaveNext
	Write8(p, 0); // HaveAppellation
	for (size_t i = 0; i < 0; i++)
	{ // CurAppellation
		BYTE *pCurAppellation = p;
		Write16(pCurAppellation, 0); // len

		Write8(pCurAppellation, 0);	 // Type
		Write8(pCurAppellation, 0);	 // Level
		Write8(pCurAppellation, 0);	 // Status
		Write8(pCurAppellation, 0);	 // Difficulty
		Write32(pCurAppellation, 0); // MapId
		Write32(pCurAppellation, 0); // Value

		len = pCurAppellation - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // SuperShoeCount
	for (size_t i = 0; i < 0; i++)
	{ // EquipedSuperShoe
		BYTE *pEquipedSuperShoe = p;
		Write16(pEquipedSuperShoe, 0); // len

		Write32(pEquipedSuperShoe, 0); // EquipedSuperShoeID

		len = pEquipedSuperShoe - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 11019, buf, len, Client->ChatRoomID, FE_OUTDOORSVRD, Client->ConnID, Notify);
}

void NotifyChairInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	for (size_t i = 0; i < MAX_CHAIR_NUM; i++)
	{ // ChairInfo
		BYTE *pChairInfo = p;
		Write16(pChairInfo, 0); // len

		for (size_t i = 0; i < MAX_NUM_IN_EACHCHAIR; i++)
		{ // UinInChair
			Write32(pChairInfo, 0);
		}

		len = pChairInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 864, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestChangeChairState(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	BYTE ChangeState = Read8(Body);
	int ChairIndx = Read32(Body);
	BYTE SeatIndx = Read8(Body);

	ResponseChangeChairState(Client, ChangeState, ChairIndx, SeatIndx);
}
void ResponseChangeChairState(ClientNode *Client, BYTE ChangeState, int ChairIndx, BYTE SeatIndx)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // Result

	Write8(p, ChangeState);
	Write32(p, ChairIndx);
	Write8(p, SeatIndx);

	len = p - buf;
	SendToClient(Client, 360, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void LeaveOutdoor(ClientNode *Client)
{
	if (!Client) return;
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] == Client)
		{
			ResponseLeaveOutdoor(Client);
			ChatRoom->Player[i] = NULL;
			Client->ChatRoomID = -1;
			ChatRoom->CurrPlayerNum--;

			// 通知其它玩家
			for (char i2 = 0; i2 < 99; i2++)
			{
				if (ChatRoom->Player[i2])
				{
					NotifyLeaveOutdoor(ChatRoom->Player[i2], Client->Uin);
				}
			}

			break;
		}
	}
}
void RequestLeaveOutdoor(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	LeaveOutdoor(Client);
}
void ResponseLeaveOutdoor(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write8(p, 0);  // ReasonLen

	len = p - buf;
	SendToClient(Client, 353, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyLeaveOutdoor(ClientNode *Client, UINT Uin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin); // Uin
	Write32(p, 0);	 // Time

	len = p - buf;
	SendToClient(Client, 853, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestHeroMove(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	MoveInfo HeroMoveInfo;
	{ // HeroMoveInfo
		BYTE *pHeroMoveInfo = p;
		len = Read16(pHeroMoveInfo);

		HeroMoveInfo.Uin = Read32(pHeroMoveInfo);
		HeroMoveInfo.WalkStatue = Read8(pHeroMoveInfo);
		for (size_t i = 0; i < 3; i++)
		{
			// Client->HeroMoveInfo->Posion[i] = HeroMoveInfo.Posion[i];
			HeroMoveInfo.Posion[i] = Read32(pHeroMoveInfo);
		}
		for (size_t i = 0; i < 9; i++)
		{
			// Client->HeroMoveInfo->Ori[i] = HeroMoveInfo.Ori[i];
			HeroMoveInfo.Ori[i] = Read32(pHeroMoveInfo);
		}
		HeroMoveInfo.AttachUin = Read32(pHeroMoveInfo);
		HeroMoveInfo.WalkExtState = Read32(pHeroMoveInfo);
		HeroMoveInfo.Nums = Read8(pHeroMoveInfo);
		for (size_t i = 0; i < 5; i++)
		{
			// Client->HeroMoveInfo->PermitSitHorsePetUins[i] = HeroMoveInfo.PermitSitHorsePetUins[i];
			HeroMoveInfo.PermitSitHorsePetUins[i] = Read32(pHeroMoveInfo);
		}
		HeroMoveInfo.EquipedPetID = Read32(pHeroMoveInfo);
		HeroMoveInfo.EquipedPetStatus = Read32(pHeroMoveInfo);
		HeroMoveInfo.PetPKResultStatus = Read32(pHeroMoveInfo);
		HeroMoveInfo.HeroMajorState = Read16(pHeroMoveInfo);
		HeroMoveInfo.HorsePetStatus = Read16(pHeroMoveInfo);
		p += len;
	}
	HeroMoveInfo.NextPoint = Read32(p);
	HeroMoveInfo.PathLen = Read16(p);
	for (size_t i = 0; i < HeroMoveInfo.PathLen; i++)
	{
		// Client->HeroMoveInfo->Path[i] = HeroMoveInfo.Path[i];
		HeroMoveInfo.Path[i] = Read32(p);
	}
	HeroMoveInfo.CurCellID = Read32(p);
	HeroMoveInfo.Speed = Read16(p);
	/*Client->HeroMoveInfo->Uin= HeroMoveInfo.Uin;
	Client->HeroMoveInfo->WalkStatue = HeroMoveInfo.WalkStatue;



	Client->HeroMoveInfo->AttachUin = HeroMoveInfo.AttachUin;
	Client->HeroMoveInfo->WalkExtState = HeroMoveInfo.WalkExtState;
	Client->HeroMoveInfo->Nums = HeroMoveInfo.Nums;

	Client->HeroMoveInfo->EquipedPetID = HeroMoveInfo.EquipedPetID;
	Client->HeroMoveInfo->EquipedPetStatus = HeroMoveInfo.EquipedPetStatus;
	Client->HeroMoveInfo->PetPKResultStatus = HeroMoveInfo.PetPKResultStatus;
	Client->HeroMoveInfo->HeroMajorState = HeroMoveInfo.HeroMajorState;
	Client->HeroMoveInfo->HorsePetStatus = HeroMoveInfo.HorsePetStatus;
	Client->HeroMoveInfo->NextPoint = HeroMoveInfo.NextPoint;
	Client->HeroMoveInfo->PathLen = HeroMoveInfo.PathLen;

	Client->HeroMoveInfo->CurCellID = HeroMoveInfo.CurCellID;
	Client->HeroMoveInfo->Speed = HeroMoveInfo.Speed;*/
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{
			NotifyHeroMove(ChatRoom->Player[i], &HeroMoveInfo);
		}
	}
}
void NotifyHeroMove(ClientNode *Client, MoveInfo *HeroMoveInfo)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	{ // HeroMoveInfo
		BYTE *pHeroMoveInfo = p;
		Write16(pHeroMoveInfo, 0); // len

		Write32(pHeroMoveInfo, HeroMoveInfo->Uin);
		Write8(pHeroMoveInfo, HeroMoveInfo->WalkStatue);
		for (size_t i = 0; i < 3; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->Posion[i]);
		}
		for (size_t i = 0; i < 9; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->Ori[i]);
		}
		Write32(pHeroMoveInfo, HeroMoveInfo->AttachUin);
		Write32(pHeroMoveInfo, HeroMoveInfo->WalkExtState);
		Write8(pHeroMoveInfo, HeroMoveInfo->Nums); // Nums
		for (size_t i = 0; i < 5; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->PermitSitHorsePetUins[i]);
		}
		Write32(pHeroMoveInfo, Client->Pet->ID); // EquipedPetID
		Write32(pHeroMoveInfo, 3);				 // EquipedPetStatus
		// Write32(pHeroMoveInfo, HeroMoveInfo->EquipedPetID); //EquipedPetID
		// Write32(pHeroMoveInfo, HeroMoveInfo->EquipedPetStatus); //EquipedPetStatus
		Write32(pHeroMoveInfo, HeroMoveInfo->PetPKResultStatus); // PetPKResultStatus
		Write16(pHeroMoveInfo, HeroMoveInfo->HeroMajorState);	 // HeroMajorState
		Write16(pHeroMoveInfo, HeroMoveInfo->HorsePetStatus);	 // HorsePetStatus
		len = pHeroMoveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, HeroMoveInfo->NextPoint); // NextPoint
	Write16(p, HeroMoveInfo->PathLen);	 // PathLen
	for (size_t i = 0; i < HeroMoveInfo->PathLen; i++)
	{
		Write32(p, HeroMoveInfo->Path[i]);
	}
	Write32(p, 0); // Time

	len = p - buf;
	SendToClient(Client, 851, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestHeroPos(ClientNode *Client, BYTE *Body, size_t BodyLen)
{

	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	MoveInfo HeroMoveInfo;
	{ // HeroMoveInfo
		BYTE *pHeroMoveInfo = p;
		len = Read16(pHeroMoveInfo);

		HeroMoveInfo.Uin = Read32(pHeroMoveInfo);
		HeroMoveInfo.WalkStatue = Read8(pHeroMoveInfo);
		for (size_t i = 0; i < 3; i++)
		{
			// Client->HeroMoveInfo->Posion[i] = HeroMoveInfo.Posion[i];
			HeroMoveInfo.Posion[i] = Read32(pHeroMoveInfo);
		}
		for (size_t i = 0; i < 9; i++)
		{
			// Client->HeroMoveInfo->Ori[i] = HeroMoveInfo.Ori[i];
			HeroMoveInfo.Ori[i] = Read32(pHeroMoveInfo);
		}
		HeroMoveInfo.AttachUin = Read32(pHeroMoveInfo);
		HeroMoveInfo.WalkExtState = Read32(pHeroMoveInfo);
		HeroMoveInfo.Nums = Read8(pHeroMoveInfo);
		for (size_t i = 0; i < 5; i++)
		{
			// Client->HeroMoveInfo->PermitSitHorsePetUins[i] = HeroMoveInfo.PermitSitHorsePetUins[i];
			HeroMoveInfo.PermitSitHorsePetUins[i] = Read32(pHeroMoveInfo);
		}
		HeroMoveInfo.EquipedPetID = Read32(pHeroMoveInfo);
		HeroMoveInfo.EquipedPetStatus = Read32(pHeroMoveInfo);
		HeroMoveInfo.PetPKResultStatus = Read32(pHeroMoveInfo);
		HeroMoveInfo.HeroMajorState = Read16(pHeroMoveInfo);
		HeroMoveInfo.HorsePetStatus = Read16(pHeroMoveInfo);
		p += len;
	}
	HeroMoveInfo.CellID = Read32(p);
	HeroMoveInfo.Flag = Read8(p);
	/*Client->HeroMoveInfo->Uin = HeroMoveInfo.Uin;
	Client->HeroMoveInfo->WalkStatue = HeroMoveInfo.WalkStatue;


	Client->HeroMoveInfo->AttachUin = HeroMoveInfo.AttachUin;
	Client->HeroMoveInfo->WalkExtState = HeroMoveInfo.WalkExtState;
	Client->HeroMoveInfo->Nums = HeroMoveInfo.Nums;

	Client->HeroMoveInfo->EquipedPetID = HeroMoveInfo.EquipedPetID;
	Client->HeroMoveInfo->EquipedPetStatus = HeroMoveInfo.EquipedPetStatus;
	Client->HeroMoveInfo->PetPKResultStatus = HeroMoveInfo.PetPKResultStatus;
	Client->HeroMoveInfo->HeroMajorState = HeroMoveInfo.HeroMajorState;
	Client->HeroMoveInfo->HorsePetStatus = HeroMoveInfo.HorsePetStatus;

	Client->HeroMoveInfo->CellID = HeroMoveInfo.CellID;
	Client->HeroMoveInfo->Flag = HeroMoveInfo.Flag;*/
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{
			NotifySynAllHeroPos(ChatRoom->Player[i], &HeroMoveInfo);
		}
	}
}

void NotifySynAllHeroPos(ClientNode *Client, MoveInfo *HeroMoveInfo)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 1); // HeroCount
	for (size_t i = 0; i < 1; i++)
	{ // HeroMoveInfo
		BYTE *pHeroMoveInfo = p;
		Write16(pHeroMoveInfo, 0); // len

		Write32(pHeroMoveInfo, HeroMoveInfo->Uin);		 // Uin
		Write8(pHeroMoveInfo, HeroMoveInfo->WalkStatue); // WalkStatue
		for (size_t i = 0; i < 3; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->Posion[i]);
		}
		for (size_t i = 0; i < 9; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->Ori[i]);
		}
		Write32(pHeroMoveInfo, HeroMoveInfo->AttachUin);
		Write32(pHeroMoveInfo, HeroMoveInfo->WalkExtState);
		Write8(pHeroMoveInfo, HeroMoveInfo->Nums); // Nums
		for (size_t i = 0; i < 5; i++)
		{
			Write32(pHeroMoveInfo, HeroMoveInfo->PermitSitHorsePetUins[i]);
		}
		Write32(pHeroMoveInfo, Client->Pet->ID); // EquipedPetID
		Write32(pHeroMoveInfo, 3);				 // EquipedPetStatus
		// Write32(pHeroMoveInfo, HeroMoveInfo->EquipedPetID); //EquipedPetID
		// Write32(pHeroMoveInfo, HeroMoveInfo->EquipedPetStatus); //EquipedPetStatus
		Write32(pHeroMoveInfo, HeroMoveInfo->PetPKResultStatus); // PetPKResultStatus
		Write16(pHeroMoveInfo, HeroMoveInfo->HeroMajorState);	 // HeroMajorState
		Write16(pHeroMoveInfo, HeroMoveInfo->HorsePetStatus);	 // HorsePetStatus

		len = pHeroMoveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0);				   // Time
	Write8(p, HeroMoveInfo->Flag); // Flag

	len = p - buf;
	SendToClient(Client, 850, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyPlayerUseHammerInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, 0);			 // Result
	Write8(p, 0);			 // ActiveValue
	Write8(p, 0);			 // TotalNum
	Write8(p, 0);			 // CurrentNum
	Write32(p, 0);			 // Money
	Write32(p, 0);			 // SuperMoney
	Write32(p, 0);			 // Exp
	Write32(p, 0);			 // Charm
	Write32(p, 0);			 // DanceExp
	Write32(p, 0);			 // Coupons
	Write32(p, 0);			 // Admiration
	Write32(p, 0);			 // LuckMoney
	Write32(p, 0);			 // TeamWorkExp
	Write32(p, 0);			 // AchievePoint
	Write32(p, 0);			 // ConsumeScore
	Write32(p, 0);			 // HistoryConsumeScore
	Write32(p, 0);			 // LadderMatchAchievePoint
	Write32(p, 0);			 // SpeedBean
	Write32(p, 0);			 // SpeedCoin

	len = p - buf;
	SendToClient(Client, 24641, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestUseHammer(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ItemID = Read32(p);
	UINT DstUin = Read32(p);
	int SrcWeight = Read32(p);
	int DstWeight = Read32(p);
	bool DstType = Read8(p);
	UINT DistToTgt = Read32(p);
	UCHAR MsgSeq = Read8(p);
	int Position1 = Read32(p);
	int Position2 = Read32(p);
	int Position3 = Read32(p);
	int CellID = Read32(p);
	NotifyUseItemPK(Client, ItemID, DstUin, DstType, Uin);
	ResponseUseHammer(Client, ItemID, DstUin, DstType);
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{
			NotifyUseItemPK(ChatRoom->Player[i], ItemID, DstUin, DstType, Uin);
		}
	}
}
void NotifyUseItemPK(ClientNode *Client, int ItemID, UINT DstUin, bool DstType, UINT Uin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin);	// SrcUin
	Write32(p, DstUin); // DstUin
	Write32(p, ItemID); // ItemID
	Write8(p, 0);		// AttachDataLen
	Write8(p, DstType); // DstType
	Write8(p, 0);		// HasPKAbilityInfo
	for (size_t i = 0; i < 0; i++)
	{ // PKAbilityInfo
		BYTE *pPKAbilityInfo = p;
		Write16(pPKAbilityInfo, 0); // len

		Write16(pPKAbilityInfo, 0); // SrcPKAbility
		Write8(pPKAbilityInfo, 0);	// IsSrcPKStrong
		Write16(pPKAbilityInfo, 0); // DstPKAbility
		Write8(pPKAbilityInfo, 0);	// IsDstPKStrong

		len = pPKAbilityInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // Para
	Write8(p, 0);  // OpType

	len = p - buf;
	SendToClient(Client, 567, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyUseItemMagic(ClientNode* Client, int ItemID, UINT SrcUin, UINT DstUin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, SrcUin); //SrcUin
	Write32(p, DstUin); //DstUin
	Write32(p, ItemID); //ItemID

	Write8(p, 0); //AttachDataLen
	Write8(p, 1); //DstType
	Write8(p, 1); //HasPKAbilityInfo
	for (size_t i = 0; i < 1; i++)
	{ //PKAbilityInfo
		BYTE* pPKAbilityInfo = p;
		Write16(pPKAbilityInfo, 0); //len

		Write16(pPKAbilityInfo, 0); //SrcPKAbility
		Write8(pPKAbilityInfo, 0); //IsSrcPKStrong
		Write16(pPKAbilityInfo, 0); //DstPKAbility
		Write8(pPKAbilityInfo, 0); //IsDstPKStrong

		len = pPKAbilityInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 567, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void ResponseUseHammer(ClientNode *Client, int ItemID, UINT DstUin, bool DstType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);		// ResultID
	Write32(p, ItemID); // ItemID
	Write32(p, DstUin); // DstUin
	Write8(p, 0);		// ReasonLen
	Write8(p, DstType); // DstType
	Write8(p, 0);		// HasPKAbilityInfo
	for (size_t i = 0; i < 0; i++)
	{ // PKAbilityInfo
		BYTE *pPKAbilityInfo = p;
		Write16(pPKAbilityInfo, 0); // len

		Write16(pPKAbilityInfo, 0); // SrcPKAbility
		Write8(pPKAbilityInfo, 0);	// IsSrcPKStrong
		Write16(pPKAbilityInfo, 0); // DstPKAbility
		Write8(pPKAbilityInfo, 0);	// IsDstPKStrong

		len = pPKAbilityInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 358, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestHeroDoAction(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short ActionID = Read16(p);
	int Para1 = Read32(p);
	ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
		{
			NotifyHeroDoAction(ChatRoom->Player[i], ActionID, Para1, Uin);
		}
	}
	ResponseHeroDoAction(Client, ActionID, Para1);
}

void NotifyPlayerDoAction(ClientNode *Client, short ActionID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);		  // Uin
	Write32(p, 0);		  // Time
	Write16(p, ActionID); // ActionID

	len = p - buf;
	SendToClient(Client, 856, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void ResponseHeroDoAction(ClientNode *Client, short ActionID, int Para1)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);	   // ResultID
	Write8(p, 0);	   // ReasonLen
	Write32(p, Para1); // Para1

	len = p - buf;
	SendToClient(Client, 355, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyHeroDoAction(ClientNode *Client, short ActionID, int Para1, UINT Uin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin);	  // Uin
	Write32(p, 0);		  // Time
	Write16(p, ActionID); // ActionID
	Write32(p, Para1);	  // Para1

	len = p - buf;
	SendToClient(Client, 855, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestChangeHammerStatus(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	{ // EquipedHammer
		BYTE *pEquipedHammer = p;
		len = Read16(pEquipedHammer);

		int EquipedHammerID = Read32(pEquipedHammer);

		p += len;
		ResponseChangeHammerStatus(Client, EquipedHammerID);
		ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
		if (!ChatRoom)
		{
			return;
		}
		for (char i = 0; i < 99; i++)
		{
			if (ChatRoom->Player[i] && ChatRoom->Player[i] != Client)
			{
				NotifyChangeHammerStatus(ChatRoom->Player[i], EquipedHammerID, Client->Uin);
			}
		}
	}
}

void ResponseChangeHammerStatus(ClientNode *Client, int EquipedHammerID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	{			   // EquipedHammer
		BYTE *pEquipedHammer = p;
		Write16(pEquipedHammer, 0); // len

		Write32(pEquipedHammer, EquipedHammerID); // EquipedHammerID

		len = pEquipedHammer - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 359, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyChangeHammerStatus(ClientNode *Client, int EquipedHammerID, UINT SrcUin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, SrcUin); // SrcUin
	Write32(p, 0);		// Time
	{					// EquipedHammer
		BYTE *pEquipedHammer = p;
		Write16(pEquipedHammer, 0); // len

		Write32(pEquipedHammer, EquipedHammerID); // EquipedHammerID

		len = pEquipedHammer - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 861, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyBriefGuildInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	WriteString(p, 0); // GuildName[]
	Write32(p, 0);	   // GuildID
	Write16(p, 0);	   // Duty
	Write32(p, 0);	   // Right
	Write32(p, 0);	   // Medal
	Write32(p, 0);	   // Contribute
	Write16(p, 0);	   // TeamAddition
	Write32(p, 0);	   // GuildScore
	Write32(p, 0);	   // LeaderUin
	Write16(p, 0);	   // YesterdayMaxOnline
	Write32(p, 0);	   // LastSingleLoginTime
	Write16(p, 0);	   // GuildLevel
	Write8(p, 0);	   // TitleLen
	Write32(p, 0);	   // DayChallenge
	Write32(p, 0);	   // CurSeasonId
	Write16(p, 0);	   // LastSeasonMapID
	Write16(p, 0);	   // LastSeasonRank
	Write32(p, 0);	   // LastHero
	Write32(p, 0);	   // CurHero
	Write32(p, 0);	   // LastEnlistedTime
	Write32(p, 0);	   // LastModGuildNameTime
	Write32(p, 0);	   // GuildCreateTime
	Write32(p, 0);	   // JoinGuildTime
	Write32(p, 0);	   // ScoreRank
	Write32(p, 0);	   // WonderLandID
	Write16(p, 0);	   // MemberNum
	Write32(p, 0);	   // Prosperity
	Write16(p, 0);	   // WonderLandDuty
	Write32(p, 0);	   // TotalWlDegree
	Write32(p, 0);	   // WeeklyWlDegree
	Write32(p, 0);	   // LastCheckWlDegreeTime
	Write32(p, 0);	   // WeeklyWlDegreeForMatch
	Write32(p, 0);	   // WeeklyMatchScore
	Write32(p, 0);	   // WeeklyUsedMatchScore
	Write32(p, 0);	   // TotalMatchScore
	WriteUn(p, 0);	   // WeeklyGettedWlWelfareIdBitSet
	Write32(p, 0);	   // LastGetWlBonusTime
	Write32(p, 0);	   // BanWlTalkEndTime
	Write32(p, 0);	   // GetWlBonusEndTime
	Write32(p, 0);	   // LastDayCheerValue
	Write32(p, 0);	   // LastDayCheerCnt
	Write32(p, 0);	   // LastRegisterTime
	Write32(p, 0);	   // CurrentOnLineGuildMembers
	Write8(p, 0);	   // LeaderFirstOpened
	Write8(p, 0);	   // AppliedChampionship

	len = p - buf;
	SendToClient(Client, 15205, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestCreateGuild(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	char *NickName = ReadString(p, MaxNickName);
	char *GuildName = ReadString(p, MaxNickName);
	UINT Guid = Read32(p);
	ResponseCreateGuild(Client, GuildName, Guid);
}

void ResponseCreateGuild(ClientNode *Client, char *GuildName, UINT Guid)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin);				// Uin
	WriteString(p, GuildName, MaxNickName); // GuildName[]
	Write32(p, Guid);						// GuildID
	Write16(p, 0);							// ResultID
	Write16(p, 0);							// ReasonLen

	len = p - buf;
	SendToClient(Client, 15000, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestAddMember(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	// char[] DstNickName[] = ReadString(p);

	char DstNickName[256];
	ResponseAddMember(Client);
}

void ResponseAddMember(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	Write32(p, 0);	   // DstUin
	WriteString(p, 0); // DstNickName[]
	Write16(p, 0);	   // ResultID
	Write16(p, 0);	   // ReasonLen

	len = p - buf;
	SendToClient(Client, 15001, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyBeAddedToGuild(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // SrcUin
	WriteString(p, 0); // SrcNickName[]
	WriteString(p, 0); // GuildName[]
	Write32(p, 0);	   // RandomFlag

	len = p - buf;
	SendToClient(Client, 15206, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestReplyBeAddedToGuild(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT SrcUin = Read32(p);
	UINT ReplyUin = Read32(p);

	// char[] ReplyNickName[] = ReadString(p);
	// char[] GuildName[] = ReadString(p);

	char ReplyNickName[256];
	char GuildName[256];

	short ResultID = Read16(p);
	USHORT ReasonLen = Read16(p);
	UINT RandomFlag = Read32(p);
}

void NotifyAddToGuildResult(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 10001);							   // ApplyUin
	WriteString(p, Client->NickName, MaxNickName); // ApplyNickName[]
	WriteString(p, "MC测试团", MaxNickName);	   // GuildName[]
	Write32(p, 0);								   // GuildID
	Write16(p, 0);								   // Duty
	Write32(p, 0);								   // Right
	Write32(p, 0);								   // Medal
	Write32(p, 10001);							   // LeaderUin
	Write16(p, 0);								   // YesterdayMaxOnline
	Write32(p, 0);								   // LastSingleLoginTime
	Write16(p, 5);								   // GuildLevel
	Write32(p, 0);								   // WonderLandID
	Write16(p, 0);								   // WonderLandDuty
	Write32(p, 10001);							   // ApproverUin
	Write8(p, 0);								   // ApplyType

	len = p - buf;
	SendToClient(Client, 15210, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestApplyJoinGuild(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	// char[] GuildName[] = ReadString(p);
	// char[] ApplyNickName[] = ReadString(p);
	char GuildName[256];
	char ApplyNickName[256];

	// U Guid = ReadU(p);

	Write8(p, 0);
	{ // SelfCond
		BYTE *pSelfCond = p;
		len = Read16(pSelfCond);

		UINT ProsperityCondition = Read32(pSelfCond);
		UINT JoinTotalCondition = Read32(pSelfCond);
		UINT HonorCondition = Read32(pSelfCond);
		UINT CharmCondition = Read32(pSelfCond);
		UINT DanceExpCondition = Read32(pSelfCond);
		UINT SkillPointCondition = Read32(pSelfCond);
		UINT ExpLevelCondition = Read32(pSelfCond);
		UINT ApplyExpLevelCondition = Read32(pSelfCond);
		UCHAR EnlistType = Read8(pSelfCond);

		p += len;
	}
	UCHAR ApplyType = Read8(p);
	UCHAR RecommendType = Read8(p);

	ResponseApplyJoinGuild(Client);
}

void ResponseApplyJoinGuild(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	WriteString(p, 0); // GuildName[]
	Write16(p, 0);	   // ResultID
	Write16(p, 0);	   // ReasonLen
	Write16(p, 0);	   // ReplyQtUinNum
	Write32(p, 0);	   // ReplyQtUin[]

	len = p - buf;
	SendToClient(Client, 15010, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyApplyJoinGuild(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // ApplyUin
	WriteString(p, 0); // ApplyNickName[]
	Write32(p, 0);	   // RandomFlag
	Write32(p, 0);	   // ExpLevel
	Write8(p, 0);	   // ApplyType
	Write8(p, 0);	   // RecommendType

	len = p - buf;
	SendToClient(Client, 15208, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestReplyJoinGuild(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT ApplyUin = Read32(p);
	// char[] ApplyNickName[] = ReadString(p);
	char ApplyNickName[256];
	UINT ReplyUin = Read32(p);
	short ResultID = Read16(p);
	USHORT ReasonLen = Read16(p);
	UINT RandomFlag = Read32(p);
	int OperationChannelType = Read32(p);
	UCHAR ApplyType = Read8(p);
	UCHAR RecommendType = Read8(p);

	ResponseReplyJoinGuild(Client);
}

void ResponseReplyJoinGuild(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write32(p, 0); // ApplyUin
	Write32(p, 0); // ReplyUin
	Write32(p, 0); // OperationChannelType

	len = p - buf;
	SendToClient(Client, 15013, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestRemoveMember(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	int OperationChannelType = Read32(p);

	ResponseRemoveMember(Client);
}

void ResponseRemoveMember(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	Write32(p, 0);	   // DstUin
	Write16(p, 0);	   // ResultID
	Write16(p, 0);	   // ReasonLen
	WriteUn(p, 0);	   // AddCash
	Write8(p, 0);	   // AddCash
	WriteString(p, 0); // SrcNickName[]
	WriteString(p, 0); // DstNickName[]
	Write32(p, 0);	   // OperationChannelType
	Write32(p, 0);	   // Contribute

	len = p - buf;
	SendToClient(Client, 15002, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyRemoveMember(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	Write32(p, 0);	   // DstUin
	WriteString(p, 0); // GuildName[]
	WriteString(p, 0); // DstNickName[]
	Write32(p, 0);	   // Guid
	WriteUn(p, 0);	   // AddCash
	Write8(p, 0);	   // AddCash

	len = p - buf;
	SendToClient(Client, 15202, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestGetGuildList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT StartIdx = Read32(p);
	UINT GuildCount = Read32(p);
}

void ResponseGetGuildList(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write32(p, 0); // StartIdx
	Write32(p, 0); // TotalCount
	Write32(p, 0); // GuildCount

	Write8(p, 1); // GuildDescInfo
	for (size_t i = 0; i < 1; i++)
	{ // GuildDescInfo
		BYTE *pGuildDescInfo = p;
		Write16(pGuildDescInfo, 0); // len

		Write32(pGuildDescInfo, 0);		// GuildID
		WriteString(pGuildDescInfo, 0); // Name[]
		Write32(pGuildDescInfo, 0);		// Level
		Write32(pGuildDescInfo, 0);		// Score
		Write32(pGuildDescInfo, 0);		// CreateTime
		Write16(pGuildDescInfo, 0);		// Attribute
		Write16(pGuildDescInfo, 0);		// MaxMember
		Write16(pGuildDescInfo, 0);		// CurrentMember
		Write32(pGuildDescInfo, 0);		// Prosperity
		Write16(pGuildDescInfo, 0);		// Level
		{								// GuildEnlistCond
			BYTE *pGuildEnlistCond = pGuildDescInfo;
			Write16(pGuildEnlistCond, 0); // len

			Write32(pGuildEnlistCond, 0); // ProsperityCondition
			Write32(pGuildEnlistCond, 0); // JoinTotalCondition
			Write32(pGuildEnlistCond, 0); // HonorCondition
			Write32(pGuildEnlistCond, 0); // CharmCondition
			Write32(pGuildEnlistCond, 0); // DanceExpCondition
			Write32(pGuildEnlistCond, 0); // SkillPointCondition
			Write32(pGuildEnlistCond, 0); // ExpLevelCondition
			Write32(pGuildEnlistCond, 0); // ApplyExpLevelCondition
			Write8(pGuildEnlistCond, 0);  // EnlistType

			len = pGuildEnlistCond - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write8(pGuildDescInfo, 0); // SubType
		{						   // CardOtherInfo
			BYTE *pCardOtherInfo = pGuildDescInfo;
			Write16(pCardOtherInfo, 0); // len

			Write32(pCardOtherInfo, 0); // MainZone1
			Write32(pCardOtherInfo, 0); // MainZone2
			Write32(pCardOtherInfo, 0); // MainMode1
			Write32(pCardOtherInfo, 0); // MainMode2
			Write32(pCardOtherInfo, 0); // OtherInterest1
			Write32(pCardOtherInfo, 0); // OtherInterest2

			len = pCardOtherInfo - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write16(pGuildDescInfo, 0); // DescLen
		Write32(pGuildDescInfo, 0); // WeekProsper
		Write32(pGuildDescInfo, 0); // GuildQQGroup
		Write8(pGuildDescInfo, 0);	// RecommendType
		Write16(pGuildDescInfo, 0); // FriendNums
		Write16(pGuildDescInfo, 0); // DCRecommendIndex

		len = pGuildDescInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen
	WriteUn(p, 0); // NextQueryNum
	Write8(p, 0);  // NextQueryNum
	Write8(p, 0);  // NextQueryKey[]

	len = p - buf;
	SendToClient(Client, 15004, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestFindGuild(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	// char[] GuildName[] = ReadString(p);
	char GuildName[256];
	UINT Guid = Read32(p);
	UINT Sort = Read32(p);
	UCHAR RecommendType = Read8(p);
	ResponseFindGuild(Client);
}

void ResponseFindGuild(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	{			   // GuildDescInfo
		BYTE *pGuildDescInfo = p;
		Write16(pGuildDescInfo, 0); // len

		Write32(pGuildDescInfo, 0);		// GuildID
		WriteString(pGuildDescInfo, 0); // Name[]
		{								// LeaderInfo
			BYTE *pLeaderInfo = pGuildDescInfo;
			Write16(pLeaderInfo, 0); // len

			Write32(pLeaderInfo, 0);	 // Uin
			WriteString(pLeaderInfo, 0); // NickName[]
			Write16(pLeaderInfo, 0);	 // Duty
			Write8(pLeaderInfo, 0);		 // OnlineFlag
			WriteUn(pLeaderInfo, 0);	 // LastLogin
			Write32(pLeaderInfo, 0);	 // MemProsperity
			Write32(pLeaderInfo, 0);	 // LastEnlistedTime
			Write8(pLeaderInfo, 0);		 // EnlistFlag
			Write16(pLeaderInfo, 0);	 // WonderLandDuty
			Write32(pLeaderInfo, 0);	 // TotalWlDegree
			Write32(pLeaderInfo, 0);	 // WeeklyWlDegree
			Write32(pLeaderInfo, 0);	 // Donate
			Write32(pLeaderInfo, 0);	 // SeasonZhanxun
			Write32(pLeaderInfo, 0);	 // WeeklyContributeValue
			Write32(pLeaderInfo, 0);	 // WeeklyProsperContributeValue
			Write32(pLeaderInfo, 0);	 // MonthlyProsperContributeValue
			Write32(pLeaderInfo, 0);	 // CommonParas[]
			Write32(pLeaderInfo, 0);	 // TimeJoinedGuild
			Write8(pLeaderInfo, 0);		 // AppliedChampionship

			len = pLeaderInfo - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write16(pGuildDescInfo, 0); // NotesLen
		Write16(pGuildDescInfo, 0); // DescLen
		Write32(pGuildDescInfo, 0); // Level
		Write32(pGuildDescInfo, 0); // Score
		Write32(pGuildDescInfo, 0); // CreateTime
		Write16(pGuildDescInfo, 0); // Attribute
		Write16(pGuildDescInfo, 0); // MaxMember
		Write16(pGuildDescInfo, 0); // CurrentMember
		Write16(pGuildDescInfo, 0); // TeamAddition
		Write16(pGuildDescInfo, 0); // YesterdayMaxOnline
		Write16(pGuildDescInfo, 0); // HistoryMaxOnline
		Write32(pGuildDescInfo, 0); // HistoryMaxOnlineTime
		Write32(pGuildDescInfo, 0); // Prosperity
		Write16(pGuildDescInfo, 0); // Level
		{							// GuildEnlistCond
			BYTE *pGuildEnlistCond = pGuildDescInfo;
			Write16(pGuildEnlistCond, 0); // len

			Write32(pGuildEnlistCond, 0); // ProsperityCondition
			Write32(pGuildEnlistCond, 0); // JoinTotalCondition
			Write32(pGuildEnlistCond, 0); // HonorCondition
			Write32(pGuildEnlistCond, 0); // CharmCondition
			Write32(pGuildEnlistCond, 0); // DanceExpCondition
			Write32(pGuildEnlistCond, 0); // SkillPointCondition
			Write32(pGuildEnlistCond, 0); // ExpLevelCondition
			Write32(pGuildEnlistCond, 0); // ApplyExpLevelCondition
			Write8(pGuildEnlistCond, 0);  // EnlistType

			len = pGuildEnlistCond - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write8(pGuildDescInfo, 0);	// NewGuildType
		Write32(pGuildDescInfo, 0); // GuildQQGroup
		{							// WorkDayActiveTimeSeg
			BYTE *pWorkDayActiveTimeSeg = pGuildDescInfo;
			Write16(pWorkDayActiveTimeSeg, 0); // len

			Write16(pWorkDayActiveTimeSeg, 0); // BeginTimeInt
			Write16(pWorkDayActiveTimeSeg, 0); // EndTimeInt

			len = pWorkDayActiveTimeSeg - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		{ // WeekendActiveTimeSeg
			BYTE *pWeekendActiveTimeSeg = pGuildDescInfo;
			Write16(pWeekendActiveTimeSeg, 0); // len

			Write16(pWeekendActiveTimeSeg, 0); // BeginTimeInt
			Write16(pWeekendActiveTimeSeg, 0); // EndTimeInt

			len = pWeekendActiveTimeSeg - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write32(pGuildDescInfo, 0); // LastEnlistTime
		Write8(pGuildDescInfo, 0);	// EnlistNum
		Write32(pGuildDescInfo, 0); // LastModGuildNameTime
		Write32(pGuildDescInfo, 0); // WonderLandID
		WriteUn(pGuildDescInfo, 0); // UiCurDonate
		Write32(pGuildDescInfo, 0); // LastDayCheerValue
		Write32(pGuildDescInfo, 0); // LastDayCheerCnt
		{							// BorderMatchInfo
			BYTE *pBorderMatchInfo = pGuildDescInfo;
			Write16(pBorderMatchInfo, 0); // len

			Write32(pBorderMatchInfo, 0);	  // Honor
			Write32(pBorderMatchInfo, 0);	  // SeasonHonor
			Write32(pBorderMatchInfo, 0);	  // SeasonZhanxunFirstUin
			Write32(pBorderMatchInfo, 0);	  // SeasonZhanxun
			WriteString(pBorderMatchInfo, 0); // SeasonZhanxunFirstNickName[]

			len = pBorderMatchInfo - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		{ // CardOtherInfo
			BYTE *pCardOtherInfo = pGuildDescInfo;
			Write16(pCardOtherInfo, 0); // len

			Write32(pCardOtherInfo, 0); // MainZone1
			Write32(pCardOtherInfo, 0); // MainZone2
			Write32(pCardOtherInfo, 0); // MainMode1
			Write32(pCardOtherInfo, 0); // MainMode2
			Write32(pCardOtherInfo, 0); // OtherInterest1
			Write32(pCardOtherInfo, 0); // OtherInterest2

			len = pCardOtherInfo - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}

		len = pGuildDescInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 15009, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGuildDetail(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT Sort = Read32(p);
	ResponseGuildDetail(Client);
}

void ResponseGuildDetail(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	{						 // GuildDescInfo
		BYTE *pGuildDescInfo = p;
		Write16(pGuildDescInfo, 0); // len

		Write32(pGuildDescInfo, 666666);					  // GuildID
		WriteString(pGuildDescInfo, "MC骑士团", MaxNickName); // Name[]
		{													  // LeaderInfo
			BYTE *pLeaderInfo = pGuildDescInfo;
			Write16(pLeaderInfo, 0); // len

			Write32(pLeaderInfo, Client->Uin);						 // Uin
			WriteString(pLeaderInfo, Client->NickName, MaxNickName); // NickName[]
			Write16(pLeaderInfo, 1);								 // Duty
			Write8(pLeaderInfo, 1);									 // OnlineFlag
			WriteUn(pLeaderInfo, 0);								 // LastLogin
			Write32(pLeaderInfo, 9999);								 // MemProsperity
			Write32(pLeaderInfo, 0);								 // LastEnlistedTime
			Write8(pLeaderInfo, 1);									 // EnlistFlag
			Write16(pLeaderInfo, 0);								 // WonderLandDuty
			Write32(pLeaderInfo, 0);								 // TotalWlDegree
			Write32(pLeaderInfo, 0);								 // WeeklyWlDegree
			Write32(pLeaderInfo, 0);								 // Donate
			Write32(pLeaderInfo, 0);								 // SeasonZhanxun
			Write32(pLeaderInfo, 0);								 // WeeklyContributeValue
			Write32(pLeaderInfo, 0);								 // WeeklyProsperContributeValue
			Write32(pLeaderInfo, 0);								 // MonthlyProsperContributeValue
			Write32(pLeaderInfo, 0);								 // CommonParas[]
			Write32(pLeaderInfo, 0);								 // TimeJoinedGuild
			Write8(pLeaderInfo, 0);									 // AppliedChampionship

			len = pLeaderInfo - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write16(pGuildDescInfo, 0);		// NotesLen
		Write16(pGuildDescInfo, 0);		// DescLen
		Write32(pGuildDescInfo, 3);		// Level
		Write32(pGuildDescInfo, 60000); // Score
		Write32(pGuildDescInfo, 0);		// CreateTime
		Write16(pGuildDescInfo, 0);		// Attribute
		Write16(pGuildDescInfo, 60);	// MaxMember
		Write16(pGuildDescInfo, 60);	// CurrentMember
		Write16(pGuildDescInfo, 9999);	// TeamAddition
		Write16(pGuildDescInfo, 0);		// YesterdayMaxOnline
		Write16(pGuildDescInfo, 0);		// HistoryMaxOnline
		Write32(pGuildDescInfo, 0);		// HistoryMaxOnlineTime
		Write32(pGuildDescInfo, 99999); // Prosperity
		Write16(pGuildDescInfo, 3);		// Level
		{								// GuildEnlistCond
			BYTE *pGuildEnlistCond = pGuildDescInfo;
			Write16(pGuildEnlistCond, 0); // len

			Write32(pGuildEnlistCond, 0); // ProsperityCondition
			Write32(pGuildEnlistCond, 0); // JoinTotalCondition
			Write32(pGuildEnlistCond, 0); // HonorCondition
			Write32(pGuildEnlistCond, 0); // CharmCondition
			Write32(pGuildEnlistCond, 0); // DanceExpCondition
			Write32(pGuildEnlistCond, 0); // SkillPointCondition
			Write32(pGuildEnlistCond, 0); // ExpLevelCondition
			Write32(pGuildEnlistCond, 0); // ApplyExpLevelCondition
			Write8(pGuildEnlistCond, 0);  // EnlistType

			len = pGuildEnlistCond - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write8(pGuildDescInfo, 1);		 // NewGuildType
		Write32(pGuildDescInfo, 666233); // GuildQQGroup
		{								 // WorkDayActiveTimeSeg
			BYTE *pWorkDayActiveTimeSeg = pGuildDescInfo;
			Write16(pWorkDayActiveTimeSeg, 0); // len

			Write16(pWorkDayActiveTimeSeg, 0); // BeginTimeInt
			Write16(pWorkDayActiveTimeSeg, 0); // EndTimeInt

			len = pWorkDayActiveTimeSeg - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		{ // WeekendActiveTimeSeg
			BYTE *pWeekendActiveTimeSeg = pGuildDescInfo;
			Write16(pWeekendActiveTimeSeg, 0); // len

			Write16(pWeekendActiveTimeSeg, 0); // BeginTimeInt
			Write16(pWeekendActiveTimeSeg, 0); // EndTimeInt

			len = pWeekendActiveTimeSeg - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		Write32(pGuildDescInfo, 0); // LastEnlistTime
		Write8(pGuildDescInfo, 0);	// EnlistNum
		Write32(pGuildDescInfo, 0); // LastModGuildNameTime
		Write32(pGuildDescInfo, 0); // WonderLandID
		WriteUn(pGuildDescInfo, 0); // UiCurDonate
		Write32(pGuildDescInfo, 0); // LastDayCheerValue
		Write32(pGuildDescInfo, 0); // LastDayCheerCnt
		{							// BorderMatchInfo
			BYTE *pBorderMatchInfo = pGuildDescInfo;
			Write16(pBorderMatchInfo, 0); // len

			Write32(pBorderMatchInfo, 0); // Honor
			Write32(pBorderMatchInfo, 0); // SeasonHonor
			Write32(pBorderMatchInfo, 0); // SeasonZhanxunFirstUin
			Write32(pBorderMatchInfo, 0); // SeasonZhanxun
			Write8(pBorderMatchInfo, 0);  // SeasonZhanxunFirstNickName[]

			len = pBorderMatchInfo - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}
		{ // CardOtherInfo
			BYTE *pCardOtherInfo = pGuildDescInfo;
			Write16(pCardOtherInfo, 0); // len

			Write32(pCardOtherInfo, 0); // MainZone1
			Write32(pCardOtherInfo, 0); // MainZone2
			Write32(pCardOtherInfo, 0); // MainMode1
			Write32(pCardOtherInfo, 0); // MainMode2
			Write32(pCardOtherInfo, 0); // OtherInterest1
			Write32(pCardOtherInfo, 0); // OtherInterest2

			len = pCardOtherInfo - pGuildDescInfo;
			Set16(pGuildDescInfo, (WORD)len);
			pGuildDescInfo += len;
		}

		len = pGuildDescInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 1); // MemberNum
	for (size_t i = 0; i < 1; i++)
	{ // Member
		BYTE *pMember = p;
		Write16(pMember, 0); // len

		Write32(pMember, 10001);							 // Uin
		WriteString(pMember, Client->NickName, MaxNickName); // NickName[]
		Write16(pMember, 0);								 // Duty
		Write8(pMember, 0);									 // OnlineFlag
		WriteUn(pMember, 0);								 // LastLogin
		Write32(pMember, 0);								 // MemProsperity
		Write32(pMember, 0);								 // LastEnlistedTime
		Write8(pMember, 0);									 // EnlistFlag
		Write16(pMember, 0);								 // WonderLandDuty
		Write32(pMember, 0);								 // TotalWlDegree
		Write32(pMember, 0);								 // WeeklyWlDegree
		Write32(pMember, 0);								 // Donate
		Write32(pMember, 0);								 // SeasonZhanxun
		Write32(pMember, 0);								 // WeeklyContributeValue
		Write32(pMember, 0);								 // WeeklyProsperContributeValue
		Write32(pMember, 0);								 // MonthlyProsperContributeValue
		Write32(pMember, 0);								 // CommonParas[]
		Write32(pMember, 0);								 // TimeJoinedGuild
		Write8(pMember, 0);									 // AppliedChampionship

		len = pMember - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // LogNum
	for (size_t i = 0; i < 0; i++)
	{ // Log
		BYTE *pLog = p;
		Write16(pLog, 0); // len

		Write32(pLog, 0); // Time
		Write8(pLog, 0);  // LogLen

		len = pLog - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // Duty
		BYTE *pDuty = p;
		Write16(pDuty, 0); // len

		Write16(pDuty, 0); // Dutys[]
		for (size_t i = 0; i < 0; i++)
		{ // AutoPromLimit
			BYTE *pAutoPromLimit = pDuty;
			Write16(pAutoPromLimit, 0); // len

			Write32(pAutoPromLimit, 0); // ScoreLimit
			Write32(pAutoPromLimit, 0); // ProsperityLimit

			len = pAutoPromLimit - pDuty;
			Set16(pDuty, (WORD)len);
			pDuty += len;
		}
		for (size_t i = 0; i < 0; i++)
		{ // ApplyPromLimit
			BYTE *pApplyPromLimit = pDuty;
			Write16(pApplyPromLimit, 0); // len

			Write32(pApplyPromLimit, 0); // ScoreLimit
			Write32(pApplyPromLimit, 0); // ProsperityLimit

			len = pApplyPromLimit - pDuty;
			Set16(pDuty, (WORD)len);
			pDuty += len;
		}
		Write8(pDuty, 0); // DutyTitle[]

		len = pDuty - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen
	Write32(p, 0); // Contribute[]
	Write32(p, 0); // SelfJoinGuildTime
	Write16(p, 0); // SelfApplyDuty
	Write8(p, 0);  // HasAward
	Write32(p, 0); // CurrentOnLineGuildMembers
	Write32(p, 0); // LastWeekProsper

	len = p - buf;
	SendToClient(Client, 15003, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGuildNotes(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT GuildID = Read32(p);
	UINT Time = Read32(p);
}

void ResponseGuildNotes(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	Write16(p, 0);	   // ResultID
	WriteString(p, 0); // Notes[]

	len = p - buf;
	SendToClient(Client, 24309, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestChangeMemberDuty(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT DstUin = Read32(p);
	USHORT NewDuty = Read16(p);
	int OperationChannelType = Read32(p);
}

void ResponseChangeMemberDuty(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	WriteString(p, 0); // SrcNickName[]
	Write32(p, 0);	   // DstUin
	WriteString(p, 0); // DstNickName[]
	Write16(p, 0);	   // NewDuty
	Write16(p, 0);	   // ResultID
	Write16(p, 0);	   // ReasonLen
	Write16(p, 0);	   // Right
	Write16(p, 0);	   // OldDuty

	len = p - buf;
	SendToClient(Client, 15006, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyChangeMemberDutyResult(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	WriteString(p, 0); // SrcNickName[]
	Write32(p, 0);	   // DstUin
	WriteString(p, 0); // DstNickName[]
	Write16(p, 0);	   // NewDuty
	Write16(p, 0);	   // Right

	len = p - buf;
	SendToClient(Client, 15204, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestModifyDutyRight(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT Duty = Read16(p);
	USHORT Right = Read16(p);
}

void ResponseModifyDutyRight(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write32(p, 0); // Time
	Write16(p, 0); // Duty
	Write16(p, 0); // Right
	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 15007, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestModifyGuildAttriute(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT NewAttr = Read16(p);
	char NewGuildType = Read8(p);
	UINT GuildQQGroup = Read32(p);
	{ // WorkDayActiveTimeSeg
		BYTE *pWorkDayActiveTimeSeg = p;
		len = Read16(pWorkDayActiveTimeSeg);

		USHORT BeginTimeInt = Read16(pWorkDayActiveTimeSeg);
		USHORT EndTimeInt = Read16(pWorkDayActiveTimeSeg);

		p += len;
	}
	{ // WeekendActiveTimeSeg
		BYTE *pWeekendActiveTimeSeg = p;
		len = Read16(pWeekendActiveTimeSeg);

		USHORT BeginTimeInt = Read16(pWeekendActiveTimeSeg);
		USHORT EndTimeInt = Read16(pWeekendActiveTimeSeg);

		p += len;
	}
	{ // CardOtherInfo
		BYTE *pCardOtherInfo = p;
		len = Read16(pCardOtherInfo);

		int MainZone1 = Read32(pCardOtherInfo);
		int MainZone2 = Read32(pCardOtherInfo);
		int MainMode1 = Read32(pCardOtherInfo);
		int MainMode2 = Read32(pCardOtherInfo);
		int OtherInterest1 = Read32(pCardOtherInfo);
		int OtherInterest2 = Read32(pCardOtherInfo);

		p += len;
	}
}

void ResponseModifyGuildAttriute(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write16(p, 0); // NewAttr
	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen
	Write8(p, 0);  // NewGuildType
	Write32(p, 0); // GuildQQGroup
	{			   // WorkDayActiveTimeSeg
		BYTE *pWorkDayActiveTimeSeg = p;
		Write16(pWorkDayActiveTimeSeg, 0); // len

		Write16(pWorkDayActiveTimeSeg, 0); // BeginTimeInt
		Write16(pWorkDayActiveTimeSeg, 0); // EndTimeInt

		len = pWorkDayActiveTimeSeg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // WeekendActiveTimeSeg
		BYTE *pWeekendActiveTimeSeg = p;
		Write16(pWeekendActiveTimeSeg, 0); // len

		Write16(pWeekendActiveTimeSeg, 0); // BeginTimeInt
		Write16(pWeekendActiveTimeSeg, 0); // EndTimeInt

		len = pWeekendActiveTimeSeg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // CardOtherInfo
		BYTE *pCardOtherInfo = p;
		Write16(pCardOtherInfo, 0); // len

		Write32(pCardOtherInfo, 0); // MainZone1
		Write32(pCardOtherInfo, 0); // MainZone2
		Write32(pCardOtherInfo, 0); // MainMode1
		Write32(pCardOtherInfo, 0); // MainMode2
		Write32(pCardOtherInfo, 0); // OtherInterest1
		Write32(pCardOtherInfo, 0); // OtherInterest2

		len = pCardOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // ModifyFirstTime

	len = p - buf;
	SendToClient(Client, 15008, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestModifyGuildNotes(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT NotesLen = Read16(p);
}

void ResponseModifyGuildNotes(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write16(p, 0); // NotesLen
	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 15011, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestModifyGuildDesc(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT DescLen = Read16(p);
}

void ResponseModifyGuildDesc(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write16(p, 0); // DescLen
	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 15012, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestPlayerGuildInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	bool Flag = Read8(p);
	if (DstUin == 3452816845) {
		return;
	}
	if (!GetClient(DstUin)){
		ResponseStartGame(Client, "该用户不在线");
		return;
	}
	if (GetClient(DstUin)->RoomID<=0) {
		ResponseStartGame(Client, "该玩家不在房间中");
		return;
	}
	RoomNode* Room = GetRoom(GetClient(DstUin)->RoomID);
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
	LeaveRoom(Client, 0);
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient == NULL)
		{
			Client->RoomID = GetClient(DstUin)->RoomID;
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

void ResponsePlayerGuildInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	Write32(p, 0);	   // DstUin
	WriteString(p, 0); // Name[]
	Write16(p, 0);	   // Duty
	Write32(p, 0);	   // GuildID
	Write32(p, 0);	   // Medal
	WriteString(p, 0); // DutyTitle[]
	Write8(p, 0);	   // Flag
	Write32(p, 0);	   // WonderLandID

	len = p - buf;
	SendToClient(Client, 15015, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGuildLogInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
}

void ResponseGuildLogInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Uin
	Write16(p, 0); // LogNum

	Write8(p, 1); // LogNum
	for (size_t i = 0; i < 1; i++)
	{ // Log
		BYTE *pLog = p;
		Write16(pLog, 0); // len

		Write32(pLog, 0); // Time
		Write8(pLog, 0);  // LogLen

		len = pLog - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 15016, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestCheckGuildName(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	char *GuildName = ReadString(p, MaxNickName);
	UINT SlefGuildID = Read32(p);
	if (strlen(GuildName) == 0)
	{
		ResponseStartGame(Client, "车队名称不能为空");
	}
	else if (strlen(GuildName) > 16)
	{
		ResponseStartGame(Client, "车队名称超过最大长度");
	}
	else
	{
		ResponseCheckGuildName(Client);
	}
}

void ResponseCheckGuildName(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write8(p, 0);			 // GuildName[]
	Write16(p, 0);			 // ResultID
	Write16(p, 0);			 // ReasonLen

	len = p - buf;
	SendToClient(Client, 15017, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyDismissGuildResult(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	WriteString(p, 0); // GuildName[]
	Write16(p, 0);	   // ReasonLen

	len = p - buf;
	SendToClient(Client, 15217, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestSavingQQshow(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	char QQshowName[256];
	short PropID = Read16(p);
	short Seq = Read16(p);
	UCHAR Finished = Read8(p);
	UINT QQshowBufferSize = Read32(p);
	UCHAR UploadType = Read8(p);

	ResponseSavingQQshow(Client);
}

void ResponseSavingQQshow(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);	   // Uin
	WriteString(p, 0); // QQshowName[]
	Write16(p, 0);	   // PropID
	Write16(p, 0);	   // ResultID
	Write8(p, 0);	   // ReasonLen
	Write8(p, 0);	   // UploadType

	len = p - buf;
	SendToClient(Client, 356, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestC2GCheckWonderLandName(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	char *DstWonderLandName = ReadString(p, MaxNickName);

	if (strlen(DstWonderLandName) == 0)
	{
		ResponseStartGame(Client, "城邦名称不能为空");
	}
	else if (strlen(DstWonderLandName) > 16)
	{
		ResponseStartGame(Client, "城邦名称超过最大长度");
	}
	else
	{
		ResponseC2GCheckWonderLandName(Client);
	}
}

void ResponseC2GCheckWonderLandName(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen
	Write8(p, 0);  // DstWonderLandName[]

	len = p - buf;
	SendToClient(Client, 895, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestC2GCreateWonderLand(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	char *WonderLandName = ReadString(p, MaxNickName);
	int ExecuterNum = Read8(p);
	for (int i = 0; i < ExecuterNum; i++)
	{
		UINT Executer = Read32(p);
	}
	bool UseDonate = Read8(p);
	ResponseC2GCreateWonderLand(Client, WonderLandName);
}

void ResponseC2GCreateWonderLand(ClientNode *Client, char *WonderLandName)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);								 // ResultID
	Write16(p, 0);								 // ReasonLen
	WriteString(p, WonderLandName, MaxNickName); // WonderLandName[]
	Write32(p, 0);								 // WonderLandID
	Write8(p, 0);								 // ExecuterNum
	// Write32(p, 0); //Executer[]
	Write8(p, 0); // UseDonate

	len = p - buf;
	SendToClient(Client, 881, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestC2GGetGuildChallengeInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstGuildID = Read32(p);
	UCHAR GetExInfo = Read8(p);
	UCHAR GetLog = Read8(p);
	UCHAR Type = Read8(p);
	ResponseC2GGetGuildChallengeInfo(Client);
}

void ResponseC2GGetGuildChallengeInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, 1);			 // DstGuildID
	{						 // Info
		BYTE *pInfo = p;
		Write16(pInfo, 0); // len

		Write16(pInfo, 200);		 // MapID
		Write16(pInfo, 1);			 // Rank
		Write32(pInfo, 0);			 // UseTime
		Write32(pInfo, Client->Uin); // HeroUin
		Write32(pInfo, 12720);		 // KartID
		Write8(pInfo, 0);			 // HeroNameLen
		Write32(pInfo, 0);			 // Victory
		Write32(pInfo, 0);			 // Failure
		Write32(pInfo, 0);			 // DayChallenge
		Write16(pInfo, 0);			 // RecentMapID
		Write16(pInfo, 0);			 // RecentRankID
		Write32(pInfo, 0);			 // CheerValue
		Write32(pInfo, 0);			 // DayCheerCnt
		Write32(pInfo, 0);			 // DayCheerValue

		len = pInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // Addition
		BYTE *pAddition = p;
		Write16(pAddition, 0); // len

		Write16(pAddition, 0); // ExpAddition
		Write16(pAddition, 0); // WageAddition

		len = pAddition - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HasExInfo
	for (size_t i = 0; i < 0; i++)
	{ // ExInfo
		BYTE *pExInfo = p;
		Write16(pExInfo, 0); // len

		Write16(pExInfo, 0); // LastSeasonMapID
		Write16(pExInfo, 0); // LastSeasonRank
		Write16(pExInfo, 0); // HistoryBestMapID
		Write16(pExInfo, 0); // HistoryBestRank
		Write32(pExInfo, 0); // TotalVictory
		Write32(pExInfo, 0); // TotalFailure
		Write32(pExInfo, 0); // HateGuildID

		len = pExInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // LogNum
	for (size_t i = 0; i < 0; i++)
	{ // Log
		BYTE *pLog = p;
		Write16(pLog, 0); // len

		Write16(pLog, 0);	  // SeasonID
		Write8(pLog, 0);	  // Type
		Write32(pLog, 0);	  // SrcGuildID
		WriteString(pLog, 0); // SrcGuildName[]
		Write32(pLog, 0);	  // Uin
		WriteString(pLog, 0); // NickName[]
		Write32(pLog, 0);	  // Kart
		Write16(pLog, 0);	  // MapID
		Write16(pLog, 0);	  // RankID
		Write32(pLog, 0);	  // UseTime
		Write32(pLog, 0);	  // Time

		len = pLog - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // Type

	len = p - buf;
	SendToClient(Client, 11211, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestBeginSailTreasure(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int Type = Read32(p);
	ResponseBeginSailTreasure(Client, Type);
}

void ResponseBeginSailTreasure(ClientNode *Client, int Type)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, Type);		 // Type
	{						 // SailTreasureBeginInfo
		BYTE *pSailTreasureBeginInfo = p;
		Write16(pSailTreasureBeginInfo, 0); // len

		Write32(pSailTreasureBeginInfo, 0); // AwardCount
		Write32(pSailTreasureBeginInfo, 0); // PlayerBornPosition
		Write32(pSailTreasureBeginInfo, 0); // GenerateTreasurePosNum
		for (size_t i = 0; i < 0; i++)
		{ // TreasureBoxIDAndPossition
			BYTE *pTreasureBoxIDAndPossition = pSailTreasureBeginInfo;
			Write16(pTreasureBoxIDAndPossition, 0); // len

			Write32(pTreasureBoxIDAndPossition, 0); // BoxID
			Write32(pTreasureBoxIDAndPossition, 0); // Position
			Write8(pTreasureBoxIDAndPossition, 0);	// HasGot
			Write32(pTreasureBoxIDAndPossition, 0); // Echo

			len = pTreasureBoxIDAndPossition - pSailTreasureBeginInfo;
			Set16(pSailTreasureBeginInfo, (WORD)len);
			pSailTreasureBeginInfo += len;
		}

		len = pSailTreasureBeginInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // SailTreasureTimeGap

	len = p - buf;
	SendToClient(Client, 389, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestOverSailTreasure(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	NotifySailTreasureOver(Client, 0);
	ResponseOverSailTreasure(Client);
}

void ResponseOverSailTreasure(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin

	len = p - buf;
	SendToClient(Client, 395, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifySailTreasureOver(ClientNode *Client, int Type)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write8(p, Type);		 // Type

	len = p - buf;
	SendToClient(Client, 392, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestEnter3DRoom(ClientNode* Client)
{

	ChatRoomNode* ChatRoom = GetChatRoom(4);
	USHORT SceneID;
	if (!ChatRoom)
	{
		return;
	}
	for (char i = 0; i < 99; i++)
	{
		if (!ChatRoom->Player[i])
		{
			Client->ChatRoomID = 4;
			ChatRoom->Player[i] = Client;
			ChatRoom->CurrPlayerNum++;

			SceneID = ChatRoom->SceneID;
			int x = ChatRoom->x;
			int y = ChatRoom->y;
			int z = ChatRoom->z;
			NotifyNPCInfo(Client);
			ResponseEnterOutdoor(Client, SceneID, x, y, z);
			NotifyGuildRoomMoreInfo(Client);
			NotifyChairInfo(Client);

			//通知其它玩家
			for (char i2 = 0; i2 < 6; i2++)
			{
				ClientNode* RoomClient = ChatRoom->Player[i2];
				if (RoomClient && RoomClient != Client)
				{
					NotifyEnterOutdoor(RoomClient, ChatRoom, Client, i);
				}
			}

			break;
		}
	}
}
void RequestOutDoorEggInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{ //Lupin
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	NotifyOutDoorEggInfo(Client);
	ResponseOutDoorEggInfo(Client, Uin);

}
void ResponseOutDoorEggInfo(ClientNode* Client, UINT Uin)
{//Lupin
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Uin); //Uin
	Write8(p, 0); //ReasonLen
	UINT Type[30], Pos[30];
	try
	{
		char FilePath[MAX_PATH];
		sprintf_s(FilePath, ".\\OutDoor\\ChatRoom\\%d.yml", Client->ChatRoomID);
		YAML::Node Config = YAML::LoadFile(FilePath);

		const size_t EggNumber = Config.size(); // Số lượng quả trứng trong tệp YAML
		Write32(p, EggNumber); // EggNumber
		for (size_t i = 0; i < EggNumber; i++)
		{
			YAML::Node item = Config[i];
			Type[i] = item["Type"].as<UINT>();
			Pos[i] = item["Pos"].as<UINT>();
			BYTE* pEggInfo = p;
			Write16(pEggInfo, 0); //len
			Write32(pEggInfo, Pos[i]); //EggPosition
			Write32(pEggInfo, Type[i]); //EggTypeID || 0: Trứng vàng , 1: Trứng bạc , 2: Trứng xanh, 3: Quái đỏ , 4: Quái xanh , 5: Niên thú
			len = pEggInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	catch (const std::exception&)
	{
		Z_ERROR("ChatRoom: %d chua duoc them EggInfo\n", Client->ChatRoomID);

	}

	len = p - buf;
	SendToClient(Client, 169, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyOutDoorEggInfo(ClientNode* Client)
{//Lupin
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	UINT Type[30], Pos[30];
	try
	{
		char FilePath[MAX_PATH];
		sprintf_s(FilePath, ".\\OutDoor\\ChatRoom\\%d.yml", Client->ChatRoomID);
		YAML::Node Config = YAML::LoadFile(FilePath);

		const size_t EggNumber = Config.size(); // Số lượng quả trứng trong tệp YAML
		Write32(p, EggNumber); // EggNumber
		for (size_t i = 0; i < EggNumber; i++)
		{
			YAML::Node item = Config[i];
			Type[i] = item["Type"].as<UINT>();
			Pos[i] = item["Pos"].as<UINT>();
			BYTE* pEggInfo = p;
			Write16(pEggInfo, 0); //len
			Write32(pEggInfo, Pos[i]); //EggPosition
			Write32(pEggInfo, Type[i]); //EggTypeID || 0: Egg Gold , 1: Egg Silver , 2: Egg Blue, 3: Red Monster , 4: Blue Monster
			len = pEggInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	catch (const std::exception&)
	{
		Z_ERROR("ChatRoom: %d chua duoc them EggInfo\n", Client->ChatRoomID);

	}
	Write8(p, 0); //Type

	len = p - buf;
	SendToClient(Client, 580, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestOutDoorSmashEgg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{ //Lupin
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	{ //EggInfo
		BYTE* pEggInfo = p;
		len = Read16(pEggInfo);

		UINT EggPosition = Read32(pEggInfo);
		int EggTypeID = Read32(pEggInfo);

		p += len;
		Z_INFO("Egg Position: %d", EggPosition);

		NotifyOutDoorSmashEgg(Client, Client->Uin, EggPosition, EggTypeID);
		ResponseOutDoorSmashEgg(Client, Uin, EggPosition, EggTypeID);
		NotifyOutDoorEggInfo(Client);
		ChatRoomNode* ChatRoom = GetChatRoom(Client->ChatRoomID);
		do
		{
			ChatRoomNode* ChatRoom = GetChatRoom(Client->ChatRoomID);
			if (!ChatRoom)
			{
				break;
			}
			for (char i = 0; i < 99; i++)
			{
				ClientNode* ChatRoomClient = ChatRoom->Player[i];
				if (ChatRoomClient && ChatRoomClient != Client)
				{
					NotifyOutDoorSmashEgg(ChatRoomClient, Uin, EggPosition, EggTypeID);
					NotifyOutDoorEggInfo(ChatRoomClient);

				}
			}
		} while (false);
	}

}
void ResponseOutDoorSmashEgg(ClientNode* Client, UINT Uin, UINT EggPosition, int EggTypeID)
{//Lupin
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	{ //EggInfo
		BYTE* pEggInfo = p;
		Write16(pEggInfo, 0); //len

		Write32(pEggInfo, EggPosition); //EggPosition
		Write32(pEggInfo, EggTypeID); //EggTypeID

		len = pEggInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	UINT ItemID;
	int ItemQuatity;
	int AvailPeriod;
	int KeyID = 0;
	int KeyNum = 0;
	bool hasKey = false;
	try
	{
		char FilePath[MAX_PATH];
		sprintf_s(FilePath, ".\\OutDoor\\EggAward\\%d.yml", EggTypeID);
		YAML::Node Config = YAML::LoadFile(FilePath);
		int r = (int)rand() % Config.size();
		UINT index = 0;
		for (int i = 0; i < Config["Key"].size(); i++)
		{
			KeyID = Config["Key"][i]["KeyID"].as<int>();
			KeyNum = Config["Key"][i]["KeyNum"].as<int>();

		}
		ItemID = Config["Items"][index]["ItemID"].as<UINT>();
		ItemQuatity = Config["Items"][index]["ItemNum"].as<int>();
		AvailPeriod = Config["Items"][index]["AvailPeriod"].as<int>();
	}

	catch (const std::exception&)
	{
		Z_ERROR("EggType: %d", EggTypeID);

	}
	int ItemNum = PlayerDB_GetItemNum(Client->Uin, KeyID);
	PlayerDB_UseItem(Client->Uin, KeyID, KeyNum);
	NotifyUseItemResult(Client, KeyID, 1, ItemNum - KeyNum);

	PlayerDB_AddItem(Client->Uin, ItemID, ItemQuatity, AvailPeriod, false);
	int NumItem = PlayerDB_GetItemNum(Client->Uin, ItemID);
	int AvailItem = PlayerDB_GetAvailPeriod(Client->Uin, ItemID);
	ItemInfo Items[1];
	Items[0].ItemID = ItemID;
	Items[0].ItemNum = NumItem;
	Items[0].AvailPeriod = AvailItem;
	Items[0].ObtainTime = time(nullptr);
	Items[0].Status = 0;
	Items[0].OtherAttribute = 0;
	Items[0].ItemType = 0;
	NotifyClientAddItem(Client, 1, Items);

	Write32(p, KeyID); //UseItemID
	Write32(p, KeyNum); //UseItemNum
	Write32(p, 0); //AddExperience
	Write32(p, 0); //AddMoney
	Write32(p, 0); //AddSuperMoney
	Write16(p, 1); //ItemNum
	for (size_t i = 0; i < 1; i++)
	{ //AddItemInfo
		BYTE* pAddItemInfo = p;
		Write16(pAddItemInfo, 0); //len

		Write32(pAddItemInfo, ItemID); //ItemID
		Write32(pAddItemInfo, ItemQuatity); //ItemNum
		Write32(pAddItemInfo, AvailPeriod); //AvailPeriod
		Write8(pAddItemInfo, 0); //Status
		Write32(pAddItemInfo, time(nullptr)); //ObtainTime
		Write32(pAddItemInfo, 0); //OtherAttribute

		len = pAddItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //ReasonLen
	Write32(p, 0); //AddCoupons

	len = p - buf;
	SendToClient(Client, 170, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyOutDoorSmashEgg(ClientNode* Client, UINT Uin, UINT EggPosition, int EggTypeID)
{//Lupin
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Uin); //Uin
	{ //EggInfo
		BYTE* pEggInfo = p;
		Write16(pEggInfo, 0); //len

		Write32(pEggInfo, EggPosition); //EggPosition
		Write32(pEggInfo, EggTypeID); //EggTypeID

		len = pEggInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 579, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestC2GGetHRoomPhyInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	bool TargetType = Read8(p);
	UINT TargetID = Read32(p);
	ResponseC2GGetHRoomPhyInfo(Client, TargetType, TargetType);
}

void ResponseC2GGetHRoomPhyInfo(ClientNode* Client, bool TargetType, UINT TargetID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write8(p, TargetType); //TargetType
	Write16(p, 2000); //ServerID
	{ //PhyInfo
		BYTE* pPhyInfo = p;
		Write16(pPhyInfo, 0); //len

		Write32(pPhyInfo, inet_addr("127.0.0.1")); //ExtenalIP
		Write8(pPhyInfo, 1); //ExtTcpPortNum
		Write16(pPhyInfo, 27015); //ExternalTcpPort[]
		Write8(pPhyInfo, 1); //ExtUdpPortNum
		Write16(pPhyInfo, 27015); //ExternalUdpPort[]

		len = pPhyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 1); //HRoomID

	len = p - buf;
	SendToClient(Client, 11043, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}