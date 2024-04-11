#pragma once
#include "Message.h"

#ifdef ZingSpeed
constexpr int MaxRoomName = 66;
#else
constexpr int MaxRoomName = 22;
#endif

class RoomNode;
typedef void(WINAPI *RoomOnTimer)(RoomNode *);
class RoomNode
{
public:
	bool isFinsh = false;
	UINT ID;
	UINT RoomNo;
	char Name[MaxRoomName];
	USHORT RoomOwnerID;
	USHORT ServerID;
	USHORT BaseGameMode;
	USHORT SubGameMode;
	BYTE SeatNum;
	BYTE CurrentPlayerNum;
	BYTE TotalPlayerNum;
	USHORT Flag;
	char Password[16];
	ClientNode *Player[6];
	NpcBaseInfo *NPCPlayer[6];
	bool OpenNpc = false;
	UINT NPCNum = 0;
	//ClientNode PlayerReal[6];
	//std::map<int, std::map<int, int>> ClientPassedCheckPointPropS;
	HANDLE Timer;
	RoomOnTimer Callback;
	//
	UINT MapID;
	UINT startStatus;
	class
	{
	public:
		char Season;
		char Timeslot;
		char Weather;
	} STDynamicInfo;

	// Dance
	int SceneID;
	int MusicID;

	//
	USHORT CreateRoomType;
	USHORT ServerMatchType;
	USHORT TeamMatchType;
	BYTE ChatRoomType;
	BYTE PresetOBNum;
	BYTE AllowAutoStart;
	BYTE TalkRoomBGID;

	char BorderFlag;
	USHORT matchQueueType;
	char ShowFlag;
};
struct STDynamic
{
	UINT MapID;
	char Season;
	char Timeslot;
	char Weather;
};
RoomNode *GetRoom(long RoomID);
void LeaveRoom(ClientNode *Client, short LeaveType);
void PrintRoom();
void CreateRoomTimer(RoomNode *Room, int Timeout, RoomOnTimer Callback);
void ResponseGetInviteList(ClientNode *Client, BYTE *Body, size_t BodyLen); // 255

void RequestGetRoomList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetRoomList(ClientNode *Client, USHORT BaseGameMode, USHORT SubGameMode, USHORT StartRoomIdx, USHORT GetRoomNum, BYTE RoomFlag);
void RequestCreateRoom(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestCreateRoomMatch(ClientNode *Client);
void RequestEnterRoomMatch(ClientNode *Client);
void ResponseCreateRoom(ClientNode *Client, RoomNode *Room);

void RequestEnterRoom(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestQuestEnterRoom(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseEnterRoom(ClientNode *Client, RoomNode *Room, char SeatID);
void NotifyEnterRoom(ClientNode *Client, RoomNode *Room, ClientNode *RoomClient, char SeatID);

void RequestLeaveRoom(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseLeaveRoom(ClientNode *Client, short LeaveType);
void NotifyLeaveRoom(ClientNode *Client, UINT uin, char SeatID, bool IsChangeRoomOwner, short NewRoomOwnerID, short LeaveType);
void RequestStartGame(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestStartGameMatch(ClientNode *Client);
void RequestPrepareReadyMath(ClientNode *Client);
void ResponseStartGame(ClientNode *Client, const char *Reason);
void WINAPI OnBegin1(RoomNode *Room);

void RequestChangeMap(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseChangeMap(ClientNode *Client, RoomNode *Room);
void NotifyChangeMap(ClientNode *Client, UINT Uin, RoomNode *Room);

void RequestChangeSeatState(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseChangeSeatState(ClientNode *Client, BYTE SeatID, BYTE SeatState);

void RequestKickFromRoom(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseKickFromRoom(ClientNode *Client, UINT DstUin);

void RequestChangeReadyState(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseChangeReadyState(ClientNode *Client, bool ReadyState);

void RequestTalk(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseTalk(ClientNode *Client, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin);
void NotifyTalk(ClientNode *Client, UINT SrcUin, BYTE Type, BYTE ClientUseFlag, UINT DestPlayerUin, short ContentLength, const char *Content);
void NotifyClientUpdatePlayerID(ClientNode* Client, RoomNode* Room);
void NotifyChangeRoomOwner(ClientNode* Client, UINT RoomID, UINT NewRoomOwnerID, UINT ChangeType); //524
void RequestChangeRoomOwner(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseChangeRoomOwner(ClientNode* Client, UINT Uin, UINT NewOwner); //713