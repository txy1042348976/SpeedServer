#pragma once

#include "Message.h"

void InitChatRoom();
class ChatRoomNode
{
public:
	UINT ID;
	const char *Name;
	short CurrPlayerNum;
	short MaxPlayerNum;
	int RoomType;
	short SceneID;
	int x;
	int y;
	int z;
	ClientNode *Player[99];
};

constexpr int ChatRoomNum = 15;
extern ChatRoomNode ChatRooms[ChatRoomNum];

void RequestEnterOutdoor(ClientNode *Client, BYTE *Body, size_t BodyLen, int RoomID);
void RequestEnterOutdoor2(ClientNode *Client, int RoomID, int chatRoomID);

void RequestChangeChairState(ClientNode *Client, BYTE *Body, size_t BodyLen);

void LeaveOutdoor(ClientNode *Client);
void RequestLeaveOutdoor(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseLeaveOutdoor(ClientNode *Client);
void NotifyLeaveOutdoor(ClientNode *Client, UINT Uin);

void RequestHeroMove(ClientNode *Client, BYTE *Body, size_t BodyLen);

void NotifySynAllHeroPos(ClientNode *Client, MoveInfo *HeroMoveInfo); // 850
void RequestHeroPos(ClientNode *Client, BYTE *Body, size_t BodyLen);  // 350

void RequestUseHammer(ClientNode *Client, BYTE *Body, size_t BodyLen);			   // 358
void ResponseUseHammer(ClientNode *Client, int ItemID, UINT DstUin, bool DstType); // 358
void RequestHeroDoAction(ClientNode *Client, BYTE *Body, size_t BodyLen);		   // 355
void ResponseHeroDoAction(ClientNode *Client, short ActionID, int Para1);		   // 355
void NotifyHeroDoAction(ClientNode *Client, short ActionID, int Para1, UINT Uin);  // 855
void NotifyHeroMove(ClientNode *Client, MoveInfo *HeroMoveInfo);
void NotifyPlayerUseHammerInfo(ClientNode *Client); // 24641
ChatRoomNode *GetChatRoom(short ChatRoomID);
void RequestBeginSailTreasure(ClientNode *Client, BYTE *Body, size_t BodyLen); // 389
void ResponseBeginSailTreasure(ClientNode *Client, int Type);				   // 389
void RequestOverSailTreasure(ClientNode *Client, BYTE *Body, size_t BodyLen);  //?
void ResponseOverSailTreasure(ClientNode *Client);							   // 395
void NotifySailTreasureOver(ClientNode *Client, int Type);					   // 392

void RequestOutDoorEggInfo(ClientNode* Client, BYTE* Body, size_t BodyLen);//169
void ResponseOutDoorEggInfo(ClientNode* Client, UINT Uin);//169
void NotifyOutDoorEggInfo(ClientNode* Client);

void RequestOutDoorSmashEgg(ClientNode* Client, BYTE* Body, size_t BodyLen); //170
void ResponseOutDoorSmashEgg(ClientNode* Client, UINT Uin, UINT EggPosition, int EggTypeID); //170

void NotifyOutDoorSmashEgg(ClientNode* Client, UINT Uin, UINT EggPosition, int EggTypeID);
void RequestC2GGetHRoomPhyInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //11043
void ResponseC2GGetHRoomPhyInfo(ClientNode* Client, bool TargetType, UINT TargetID); //11043