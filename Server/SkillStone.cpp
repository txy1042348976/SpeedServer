#include "SkillStone.h"
#include <stdio.h>
#include <time.h>
#include <map>
#include <string>
#include <sys/stat.h>

#include "Database.h"
#include "Item.h"
#include "sqlite/sqlite3.h"
#include "yaml-cpp/yaml.h"

#include <iostream>

extern sqlite3 *PlayerDB;

void RequestInlaySkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	int KartID = Read32(Body);
	int StoneGrooveID = Read32(Body);
	int SkillStoneID = Read32(Body);
	int ID = -1;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		

	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;

		
		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (ID != -1)
		{
			sql = "UPDATE KartStone SET StoneUseOccaType=?,SkillStoneID=?  WHERE ID=? AND StoneGrooveID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, 0);
				sqlite3_bind_int(stmt, 2, SkillStoneID);
				sqlite3_bind_int(stmt, 3, ID);
				sqlite3_bind_int(stmt, 4, StoneGrooveID);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
	
	NotifyUpdateKartSkillStoneInfo(Client, KartID);
	ResponseInlaySkillStone(Client, KartID, StoneGrooveID, SkillStoneID);
}
void ResponseInlaySkillStone(ClientNode *Client, int KartID, int StoneGrooveID, int SkillStoneID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write16(p, 0); // ReasonLen
	Write32(p, KartID);
	Write32(p, StoneGrooveID);
	Write32(p, SkillStoneID);

	len = p - buf;
	SendToClient(Client, 901, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyUpdateKartSkillStoneInfo(ClientNode *Client, int KartID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int ID = -1;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;


		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (ID == -1)
		{
			return;
		}

		Write32(p, Client->Uin); // Uin
		{						 // KartStoneInfo
			BYTE* pKartStoneInfo = p;
			Write16(pKartStoneInfo, 0); // len

			Write32(pKartStoneInfo, KartID); // KartID

			BYTE* pStoneGrooveNum = pKartStoneInfo;
			Write32(pKartStoneInfo, 0); // StoneGrooveNum
			{
				int StoneGrooveNum = 0;
				const char* sql = NULL;
				sqlite3_stmt* stmt = NULL;
				int result;
				sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{ // StoneGrooveInfo
						StoneGrooveNum++;

						BYTE* pStoneGrooveInfo = pKartStoneInfo;
						Write16(pStoneGrooveInfo, 0); // len

						Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); // StoneUseOccaType
						Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); // SkillStoneID

						len = pStoneGrooveInfo - pKartStoneInfo;
						Set16(pKartStoneInfo, (WORD)len);
						pKartStoneInfo += len;
					}
				}
				else
				{
					Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				Set32(pStoneGrooveNum, StoneGrooveNum);
			}

			len = pKartStoneInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	

	len = p - buf;
	SendToClient(Client, 906, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyUpdateKartSkillStoneInfo2(ClientNode *Client, int KartID, int SkinID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int ID = -1;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;


		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (ID == -1)
		{
			return;
		}

		Write32(p, Client->Uin); // Uin
		{						 // KartStoneInfo
			BYTE* pKartStoneInfo = p;
			Write16(pKartStoneInfo, 0); // len

			Write32(pKartStoneInfo, SkinID); // KartID

			BYTE* pStoneGrooveNum = pKartStoneInfo;
			Write32(pKartStoneInfo, 0); // StoneGrooveNum
			{
				int StoneGrooveNum = 0;
				const char* sql = NULL;
				sqlite3_stmt* stmt = NULL;
				int result;
				sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{ // StoneGrooveInfo
						Client->SkillStoneList[StoneGrooveNum] = sqlite3_column_int(stmt, 1);
						StoneGrooveNum++;

						BYTE* pStoneGrooveInfo = pKartStoneInfo;
						Write16(pStoneGrooveInfo, 0); // len

						Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); // StoneUseOccaType
						Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); // SkillStoneID

						len = pStoneGrooveInfo - pKartStoneInfo;
						Set16(pKartStoneInfo, (WORD)len);
						pKartStoneInfo += len;
					}
				}
				else
				{
					Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				Set32(pStoneGrooveNum, StoneGrooveNum);
			}

			len = pKartStoneInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	

	len = p - buf;
	SendToClient(Client, 906, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyUpdateKartSkillStoneInfo4(ClientNode* Client, int KartID, int SkinID, int buffID)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;
	int ID = -1;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;


		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (ID == -1)
		{
			return;
		}

		Write32(p, Client->Uin); // Uin
		{						 // KartStoneInfo
			BYTE* pKartStoneInfo = p;
			Write16(pKartStoneInfo, 0); // len

			Write32(pKartStoneInfo, SkinID); // KartID
			if (buffID == 18554) {
				Write32(pKartStoneInfo, 2); // StoneGrooveNum
				{ // StoneGrooveInfo
					Client->SkillStoneList[0] = 21986;
					BYTE* pStoneGrooveInfo = pKartStoneInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, 0); // StoneUseOccaType
					Write32(pStoneGrooveInfo, 21986); // SkillStoneID

					len = pStoneGrooveInfo - pKartStoneInfo;
					Set16(pKartStoneInfo, (WORD)len);
					pKartStoneInfo += len;
				}
				{ // StoneGrooveInfo
					Client->SkillStoneList[1] = 21980;
					BYTE* pStoneGrooveInfo = pKartStoneInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, 0); // StoneUseOccaType
					Write32(pStoneGrooveInfo, 21980); // SkillStoneID

					len = pStoneGrooveInfo - pKartStoneInfo;
					Set16(pKartStoneInfo, (WORD)len);
					pKartStoneInfo += len;
				}
			}
			else if (buffID == 19753) {
				Write32(pKartStoneInfo, 3); // StoneGrooveNum
				{ // StoneGrooveInfo
					Client->SkillStoneList[0] = 21988;
					BYTE* pStoneGrooveInfo = pKartStoneInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, 0); // StoneUseOccaType
					Write32(pStoneGrooveInfo, 21988); // SkillStoneID

					len = pStoneGrooveInfo - pKartStoneInfo;
					Set16(pKartStoneInfo, (WORD)len);
					pKartStoneInfo += len;
				}
				{ // StoneGrooveInfo
					Client->SkillStoneList[1] = 21982;
					BYTE* pStoneGrooveInfo = pKartStoneInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, 0); // StoneUseOccaType
					Write32(pStoneGrooveInfo, 21982); // SkillStoneID

					len = pStoneGrooveInfo - pKartStoneInfo;
					Set16(pKartStoneInfo, (WORD)len);
					pKartStoneInfo += len;
				}
				{ // StoneGrooveInfo
					Client->SkillStoneList[2] = 21994;
					BYTE* pStoneGrooveInfo = pKartStoneInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, 0); // StoneUseOccaType
					Write32(pStoneGrooveInfo, 21994); // SkillStoneID

					len = pStoneGrooveInfo - pKartStoneInfo;
					Set16(pKartStoneInfo, (WORD)len);
					pKartStoneInfo += len;
				}
			}
			else {
				BYTE* pStoneGrooveNum = pKartStoneInfo;
				Write32(pKartStoneInfo, 0); // StoneGrooveNum
				{
					int StoneGrooveNum = 0;
					const char* sql = NULL;
					sqlite3_stmt* stmt = NULL;
					int result;
					sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
					result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
					if (result == SQLITE_OK)
					{
						sqlite3_bind_int(stmt, 1, ID);
						while (sqlite3_step(stmt) == SQLITE_ROW)
						{ // StoneGrooveInfo
							Client->SkillStoneList[StoneGrooveNum] = sqlite3_column_int(stmt, 1);
							StoneGrooveNum++;

							BYTE* pStoneGrooveInfo = pKartStoneInfo;
							Write16(pStoneGrooveInfo, 0); // len

							Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); // StoneUseOccaType
							Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); // SkillStoneID

							len = pStoneGrooveInfo - pKartStoneInfo;
							Set16(pKartStoneInfo, (WORD)len);
							pKartStoneInfo += len;
						}
					}
					else
					{
						Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
					}
					sqlite3_finalize(stmt);
					stmt = NULL;
					Set32(pStoneGrooveNum, StoneGrooveNum);
				}
			}
			

			len = pKartStoneInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}


	len = p - buf;
	SendToClient(Client, 906, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyUpdateKartSkillStoneInfo3(ClientNode *Client, int KartID, int SkinID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	{						 // KartStoneInfo
		BYTE *pKartStoneInfo = p;
		Write16(pKartStoneInfo, 0); // len

		Write32(pKartStoneInfo, SkinID); // KartID

		BYTE *pStoneGrooveNum = pKartStoneInfo;
		UINT StoneUseOccaType[] = {21988, 21982, 21994};
		Write32(pKartStoneInfo, 0); // StoneGrooveNum
		{
			for (int i = 0; i < 3; i++)
			{ // StoneGrooveInfo

				Client->SkillStoneList[i] = StoneUseOccaType[i];
				BYTE *pStoneGrooveInfo = pKartStoneInfo;
				Write16(pStoneGrooveInfo, 0); // len

				Write32(pStoneGrooveInfo, 0);					// StoneUseOccaType
				Write32(pStoneGrooveInfo, StoneUseOccaType[i]); // SkillStoneID

				len = pStoneGrooveInfo - pKartStoneInfo;
				Set16(pKartStoneInfo, (WORD)len);
				pKartStoneInfo += len;
			}
			Set32(pStoneGrooveNum, 3);
		}

		len = pKartStoneInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 906, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestAddStoneGroove(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	// WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int KartID = Read32(p);
	int CarveKnifeID = Read32(p);
	int StoneGrooveType = Read32(p);
#ifdef DEBUG
	Z_INFO("请求添加石槽： %d %d %d\n", KartID, CarveKnifeID, StoneGrooveType);
#endif
	PlayerDB_AddStoneGroove(Client, KartID, StoneGrooveType);
	NotifyUpdateKartSkillStoneInfo(Client, KartID);
	ResponseAddStoneGroove(Client, KartID, CarveKnifeID, StoneGrooveType);
}
void ResponseAddStoneGroove(ClientNode *Client, int KartID, int CarveKnifeID, int StoneGrooveType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);				 // ResultID
	Write32(p, 0);				 // ReasonLen
	Write32(p, KartID);			 // KartID
	Write32(p, CarveKnifeID);	 // CarveKnifeID
	Write32(p, StoneGrooveType); // StoneGrooveType

	len = p - buf;
	SendToClient(Client, 903, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestRemoveSkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	// WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int KartID = Read32(p);
	int StoneGrooveID = Read32(p);
	int RemovalToolID = Read32(p);
	int SkillStoneID = PlayerDB_RemoveSkillStone(Client, KartID, StoneGrooveID);
#ifdef DEBUG
	Z_INFO("请求删除技能石： %d %d %d\n", KartID, StoneGrooveID, RemovalToolID);
	Z_INFO("技能宝石ID:%d\n", SkillStoneID);
#endif
	ItemInfo item = PlayerDB_AddItem(Uin, SkillStoneID, 1, 0, 0);
	ItemInfo Items[1] = {item};
	NotifyClientAddItem(Client, 1, Items);
	NotifyUpdateKartSkillStoneInfo(Client, KartID);
	ResponseRemoveSkillStone(Client, KartID, StoneGrooveID, RemovalToolID, SkillStoneID);
}
void ResponseRemoveSkillStone(ClientNode *Client, int KartID, int StoneGrooveID, int RemovalToolID, int SkillStoneID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);			   // ResultID
	Write32(p, KartID);		   // KartID
	Write32(p, StoneGrooveID); // StoneGrooveID
	Write32(p, RemovalToolID); // RemovalToolID
	Write32(p, SkillStoneID);  // ObtainSkillStoneID

	len = p - buf;
	SendToClient(Client, 912, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}