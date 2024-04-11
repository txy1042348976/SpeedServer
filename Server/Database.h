#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Message.h"
#include "sqlite/sqlite3.h"
#pragma comment(lib, "sqlite3.lib")
void InitDB();
void CloseDB();
void InitTaskInfo();
BOOL UserDB_Register(const char *Username, const char *Password);
DWORD UserDB_GetUin(const char *Username);
BOOL UserDB_CmpPassword(DWORD Uin, const char *Password);
void UserDB_ConnID(DWORD Uin, DWORD ConnID);
void PlayDB_ClearDress(UINT Uin, UINT SuitIdx, UINT GenderType);
void PlayDB_SaveDress(UINT Uin, UINT SuitIdx, UINT ItemID, UINT GenderType);
void PlayDB_UpdateItemStatus(UINT Uin, UINT ItemID, UINT Status);
ItemInfo PlayerDB_AddItem(DWORD Uin, DWORD ItemID, DWORD ItemNum, int AvailPeriod, bool Status);
ItemInfo PlayerDB_AddItem2(DWORD Uin, DWORD ItemID, DWORD ItemNum, int AvailPeriod, bool Status);
DWORD PlayerDB_GetMapRecord(DWORD Uin, int MapID);
bool PlayerDB_SetMapRecord(DWORD Uin, int MapID, DWORD Record);
void PlayerDB_GetCharacterInfo(ClientNode *Client);
void PlayerDB_UpdateCharacterInfo(ClientNode *Client);
void PlayerDB_UpdateMoneyExp(ClientNode *Client);
void PlayerDB_AddStoneGroove(ClientNode *Client, int KartID, int StoneGrooveType);
int PlayerDB_RemoveSkillStone(ClientNode *Client, int KartID, int StoneGrooveID);
BOOL PlayerDB_DeleteItem(DWORD Uin, int ItemID);

int PlayerDB_GetItemNum(DWORD Uin, DWORD ItemID);
int PlayerDB_GetAvailPeriod(DWORD Uin, DWORD ItemID);
int PlayerDB_UseItem(DWORD Uin, DWORD ItemID, int UseCount);

void PlayerDB_DoWareHouseOperation(UINT Uin, DWORD ItemID, int OpType);
ItemInfo PlayerDB_GetItemInfo(DWORD Uin, UINT ItemID);

BOOL PlayerDB_CheckNickNameUse(DWORD Uin, const char *Nickname);
// BOOL DB_BaseRun(const char* sql, SqlParam* params,UINT length, sqlite3* RunDB);
void loadNotKartStone(UINT ItemID, UINT ID);
void loadPrevKartStone(UINT ItemID, UINT ID);
void loadDefaultKartStone(UINT ItemID, UINT ID);
void loadKartStone(UINT ItemID, UINT ID,ClientNode* Client);

UINT PlayerDB_GetItemNum1(UINT Uin, UINT ItemID);
void PlayerDB_DelItem(UINT Uin, UINT ItemID);
void PlayerDB_Reduce(UINT Uin, UINT ItemID, int Num);