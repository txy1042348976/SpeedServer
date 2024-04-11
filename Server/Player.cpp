#include "Message.h"
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <time.h>
#include <map>
#include <string>
#include <sys/stat.h>
#include <random>
#include "Database.h"
#include "Item.h"
#include "sqlite/sqlite3.h"
#include "yaml-cpp/yaml.h"
#include "GameServer.h"
#include <iostream>
#include <vector>
#include <mutex>
extern sqlite3 *PlayerDB;
std::map<UINT, AchieveInfo *> AchieveInfosems;			// 成就列表
std::map<UINT, ItemInfo> EndGame;						// 游戏结束了
std::map<UINT, std::map<UINT, DoBagItem *>> DoBagItems; // 置顶星标
INT AchievePointSum = 0;
std::mutex Achieve;
void LoadAchieveInfo()//成就
{
	YAML::Node Config = YAML::LoadFile(".\\Config\\AchieveInfo.yml");
	for (size_t i = 0; i < Config.size(); i++)
	{
		YAML::Node item = Config[i];
		AchieveInfo *Achieve = new AchieveInfo;
		Achieve->ID = item["ID"].as<UINT>();
		Achieve->ShowType = item["ShowType"].as<UINT>();
		Achieve->LogicType = item["LogicType"].as<UINT>();
		Achieve->Status = item["Status"].as<UINT>();
		Achieve->Progress = item["Progress"].as<UINT>();
		Achieve->TotalProgress = item["TotalProgress"].as<UINT>();
		AchievePointSum += item["TotalProgress"].as<UINT>();
		Achieve->FinTime = item["FinTime"].as<LONG>();
		Achieve->AchieveValue = item["AchieveValue"].as<LONG>();
		Achieve->AchieveFlagType = item["AchieveFlagType"].as<double>();
		Achieve->zhanwei = item["zhanwei"].as<UINT>();
		AchieveInfosems[Achieve->ID] = Achieve;
	}
	Z_INFO("成就列表加载完成[%d]!\n", AchieveInfosems.size());
}
void WritePlayerDBBaseInfo(BYTE *&p, UINT Uin)
{ // PlayerDBBaseInfo
	WORD len;
	BYTE *pPlayerDBBaseInfo = p;
	Write16(pPlayerDBBaseInfo, 0); // len

	std::string NickName = "";
	char Gender = 0;
	char Country = 0;
	UINT License = 0;
	UINT Experience = 0;
	UINT SuperMoney = 0;
	UINT Money = 0;
	UINT WinNum = 0;
	UINT SecondNum = 0;
	UINT ThirdNum = 0;
	UINT TotalNum = 0;
	UINT CurHonor = 0;
	UINT TotalHonor = 0;
	UINT TodayHonor = 0;
	UINT RelaxTime = 0;
	UINT MonthDurationBefor = 0;
	UINT MonthDurationCur = 0;
	UINT Charm = 0;
	UINT DurationGame = 0;
	UINT DanceExp = 0;
	UINT Coupons = 0;
	UINT Admiration = 0;
	UINT LuckMoney = 0;
	UINT TeamWorkExp = 0;
	UINT AchievePoint = AchievePointSum;
	UINT RegTime = 0;
	UINT SpeedCoin = 0;
	std::string Signature = "";
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;

		sql = "SELECT NickName,Gender,Country,License,Experience,SuperMoney,Money,WinNum,SecondNum,ThirdNum,TotalNum,CurHonor,TotalHonor,TodayHonor,RelaxTime,MonthDurationBefor,MonthDurationCur,Charm,DurationGame,DanceExp,Coupons,Admiration,LuckMoney,TeamWorkExp,AchievePoint,RegTime,Signature,SpeedCoin  FROM BaseInfo  WHERE Uin=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				NickName = (char*)sqlite3_column_text(stmt, 0);
				Gender = sqlite3_column_int(stmt, 1);
				Country = sqlite3_column_int(stmt, 2);
				License = sqlite3_column_int(stmt, 3);
				Experience = sqlite3_column_int(stmt, 4);
				SuperMoney = sqlite3_column_int(stmt, 5);
				Money = sqlite3_column_int(stmt, 6);
				WinNum = sqlite3_column_int(stmt, 7);
				SecondNum = sqlite3_column_int(stmt, 8);
				ThirdNum = sqlite3_column_int(stmt, 9);
				TotalNum = sqlite3_column_int(stmt, 10);
				CurHonor = sqlite3_column_int(stmt, 11);
				TotalHonor = sqlite3_column_int(stmt, 12);
				TodayHonor = sqlite3_column_int(stmt, 13);
				RelaxTime = sqlite3_column_int(stmt, 14);
				MonthDurationBefor = sqlite3_column_int(stmt, 15);
				MonthDurationCur = sqlite3_column_int(stmt, 16);
				Charm = sqlite3_column_int(stmt, 17);
				DurationGame = sqlite3_column_int(stmt, 18);
				DanceExp = sqlite3_column_int(stmt, 19);
				Coupons = sqlite3_column_int(stmt, 20);
				Admiration = sqlite3_column_int(stmt, 21);
				LuckMoney = sqlite3_column_int(stmt, 22);
				TeamWorkExp = sqlite3_column_int(stmt, 23);
				RegTime = sqlite3_column_int(stmt, 25);
				Signature = (char*)sqlite3_column_text(stmt, 26);
				SpeedCoin = sqlite3_column_int(stmt, 27);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	

	strcpy_s((char *)pPlayerDBBaseInfo, MaxNickName, NickName.c_str());
	pPlayerDBBaseInfo += MaxNickName;

	Write8(pPlayerDBBaseInfo, Gender | 0x48);
	Write8(pPlayerDBBaseInfo, Country);
	Write32(pPlayerDBBaseInfo, License);
	Write32(pPlayerDBBaseInfo, Experience);
	Write32(pPlayerDBBaseInfo, SuperMoney); // 点券
	Write32(pPlayerDBBaseInfo, Money);		// 酷币
	Write32(pPlayerDBBaseInfo, WinNum);
	Write32(pPlayerDBBaseInfo, SecondNum);
	Write32(pPlayerDBBaseInfo, ThirdNum);
	Write32(pPlayerDBBaseInfo, TotalNum);
	Write32(pPlayerDBBaseInfo, CurHonor);
	Write32(pPlayerDBBaseInfo, TotalHonor);
	Write32(pPlayerDBBaseInfo, TodayHonor);
	Write32(pPlayerDBBaseInfo, RelaxTime);
	Write32(pPlayerDBBaseInfo, MonthDurationBefor);
	Write32(pPlayerDBBaseInfo, MonthDurationCur);
	Write32(pPlayerDBBaseInfo, Charm);
	Write32(pPlayerDBBaseInfo, DurationGame);
	Write32(pPlayerDBBaseInfo, DanceExp);
	Write32(pPlayerDBBaseInfo, Coupons); // 消费券
	Write32(pPlayerDBBaseInfo, Admiration);
	Write32(pPlayerDBBaseInfo, LuckMoney);
	Write32(pPlayerDBBaseInfo, TeamWorkExp);
	Write32(pPlayerDBBaseInfo, AchievePoint);
	Write32(pPlayerDBBaseInfo, RegTime);

	strcpy_s((char *)pPlayerDBBaseInfo, MaxSignature, Signature.c_str());
	pPlayerDBBaseInfo += MaxSignature;

	Write32(pPlayerDBBaseInfo, 9999); // LuckyMatchPoint
	Write32(pPlayerDBBaseInfo, 9999); // LuckyMatchNum
	Write16(pPlayerDBBaseInfo, 9999); // LuckyMatchFirstNum
	Write8(pPlayerDBBaseInfo, 9999);  // LuckyMatchBestRand
	Write32(pPlayerDBBaseInfo, 9999); // LuckyMatchWinNum
	Write32(pPlayerDBBaseInfo, 9999); // FizzPointTotal
	Write16(pPlayerDBBaseInfo, 9999); // FizzPointDaily
	Write16(pPlayerDBBaseInfo, 9999); // FizzPointWeekly
	Write16(pPlayerDBBaseInfo, 9999); // FizzPointLastWeek
	Write8(pPlayerDBBaseInfo, 1);	  // FizzLotteryStatus
	Write32(pPlayerDBBaseInfo, 0);	  // FizzLastUpdateTime
	Write32(pPlayerDBBaseInfo, 37);	  // SNSLocaleCode
	Write32(pPlayerDBBaseInfo, 0);	  // AuctionRightUnlockTime
	Write8(pPlayerDBBaseInfo, 0);	  // ReserveFlag
	Write32(pPlayerDBBaseInfo, 9999); // CurConsumeScore
	Write32(pPlayerDBBaseInfo, 9999); // HistoryConsumeScore
	Write8(pPlayerDBBaseInfo, 0);	  // CrashModeSponsorID
	Write32(pPlayerDBBaseInfo, 9999); // Popularity
	Write32(pPlayerDBBaseInfo, 9999); // LadderMatchAchievePoint
	Write32(pPlayerDBBaseInfo, 0);	  // Cash
	Write32(pPlayerDBBaseInfo, 1);	  // PlayerGPMIdentity
	Write32(pPlayerDBBaseInfo, 9999); // PointTotalScore
	Write32(pPlayerDBBaseInfo, 9999); // totalMapMedalNum
	Write32(pPlayerDBBaseInfo, 9999); // lastMapMedalNum
	Write32(pPlayerDBBaseInfo, 9999); // curMapMedalNum
	Write32(pPlayerDBBaseInfo, 0);	  // curMapMedalSeasonId
	Write32(pPlayerDBBaseInfo, 0);	  // lastClearMapMedalTime
#ifndef ZingSpeed
	Write32(pPlayerDBBaseInfo, 0);		   // max2048Score
	Write32(pPlayerDBBaseInfo, 0);		   // curPlay2048LeftNum
	Write32(pPlayerDBBaseInfo, 0);		   // CheerValue
	Write16(pPlayerDBBaseInfo, 112);	   // SeasonID
	Write32(pPlayerDBBaseInfo, 0);		   // LastCheerValue
	Write32(pPlayerDBBaseInfo, 0);		   // SpeedBean
	Write32(pPlayerDBBaseInfo, SpeedCoin); // SpeedCoin
#endif

	len = pPlayerDBBaseInfo - p;
	Set16(p, len);
	p += len;
}

void WritePlayerGuildInfo(BYTE *&p, UINT Uin)
{ // PlayerGuildInfo
	WORD len;

	BYTE *pPlayerGuildInfo = p;
	Write16(pPlayerGuildInfo, 0); // len

	// WriteString(pPlayerGuildInfo,"MC测试团", MaxNickName);
	strcpy_s((char *)pPlayerGuildInfo, MaxNickName, ""); // GuildName
	pPlayerGuildInfo += MaxNickName;

	Write16(pPlayerGuildInfo, 0); // Duty
	Write32(pPlayerGuildInfo, 1); // GuildID
	Write32(pPlayerGuildInfo, 0); // Right
	Write32(pPlayerGuildInfo, 0); // Medal
	Write32(pPlayerGuildInfo, 0); // Contribute

	memset(pPlayerGuildInfo, 0, 9); // m_szDutyTitle[]:
	pPlayerGuildInfo += 9;

	len = pPlayerGuildInfo - p;
	Set16(p, len);
	p += len;
}

UINT WriteEquipedItem(BYTE *&p, UINT Uin)
{ // EquipedItem
	WORD len;

	BYTE *pItemNum = p;
	Write16(p, 1); // EquipedItemNum
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		size_t i = 0;
		UINT KartItemId = 0;
		sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				UINT ItemID = sqlite3_column_int(stmt, 0);
				BYTE Status = sqlite3_column_int(stmt, 3);

				if (Status)
				{ // PlayerItemInfo
					i++;
					if (GetItemType(ItemID) == EAIT_CAR)
					{
						ClientNode* uin = GetClient(Uin);
						if (uin)
						{
							if (uin->OrgKartId == uin->KartID)
							{
								KartItemId = uin->SkinID;
								ItemID = uin->SkinID;
							}
						}
					}

					BYTE* pItemInfo = p;
					Write16(pItemInfo, 0); // len
					Write32(pItemInfo, ItemID);
					Write32(pItemInfo, sqlite3_column_int(stmt, 1));
					Write32(pItemInfo, sqlite3_column_int(stmt, 2));
					Write8(pItemInfo, Status);
					Write32(pItemInfo, sqlite3_column_int(stmt, 4));
					Write32(pItemInfo, sqlite3_column_int(stmt, 5));
#ifndef ZingSpeed
					Write16(pItemInfo, 0); // ItemType
#endif
					len = pItemInfo - p;
					Set16(p, (WORD)len);
					p += len;
				}
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		Set16(pItemNum, (WORD)i);
		return KartItemId;
	}
	
	
}
void WriteVisbleInfo(BYTE*& p, UINT Uin, char SeatID, short PlayerID)
{ // PlayerRoomVisibleInfo
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int GuildVipLevel = 0;
	int GuildVipPoint = 0;
	int GuildVipLeftDays = 0;
	int LoverVipLevel = 0;
	int LoverVipPoint = 0;
	int LoverVipLeftDays = 0;
	int GardenLevel = 0;
	int GardenPoint = 0;
	int GardenExpireTime = 0;
	int VipLevel = 0;
	int CharmValueOfMonth = 0;
	int VipFlag = 0;
	int VipGrowRate = 0;
	int EmperorLevel = 0;
	int EmperorPoint = 0;
	int EmperorLeftDays = 0;
	int EmperorGrowRate = 0;
	int NobleLevel = 0;
	int NoblePoint = 0;
	int NobleLeftDays = 0;
	UINT PetId = 0;
	UINT KartId = 0;
	UINT NobleID = 0;
	sql = "SELECT GuildVipLevel,GuildVipPoint,GuildVipLeftDays,LoverVipLevel,LoverVipPoint,LoverVipLeftDays,GardenLevel,GardenPoint,GardenExpireTime,VipLevel,CharmValueOfMonth,VipFlag,VipGrowRate,EmperorLevel,EmperorPoint,EmperorLeftDays,EmperorGrowRate,NobleLevel,NoblePoint,NobleLeftDays,NobleID  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			GuildVipLevel = sqlite3_column_int(stmt, 0);
			GuildVipPoint = sqlite3_column_int(stmt, 1);
			GuildVipLeftDays = sqlite3_column_int(stmt, 2);
			LoverVipLevel = sqlite3_column_int(stmt, 3);
			LoverVipPoint = sqlite3_column_int(stmt, 4);
			LoverVipLeftDays = sqlite3_column_int(stmt, 5);
			GardenLevel = sqlite3_column_int(stmt, 6);
			GardenPoint = sqlite3_column_int(stmt, 7);
			GardenExpireTime = sqlite3_column_int(stmt, 8);
			VipLevel = sqlite3_column_int(stmt, 9);
			CharmValueOfMonth = sqlite3_column_int(stmt, 10);
			VipFlag = sqlite3_column_int(stmt, 11);
			VipGrowRate = sqlite3_column_int(stmt, 12);
			EmperorLevel = sqlite3_column_int(stmt, 13);
			EmperorPoint = sqlite3_column_int(stmt, 14);
			EmperorLeftDays = sqlite3_column_int(stmt, 15);
			EmperorGrowRate = sqlite3_column_int(stmt, 16);
			NobleLevel = sqlite3_column_int(stmt, 17);
			NoblePoint = sqlite3_column_int(stmt, 18);
			NobleLeftDays = sqlite3_column_int(stmt, 19);
			NobleID = sqlite3_column_int(stmt, 20);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	{//贵族等级优化
		if (VipFlag > 7) {
			VipFlag = 7;
		}
		if (VipLevel > 9) {
			VipLevel = 9;
		}
		if (NobleLevel > 7) {
			NobleLevel = 7;
		}
	}
	WORD len;
	int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_SPEEDMEMBER | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG;

	BYTE* pPlayerRoomVisibleInfo = p;
	Write16(pPlayerRoomVisibleInfo, 0); // len

	Write32(pPlayerRoomVisibleInfo, Uin);					// Uin
	Write32(pPlayerRoomVisibleInfo, Identity);				// Identity
	Write8(pPlayerRoomVisibleInfo, SeatID);					// SeatID
	Write8(pPlayerRoomVisibleInfo, GetClient(Uin)->TeamID); // TeamID
	Write16(pPlayerRoomVisibleInfo, PlayerID);				// PlayerID
	Write8(pPlayerRoomVisibleInfo, 0);						// Status

	WritePlayerDBBaseInfo(pPlayerRoomVisibleInfo, Uin);
	WritePlayerGuildInfo(pPlayerRoomVisibleInfo, Uin);
	// WriteEquipedItem(pPlayerRoomVisibleInfo, Uin);
	KartId = WriteEquipedItem2(pPlayerRoomVisibleInfo, Uin, &PetId);
	//KartId = WriteEquipedItem3(pPlayerRoomVisibleInfo, Uin, &PetId);
	int PetNum = PetId > 1 ? 1 : 0;
	BYTE* pPetNum = pPlayerRoomVisibleInfo;
	Write16(pPlayerRoomVisibleInfo, 0); // PetNum
	if (PetNum > 0)
	{
		{
			BYTE* pPetInfo = pPlayerRoomVisibleInfo;
			Write16(pPetInfo, 0);	  // len
			Write32(pPetInfo, Uin);	  // Uin
			Write32(pPetInfo, PetId); // PetId
			if (GetClient(Uin)) {
				memcpy(pPetInfo, GetClient(Uin)->Pet->Name, 23);
				pPetInfo += 23;
				Write8(pPetInfo, 1);   // IsProper
				Write16(pPetInfo, 30); // Level
				Write8(pPetInfo, GetClient(Uin)->Pet->Status);   // Status
			}
			else {
				memcpy(pPetInfo, "神宠", 23);
				pPetInfo += 23;
				Write8(pPetInfo, 1);   // IsProper
				Write16(pPetInfo, 30); // Level
				Write8(pPetInfo, GetPetStatus(30, PetId));   // Status
			}
			
			Write16(pPetInfo, 0);  // PetSkillNum
			for (size_t i = 0; i < 0; i++)
			{ // PetSkillList
				BYTE* pPetSkillList = pPetInfo;
				Write16(pPetSkillList, 0); // len

				Write16(pPetSkillList, 0);	  // SkillID
				Write8(pPetSkillList, i + 1); // Status
				Write8(pPetSkillList, 1);	  // Active
				Write32(pPetSkillList, 0);	  // Value

				len = pPetSkillList - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write32(pPetInfo, 1); // StrengLevel
			Write8(pPetInfo, 4);  // TopStatus
			{					  // CommonInfo
				BYTE* pCommonInfo = pPetInfo;
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
	}

	Set16(pPetNum, (WORD)PetNum);

	{ // PlayerGuildMatchInfo
		BYTE* pPlayerGuildMatchInfo = pPlayerRoomVisibleInfo;
		Write16(pPlayerGuildMatchInfo, 0); // len

		Write32(pPlayerGuildMatchInfo, 0); // SelfPoint
		Write32(pPlayerGuildMatchInfo, 0); // WinNum
		Write32(pPlayerGuildMatchInfo, 0); // TotalNum

		len = pPlayerGuildMatchInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}
	Write8(pPlayerRoomVisibleInfo, 1); // IsInTopList
	Write8(pPlayerRoomVisibleInfo, 0);	  // LoverRaceOrPro
	Write8(pPlayerRoomVisibleInfo, 0);	  // TmpEffectNum
	// m_stPlayerRoomVisibleInfo[].m_aiTmpEffectItems[]

	if (GetClient(Uin)) {
		Write8(pPlayerRoomVisibleInfo, GetClient(Uin)->OBState); // OBState
	}
	else {
		Write8(pPlayerRoomVisibleInfo, 0); // OBState
	}
	
	Write8(pPlayerRoomVisibleInfo, 0); // DebutOrX5
	Write8(pPlayerRoomVisibleInfo, 0); // RandKeyFlag

	Write16(pPlayerRoomVisibleInfo, VipFlag); // VipFlag

	Write8(pPlayerRoomVisibleInfo, 0); // HaveAppellation
	/*
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Type
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Level
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Status
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u8Difficulty
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_i32MapId
m_stPlayerRoomVisibleInfo[].m_astCurAppellation[].m_u32Value
	*/
	for (size_t i = 0; i < 0; i++)
	{ // Record
		BYTE* HaveAppellation = pPlayerRoomVisibleInfo;
		Write16(HaveAppellation, 0); // len

		Write8(HaveAppellation, 0);	 // Type
		Write8(HaveAppellation, 0);	 // Level
		Write8(HaveAppellation, 0);	 // Status
		Write8(HaveAppellation, 0);	 // Difficulty
		Write32(HaveAppellation, 0); // MapId
		Write32(HaveAppellation, 0); // Value

		len = HaveAppellation - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}

	{ // NobleInfo
		BYTE* pNobleInfo = pPlayerRoomVisibleInfo;
		Write16(pNobleInfo, 0); // len

		Write32(pNobleInfo, NobleID);			// NobleID
		Write8(pNobleInfo, NobleLevel);		// NobleLevel
		Write32(pNobleInfo, NoblePoint);	// NoblePoint
		Write32(pNobleInfo, NobleLeftDays); // NobleLeftDays

		len = pNobleInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}

	Write8(pPlayerRoomVisibleInfo, 0); // HasCarryWizard
	/*
m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_ushWizardID
m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_szNickname[]:
m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_ushWizardType
m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_bOrder
m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_bEvolutionBranch
m_stPlayerRoomVisibleInfo[].m_astCarryWizardInfo[].m_bIsHuanHua
	*/

	{ // GuildVipBaseInfo
		BYTE* pGuildVipBaseInfo = pPlayerRoomVisibleInfo;
		Write16(pGuildVipBaseInfo, 0); // len

		Write8(pGuildVipBaseInfo, GuildVipLevel);  // GuildVipLevel
		Write32(pGuildVipBaseInfo, GuildVipPoint); // GuildVipPoint

		len = pGuildVipBaseInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}

	Write32(pPlayerRoomVisibleInfo, 0); // ExFlag

	Write8(pPlayerRoomVisibleInfo, 0); // HaveLDMBaseInfo
	/*
m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u8Grade
m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u32Score
m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u32MaxScore
m_stPlayerRoomVisibleInfo[].m_stPlayerLDMBaseInfo[].m_u32MaxGrade
	*/

	Write8(pPlayerRoomVisibleInfo, 0); // HasWl
	/*
m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_uiWonderLandID
m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_szWonderLandName[]:
m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_ushWonderLandDuty
m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_ucSubType
m_stPlayerRoomVisibleInfo[].m_astPlayerWlVisibleInfo[].m_bGuildGrade
	*/
	Write8(pPlayerRoomVisibleInfo, 1); // HasLoverVip
	{								   // 情侣贵族
		BYTE* pLoverVipBaseInfo = pPlayerRoomVisibleInfo;
		Write16(pLoverVipBaseInfo, 0); // len

		Write8(pLoverVipBaseInfo, LoverVipLevel); // LoverVipLevel 情侣贵族等级
		Write32(pLoverVipBaseInfo, 100);		  // LoverVipPoint 情侣贵族点数
		Write8(pLoverVipBaseInfo, 4100);		  // GrowRate

		len = pLoverVipBaseInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}

	Write8(pPlayerRoomVisibleInfo, 0); // HasBattleModeSkillInfoList
	for (size_t i = 0; i < 0; i++)
	{ // BattleModeSkillInfoList
		BYTE* pBattleModeSkillInfoList = pPlayerRoomVisibleInfo;
		Write16(pBattleModeSkillInfoList, 0); // len

		Write8(pBattleModeSkillInfoList, 0); // SkillNum
		for (size_t i = 0; i < 0; i++)
		{ // SkillInfo
			BYTE* pSkillInfo = pBattleModeSkillInfoList;
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
	Write8(pPlayerRoomVisibleInfo, 1); // 花园
	{								   // PersonalGardenBaseInfo
		BYTE* pPersonalGardenBaseInfo = pPlayerRoomVisibleInfo;
		Write16(pPersonalGardenBaseInfo, 0); // len

		Write8(pPersonalGardenBaseInfo, GardenLevel); // GardenLevel 花园等级
		Write32(pPersonalGardenBaseInfo, 5);		  // GardenPoint 花园点数
		Write8(pPersonalGardenBaseInfo, 5);			  // GrowRate
		Write32(pPersonalGardenBaseInfo, 5);		  // GardenExpireTime
		{											  // SimpleInfo
			BYTE* pSimpleInfo = pPersonalGardenBaseInfo;
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
	{ // ConsumeVipInfo  魅力达人
		BYTE* pConsumeVipInfo = pPlayerRoomVisibleInfo;
		Write16(pConsumeVipInfo, 0); // len

		Write32(pConsumeVipInfo, VipLevel);			 // VipLevel 魅力等级
		Write32(pConsumeVipInfo, CharmValueOfMonth); // CharmValueOfMonth
		Write32(pConsumeVipInfo, 0);				 // SearchTreasureNums
		Write32(pConsumeVipInfo, 0);				 // GetTreasureNums

		len = pConsumeVipInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}
	Write8(pPlayerRoomVisibleInfo, 0);//HaveBaby
	{//LoverBabyInfo 情侣宝宝信息
		BYTE* pLoverBabyInfo = pPlayerRoomVisibleInfo;
		Write16(pLoverBabyInfo, 0); //len

		Write32(pLoverBabyInfo, 0);//LoverBabyID
		Write32(pLoverBabyInfo, 0);//ItemID
		Write32(pLoverBabyInfo, 0);//GrowValue
		Write32(pLoverBabyInfo, 0);//MentalValue
		Write32(pLoverBabyInfo, 0);//BabyProfileLevel
		Write32(pLoverBabyInfo, 0);//StrengthLevel
		Write32(pLoverBabyInfo, 0);//GrowLevel

		Write16(pLoverBabyInfo, 0);//LoverBabyStat
		Write16(pLoverBabyInfo, 0);//LoverBabyEquipStat
		Write16(pLoverBabyInfo, 0);//LoverBabyTransferdStatus

		len = pLoverBabyInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}
	{ // EmperorInfo //钻皇
		BYTE* pEmperorInfo = pPlayerRoomVisibleInfo;
		Write16(pEmperorInfo, 0); // len

		Write8(pEmperorInfo, EmperorLevel); // EmperorLevel 钻皇等级
		Write32(pEmperorInfo, 6666);		// EmperorPoint 钻皇点数
		Write32(pEmperorInfo, 30);			// EmperorLeftDays 钻皇到期时间
		Write8(pEmperorInfo, 31);			// EmperorGrowRate 钻皇成长率

		len = pEmperorInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}
	// 新加的
	Write16(pPlayerRoomVisibleInfo, 0); // RaceCardNums
	for (size_t i = 0; i < 0; i++)
	{ // RaceCardAddValueInfo
		BYTE* pRaceCardAddValueInfo = pPlayerRoomVisibleInfo;
		Write16(pRaceCardAddValueInfo, 0); // len

		Write32(pRaceCardAddValueInfo, 0); // RaceCardID
		Write32(pRaceCardAddValueInfo, 0); // AddValue

		len = pRaceCardAddValueInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}
	{ // GansterScoreInfo
		BYTE* pGansterScoreInfo = pPlayerRoomVisibleInfo;
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
		BYTE* pEquippedActiveKartInfo = pPlayerRoomVisibleInfo;
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
	Write8(pPlayerRoomVisibleInfo, 0); //HaveRaiseSecondBaby
	{ //SecondLoverBabyInfo 第二个情人宝宝信息
		BYTE* pSecondLoverBabyInfo = pPlayerRoomVisibleInfo;
		Write16(pSecondLoverBabyInfo, 0); //len

		Write32(pSecondLoverBabyInfo, 0); //LoverBabyID
		Write32(pSecondLoverBabyInfo, 0); //ItemID
		Write32(pSecondLoverBabyInfo, 0); //GrowValue
		Write32(pSecondLoverBabyInfo, 0); //MentalValue
		Write32(pSecondLoverBabyInfo, 0); //BabyProfileLevel
		Write32(pSecondLoverBabyInfo, 0); //StrengthLevel
		Write32(pSecondLoverBabyInfo, 0); //GrowLevel
		//WriteString(pSecondLoverBabyInfo, 0); //LoveBabyNickName[]
		Write16(pSecondLoverBabyInfo, 0); //LoverBabyStat
		Write16(pSecondLoverBabyInfo, 0); //LoverBabyEquipStat
		Write16(pSecondLoverBabyInfo, 0); //LoverBabyTransferdStatus

		len = pSecondLoverBabyInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}
	Write8(pPlayerRoomVisibleInfo, 0); // HasRankedMatchInfo
	for (size_t i = 0; i < 0; i++)
	{ // RankedMatchGradeInfo
		BYTE* pRankedMatchGradeInfo = pPlayerRoomVisibleInfo;
		Write16(pRankedMatchGradeInfo, 0); // len

		Write32(pRankedMatchGradeInfo, 27);	  // GradeLevel
		Write32(pRankedMatchGradeInfo, 9999); // ChildLevel
		Write32(pRankedMatchGradeInfo, 9999); // GradeScore
		Write32(pRankedMatchGradeInfo, 9999); // MaxGradeScore
		Write32(pRankedMatchGradeInfo, 9999); // TotalScore

		len = pRankedMatchGradeInfo - pPlayerRoomVisibleInfo;
		Set16(pPlayerRoomVisibleInfo, (WORD)len);
		pPlayerRoomVisibleInfo += len;
	}
	Write8(pPlayerRoomVisibleInfo, 0); // HasHuanLingChangeInfo
	for (size_t i = 0; i < 0; i++)
	{ // HuanLingSuitInfo
		BYTE* pHuanLingSuitInfo = pPlayerRoomVisibleInfo;
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
	Write8(pPlayerRoomVisibleInfo, 1);	// SpecialActivityInfoNum
	for (size_t i = 0; i < 1; i++)
	{ // SpecialActivityInfo
		BYTE* pSpecialActivityInfo = pPlayerRoomVisibleInfo;
		Write16(pSpecialActivityInfo, 0); // len

		Write8(pSpecialActivityInfo, 1); // ProfessionLicenseInfoNum
		for (size_t i = 0; i < 1; i++)
		{ // ProfessionLicenseInfo
			BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
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
	for (size_t i = 0; i < 0; i++)
	{ // ItemOrder
		BYTE* pItemOrder = pPlayerRoomVisibleInfo;
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
	for (size_t i = 0; i < 0; i++)
	{ // NiChangPavilionEquipItemInfo
		BYTE* pNiChangPavilionEquipItemInfo = pPlayerRoomVisibleInfo;
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
	for (size_t i = 0; i < 0; i++)
	{ // StarsFightingInfo
		BYTE* pStarsFightingInfo = pPlayerRoomVisibleInfo;
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
	Set16(p, len);
	p += len;
}

void RequestRegister2(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	const char *Nickname = (char *)Body;
	Body += MaxNickName;
	bool SetMysql = false;
	if (!PlayerDB_CheckNickNameUse(Uin, Nickname))
	{
		ResponseCheckName(Client, Nickname, "抱歉，该角色名称已被使用...\n请选择其他名称！");
		return;
	}
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		SetMysql = true;
	}
	if (SetMysql) {
	}
	else {
		sql = "BEGIN"; // 开始事务
		result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

		sql = "INSERT INTO Player (Uin,VipFlag,IsInTopList) VALUES (?,?,?);";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, 0x0106);
			sqlite3_bind_int(stmt, 3, 1);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	

	BYTE Gender = Read8(Body);
	BYTE Country = Read8(Body);
	if (SetMysql) {
	}
	else {
		sql = "INSERT INTO BaseInfo (Uin,NickName,Gender,Country) VALUES (?,?,?,?);";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_text(stmt, 2, Nickname, strlen(Nickname), SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 3, Gender);
			sqlite3_bind_int(stmt, 4, Country);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		sql = "INSERT INTO BaseGameTypeStatus (Uin) VALUES (?);";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	
	
	UINT RoleID = Read32(Body);
	UINT HairID = Read32(Body);
	UINT FaceID = Read32(Body);
	UINT CoatID = Read32(Body);
	UINT GloveID = Read32(Body);
	UINT TrousersID = Read32(Body);
	UINT KartID = Read32(Body);
	UINT ColorID = Read32(Body);

	PlayerDB_AddItem(Client->Uin, RoleID, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, HairID, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, FaceID, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, CoatID, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, GloveID, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, TrousersID, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, KartID, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, ColorID, 1, -1, true);
	// 3D 男装
	PlayerDB_AddItem(Client->Uin, 27178, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27179, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27180, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27181, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27182, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 127070, 1, -1, false);
	// 3D 女装
	PlayerDB_AddItem(Client->Uin, 27190, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27191, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27192, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27193, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 27194, 1, -1, true);
	PlayerDB_AddItem(Client->Uin, 128421, 1, -1, false);
	PlayerDB_AddItem(Client->Uin, 127080, 1, -1, false);

	YAML::Node NewUserConfig = getGameConfig()["NewUser"];
	UINT Mode = NewUserConfig["Mode"].as<UINT>();
	if (Mode == 0 || Mode == 2)
	{
		PlayerDB_AddItem(Client->Uin, 12720, 1, -1, false); // 雷诺
		PlayerDB_AddItem(Client->Uin, 15225, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 10020, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 96462, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 95405, 1, -1, false); // 飞碟

		// t3
		PlayerDB_AddItem(Client->Uin, 127438, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 119058, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 109560, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 102219, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 94838, 1, -1, false);	 // 飞碟
		PlayerDB_AddItem(Client->Uin, 85942, 1, -1, false);	 // 飞碟
		PlayerDB_AddItem(Client->Uin, 78292, 1, -1, false);	 // 飞碟
		PlayerDB_AddItem(Client->Uin, 71452, 1, -1, false);	 // 飞碟
		PlayerDB_AddItem(Client->Uin, 63429, 1, -1, false);	 // 飞碟
		PlayerDB_AddItem(Client->Uin, 127441, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 109563, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 110710, 1, -1, false); // 飞碟
		PlayerDB_AddItem(Client->Uin, 31941, 1, -1, false);	 // 飞碟
		PlayerDB_AddItem(Client->Uin, 31940, 1, -1, false);	 // 飞碟
		PlayerDB_AddItem(Client->Uin, 19294, 1, -1, false);	 // 飞碟

		PlayerDB_AddItem(Client->Uin, 21982, 1, -1, false); // 效率宝珠LV6
		PlayerDB_AddItem(Client->Uin, 21988, 1, -1, false); // 重生宝珠LV6
		PlayerDB_AddItem(Client->Uin, 21994, 1, -1, false); // 坚韧宝珠LV6
		PlayerDB_AddItem(Client->Uin, 22000, 1, -1, false); // 苍天宝珠LV6
		PlayerDB_AddItem(Client->Uin, 22006, 1, -1, false); // 大地宝珠LV6
		PlayerDB_AddItem(Client->Uin, 22012, 1, -1, false); // 无双宝珠LV6
		PlayerDB_AddItem(Client->Uin, 22018, 1, -1, false); // 后发宝珠LV6
		PlayerDB_AddItem(Client->Uin, 22024, 1, -1, false); // 愤怒宝珠LV6

		PlayerDB_AddItem(Client->Uin, 12376, 9999, 0, false); // 点火装置+1
		PlayerDB_AddItem(Client->Uin, 12377, 9999, 0, false); // 进气系统+1
		PlayerDB_AddItem(Client->Uin, 12378, 9999, 0, false); // 燃料系统+1
		PlayerDB_AddItem(Client->Uin, 12379, 9999, 0, false); // 悬挂系统+1
		PlayerDB_AddItem(Client->Uin, 12380, 9999, 0, false); // 引擎装置+1
		PlayerDB_AddItem(Client->Uin, 96597, 9999, 0, false); // 防护装置+1
		PlayerDB_AddItem(Client->Uin, 12386, 9999, 0, false); // 点火装置+2
		PlayerDB_AddItem(Client->Uin, 12387, 9999, 0, false); // 进气系统+2
		PlayerDB_AddItem(Client->Uin, 12388, 9999, 0, false); // 燃料系统+2
		PlayerDB_AddItem(Client->Uin, 12389, 9999, 0, false); // 悬挂系统+2
		PlayerDB_AddItem(Client->Uin, 12390, 9999, 0, false); // 引擎装置+2
		PlayerDB_AddItem(Client->Uin, 96599, 9999, 0, false); // 防护装置+2

		PlayerDB_AddItem(Client->Uin, 10207, 999, 0, false); // 小喇叭
		PlayerDB_AddItem(Client->Uin, 14728, 999, 0, false); // 小喇叭
		PlayerDB_AddItem(Client->Uin, 10791, 1, -1, false);	 // 临时驾照
		PlayerDB_AddItem(Client->Uin, 27137, 1, -1, false);	 // 皇权宝石
		PlayerDB_AddItem(Client->Uin, 30450, 1, -1, false);	 // 车队贵族驾照
		PlayerDB_AddItem(Client->Uin, 63505, 1, -1, false);	 // 后花园钥匙
		PlayerDB_AddItem(Client->Uin, 63555, 1, -1, false);	 // 钻皇宝石
		PlayerDB_AddItem(Client->Uin, 65736, 1, -1, false);	 // 紫钻
		PlayerDB_AddItem(Client->Uin, 31964, 1, -1, false);	 // 情侣宝石
		PlayerDB_AddItem(Client->Uin, 62787, 1, -1, false);	 // 魅力
	}
	if (Mode == 1 || Mode == 2)
	{
		YAML::Node ModeList = NewUserConfig["ModeList"];
		for (size_t i = 0; i < ModeList.size(); i++)
		{
			YAML::Node Mode = ModeList[i];
			PlayerDB_AddItem(Client->Uin, Mode["ItemID"].as<INT>(), Mode["ItemNum"].as<INT>(), Mode["AvailPeriod"].as<INT>(), false); // 魅力
		}
	}

	sql = "COMMIT"; // 提交事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);
	if (SetMysql) {
		
	}
	else {
		ItemInfo Items[3000];
		size_t ItemNum = 0;
		bool HasMoreInfo = false;
		sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		if (stmt)
		{
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
				Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
				Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
				Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
				Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
				Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
				Items[ItemNum].ItemType = 0;
				if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
				{
					Client->KartID = Items[ItemNum].ItemID;
					Z_INFO("赛车ID:%d\n", Client->KartID);
				}
				else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
				{
					Client->Pet->ID = Items[ItemNum].ItemID;
					GetPetInfo(Client);
				}

				ItemNum++;
				if (ItemNum >= 200)
				{
					HasMoreInfo = true;
					break;
				}
			}
		}
		ResponseRegister(Client, Nickname, Gender, Country, ItemNum, Items);
		if (stmt)
		{
			while (HasMoreInfo)
			{
				ItemNum = 0;
				HasMoreInfo = false;
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
					Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
					Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
					Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
					Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
					Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
					Items[ItemNum].ItemType = 0;
					if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
					{
						Client->KartID = Items[ItemNum].ItemID;
						Z_INFO("赛车ID:%d\n", Client->KartID);
					}
					else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
					{
						Client->Pet->ID = Items[ItemNum].ItemID;
						GetPetInfo(Client);
					}

					ItemNum++;
					if (ItemNum >= 200)
					{
						HasMoreInfo = true;
						break;
					}
				}
				NotifyClientAddItem(Client, ItemNum, Items);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
	ResponseSkipFreshTask(Client); // 跳过新手任务
}
void ResponseRegister(ClientNode *Client, const char *Nickname, BYTE Gender, BYTE Country, UINT ItemNum, ItemInfo *Items)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write32(p, Client->Uin);

	strcpy_s((char *)p, MaxNickName, Nickname);
	p += MaxNickName;

	Write8(p, Gender);
	Write8(p, Country);

	Write16(p, ItemNum); // AddItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE *pItemInfo = p;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, Items[i].ItemID);
			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); // ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}

	Write16(p, 0); // AddTaskNum

	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 129, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestLogin(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT ClientVersion = Read32(Body);
	Client->ClientVersion = ClientVersion;
	if (GetClient(Uin) != NULL)
	{
		if (GetClient(Uin)->TotalRound > 0)
		{
			RequestLeaveGame(GetClient(Uin), Body, BodyLen);
		}
		// RequestLeaveGame(GetClient(Uin), Body, BodyLen);
		NotifyKickPlayer(GetClient(Uin), "你的用户已经在另一个设备登录,您被迫下线！", 0);
		// ResponseStartGame(GetClient(Uin),"/<customlink=UI.children.AppEventHandle.OnKickPlayerMsg('2','你的账号在另一个设备登录,已经被踢出服务状态')>你的账号在另一个设备登录,请点击这里退出");
		GetClient(Uin)->IsExit = true;

		/*ResponseLogout(GetClient(Uin));
		GetClient(Uin)->Server->Disconnect(GetClient(Uin)->ConnID);
		DelClient(Uin);*/
	}
	// QQNickName[20]
	Body += MaxQQNickName;
	// Gender
	Body += 1;
	// IconID
	Body += 1;

	UINT AttachIdentify = Read32(Body);
	UINT LoginType = Read32(Body);
	UINT LoaderTipVersion = Read32(Body);
	UINT ClientStatusContex = Read32(Body);
	BYTE IsSimplifyVersion = Read8(Body);
	UINT CurTopMapRecordVersion = Read32(Body);
	UINT CurKartRoomPushInfoVersion = Read32(Body);
	/*
m_sPreSvrID
m_u8TGPSignatureLen
m_uiRandomKey
m_stClientFingerprint.m_uiPID
m_stClientFingerprint.m_uiMacAddrHigh
m_stClientFingerprint.m_uiMacAddrLow
m_stClientFingerprint.m_uiCPUID
m_stClientFingerprint.m_uiBiosID
m_uiConnID
m_bUseTGP
m_usBarPriviSignatureLen
m_iNetBarLev
m_bReconnect
m_shLoginBuffLen
m_usClientKeyLen
m_uiPlayerFlag
m_ucLoginSrcType
m_ucRequestLoginType
m_ushNetBarTokenLen
	*/

	Client->Uin = Uin;
	UserDB_ConnID(Client->Uin, Client->ConnID);
	Client->Pet = new PetInfo{0, "", 0, 0, 0, 0, 0};
	Client->Engage = new EngageInfo{};
	// Client->PassedCheckPointPropS = { {0, 0} };
	// Client->HeroMoveInfo = new MoveInfo;
	Client->IsExit = false;
	AddClient(Client, Uin);
	if (getGameConfig()["HaveEngage"] && getGameConfig()["HaveEngage"].as<UINT>() == 1)
	{
		NotifyEngageResult3(Client);
	}
	// printf("ClientVersion:%d, LoginType:%d\n", ClientVersion, LoginType);

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	ItemInfo Items[200];
	size_t ItemNum = 0;
	bool HasMoreInfo = false;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ? and WareHouseStatus = 0";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		if (stmt)
		{
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
				Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
				Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
				Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
				Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
				Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
				Items[ItemNum].ItemType = 0;
				if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
				{
					Client->KartID = Items[ItemNum].ItemID;
					// printf("KartID:%d\n", Client->KartID);
				}
				else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
				{
					Client->Pet->ID = Items[ItemNum].ItemID;
					GetPetInfo(Client);
				}

				ItemNum++;
				if (ItemNum >= 100)
				{
					HasMoreInfo = true;
					break;
				}
			}
		}

		ResponseLogin(Client, ClientVersion, LoginType, ItemNum, Items, HasMoreInfo);
		if (stmt)
		{
			while (HasMoreInfo)
			{
				ItemNum = 0;
				HasMoreInfo = false;
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
					Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
					Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
					Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
					Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
					Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
					Items[ItemNum].ItemType = 0;
					if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_CAR)
					{
						Client->KartID = Items[ItemNum].ItemID;
						// printf("KartID:%d\n", Client->KartID);
					}
					else if (Items[ItemNum].Status && GetItemType(Items[ItemNum].ItemID) == EAIT_PET)
					{
						Client->Pet->ID = Items[ItemNum].ItemID;
						GetPetInfo(Client);
					}

					ItemNum++;
					if (ItemNum >= 100)
					{
						// printf("!!ItemNum:%d\n", ItemNum);
						HasMoreInfo = true;
						break;
					}
				}
				// 这对于 ZingSpeed 好像不起作用
				NotifyLoginMoreInfo(Client, ItemNum, Items, HasMoreInfo);
				// 修复道具上限问题
				NotifyClientAddItem(Client, ItemNum, Items);
				ItemStatus NewStatus;
				for (size_t i = 0; i < ItemNum; i++)
				{
					if (Items[i].Status)
					{
						NewStatus.ItemID = Items[i].ItemID;
						NewStatus.NewStatus = true;
						ResponseChangeItemStatus(Client, 1, &NewStatus);
					}
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
	
	getGameMode(Client);
	NotifyClientVipFlag(Client);
	NotifyMsgBox(Client);
	NotifyRandRoomNameList(Client);
	NotifyTopListDesc(Client);
	NotifySpeed2Cfg(Client);
	NotifySvrConfig(Client);
	NotifySvrConfig2(Client);
	NotifySvrConfig3(Client);
	NotifyTopUIItemInfo(Client);
	NotifyRedPointInfo(Client);
	NotifySkillStoneKartInfo(Client);
	if (getGameConfig()["HaveAppellation"] && getGameConfig()["HaveAppellation"].as<UINT>() == 1)
	{
		if (Client->AppellationType > 0)
		{
			ResponseWearAppellation(Client, Client->AppellationType);
		}
	}
	NoitfyDobagStatus(Client);
	InitClientGameArrt(Client);
	NotifyS2CMyKartAltas(Client);
	NotifySpecialActivityInfo(Client);
	NotifyKartActiveCfgInfo(Client);
	NotifyPlayerAllActiveKartInfo(Client);
	NotifyPetStrengCfgInfo(Client);
	NotifyPetHuanHuaCfgInfo(Client);
	NotifySecondPassInfo(Client);
	if (Client->Engage->UIN > 0) {
		
		NotifyClientLoveBabyInfoWhenLogIn(Client);
	}
	//NotifyPlayerLimitedWakeKartInfo(Client);
	// NotifyTaskAward(Client,47,1);//生日
}
void getKartSkin(ClientNode *Client)
{
	ResponseSaveShapeRefit4(Client, Client->KartID);
}
void ChangKartMode(ClientNode *Client, UINT kartID)
{
	if (Client->KartID == kartID)
	{
		getKartSkin(Client);
		return;
	}
	ItemStatus *aItemStatus = (ItemStatus *)malloc(sizeof(ItemStatus) * 2);
	aItemStatus[0].ItemID = Client->KartID;
	aItemStatus[0].NewStatus = 0;
	aItemStatus[1].ItemID = kartID;
	aItemStatus[1].NewStatus = 1;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	for (int i = 0; i < 2; i++)
	{
		if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
		{
		}
		else {
			sql = "UPDATE Item SET Status=? WHERE Uin=? AND ItemID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, aItemStatus[i].NewStatus);
				sqlite3_bind_int(stmt, 2, Client->Uin);
				sqlite3_bind_int(stmt, 3, aItemStatus[i].ItemID);
				result = sqlite3_step(stmt);

				Z_INFO("物品ID:%d\n", aItemStatus[i].ItemID);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		
	}

	ResponseChangeItemStatus(Client, 2, aItemStatus);
	Client->KartID = kartID;
	getKartSkin(Client);
}
void getGameMode(ClientNode *Client)
{
	const unsigned char* pMsg1311171 = EHexToByte("000001010000007540D0000E220443B00160C502E47BE55FF81C310E0000000000000000F59D");

	SendToClient(Client, 131, pMsg1311171, 38, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		if (Client->ServerID == 900) // 滑板
		{
			UINT SlideID = 19294;
			sql = "SELECT SlideID FROM BaseGameTypeStatus WHERE Uin = ?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{

					if (sqlite3_column_int(stmt, 0) > 0)
					{
						SlideID = sqlite3_column_int(stmt, 0);
					}
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ChangKartMode(Client, SlideID);
		}
		else if (Client->ServerID == 1000) // 轮滑
		{
			UINT SkateID = 0;
			if (Client->Gender == 0)
			{
				SkateID = 31941;
				sql = "SELECT SkateID0 FROM BaseGameTypeStatus WHERE Uin = ?";
			}
			else
			{
				SkateID = 31940;
				sql = "SELECT SkateID1 FROM BaseGameTypeStatus WHERE Uin = ?";
			}

			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{

					if (sqlite3_column_int(stmt, 0) > 0)
					{
						SkateID = sqlite3_column_int(stmt, 0);
					}
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ChangKartMode(Client, SkateID);
		}
		else
		{
			UINT KartID = 10020;
			sql = "SELECT KartID FROM BaseGameTypeStatus WHERE Uin = ?";

			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{

					if (sqlite3_column_int(stmt, 0) > 0)
					{
						KartID = sqlite3_column_int(stmt, 0);
					}
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ChangKartMode(Client, KartID);
		}
	}
	
}
void NotifyRankedMatchSeasonInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 1);			 // PlayerSeasonInfoNums
	UINT SeasonID = 0;
	if (Client->ClientVersion == 18812) {
		SeasonID = 36;
	}
	else if (Client->ClientVersion >= 18812) {
		SeasonID = 37;
	}
	for (size_t i = 0; i < 1; i++)
	{ // PlayerSeasonInfos
		BYTE *pPlayerSeasonInfos = p;
		Write16(pPlayerSeasonInfos, 0); // len
		
		Write16(pPlayerSeasonInfos, SeasonID);		// SeasonID
		Write16(pPlayerSeasonInfos, 27);		// GradeLevel
		Write8(pPlayerSeasonInfos, 9999);		// ChildLevel
		Write16(pPlayerSeasonInfos, 27);		// HighGradeLevel
		Write8(pPlayerSeasonInfos, 9999);		// HighChildLevel
		Write8(pPlayerSeasonInfos, 9999);		// SeasonInfoProcesedTag
		Write8(pPlayerSeasonInfos, 9999);		// SeasonComputeInfoTipTag
		Write16(pPlayerSeasonInfos, 9999);		// SeasonInheritTag
		Write8(pPlayerSeasonInfos, 9999);		// SeasonInheritTipTag
		Write16(pPlayerSeasonInfos, 9999);		// SpeedGodNums
		Write32(pPlayerSeasonInfos, 9999);		// SpeedRoundNums
		Write32(pPlayerSeasonInfos, 9999);		// SpeedFinishRoundNums
		Write32(pPlayerSeasonInfos, 9999);		// TotalRankIndex
		Write32(pPlayerSeasonInfos, 9999);		// ContiFinishRoundNums
		Write32(pPlayerSeasonInfos, 9999);		// SpeedKillAllRoundNums
		Write32(pPlayerSeasonInfos, 9999);		// PropRoundNums
		Write32(pPlayerSeasonInfos, 9999);		// PropFinishRoundNums
		Write32(pPlayerSeasonInfos, 9999);		// ContiFinishPropRoundNums
		Write32(pPlayerSeasonInfos, 9999);		// PropKillAllRoundNums
		Write8(pPlayerSeasonInfos, 9999);		// LastRoundIndex
		Write32(pPlayerSeasonInfos, 881587969); // OpenDate
		Write32(pPlayerSeasonInfos, 881637632); // CloseDate
		Write32(pPlayerSeasonInfos, 9999);		// HistroyContFinishRoundNums

		len = pPlayerSeasonInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 9999); // RankedMatchSpeedGodNums
	Write8(p, 9999);  // LegendSpeedGod
	Write32(p, 37);	  // CurrentSeasonID

	len = p - buf;
	SendToClient(Client, 25005, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void ResponseLogin(ClientNode *Client, UINT ClientVersion, UINT LoginType, UINT ItemNum, ItemInfo *Items, bool HasMoreInfo)
{
	BYTE *buf = new BYTE[32768];
	// BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	USHORT ResultID = 0;
	const char *Reason = "";
	int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG;
	int IsInTopList = 0;

	if (!Client->IsLogin)
	{
		ResultID = 1;
		Reason = "the password is incorrect";
	}
	else
	{
		if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
		{
			
			
		}
		else {
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;

			sql = "SELECT VipFlag,IsInTopList  FROM Player  WHERE Uin=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				result = sqlite3_step(stmt);
				if (result == SQLITE_ROW)
				{
					ResultID = 0;
					if (sqlite3_column_int(stmt, 0) != 0)
					{
						Identity |= ID_IDENTIFY_SPEEDMEMBER;
					}
					IsInTopList = sqlite3_column_int(stmt, 1);
				}
				else
				{
					ResultID = 1; // 未注册
				}
			}
			else
			{
				ResultID = 1;
				Reason = sqlite3_errmsg(PlayerDB);
				Z_INFO("%s\n", Reason);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		
	}

	Write16(p, ResultID); // ResultID

	Write8(p, sizeof(Client->Key)); // KeyGameDataLength
	memcpy(p, Client->Key, sizeof(Client->Key));
	p += sizeof(Client->Key);

	Write32(p, Client->Uin);
	Write32(p, Identity);
	Write16(p, Client->ConnID); // PlayerID
	Write32(p, LoginType);		// LoginType
	Write32(p, 4);				// GameSvrdIdentity

	PlayerDB_GetCharacterInfo(Client);

	WritePlayerDBBaseInfo(p, Client->Uin);
	// WritePlayerGuildInfo(p, Client->Uin);
	Write16(p, ItemNum); // ItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE *pItemInfo = p;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, Items[i].ItemID);

			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); // ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}

	Write16(p, 0); // TaskNum

	/*
m_astPlayerDBTaskInfo[].m_iTaskID
m_astPlayerDBTaskInfo[].m_shOptionID
m_astPlayerDBTaskInfo[].m_cStatus
m_astPlayerDBTaskInfo[].m_bPrevStatus
m_astPlayerDBTaskInfo[].m_iProgressRate
m_astPlayerDBTaskInfo[].m_iOther
m_astPlayerDBTaskInfo[].m_uiTakeTime
m_astPlayerDBTaskInfo[].m_uiLastUpdateTime
	*/

	Write8(p, 0);					 // RecomCountry
	Write32(p, (UINT)time(nullptr)); // ServerTime

	len = strlen(Reason);
	Write8(p, (BYTE)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;
	NotifyEngageResult4(Client, p);
	//if(Client->Engage->UIN>0)
	//{ // PlayerRelationInfo
	//	BYTE *pPlayerRelationInfo = p;
	//	Write16(pPlayerRelationInfo, 0); // len

	//	Write32(pPlayerRelationInfo, Client->Uin); // SrcUin
	//	Write32(pPlayerRelationInfo, 3); // RelationFlag
	//	Write32(pPlayerRelationInfo, Client->Engage->UIN); // RelationUin
	//	WriteString(pPlayerRelationInfo, Client->Engage->NickName,16);	 // NickName

	//	Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // EngageTime
	//	Write32(pPlayerRelationInfo, 0); // NextCanBookingTimeLimit
	//	Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // BeginHoneyMoonTime
	//	Write32(pPlayerRelationInfo, Client->Engage->EngageTime); // EndHoneyMoonTime
	//	Write8(pPlayerRelationInfo, 4);	 // EngageFlag

	//	len = pPlayerRelationInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}else
	//{ //PlayerRelationInfo玩家关系信息
	//	BYTE* pPlayerRelationInfo = p;
	//	Write16(pPlayerRelationInfo, 0); //len


	//	Write32(pPlayerRelationInfo, Client->Uin); //SrcUin
	//	Write32(pPlayerRelationInfo, 0); //RelationFlag
	//	Write32(pPlayerRelationInfo, 0); //RelationUin

	//	char RelationNickName[MaxNickName] = "";
	//	memcpy(pPlayerRelationInfo, RelationNickName, MaxNickName);
	//	pPlayerRelationInfo += MaxNickName;

	//	Write32(pPlayerRelationInfo, 0); //EngageTime启动时间参与时间
	//	Write32(pPlayerRelationInfo, 0); //NextCanBookingTimeLimit
	//	Write32(pPlayerRelationInfo, 0); //BeginHoneyMoonTime
	//	Write32(pPlayerRelationInfo, 0); //EndHoneyMoonTime
	//	Write8(pPlayerRelationInfo, 0); //EngageFlag


	//	len = pPlayerRelationInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}

	BYTE *pMapNum = p;
	Write16(p, 0); // MapNum
	{
		size_t i = 0;
		if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
		{
			
		}
		else {
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			
			sql = "SELECT MapID,Record  FROM MapRecord WHERE Uin = ?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{ // Record
					i++;

					BYTE* pRecord = p;
					Write16(pRecord, 0); // len

					Write32(pRecord, sqlite3_column_int(stmt, 0)); // MapID
					Write32(pRecord, sqlite3_column_int(stmt, 1)); // Record

					len = pRecord - p;
					Set16(p, (WORD)len);
					p += len;
				}
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		
		Set16(pMapNum, (WORD)i);
	}

	BYTE *pPetNum = p;
	int PetNum = 0;
	Write16(p, 0); // PetNum
	{
		if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
		{
			
		}
		else {
			sqlite3_stmt* stmt = NULL;
			if (sqlite3_prepare_v2(PlayerDB, "SELECT PetId,Name,Experience,PL,Status FROM Pet WHERE Uin=?;", -1, &stmt, NULL) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);

				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					PetNum++;
					BYTE* pPetInfo = p;
					Write16(pPetInfo, 0); // len

					Write32(pPetInfo, Client->Uin);					// Uin
					Write32(pPetInfo, sqlite3_column_int(stmt, 0)); // PetId
					memcpy(pPetInfo, (char*)sqlite3_column_text(stmt, 1), 23);
					pPetInfo += 23;
					Write8(pPetInfo, 1); // IsProper
					UINT Level = GetPetLevel(sqlite3_column_int(stmt, 2));
					UINT Status = sqlite3_column_int(stmt, 4);
					Write16(pPetInfo, Level);						// LevelLimit
					Write16(pPetInfo, Level);						// Level
					Write32(pPetInfo, sqlite3_column_int(stmt, 2)); // CurLevelExp
					Write32(pPetInfo, GetPetMaxExp(Level));			// CurLevelMaxExp
					Write8(pPetInfo, Status);						// Status
					Write16(pPetInfo, sqlite3_column_int(stmt, 3)); // PL
					Write16(pPetInfo, 100);							// TotalPL
					YAML::Node PetSkills = GetPetConfig(sqlite3_column_int(stmt, 0));

					if (PetSkills.size() < Status)
					{
						Write16(pPetInfo, 0); // PetSkillNum
					}
					else
					{
						Write16(pPetInfo, Status); // PetSkillNum
						for (size_t i = 0; i < Status; i++)
						{ // PetSkillList
							BYTE* pPetSkillList = pPetInfo;
							Write16(pPetSkillList, 0); // len

							Write16(pPetSkillList, PetSkills[i]["SkillID"].as<UINT>());		   // SkillID
							Write8(pPetSkillList, i + 1);									   // Status
							Write8(pPetSkillList, 1);										   // Active
							Write32(pPetSkillList, PetSkills[i]["Value"].as<float>() * Level); // Value

							len = pPetSkillList - pPetInfo;
							Set16(pPetInfo, (WORD)len);
							pPetInfo += len;
						}
					}

					Write32(pPetInfo, 10); // StrengLevel
					Write8(pPetInfo, 4);   // TopStatus
					{					   // CommonInfo
						BYTE* pCommonInfo = pPetInfo;
						Write16(pCommonInfo, 0); // len

						Write32(pCommonInfo, 0); // HuanHuaLevel
						Write8(pCommonInfo, 0);	 // CanHuanHua

						len = pCommonInfo - pPetInfo;
						Set16(pPetInfo, (WORD)len);
						pPetInfo += len;
					}

					len = pPetInfo - p;
					Set16(p, (WORD)len);
					p += len;
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		
	}
	Set16(pPetNum, (WORD)PetNum);

	Write8(p, (bool)IsInTopList); // IsInTopList
	Write32(p, (DWORD)time(nullptr));				  // LastLoginTime
	Write32(p, 9999);			  // MasterPoint
	Write8(p, 0);				  // LoginRealNameFlag

	
	if (ClientVersion > 18457)
	{
		for (size_t i = 0; i < 24; i++) {
			Write8(p, 0);// ExRightFlag[]
		}
	}

	Write8(p, 0); // OverdueItemNum
	/*
m_astOverdueItem[%d].m_iItemID
m_astOverdueItem[%d].m_iOverdueTime
m_astOverdueItem[%d].m_iOtherAttr
	*/

	BYTE *pStoneKartNum = p;
	Write32(p, 0); // StoneKartNum
	
	int result;
	int GuildVipLevel = 0;
	int GuildVipPoint = 0;
	int GuildVipLeftDays = 0;
	int LoverVipLevel = 0;
	int LoverVipPoint = 0;
	int LoverVipLeftDays = 0;
	int GardenLevel = 0;
	int GardenPoint = 0;
	int GardenExpireTime = 0;
	int VipLevel = 0;
	int CharmValueOfMonth = 0;
	int VipFlag = 0;
	int VipGrowRate = 0;
	int EmperorLevel = 0;
	int EmperorPoint = 0;
	int EmperorLeftDays = 0;
	int EmperorGrowRate = 0;
	int NobleLevel = 0;
	int NoblePoint = 0;
	int NobleLeftDays = 0;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		sql = "SELECT GuildVipLevel,GuildVipPoint,GuildVipLeftDays,LoverVipLevel,LoverVipPoint,LoverVipLeftDays,GardenLevel,GardenPoint,GardenExpireTime,VipLevel,CharmValueOfMonth,VipFlag,VipGrowRate,EmperorLevel,EmperorPoint,EmperorLeftDays,EmperorGrowRate,NobleLevel,NoblePoint,NobleLeftDays,SecPwd,SecPwdStatus,Verify,FrozenLeftSeconds,FrozenSeconds  FROM BaseInfo  WHERE Uin=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				GuildVipLevel = sqlite3_column_int(stmt, 0);
				GuildVipPoint = sqlite3_column_int(stmt, 1);
				GuildVipLeftDays = sqlite3_column_int(stmt, 2);
				LoverVipLevel = sqlite3_column_int(stmt, 3);
				LoverVipPoint = sqlite3_column_int(stmt, 4);
				LoverVipLeftDays = sqlite3_column_int(stmt, 5);
				GardenLevel = sqlite3_column_int(stmt, 6);
				GardenPoint = sqlite3_column_int(stmt, 7);
				GardenExpireTime = sqlite3_column_int(stmt, 8);
				VipLevel = sqlite3_column_int(stmt, 9);
				CharmValueOfMonth = sqlite3_column_int(stmt, 10);
				VipFlag = sqlite3_column_int(stmt, 11);
				VipGrowRate = sqlite3_column_int(stmt, 12);
				EmperorLevel = sqlite3_column_int(stmt, 13);
				EmperorPoint = sqlite3_column_int(stmt, 14);
				EmperorLeftDays = sqlite3_column_int(stmt, 15);
				EmperorGrowRate = sqlite3_column_int(stmt, 16);
				NobleLevel = sqlite3_column_int(stmt, 17);
				NoblePoint = sqlite3_column_int(stmt, 18);
				NobleLeftDays = sqlite3_column_int(stmt, 19);
				char SecPwd[MaxSecPwd] = "";
				memcpy(SecPwd, (char*)sqlite3_column_text(stmt, 20), MaxSecPwd);
				sprintf_s(Client->SecPwd, SecPwd);
				Client->SecPwdStats = sqlite3_column_int(stmt, 21);
				Client->Verify = sqlite3_column_int(stmt, 22);
				Client->FrozenLeftSeconds = sqlite3_column_int(stmt, 23);
				Client->FrozenSeconds = sqlite3_column_int(stmt, 24);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	{//贵族等级优化
		if (VipFlag > 7) {
			VipFlag = 7;
		}
		if (VipLevel > 9 ){
			VipLevel = 9;
		}
		if (NobleLevel > 7) {
			NobleLevel = 7;
		}
	}
	Write32(p, 0);			// LockedMapID
	Write32(p, 0);			// TotalGuildProsperity
	Write32(p, 0);			// ClientStatusContex
	Write8(p, 1);			// IsHoneyMoon
	Write8(p, HasMoreInfo); // HasMoreInfo
	Write16(p, 0);			// CurSvrID

	{ // NobleInfo
		BYTE *pNobleInfo = p;
		Write16(pNobleInfo, 0); // len

		Write32(pNobleInfo, Client->NobleID);	// NobleID
		Write8(pNobleInfo, NobleLevel);		// NobleLevel 贵族等级
		Write32(pNobleInfo, NoblePoint);	// NoblePoint 贵族成长值
		Write32(pNobleInfo, NobleLeftDays); // NobleLeftDays 贵族天数

		len = pNobleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 34463); // ExtendItemNum
	Write8(p, 60);	   // UpdateOnlineInfoInterval

#ifndef ZingSpeed
	{ // GuildVipBaseInfo

		BYTE *pGuildVipBaseInfo = p;
		Write16(pGuildVipBaseInfo, 0); // len

		Write8(pGuildVipBaseInfo, GuildVipLevel);  // GuildVipLevel 车队贵族
		Write32(pGuildVipBaseInfo, GuildVipPoint); // GuildVipPoint 车队贵族成长

		len = pGuildVipBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // GuildVipOtherInfo
		BYTE *pGuildVipOtherInfo = p;
		Write16(pGuildVipOtherInfo, 0); // len

		Write32(pGuildVipOtherInfo, GuildVipLeftDays); // GuildVipLeftDays 车队贵族成长
		Write8(pGuildVipOtherInfo, 0);				   // CanReceiveGift

		len = pGuildVipOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
#endif
	Write8(p, 0); // HasLDMInfo
	/*
m_astPlayerLDMBaseInfo[%d].m_u8Grade
m_astPlayerLDMBaseInfo[%d].m_u32Score
m_astPlayerLDMBaseInfo[%d].m_u32MaxScore
m_astPlayerLDMBaseInfo[%d].m_u32MaxGrade
	*/

	Write32(p, 0); // ForbiddenModeFreeTime
	Write32(p, 0); // ForbiddenModeBitSet
	// Write32(p, 0x7FFFFFFF); //ForbiddenModeFreeTime
	// Write32(p, 0x7FFFFFFF); //ForbiddenModeBitSet

#ifndef ZingSpeed
	{ // LoverVipBaseInfo
		BYTE *pLoverVipBaseInfo = p;
		Write16(pLoverVipBaseInfo, 0); // len

		Write8(pLoverVipBaseInfo, LoverVipLevel);  // LoverVipLevel 情侣贵族
		Write32(pLoverVipBaseInfo, LoverVipPoint); // LoverVipPoint 情侣贵族
		Write8(pLoverVipBaseInfo, 9999);		   // GrowRate

		len = pLoverVipBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // LoverVipOtherInfo
		BYTE *pLoverVipOtherInfo = p;
		Write16(pLoverVipOtherInfo, 0); // len

		Write32(pLoverVipOtherInfo, LoverVipLeftDays); // LoverVipLeftDays 情侣贵族
		Write8(pLoverVipOtherInfo, 1);				   // CanReceiveGift
		Write8(pLoverVipOtherInfo, 1);				   // ShowExpireTips

		len = pLoverVipOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
#endif
	Write8(p, 3);		// SkateTaskLevel
	Write32(p, 999999); // SkateCoin
	Write8(p, 999999);	// SkateExpSkillLevel
	Write16(p, 999999); // SkateCoinCountDaily
	Write32(p, 999999); // SkateCoinHistoryTotal
	Write32(p, 999999); // MaxDayPveScore
	Write32(p, 999999); // MaxHistoryPveScore

#ifndef ZingSpeed
	Write32(p, 9999); // LoveValue

	Write8(p, 0); // HasCheerAddition
	/*
m_astCheerAdditon[%d].m_ushExpAddition
m_astCheerAdditon[%d].m_ushWageAddition
	*/

	{ // PersonalGardenBaseInfo
		BYTE *pPersonalGardenBaseInfo = p;
		Write16(pPersonalGardenBaseInfo, 0); // len

		Write8(pPersonalGardenBaseInfo, GardenLevel);  // GardenLevel 后花园
		Write32(pPersonalGardenBaseInfo, GardenPoint); // GardenPoint 后花园
		if (GardenLevel == 0)
		{
			Write8(pPersonalGardenBaseInfo, 0); // GrowRate
		}
		else
		{
			Write8(pPersonalGardenBaseInfo, 88888); // GrowRate
		}

		Write32(pPersonalGardenBaseInfo, GardenExpireTime); // GardenExpireTime后花园
		{													// SimpleInfo
			BYTE *pSimpleInfo = pPersonalGardenBaseInfo;
			Write16(pSimpleInfo, 0); // len
			if (GardenLevel == 0)
			{
				Write32(pSimpleInfo, 0); // WeekPopularity
				Write32(pSimpleInfo, 0); // TotalPopularity
				Write32(pSimpleInfo, 0); // LastUpdatePopularityTime
				Write8(pSimpleInfo, 0);	 // PrivateType
			}
			else
			{
				Write32(pSimpleInfo, 1888888); // WeekPopularity
				Write32(pSimpleInfo, 1888888); // TotalPopularity
				Write32(pSimpleInfo, 7);	   // LastUpdatePopularityTime
				Write8(pSimpleInfo, 7);		   // PrivateType
			}

			len = pSimpleInfo - pPersonalGardenBaseInfo;
			Set16(pPersonalGardenBaseInfo, (WORD)len);
			pPersonalGardenBaseInfo += len;
		}

		len = pPersonalGardenBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // ConsumeVipInfo
		BYTE *pConsumeVipInfo = p;
		Write16(pConsumeVipInfo, 0); // len

		Write32(pConsumeVipInfo, VipLevel);			 // VipLevel 魅力
		Write32(pConsumeVipInfo, CharmValueOfMonth); // CharmValueOfMonth
		Write32(pConsumeVipInfo, 888);				 // SearchTreasureNums
		Write32(pConsumeVipInfo, 888);				 // GetTreasureNums

		len = pConsumeVipInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // EmperorInfo
		BYTE *pEmperorInfo = p;
		Write16(pEmperorInfo, 0); // len

		Write8(pEmperorInfo, EmperorLevel);		// EmperorLevel 钻皇
		Write32(pEmperorInfo, EmperorPoint);	// EmperorPoint 钻皇
		Write32(pEmperorInfo, EmperorLeftDays); // EmperorLeftDays 钻皇
		Write8(pEmperorInfo, EmperorGrowRate);	// EmperorGrowRate 钻皇

		len = pEmperorInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // EmperorOtherInfo
		BYTE *pEmperorOtherInfo = p;
		Write16(pEmperorOtherInfo, 0); // len

		Write32(pEmperorOtherInfo, 888); // ExpiredTime
		Write8(pEmperorOtherInfo, 1);	 // ShowExpireTips

		len = pEmperorOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 9999); // ShuttleScoreWeek

	{ // ActivityInfo
		BYTE *pActivityInfo = p;
		Write16(pActivityInfo, 0); // len

		Write32(pActivityInfo, 2665); // TotalActivity
		Write32(pActivityInfo, 10);	  // ActivityLevel

		len = pActivityInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // GansterScoreInfo
		BYTE *pGansterScoreInfo = p;
		Write16(pGansterScoreInfo, 0); // len

		Write32(pGansterScoreInfo, 59);	  // GansterSeasonID
		Write32(pGansterScoreInfo, 9999); // GansterScore
		Write32(pGansterScoreInfo, 9999); // PoliceScore
		Write32(pGansterScoreInfo, 9999); // TotalGansterScore

		len = pGansterScoreInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, Client->Uin); // OlympicId
	Write32(p, 0);			 // NPCEliminateWinTimes

	{ // BorderInfo
		BYTE *pBorderInfo = p;
		Write16(pBorderInfo, 0); // len

		Write32(pBorderInfo, 43);	// SeasonID
		Write32(pBorderInfo, 9999); // Zhanxun
		Write32(pBorderInfo, 9999); // SeasonZhanxun

		len = pBorderInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0);	 // ReduceReturnRate
	Write32(p, 0);	 // ReduceReturnAvailPeriod
	Write8(p, 1);	 // SpecialActivityStatus
	Write32(p, 300); // ThemeHouseDressDegreeValue

	Write8(p, 1); // 3DRoomEnabled
	Write8(p, 1); // 3DRoomEnabled_OB
	Write8(p, 1); // BuyZizuanPopupOpen

	Write32(p, 0); // BuyZizuanPopupLimit
	Write8(p, 1);  // EnableReconnectOpt

	Write8(p, 0); // HasRankedMatchInfo
	for (size_t i = 0; i < 0; i++)
	{ // RankedMatchGradeInfo
		BYTE *pRankedMatchGradeInfo = p;
		Write16(pRankedMatchGradeInfo, 0); // len

		Write32(pRankedMatchGradeInfo, 27);	  // GradeLevel
		Write32(pRankedMatchGradeInfo, 9999); // ChildLevel
		Write32(pRankedMatchGradeInfo, 9999); // GradeScore
		Write32(pRankedMatchGradeInfo, 9999); // MaxGradeScore
		Write32(pRankedMatchGradeInfo, 9999); // TotalScore

		len = pRankedMatchGradeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	/*
m_stRankedMatchGradeInfo[%d].m_uiGradeLevel
m_stRankedMatchGradeInfo[%d].m_uiChildLevel
m_stRankedMatchGradeInfo[%d].m_uiGradeScore
m_stRankedMatchGradeInfo[%d].m_uiMaxGradeScore
m_stRankedMatchGradeInfo[%d].m_uiTotalScore
	*/

	Write8(p, 0); // HasHuanLingChangeInfo
	/*
m_astHuanLingSuitInfo[%d].m_iSuitID
m_astHuanLingSuitInfo[%d].m_ucChangeLevel
	*/

	Write8(p, 1);  // EquipSealType
	Write8(p, 1);  // hCreditStarFlag
	Write8(p, 1);  // PersonalPanelSelectRankedMatchFrameTag
	Write8(p, 0);  // VersionURLLen
	Write8(p, 0);  // PrivacyURLLen
	Write8(p, 1);  // PersonalRankedMatchLevelShowTag
	Write16(p, 0); // LeftDeletingRoleDays
	Write8(p, 0);  // OnlyRspEquippedItem
	Write32(p, 0); // LoginSwitchFlag1
	Write16(p, 0); // LeftUploadPLogNum
#endif

	len = p - buf;
	int MsgID = GetServerType(Client->ServerID);
	if (MsgID == Relax)
	{
		MsgID = 98;
	}
	else if (MsgID == Dance)
	{
		MsgID = 95;
	}
	else if (MsgID == Engage)
	{
		MsgID = 100;//位置
	}
	else
	{
		MsgID = 100;
	}
	// printf("ServerID:%d\n", Client->ServerID);
	SendToClient(Client, MsgID, buf, len, Client->ConnID, FE_GAMESVRD, Client->ConnID, Response);
}
void NotifyLoginMoreInfo(ClientNode *Client, UINT ItemNum, ItemInfo *Items, bool HasNext)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, ItemNum); // ItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE *pItemInfo = p;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, Items[i].ItemID);
			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); // ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}

	Write16(p, 0); // TaskNum
	/*
m_astPlayerDBTaskInfo[].m_iTaskID
m_astPlayerDBTaskInfo[].m_shOptionID
m_astPlayerDBTaskInfo[].m_cStatus
m_astPlayerDBTaskInfo[].m_bPrevStatus
m_astPlayerDBTaskInfo[].m_iProgressRate
m_astPlayerDBTaskInfo[].m_iOther
m_astPlayerDBTaskInfo[].m_uiTakeTime
m_astPlayerDBTaskInfo[].m_uiLastUpdateTime
	*/

	Write16(p, 0); // PetNum

	Write16(p, 0); // MapNum
	/*
m_astRecord[].m_iMapID
m_astRecord[].m_iRecord
m_astRecord[].m_iLastUpdateTime
	*/

	Write16(p, 0); // OverdueItemNum
	/*
m_astOverdueItem[].m_iItemID
m_astOverdueItem[].m_iOverdueTime
m_astOverdueItem[].m_iOtherAttr
	*/

	Write16(p, 0); // StoneKartNum
	/*
m_astKartStoneGrooveInfo[].m_iKartID
m_astKartStoneGrooveInfo[].m_iStoneGrooveNum
m_astKartStoneGrooveInfo[].m_astStoneGrooveInfo[].m_iStoneUseOccaType
m_astKartStoneGrooveInfo[].m_astStoneGrooveInfo[].m_iSkillStoneID
	*/

	Write8(p, HasNext);

	len = p - buf;
	SendToClient(Client, 223, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestLogout(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT LogOutFlag = Read32(Body);
	int ServerKickReason = Read32(Body);

	ResponseLogout(Client);
	Client->Server->Disconnect(Client->ConnID);
}
void ResponseLogout(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write8(p, 0);  // ReasonLen

	len = p - buf;
	SendToClient(Client, 101, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestHello(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseHello(Client, Uin, Time);
}
void AutoTalk()
{
	struct AUTOBUGLET // 自动喇叭喊话
	{
		const char *Content;  // 文本
		BYTE BugletType;	  // 喇叭类型
		UINT Gender;		  // 性别
		const char *NickName; // 名字
	};
	//12 全服喇叭
	//0 小喇叭
	//1 祝福小喇叭
	//3 新年小喇叭
	//13 弹幕小喇叭
	//14 休闲区大喇叭
	//11 钻皇大喇叭
	//10 城主大喇叭
	//7 皇族大喇叭
	//4 车队小喇叭
	//5 紫钻小喇叭
	AUTOBUGLET AUTOBUGLETCFG[] =
		{
			// Content 内容
			// BugletType 喇叭类型
			// Gender 性别
			// NickName 名字
			{"CPDD", 0, 0, "  小仙女"},
			{"找哥哥,可以给我玩玉麒麟的", 3, 0, "  老马是我碟"},
			{"休闲区约架,车队的速来", 10, 0, "试车房做饭的"},
			{"CPDD,你是唯一", 12, 0, "试车房做饭的"},
			{"一个破单机还要怎么样玩", 12, 0, "国服打野"},
			{"刷物品在大厅口令红包", 12, 0, "沸羊羊你真粗鲁"},
		};
	UINT BugletNum = sizeof(AUTOBUGLETCFG) / sizeof(AUTOBUGLETCFG[0]);
	int randomID;
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<int> dis(1, BugletNum);
	randomID = dis(eng);
	ClientNode *Temp = new ClientNode();
	Temp->Uin = 9999 + randomID;
	Temp->Gender = AUTOBUGLETCFG[randomID - 1].Gender;
	char szCharName[MaxNickName] = "";
	memcpy(szCharName, AUTOBUGLETCFG[randomID - 1].NickName, MaxNickName);
	strcpy_s(Temp->NickName, szCharName);
	NotifyBugletTalkALL(Temp, strlen(AUTOBUGLETCFG[randomID - 1].Content), (char *)AUTOBUGLETCFG[randomID - 1].Content, AUTOBUGLETCFG[randomID - 1].BugletType);
}
void ResponseHello(ClientNode *Client, UINT Uin, UINT Time)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	YAML::Node GameConfig = getGameConfig();
	if (GameConfig["AutoTalk"] && GameConfig["AutoTalk"].as<UINT>() == 1)
	{
		AutoTalk();
		AutoTalk();
	}

	Write16(p, 0);					  // ResultID
	Write8(p, 0);					  // ReasonLen
	Write32(p, Time);				  // Time
	Write32(p, Uin);				  // Uin
	Write32(p, (DWORD)time(nullptr)); // ServerTime
#ifndef ZingSpeed
	Write32(p, 0); // ServerMicroSecond
#endif

	len = p - buf;
	SendToClient(Client, 112, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
char *U2G(const char *utf8)
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

// GB2312到UTF-8的转换
char *G2U(const char *gb2312)
{
	int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char *str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr)
		delete[] wstr;
	return str;
}
void RequestUseItem(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ItemID = Read32(p);
	UINT DstUin = Read32(p);
	bool AttachDataLen = Read8(p);
	UCHAR OpType = Read8(p);
	UINT Para = Read32(p);
	UCHAR MultiUseFlag = Read8(p);
	int MultiUseNum = Read32(p);
	Z_INFO("[游戏服务][物品id:%d]\n", ItemID); // 宝石id
	if (ItemID == 63555)
	{ // 钻皇
		RequestChangeVip(Client, Body, BodyLen, 63555);
		ResponseStartGame(Client, "/<a>/C01开通钻皇成功");
		// ResponseStartGame(Client, "/<customlink=for i = 100, 1, -1 do cmd_showTreasureBox(10020) end;>默契宝盒*100");
	}
	else if (ItemID == 63505)
	{ // 后花园钥匙
		RequestChangeVip(Client, Body, BodyLen, 63505);
		ResponseStartGame(Client, "/<a>/C01后花园已经开通");
	}
	else if (ItemID == 30450)
	{ // 车队贵族
		RequestChangeVip(Client, Body, BodyLen, 30450);
		ResponseStartGame(Client, "/<a>/C01车队贵族已经开通");
		// ResponseStartGame(Client, "/<customlink=for i = 100, 1, -1 do cmd_OpenTreasureBox(宝箱ID) end;>开启");
	}
	else if (ItemID == 27137)
	{ // 皇权贵族
		RequestChangeVip(Client, Body, BodyLen, 27137);
		ResponseStartGame(Client, "/<a>/C01皇权贵族已经开通");
	}
	else if (ItemID == 31964)
	{ // 情侣贵族
		RequestChangeVip(Client, Body, BodyLen, 31964);
		ResponseStartGame(Client, "/<a>/C01情侣贵族已经开通");
	}
	else
	{
		ResponseUseItem(Client, ItemID, DstUin);
		NotifyUseItemMagic(Client, ItemID, Uin, DstUin);
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
					NotifyUseItemMagic(ChatRoomClient, ItemID, Uin, DstUin);
				}
			}
		} while (false);
	}
}

void ResponseUseItem(ClientNode* Client, int ItemID, UINT DstUin)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, ItemID); //ItemID
	Write32(p, DstUin); //DstUin
	Write8(p, 0); //AttachDataLen
	Write8(p, 0); //ReasonLen
	Write8(p, 0); //OpType

	len = p - buf;
	SendToClient(Client, 158, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestChangeVip(ClientNode *Client, BYTE *Body, size_t BodyLen, int itemId)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	if (itemId == 63555)
	{
		long randomID;
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<int> dis(0, 999999999);
		randomID = dis(eng);
		// 钻皇
		sql = "UPDATE BaseInfo SET EmperorLevel=1,EmperorPoint=EmperorPoint+100,EmperorLeftDays=EmperorLeftDays+31,EmperorGrowRate=EmperorGrowRate+100  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		sql = "UPDATE BaseInfo SET GuildVipLevel=1,GuildVipPoint=GuildVipPoint+100,GuildVipLeftDays=GuildVipLeftDays+31  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		sql = "UPDATE BaseInfo SET NobleLevel=NobleLevel+1,NoblePoint=NoblePoint+100,NobleLeftDays=NobleLeftDays+31,NobleID=?  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, randomID);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		sql = "UPDATE BaseInfo SET LoverVipLevel=1,LoverVipPoint=LoverVipPoint+100,LoverVipLeftDays=LoverVipLeftDays+31  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		NotifyClientUpdateNobleInfo(Client);
		NotifyClientUpdateEmperorInfo(Client);
		NotifyClientUpdateLoverVipInfo(Client);
		NotifyClientUpdateGuildVipInfo(Client);
	}
	else if (itemId == 63505)
	{ // 后花园钥匙
		sql = "UPDATE BaseInfo SET GardenLevel=GardenLevel+1,GardenPoint=GardenPoint+100,GardenExpireTime=GardenExpireTime+100  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		NotifyClientUpdatePersonalGardenBaseInfo(Client);
	}
	else if (itemId == 30450)
	{ // 车队贵族
		sql = "UPDATE BaseInfo SET GuildVipLevel=1,GuildVipPoint=GuildVipPoint+100,GuildVipLeftDays=GuildVipLeftDays+31  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		NotifyClientUpdateGuildVipInfo(Client);
	}
	else if (itemId == 27137)
	{ // 皇权贵族
		long randomID;
		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<int> dis(0, 999999999);
		randomID = dis(eng);
		sql = "UPDATE BaseInfo SET NobleLevel=NobleLevel+1,NoblePoint=NoblePoint+100,NobleLeftDays=NobleLeftDays+31, NobleID = ?  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, randomID);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		NotifyClientUpdateNobleInfo(Client);
	}
	else if (itemId == 31964)
	{ // 情侣贵族
		sql = "UPDATE BaseInfo SET LoverVipLevel=1,LoverVipPoint=LoverVipPoint+100,LoverVipLeftDays=LoverVipLeftDays+31  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		NotifyClientUpdateLoverVipInfo(Client);
	}
	else if (itemId == 65736)
	{ // 紫钻
		sql = "UPDATE BaseInfo SET VipFlag=VipFlag+1,VipGrowRate=VipGrowRate+100  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
	}
	else if (itemId == 62787)
	{ // 魅力
		sql = "UPDATE BaseInfo SET VipLevel=VipLevel+1,CharmValueOfMonth=CharmValueOfMonth+100  WHERE Uin=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			result = sqlite3_step(stmt);
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
	}
	stmt = NULL;
}
void RequestRandomOneNobleID(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR RandType = Read8(p);
	ResponseRandomOneNobleID(Client, RandType);
}

void ResponseRandomOneNobleID(ClientNode* Client, UCHAR RandType)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	long randomID;
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<int> dis(0, 999999999);
	randomID = dis(eng);
	Write16(p, 0); //ResultID
	Write8(p, RandType); //RandType
	Write32(p, randomID); //NobleID
	Write8(p, 1); //CanBroadcast

	len = p - buf;
	SendToClient(Client, 1652, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestDropRandomNobleID(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT NobleID = Read32(p);

}

void RequestReplaceNobleID(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT NobleID = Read32(p);
	UCHAR NeedBroadcast = Read8(p);
	ResponseReplaceNobleID(Client, NobleID);
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "UPDATE BaseInfo SET NobleID = ?  WHERE Uin=?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, NobleID);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	NotifyClientUpdateNobleInfo(Client);
}

void ResponseReplaceNobleID(ClientNode* Client,UINT NobleID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write32(p, NobleID); //NobleID

	len = p - buf;
	SendToClient(Client, 1654, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestChange(ClientNode *Client, BYTE *Body, size_t BodyLen, int itemId)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int itemIdOut = -1;
	int result;
	{ // 查询物品
		sql = "SELECT itemIOut FROM BoxItemInfo where itemIOut=?";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, itemId);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				itemIdOut = sqlite3_column_int(stmt, 0);
			}
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
	}
	if (itemIdOut == itemId)
	{
		int ItemID = 0, type = 0;
		std::string itemName = "";

		{ // 获取

			sql = "SELECT ItemID,itemName,type FROM itemInfo where ItemID=?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, itemId);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					ItemID = sqlite3_column_int(stmt, 0);
					itemName = U2G((char *)sqlite3_column_text(stmt, 1));
					std::string itemIdStr = std::to_string(ItemID);
					itemName = "/<itemshow=" + itemIdStr + ">[" + itemName + "]中";
					type = sqlite3_column_int(stmt, 2);
				}
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
		}
		{ // 打开逻辑 1为全部打开 2为抽奖
			if (type == 1)
			{
				const char *Reason = "恭喜你在";
				std::string res1;
				res1 = Reason + itemName + "获得了";
				sql = "SELECT itemIdSrc,itemName,Level,itemType,gender FROM BoxItemInfo LEFT JOIN ItemInfo ON BoxItemInfo.itemIdSrc = ItemInfo.itemID WHERE itemIOut = ?";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						int gender = sqlite3_column_int(stmt, 4);
						if (gender != -1)
						{
							if (gender != Client->Gender)
							{
								continue;
							}
						}
						ItemInfo Items[3000];
						ItemInfo Item = PlayerDB_AddItem(Client->Uin, sqlite3_column_int(stmt, 0), 1, -1, false);
						Items[0].ItemID = sqlite3_column_int(stmt, 0);
						Items[0].ItemNum = Item.ItemNum;
						Items[0].AvailPeriod = Item.AvailPeriod;
						Items[0].Status = Item.Status;
						Items[0].ObtainTime = Item.ObtainTime;
						Items[0].OtherAttribute = Item.OtherAttribute;
						Items[0].ItemType = Item.ItemType;
						NotifyClientAddItem(Client, 1, Items);
						std::string temp = U2G((char *)sqlite3_column_text(stmt, 1));
						std::string itemIdStr = std::to_string(sqlite3_column_int(stmt, 0));
						temp = "，/<itemshow=" + itemIdStr + ">[" + temp + "]";
						type = sqlite3_column_int(stmt, 2);
						res1 += temp;
					}
					res1 += "。";
					ResponseStartGame(Client, res1.c_str());
				}
				else
				{
					Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
				}
				sqlite3_finalize(stmt);
			}
			else if (type == 2)
			{
				const char *Reason = "恭喜你在";
				std::string res1;
				res1 = Reason + itemName + "获得了";
				int sum = -1;
				int randoms[9999];
				std::string randomName[9999];
				sql = "SELECT itemIdSrc,itemName,Level,itemType,gender,probability FROM BoxItemInfo LEFT JOIN ItemInfo ON BoxItemInfo.itemIdSrc = ItemInfo.itemID WHERE itemIOut = ? and probability >0";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						int gender = sqlite3_column_int(stmt, 4);
						if (gender != -1)
						{
							if (gender != Client->Gender)
							{
								continue;
							}
						}
						int count = sqlite3_column_int(stmt, 5);
						for (int i = 0; i < count; i++)
						{
							randoms[sum + 1] = sqlite3_column_int(stmt, 0);
							randomName[sum + 1] = U2G((char *)sqlite3_column_text(stmt, 1));
							sum++;
						}
					}
					int randomID;
					std::random_device rd;
					std::mt19937 eng(rd());
					std::uniform_int_distribution<int> dis(0, sum);
					randomID = dis(eng);
					ItemInfo Items[3000];
					ItemInfo Item = PlayerDB_AddItem(Client->Uin, randoms[randomID], 1, -1, false);
					Items[0].ItemID = randoms[randomID];
					Items[0].ItemNum = Item.ItemNum;
					Items[0].AvailPeriod = Item.AvailPeriod;
					Items[0].Status = Item.Status;
					Items[0].ObtainTime = Item.ObtainTime;
					Items[0].OtherAttribute = Item.OtherAttribute;
					Items[0].ItemType = Item.ItemType;
					NotifyClientAddItem(Client, 1, Items);
					std::string temp = randomName[randomID];
					std::string itemIdStr = std::to_string(randoms[randomID]);
					temp = "，/<itemshow=" + itemIdStr + ">[" + temp + "]";
					res1 += temp;
					res1 += "。";
					ResponseStartGame(Client, res1.c_str());
				}
				else
				{
					Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
				}
				sqlite3_finalize(stmt);
			}
			else if (type == 3)
			{ // 混合模式
				const char *Reason = "恭喜你在";
				std::string res1;
				res1 = Reason + itemName + "获得了";
				sql = "SELECT itemIdSrc,itemName,Level,itemType,gender FROM BoxItemInfo LEFT JOIN ItemInfo ON BoxItemInfo.itemIdSrc = ItemInfo.itemID WHERE itemIOut = ? and probability ==0";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						int gender = sqlite3_column_int(stmt, 4);
						if (gender != -1)
						{
							if (gender != Client->Gender)
							{
								continue;
							}
						}
						ItemInfo Items[3000];
						ItemInfo Item = PlayerDB_AddItem(Client->Uin, sqlite3_column_int(stmt, 0), 1, -1, false);
						Items[0].ItemID = sqlite3_column_int(stmt, 0);
						Items[0].ItemNum = Item.ItemNum;
						Items[0].AvailPeriod = Item.AvailPeriod;
						Items[0].Status = Item.Status;
						Items[0].ObtainTime = Item.ObtainTime;
						Items[0].OtherAttribute = Item.OtherAttribute;
						Items[0].ItemType = Item.ItemType;
						NotifyClientAddItem(Client, 1, Items);
						std::string temp = U2G((char *)sqlite3_column_text(stmt, 1));
						std::string itemIdStr = std::to_string(sqlite3_column_int(stmt, 0));
						temp = "，/<itemshow=" + itemIdStr + ">[" + temp + "]";
						type = sqlite3_column_int(stmt, 2);
						res1 += temp;
					}
				}
				else
				{
					Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
				}
				sqlite3_finalize(stmt);
				int sum = -1;
				int randoms[9999];
				std::string randomName[9999];
				sql = "SELECT itemIdSrc,itemName,Level,itemType,gender,probability FROM BoxItemInfo LEFT JOIN ItemInfo ON BoxItemInfo.itemIdSrc = ItemInfo.itemID WHERE itemIOut = ? and probability >0";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{
						int gender = sqlite3_column_int(stmt, 4);
						if (gender != -1)
						{
							if (gender != Client->Gender)
							{
								continue;
							}
						}
						int count = sqlite3_column_int(stmt, 5);
						for (int i = 0; i < count; i++)
						{
							randoms[sum + 1] = sqlite3_column_int(stmt, 0);
							randomName[sum + 1] = U2G((char *)sqlite3_column_text(stmt, 1));
							sum++;
						}
					}
					int randomID;
					std::random_device rd;
					std::mt19937 eng(rd());
					std::uniform_int_distribution<int> dis(0, sum);
					randomID = dis(eng);
					ItemInfo Items[3000];
					ItemInfo Item = PlayerDB_AddItem(Client->Uin, randoms[randomID], 1, -1, false);
					Items[0].ItemID = randoms[randomID];
					Items[0].ItemNum = Item.ItemNum;
					Items[0].AvailPeriod = Item.AvailPeriod;
					Items[0].Status = Item.Status;
					Items[0].ObtainTime = Item.ObtainTime;
					Items[0].OtherAttribute = Item.OtherAttribute;
					Items[0].ItemType = Item.ItemType;
					NotifyClientAddItem(Client, 1, Items);
					std::string temp = randomName[randomID];
					std::string itemIdStr = std::to_string(randoms[randomID]);
					temp = "，/<itemshow=" + itemIdStr + ">[" + temp + "]";
					res1 += temp;
				}
				else
				{
					Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
				}
				sqlite3_finalize(stmt);
				res1 += "。";
				ResponseStartGame(Client, res1.c_str());
			}
			else
			{
				ResponseStartGame(Client, "无效的打开方式");
			}
		}
	}
	else
	{
		ResponseStartGame(Client, "未配置的物品");
	}

	stmt = NULL;
}
// while (sqlite3_step(stmt) == SQLITE_ROW)
//{ //KartRefitInfo
//	if (i < 100) {
//		i++;
//		BYTE* pKartRefitInfo = p;
//		Write16(pKartRefitInfo, 0); //len
//
//		Write32(pKartRefitInfo, Client->Uin); //Uin
//		Write32(pKartRefitInfo, sqlite3_column_int(stmt, 0)); //KartId
void RequestChangeGender(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	BYTE NewGender = Read8(Body);

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	sql = "UPDATE BaseInfo SET Gender=?  WHERE Uin=? ;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, NewGender);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	ResponseChangeGender(Client, NewGender, 0);
}
void ResponseChangeGender(ClientNode *Client, BYTE NewGender, BYTE UseType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write8(p, NewGender);
	Write8(p, 0); // ReasonLen
#ifndef ZingSpeed
	Write8(p, UseType);
#endif
	Client->Gender = NewGender;
	len = p - buf;
	SendToClient(Client, 162, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	getGameMode(Client);
}
void NotifyChangeItemStatus(ClientNode *Client, UINT Uin, USHORT ItemNum, ItemStatus *ItemStatus)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT KartId = 0;
	Write32(p, Uin);
	ClientNode *Temp = GetClient(Uin);
	Write16(p, ItemNum);
	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE *pItemStatus = p;
		Write16(pItemStatus, 0); // len
		BYTE NewStatus = ItemStatus[i].NewStatus;
		UINT ItemID = ItemStatus[i].ItemID;
		UINT ItemType = GetItemType(ItemID);
		if (NewStatus)
		{
			if (ItemType == EAIT_CAR|| ItemType == EAIT_SKIN)
			{
				KartId = ItemID;
			}
		}
		// if (ItemStatus[i].ItemID == Temp->KartID && Temp->KartID == Temp->OrgKartId && ItemStatus[i].NewStatus == 1) {
		//	Write32(pItemStatus, Temp->SkinID);
		// }
		// else {
		//	Write32(pItemStatus, ItemStatus[i].ItemID);
		// }
		Write32(pItemStatus, ItemStatus[i].ItemID);

		Write32(pItemStatus, 1);					  // ItemNum
		Write32(pItemStatus, -1);					  // AvailPeriod
		Write8(pItemStatus, ItemStatus[i].NewStatus); // Status
		Write32(pItemStatus, 1);					  // ObtainTime
		Write32(pItemStatus, 0);					  // OtherAttribute
		Write16(pItemStatus, 0);					  // ItemType

		len = pItemStatus - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 10); // SpecFlag

	// Write16(p, 0); //KartNum
	// KartId
	if (KartId >= 1)
	{
		// UINT ShapeSuitID = GetKartItemShapeSuitID(KartId, Uin);
		UINT ShapeSuitID = 0;
		UINT LegendSuitLevel = 0;
		UINT LegendSuitLevelChoice = 0;
		UINT ShapeLegendSuitID = 0;
		UINT KartHeadRefitItemID = 0;
		UINT KartTailRefitItemID = 0;
		UINT KartFlankRefitItemID = 0;
		UINT KartTireRefitItemID = 0;

		const char *sql = nullptr;
		sqlite3_stmt *stmt = nullptr;
		sql = "SELECT ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,KartID FROM KartRefit WHERE Uin = ? AND (KartID=? OR ShapeSuitID = ?);";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, KartId);
			sqlite3_bind_int(stmt, 3, KartId);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				ShapeSuitID = sqlite3_column_int(stmt, 0);
				LegendSuitLevel = sqlite3_column_int(stmt, 1);
				LegendSuitLevelChoice = sqlite3_column_int(stmt, 2);
				ShapeLegendSuitID = sqlite3_column_int(stmt, 3);
				KartHeadRefitItemID = sqlite3_column_int(stmt, 4);
				KartTailRefitItemID = sqlite3_column_int(stmt, 5);
				KartFlankRefitItemID = sqlite3_column_int(stmt, 6);
				KartTireRefitItemID = sqlite3_column_int(stmt, 7);
				KartId = sqlite3_column_int(stmt, 8);
			}
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;

		Write16(p, 1); // KartNum
		{			   // KartRefitInfo
			BYTE *pKartRefitInfo = p;
			Write16(pKartRefitInfo, 0);						// len
			Write32(pKartRefitInfo, Uin);					// Uin
			Write32(pKartRefitInfo, KartId);				// KartId
			Write32(pKartRefitInfo, 99);					// RefitCout
			Write16(pKartRefitInfo, 0);						// MaxFlags
			Write16(pKartRefitInfo, 0);						// WWeight
			Write16(pKartRefitInfo, 0);						// SpeedWeight
			Write16(pKartRefitInfo, 0);						// JetWeight
			Write16(pKartRefitInfo, 0);						// SJetWeight
			Write16(pKartRefitInfo, 0);						// AccuWeight
			Write32(pKartRefitInfo, 0);						// ShapeRefitCount
			Write32(pKartRefitInfo, KartHeadRefitItemID);	// KartHeadRefitItemID
			Write32(pKartRefitInfo, KartTailRefitItemID);	// KartTailRefitItemID
			Write32(pKartRefitInfo, KartFlankRefitItemID);	// KartFlankRefitItemID
			Write32(pKartRefitInfo, KartTireRefitItemID);	// KartTireRefitItemID
			Write32(pKartRefitInfo, 0);						// SecondRefitCount
			Write16(pKartRefitInfo, 0);						// Speed2Weight
			Write16(pKartRefitInfo, 0);						// DriftVecWeight
			Write16(pKartRefitInfo, 0);						// AdditionalZSpeedWeight
			Write16(pKartRefitInfo, 0);						// AntiCollisionWeight
			Write16(pKartRefitInfo, 0);						// LuckyValue
			Write16(pKartRefitInfo, 0);						// RefitLuckyValueMaxWeight
			Write32(pKartRefitInfo, ShapeSuitID);			// ShapeSuitID
			Write8(pKartRefitInfo, LegendSuitLevel);		// LegendSuitLevel
			Write32(pKartRefitInfo, LegendSuitLevelChoice); // LegendSuitLevelChoice
			Write32(pKartRefitInfo, ShapeLegendSuitID);		// ShapeLegendSuitID

			len = pKartRefitInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	else
	{
		Write16(p, 0); // KartNum
	}

	Write8(p, 0); // ChangeType

	Write8(p, 0); // EquipNiChangItemNum


	len = p - buf;
	SendToClient(Client, 527, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}
void NotifyChangeItemStatus2(ClientNode* Client, UINT Uin, USHORT ItemNum, ItemStatus* ItemStatus)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;
	UINT KartId = 0;
	Write32(p, Uin);

	Write16(p, ItemNum);
	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE* pItemStatus = p;
		Write16(pItemStatus, 0); // len
		BYTE NewStatus = ItemStatus[i].NewStatus;
		UINT ItemID = ItemStatus[i].ItemID;
		UINT ItemType = GetItemType(ItemID);
		if (NewStatus)
		{
			if (ItemType == EAIT_CAR || ItemType == EAIT_SKIN)
			{
				KartId = ItemID;
			}
		}
		/*if (ItemStatus[i].NewStatus==1) {
			Client->KartID = ItemStatus[i].ItemID;
		}*/
		Write32(pItemStatus, ItemStatus[i].ItemID);
		Write32(pItemStatus, 1);					  // ItemNum
		Write32(pItemStatus, -1);					  // AvailPeriod
		Write8(pItemStatus, ItemStatus[i].NewStatus); // Status
		Write32(pItemStatus, 1);					  // ObtainTime
		Write32(pItemStatus, 0);					  // OtherAttribute
		Write16(pItemStatus, 0);					  // ItemType

		len = pItemStatus - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // SpecFlag

	// Write16(p, 0); //KartNum
	// KartId
	//if (KartId >= 1)
	{
		// UINT ShapeSuitID = GetKartItemShapeSuitID(KartId, Uin);
		UINT ShapeSuitID = 0;
		//UINT KartId = 0;
		UINT LegendSuitLevel = 0;
		UINT LegendSuitLevelChoice = 0;
		UINT ShapeLegendSuitID = 0;
		const char* sql = nullptr;
		sqlite3_stmt* stmt = nullptr;
		sql = "SELECT ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID,KartID FROM KartRefit WHERE Uin = ? AND (KartID=? OR ShapeSuitID =?);";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Uin);
			sqlite3_bind_int(stmt, 2, KartId);
			sqlite3_bind_int(stmt, 3, KartId);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				ShapeSuitID = sqlite3_column_int(stmt, 0);
				LegendSuitLevel = sqlite3_column_int(stmt, 1);
				LegendSuitLevelChoice = sqlite3_column_int(stmt, 2);
				ShapeLegendSuitID = sqlite3_column_int(stmt, 3);
				KartId = sqlite3_column_int(stmt, 4);
			}
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;

		Write16(p, 1); // KartNum
		{			   // KartRefitInfo
			BYTE* pKartRefitInfo = p;
			Write16(pKartRefitInfo, 0);						// len
			Write32(pKartRefitInfo, Uin);					// Uin
			Write32(pKartRefitInfo, KartId);				// KartId
			Write32(pKartRefitInfo, 99);					// RefitCout
			Write16(pKartRefitInfo, 0);						// MaxFlags
			Write16(pKartRefitInfo, 0);						// WWeight
			Write16(pKartRefitInfo, 0);						// SpeedWeight
			Write16(pKartRefitInfo, 0);						// JetWeight
			Write16(pKartRefitInfo, 0);						// SJetWeight
			Write16(pKartRefitInfo, 0);						// AccuWeight
			Write32(pKartRefitInfo, 4);						// ShapeRefitCount
			Write32(pKartRefitInfo, 0);						// KartHeadRefitItemID
			Write32(pKartRefitInfo, 0);						// KartTailRefitItemID
			Write32(pKartRefitInfo, 0);						// KartFlankRefitItemID
			Write32(pKartRefitInfo, 0);						// KartTireRefitItemID
			Write32(pKartRefitInfo, 0);						// SecondRefitCount
			Write16(pKartRefitInfo, 0);						// Speed2Weight
			Write16(pKartRefitInfo, 0);						// DriftVecWeight
			Write16(pKartRefitInfo, 0);						// AdditionalZSpeedWeight
			Write16(pKartRefitInfo, 0);						// AntiCollisionWeight
			Write16(pKartRefitInfo, 0);						// LuckyValue
			Write16(pKartRefitInfo, 0);						// RefitLuckyValueMaxWeight
			Write32(pKartRefitInfo, ShapeSuitID);			// ShapeSuitID
			Write8(pKartRefitInfo, LegendSuitLevel);		// LegendSuitLevel
			Write32(pKartRefitInfo, LegendSuitLevelChoice); // LegendSuitLevelChoice
			Write32(pKartRefitInfo, ShapeLegendSuitID);		// ShapeLegendSuitID

			len = pKartRefitInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	//else
	//{
	//	Write16(p, 0); // KartNum
	//}

	Write8(p, 0); // ChangeType

#ifndef ZingSpeed
	Write8(p, 0); // EquipNiChangItemNum

#endif

	len = p - buf;
	SendToClient(Client, 527, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Notify);
}

void RequestChangeItemStatus(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	// UCHAR SpecFlag = Read8(Body);
	// UCHAR ChangeType = Read8(Body);
	USHORT ItemNum = Read16(Body);

	if (ItemNum == 0)
	{
		return;
	}
	ItemStatus *aItemStatus = (ItemStatus *)malloc(sizeof(ItemStatus) * ItemNum);
	if (!aItemStatus)
	{
		return;
	}
	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE *pItemStatus = Body;
		USHORT len = Read16(pItemStatus);
		INT ItemID = Read32(pItemStatus);
		UINT NewStatus = Read8(pItemStatus);
		aItemStatus[i].ItemID = ItemID;
		aItemStatus[i].NewStatus = NewStatus;
		Body += len;
	}
	/*
m_ucSpecFlag
m_ucChangeType
m_usItemNum
m_stEquipedItem[]
m_iNewKartID
	*/

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "BEGIN"; // 开始事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);

	for (size_t i2 = 0; i2 < ItemNum; i2++)
	{
		if (aItemStatus[i2].NewStatus && GetItemType(aItemStatus[i2].ItemID) == EAIT_CAR)
		{
			if (Client->ServerID == 900 && GetItemGameType(aItemStatus[i2].ItemID) != 1)
			{ // 滑板
				ResponseStartGame(Client, "请装备滑板");
				return;
			}
			else if (Client->ServerID == 1000 && GetItemGameType(aItemStatus[i2].ItemID) != 2)
			{ // 轮滑
				ResponseStartGame(Client, "请装备轮滑");
				return;
			}
			else
			{ // 其他
				// if (Client->ServerID != 116) {
				if (GetItemGameType(aItemStatus[i2].ItemID) == 1 && Client->ServerID != 900)
				{
					ResponseStartGame(Client, "普通模式无法装备滑板");
					return;
				}
				else if (GetItemGameType(aItemStatus[i2].ItemID) == 2 && Client->ServerID != 1000)
				{
					ResponseStartGame(Client, "普通模式无法装备轮滑");
					return;
				}
				//}

				Client->KartID = aItemStatus[i2].ItemID;

				Z_INFO("新赛车ID:%d\n", Client->KartID);
			}
		}
		else if (aItemStatus[i2].NewStatus && GetItemType(aItemStatus[i2].ItemID) == EAIT_PET)
		{
			Client->Pet->ID = aItemStatus[i2].ItemID;
			GetPetInfo(Client);
			
		}
		if (aItemStatus[i2].NewStatus == 1)
		{
			sql = "SELECT ShapeSuitID FROM KartRefit WHERE Uin = ? AND KartID = ?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, aItemStatus[i2].ItemID);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{
					if (GetItemType(sqlite3_column_int(stmt, 0)) == EAIT_SKIN)
					{
						Client->OrgKartId = Client->KartID;
						Client->SkinID = sqlite3_column_int(stmt, 0);
					}
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}

		size_t i = 0;
		sql = "UPDATE Item SET Status=? WHERE Uin=? AND ItemID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, aItemStatus[i2].NewStatus);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			sqlite3_bind_int(stmt, 3, aItemStatus[i2].ItemID);
			result = sqlite3_step(stmt);

			Z_INFO("物品ID:%d\n", aItemStatus[i2].ItemID);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (aItemStatus[i2].NewStatus)
		{
			if (GetItemType(aItemStatus[i2].ItemID) == EAIT_CAR && GetItemGameType(aItemStatus[i2].ItemID) == 0)
			{
				sql = "UPDATE BaseGameTypeStatus SET KartID=? WHERE Uin=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, aItemStatus[i2].ItemID);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					result = sqlite3_step(stmt);

					Z_INFO("物品ID:%d\n", aItemStatus[i2].ItemID);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			else if (GetItemGameType(aItemStatus[i2].ItemID) == 1)
			{
				sql = "UPDATE BaseGameTypeStatus SET SlideID=? WHERE Uin=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, aItemStatus[i2].ItemID);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					result = sqlite3_step(stmt);

					Z_INFO("物品ID:%d\n", aItemStatus[i2].ItemID);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			else if (GetItemGameType(aItemStatus[i2].ItemID) == 2 && Client->Gender == 0)
			{
				sql = "UPDATE BaseGameTypeStatus SET SkateID0=? WHERE Uin=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, aItemStatus[i2].ItemID);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					result = sqlite3_step(stmt);

					Z_INFO("物品ID:%d\n", aItemStatus[i2].ItemID);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			else if (GetItemGameType(aItemStatus[i2].ItemID) == 2 && Client->Gender == 1)
			{
				sql = "UPDATE BaseGameTypeStatus SET SkateID1=? WHERE Uin=?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, aItemStatus[i2].ItemID);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					result = sqlite3_step(stmt);

					Z_INFO("物品ID:%d\n", aItemStatus[i2].ItemID);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
		}
	}

	sql = "COMMIT"; // 提交事务
	result = sqlite3_exec(PlayerDB, sql, NULL, NULL, NULL);
	ResponseChangeItemStatus(Client, ItemNum, aItemStatus);
	// ResponseSaveShapeRefit4(Client,Client->KartID);
	do
	{ // 普通房间
		RoomNode *Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			break;
		}
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
			}
		}
	} while (false);
	do
	{ // 休闲区
		ChatRoomNode *ChatRoom = GetChatRoom(Client->ChatRoomID);
		if (!ChatRoom)
		{
			return;
		}
		for (char i = 0; i < 99; i++)
		{
			ClientNode *RoomClient = ChatRoom->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
			}
		}
	} while (false);
	free(aItemStatus);
}
void ResponseChangeItemStatus(ClientNode *Client, USHORT ItemNum, ItemStatus *ItemStatus)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID

	Write16(p, ItemNum);

	for (size_t i = 0; i < ItemNum; i++)
	{
		BYTE *pItemStatus = p;
		Write16(pItemStatus, 0); // len
		Write32(pItemStatus, ItemStatus[i].ItemID);

		Write8(pItemStatus, ItemStatus[i].NewStatus);

		len = pItemStatus - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // ReasonLen

	Write8(p, 10); // SpecFlag
	Write8(p, 0);  // ChangeType

	len = p - buf;
	SendToClient(Client, 130, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestChangeModel(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE NewModel = Read8(Body);
	// UCHAR UseType = Read8(Body);

	ResponseChangeModel(Client, NewModel);
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
			NotifyChangeModel(RoomClient, Client->Uin, NewModel);
		}
	}
}
void ResponseChangeModel(ClientNode *Client, BYTE NewModel)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write8(p, 0);  // ReasonLen
	Write8(p, NewModel);
#ifndef ZingSpeed
	Write8(p, 0); // UseType
	Write8(p, 0); // TDCExcluItemNum
	// Write32(p, 0); //TDCExclusiveItemID[]
#endif

	len = p - buf;
	SendToClient(Client, 277, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyChangeModel(ClientNode *Client, UINT Uin, BYTE NewModel)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin);
	Write8(p, NewModel);
#ifndef ZingSpeed
	Write8(p, 0); // TDCExcluItemNum
	// Write32(p, 0); //TDCExclusiveItemID[]
#endif

	len = p - buf;
	SendToClient(Client, 278, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyClientVipFlag(ClientNode *Client)
{

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int VipFlag = 0;
	int VipGrowRate = 0;

	sql = "SELECT VipFlag,VipGrowRate  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			VipFlag = sqlite3_column_int(stmt, 0);
			VipGrowRate = sqlite3_column_int(stmt, 1);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	{//贵族等级优化
		if (VipFlag > 7) {
			VipFlag = 7;
		}
	}
	Write32(p, Client->Uin); // Uin
	Write16(p, VipFlag);	 // VipFlag 紫钻
	Write16(p, 0);			 // MsgLen
	Write16(p, VipGrowRate); // VipGrowRate

	len = p - buf;
	SendToClient(Client, 948, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdateNobleInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int NobleLevel = 0;
	int NoblePoint = 0;
	int NobleLeftDays = 0;
	int NobleID = 0;
	sql = "SELECT NobleLevel,NoblePoint,NobleLeftDays,NobleID  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			NobleLevel = sqlite3_column_int(stmt, 0);
			NoblePoint = sqlite3_column_int(stmt, 1);
			NobleLeftDays = sqlite3_column_int(stmt, 2);
			NobleID = sqlite3_column_int(stmt, 3);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	{//贵族等级优化
		if (NobleLevel > 7) {
			NobleLevel = 7;
		}
	}
	Write32(p, Client->Uin); // Uin
	{						 // NobleInfo
		BYTE *pNobleInfo = p;
		Write16(pNobleInfo, 0); // len

		Write32(pNobleInfo, NobleID);	// NobleID
		Write8(pNobleInfo, NobleLevel);		// NobleLevel
		Write32(pNobleInfo, NoblePoint);	// NoblePoint
		Write32(pNobleInfo, NobleLeftDays); // NobleLeftDays

		len = pNobleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 961, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdateEmperorInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int EmperorLevel = 0;
	int EmperorPoint = 0;
	int EmperorLeftDays = 0;
	int EmperorGrowRate = 0;
	sql = "SELECT EmperorLevel,EmperorPoint,EmperorLeftDays,EmperorGrowRate  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			EmperorLevel = sqlite3_column_int(stmt, 0);
			EmperorPoint = sqlite3_column_int(stmt, 1);
			EmperorLeftDays = sqlite3_column_int(stmt, 2);
			EmperorGrowRate = sqlite3_column_int(stmt, 3);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write32(p, Client->Uin); // Uin
	Write8(p, 0);			 // OpenType
	{						 // EmperorInfo
		BYTE *pEmperorInfo = p;
		Write16(pEmperorInfo, 0); // len

		Write8(pEmperorInfo, EmperorLevel);		// EmperorLevel
		Write32(pEmperorInfo, EmperorPoint);	// EmperorPoint
		Write32(pEmperorInfo, EmperorLeftDays); // EmperorLeftDays
		Write8(pEmperorInfo, EmperorGrowRate);	// EmperorGrowRate

		len = pEmperorInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // EmperorOtherInfo
		BYTE *pEmperorOtherInfo = p;
		Write16(pEmperorOtherInfo, 0); // len

		Write32(pEmperorOtherInfo, 0); // ExpiredTime
		Write8(pEmperorOtherInfo, 0);  // ShowExpireTips

		len = pEmperorOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 978, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdateLoverVipInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int LoverVipLevel = 0;
	int LoverVipPoint = 0;
	int LoverVipLeftDays = 0;
	sql = "SELECT LoverVipLevel,LoverVipPoint,LoverVipLeftDays  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			LoverVipLevel = sqlite3_column_int(stmt, 0);
			LoverVipPoint = sqlite3_column_int(stmt, 1);
			LoverVipLeftDays = sqlite3_column_int(stmt, 2);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write32(p, Client->Uin); // Uin
	{						 // LoverVipBaseInfo
		BYTE *pLoverVipBaseInfo = p;
		Write16(pLoverVipBaseInfo, 0); // len

		Write8(pLoverVipBaseInfo, LoverVipLevel);  // LoverVipLevel
		Write32(pLoverVipBaseInfo, LoverVipPoint); // LoverVipPoint
		Write8(pLoverVipBaseInfo, 0);			   // GrowRate

		len = pLoverVipBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // LoverVipOtherInfo
		BYTE *pLoverVipOtherInfo = p;
		Write16(pLoverVipOtherInfo, 0); // len

		Write32(pLoverVipOtherInfo, LoverVipLeftDays); // LoverVipLeftDays
		Write8(pLoverVipOtherInfo, 0);				   // CanReceiveGift
		Write8(pLoverVipOtherInfo, 0);				   // ShowExpireTips

		len = pLoverVipOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 13800, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdatePersonalGardenBaseInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int GardenLevel = 0;
	int GardenPoint = 0;
	int GardenExpireTime = 0;
	sql = "SELECT GardenLevel,GardenPoint,GardenExpireTime  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			GardenLevel = sqlite3_column_int(stmt, 0);
			GardenPoint = sqlite3_column_int(stmt, 1);
			GardenExpireTime = sqlite3_column_int(stmt, 2);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write32(p, 0); // Uin
	{			   // BaseInfo
		BYTE *pBaseInfo = p;
		Write16(pBaseInfo, 0); // len

		Write8(pBaseInfo, GardenLevel);		  // GardenLevel
		Write32(pBaseInfo, GardenPoint);	  // GardenPoint
		Write8(pBaseInfo, 0);				  // GrowRate
		Write32(pBaseInfo, GardenExpireTime); // GardenExpireTime
		{									  // SimpleInfo
			BYTE *pSimpleInfo = pBaseInfo;
			Write16(pSimpleInfo, 0); // len

			if (GardenLevel == 0)
			{
				Write32(pSimpleInfo, 0); // WeekPopularity
				Write32(pSimpleInfo, 0); // TotalPopularity
				Write32(pSimpleInfo, 0); // LastUpdatePopularityTime
				Write8(pSimpleInfo, 0);	 // PrivateType
			}
			else
			{
				Write32(pSimpleInfo, 1888888); // WeekPopularity
				Write32(pSimpleInfo, 1888888); // TotalPopularity
				Write32(pSimpleInfo, 7);	   // LastUpdatePopularityTime
				Write8(pSimpleInfo, 7);		   // PrivateType
			}

			len = pSimpleInfo - pBaseInfo;
			Set16(pBaseInfo, (WORD)len);
			pBaseInfo += len;
		}

		len = pBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 20301, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdateConsumeVipRealteInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int VipLevel = 0;
	int CharmValueOfMonth = 0;
	sql = "SELECT VipLevel,CharmValueOfMonth  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			VipLevel = sqlite3_column_int(stmt, 0);
			CharmValueOfMonth = sqlite3_column_int(stmt, 1);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	{//贵族等级优化
		if (VipLevel > 9) {
			VipLevel = 9;
		}
		
	}
	Write32(p, Client->Uin); // Uin
	{						 // ConsumeVipRelateInfo
		BYTE *pConsumeVipRelateInfo = p;
		Write16(pConsumeVipRelateInfo, 0); // len

		Write32(pConsumeVipRelateInfo, VipLevel);		   // VipLevel
		Write32(pConsumeVipRelateInfo, CharmValueOfMonth); // CharmValueOfMonth
		Write32(pConsumeVipRelateInfo, 0);				   // SearchTreasureNums
		Write32(pConsumeVipRelateInfo, 0);				   // GetTreasureNums

		len = pConsumeVipRelateInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 16062, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdateGuildVipInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int GuildVipLevel = 0;
	int GuildVipPoint = 0;
	int GuildVipLeftDays = 0;
	sql = "SELECT GuildVipLevel,GuildVipPoint,GuildVipLeftDays  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			GuildVipLevel = sqlite3_column_int(stmt, 0);
			GuildVipPoint = sqlite3_column_int(stmt, 1);
			GuildVipLeftDays = sqlite3_column_int(stmt, 1);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write32(p, Client->Uin); // Uin
	{						 // BaseInfo
		BYTE *pBaseInfo = p;
		Write16(pBaseInfo, 0); // len

		Write8(pBaseInfo, GuildVipLevel);  // GuildVipLevel
		Write32(pBaseInfo, GuildVipPoint); // GuildVipPoint

		len = pBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // OtherInfo
		BYTE *pOtherInfo = p;
		Write16(pOtherInfo, 0); // len

		Write32(pOtherInfo, GuildVipLeftDays); // GuildVipLeftDays
		Write8(pOtherInfo, 0);				   // CanReceiveGift

		len = pOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 12031, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyKickFromServer(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ReasonID
	Write32(p, Client->Uin); // SrcUin
	Write8(p, 0);			 // ReasonLen
	Write8(p, 0);			 // AntiCheatMode
	Write8(p, 0);			 // KickType

	len = p - buf;
	SendToClient(Client, 900, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Notify);
}

void ResponseKartRefitInfoMoreInfo(ClientNode *Client, size_t num);

void RequestKartRefitInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseKartRefitInfo(Client);
}

void ResponseKartRefitInfo(ClientNode *Client)
{
	BYTE buf[8192 * 4];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin

	bool HasMoreInfo = false;

	BYTE *pKartNum = p;
	Write16(p, 0); // KartNum
	{
		const char *sql = NULL;
		sqlite3_stmt *stmt = NULL;
		int result;
		size_t i = 0;
		sql = "SELECT KartID,RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin = ? And  (RefitCout >0 Or shapesuitID >0 Or(KartHeadRefitItemID>0 Or KartTailRefitItemID>0 Or KartFlankRefitItemID>0 Or KartTireRefitItemID>0)) order by shapesuitID desc,RefitCout desc,KartID desc;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{ // KartRefitInfo
				if (i < 300)
				{
					i++;
					BYTE *pKartRefitInfo = p;
					Write16(pKartRefitInfo, 0); // len

					INT temp = 0;
					Write32(pKartRefitInfo, Client->Uin);					   // Uin
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartId
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // RefitCout
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // MaxFlags
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // WWeight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // SpeedWeight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // JetWeight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // SJetWeight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // AccuWeight
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // ShapeRefitCount
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartHeadRefitItemID
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartTailRefitItemID
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartFlankRefitItemID
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartTireRefitItemID
					{														   // KartRefitExInfo
						BYTE *pKartRefitExInfo = pKartRefitInfo;
						Write16(pKartRefitExInfo, 0); // len
						if (sqlite3_column_int(stmt, 20) == 0) {
							if (sqlite3_column_int(stmt, 9) > 0) {
								Write8(pKartRefitExInfo, 3); // SpeedRefitStar
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}
							if (sqlite3_column_int(stmt, 10) > 0) {
								Write8(pKartRefitExInfo, 3); // JetRefitStar
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}
							if (sqlite3_column_int(stmt, 11) > 0) {
								Write8(pKartRefitExInfo, 3); // SJetRefitStar
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}
							if (sqlite3_column_int(stmt, 12) > 0) {
								Write8(pKartRefitExInfo, 3); // AccuRefitStar
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}

							if (sqlite3_column_int(stmt, 9) > 0) {
								Write8(pKartRefitExInfo, 10); // SpeedAddRatio
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}
							if (sqlite3_column_int(stmt, 10) > 0) {
								Write8(pKartRefitExInfo, 10); // JetAddRatio
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}
							if (sqlite3_column_int(stmt, 11) > 0) {
								Write8(pKartRefitExInfo, 10); // SJetAddRatio
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}
							if (sqlite3_column_int(stmt, 12) > 0) {
								Write8(pKartRefitExInfo, 10); // AccuAddRatio
							}
							else {
								Write8(pKartRefitExInfo, 0);
							}
						}
						else {
							Write8(pKartRefitExInfo, 3); // SpeedRefitStar
							Write8(pKartRefitExInfo, 3); // JetRefitStar
							Write8(pKartRefitExInfo, 3); // SJetRefitStar
							Write8(pKartRefitExInfo, 3); // AccuRefitStar
							Write8(pKartRefitExInfo, 10); // SpeedAddRatio
							Write8(pKartRefitExInfo, 10); // JetAddRatio
							Write8(pKartRefitExInfo, 10); // SJetAddRatio
							Write8(pKartRefitExInfo, 10); // AccuAddRatio
						}
						len = pKartRefitExInfo - pKartRefitInfo;
						Set16(pKartRefitInfo, (WORD)len);
						pKartRefitInfo += len;
					}
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // SecondRefitCount
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // Speed2Weight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // DriftVecWeight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // AdditionalZSpeedWeight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // AntiCollisionWeight
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // LuckyValue
					Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // RefitLuckyValueMaxWeight
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // ShapeSuitID
					Write8(pKartRefitInfo, sqlite3_column_int(stmt, temp++));  // LegendSuitLevel
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // LegendSuitLevelChoice
					Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // ShapeLegendSuitID

					len = pKartRefitInfo - p;
					Set16(p, (WORD)len);
					p += len;
				}
				else
				{
					HasMoreInfo = true;
					break;
				}
			}
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
#ifdef DEBUG
		Z_INFO("更多赛车皮肤信息 信息列表: %d num: %d\n", HasMoreInfo, i);
#endif
		Set16(pKartNum, (WORD)i);
	}

#ifndef ZingSpeed
	Write32(p, 0); // MaxLuckyWeight
	Write32(p, 0); // LuckyValue
	Write32(p, 0); // MaxLuckyValue
	Write8(p, 0);  // LevelNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //LuckyValueInfo
		BYTE* pLuckyValueInfo = p;
		Write16(pLuckyValueInfo, 0); //len

		Write32(pLuckyValueInfo, 0); //Level
		Write32(pLuckyValueInfo, 0); //LuckyValue

		len = pLuckyValueInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, HasMoreInfo); // HasMoreInfo
#endif

	len = p - buf;
	SendToClient(Client, 271, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (HasMoreInfo)
	{
		ResponseKartRefitInfoMoreInfo(Client, 1);
	}
}

void ResponseKartRefitInfoMoreInfo(ClientNode *Client, size_t num)
{
	BYTE buf[8192 * 3];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	BYTE *pKartNum = p;
	Write16(p, 0); // KartNum
	bool HasMoreInfo = false;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	size_t i = 0;
	sql = "SELECT KartID,RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin = ? And  (RefitCout >0 Or shapesuitID >0 Or(KartHeadRefitItemID>0 Or KartTailRefitItemID>0 Or KartFlankRefitItemID>0 Or KartTireRefitItemID>0)) order by shapesuitID desc,RefitCout desc,KartID desc;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		if (num == 1)
		{
			sqlite3_bind_int(stmt, 2, num * 300);
		}
		else
		{
			sqlite3_bind_int(stmt, 2, num * 200);
		}

		while (sqlite3_step(stmt) == SQLITE_ROW)
		{ // KartRefitInfo
			if (i < 200)
			{
				i++;
				BYTE *pKartRefitInfo = p;
				Write16(pKartRefitInfo, 0); // len

				INT temp = 0;
				Write32(pKartRefitInfo, Client->Uin);					   // Uin
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartId
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // RefitCout
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // MaxFlags
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // WWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // SpeedWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // JetWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // SJetWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // AccuWeight
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // ShapeRefitCount
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartHeadRefitItemID
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartTailRefitItemID
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartFlankRefitItemID
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // KartTireRefitItemID
				{														   // KartRefitExInfo
					BYTE *pKartRefitExInfo = pKartRefitInfo;
					Write16(pKartRefitExInfo, 0); // len

					if (sqlite3_column_int(stmt, 20) == 0) {
						if (sqlite3_column_int(stmt, 9) > 0) {
							Write8(pKartRefitExInfo, 3); // SpeedRefitStar
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}
						if (sqlite3_column_int(stmt, 10) > 0) {
							Write8(pKartRefitExInfo, 3); // JetRefitStar
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}
						if (sqlite3_column_int(stmt, 11) > 0) {
							Write8(pKartRefitExInfo, 3); // SJetRefitStar
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}
						if (sqlite3_column_int(stmt, 12) > 0) {
							Write8(pKartRefitExInfo, 3); // AccuRefitStar
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}

						if (sqlite3_column_int(stmt, 9) > 0) {
							Write8(pKartRefitExInfo, 10); // SpeedAddRatio
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}
						if (sqlite3_column_int(stmt, 10) > 0) {
							Write8(pKartRefitExInfo, 10); // JetAddRatio
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}
						if (sqlite3_column_int(stmt, 11) > 0) {
							Write8(pKartRefitExInfo, 10); // SJetAddRatio
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}
						if (sqlite3_column_int(stmt, 12) > 0) {
							Write8(pKartRefitExInfo, 10); // AccuAddRatio
						}
						else {
							Write8(pKartRefitExInfo, 0);
						}
					}
					else {
						Write8(pKartRefitExInfo, 3); // SpeedRefitStar
						Write8(pKartRefitExInfo, 3); // JetRefitStar
						Write8(pKartRefitExInfo, 3); // SJetRefitStar
						Write8(pKartRefitExInfo, 3); // AccuRefitStar
						Write8(pKartRefitExInfo, 10); // SpeedAddRatio
						Write8(pKartRefitExInfo, 10); // JetAddRatio
						Write8(pKartRefitExInfo, 10); // SJetAddRatio
						Write8(pKartRefitExInfo, 10); // AccuAddRatio
					}

					len = pKartRefitExInfo - pKartRefitInfo;
					Set16(pKartRefitInfo, (WORD)len);
					pKartRefitInfo += len;
				}
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // SecondRefitCount
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // Speed2Weight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // DriftVecWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // AdditionalZSpeedWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // AntiCollisionWeight
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // LuckyValue
				Write16(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // RefitLuckyValueMaxWeight
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // ShapeSuitID
				Write8(pKartRefitInfo, sqlite3_column_int(stmt, temp++));  // LegendSuitLevel
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // LegendSuitLevelChoice
				Write32(pKartRefitInfo, sqlite3_column_int(stmt, temp++)); // ShapeLegendSuitID

				len = pKartRefitInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
			else
			{
				HasMoreInfo = true;
				break;
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
#ifdef DEBUG
	Z_INFO("更多赛车外表改装信息 信息列表: %d num: %d\n", HasMoreInfo, i);
#endif
	Set16(pKartNum, (WORD)i);

	Write8(p, HasMoreInfo); // HasNext

	len = p - buf;
	SendToClient(Client, 318, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (HasMoreInfo)
	{
		num++;
		ResponseKartRefitInfoMoreInfo(Client, num);
	}
}
void RequestAddFriendGroup(ClientNode* Client)
{
	YAML::Node GameConfig = getGameConfig();
	if (GameConfig["HaveFriend"] && GameConfig["HaveFriend"].as<UINT>() == 1)
	{
		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		sql = "SELECT Uin FROM BaseInfo";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		if (stmt)
		{
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				if (sqlite3_column_int(stmt, 0) != Client->Uin)
				{
					ResponseFindPlayerByQQ(Client, sqlite3_column_int(stmt, 0),0,0);
				}
			}
		}
	}
	

}

void RequestNewGetFriendList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	YAML::Node GameConfig = getGameConfig();

	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "UPDATE BaseInfo SET Verify=0 WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (GameConfig["HaveFriend"] && GameConfig["HaveFriend"].as<UINT>() == 1)
	{
		ResponseNewGetFriendList(Client);
	}
	else {
		ResponseNewGetFriendList2(Client);
	}

	if (GameConfig["HaveAppellation"] && GameConfig["HaveAppellation"].as<UINT>() == 1)
	{
		
		NotifyAppellationList(Client);
	}

	if (GameConfig["HaveGuild"] && GameConfig["HaveGuild"].as<UINT>() == 1)
	{
		NotifyAddToGuildResult(Client);
	}

	if (GameConfig["HaveRankedMatch"] && GameConfig["HaveRankedMatch"].as<UINT>() == 1)
	{
		NotifyRankedMatchSeasonInfo(Client);
	}
	NotifySpecialActivityInfo(Client);
	NotifyPetStrengCfgInfo(Client);
	NotifyPetHuanHuaCfgInfo(Client);
}
void ResponseNewGetFriendList(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	INT TotalFriendNum = 100;
	INT FriendGroupNum = 1;
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	ItemInfo Items[200];
	size_t FriendNum = 0;
	bool HasMoreInfo = false;
		sql = "SELECT Uin FROM BaseInfo";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		if (stmt)
		{
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{
				if (sqlite3_column_int(stmt, 0) != Client->Uin)
				{
					Items[FriendNum].ItemID = sqlite3_column_int(stmt, 0);
					FriendNum++;
				}
			}
		}
	
	Write16(p, 0);				// ResultID
	Write32(p, Client->Uin);	// Uin
	Write16(p, 9999);			// MaxFriendNum
	Write16(p, TotalFriendNum); // TotalFriendNum
	Write16(p, FriendGroupNum); // FriendGroupNum

	for (size_t i = 0; i < FriendGroupNum; i++)
	{ // FriendGroupInfo
		BYTE *pFriendGroupInfo = p;
		Write16(pFriendGroupInfo, 0); // len

		Write16(pFriendGroupInfo, 1); // GroupID
		// GroupName[]
		/*Write8(pFriendGroupInfo, MaxNickName);
		memcpy(pFriendGroupInfo, "我的好友", MaxNickName);
		pFriendGroupInfo += MaxNickName;*/
		WriteString(pFriendGroupInfo, "玩家列表", MaxNickName, false);
		// FriendNum = 20;
		Write16(pFriendGroupInfo, FriendNum); // FriendNum
		for (size_t i = 0; i < FriendNum; i++)
		{ // Friend
			BYTE *pFriend = pFriendGroupInfo;
			Write16(pFriend, 0); // len
			// ResponseFindPlayerByQQ2(Client, Items[FriendNum].ItemID);
			Write32(pFriend, Items[i].ItemID); // FriendUin
			Write32(pFriend, 0);			   // RelationFlag
			Write32(pFriend, 0);			   // Degree
			Write8(pFriend, 0);				   // Flag
			Write32(pFriend, 0);			   // ReadedEventID

			len = pFriend - pFriendGroupInfo;
			Set16(pFriendGroupInfo, (WORD)len);
			pFriendGroupInfo += len;
		}

		len = pFriendGroupInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // ReasonLen


	len = p - buf;
	SendToClient(Client, 163, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetGroupFriendBaseInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short GroupID = Read16(p);
	//ResponseGetGroupFriendBaseInfo(Client,Uin);
}

void ResponseGetGroupFriendBaseInfo(ClientNode* Client, UINT Uin,const char NickName)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write16(p, 1); //GroupID
	{ //GroupFriendBaseInfo
		BYTE* pGroupFriendBaseInfo = p;
		Write16(pGroupFriendBaseInfo, 0); //len

		WriteUn(pGroupFriendBaseInfo, Uin); //Uin
		WriteString(pGroupFriendBaseInfo, Client->NickName,16); //NickName[]
		Write8(pGroupFriendBaseInfo, 0); //Gender
		WriteUn(pGroupFriendBaseInfo, 0); //Identity
		Write8(pGroupFriendBaseInfo, 0); //IsOnLine
		WriteUn(pGroupFriendBaseInfo, 0); //Experience
		Write32(pGroupFriendBaseInfo, 0); //License
		Write32(pGroupFriendBaseInfo, 0); //TotalHonor
		Write32(pGroupFriendBaseInfo, 0); //Charm
		Write32(pGroupFriendBaseInfo, 0); //DanceExp
		Write32(pGroupFriendBaseInfo, 0); //Admiration
		Write32(pGroupFriendBaseInfo, 0); //TeamWorkExp
		Write8(pGroupFriendBaseInfo, 0); //Signature[]
		WriteUn(pGroupFriendBaseInfo, 0); //VipFlag
		Write32(pGroupFriendBaseInfo, 0); //LocaleCode
		Write8(pGroupFriendBaseInfo, 0); //HaveAppellation
		for (size_t i = 0; i < 0; i++)
		{ //CurAppellation
			BYTE* pCurAppellation = pGroupFriendBaseInfo;
			Write16(pCurAppellation, 0); //len

			Write8(pCurAppellation, 0); //Type
			Write8(pCurAppellation, 0); //Level
			Write8(pCurAppellation, 0); //Status
			Write8(pCurAppellation, 0); //Difficulty
			Write32(pCurAppellation, 0); //MapId
			Write32(pCurAppellation, 0); //Value

			len = pCurAppellation - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		{ //NobleInfo
			BYTE* pNobleInfo = pGroupFriendBaseInfo;
			Write16(pNobleInfo, 0); //len

			Write32(pNobleInfo, 0); //NobleID
			Write8(pNobleInfo, 0); //NobleLevel
			Write32(pNobleInfo, 0); //NoblePoint
			Write32(pNobleInfo, 0); //NobleLeftDays

			len = pNobleInfo - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		Write8(pGroupFriendBaseInfo, 0); //HasWizard
		{ //GuildVipBaseInfo
			BYTE* pGuildVipBaseInfo = pGroupFriendBaseInfo;
			Write16(pGuildVipBaseInfo, 0); //len

			Write8(pGuildVipBaseInfo, 0); //GuildVipLevel
			Write32(pGuildVipBaseInfo, 0); //GuildVipPoint

			len = pGuildVipBaseInfo - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		Write8(pGroupFriendBaseInfo, 0); //HasLDMInfo
		for (size_t i = 0; i < 0; i++)
		{ //LDMInfo
			BYTE* pLDMInfo = pGroupFriendBaseInfo;
			Write16(pLDMInfo, 0); //len

			Write8(pLDMInfo, 0); //Grade
			Write32(pLDMInfo, 0); //Score
			Write32(pLDMInfo, 0); //MaxScore
			Write32(pLDMInfo, 0); //MaxGrade

			len = pLDMInfo - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		Write8(pGroupFriendBaseInfo, 0); //HasLoverVip
		for (size_t i = 0; i < 0; i++)
		{ //LoverVipInfo
			BYTE* pLoverVipInfo = pGroupFriendBaseInfo;
			Write16(pLoverVipInfo, 0); //len

			Write8(pLoverVipInfo, 0); //LoverVipLevel
			Write32(pLoverVipInfo, 0); //LoverVipPoint
			Write8(pLoverVipInfo, 0); //GrowRate

			len = pLoverVipInfo - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		Write32(pGroupFriendBaseInfo, 0); //MaxDayPveScore
		Write32(pGroupFriendBaseInfo, 0); //MaxHistoryPveScore
		WriteUn(pGroupFriendBaseInfo, 0); //totalMapMedalNum
		Write32(pGroupFriendBaseInfo, 0); //ChumCircleID
		Write8(pGroupFriendBaseInfo, 0); //GardenLevel
		{ //EmperorInfo
			BYTE* pEmperorInfo = pGroupFriendBaseInfo;
			Write16(pEmperorInfo, 0); //len

			Write8(pEmperorInfo, 0); //EmperorLevel
			Write32(pEmperorInfo, 0); //EmperorPoint
			Write32(pEmperorInfo, 0); //EmperorLeftDays
			Write8(pEmperorInfo, 0); //EmperorGrowRate

			len = pEmperorInfo - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		Write8(pGroupFriendBaseInfo, 0); //GardenPrivateType
		{ //ConsumeVipInfo
			BYTE* pConsumeVipInfo = pGroupFriendBaseInfo;
			Write16(pConsumeVipInfo, 0); //len

			WriteUn(pConsumeVipInfo, 0); //VipLevel
			WriteUn(pConsumeVipInfo, 0); //CharmValueOfMonth
			WriteUn(pConsumeVipInfo, 0); //SearchTreasureNums
			WriteUn(pConsumeVipInfo, 0); //GetTreasureNums

			len = pConsumeVipInfo - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		Write8(pGroupFriendBaseInfo, 0); //IsHelperAppOnline
		Write8(pGroupFriendBaseInfo, 0); //AreaID
		Write8(pGroupFriendBaseInfo, 0); //HasRankedMatchInfo
		for (size_t i = 0; i < 0; i++)
		{ //RankedMatchGradeInfo
			BYTE* pRankedMatchGradeInfo = pGroupFriendBaseInfo;
			Write16(pRankedMatchGradeInfo, 0); //len

			Write32(pRankedMatchGradeInfo, 0); //GradeLevel
			Write32(pRankedMatchGradeInfo, 0); //ChildLevel
			Write32(pRankedMatchGradeInfo, 0); //GradeScore
			Write32(pRankedMatchGradeInfo, 0); //MaxGradeScore
			Write32(pRankedMatchGradeInfo, 0); //TotalScore

			len = pRankedMatchGradeInfo - pGroupFriendBaseInfo;
			Set16(pGroupFriendBaseInfo, (WORD)len);
			pGroupFriendBaseInfo += len;
		}
		Write8(pGroupFriendBaseInfo, 0); //ActiveStatus
		Write32(pGroupFriendBaseInfo, 0); //HelperLev

		len = pGroupFriendBaseInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //ReasonLen

	len = p - buf;
	SendToClient(Client, 148, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseNewGetFriendList2(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	INT TotalFriendNum = 1;
	INT FriendGroupNum = 1;
	Write16(p, 0);				// ResultID
	Write32(p, Client->Uin);	// Uin
	Write16(p, 1);				// MaxFriendNum
	Write16(p, TotalFriendNum); // TotalFriendNum
	Write16(p, FriendGroupNum); // FriendGroupNum

	for (size_t i = 0; i < FriendGroupNum; i++)
	{ // FriendGroupInfo
		BYTE *pFriendGroupInfo = p;
		Write16(pFriendGroupInfo, 0); // len

		Write16(pFriendGroupInfo, 1); // GroupID
		// GroupName[]
		/*Write8(pFriendGroupInfo, MaxNickName);
		memcpy(pFriendGroupInfo, "我的好友", MaxNickName);
		pFriendGroupInfo += MaxNickName;*/
		WriteString(pFriendGroupInfo, "我的好友", MaxNickName, false);
		// FriendNum = 20;
		Write16(pFriendGroupInfo, 1); // FriendNum
		for (size_t i = 0; i < 1; i++)
		{ // Friend
			BYTE *pFriend = pFriendGroupInfo;
			Write16(pFriend, 0); // len
			// ResponseFindPlayerByQQ2(Client, Items[FriendNum].ItemID);
			Write32(pFriend, 0); // FriendUin
			Write32(pFriend, 0); // RelationFlag
			Write32(pFriend, 0); // Degree
			Write8(pFriend, 0);	 // Flag
			Write32(pFriend, 0); // ReadedEventID

			len = pFriend - pFriendGroupInfo;
			Set16(pFriendGroupInfo, (WORD)len);
			pFriendGroupInfo += len;
		}

		len = pFriendGroupInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 163, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyClientAddItem(ClientNode *Client, UINT ItemNum, ItemInfo *Items)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, Client->Uin); // SrcUin
	Write16(p, ItemNum);	 // ItemNum
	{
		for (size_t i = 0; i < ItemNum; i++)
		{
			BYTE *pItemInfo = p;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, Items[i].ItemID);
			Write32(pItemInfo, Items[i].ItemNum);
			Write32(pItemInfo, (UINT)Items[i].AvailPeriod);
			Write8(pItemInfo, Items[i].Status);
			Write32(pItemInfo, Items[i].ObtainTime);
			Write32(pItemInfo, Items[i].OtherAttribute);
#ifndef ZingSpeed
			Write16(pItemInfo, Items[i].ItemType); // ItemType
#endif

			len = pItemInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}

	Write16(p, 0); // AttachLen
	Write16(p, 0); // AddKartNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //AddKartRefitInfo
		BYTE* pAddKartRefitInfo = p;
		Write16(pAddKartRefitInfo, 0); //len

		Write32(pAddKartRefitInfo, 0); //Uin
		Write32(pAddKartRefitInfo, 0); //KartId
		Write32(pAddKartRefitInfo, 0); //RefitCout
		Write16(pAddKartRefitInfo, 0); //MaxFlags
		Write16(pAddKartRefitInfo, 0); //WWeight
		Write16(pAddKartRefitInfo, 0); //SpeedWeight
		Write16(pAddKartRefitInfo, 0); //JetWeight
		Write16(pAddKartRefitInfo, 0); //SJetWeight
		Write16(pAddKartRefitInfo, 0); //AccuWeight
		Write32(pAddKartRefitInfo, 0); //ShapeRefitCount
		Write32(pAddKartRefitInfo, 0); //KartHeadRefitItemID
		Write32(pAddKartRefitInfo, 0); //KartTailRefitItemID
		Write32(pAddKartRefitInfo, 0); //KartFlankRefitItemID
		Write32(pAddKartRefitInfo, 0); //KartTireRefitItemID

		len = pAddKartRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // IncRedPacketNum

	len = p - buf;
	SendToClient(Client, 529, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestDeleteOneItem(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	int ItemID = Read32(Body);
	int ListItemSkipDelete[] = {10020};

	bool ItemSkipDelete = true;
	for (int i = 0; i < sizeof(ListItemSkipDelete) / sizeof(ListItemSkipDelete[0]); i++)
	{
		if (ListItemSkipDelete[i] == ItemID)
		{
			ItemSkipDelete = false;
			break;
		}
	}

	int ResultID = 0;
	const char *Reason = "";

	if (ItemSkipDelete == true)
	{
		PlayerDB_DeleteItem(Uin, ItemID);
	}
	else
	{
		ResultID = 1;
		Reason = "无法删除默认项目!";
	}

	ResponseDeleteOneItem(Client, ItemID, ResultID, Reason);
}
void ResponseDeleteOneItem(ClientNode *Client, int ItemID, int ResultID, const char *Reason)
{
	BYTE *buf = new BYTE[1024 * 8];
	BYTE *p = buf;
	size_t len;

	Write16(p, ResultID); // ResultID

	Write32(p, Client->Uin); // Uin
	Write32(p, ItemID);		 // ItemID

	len = strlen(Reason);
	Write8(p, (BYTE)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;

	len = p - buf;
	SendToClient(Client, 261, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyUseItemResult(ClientNode *Client, int ItemID, int UseItemNum, int CurrentItemNum)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin);	// Uin
	Write32(p, ItemID);			// ItemID
	Write32(p, UseItemNum);		// UseItemNum
	Write32(p, CurrentItemNum); // CurrentItemNum

	len = p - buf;
	SendToClient(Client, 556, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestSecondRefitKart(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	// 保存参数
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int KartItemID = Read32(p);
	int RefitItemID = Read32(p);
	int RefitItemIDMain = RefitItemID;
	short RefitTimes = Read16(p);
	UCHAR KartNameLen = Read8(p);

	// 开始处理
	const char *Reason = u8"Sửa đổi thất bại!";
	const char *SuccessReason = "改装成功!";
	const char *FailReason = "改装失败!";
	const char *MaxReason = "改装上限!";
	YAML::Node GameConfig = getGameConfig();
	INT RefitMax = GameConfig["SecRefitMax"].as<INT>();
	int RefitRate = GameConfig["SecRefitRate"].as<INT>();
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int RefitItemNum = 0;
	// GetItemNum
	if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, RefitItemID);

		if (sqlite3_step(stmt) == SQLITE_ROW)
			RefitItemNum = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (RefitItemNum >= RefitTimes)
	{
		short MaxFlags = 0;
		short WWeight = 0;
		short SpeedWeight = 0;
		short JetWeight = 0;
		short SJetWeight = 0;
		short AccuWeight = 0;
		short SecondRefitCount = 0;
		short Speed2Weight = 0;
		short DriftVecWeight = 0;
		short AdditionalZSpeedWeight = 0;
		short AntiCollisionWeight = 0;
		short LuckyValue = 0;
		short RefitLuckyValueMaxWeight = 0;
		short ShapeSuitID = 0;
		short LegendSuitLevel = 0;
		short LegendSuitLevelChoice = 0;
		short ShapeLegendSuitID = 0;
		for (size_t Refiti = 1; Refiti <= RefitTimes; Refiti++)
		{

			UINT Speed2WeightRefitCount = 0;
			UINT DriftVecWeightRefitCount = 0;
			UINT AdditionalZSpeedWeightRefitCount = 0;
			int randomID;
			std::random_device rd;
			std::mt19937 eng(rd());
			std::uniform_int_distribution<int> dis(1, 100);
			randomID = dis(eng);

			INT Temp = 0; // 当前改装值
			switch (RefitItemIDMain)
			{
			case 65025: // 普通推进
				Speed2Weight = 1;
				Speed2WeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT Speed2Weight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + Speed2Weight >= RefitMax)
				{
					Speed2Weight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					Speed2Weight = 0;
				}
				break;
			case 65026: // 精良推进
				Speed2Weight = 2;
				Speed2WeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT Speed2Weight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + Speed2Weight >= RefitMax)
				{
					Speed2Weight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					Speed2Weight = 0;
				}
				break;
			case 65027: // 极致推进
				Speed2Weight = 3;
				Speed2WeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT Speed2Weight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + Speed2Weight >= RefitMax)
				{
					Speed2Weight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					Speed2Weight = 0;
				}
				break;
			case 65028: // 普通离合
				DriftVecWeight = 1;
				DriftVecWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT DriftVecWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + DriftVecWeight >= RefitMax)
				{
					DriftVecWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					DriftVecWeight = 0;
				}
				break;
			case 65029: // 精良离合
				DriftVecWeight = 2;
				DriftVecWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT DriftVecWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + DriftVecWeight >= RefitMax)
				{
					DriftVecWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					DriftVecWeight = 0;
				}
				break;
			case 65030: // 极致离合
				DriftVecWeight = 3;
				DriftVecWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT DriftVecWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + DriftVecWeight >= RefitMax)
				{
					DriftVecWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					DriftVecWeight = 0;
				}
				break;
			case 65031: // 普通平衡
				AdditionalZSpeedWeight = 1;
				AdditionalZSpeedWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AdditionalZSpeedWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + AdditionalZSpeedWeight >= RefitMax)
				{
					AdditionalZSpeedWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					AdditionalZSpeedWeight = 0;
				}
				break;
			case 65032: // 精良平衡
				AdditionalZSpeedWeight = 2;
				AdditionalZSpeedWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AdditionalZSpeedWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + AdditionalZSpeedWeight >= RefitMax)
				{
					AdditionalZSpeedWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					AdditionalZSpeedWeight = 0;
				}
				break;
			case 65033: // 极致平衡
				AdditionalZSpeedWeight = 3;
				AdditionalZSpeedWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AdditionalZSpeedWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + AdditionalZSpeedWeight >= RefitMax)
				{
					AdditionalZSpeedWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					AdditionalZSpeedWeight = 0;
				}
				break;
			case 65034: // 推进-1
				Speed2Weight = -1;
				Speed2WeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT Speed2Weight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp==0) {
					Reason = "改装达到最低值!";
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			case 65035: // 离合-1
				DriftVecWeight = -1;
				DriftVecWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT DriftVecWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装达到最低值!";
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			case 65036: // 平衡-1
				AdditionalZSpeedWeight = -1;
				AdditionalZSpeedWeightRefitCount = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AdditionalZSpeedWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemID);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装达到最低值!";
					ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			}
			RefitItemNum--;
			if (RefitItemNum <= RefitTimes)
			{ // Delete Item
				if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, RefitItemID);
					sqlite3_step(stmt);
				}
			}
			else
			{
				if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, 1);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					sqlite3_bind_int(stmt, 3, RefitItemID);
					sqlite3_step(stmt);
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;

			NotifyUseItemResult(Client, RefitItemID, RefitTimes, RefitItemNum);

			sql = "UPDATE KartRefit SET SecondRefitCount=SecondRefitCount+?,Speed2Weight=Speed2Weight+?,DriftVecWeight=DriftVecWeight+?,AdditionalZSpeedWeight=AdditionalZSpeedWeight+?, Speed2WeightRefitCout = Speed2WeightRefitCout+? , DriftVecWeightRefitCout = DriftVecWeightRefitCout+?,AdditionalZSpeedWeightRefitCout = AdditionalZSpeedWeightRefitCout+? WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, 1);
				sqlite3_bind_int(stmt, 2, Speed2Weight);
				sqlite3_bind_int(stmt, 3, DriftVecWeight);
				sqlite3_bind_int(stmt, 4, AdditionalZSpeedWeight);

				
				sqlite3_bind_int(stmt, 5, Speed2WeightRefitCount);
				sqlite3_bind_int(stmt, 6, DriftVecWeightRefitCount);
				sqlite3_bind_int(stmt, 7, AdditionalZSpeedWeightRefitCount);


				sqlite3_bind_int(stmt, 8, Client->Uin);
				sqlite3_bind_int(stmt, 9, KartItemID);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseSecondRefitKart(Client, KartItemID, RefitItemID, Reason, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight, RefitTimes, Refiti);
		}
	}
}

void ResponseSecondRefitKart(ClientNode *Client, int KartItemId, int RefitItemID, const char *Reason, short inc_Speed2Weight, short inc_DriftVecWeight, short inc_AdditionalZSpeedWeight, short RefitTimes, int Idx)
{
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int RefitCout = 0;
	short MaxFlags = 0;
	short WWeight = 0;
	short SpeedWeight = 0;
	short JetWeight = 0;
	short SJetWeight = 0;
	short AccuWeight = 0;
	int ShapeRefitCount = 0;

	short SecondRefitCount = 0;
	short Speed2Weight = 0;
	short DriftVecWeight = 0;
	short AdditionalZSpeedWeight = 0;
	short AntiCollisionWeight = 0;
	short LuckyValue = 0;
	short RefitLuckyValueMaxWeight = 0;
	short ShapeSuitID = 0;
	short LegendSuitLevel = 0;
	short LegendSuitLevelChoice = 0;
	short ShapeLegendSuitID = 0;
	int KartHeadRefitItemID = 0;
	int KartTailRefitItemID = 0;
	int KartFlankRefitItemID = 0;
	int KartTireRefitItemID = 0;

	sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartItemId);
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
			SecondRefitCount = sqlite3_column_int(stmt, 12);
			Speed2Weight = sqlite3_column_int(stmt, 13);
			DriftVecWeight = sqlite3_column_int(stmt, 14);
			AdditionalZSpeedWeight = sqlite3_column_int(stmt, 15);
			AntiCollisionWeight = sqlite3_column_int(stmt, 16);
			LuckyValue = sqlite3_column_int(stmt, 17);
			RefitLuckyValueMaxWeight = sqlite3_column_int(stmt, 18);
			ShapeSuitID = sqlite3_column_int(stmt, 19);
			LegendSuitLevel = sqlite3_column_int(stmt, 20);
			LegendSuitLevelChoice = sqlite3_column_int(stmt, 21);
			ShapeLegendSuitID = sqlite3_column_int(stmt, 22);
		}
	}

	sqlite3_finalize(stmt);
	stmt = NULL;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, KartItemId);	 // KartItemID
	Write32(p, RefitItemID); // RefitItemID
	Write16(p, RefitTimes);	 // RefitTimes
	Write16(p, 1);			 // ResponseIdx
	Write8(p, 1);			 // LastIdx
	Write16(p, 0);			 // ResultID
	Write8(p, 0);			 // ResultID
	{						 // Increasement
		BYTE *pIncreasement = p;
		Write16(pIncreasement, 0); // len

		Write32(pIncreasement, Client->Uin);				// Uin
		Write32(pIncreasement, KartItemId);					// KartId
		Write32(pIncreasement, 0);							// RefitCout
		Write16(pIncreasement, 0);							// MaxFlags
		Write16(pIncreasement, 0);							// WWeight
		Write16(pIncreasement, 0);							// SpeedWeight
		Write16(pIncreasement, 0);							// JetWeight
		Write16(pIncreasement, 0);							// SJetWeight
		Write16(pIncreasement, 0);							// AccuWeight
		Write32(pIncreasement, 0);							// ShapeRefitCount
		Write32(pIncreasement, 0);							// KartHeadRefitItemID
		Write32(pIncreasement, 0);							// KartTailRefitItemID
		Write32(pIncreasement, 0);							// KartFlankRefitItemID
		Write32(pIncreasement, 0);							// KartTireRefitItemID
		Write32(pIncreasement, RefitTimes);					// SecondRefitCount
		Write16(pIncreasement, inc_Speed2Weight);			// Speed2Weight
		Write16(pIncreasement, inc_DriftVecWeight);			// DriftVecWeight
		Write16(pIncreasement, inc_AdditionalZSpeedWeight); // AdditionalZSpeedWeight
		Write16(pIncreasement, 0);							// AntiCollisionWeight
		Write16(pIncreasement, 0);							// LuckyValue
		Write16(pIncreasement, 0);							// RefitLuckyValueMaxWeight
		Write32(pIncreasement, 0);							// ShapeSuitID
		Write8(pIncreasement, 0);							// LegendSuitLevel
		Write32(pIncreasement, 0);							// LegendSuitLevelChoice
		Write32(pIncreasement, 0);							// ShapeLegendSuitID

		len = pIncreasement - p;
		Set16(p, (WORD)len);
		p += len;
	}
	MaxFlags = getMaxFlag(SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight);
	sql = "UPDATE KartRefit SET MaxFlags = ?  WHERE Uin=? AND KartID=?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{

		sqlite3_bind_int(stmt, 1, MaxFlags);

		sqlite3_bind_int(stmt, 2, Client->Uin);
		sqlite3_bind_int(stmt, 3, KartItemId);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	{ // CurAttr
		BYTE *pCurAttr = p;
		Write16(pCurAttr, 0); // len

		Write32(pCurAttr, Client->Uin);			   // Uin
		Write32(pCurAttr, KartItemId);			   // KartId
		Write32(pCurAttr, 0);					   // RefitCout
		Write16(pCurAttr, MaxFlags);					   // MaxFlags
		Write16(pCurAttr, 0);					   // WWeight
		Write16(pCurAttr, 0);					   // SpeedWeight
		Write16(pCurAttr, 0);					   // JetWeight
		Write16(pCurAttr, 0);					   // SJetWeight
		Write16(pCurAttr, 0);					   // AccuWeight
		Write32(pCurAttr, 0);					   // ShapeRefitCount
		Write32(pCurAttr, 0);					   // KartHeadRefitItemID
		Write32(pCurAttr, 0);					   // KartTailRefitItemID
		Write32(pCurAttr, 0);					   // KartFlankRefitItemID
		Write32(pCurAttr, 0);					   // KartTireRefitItemID
		Write32(pCurAttr, SecondRefitCount);	   // SecondRefitCount
		Write16(pCurAttr, Speed2Weight);		   // Speed2Weight
		Write16(pCurAttr, DriftVecWeight);		   // DriftVecWeight
		Write16(pCurAttr, AdditionalZSpeedWeight); // AdditionalZSpeedWeight
		Write16(pCurAttr, 0);					   // AntiCollisionWeight
		Write16(pCurAttr, 0);					   // LuckyValue
		Write16(pCurAttr, 0);					   // RefitLuckyValueMaxWeight
		Write32(pCurAttr, 0);					   // ShapeSuitID
		Write8(pCurAttr, 0);					   // LegendSuitLevel
		Write32(pCurAttr, 0);					   // LegendSuitLevelChoice
		Write32(pCurAttr, 0);					   // ShapeLegendSuitID

		len = pCurAttr - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // ExpAward
	Write32(p, 0); // RefitFragmentItemID
	Write32(p, 0); // RefitFragmentItemNum

	len = p - buf;
	SendToClient(Client, 21110, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestRefitKart(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	int KartItemId = Read32(Body);

	UCHAR KartNameLen = Read8(Body);
	Body += KartNameLen;

	int RefitItemId = Read32(Body);
	int RefitItemIdMain = RefitItemId;
	{//配件预处理
		//点火
		if (RefitItemId== 21925|| RefitItemId == 21926 || RefitItemId == 21927 || RefitItemId == 21940 || RefitItemId == 21941 || RefitItemId == 21942 || RefitItemId == 21955 || RefitItemId == 21956 || RefitItemId == 21957 || RefitItemId == 74844) {
			RefitItemIdMain = 12376;
		}
		//进气
		if (RefitItemId == 21928 || RefitItemId == 21929 || RefitItemId == 21930 || RefitItemId == 21943 || RefitItemId == 21944 || RefitItemId == 21945 || RefitItemId == 21958 || RefitItemId == 21959 || RefitItemId == 21960 || RefitItemId == 74845) {
			RefitItemIdMain = 12377;
		}
		//燃料
		if (RefitItemId == 21922 || RefitItemId == 21923 || RefitItemId == 21924 || RefitItemId == 21937 || RefitItemId == 21938 || RefitItemId == 21939 || RefitItemId == 21952 || RefitItemId == 21953 || RefitItemId == 21954 || RefitItemId == 74846) {
			RefitItemIdMain = 12378;
		}
		//引擎
		if (RefitItemId == 21919 || RefitItemId == 21920 || RefitItemId == 21921 || RefitItemId == 21934 || RefitItemId == 21935 || RefitItemId == 21936 || RefitItemId == 21949 || RefitItemId == 21950 || RefitItemId == 21951 || RefitItemId == 74848) {
			RefitItemIdMain = 12380;
		}
		//悬挂
		if (RefitItemId == 21916 || RefitItemId == 21917 || RefitItemId == 21918 || RefitItemId == 21931 || RefitItemId == 21932 || RefitItemId == 21933 || RefitItemId == 21946 || RefitItemId == 21947 || RefitItemId == 21948 || RefitItemId == 74847) {
			RefitItemIdMain = 12379;
		}
		//防护
		if (RefitItemId == 96600 || RefitItemId == 96601 || RefitItemId == 96602 || RefitItemId == 96603 || RefitItemId == 96604 || RefitItemId == 96605 || RefitItemId == 96606 || RefitItemId == 96607 || RefitItemId == 96608 || RefitItemId == 96609) {
			RefitItemIdMain = 96597;
		}
	}
	int Reserved = Read32(Body);
#ifndef ZingSpeed
	short RefitTimes = Read16(Body);
#endif
	// printf("[RequestRefitKart] KartItemId: %d, KartNameLen: %d, RefitItemId: %d, Reserved: %d\n", KartItemId, KartNameLen, RefitItemId, Reserved);

	const char *Reason = u8"Sửa đổi thất bại!";
	const char *SuccessReason = "改装成功!";
	const char *FailReason = "改装失败!";
	const char *MaxReason = "改装上限!";
	YAML::Node GameConfig = getGameConfig();
	INT RefitMax = GameConfig["RefitMax"].as<INT>();
	int RefitRate = GameConfig["RefitRate"].as<INT>();
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	int RefitItemNum = 0;
	// GetItemNum
	if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, RefitItemId);

		if (sqlite3_step(stmt) == SQLITE_ROW)
			RefitItemNum = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	// GetItemNum

	if (RefitItemNum >= RefitTimes)
	{

		short MaxFlags = 0;
		short WWeight = 0;
		short SpeedWeight = 0;
		short JetWeight = 0;
		short SJetWeight = 0;
		short AccuWeight = 0;
		short SecondRefitCount = 0;
		short Speed2Weight = 0;
		short DriftVecWeight = 0;
		short AdditionalZSpeedWeight = 0;
		short AntiCollisionWeight = 0;
		short LuckyValue = 0;
		short RefitLuckyValueMaxWeight = 0;
		short ShapeSuitID = 0;
		short LegendSuitLevel = 0;
		short LegendSuitLevelChoice = 0;
		short ShapeLegendSuitID = 0;

		
		for (size_t Refiti = 1; Refiti <= RefitTimes; Refiti++)
		{
			short SJetWeightRefitCout = 0;
			short AccuWeightRefitCout = 0;
			short JetWeightRefitCout = 0;
			short WWeightRefitCout = 0;
			short SpeedWeightRefitCout = 0;
			short AntiCollisionWeightRefitCout = 0;
			int randomID;
			std::random_device rd;
			std::mt19937 eng(rd());
			std::uniform_int_distribution<int> dis(1, 100);
			randomID = dis(eng);

			INT Temp = 0; // 当前改装值
			switch (RefitItemIdMain)
			{
			case 12376: // Đánh lửa+1 //点火装置+1
				SJetWeight = 1;
				SJetWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT SJetWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + SJetWeight >= RefitMax)
				{
					SJetWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					SJetWeight = 0;
				}
				break;
			case 12377: // Bơm khí+1 //进气系统+1
				AccuWeight = 1;
				AccuWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AccuWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + AccuWeight >= RefitMax)
				{
					AccuWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					AccuWeight = 0;
				}
				break;
			case 12378: // Nhiên liệu+1 //燃料系统+1
				JetWeight = 1;
				JetWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT JetWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + JetWeight >= RefitMax)
				{
					JetWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					JetWeight = 0;
				}
				break;
			case 12379: // Giảm Sóc+1 //悬挂系统+1
				WWeight = 1;
				WWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT WWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + WWeight >= RefitMax)
				{
					WWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					WWeight = 0;
				}
				break;
			case 12380: // Động cơ+1 //引擎装置+1
				SpeedWeight = 1;
				SpeedWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT SpeedWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + SpeedWeight >= RefitMax)
				{
					SpeedWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					SpeedWeight = 0;
				}
				break;
			case 12381: // Đánh lửa-1 点火-1
				SJetWeight = -1;
				SJetWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT SJetWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装值已经最低!";
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				
				break;
			case 12382: // Bơm khí-1
				AccuWeight = -1;
				AccuWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AccuWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装值已经最低!";
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			case 12383: // Nhiên liệu-1
				JetWeight = -1;
				JetWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT JetWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装值已经最低!";
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			case 12384: // Giảm Sóc-1
				WWeight = -1;
				WWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT WWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装值已经最低!";
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			case 12385: // Động cơ-1
				SpeedWeight = -1;
				SpeedWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT SpeedWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装值已经最低!";
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			case 12386: // Đánh lửa+2
				SJetWeight = 2;
				SJetWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT SJetWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + SJetWeight >= RefitMax)
				{
					SJetWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					SJetWeight = 0;
				}
				break;
			case 12387: // Bơm khí+2
				AccuWeight = 2;
				AccuWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AccuWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + AccuWeight >= RefitMax)
				{
					AccuWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					AccuWeight = 0;
				}
				break;
			case 12388: // Nhiên liệu+2
				JetWeight = 2;
				JetWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT JetWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + JetWeight >= RefitMax)
				{
					JetWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					JetWeight = 0;
				}
				break;
			case 12389: // Giảm Sóc+2
				WWeight = 2;
				WWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT WWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + WWeight >= RefitMax)
				{
					WWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					WWeight = 0;
				}
				break;
			case 12390: // Động cơ+2
				SpeedWeight = 2;
				SpeedWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT SpeedWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + SpeedWeight >= RefitMax)
				{
					SpeedWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					SpeedWeight = 0;
				}
				break;
			case 96597: // 防护装置+1
				AntiCollisionWeight = 1;
				AntiCollisionWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AntiCollisionWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + AntiCollisionWeight >= RefitMax)
				{
					AntiCollisionWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					AntiCollisionWeight = 0;
				}
				break;
			case 96598: // 防护装置-1
				AntiCollisionWeight = -1;
				AntiCollisionWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AntiCollisionWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp == 0) {
					Reason = "改装值已经最低!";
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else {
					Reason = "Success!";
				}
				break;
			case 96599: // 防护装置+2
				AntiCollisionWeight = 2;
				AntiCollisionWeightRefitCout = 1;
				if (sqlite3_prepare_v2(PlayerDB, "SELECT AntiCollisionWeight from KartRefit WHERE Uin=? AND KartID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KartItemId);
					if (sqlite3_step(stmt) == SQLITE_ROW)
						Temp = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				if (Temp >= RefitMax)
				{
					Reason = MaxReason;
					ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
					return;
				}
				else if (Temp + AntiCollisionWeight >= RefitMax)
				{
					AntiCollisionWeight = RefitMax - Temp;
				}
				if (randomID <= RefitRate)
				{
					Reason = SuccessReason;
				}
				else
				{
					Reason = FailReason;
					AntiCollisionWeight = 0;
				}
				break;
			}
			RefitItemNum--;
			if (RefitItemNum <= RefitTimes)
			{ // Delete Item
				if (RefitItemId == 10020)
				{
					return;
				}
				if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, RefitItemId);
					sqlite3_step(stmt);
				}
			}
			else
			{
				if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, 1);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					sqlite3_bind_int(stmt, 3, RefitItemId);
					sqlite3_step(stmt);
				}
			}
			sqlite3_finalize(stmt);
			stmt = NULL;

			NotifyUseItemResult(Client, RefitItemId, 1, RefitItemNum);

			sql = "UPDATE KartRefit SET RefitCout=RefitCout+1, SJetWeight=SJetWeight+?,AccuWeight=AccuWeight+?,JetWeight=JetWeight+?,WWeight=WWeight+?,SpeedWeight=SpeedWeight+?,AntiCollisionWeight=AntiCollisionWeight+?,SJetWeightRefitCout = SJetWeightRefitCout + ? , AccuWeightRefitCout = AccuWeightRefitCout + ? , JetWeightRefitCout = JetWeightRefitCout + ? , WWeightRefitCout = WWeightRefitCout + ? , SpeedWeightRefitCout = SpeedWeightRefitCout + ? , AntiCollisionWeightRefitCout = AntiCollisionWeightRefitCout + ? WHERE Uin=? AND KartID=?";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				
				sqlite3_bind_int(stmt, 1, SJetWeight);
				sqlite3_bind_int(stmt, 2, AccuWeight);
				sqlite3_bind_int(stmt, 3, JetWeight);
				sqlite3_bind_int(stmt, 4, WWeight);
				sqlite3_bind_int(stmt, 5, SpeedWeight);
				sqlite3_bind_int(stmt, 6, AntiCollisionWeight);

				sqlite3_bind_int(stmt, 7, SJetWeightRefitCout);
				sqlite3_bind_int(stmt, 8, AccuWeightRefitCout);
				sqlite3_bind_int(stmt, 9, JetWeightRefitCout);
				sqlite3_bind_int(stmt, 10, WWeightRefitCout);
				sqlite3_bind_int(stmt, 11, SpeedWeightRefitCout);
				sqlite3_bind_int(stmt, 12, AntiCollisionWeightRefitCout);

				sqlite3_bind_int(stmt, 13, Client->Uin);
				sqlite3_bind_int(stmt, 14, KartItemId);
				result = sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseRefitKart(Client, KartItemId, Reason, SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, RefitTimes, Refiti);
		}
	}
}
void ResponseRefitKart(ClientNode *Client, int KartItemId, const char *Reason, short inc_SJetWeight, short inc_AccuWeight, short inc_JetWeight, short inc_WWeight, short inc_SpeedWeight, short inc_AntiCollisionWeight, short RefitTimes, int Idx)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0);			 // Result
	Write32(p, Client->Uin); // Uin
	Write32(p, KartItemId);	 // KartItemId

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int RefitCout = 0;
	short MaxFlags = 0;
	short WWeight = 0;
	short SpeedWeight = 0;
	short JetWeight = 0;
	short SJetWeight = 0;
	short AccuWeight = 0;
	int ShapeRefitCount = 0;

	short SecondRefitCount = 0;
	short Speed2Weight = 0;
	short DriftVecWeight = 0;
	short AdditionalZSpeedWeight = 0;
	short AntiCollisionWeight = 0;
	short LuckyValue = 0;
	short RefitLuckyValueMaxWeight = 0;
	short ShapeSuitID = 0;
	short LegendSuitLevel = 0;
	short LegendSuitLevelChoice = 0;
	short ShapeLegendSuitID = 0;
	int KartHeadRefitItemID = 0;
	int KartTailRefitItemID = 0;
	int KartFlankRefitItemID = 0;
	int KartTireRefitItemID = 0;

	sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartItemId);
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
			SecondRefitCount = sqlite3_column_int(stmt, 12);
			Speed2Weight = sqlite3_column_int(stmt, 13);
			DriftVecWeight = sqlite3_column_int(stmt, 14);
			AdditionalZSpeedWeight = sqlite3_column_int(stmt, 15);
			AntiCollisionWeight = sqlite3_column_int(stmt, 16);
			LuckyValue = sqlite3_column_int(stmt, 17);
			RefitLuckyValueMaxWeight = sqlite3_column_int(stmt, 18);
			ShapeSuitID = sqlite3_column_int(stmt, 19);
			LegendSuitLevel = sqlite3_column_int(stmt, 20);
			LegendSuitLevelChoice = sqlite3_column_int(stmt, 21);
			ShapeLegendSuitID = sqlite3_column_int(stmt, 22);
			Client->OrgKartId = KartItemId;
			Client->SkinID = ShapeSuitID;
		}
	}

	sqlite3_finalize(stmt);
	stmt = NULL;
	
	{ // Increasement

		BYTE *pIncreasement = p;
		Write16(pIncreasement, 0); // len

		Write32(pIncreasement, Client->Uin);	 // Uin
		Write32(pIncreasement, KartItemId);		 // KartId
		Write32(pIncreasement, RefitTimes);		 // RefitCout
		Write16(pIncreasement, MaxFlags);				 // MaxFlags
		Write16(pIncreasement, inc_WWeight);	 // WWeight
		Write16(pIncreasement, inc_SpeedWeight); // SpeedWeight
		Write16(pIncreasement, inc_JetWeight);	 // JetWeight
		Write16(pIncreasement, inc_SJetWeight);	 // SJetWeight
		Write16(pIncreasement, inc_AccuWeight);	 // AccuWeight
		Write32(pIncreasement, 0);				 // ShapeRefitCount
		Write32(pIncreasement, 0);				 // KartHeadRefitItemID
		Write32(pIncreasement, 0);				 // KartTailRefitItemID
		Write32(pIncreasement, 0);				 // KartFlankRefitItemID
		Write32(pIncreasement, 0);				 // KartTireRefitItemID
		// Write32(pIncreasement, SecondRefitCount); //SecondRefitCount
		// Write16(pIncreasement, Speed2Weight); //Speed2Weight
		// Write16(pIncreasement, DriftVecWeight); //DriftVecWeight
		// Write16(pIncreasement, AdditionalZSpeedWeight); //AdditionalZSpeedWeight
		// Write16(pIncreasement, AntiCollisionWeight); //AntiCollisionWeight

		Write32(pIncreasement, SecondRefitCount);		 // SecondRefitCount
		Write16(pIncreasement, Speed2Weight);			 // Speed2Weight
		Write16(pIncreasement, DriftVecWeight);			 // DriftVecWeight
		Write16(pIncreasement, AdditionalZSpeedWeight);	 // AdditionalZSpeedWeight
		Write16(pIncreasement, inc_AntiCollisionWeight); // AntiCollisionWeight
		Write16(pIncreasement, 0);						 // LuckyValue
		Write16(pIncreasement, 0);						 // RefitLuckyValueMaxWeight
		Write32(pIncreasement, ShapeSuitID);			 // ShapeSuitID
		Write8(pIncreasement, LegendSuitLevel);			 // LegendSuitLevel
		Write32(pIncreasement, LegendSuitLevelChoice);	 // LegendSuitLevelChoice
		Write32(pIncreasement, ShapeLegendSuitID);		 // ShapeLegendSuitID

		len = pIncreasement - p;
		Set16(p, (WORD)len);
		p += len;
	}
	MaxFlags = getMaxFlag(SJetWeight, AccuWeight, JetWeight, WWeight, SpeedWeight, AntiCollisionWeight, Speed2Weight, DriftVecWeight, AdditionalZSpeedWeight);
	sql = "UPDATE KartRefit SET MaxFlags = ?  WHERE Uin=? AND KartID=?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{

		sqlite3_bind_int(stmt, 1, MaxFlags);

		sqlite3_bind_int(stmt, 2, Client->Uin);
		sqlite3_bind_int(stmt, 3, KartItemId);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	{ // CurAttr
		BYTE *pCurAttr = p;
		Write16(pCurAttr, 0); // len

		Write32(pCurAttr, Client->Uin);			 // Uin
		Write32(pCurAttr, KartItemId);			 // KartId
		Write32(pCurAttr, RefitCout);			 // RefitCout
		Write16(pCurAttr, MaxFlags);			 // MaxFlags
		Write16(pCurAttr, WWeight);				 // WWeight
		Write16(pCurAttr, SpeedWeight);			 // SpeedWeight
		Write16(pCurAttr, JetWeight);			 // JetWeight
		Write16(pCurAttr, SJetWeight);			 // SJetWeight
		Write16(pCurAttr, AccuWeight);			 // AccuWeight
		Write32(pCurAttr, ShapeRefitCount);		 // ShapeRefitCount
		Write32(pCurAttr, KartHeadRefitItemID);	 // KartHeadRefitItemID
		Write32(pCurAttr, KartTailRefitItemID);	 // KartTailRefitItemID
		Write32(pCurAttr, KartFlankRefitItemID); // KartFlankRefitItemID
		Write32(pCurAttr, KartTireRefitItemID);	 // KartTireRefitItemID
		// Write32(pCurAttr, SecondRefitCount); //SecondRefitCount
		// Write16(pCurAttr, Speed2Weight); //Speed2Weight
		// Write16(pCurAttr, DriftVecWeight); //DriftVecWeight
		// Write16(pCurAttr, AdditionalZSpeedWeight); //AdditionalZSpeedWeight
		// Write16(pCurAttr, AntiCollisionWeight); //AntiCollisionWeight

		Write32(pCurAttr, SecondRefitCount);	   // SecondRefitCount
		Write16(pCurAttr, Speed2Weight);		   // Speed2Weight
		Write16(pCurAttr, DriftVecWeight);		   // DriftVecWeight
		Write16(pCurAttr, AdditionalZSpeedWeight); // AdditionalZSpeedWeight
		Write16(pCurAttr, AntiCollisionWeight);	   // AntiCollisionWeight
		Write16(pCurAttr, 0);					   // LuckyValue
		Write16(pCurAttr, 0);					   // RefitLuckyValueMaxWeight
		Write32(pCurAttr, ShapeSuitID);			   // ShapeSuitID
		Write8(pCurAttr, LegendSuitLevel);		   // LegendSuitLevel
		Write32(pCurAttr, LegendSuitLevelChoice);  // LegendSuitLevelChoice
		Write32(pCurAttr, ShapeLegendSuitID);	   // ShapeLegendSuitID

		len = pCurAttr - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 500); // ExpAward
	Write32(p, 250); // MoneyAward

	len = strlen(Reason);
	Write16(p, (WORD)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;

#ifndef ZingSpeed
	Write16(p, RefitTimes); // RefitTimes
	Write16(p, Idx);		// ResponseIdx
	Write8(p, Idx);			// LastIdx
	Write32(p, 0);			// LuckyValue
#endif

	len = p - buf;
	SendToClient(Client, 174, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestModifyPlayerSignature(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	char Signature[MaxSignature] = {};
	memcpy(Signature, p, MaxSignature);
	p += MaxSignature;

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	sql = "UPDATE BaseInfo SET Signature=? WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_text(stmt, 1, Signature, strnlen_s(Signature, MaxSignature), SQLITE_TRANSIENT);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	ResponseModifyPlayerSignature(Client, Signature);
}

void ResponseModifyPlayerSignature(ClientNode *Client, char *Signature)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write8(p, 0);  // ReasonLen

	// Signature[]
	memcpy(p, Signature, MaxSignature);
	p += MaxSignature;

	len = p - buf;
	SendToClient(Client, 376, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestFindPlayerByQQ(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	char NickName[MaxNickName] = {};
	memcpy(NickName, p, MaxNickName);
	p += MaxNickName;

	UINT DstUin = Read32(p);
	UINT DstNobleID = Read32(p);
	UCHAR Contex = Read8(p);
#ifndef ZingSpeed
	UCHAR AreaIdDst = Read8(p);
	UCHAR IgnoreByQQ2Msg = Read8(p);
	UCHAR SimpleInfo = Read8(p);
#endif
	// ResponseChangeIdentity(Client);
	ResponseFindPlayerByQQ(Client, DstUin, AreaIdDst, SimpleInfo);
}
UINT WriteEquipedItem2(BYTE *&p, UINT Uin, UINT *cPetId)
{ // EquipedItem
	WORD len;

	BYTE *pItemNum = p;
	Write16(p, 0); // EquipedItemNum

	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;
	int result;
	size_t i = 0;
	UINT KartItemId = 0;
	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			UINT ItemID = sqlite3_column_int(stmt, 0);
			BYTE Status = sqlite3_column_int(stmt, 3);

			if (Status)
			{ // PlayerItemInfo
				i++;
				unsigned int ItemType = GetItemType(ItemID);
				if (ItemType == EAIT_CAR)
				{
					KartItemId = ItemID;
				}
				else if (ItemType == EAIT_PET)
				{
					*cPetId = ItemID;
				}

				BYTE *pItemInfo = p;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, ItemID);
				Write32(pItemInfo, sqlite3_column_int(stmt, 1));
				Write32(pItemInfo, sqlite3_column_int(stmt, 2));
				Write8(pItemInfo, Status);
				Write32(pItemInfo, sqlite3_column_int(stmt, 4));
				Write32(pItemInfo, sqlite3_column_int(stmt, 5));
#ifndef ZingSpeed
				Write16(pItemInfo, 0); // ItemType
#endif
				len = pItemInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Set16(pItemNum, (WORD)i);

	//{
	//	BYTE* pItemInfo = p;
	//	Write16(pItemInfo, 0); //len

	//	Write32(pItemInfo, 127758);
	//	Write32(pItemInfo, 1);
	//	Write32(pItemInfo, -1);
	//	Write8(pItemInfo, 1);
	//	Write32(pItemInfo, 1678474256);
	//	Write32(pItemInfo, 0);
	//	Write16(pItemInfo, 0); //ItemType
	//	len = pItemInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}

	return KartItemId;
}
UINT WriteEquipedItem3(BYTE *&p, UINT Uin, UINT *cPetId)
{ // EquipedItem
	WORD len;

	BYTE *pItemNum = p;
	Write16(p, 0); // EquipedItemNum

	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;
	int result;
	size_t i = 0;
	UINT KartItemId = 0;
	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			UINT ItemID = sqlite3_column_int(stmt, 0);
			BYTE Status = sqlite3_column_int(stmt, 3);

			if (Status)
			{ // PlayerItemInfo
				i++;
				unsigned int ItemType = GetItemType(ItemID);
				if (GetItemType(ItemID) == EAIT_CAR)
				{ // 废弃的代码

					ClientNode *uin = GetClient(Uin);
					if (uin)
					{
						ResponseSaveShapeRefit4(uin, uin->KartID);
					}
					KartItemId = ItemID;
				}
				else if (ItemType == EAIT_PET)
				{
					*cPetId = ItemID;
				}

				BYTE *pItemInfo = p;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, ItemID);
				Write32(pItemInfo, sqlite3_column_int(stmt, 1));
				Write32(pItemInfo, sqlite3_column_int(stmt, 2));
				Write8(pItemInfo, Status);
				Write32(pItemInfo, sqlite3_column_int(stmt, 4));
				Write32(pItemInfo, sqlite3_column_int(stmt, 5));
				Write16(pItemInfo, 0); // ItemType

				len = pItemInfo - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	Set16(pItemNum, (WORD)i);

	return KartItemId;
}
void ResponseFindPlayerByQQ(ClientNode *Client, UINT DstUin, UCHAR AreaIdDst, UCHAR SimpleInfo)
{
	ClientNode* Dst = GetClient(DstUin);
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int GuildVipLevel = 0;
	int GuildVipPoint = 0;
	int GuildVipLeftDays = 0;
	int LoverVipLevel = 0;
	int LoverVipPoint = 0;
	int LoverVipLeftDays = 0;
	int GardenLevel = 0;
	int GardenPoint = 0;
	int GardenExpireTime = 0;
	int VipLevel = 0;
	int CharmValueOfMonth = 0;
	int VipFlag = 0;
	int VipGrowRate = 0;
	int EmperorLevel = 0;
	int EmperorPoint = 0;
	int EmperorLeftDays = 0;
	int EmperorGrowRate = 0;
	int NobleLevel = 0;
	int NoblePoint = 0;
	int NobleLeftDays = 0;
	int AppellationType = 1;
	int NobleID = 0;

	sql = "SELECT GuildVipLevel,GuildVipPoint,GuildVipLeftDays,LoverVipLevel,LoverVipPoint,LoverVipLeftDays,GardenLevel,GardenPoint,GardenExpireTime,VipLevel,CharmValueOfMonth,VipFlag,VipGrowRate,EmperorLevel,EmperorPoint,EmperorLeftDays,EmperorGrowRate,NobleLevel,NoblePoint,NobleLeftDays,AppellationType,NobleID  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, DstUin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			GuildVipLevel = sqlite3_column_int(stmt, 0);
			GuildVipPoint = sqlite3_column_int(stmt, 1);
			GuildVipLeftDays = sqlite3_column_int(stmt, 2);
			LoverVipLevel = sqlite3_column_int(stmt, 3);
			LoverVipPoint = sqlite3_column_int(stmt, 4);
			LoverVipLeftDays = sqlite3_column_int(stmt, 5);
			GardenLevel = sqlite3_column_int(stmt, 6);
			GardenPoint = sqlite3_column_int(stmt, 7);
			GardenExpireTime = sqlite3_column_int(stmt, 8);
			VipLevel = sqlite3_column_int(stmt, 9);
			CharmValueOfMonth = sqlite3_column_int(stmt, 10);
			VipFlag = sqlite3_column_int(stmt, 11);
			VipGrowRate = sqlite3_column_int(stmt, 12);
			EmperorLevel = sqlite3_column_int(stmt, 13);
			EmperorPoint = sqlite3_column_int(stmt, 14);
			EmperorLeftDays = sqlite3_column_int(stmt, 15);
			EmperorGrowRate = sqlite3_column_int(stmt, 16);
			NobleLevel = sqlite3_column_int(stmt, 17);
			NoblePoint = sqlite3_column_int(stmt, 18);
			NobleLeftDays = sqlite3_column_int(stmt, 19);
			NobleID = sqlite3_column_int(stmt, 21);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	{//贵族等级优化
		if (VipFlag > 7) {
			VipFlag = 7;
		}
		if (VipLevel > 9) {
			VipLevel = 9;
		}
		if (NobleLevel > 7) {
			NobleLevel = 7;
		}
	}
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT KartItemId = 0;
	int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_QQLEVEL4 | ID_IDENTIFY_QQLEVEL5 | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG;
	Identity |= ID_IDENTIFY_SPEEDMEMBER;
	int IsInTopList = 1;
	UINT PetId = 0;
	Write16(p, 0); // ResultID
	{			   // PlayerInfo
		BYTE *pPlayerInfo = p;
		Write16(pPlayerInfo, 0); // len

		Write32(pPlayerInfo, DstUin);	// Uin
		Write32(pPlayerInfo, Identity); // Identity
		if (GetClient(DstUin))
		{
			Write16(pPlayerInfo, GetClient(DstUin)->ConnID); // PlayerID
		}
		else
		{
			Write16(pPlayerInfo, 0); // PlayerID
		}

		WritePlayerDBBaseInfo(pPlayerInfo, DstUin);
		WritePlayerGuildInfo(pPlayerInfo, DstUin);
		KartItemId = WriteEquipedItem2(pPlayerInfo, DstUin, &PetId);

		Write32(pPlayerInfo, 300); // MasterPoint
		Write32(pPlayerInfo, 301); // TotalGuildProsperity

		Write16(pPlayerInfo, VipFlag);	   // VipFlag
		Write16(pPlayerInfo, VipGrowRate); // VipGrowRate

		Write8(pPlayerInfo, 1); // AppellationNum

		for (size_t i = 0; i < 1; i++)
		{ // Record
			BYTE *pRecord = pPlayerInfo;
			Write16(pRecord, 0); // len

			Write8(pRecord, AppellationType);					   // Type
			Write8(pRecord, GetAppellationLevel(AppellationType)); // Level
			Write8(pRecord, 1);									   // Status
			Write8(pRecord, 0);									   // Difficulty
			Write32(pRecord, 0);								   // MapId
			Write32(pRecord, 0);								   // Value

			len = pRecord - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		{ // NobleInfo
			BYTE *pNobleInfo = pPlayerInfo;
			Write16(pNobleInfo, 0); // len

			Write32(pNobleInfo, NobleID);		// NobleID
			Write8(pNobleInfo, NobleLevel);		// NobleLevel
			Write32(pNobleInfo, NoblePoint);	// NoblePoint
			Write32(pNobleInfo, NobleLeftDays); // NobleLeftDays

			len = pNobleInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, 0); // HasCarryWizard
		/*
		{ //CarryWizardInfo
			BYTE* pCarryWizardInfo = pPlayerInfo;
			Write16(pCarryWizardInfo, 0); //len

			Write16(pCarryWizardInfo, 0); //WizardID
			WriteString(pCarryWizardInfo, 0); //NickName[]
			Write16(pCarryWizardInfo, 0); //WizardType
			Write8(pCarryWizardInfo, 0); //Order
			Write8(pCarryWizardInfo, 0); //EvolutionBranch
			Write8(pCarryWizardInfo, 0); //IsHuanHua

			len = pCarryWizardInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

#ifndef ZingSpeed
		{ // GuildVipBaseInfo
			BYTE *pGuildVipBaseInfo = pPlayerInfo;
			Write16(pGuildVipBaseInfo, 0); // len

			Write8(pGuildVipBaseInfo, GuildVipLevel);  // GuildVipLevel
			Write32(pGuildVipBaseInfo, GuildVipPoint); // GuildVipPoint

			len = pGuildVipBaseInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
#endif

		Write8(pPlayerInfo, 0); // HasLDMInfo
		/*
		for (size_t i = 0; i < n; i++)
		{ //LDMInfo
			BYTE* pLDMInfo = pPlayerInfo;
			Write16(pLDMInfo, 0); //len

			Write8(pLDMInfo, 0); //IsInCurSeason
			Write32(pLDMInfo, 0); //Score
			Write32(pLDMInfo, 0); //MaxScore
			Write8(pLDMInfo, 0); //Grade
			Write8(pLDMInfo, 0); //MaxGrade
			Write32(pLDMInfo, 0); //WinStreakCount
			Write32(pLDMInfo, 0); //MatchCount
			Write32(pLDMInfo, 0); //CollisionCount
			Write32(pLDMInfo, 0); //BeatCount

			len = pLDMInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

#ifndef ZingSpeed
		Write8(pPlayerInfo, 1); // HasLoverVip

		for (size_t i = 0; i < 1; i++)
		{ // LoverVipInfo
			BYTE *pLoverVipInfo = pPlayerInfo;
			Write16(pLoverVipInfo, 0); // len

			Write8(pLoverVipInfo, LoverVipLevel);  // LoverVipLevel
			Write32(pLoverVipInfo, LoverVipPoint); // LoverVipPoint
			Write8(pLoverVipInfo, 9999);			   // GrowRate

			len = pLoverVipInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

#endif

		Write8(pPlayerInfo, 0); // IsShowMounts

#ifndef ZingSpeed
		Write8(pPlayerInfo, 1); // HasGarden

		{ // PersonalGardenBaseInfo
			BYTE *pPersonalGardenBaseInfo = pPlayerInfo;
			Write16(pPersonalGardenBaseInfo, 0); // len

			Write8(pPersonalGardenBaseInfo, GardenLevel);		// GardenLevel
			Write32(pPersonalGardenBaseInfo, GardenPoint);		// GardenPoint
			Write8(pPersonalGardenBaseInfo, 0);					// GrowRate
			Write32(pPersonalGardenBaseInfo, GardenExpireTime); // GardenExpireTime
			{													// SimpleInfo
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

			len = pPersonalGardenBaseInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, VipLevel);	// isConsumeVip
		Write32(pPlayerInfo, VipLevel); // ConsumeVipLevel
		Write32(pPlayerInfo, 10);		// SearchTreasureNums
		Write32(pPlayerInfo, 10);		// GetTreasureNums
		Write32(pPlayerInfo, 99);		// ConsumeVipCharmVlaueOfMonth
		{								// EmperorInfo
			BYTE *pEmperorInfo = pPlayerInfo;
			Write16(pEmperorInfo, 0); // len

			Write8(pEmperorInfo, EmperorLevel);		// EmperorLevel
			Write32(pEmperorInfo, EmperorPoint);	// EmperorPoint
			Write32(pEmperorInfo, EmperorLeftDays); // EmperorLeftDays
			Write8(pEmperorInfo, EmperorGrowRate);	// EmperorGrowRate

			len = pEmperorInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		{ // EmperorOtherInfo
			BYTE *pEmperorOtherInfo = pPlayerInfo;
			Write16(pEmperorOtherInfo, 0); // len

			Write32(pEmperorOtherInfo, 3576362056); // ExpiredTime
			Write8(pEmperorOtherInfo, 1);			// ShowExpireTips

			len = pEmperorOtherInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		{ // ActivityInfo
			BYTE *pActivityInfo = pPlayerInfo;
			Write16(pActivityInfo, 0); // len

			Write32(pActivityInfo, 0); // TotalActivity
			Write32(pActivityInfo, 0); // ActivityLevel

			len = pActivityInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, 1); // HaveWakedKartAttributeAddInfo

		{ // WakeKartAttributeAddInfo
			BYTE *pWakeKartAttributeAddInfo = pPlayerInfo;
			Write16(pWakeKartAttributeAddInfo, 0); // len

			Write16(pWakeKartAttributeAddInfo, 0); // AccAddValue
			Write16(pWakeKartAttributeAddInfo, 0); // SpeedAddValue
			Write16(pWakeKartAttributeAddInfo, 0); // JetAddValue
			Write16(pWakeKartAttributeAddInfo, 0); // SJetAddValue

			len = pWakeKartAttributeAddInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, 0); // HaveLoverBabyInfo
		//{						// FindLoverBabyInfo
		//	BYTE *pFindLoverBabyInfo = pPlayerInfo;
		//	Write16(pFindLoverBabyInfo, 0); // len

		//	Write32(pFindLoverBabyInfo, 3);		  // BabyStatus
		//	Write32(pFindLoverBabyInfo, 71554);	  // LoverBabyItemID
		//	Write32(pFindLoverBabyInfo, 1161146); // LoverBabyID
		//	Write32(pFindLoverBabyInfo, 62);	  // BabyGrowLevel
		//	Write32(pFindLoverBabyInfo, 0);		  // BabyStrengthLevel
		//	Write16(pFindLoverBabyInfo, 0);		  // LoverBabyStat
		//	Write16(pFindLoverBabyInfo, 1);		  // LoverBabyEquipStat
		//	Write16(pFindLoverBabyInfo, 0);		  // LoverBabyTransferdStatus

		//	len = pFindLoverBabyInfo - pPlayerInfo;
		//	Set16(pPlayerInfo, (WORD)len);
		//	pPlayerInfo += len;
		//}

		{ // GansterScoreInfo
			BYTE *pGansterScoreInfo = pPlayerInfo;
			Write16(pGansterScoreInfo, 0); // len

			Write32(pGansterScoreInfo, 0); // GansterSeasonID
			Write32(pGansterScoreInfo, 0); // GansterScore
			Write32(pGansterScoreInfo, 0); // PoliceScore
			Write32(pGansterScoreInfo, 0); // TotalGansterScore

			len = pGansterScoreInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write32(pPlayerInfo, 0); // OlympicId
		Write32(pPlayerInfo, 0); // LastOlympicUpdateTime
		Write32(pPlayerInfo, 0); // NPCEliminateWinTimes
		{						 // BorderInfo
			BYTE *pBorderInfo = pPlayerInfo;
			Write16(pBorderInfo, 0); // len

			Write32(pBorderInfo, 0); // SeasonID
			Write32(pBorderInfo, 0); // Zhanxun
			Write32(pBorderInfo, 0); // SeasonZhanxun

			len = pBorderInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		Write8(pPlayerInfo, 0);	   // SpecialActivityID
		Write32(pPlayerInfo, 100); // ThemeHouseCollectValue
		Write8(pPlayerInfo, 0);	   // HaveSecondLoverBabyInfo
		{ //SecondLoverBabyInfo
			BYTE* pSecondLoverBabyInfo = pPlayerInfo;
			Write16(pSecondLoverBabyInfo, 0); //len

			Write32(pSecondLoverBabyInfo, 0); //BabyStatus
			Write32(pSecondLoverBabyInfo, 0); //LoverBabyItemID
			Write32(pSecondLoverBabyInfo, 0); //LoverBabyID
			Write32(pSecondLoverBabyInfo, 0); //BabyGrowLevel
			Write32(pSecondLoverBabyInfo, 0); //BabyStrengthLevel
			Write16(pSecondLoverBabyInfo, 0); //LoverBabyStat
			Write16(pSecondLoverBabyInfo, 0); //LoverBabyEquipStat
			Write16(pSecondLoverBabyInfo, 0); //LoverBabyTransferdStatus

			len = pSecondLoverBabyInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		Write8(pPlayerInfo, 0);	 // EquipBaby
		Write8(pPlayerInfo, 0);	 // ActiveStatus
		Write32(pPlayerInfo, 0); // HelperLev

		Write8(pPlayerInfo, 0); // HasRankedMatchInfo
		/*
		{ //RankedMatchGradeInfo
			BYTE* pRankedMatchGradeInfo = pPlayerInfo;
			Write16(pRankedMatchGradeInfo, 0); //len

			Write32(pRankedMatchGradeInfo, 0); //GradeLevel
			Write32(pRankedMatchGradeInfo, 0); //ChildLevel
			Write32(pRankedMatchGradeInfo, 0); //GradeScore
			Write32(pRankedMatchGradeInfo, 0); //MaxGradeScore
			Write32(pRankedMatchGradeInfo, 0); //TotalScore

			len = pRankedMatchGradeInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write16(pPlayerInfo, 0); // PlayerSeasonInfoNums
		/*
		for (size_t i = 0; i < n; i++)
		{ //PlayerSeasonInfos
			BYTE* pPlayerSeasonInfos = pPlayerInfo;
			Write16(pPlayerSeasonInfos, 0); //len

			Write16(pPlayerSeasonInfos, 0); //SeasonID
			Write16(pPlayerSeasonInfos, 0); //GradeLevel
			Write8(pPlayerSeasonInfos, 0); //ChildLevel
			Write16(pPlayerSeasonInfos, 0); //HighGradeLevel
			Write8(pPlayerSeasonInfos, 0); //HighChildLevel
			Write8(pPlayerSeasonInfos, 0); //SeasonInfoProcesedTag
			Write8(pPlayerSeasonInfos, 0); //SeasonComputeInfoTipTag
			Write16(pPlayerSeasonInfos, 0); //SeasonInheritTag
			Write8(pPlayerSeasonInfos, 0); //SeasonInheritTipTag
			Write16(pPlayerSeasonInfos, 0); //SpeedGodNums
			Write32(pPlayerSeasonInfos, 0); //SpeedRoundNums
			Write32(pPlayerSeasonInfos, 0); //SpeedFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //TotalRankIndex
			Write32(pPlayerSeasonInfos, 0); //ContiFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //SpeedKillAllRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //ContiFinishPropRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropKillAllRoundNums
			Write8(pPlayerSeasonInfos, 0); //LastRoundIndex
			Write32(pPlayerSeasonInfos, 0); //OpenDate
			Write32(pPlayerSeasonInfos, 0); //CloseDate
			Write32(pPlayerSeasonInfos, 0); //HistroyContFinishRoundNums

			len = pPlayerSeasonInfos - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0);	 // IsHelperAppOnline
		Write32(pPlayerInfo, 0); // RankedMatchSpeedGodNums
		Write8(pPlayerInfo, 0);	 // EquipSealType

		Write8(pPlayerInfo, 0); // HasHuanLingChangeInfo
		/*
		{ //HuanLingSuitInfo
			BYTE* pHuanLingSuitInfo = pPlayerInfo;
			Write16(pHuanLingSuitInfo, 0); //len

			Write32(pHuanLingSuitInfo, 0); //SuitID
			Write8(pHuanLingSuitInfo, 0); //ChangeLevel

			len = pHuanLingSuitInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); // SpecialActivityIDNum
		Write8(pPlayerInfo, 0); // PersonalPanelSelectRankedMatchFrameTag

		Write8(pPlayerInfo, 0); // SpecialActivityInfoNum
		
		for (size_t i = 0; i < 0; i++)
		{ //SpecialActivityInfo
			BYTE* pSpecialActivityInfo = pPlayerInfo;
			Write16(pSpecialActivityInfo, 0); //len

			Write8(pSpecialActivityInfo, 1); //ProfessionLicenseInfoNum
			for (size_t i = 0; i < 1; i++)
			{ //ProfessionLicenseInfo
				BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
				Write16(pProfessionLicenseInfo, 0); //len

				Write8(pProfessionLicenseInfo, 6); //BigLevel
				Write8(pProfessionLicenseInfo, 5); //SmallLevel

				len = pProfessionLicenseInfo - pSpecialActivityInfo;
				Set16(pSpecialActivityInfo, (WORD)len);
				pSpecialActivityInfo += len;
			}

			len = pSpecialActivityInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		
		Write8(pPlayerInfo, 0); // PersonalRankedMatchLevelShowTag

		Write8(pPlayerInfo, 0); // ItemNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ItemOrder
			BYTE* pItemOrder = pPlayerInfo;
			Write16(pItemOrder, 0); //len

			Write32(pItemOrder, 0); //KartID
			Write32(pItemOrder, 0); //Order
			Write32(pItemOrder, 0); //BackGroundID

			len = pItemOrder - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); // ExRightFlagLen

		Write8(pPlayerInfo, 0); // EquipNiChangItemNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //NiChangPavilionEquipItemInfo
			BYTE* pNiChangPavilionEquipItemInfo = pPlayerInfo;
			Write16(pNiChangPavilionEquipItemInfo, 0); //len

			Write32(pNiChangPavilionEquipItemInfo, 0); //ItemId
			Write16(pNiChangPavilionEquipItemInfo, 0); //CurrentEquipColorationPlanId

			len = pNiChangPavilionEquipItemInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/
#endif
		len = pPlayerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	if (GetClient(DstUin)) {
		
		Write8(p, 1); // IsOnLine
		{			  // PositionInfo
			BYTE* pPositionInfo = p;
			Write16(pPositionInfo, 0); // len

			Write8(pPositionInfo, 0);  // ServerType
			Write16(pPositionInfo, 1); // Status
			Write32(pPositionInfo, GetClient(DstUin)->ServerID); // ServerID
			{// RoomInfo
				BYTE* pRoomInfo = pPositionInfo;
				Write16(pRoomInfo, 0); // len

				Write16(pRoomInfo, Dst->RoomID); // RoomID
				Write16(pRoomInfo, Dst->BaseGameMode); // BaseGameMode
				Write16(pRoomInfo, Dst->SubGameMode); // SubGameMode
				Write8(pRoomInfo, 0);  // CurrentPlayerNum
				Write8(pRoomInfo, 0);  // TotalSeatNum
				Write8(pRoomInfo, 0);  // Status
				Write32(pRoomInfo, Dst->MapID); // MapID
				Write8(pRoomInfo, 0);  // Flag

				// RoomName[]
				WriteString(pRoomInfo, "游戏zhong");

				Write8(pRoomInfo, 0); // HasFBInfo
				/*
				{ //FBInfo
					BYTE* pFBInfo = pRoomInfo;
					Write16(pFBInfo, 0); //len

					Write8(pFBInfo, 0); //AvailableLev
					Write8(pFBInfo, 0); //Lev
					Write8(pFBInfo, 0); //SubLev
					Write32(pFBInfo, 0); //BossID
					Write8(pFBInfo, 0); //FBSeason

					len = pFBInfo - pRoomInfo;
					Set16(pRoomInfo, (WORD)len);
					pRoomInfo += len;
				}
				*/
#ifndef ZingSpeed
				Write8(pRoomInfo, 0);  // SpeFlag
				Write32(pRoomInfo, Dst->RoomID); // RoomNo
				Write16(pRoomInfo, Dst->ServerID); // SvrId
				Write16(pRoomInfo, 0); // SceneID
				Write8(pRoomInfo, 0);  // CurrentOBNum
				Write8(pRoomInfo, 0);  // TotalOBNum
				Write8(pRoomInfo, 0);  // SpecialActivityStatus
				Write8(pRoomInfo, 0);  // AllowAutoStart
				Write32(pRoomInfo, 0); // Gender
				Write32(pRoomInfo, 0); // LocaleCode
				Write8(pRoomInfo, 0);  // TalkRoomBGID
				Write8(pRoomInfo, 0);  // SpecialActivityIDNum

				Write8(pRoomInfo, 0); // SpecialActivityInfoNum

				for (size_t i = 0; i < 0; i++)
				{ //SpecialActivityInfo
					BYTE* pSpecialActivityInfo = pRoomInfo;
					Write16(pSpecialActivityInfo, 0); //len

					Write8(pSpecialActivityInfo, 1); //ProfessionLicenseInfoNum
					for (size_t i = 0; i < 1; i++)
					{ //ProfessionLicenseInfo
						BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
						Write16(pProfessionLicenseInfo, 0); //len

						Write8(pProfessionLicenseInfo, 6); //BigLevel
						Write8(pProfessionLicenseInfo, 5); //SmallLevel

						len = pProfessionLicenseInfo - pSpecialActivityInfo;
						Set16(pSpecialActivityInfo, (WORD)len);
						pSpecialActivityInfo += len;
					}

					len = pSpecialActivityInfo - pRoomInfo;
					Set16(pRoomInfo, (WORD)len);
					pRoomInfo += len;
				}


				Write8(pRoomInfo, 0); // ValueNum
				//Write32(pRoomInfo, 0); //EnterRoomValue[]
				Write8(pRoomInfo, 0); // ParaNum
				//Write32(pRoomInfo, 0); //ParaList[]
				Write32(pRoomInfo, Dst->RoomID); // GSvrRoomNo

				// OwnerName[]
				memset(pRoomInfo, 0, MaxNickName);
				pRoomInfo += MaxNickName;
#endif
				len = pRoomInfo - pPositionInfo;
				Set16(pPositionInfo, (WORD)len);
				pPositionInfo += len;
			}
			Write8(pPositionInfo, 0); // StatusLen
			Write8(pPositionInfo, 1); // CanJoin

			len = pPositionInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	else {
		Write8(p, 0); // IsOnLine
		{			  // PositionInfo
			BYTE* pPositionInfo = p;
			Write16(pPositionInfo, 0); // len

			Write8(pPositionInfo, 0);  // ServerType
			Write16(pPositionInfo, 0); // Status
			Write32(pPositionInfo, 0); // ServerID
			{// RoomInfo
				BYTE* pRoomInfo = pPositionInfo;
				Write16(pRoomInfo, 0); // len

				Write16(pRoomInfo, 0); // RoomID
				Write16(pRoomInfo, 0); // BaseGameMode
				Write16(pRoomInfo, 0); // SubGameMode
				Write8(pRoomInfo, 0);  // CurrentPlayerNum
				Write8(pRoomInfo, 0);  // TotalSeatNum
				Write8(pRoomInfo, 0);  // Status
				Write32(pRoomInfo, 0); // MapID
				Write8(pRoomInfo, 0);  // Flag

				// RoomName[]
				memset(pRoomInfo, 0, MaxNickName);
				pRoomInfo += MaxNickName;

				Write8(pRoomInfo, 0); // HasFBInfo
				/*
				{ //FBInfo
					BYTE* pFBInfo = pRoomInfo;
					Write16(pFBInfo, 0); //len

					Write8(pFBInfo, 0); //AvailableLev
					Write8(pFBInfo, 0); //Lev
					Write8(pFBInfo, 0); //SubLev
					Write32(pFBInfo, 0); //BossID
					Write8(pFBInfo, 0); //FBSeason

					len = pFBInfo - pRoomInfo;
					Set16(pRoomInfo, (WORD)len);
					pRoomInfo += len;
				}
				*/
#ifndef ZingSpeed
				Write8(pRoomInfo, 0);  // SpeFlag
				Write32(pRoomInfo, 0); // RoomNo
				Write16(pRoomInfo, 0); // SvrId
				Write16(pRoomInfo, 0); // SceneID
				Write8(pRoomInfo, 0);  // CurrentOBNum
				Write8(pRoomInfo, 0);  // TotalOBNum
				Write8(pRoomInfo, 0);  // SpecialActivityStatus
				Write8(pRoomInfo, 0);  // AllowAutoStart
				Write32(pRoomInfo, 0); // Gender
				Write32(pRoomInfo, 0); // LocaleCode
				Write8(pRoomInfo, 0);  // TalkRoomBGID
				Write8(pRoomInfo, 0);  // SpecialActivityIDNum

				Write8(pRoomInfo, 0); // SpecialActivityInfoNum

				for (size_t i = 0; i < 0; i++)
				{ //SpecialActivityInfo
					BYTE* pSpecialActivityInfo = pRoomInfo;
					Write16(pSpecialActivityInfo, 0); //len

					Write8(pSpecialActivityInfo, 1); //ProfessionLicenseInfoNum
					for (size_t i = 0; i < 1; i++)
					{ //ProfessionLicenseInfo
						BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
						Write16(pProfessionLicenseInfo, 0); //len

						Write8(pProfessionLicenseInfo, 6); //BigLevel
						Write8(pProfessionLicenseInfo, 5); //SmallLevel

						len = pProfessionLicenseInfo - pSpecialActivityInfo;
						Set16(pSpecialActivityInfo, (WORD)len);
						pSpecialActivityInfo += len;
					}

					len = pSpecialActivityInfo - pRoomInfo;
					Set16(pRoomInfo, (WORD)len);
					pRoomInfo += len;
				}


				Write8(pRoomInfo, 0); // ValueNum
				//Write32(pRoomInfo, 0); //EnterRoomValue[]
				Write8(pRoomInfo, 0); // ParaNum
				//Write32(pRoomInfo, 0); //ParaList[]
				Write32(pRoomInfo, 0); // GSvrRoomNo

				// OwnerName[]
				memset(pRoomInfo, 0, MaxNickName);
				pRoomInfo += MaxNickName;
#endif
				len = pRoomInfo - pPositionInfo;
				Set16(pPositionInfo, (WORD)len);
				pPositionInfo += len;
			}
			Write8(pPositionInfo, 0); // StatusLen
			Write8(pPositionInfo, 0); // CanJoin

			len = pPositionInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	
	Write8(p, 0); // ReasonLen
	NotifyEngageResult5(DstUin,p);
	 /* {// PlayerRelationInfo
		BYTE *pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); // len

		Write32(pPlayerRelationInfo, 0); // SrcUin
		Write32(pPlayerRelationInfo, 0); // RelationFlag
		Write32(pPlayerRelationInfo, 0); // RelationUin
		Write8(pPlayerRelationInfo, 0);	 // RelationUin
		// RelationNickName[]
		memset(pPlayerRelationInfo, 0, MaxNickName);
		pPlayerRelationInfo += MaxNickName;

		Write32(pPlayerRelationInfo, 0); // EngageTime
		Write32(pPlayerRelationInfo, 0); // NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); // BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); // EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0);	 // EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}*/
	{ // CurKartAttr
		BYTE *pCurKartAttr = p;
		Write16(pCurKartAttr, 0); // len

		const char *sql = NULL;
		sqlite3_stmt *stmt = NULL;
		int result;
		int RefitCout = 0;
		short MaxFlags = 0;
		short WWeight = 0;
		short SpeedWeight = 0;
		short JetWeight = 0;
		short SJetWeight = 0;
		short AccuWeight = 0;
		short Speed2Weight = 0;
		short DriftVecWeight = 0;
		short AdditionalZSpeedWeight = 0;
		short AntiCollisionWeight = 0;
		short SecondRefitCount = 0;
		int ShapeRefitCount = 0;
		int KartHeadRefitItemID = 0;
		int KartTailRefitItemID = 0;
		int KartFlankRefitItemID = 0;
		int KartTireRefitItemID = 0;
		short LuckyValue = 0;
		short RefitLuckyValueMaxWeight = 0;
		int ShapeSuitID = 0;
		short LegendSuitLevel = 0;
		short LegendSuitLevelChoice = 0;
		int ShapeLegendSuitID = 0;

		sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, DstUin);
			sqlite3_bind_int(stmt, 2, KartItemId);
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
				SecondRefitCount = sqlite3_column_int(stmt, 12);
				Speed2Weight = sqlite3_column_int(stmt, 13);
				DriftVecWeight = sqlite3_column_int(stmt, 14);
				AdditionalZSpeedWeight = sqlite3_column_int(stmt, 15);
				AntiCollisionWeight = sqlite3_column_int(stmt, 16);
				LuckyValue = sqlite3_column_int(stmt, 17);
				RefitLuckyValueMaxWeight = sqlite3_column_int(stmt, 18);
				ShapeSuitID = sqlite3_column_int(stmt, 19);
				LegendSuitLevel = sqlite3_column_int(stmt, 20);
				LegendSuitLevelChoice = sqlite3_column_int(stmt, 21);
				ShapeLegendSuitID = sqlite3_column_int(stmt, 22);
			}
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		Write32(pCurKartAttr, DstUin);				 // Uin
		Write32(pCurKartAttr, KartItemId);			 // KartId
		Write32(pCurKartAttr, RefitCout);			 // RefitCout
		Write16(pCurKartAttr, MaxFlags);			 // MaxFlags
		Write16(pCurKartAttr, WWeight);				 // WWeight
		Write16(pCurKartAttr, SpeedWeight);			 // SpeedWeight
		Write16(pCurKartAttr, JetWeight);			 // JetWeight
		Write16(pCurKartAttr, SJetWeight);			 // SJetWeight
		Write16(pCurKartAttr, AccuWeight);			 // AccuWeight
		Write32(pCurKartAttr, ShapeRefitCount);		 // ShapeRefitCount
		Write32(pCurKartAttr, KartHeadRefitItemID);	 // KartHeadRefitItemID
		Write32(pCurKartAttr, KartTailRefitItemID);	 // KartTailRefitItemID
		Write32(pCurKartAttr, KartFlankRefitItemID); // KartFlankRefitItemID
		Write32(pCurKartAttr, KartTireRefitItemID);	 // KartTireRefitItemID
		{ // KartRefitExInfo
			BYTE* pKartRefitExInfo = pCurKartAttr;
			Write16(pKartRefitExInfo, 0); // len

			if (ShapeSuitID == 0) {
				if (KartHeadRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTailRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // JetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartFlankRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // SJetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTireRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // AccuRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}

				if (KartHeadRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTailRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // JetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartFlankRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // SJetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTireRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // AccuAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
			}
			else {
				Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				Write8(pKartRefitExInfo, 3); // JetRefitStar
				Write8(pKartRefitExInfo, 3); // SJetRefitStar
				Write8(pKartRefitExInfo, 3); // AccuRefitStar
				Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				Write8(pKartRefitExInfo, 10); // JetAddRatio
				Write8(pKartRefitExInfo, 10); // SJetAddRatio
				Write8(pKartRefitExInfo, 10); // AccuAddRatio
			}

			len = pKartRefitExInfo - pCurKartAttr;
			Set16(pCurKartAttr, (WORD)len);
			pCurKartAttr += len;
		}
		// Write32(pCurKartAttr, SecondRefitCount); //SpeedWeight
		// Write16(pCurKartAttr, Speed2Weight); //SpeedWeight
		// Write16(pCurKartAttr, DriftVecWeight); //SpeedWeight
		// Write16(pCurKartAttr, AdditionalZSpeedWeight); //SpeedWeight
		// Write16(pCurKartAttr, AntiCollisionWeight); //SpeedWeight
		

		Write32(pCurKartAttr, SecondRefitCount);	   // SecondRefitCount
		Write16(pCurKartAttr, Speed2Weight);		   // Speed2Weight
		Write16(pCurKartAttr, DriftVecWeight);		   // DriftVecWeight
		Write16(pCurKartAttr, AdditionalZSpeedWeight); // AdditionalZSpeedWeight
		Write16(pCurKartAttr, AntiCollisionWeight);	   // AntiCollisionWeight
		Write16(pCurKartAttr, 0);					   // LuckyValue
		Write16(pCurKartAttr, 0);					   // RefitLuckyValueMaxWeight
		Write32(pCurKartAttr, ShapeSuitID);			   // ShapeSuitID
		Write8(pCurKartAttr, LegendSuitLevel);		   // LegendSuitLevel
		Write32(pCurKartAttr, LegendSuitLevelChoice);  // LegendSuitLevelChoice
		Write32(pCurKartAttr, ShapeLegendSuitID);	   // ShapeLegendSuitID
		
		len = pCurKartAttr - p;
		Set16(p, (WORD)len);
		p += len;
	}
	int PetNum = PetId > 1 ? 1 : 0;
	BYTE* pPetNum = p;
	Write16(p, 0); //PetNum
	{
		auto uPetItem = GetPetItem(PetId,DstUin);
		if (uPetItem->ID!=-1) {
			BYTE* pPetInfo = p;

			Write16(pPetInfo, 0); //len
			Write32(pPetInfo, DstUin); //Uin
			Write32(pPetInfo, uPetItem->ID); //PetId
			memcpy(pPetInfo, uPetItem->Name, 23);
			pPetInfo += 23;
			Write8(pPetInfo, 0); //IsProper		
			Write16(pPetInfo, uPetItem->Level); //Level
			Write8(pPetInfo, uPetItem->Status); //Status
			Write16(pPetInfo, 0); //PetSkillNum	
			for (size_t i = 0; i < 0; i++)
			{ //PetSkillList
				BYTE* pPetSkillList = pPetInfo;
				Write16(pPetSkillList, 0); //len

				Write16(pPetSkillList, 0); //SkillID
				Write8(pPetSkillList, i + 1); //Status
				Write8(pPetSkillList, 1); //Active
				Write32(pPetSkillList, 0); //Value

				len = pPetSkillList - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write32(pPetInfo, 10); //StrengLevel
			Write8(pPetInfo, 4); //TopStatus
			{ //CommonInfo
				BYTE* pCommonInfo = pPetInfo;
				Write16(pCommonInfo, 0); //len

				Write32(pCommonInfo, 0); //HuanHuaLevel
				Write8(pCommonInfo, 0); //CanHuanHua

				len = pCommonInfo - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write16(pPetInfo, 0); //PKStar
			Write32(pPetInfo, 0); //PetSkinID


			len = pPetInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		else if(PetId>0){
			BYTE* pPetInfo = p;
			Write16(pPetInfo, 0); //len
			Write32(pPetInfo, DstUin); //Uin
			Write32(pPetInfo, PetId); //PetId
			memcpy(pPetInfo, "未配置昵称", 23);
			pPetInfo += 23;
			Write8(pPetInfo, 1); //IsProper		
			Write16(pPetInfo, 99); //Level
			Write8(pPetInfo, GetPetStatus(30, PetId)); //Status
			Write16(pPetInfo, 3); //PetSkillNum	
			for (size_t i = 0; i < 3; i++)
			{ //PetSkillList
				BYTE* pPetSkillList = pPetInfo;
				Write16(pPetSkillList, 0); //len

				Write16(pPetSkillList, i + 1); //SkillID
				Write8(pPetSkillList, i + 1); //Status
				Write8(pPetSkillList, 1); //Active
				Write32(pPetSkillList, 300); //Value

				len = pPetSkillList - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write32(pPetInfo, 10); //StrengLevel
			Write8(pPetInfo, 4); //TopStatus
			{ //CommonInfo
				BYTE* pCommonInfo = pPetInfo;
				Write16(pCommonInfo, 0); //len

				Write32(pCommonInfo, 0); //HuanHuaLevel
				Write8(pCommonInfo, 0); //CanHuanHua

				len = pCommonInfo - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write16(pPetInfo, 0); //PKStar
			Write32(pPetInfo, 0); //PetSkinID

			len = pPetInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

	}
	Set16(pPetNum, (WORD)PetNum);

	Write8(p, 1); // IsInTopList
	{// KartStoneGrooveInfo
		BYTE *pKartStoneGrooveInfo = p;
		Write16(pKartStoneGrooveInfo, 0); // len
		int ID = -1;
		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, DstUin);
			sqlite3_bind_int(stmt, 2, KartItemId);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		Write32(pKartStoneGrooveInfo, KartItemId); // KartID
		BYTE* pStoneGrooveNum = pKartStoneGrooveInfo;
		Write32(pKartStoneGrooveInfo, 0); // StoneGrooveNum
		if (ID == -1)
		{
			
			for (size_t i = 0; i < 0; i++)
			{ //StoneGrooveInfo
				BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
				Write16(pStoneGrooveInfo, 0); //len

				Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
				Write32(pStoneGrooveInfo, 0); //SkillStoneID

				len = pStoneGrooveInfo - pKartStoneGrooveInfo;
				Set16(pKartStoneGrooveInfo, (WORD)len);
				pKartStoneGrooveInfo += len;
			}
		}
		else {
			int StoneGrooveNum = 0;
			sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ID);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{ // StoneGrooveInfo
					StoneGrooveNum++;

					BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); // StoneUseOccaType
					Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); // SkillStoneID

					len = pStoneGrooveInfo - pKartStoneGrooveInfo;
					Set16(pKartStoneGrooveInfo, (WORD)len);
					pKartStoneGrooveInfo += len;
				}
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			Set32(pStoneGrooveNum, StoneGrooveNum);
			//for (size_t i = 0; i < 0; i++)
			//{ //StoneGrooveInfo
			//	BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
			//	Write16(pStoneGrooveInfo, 0); //len

			//	Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
			//	Write32(pStoneGrooveInfo, 0); //SkillStoneID

			//	len = pStoneGrooveInfo - pKartStoneGrooveInfo;
			//	Set16(pKartStoneGrooveInfo, (WORD)len);
			//	pKartStoneGrooveInfo += len;
			//}
		}
		
		

		len = pKartStoneGrooveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // Contex

	{ // PlayerWlVisibleInfo
		BYTE *pPlayerWlVisibleInfo = p;
		Write16(pPlayerWlVisibleInfo, 0); // len

		Write32(pPlayerWlVisibleInfo, 0); // WonderLandID

		// WonderLandName[]
		memset(pPlayerWlVisibleInfo, 0, MaxNickName);
		pPlayerWlVisibleInfo += MaxNickName;

		Write16(pPlayerWlVisibleInfo, 0); // WonderLandDuty
		Write8(pPlayerWlVisibleInfo, 0);  // SubType
		Write8(pPlayerWlVisibleInfo, 0);  // GuildGrade

		len = pPlayerWlVisibleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); // SkateCoin
	Write32(p, 0); // MaxDayPveScore
	Write32(p, 0); // MaxHistoryPveScore

	{ // EquipedCarBattleModeSkillInfoList
		BYTE *pEquipedCarBattleModeSkillInfoList = p;
		Write16(pEquipedCarBattleModeSkillInfoList, 0); // len

		Write8(pEquipedCarBattleModeSkillInfoList, 0); // SkillNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //SkillInfo
			BYTE* pSkillInfo = pEquipedCarBattleModeSkillInfoList;
			Write16(pSkillInfo, 0); //len

			Write16(pSkillInfo, 0); //SkillId
			Write8(pSkillInfo, 0); //SkillLevel

			len = pSkillInfo - pEquipedCarBattleModeSkillInfoList;
			Set16(pEquipedCarBattleModeSkillInfoList, (WORD)len);
			pEquipedCarBattleModeSkillInfoList += len;
		}
		*/

		len = pEquipedCarBattleModeSkillInfoList - p;
		Set16(p, (WORD)len);
		p += len;
	}
#ifndef ZingSpeed
	Write8(p, 0); // HasCCVisibleInfo
	/*
	{ //ChumCircleInfo
		BYTE* pChumCircleInfo = p;
		Write16(pChumCircleInfo, 0); //len

		Write32(pChumCircleInfo, 0); //ChumCircleID

		len = pChumCircleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, 0); // HasCheerAddition
	/*
	{ //CheerAdditon
		BYTE* pCheerAdditon = p;
		Write16(pCheerAdditon, 0); //len

		Write16(pCheerAdditon, 0); //ExpAddition
		Write16(pCheerAdditon, 0); //WageAddition

		len = pCheerAdditon - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write32(p, 0); // LoveValue
	Write16(p, 0); // CanEnterChannelFalg[]
	Write8(p, 0);  // SpecialActivityID
	Write32(p, 0); // ThemeHouseDressDegreeValue
	Write8(p, AreaIdDst);  // AreaIdDst
	Write8(p, 0);  // SpecialActivityIDNum
	Write8(p, SimpleInfo);  // SimpleInfo
#endif

	len = p - buf;
	SendToClient(Client, 122, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseFindPlayerByQQ2(ClientNode* Client, UINT DstUin)
{
	ClientNode* Dst = GetClient(DstUin);
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int GuildVipLevel = 0;
	int GuildVipPoint = 0;
	int GuildVipLeftDays = 0;
	int LoverVipLevel = 0;
	int LoverVipPoint = 0;
	int LoverVipLeftDays = 0;
	int GardenLevel = 0;
	int GardenPoint = 0;
	int GardenExpireTime = 0;
	int VipLevel = 0;
	int CharmValueOfMonth = 0;
	int VipFlag = 0;
	int VipGrowRate = 0;
	int EmperorLevel = 0;
	int EmperorPoint = 0;
	int EmperorLeftDays = 0;
	int EmperorGrowRate = 0;
	int NobleLevel = 0;
	int NoblePoint = 0;
	int NobleLeftDays = 0;
	int NobleID = 0;

	sql = "SELECT GuildVipLevel,GuildVipPoint,GuildVipLeftDays,LoverVipLevel,LoverVipPoint,LoverVipLeftDays,GardenLevel,GardenPoint,GardenExpireTime,VipLevel,CharmValueOfMonth,VipFlag,VipGrowRate,EmperorLevel,EmperorPoint,EmperorLeftDays,EmperorGrowRate,NobleLevel,NoblePoint,NobleLeftDays,NobleID  FROM BaseInfo  WHERE Uin=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, DstUin);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			GuildVipLevel = sqlite3_column_int(stmt, 0);
			GuildVipPoint = sqlite3_column_int(stmt, 1);
			GuildVipLeftDays = sqlite3_column_int(stmt, 2);
			LoverVipLevel = sqlite3_column_int(stmt, 3);
			LoverVipPoint = sqlite3_column_int(stmt, 4);
			LoverVipLeftDays = sqlite3_column_int(stmt, 5);
			GardenLevel = sqlite3_column_int(stmt, 6);
			GardenPoint = sqlite3_column_int(stmt, 7);
			GardenExpireTime = sqlite3_column_int(stmt, 8);
			VipLevel = sqlite3_column_int(stmt, 9);
			CharmValueOfMonth = sqlite3_column_int(stmt, 10);
			VipFlag = sqlite3_column_int(stmt, 11);
			VipGrowRate = sqlite3_column_int(stmt, 12);
			EmperorLevel = sqlite3_column_int(stmt, 13);
			EmperorPoint = sqlite3_column_int(stmt, 14);
			EmperorLeftDays = sqlite3_column_int(stmt, 15);
			EmperorGrowRate = sqlite3_column_int(stmt, 16);
			NobleLevel = sqlite3_column_int(stmt, 17);
			NoblePoint = sqlite3_column_int(stmt, 18);
			NobleLeftDays = sqlite3_column_int(stmt, 19);
			NobleID = sqlite3_column_int(stmt, 20);
		}
		else
		{
#ifdef DEBUG
			Z_ERROR("不存在的用户%d\n", DstUin);
			return;
#endif
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;
	UINT KartItemId = 0;
	int Identity = ID_IDENTIFY_QQLEVEL1 | ID_IDENTIFY_QQLEVEL2 | ID_IDENTIFY_QQLEVEL3 | ID_IDENTIFY_QQLEVEL4 | ID_IDENTIFY_QQLEVEL5 | ID_IDENTIFY_QQFLAG | ID_IDENTIFY_HAVEWORD | ID_IDENTIFY_FINISHQQFLAG;
	Identity |= ID_IDENTIFY_SPEEDMEMBER;
	int IsInTopList = 1;
	UINT PetId = 0;
	Write16(p, 0); // ResultID
	{			   // PlayerInfo
		BYTE* pPlayerInfo = p;
		Write16(pPlayerInfo, 0); // len

		Write32(pPlayerInfo, DstUin);	// Uin
		Write32(pPlayerInfo, Identity); // Identity
		if (GetClient(DstUin))
		{
			Write16(pPlayerInfo, GetClient(DstUin)->ConnID); // PlayerID
		}
		else
		{
			Write16(pPlayerInfo, 0); // PlayerID
		}

		WritePlayerDBBaseInfo(pPlayerInfo, DstUin);
		WritePlayerGuildInfo(pPlayerInfo, DstUin);
		KartItemId = WriteEquipedItem2(pPlayerInfo, DstUin, &PetId);

		Write32(pPlayerInfo, 300); // MasterPoint
		Write32(pPlayerInfo, 301); // TotalGuildProsperity

		Write16(pPlayerInfo, VipFlag);	   // VipFlag
		Write16(pPlayerInfo, VipGrowRate); // VipGrowRate

		Write8(pPlayerInfo, 1); // AppellationNum

		for (size_t i = 0; i < 1; i++)
		{ // Record
			BYTE* pRecord = pPlayerInfo;
			Write16(pRecord, 0); // len

			Write8(pRecord, 0);	 // Type
			Write8(pRecord, 0);	 // Level
			Write8(pRecord, 0);	 // Status
			Write8(pRecord, 0);	 // Difficulty
			Write32(pRecord, 0); // MapId
			Write32(pRecord, 0); // Value

			len = pRecord - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		{ // NobleInfo
			BYTE* pNobleInfo = pPlayerInfo;
			Write16(pNobleInfo, 0); // len

			Write32(pNobleInfo, NobleID);		// NobleID
			Write8(pNobleInfo, NobleLevel);		// NobleLevel
			Write32(pNobleInfo, NoblePoint);	// NoblePoint
			Write32(pNobleInfo, NobleLeftDays); // NobleLeftDays

			len = pNobleInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, 0); // HasCarryWizard
		/*
		{ //CarryWizardInfo
			BYTE* pCarryWizardInfo = pPlayerInfo;
			Write16(pCarryWizardInfo, 0); //len

			Write16(pCarryWizardInfo, 0); //WizardID
			WriteString(pCarryWizardInfo, 0); //NickName[]
			Write16(pCarryWizardInfo, 0); //WizardType
			Write8(pCarryWizardInfo, 0); //Order
			Write8(pCarryWizardInfo, 0); //EvolutionBranch
			Write8(pCarryWizardInfo, 0); //IsHuanHua

			len = pCarryWizardInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

#ifndef ZingSpeed
		{ // GuildVipBaseInfo
			BYTE* pGuildVipBaseInfo = pPlayerInfo;
			Write16(pGuildVipBaseInfo, 0); // len

			Write8(pGuildVipBaseInfo, GuildVipLevel);  // GuildVipLevel
			Write32(pGuildVipBaseInfo, GuildVipPoint); // GuildVipPoint

			len = pGuildVipBaseInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
#endif

		Write8(pPlayerInfo, 0); // HasLDMInfo
		/*
		for (size_t i = 0; i < n; i++)
		{ //LDMInfo
			BYTE* pLDMInfo = pPlayerInfo;
			Write16(pLDMInfo, 0); //len

			Write8(pLDMInfo, 0); //IsInCurSeason
			Write32(pLDMInfo, 0); //Score
			Write32(pLDMInfo, 0); //MaxScore
			Write8(pLDMInfo, 0); //Grade
			Write8(pLDMInfo, 0); //MaxGrade
			Write32(pLDMInfo, 0); //WinStreakCount
			Write32(pLDMInfo, 0); //MatchCount
			Write32(pLDMInfo, 0); //CollisionCount
			Write32(pLDMInfo, 0); //BeatCount

			len = pLDMInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

#ifndef ZingSpeed
		Write8(pPlayerInfo, 1); // HasLoverVip

		for (size_t i = 0; i < 1; i++)
		{ // LoverVipInfo
			BYTE* pLoverVipInfo = pPlayerInfo;
			Write16(pLoverVipInfo, 0); // len

			Write8(pLoverVipInfo, LoverVipLevel);  // LoverVipLevel
			Write32(pLoverVipInfo, LoverVipPoint); // LoverVipPoint
			Write8(pLoverVipInfo, 0);			   // GrowRate

			len = pLoverVipInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

#endif

		Write8(pPlayerInfo, 0); // IsShowMounts

#ifndef ZingSpeed
		Write8(pPlayerInfo, 1); // HasGarden

		{ // PersonalGardenBaseInfo
			BYTE* pPersonalGardenBaseInfo = pPlayerInfo;
			Write16(pPersonalGardenBaseInfo, 0); // len

			Write8(pPersonalGardenBaseInfo, GardenLevel);		// GardenLevel
			Write32(pPersonalGardenBaseInfo, GardenPoint);		// GardenPoint
			Write8(pPersonalGardenBaseInfo, 0);					// GrowRate
			Write32(pPersonalGardenBaseInfo, GardenExpireTime); // GardenExpireTime
			{													// SimpleInfo
				BYTE* pSimpleInfo = pPersonalGardenBaseInfo;
				Write16(pSimpleInfo, 0); // len

				Write32(pSimpleInfo, 0); // WeekPopularity
				Write32(pSimpleInfo, 0); // TotalPopularity
				Write32(pSimpleInfo, 0); // LastUpdatePopularityTime
				Write8(pSimpleInfo, 0);	 // PrivateType

				len = pSimpleInfo - pPersonalGardenBaseInfo;
				Set16(pPersonalGardenBaseInfo, (WORD)len);
				pPersonalGardenBaseInfo += len;
			}

			len = pPersonalGardenBaseInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, VipLevel);	// isConsumeVip
		Write32(pPlayerInfo, VipLevel); // ConsumeVipLevel
		Write32(pPlayerInfo, 0);		// SearchTreasureNums
		Write32(pPlayerInfo, 0);		// GetTreasureNums
		Write32(pPlayerInfo, 0);		// ConsumeVipCharmVlaueOfMonth
		{								// EmperorInfo
			BYTE* pEmperorInfo = pPlayerInfo;
			Write16(pEmperorInfo, 0); // len

			Write8(pEmperorInfo, EmperorLevel);		// EmperorLevel
			Write32(pEmperorInfo, EmperorPoint);	// EmperorPoint
			Write32(pEmperorInfo, EmperorLeftDays); // EmperorLeftDays
			Write8(pEmperorInfo, EmperorGrowRate);	// EmperorGrowRate

			len = pEmperorInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		{ // EmperorOtherInfo
			BYTE* pEmperorOtherInfo = pPlayerInfo;
			Write16(pEmperorOtherInfo, 0); // len

			Write32(pEmperorOtherInfo, 0); // ExpiredTime
			Write8(pEmperorOtherInfo, 0);  // ShowExpireTips

			len = pEmperorOtherInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		{ // ActivityInfo
			BYTE* pActivityInfo = pPlayerInfo;
			Write16(pActivityInfo, 0); // len

			Write32(pActivityInfo, 0); // TotalActivity
			Write32(pActivityInfo, 0); // ActivityLevel

			len = pActivityInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write8(pPlayerInfo, 0); // HaveWakedKartAttributeAddInfo
		/*
		{ //WakeKartAttributeAddInfo
			BYTE* pWakeKartAttributeAddInfo = pPlayerInfo;
			Write16(pWakeKartAttributeAddInfo, 0); //len

			Write16(pWakeKartAttributeAddInfo, 0); //AccAddValue
			Write16(pWakeKartAttributeAddInfo, 0); //SpeedAddValue
			Write16(pWakeKartAttributeAddInfo, 0); //JetAddValue
			Write16(pWakeKartAttributeAddInfo, 0); //SJetAddValue

			len = pWakeKartAttributeAddInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); // HaveLoverBabyInfo

		//{ //FindLoverBabyInfo
		//	BYTE* pFindLoverBabyInfo = pPlayerInfo;
		//	Write16(pFindLoverBabyInfo, 0); //len

		//	Write32(pFindLoverBabyInfo, 1); //BabyStatus
		//	Write32(pFindLoverBabyInfo, 71554); //LoverBabyItemID
		//	Write32(pFindLoverBabyInfo, 71554); //LoverBabyID
		//	Write32(pFindLoverBabyInfo, 0); //BabyGrowLevel
		//	Write32(pFindLoverBabyInfo, 0); //BabyStrengthLevel
		//	Write16(pFindLoverBabyInfo, 0); //LoverBabyStat
		//	Write16(pFindLoverBabyInfo, 0); //LoverBabyEquipStat
		//	Write16(pFindLoverBabyInfo, 0); //LoverBabyTransferdStatus

		//	len = pFindLoverBabyInfo - pPlayerInfo;
		//	Set16(pPlayerInfo, (WORD)len);
		//	pPlayerInfo += len;
		//}

		{ // GansterScoreInfo
			BYTE* pGansterScoreInfo = pPlayerInfo;
			Write16(pGansterScoreInfo, 0); // len

			Write32(pGansterScoreInfo, 0); // GansterSeasonID
			Write32(pGansterScoreInfo, 0); // GansterScore
			Write32(pGansterScoreInfo, 0); // PoliceScore
			Write32(pGansterScoreInfo, 0); // TotalGansterScore

			len = pGansterScoreInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}

		Write32(pPlayerInfo, 0); // OlympicId
		Write32(pPlayerInfo, 0); // LastOlympicUpdateTime
		Write32(pPlayerInfo, 0); // NPCEliminateWinTimes
		{						 // BorderInfo
			BYTE* pBorderInfo = pPlayerInfo;
			Write16(pBorderInfo, 0); // len

			Write32(pBorderInfo, 0); // SeasonID
			Write32(pBorderInfo, 0); // Zhanxun
			Write32(pBorderInfo, 0); // SeasonZhanxun

			len = pBorderInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		Write8(pPlayerInfo, 0);	   // SpecialActivityID
		Write32(pPlayerInfo, 100); // ThemeHouseCollectValue
		Write8(pPlayerInfo, 0);	   // HaveSecondLoverBabyInfo
		//{ //SecondLoverBabyInfo
		//	BYTE* pSecondLoverBabyInfo = pPlayerInfo;
		//	Write16(pSecondLoverBabyInfo, 0); //len

		//	Write32(pSecondLoverBabyInfo, 0); //BabyStatus
		//	Write32(pSecondLoverBabyInfo, 0); //LoverBabyItemID
		//	Write32(pSecondLoverBabyInfo, 0); //LoverBabyID
		//	Write32(pSecondLoverBabyInfo, 0); //BabyGrowLevel
		//	Write32(pSecondLoverBabyInfo, 0); //BabyStrengthLevel
		//	Write16(pSecondLoverBabyInfo, 0); //LoverBabyStat
		//	Write16(pSecondLoverBabyInfo, 0); //LoverBabyEquipStat
		//	Write16(pSecondLoverBabyInfo, 0); //LoverBabyTransferdStatus

		//	len = pSecondLoverBabyInfo - pPlayerInfo;
		//	Set16(pPlayerInfo, (WORD)len);
		//	pPlayerInfo += len;
		//}
		Write8(pPlayerInfo, 0);	 // EquipBaby
		Write8(pPlayerInfo, 0);	 // ActiveStatus
		Write32(pPlayerInfo, 0); // HelperLev

		Write8(pPlayerInfo, 0); // HasRankedMatchInfo
		/*
		{ //RankedMatchGradeInfo
			BYTE* pRankedMatchGradeInfo = pPlayerInfo;
			Write16(pRankedMatchGradeInfo, 0); //len

			Write32(pRankedMatchGradeInfo, 0); //GradeLevel
			Write32(pRankedMatchGradeInfo, 0); //ChildLevel
			Write32(pRankedMatchGradeInfo, 0); //GradeScore
			Write32(pRankedMatchGradeInfo, 0); //MaxGradeScore
			Write32(pRankedMatchGradeInfo, 0); //TotalScore

			len = pRankedMatchGradeInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write16(pPlayerInfo, 0); // PlayerSeasonInfoNums
		/*
		for (size_t i = 0; i < n; i++)
		{ //PlayerSeasonInfos
			BYTE* pPlayerSeasonInfos = pPlayerInfo;
			Write16(pPlayerSeasonInfos, 0); //len

			Write16(pPlayerSeasonInfos, 0); //SeasonID
			Write16(pPlayerSeasonInfos, 0); //GradeLevel
			Write8(pPlayerSeasonInfos, 0); //ChildLevel
			Write16(pPlayerSeasonInfos, 0); //HighGradeLevel
			Write8(pPlayerSeasonInfos, 0); //HighChildLevel
			Write8(pPlayerSeasonInfos, 0); //SeasonInfoProcesedTag
			Write8(pPlayerSeasonInfos, 0); //SeasonComputeInfoTipTag
			Write16(pPlayerSeasonInfos, 0); //SeasonInheritTag
			Write8(pPlayerSeasonInfos, 0); //SeasonInheritTipTag
			Write16(pPlayerSeasonInfos, 0); //SpeedGodNums
			Write32(pPlayerSeasonInfos, 0); //SpeedRoundNums
			Write32(pPlayerSeasonInfos, 0); //SpeedFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //TotalRankIndex
			Write32(pPlayerSeasonInfos, 0); //ContiFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //SpeedKillAllRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropFinishRoundNums
			Write32(pPlayerSeasonInfos, 0); //ContiFinishPropRoundNums
			Write32(pPlayerSeasonInfos, 0); //PropKillAllRoundNums
			Write8(pPlayerSeasonInfos, 0); //LastRoundIndex
			Write32(pPlayerSeasonInfos, 0); //OpenDate
			Write32(pPlayerSeasonInfos, 0); //CloseDate
			Write32(pPlayerSeasonInfos, 0); //HistroyContFinishRoundNums

			len = pPlayerSeasonInfos - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0);	 // IsHelperAppOnline
		Write32(pPlayerInfo, 0); // RankedMatchSpeedGodNums
		Write8(pPlayerInfo, 0);	 // EquipSealType

		Write8(pPlayerInfo, 0); // HasHuanLingChangeInfo
		/*
		{ //HuanLingSuitInfo
			BYTE* pHuanLingSuitInfo = pPlayerInfo;
			Write16(pHuanLingSuitInfo, 0); //len

			Write32(pHuanLingSuitInfo, 0); //SuitID
			Write8(pHuanLingSuitInfo, 0); //ChangeLevel

			len = pHuanLingSuitInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); // SpecialActivityIDNum
		Write8(pPlayerInfo, 0); // PersonalPanelSelectRankedMatchFrameTag

		Write8(pPlayerInfo, 0); // SpecialActivityInfoNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //SpecialActivityInfo
			BYTE* pSpecialActivityInfo = pPlayerInfo;
			Write16(pSpecialActivityInfo, 0); //len

			Write8(pSpecialActivityInfo, 0); //ProfessionLicenseInfoNum
			for (size_t i = 0; i < n; i++)
			{ //ProfessionLicenseInfo
				BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
				Write16(pProfessionLicenseInfo, 0); //len

				Write8(pProfessionLicenseInfo, 0); //BigLevel
				Write8(pProfessionLicenseInfo, 0); //SmallLevel

				len = pProfessionLicenseInfo - pSpecialActivityInfo;
				Set16(pSpecialActivityInfo, (WORD)len);
				pSpecialActivityInfo += len;
			}

			len = pSpecialActivityInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/
		Write8(pPlayerInfo, 0); // PersonalRankedMatchLevelShowTag

		Write8(pPlayerInfo, 0); // ItemNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ItemOrder
			BYTE* pItemOrder = pPlayerInfo;
			Write16(pItemOrder, 0); //len

			Write32(pItemOrder, 0); //KartID
			Write32(pItemOrder, 0); //Order
			Write32(pItemOrder, 0); //BackGroundID

			len = pItemOrder - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/

		Write8(pPlayerInfo, 0); // ExRightFlagLen

		Write8(pPlayerInfo, 0); // EquipNiChangItemNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //NiChangPavilionEquipItemInfo
			BYTE* pNiChangPavilionEquipItemInfo = pPlayerInfo;
			Write16(pNiChangPavilionEquipItemInfo, 0); //len

			Write32(pNiChangPavilionEquipItemInfo, 0); //ItemId
			Write16(pNiChangPavilionEquipItemInfo, 0); //CurrentEquipColorationPlanId

			len = pNiChangPavilionEquipItemInfo - pPlayerInfo;
			Set16(pPlayerInfo, (WORD)len);
			pPlayerInfo += len;
		}
		*/
#endif
		len = pPlayerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); // IsOnLine
	{			  // PositionInfo
		BYTE* pPositionInfo = p;
		Write16(pPositionInfo, 0); // len

		Write8(pPositionInfo, 0);  // ServerType
		Write16(pPositionInfo, 0); // Status
		Write32(pPositionInfo, 0); // ServerID
		{						   // RoomInfo
			BYTE* pRoomInfo = pPositionInfo;
			Write16(pRoomInfo, 0); // len

			Write16(pRoomInfo, 0); // RoomID
			Write16(pRoomInfo, 0); // BaseGameMode
			Write16(pRoomInfo, 0); // SubGameMode
			Write8(pRoomInfo, 0);  // CurrentPlayerNum
			Write8(pRoomInfo, 0);  // TotalSeatNum
			Write8(pRoomInfo, 0);  // Status
			Write32(pRoomInfo, 0); // MapID
			Write8(pRoomInfo, 0);  // Flag

			// RoomName[]
			memset(pRoomInfo, 0, MaxRoomName);
			pRoomInfo += MaxRoomName;

			Write8(pRoomInfo, 0); // HasFBInfo
			/*
			{ //FBInfo
				BYTE* pFBInfo = pRoomInfo;
				Write16(pFBInfo, 0); //len

				Write8(pFBInfo, 0); //AvailableLev
				Write8(pFBInfo, 0); //Lev
				Write8(pFBInfo, 0); //SubLev
				Write32(pFBInfo, 0); //BossID
				Write8(pFBInfo, 0); //FBSeason

				len = pFBInfo - pRoomInfo;
				Set16(pRoomInfo, (WORD)len);
				pRoomInfo += len;
			}
			*/
#ifndef ZingSpeed
			Write8(pRoomInfo, 0);  // SpeFlag
			Write32(pRoomInfo, 0); // RoomNo
			Write16(pRoomInfo, 0); // SvrId
			Write16(pRoomInfo, 0); // SceneID
			Write8(pRoomInfo, 0);  // CurrentOBNum
			Write8(pRoomInfo, 0);  // TotalOBNum
			Write8(pRoomInfo, 0);  // SpecialActivityStatus
			Write8(pRoomInfo, 0);  // AllowAutoStart
			Write32(pRoomInfo, 0); // Gender
			Write32(pRoomInfo, 0); // LocaleCode
			Write8(pRoomInfo, 0);  // TalkRoomBGID
			Write8(pRoomInfo, 0);  // SpecialActivityIDNum

			Write8(pRoomInfo, 0); // SpecialActivityInfoNum
			/*
			for (size_t i = 0; i < n; i++)
			{ //SpecialActivityInfo
				BYTE* pSpecialActivityInfo = pRoomInfo;
				Write16(pSpecialActivityInfo, 0); //len

				Write8(pSpecialActivityInfo, 0); //ProfessionLicenseInfoNum
				for (size_t i = 0; i < n; i++)
				{ //ProfessionLicenseInfo
					BYTE* pProfessionLicenseInfo = pSpecialActivityInfo;
					Write16(pProfessionLicenseInfo, 0); //len

					Write8(pProfessionLicenseInfo, 0); //BigLevel
					Write8(pProfessionLicenseInfo, 0); //SmallLevel

					len = pProfessionLicenseInfo - pSpecialActivityInfo;
					Set16(pSpecialActivityInfo, (WORD)len);
					pSpecialActivityInfo += len;
				}

				len = pSpecialActivityInfo - pRoomInfo;
				Set16(pRoomInfo, (WORD)len);
				pRoomInfo += len;
			}
			*/

			Write8(pRoomInfo, 0); // ValueNum
			// Write32(pRoomInfo, 0); //EnterRoomValue[]
			Write8(pRoomInfo, 0); // ParaNum
			// Write32(pRoomInfo, 0); //ParaList[]
			Write32(pRoomInfo, 0); // GSvrRoomNo

			// OwnerName[]
			memset(pRoomInfo, 0, MaxNickName);
			pRoomInfo += MaxNickName;
#endif
			len = pRoomInfo - pPositionInfo;
			Set16(pPositionInfo, (WORD)len);
			pPositionInfo += len;
		}
		Write8(pPositionInfo, 0); // StatusLen
		Write8(pPositionInfo, 0); // CanJoin

		len = pPositionInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // ReasonLen
	{			  // PlayerRelationInfo
		BYTE* pPlayerRelationInfo = p;
		Write16(pPlayerRelationInfo, 0); // len

		Write32(pPlayerRelationInfo, 0); // SrcUin
		Write32(pPlayerRelationInfo, 0);		   // RelationFlag
		Write32(pPlayerRelationInfo, 0);	   // RelationUin

		// RelationNickName[]
		memset(pPlayerRelationInfo, 0, MaxNickName);
		pPlayerRelationInfo += MaxNickName;

		Write32(pPlayerRelationInfo, 0); // EngageTime
		Write32(pPlayerRelationInfo, 0); // NextCanBookingTimeLimit
		Write32(pPlayerRelationInfo, 0); // BeginHoneyMoonTime
		Write32(pPlayerRelationInfo, 0); // EndHoneyMoonTime
		Write8(pPlayerRelationInfo, 0);	 // EngageFlag

		len = pPlayerRelationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // CurKartAttr
		BYTE* pCurKartAttr = p;
		Write16(pCurKartAttr, 0); // len

		const char* sql = NULL;
		sqlite3_stmt* stmt = NULL;
		int result;
		int RefitCout = 0;
		short MaxFlags = 0;
		short WWeight = 0;
		short SpeedWeight = 0;
		short JetWeight = 0;
		short SJetWeight = 0;
		short AccuWeight = 0;
		short Speed2Weight = 0;
		short DriftVecWeight = 0;
		short AdditionalZSpeedWeight = 0;
		short AntiCollisionWeight = 0;
		short SecondRefitCount = 0;
		int ShapeRefitCount = 0;
		int KartHeadRefitItemID = 0;
		int KartTailRefitItemID = 0;
		int KartFlankRefitItemID = 0;
		int KartTireRefitItemID = 0;
		short LuckyValue = 0;
		short RefitLuckyValueMaxWeight = 0;
		int ShapeSuitID = 0;
		short LegendSuitLevel = 0;
		short LegendSuitLevelChoice = 0;
		int ShapeLegendSuitID = 0;

		sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID  FROM KartRefit WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, DstUin);
			sqlite3_bind_int(stmt, 2, KartItemId);
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
				SecondRefitCount = sqlite3_column_int(stmt, 12);
				Speed2Weight = sqlite3_column_int(stmt, 13);
				DriftVecWeight = sqlite3_column_int(stmt, 14);
				AdditionalZSpeedWeight = sqlite3_column_int(stmt, 15);
				AntiCollisionWeight = sqlite3_column_int(stmt, 16);
				LuckyValue = sqlite3_column_int(stmt, 17);
				RefitLuckyValueMaxWeight = sqlite3_column_int(stmt, 18);
				ShapeSuitID = sqlite3_column_int(stmt, 19);
				LegendSuitLevel = sqlite3_column_int(stmt, 20);
				LegendSuitLevelChoice = sqlite3_column_int(stmt, 21);
				ShapeLegendSuitID = sqlite3_column_int(stmt, 22);
			}
		}
		else
		{
			Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		Write32(pCurKartAttr, DstUin);				 // Uin
		Write32(pCurKartAttr, KartItemId);			 // KartId
		Write32(pCurKartAttr, RefitCout);			 // RefitCout
		Write16(pCurKartAttr, MaxFlags);			 // MaxFlags
		Write16(pCurKartAttr, WWeight);				 // WWeight
		Write16(pCurKartAttr, SpeedWeight);			 // SpeedWeight
		Write16(pCurKartAttr, JetWeight);			 // JetWeight
		Write16(pCurKartAttr, SJetWeight);			 // SJetWeight
		Write16(pCurKartAttr, AccuWeight);			 // AccuWeight
		Write32(pCurKartAttr, ShapeRefitCount);		 // ShapeRefitCount
		Write32(pCurKartAttr, KartHeadRefitItemID);	 // KartHeadRefitItemID
		Write32(pCurKartAttr, KartTailRefitItemID);	 // KartTailRefitItemID
		Write32(pCurKartAttr, KartFlankRefitItemID); // KartFlankRefitItemID
		Write32(pCurKartAttr, KartTireRefitItemID);	 // KartTireRefitItemID
		// Write32(pCurKartAttr, SecondRefitCount); //SpeedWeight
		// Write16(pCurKartAttr, Speed2Weight); //SpeedWeight
		// Write16(pCurKartAttr, DriftVecWeight); //SpeedWeight
		// Write16(pCurKartAttr, AdditionalZSpeedWeight); //SpeedWeight
		// Write16(pCurKartAttr, AntiCollisionWeight); //SpeedWeight
		{ // KartRefitExInfo
			BYTE* pKartRefitExInfo = pCurKartAttr;
			Write16(pKartRefitExInfo, 0); // len

			Write8(pKartRefitExInfo, 0); // SpeedRefitStar
			Write8(pKartRefitExInfo, 0); // JetRefitStar
			Write8(pKartRefitExInfo, 0); // SJetRefitStar
			Write8(pKartRefitExInfo, 0); // AccuRefitStar
			Write8(pKartRefitExInfo, 0); // SpeedAddRatio
			Write8(pKartRefitExInfo, 0); // JetAddRatio
			Write8(pKartRefitExInfo, 0); // SJetAddRatio
			Write8(pKartRefitExInfo, 0); // AccuAddRatio

			len = pKartRefitExInfo - pCurKartAttr;
			Set16(pCurKartAttr, (WORD)len);
			pCurKartAttr += len;
		}

		Write32(pCurKartAttr, SecondRefitCount);	   // SecondRefitCount
		Write16(pCurKartAttr, Speed2Weight);		   // Speed2Weight
		Write16(pCurKartAttr, DriftVecWeight);		   // DriftVecWeight
		Write16(pCurKartAttr, AdditionalZSpeedWeight); // AdditionalZSpeedWeight
		Write16(pCurKartAttr, AntiCollisionWeight);	   // AntiCollisionWeight
		Write16(pCurKartAttr, 0);					   // LuckyValue
		Write16(pCurKartAttr, 0);					   // RefitLuckyValueMaxWeight
		Write32(pCurKartAttr, ShapeSuitID);			   // ShapeSuitID
		Write8(pCurKartAttr, LegendSuitLevel);		   // LegendSuitLevel
		Write32(pCurKartAttr, LegendSuitLevelChoice);  // LegendSuitLevelChoice
		Write32(pCurKartAttr, ShapeLegendSuitID);	   // ShapeLegendSuitID

		len = pCurKartAttr - p;
		Set16(p, (WORD)len);
		p += len;
	}
	int PetNum = PetId > 1 ? 1 : 0;
	BYTE* pPetNum = p;
	Write16(p, 0); //PetNum
	{
		auto uPetItem = GetPetItem(PetId, DstUin);
		if (uPetItem->ID != -1) {
			BYTE* pPetInfo = p;

			Write16(pPetInfo, 0); //len
			Write32(pPetInfo, DstUin); //Uin
			Write32(pPetInfo, uPetItem->ID); //PetId
			memcpy(pPetInfo, uPetItem->Name, 23);
			pPetInfo += 23;
			Write8(pPetInfo, 0); //IsProper		
			Write16(pPetInfo, uPetItem->Level); //Level
			Write8(pPetInfo, uPetItem->Status); //Status
			Write16(pPetInfo, 0); //PetSkillNum	
			for (size_t i = 0; i < 0; i++)
			{ //PetSkillList
				BYTE* pPetSkillList = pPetInfo;
				Write16(pPetSkillList, 0); //len

				Write16(pPetSkillList, 0); //SkillID
				Write8(pPetSkillList, i + 1); //Status
				Write8(pPetSkillList, 1); //Active
				Write32(pPetSkillList, 0); //Value

				len = pPetSkillList - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write32(pPetInfo, 10); //StrengLevel
			Write8(pPetInfo, 4); //TopStatus
			{ //CommonInfo
				BYTE* pCommonInfo = pPetInfo;
				Write16(pCommonInfo, 0); //len

				Write32(pCommonInfo, 0); //HuanHuaLevel
				Write8(pCommonInfo, 0); //CanHuanHua

				len = pCommonInfo - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write16(pPetInfo, 0); //PKStar
			Write32(pPetInfo, 0); //PetSkinID


			len = pPetInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		else if (PetId > 0) {
			BYTE* pPetInfo = p;
			Write16(pPetInfo, 0); //len
			Write32(pPetInfo, DstUin); //Uin
			Write32(pPetInfo, PetId); //PetId
			memcpy(pPetInfo, "未配置昵称", 23);
			pPetInfo += 23;
			Write8(pPetInfo, 1); //IsProper		
			Write16(pPetInfo, 99); //Level
			Write8(pPetInfo, GetPetStatus(30, PetId)); //Status
			Write16(pPetInfo, 3); //PetSkillNum	
			for (size_t i = 0; i < 3; i++)
			{ //PetSkillList
				BYTE* pPetSkillList = pPetInfo;
				Write16(pPetSkillList, 0); //len

				Write16(pPetSkillList, i + 1); //SkillID
				Write8(pPetSkillList, i + 1); //Status
				Write8(pPetSkillList, 1); //Active
				Write32(pPetSkillList, 300); //Value

				len = pPetSkillList - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write32(pPetInfo, 10); //StrengLevel
			Write8(pPetInfo, 4); //TopStatus
			{ //CommonInfo
				BYTE* pCommonInfo = pPetInfo;
				Write16(pCommonInfo, 0); //len

				Write32(pCommonInfo, 0); //HuanHuaLevel
				Write8(pCommonInfo, 0); //CanHuanHua

				len = pCommonInfo - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}
			Write16(pPetInfo, 0); //PKStar
			Write32(pPetInfo, 0); //PetSkinID

			len = pPetInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

	}
	Set16(pPetNum, (WORD)PetNum);

	Write8(p, 1); // IsInTopList
	{// KartStoneGrooveInfo
		BYTE* pKartStoneGrooveInfo = p;
		Write16(pKartStoneGrooveInfo, 0); // len
		int ID = -1;
		sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, DstUin);
			sqlite3_bind_int(stmt, 2, KartItemId);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				ID = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		Write32(pKartStoneGrooveInfo, KartItemId); // KartID
		BYTE* pStoneGrooveNum = pKartStoneGrooveInfo;
		Write32(pKartStoneGrooveInfo, 0); // StoneGrooveNum
		if (ID == -1)
		{

			for (size_t i = 0; i < 0; i++)
			{ //StoneGrooveInfo
				BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
				Write16(pStoneGrooveInfo, 0); //len

				Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
				Write32(pStoneGrooveInfo, 0); //SkillStoneID

				len = pStoneGrooveInfo - pKartStoneGrooveInfo;
				Set16(pKartStoneGrooveInfo, (WORD)len);
				pKartStoneGrooveInfo += len;
			}
		}
		else {
			int StoneGrooveNum = 0;
			sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ID);
				while (sqlite3_step(stmt) == SQLITE_ROW)
				{ // StoneGrooveInfo
					StoneGrooveNum++;

					BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 0)); // StoneUseOccaType
					Write32(pStoneGrooveInfo, sqlite3_column_int(stmt, 1)); // SkillStoneID

					len = pStoneGrooveInfo - pKartStoneGrooveInfo;
					Set16(pKartStoneGrooveInfo, (WORD)len);
					pKartStoneGrooveInfo += len;
				}
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			Set32(pStoneGrooveNum, StoneGrooveNum);
			//for (size_t i = 0; i < 0; i++)
			//{ //StoneGrooveInfo
			//	BYTE* pStoneGrooveInfo = pKartStoneGrooveInfo;
			//	Write16(pStoneGrooveInfo, 0); //len

			//	Write32(pStoneGrooveInfo, 0); //StoneUseOccaType
			//	Write32(pStoneGrooveInfo, 0); //SkillStoneID

			//	len = pStoneGrooveInfo - pKartStoneGrooveInfo;
			//	Set16(pKartStoneGrooveInfo, (WORD)len);
			//	pKartStoneGrooveInfo += len;
			//}
		}



		len = pKartStoneGrooveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // Contex

	{ // PlayerWlVisibleInfo
		BYTE* pPlayerWlVisibleInfo = p;
		Write16(pPlayerWlVisibleInfo, 0); // len

		Write32(pPlayerWlVisibleInfo, 0); // WonderLandID

		// WonderLandName[]
		memset(pPlayerWlVisibleInfo, 0, MaxNickName);
		pPlayerWlVisibleInfo += MaxNickName;

		Write16(pPlayerWlVisibleInfo, 0); // WonderLandDuty
		Write8(pPlayerWlVisibleInfo, 0);  // SubType
		Write8(pPlayerWlVisibleInfo, 0);  // GuildGrade

		len = pPlayerWlVisibleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); // SkateCoin
	Write32(p, 0); // MaxDayPveScore
	Write32(p, 0); // MaxHistoryPveScore

	{ // EquipedCarBattleModeSkillInfoList
		BYTE* pEquipedCarBattleModeSkillInfoList = p;
		Write16(pEquipedCarBattleModeSkillInfoList, 0); // len

		Write8(pEquipedCarBattleModeSkillInfoList, 0); // SkillNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //SkillInfo
			BYTE* pSkillInfo = pEquipedCarBattleModeSkillInfoList;
			Write16(pSkillInfo, 0); //len

			Write16(pSkillInfo, 0); //SkillId
			Write8(pSkillInfo, 0); //SkillLevel

			len = pSkillInfo - pEquipedCarBattleModeSkillInfoList;
			Set16(pEquipedCarBattleModeSkillInfoList, (WORD)len);
			pEquipedCarBattleModeSkillInfoList += len;
		}
		*/

		len = pEquipedCarBattleModeSkillInfoList - p;
		Set16(p, (WORD)len);
		p += len;
	}
#ifndef ZingSpeed
	Write8(p, 0); // HasCCVisibleInfo
	/*
	{ //ChumCircleInfo
		BYTE* pChumCircleInfo = p;
		Write16(pChumCircleInfo, 0); //len

		Write32(pChumCircleInfo, 0); //ChumCircleID

		len = pChumCircleInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write8(p, 0); // HasCheerAddition
	/*
	{ //CheerAdditon
		BYTE* pCheerAdditon = p;
		Write16(pCheerAdditon, 0); //len

		Write16(pCheerAdditon, 0); //ExpAddition
		Write16(pCheerAdditon, 0); //WageAddition

		len = pCheerAdditon - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write32(p, 0); // LoveValue
	Write16(p, 0); // CanEnterChannelFalg[]
	Write8(p, 0);  // SpecialActivityID
	Write32(p, 0); // ThemeHouseDressDegreeValue
	Write8(p, 0);  // AreaIdDst
	Write8(p, 0);  // SpecialActivityIDNum
	Write8(p, 0);  // SimpleInfo
#endif

	len = p - buf;
	SendToClient(Client, 122, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetAchieveList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);

	ResponseGetAchieveList(Client, DstUin);
}

void ResponseGetAchieveList(ClientNode *Client, UINT DstUin)
{

	int n = 642; // n好像不能太大

	BYTE buf[8192 * 3];
	BYTE *p = buf;
	size_t len;
	Write32(p, Client->Uin); // Uin
	Write32(p, DstUin);		 // DstUin
	Write16(p, 0);			 // Result

	Write16(p, AchieveInfosems.size()); // AchieveNum
	std::map<UINT, AchieveInfo *>::iterator iter;
	for (iter = AchieveInfosems.begin(); iter != AchieveInfosems.end(); iter++)
	{ // Achieves
		AchieveInfo *AchieveInfo = iter->second;
		BYTE *pAchieves = p;
		Write16(pAchieves, 0); // len

		Write16(pAchieves, AchieveInfo->ID);			  // ID  //7937
		Write8(pAchieves, AchieveInfo->ShowType);		  // ShowType
		Write8(pAchieves, AchieveInfo->LogicType);		  // LogicType
		Write8(pAchieves, 3);							  // Status
		Write16(pAchieves, AchieveInfo->TotalProgress);	  // Progress
		Write16(pAchieves, AchieveInfo->TotalProgress);	  // TotalProgress
		Write32(pAchieves, AchieveInfo->FinTime);		  // FinTime
		Write16(pAchieves, AchieveInfo->AchieveValue);	  // AchieveValue
		Write32(pAchieves, AchieveInfo->AchieveFlagType); // AchieveFlagType
		Write32(pAchieves, AchieveInfo->zhanwei);		  // zhanwei

		len = pAchieves - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 373, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestSaveShapeRefit(ClientNode* Client, BYTE* Body, size_t BodyLen) // 316
{
	int RefitItemID[4];
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	int KartItemID = Read32(Body);
	UCHAR RefitItemNum = Read8(Body);
	RefitItemID[0] = Read32(Body);
	RefitItemID[1] = Read32(Body);
	RefitItemID[2] = Read32(Body);
	RefitItemID[3] = Read32(Body);
	int ShapeSuitID = Read32(Body);
	unsigned int ShapeLegendSuitID = 0;
	if (ShapeSuitID == 120246)
	{
		const char* sql = nullptr;
		sqlite3_stmt* stmt = nullptr;
		sql = "SELECT LegendSuitLevelChoice FROM KartRefit WHERE Uin = ? AND KartID=?;";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartItemID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				UINT ShapeIDlist[] = { 118727, 120247, 120248, 118728, 118729 };
				UINT ShapeIDIndex = 0;
				ShapeLegendSuitID = ShapeSuitID;
				switch (sqlite3_column_int(stmt, 0))
				{
				case 2:
					ShapeSuitID = ShapeIDlist[0];
					break;
				case 4:
					ShapeSuitID = ShapeIDlist[1];
					break;
				case 8:
					ShapeSuitID = ShapeIDlist[2];
					break;
				case 16:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 48:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 112:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 128:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 384:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 896:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 1920:
					ShapeSuitID = ShapeIDlist[4];
					break;
				}
			}
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;
	}
	if (ShapeSuitID == 127359)
	{
		const char* sql = nullptr;
		sqlite3_stmt* stmt = nullptr;
		sql = "SELECT LegendSuitLevelChoice FROM KartRefit WHERE Uin = ? AND KartID=?;";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartItemID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				UINT ShapeIDlist[] = { 126533, 126534, 126535, 126536, 126537 };
				UINT ShapeIDIndex = 0;
				ShapeLegendSuitID = ShapeSuitID;
				switch (sqlite3_column_int(stmt, 0))
				{
				case 2:
					ShapeSuitID = ShapeIDlist[0];
					break;
				case 4:
					ShapeSuitID = ShapeIDlist[1];
					break;
				case 8:
					ShapeSuitID = ShapeIDlist[2];
					break;
				case 16:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 48:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 112:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 128:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 384:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 896:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 1920:
					ShapeSuitID = ShapeIDlist[4];
					break;
				}
			}
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;
	}
	if (ShapeSuitID == 127360)
	{
		const char* sql = nullptr;
		sqlite3_stmt* stmt = nullptr;
		sql = "SELECT LegendSuitLevelChoice FROM KartRefit WHERE Uin = ? AND KartID=?;";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartItemID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				UINT ShapeIDlist[] = { 127361, 127362, 127363, 127364, 127365 };
				UINT ShapeIDIndex = 0;
				ShapeLegendSuitID = ShapeSuitID;
				switch (sqlite3_column_int(stmt, 0))
				{
				case 2:
					ShapeSuitID = ShapeIDlist[0];
					break;
				case 4:
					ShapeSuitID = ShapeIDlist[1];
					break;
				case 8:
					ShapeSuitID = ShapeIDlist[2];
					break;
				case 16:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 48:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 112:
					ShapeSuitID = ShapeIDlist[3];
					break;
				case 128:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 384:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 896:
					ShapeSuitID = ShapeIDlist[4];
					break;
				case 1920:
					ShapeSuitID = ShapeIDlist[4];
					break;
				}
			}
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;
	}
	if (ShapeSuitID == 126355)
	{
		ShapeLegendSuitID = 126355;
		ShapeSuitID = 127437;
	}
	else if (ShapeSuitID == 123513)
	{
		ShapeLegendSuitID = 123513;
		ShapeSuitID = 123238;
	}
	else if (ShapeSuitID == 130539)
	{
		ShapeLegendSuitID = 130539;
		ShapeSuitID = 130800;
	}
#ifdef DEBUG
	//Z_INFO("皮肤外表ID:%d|%d|%d|%d\n", RefitItemID[0], RefitItemID[1], RefitItemID[2], RefitItemID[3]);
	//Z_INFO("皮肤外表 - 赛车ID:%d\n 改装个数:%d \n 改装ID:%d|%d|%d|%d \n 皮肤ID: %d \n", KartItemID, RefitItemNum, RefitItemID[0], RefitItemID[1], RefitItemID[2], RefitItemID[3], ShapeSuitID);
#endif
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	if (RefitItemID[0] == 0 && RefitItemID[1] == 0 && RefitItemID[2] == 0 && RefitItemID[3] == 0)
	{
		sql = "UPDATE KartRefit SET ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, ShapeSuitID);
			sqlite3_bind_int(stmt, 2, ShapeLegendSuitID);

			sqlite3_bind_int(stmt, 3, Client->Uin);
			sqlite3_bind_int(stmt, 4, KartItemID);
			result = sqlite3_step(stmt);
			Z_INFO("结果:%d", result);
		}
		else
		{
			printf("皮肤改装错误");
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (ShapeSuitID==0) {
			sql = "UPDATE KartRefit SET KartHeadRefitItemID=?,KartTailRefitItemID=?,KartFlankRefitItemID=?,KartTireRefitItemID=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, RefitItemID[0]);
				sqlite3_bind_int(stmt, 2, RefitItemID[1]);
				sqlite3_bind_int(stmt, 3, RefitItemID[2]);
				sqlite3_bind_int(stmt, 4, RefitItemID[3]);
				sqlite3_bind_int(stmt, 5, ShapeSuitID);
				sqlite3_bind_int(stmt, 6, ShapeLegendSuitID);

				sqlite3_bind_int(stmt, 7, Client->Uin);
				sqlite3_bind_int(stmt, 8, KartItemID);
				result = sqlite3_step(stmt);
				Z_INFO("结果:%d", result);
			}
			else
			{
				printf("皮肤改装错误");
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
	}
	else
	{
		sql = "UPDATE KartRefit SET KartHeadRefitItemID=?,KartTailRefitItemID=?,KartFlankRefitItemID=?,KartTireRefitItemID=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, RefitItemID[0]);
			sqlite3_bind_int(stmt, 2, RefitItemID[1]);
			sqlite3_bind_int(stmt, 3, RefitItemID[2]);
			sqlite3_bind_int(stmt, 4, RefitItemID[3]);
			sqlite3_bind_int(stmt, 5, ShapeSuitID);
			sqlite3_bind_int(stmt, 6, ShapeLegendSuitID);

			sqlite3_bind_int(stmt, 7, Client->Uin);
			sqlite3_bind_int(stmt, 8, KartItemID);
			result = sqlite3_step(stmt);
			Z_INFO("结果:%d", result);
		}
		else
		{
			printf("皮肤改装错误");
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	if (ShapeSuitID == 0)
	{
		ResponseSaveShapeRefit4(Client, KartItemID);
	}
	else
	{
		ResponseSaveShapeRefit(Client, KartItemID, RefitItemNum, RefitItemID, ShapeSuitID);
	}
	if (KartItemID!= Client->KartID) {
		ResponseSaveShapeRefit4(Client, Client->KartID);
	}
	//ResponseSaveShapeRefit(Client, KartItemID, RefitItemNum, RefitItemID, ShapeSuitID);

}
void ResponseSaveShapeRefit(ClientNode* Client, int KartID, int RefitItemNum, int RefitItemID[], int ShapeSuitID)
{
	int suitid = 0;
	suitid = ShapeSuitID;
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;
	UINT LegendSuitLevel = 0;
	UINT LegendSuitLevelChoice = 0;
	UINT ShapeLegendSuitID = 0;
	const char* sql = nullptr;
	sqlite3_stmt* stmt = nullptr;
	sql = "SELECT ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID FROM KartRefit WHERE Uin = ? AND KartID=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			ShapeSuitID = sqlite3_column_int(stmt, 0);
			LegendSuitLevel = sqlite3_column_int(stmt, 1);
			LegendSuitLevelChoice = sqlite3_column_int(stmt, 2);
			ShapeLegendSuitID = sqlite3_column_int(stmt, 3);
		}
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	Write16(p, 0);		// ResultID
	Write8(p, 0);		// ReasonLen
	Write32(p, KartID); // KartItemID
	{					// KartRefitExInfo
		BYTE* pKartRefitExInfo = p;
		Write16(pKartRefitExInfo, 0); // len

		if (ShapeSuitID == 0) {
			if (RefitItemID[0] > 0) {
				Write8(pKartRefitExInfo, 3); // SpeedRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (RefitItemID[1] > 0) {
				Write8(pKartRefitExInfo, 3); // JetRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (RefitItemID[2] > 0) {
				Write8(pKartRefitExInfo, 3); // SJetRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (RefitItemID[3] > 0) {
				Write8(pKartRefitExInfo, 3); // AccuRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}

			if (RefitItemID[0] > 0) {
				Write8(pKartRefitExInfo, 10); // SpeedAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (RefitItemID[1] > 0) {
				Write8(pKartRefitExInfo, 10); // JetAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (RefitItemID[2] > 0) {
				Write8(pKartRefitExInfo, 10); // SJetAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (RefitItemID[3] > 0) {
				Write8(pKartRefitExInfo, 10); // AccuAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
		}
		else {
			Write8(pKartRefitExInfo, 3); // SpeedRefitStar
			Write8(pKartRefitExInfo, 3); // JetRefitStar
			Write8(pKartRefitExInfo, 3); // SJetRefitStar
			Write8(pKartRefitExInfo, 3); // AccuRefitStar
			Write8(pKartRefitExInfo, 10); // SpeedAddRatio
			Write8(pKartRefitExInfo, 10); // JetAddRatio
			Write8(pKartRefitExInfo, 10); // SJetAddRatio
			Write8(pKartRefitExInfo, 10); // AccuAddRatio
		}

		len = pKartRefitExInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, RefitItemNum);	// RefitItemNum
	Write32(p, RefitItemID[0]); // RefitItemID[]
	Write32(p, RefitItemID[1]); // RefitItemID[]
	Write32(p, RefitItemID[2]); // RefitItemID[]
	Write32(p, RefitItemID[3]); // RefitItemID[]

	Write32(p, ShapeSuitID); // ShapeSuitID
	{						 // RefitClientInfo
		BYTE* pRefitClientInfo = p;
		Write16(pRefitClientInfo, 0); // len

		Write32(pRefitClientInfo, Client->Uin);	   // Uin
		Write32(pRefitClientInfo, KartID);		   // KartId
		Write32(pRefitClientInfo, 0);			   // RefitCout
		Write16(pRefitClientInfo, 0);			   // MaxFlags
		Write16(pRefitClientInfo, 0);			   // WWeight
		Write16(pRefitClientInfo, 0);			   // SpeedWeight
		Write16(pRefitClientInfo, 0);			   // JetWeight
		Write16(pRefitClientInfo, 0);			   // SJetWeight
		Write16(pRefitClientInfo, 0);			   // AccuWeight
		Write32(pRefitClientInfo, 0);			   // ShapeRefitCount
		Write32(pRefitClientInfo, RefitItemID[0]); // KartHeadRefitItemID
		Write32(pRefitClientInfo, RefitItemID[1]); // KartTailRefitItemID
		Write32(pRefitClientInfo, RefitItemID[2]); // KartFlankRefitItemID
		Write32(pRefitClientInfo, RefitItemID[3]); // KartTireRefitItemID
		{										   // KartRefitExInfo
			BYTE* pKartRefitExInfo = pRefitClientInfo;
			Write16(pKartRefitExInfo, 0); // len

			if (ShapeSuitID == 0) {
				if (RefitItemID[0] > 0) {
					Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (RefitItemID[1] > 0) {
					Write8(pKartRefitExInfo, 3); // JetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (RefitItemID[2] > 0) {
					Write8(pKartRefitExInfo, 3); // SJetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (RefitItemID[3] > 0) {
					Write8(pKartRefitExInfo, 3); // AccuRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}

				if (RefitItemID[0] > 0) {
					Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (RefitItemID[1] > 0) {
					Write8(pKartRefitExInfo, 10); // JetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (RefitItemID[2] > 0) {
					Write8(pKartRefitExInfo, 10); // SJetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (RefitItemID[3] > 0) {
					Write8(pKartRefitExInfo, 10); // AccuAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
			}
			else {
				Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				Write8(pKartRefitExInfo, 3); // JetRefitStar
				Write8(pKartRefitExInfo, 3); // SJetRefitStar
				Write8(pKartRefitExInfo, 3); // AccuRefitStar
				Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				Write8(pKartRefitExInfo, 10); // JetAddRatio
				Write8(pKartRefitExInfo, 10); // SJetAddRatio
				Write8(pKartRefitExInfo, 10); // AccuAddRatio
			}

			len = pKartRefitExInfo - pRefitClientInfo;
			Set16(pRefitClientInfo, (WORD)len);
			pRefitClientInfo += len;
		}
		Write32(pRefitClientInfo, 0);					  // SecondRefitCount
		Write16(pRefitClientInfo, 0);					  // Speed2Weight
		Write16(pRefitClientInfo, 0);					  // DriftVecWeight
		Write16(pRefitClientInfo, 0);					  // AdditionalZSpeedWeight
		Write16(pRefitClientInfo, 0);					  // AntiCollisionWeight
		Write16(pRefitClientInfo, 0);					  // LuckyValue
		Write16(pRefitClientInfo, 0);					  // RefitLuckyValueMaxWeight
		Write32(pRefitClientInfo, ShapeSuitID);			  // ShapeSuitID
		Write8(pRefitClientInfo, LegendSuitLevel);		  // LegendSuitLevel
		Write32(pRefitClientInfo, LegendSuitLevelChoice); // LegendSuitLevelChoice
		Write32(pRefitClientInfo, ShapeLegendSuitID);	  // ShapeLegendSuitID

		len = pRefitClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 316, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (Client->KartID == KartID)
	{
		ItemStatus* aItemStatus = (ItemStatus*)malloc(sizeof(ItemStatus) * 2);
		if (ShapeSuitID != 0)
		{
			aItemStatus[0].ItemID = KartID;
			aItemStatus[0].NewStatus = 0;
			aItemStatus[1].ItemID = ShapeSuitID;
			aItemStatus[1].NewStatus = 1;
			Client->SkinID = ShapeSuitID;
			Client->OrgKartId = KartID;
			ClientNode* temp = GetClient(Client->Uin);
			temp->SkinID = ShapeSuitID;
			temp->OrgKartId = KartID;
		}
		else
		{
			aItemStatus[0].ItemID = Client->SkinID;
			aItemStatus[0].NewStatus = 0;
			aItemStatus[1].ItemID = KartID;
			aItemStatus[1].NewStatus = 1;
			Client->SkinID = 0;
			Client->OrgKartId = 0;
			ClientNode* temp = GetClient(Client->Uin);
			temp->SkinID = 0;
			temp->OrgKartId = 0;
		}

		RoomNode* Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			// NotifyChangeItemStatus(Client, Client->Uin, 2, aItemStatus);
		}
		else
		{
			for (char i = 0; i < 6; i++)
			{
				ClientNode* RoomClient = Room->Player[i];
				if (RoomClient && RoomClient != Client)
				{
					NotifyChangeItemStatus2(RoomClient, Client->Uin, 2, aItemStatus);
				}
			}
		}
	}
}
void ResponseSaveShapeRefit2(ClientNode *Client, int KartID, int ShapeSuitID)
{
	// 新增皮肤处理
	if (Client->KartID == KartID)
	{
		ItemStatus *aItemStatus = (ItemStatus *)malloc(sizeof(ItemStatus) * 2);
		if (ShapeSuitID != 0)
		{
			aItemStatus[0].ItemID = KartID;
			aItemStatus[0].NewStatus = 0;
			aItemStatus[1].ItemID = ShapeSuitID;
			aItemStatus[1].NewStatus = 1;
			Client->SkinID = ShapeSuitID;
			Client->OrgKartId = KartID;
			ClientNode *temp = GetClient(Client->Uin);
			if (temp)
			{
				temp->SkinID = ShapeSuitID;
				temp->OrgKartId = KartID;
			}
			else
			{
				Client->SkinID = ShapeSuitID;
				Client->OrgKartId = KartID;
			}
		}
		else
		{
			aItemStatus[0].ItemID = Client->SkinID;
			aItemStatus[0].NewStatus = 0;
			aItemStatus[1].ItemID = KartID;
			aItemStatus[1].NewStatus = 1;
			Client->SkinID = 0;
			Client->OrgKartId = 0;
			ClientNode *temp = GetClient(Client->Uin);

			if (temp)
			{
				temp->SkinID = 0;
				temp->OrgKartId = 0;
			}
			else
			{
				Client->SkinID = 0;
				Client->OrgKartId = 0;
			}
		}

		// 到这里皮肤就处理完了
		int suitid = 0;
		suitid = ShapeSuitID;
		BYTE buf[8192];
		BYTE *p = buf;
		size_t len;
		UINT LegendSuitLevel = 0;
		UINT LegendSuitLevelChoice = 0;
		UINT ShapeLegendSuitID = 0;
		UINT KartHeadRefitItemID = 0;
		UINT KartTailRefitItemID = 0;
		UINT KartFlankRefitItemID = 0;
		UINT KartTireRefitItemID = 0;
		const char *sql = nullptr;
		sqlite3_stmt *stmt = nullptr;
		sql = "SELECT ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID FROM KartRefit WHERE Uin = ? AND KartID=?;";
		if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, KartID);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				ShapeSuitID = sqlite3_column_int(stmt, 0);
				LegendSuitLevel = sqlite3_column_int(stmt, 1);
				LegendSuitLevelChoice = sqlite3_column_int(stmt, 2);
				ShapeLegendSuitID = sqlite3_column_int(stmt, 3);
				KartHeadRefitItemID = sqlite3_column_int(stmt, 4);
				KartTailRefitItemID = sqlite3_column_int(stmt, 5);
				KartFlankRefitItemID = sqlite3_column_int(stmt, 6);
				KartTireRefitItemID = sqlite3_column_int(stmt, 7);
			}
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;
		Write16(p, 0);		// ResultID
		Write8(p, 0);		// ReasonLen
		Write32(p, KartID); // KartItemID
		{					// KartRefitExInfo
			BYTE *pKartRefitExInfo = p;
			Write16(pKartRefitExInfo, 0); // len

			if (ShapeSuitID == 0) {
				if (KartHeadRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTailRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // JetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartFlankRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // SJetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTireRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // AccuRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}

				if (KartHeadRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTailRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // JetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartFlankRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // SJetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTireRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // AccuAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
			}
			else {
				Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				Write8(pKartRefitExInfo, 3); // JetRefitStar
				Write8(pKartRefitExInfo, 3); // SJetRefitStar
				Write8(pKartRefitExInfo, 3); // AccuRefitStar
				Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				Write8(pKartRefitExInfo, 10); // JetAddRatio
				Write8(pKartRefitExInfo, 10); // SJetAddRatio
				Write8(pKartRefitExInfo, 10); // AccuAddRatio
			}

			len = pKartRefitExInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 4);  // RefitItemNum
		Write32(p, 0); // RefitItemID[]
		Write32(p, 0); // RefitItemID[]
		Write32(p, 0); // RefitItemID[]
		Write32(p, 0); // RefitItemID[]

		Write32(p, ShapeSuitID); // ShapeSuitID
		{						 // RefitClientInfo
			BYTE *pRefitClientInfo = p;
			Write16(pRefitClientInfo, 0); // len

			Write32(pRefitClientInfo, Client->Uin);			 // Uin
			Write32(pRefitClientInfo, KartID);				 // KartId
			Write32(pRefitClientInfo, 0);					 // RefitCout
			Write16(pRefitClientInfo, 0);					 // MaxFlags
			Write16(pRefitClientInfo, 0);					 // WWeight
			Write16(pRefitClientInfo, 0);					 // SpeedWeight
			Write16(pRefitClientInfo, 0);					 // JetWeight
			Write16(pRefitClientInfo, 0);					 // SJetWeight
			Write16(pRefitClientInfo, 0);					 // AccuWeight
			Write32(pRefitClientInfo, 0);					 // ShapeRefitCount
			Write32(pRefitClientInfo, KartHeadRefitItemID);	 // KartHeadRefitItemID
			Write32(pRefitClientInfo, KartTailRefitItemID);	 // KartTailRefitItemID
			Write32(pRefitClientInfo, KartFlankRefitItemID); // KartFlankRefitItemID
			Write32(pRefitClientInfo, KartTireRefitItemID);	 // KartTireRefitItemID
			{												 // KartRefitExInfo
				BYTE *pKartRefitExInfo = pRefitClientInfo;
				Write16(pKartRefitExInfo, 0); // len

				if (ShapeSuitID == 0) {
					if (KartHeadRefitItemID > 0) {
						Write8(pKartRefitExInfo, 3); // SpeedRefitStar
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}
					if (KartTailRefitItemID > 0) {
						Write8(pKartRefitExInfo, 3); // JetRefitStar
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}
					if (KartFlankRefitItemID > 0) {
						Write8(pKartRefitExInfo, 3); // SJetRefitStar
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}
					if (KartTireRefitItemID > 0) {
						Write8(pKartRefitExInfo, 3); // AccuRefitStar
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}

					if (KartHeadRefitItemID > 0) {
						Write8(pKartRefitExInfo, 10); // SpeedAddRatio
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}
					if (KartTailRefitItemID > 0) {
						Write8(pKartRefitExInfo, 10); // JetAddRatio
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}
					if (KartFlankRefitItemID > 0) {
						Write8(pKartRefitExInfo, 10); // SJetAddRatio
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}
					if (KartTireRefitItemID > 0) {
						Write8(pKartRefitExInfo, 10); // AccuAddRatio
					}
					else {
						Write8(pKartRefitExInfo, 0);
					}
				}
				else {
					Write8(pKartRefitExInfo, 3); // SpeedRefitStar
					Write8(pKartRefitExInfo, 3); // JetRefitStar
					Write8(pKartRefitExInfo, 3); // SJetRefitStar
					Write8(pKartRefitExInfo, 3); // AccuRefitStar
					Write8(pKartRefitExInfo, 10); // SpeedAddRatio
					Write8(pKartRefitExInfo, 10); // JetAddRatio
					Write8(pKartRefitExInfo, 10); // SJetAddRatio
					Write8(pKartRefitExInfo, 10); // AccuAddRatio
				}

				len = pKartRefitExInfo - pRefitClientInfo;
				Set16(pRefitClientInfo, (WORD)len);
				pRefitClientInfo += len;
			}
			Write32(pRefitClientInfo, 0);					  // SecondRefitCount
			Write16(pRefitClientInfo, 0);					  // Speed2Weight
			Write16(pRefitClientInfo, 0);					  // DriftVecWeight
			Write16(pRefitClientInfo, 0);					  // AdditionalZSpeedWeight
			Write16(pRefitClientInfo, 0);					  // AntiCollisionWeight
			Write16(pRefitClientInfo, 0);					  // LuckyValue
			Write16(pRefitClientInfo, 0);					  // RefitLuckyValueMaxWeight
			Write32(pRefitClientInfo, ShapeSuitID);			  // ShapeSuitID
			Write8(pRefitClientInfo, LegendSuitLevel);		  // LegendSuitLevel
			Write32(pRefitClientInfo, LegendSuitLevelChoice); // LegendSuitLevelChoice
			Write32(pRefitClientInfo, ShapeLegendSuitID);	  // ShapeLegendSuitID

			len = pRefitClientInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

		len = p - buf;
		SendToClient(Client, 316, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
		RoomNode *Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			// NotifyChangeItemStatus(Client, Client->Uin, 2, aItemStatus);
		}
		else
		{
			for (char i = 0; i < 6; i++)
			{
				ClientNode *RoomClient = Room->Player[i];
				if (RoomClient && RoomClient != Client)
				{
					NotifyChangeItemStatus2(RoomClient, Client->Uin, 2, aItemStatus);
				}
			}
		}
	}
}
void ResponseSaveShapeRefit4(ClientNode *Client, int KartID)
{
	int ShapeSuitID = 0;
	int suitid = 0;
	suitid = ShapeSuitID;
	UINT LegendSuitLevel = 0;
	UINT LegendSuitLevelChoice = 0;
	UINT ShapeLegendSuitID = 0;
	UINT KartHeadRefitItemID = 0;
	UINT KartTailRefitItemID = 0;
	UINT KartFlankRefitItemID = 0;
	UINT KartTireRefitItemID = 0;
	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;
	sql = "SELECT ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID FROM KartRefit WHERE Uin = ? AND KartID=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			ShapeSuitID = sqlite3_column_int(stmt, 0);
			LegendSuitLevel = sqlite3_column_int(stmt, 1);
			LegendSuitLevelChoice = sqlite3_column_int(stmt, 2);
			ShapeLegendSuitID = sqlite3_column_int(stmt, 3);
			KartHeadRefitItemID = sqlite3_column_int(stmt, 4);
			KartTailRefitItemID = sqlite3_column_int(stmt, 5);
			KartFlankRefitItemID = sqlite3_column_int(stmt, 6);
			KartTireRefitItemID = sqlite3_column_int(stmt, 7);
		}
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	/*if (ShapeSuitID > 0) {
		return;
	}*/
	ItemStatus *aItemStatus = (ItemStatus *)malloc(sizeof(ItemStatus) * 1);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);		// ResultID
	Write8(p, 0);		// ReasonLen
	Write32(p, KartID); // KartItemID
	{					// KartRefitExInfo
		BYTE *pKartRefitExInfo = p;
		Write16(pKartRefitExInfo, 0); // len

		if (ShapeSuitID == 0) {
			if (KartHeadRefitItemID > 0) {
				Write8(pKartRefitExInfo, 3); // SpeedRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (KartTailRefitItemID > 0) {
				Write8(pKartRefitExInfo, 3); // JetRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (KartFlankRefitItemID > 0) {
				Write8(pKartRefitExInfo, 3); // SJetRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (KartTireRefitItemID > 0) {
				Write8(pKartRefitExInfo, 3); // AccuRefitStar
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}

			if (KartHeadRefitItemID > 0) {
				Write8(pKartRefitExInfo, 10); // SpeedAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (KartTailRefitItemID > 0) {
				Write8(pKartRefitExInfo, 10); // JetAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (KartFlankRefitItemID > 0) {
				Write8(pKartRefitExInfo, 10); // SJetAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
			if (KartTireRefitItemID > 0) {
				Write8(pKartRefitExInfo, 10); // AccuAddRatio
			}
			else {
				Write8(pKartRefitExInfo, 0);
			}
		}
		else {
			Write8(pKartRefitExInfo, 3); // SpeedRefitStar
			Write8(pKartRefitExInfo, 3); // JetRefitStar
			Write8(pKartRefitExInfo, 3); // SJetRefitStar
			Write8(pKartRefitExInfo, 3); // AccuRefitStar
			Write8(pKartRefitExInfo, 10); // SpeedAddRatio
			Write8(pKartRefitExInfo, 10); // JetAddRatio
			Write8(pKartRefitExInfo, 10); // SJetAddRatio
			Write8(pKartRefitExInfo, 10); // AccuAddRatio
		}

		len = pKartRefitExInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 4);					  // RefitItemNum
	Write32(p, KartHeadRefitItemID);  // RefitItemID[]
	Write32(p, KartTailRefitItemID);  // RefitItemID[]
	Write32(p, KartFlankRefitItemID); // RefitItemID[]
	Write32(p, KartTireRefitItemID);  // RefitItemID[]

	Write32(p, ShapeSuitID); // ShapeSuitID
	{						 // RefitClientInfo
		BYTE *pRefitClientInfo = p;
		Write16(pRefitClientInfo, 0); // len

		Write32(pRefitClientInfo, Client->Uin);			 // Uin
		Write32(pRefitClientInfo, KartID);				 // KartId
		Write32(pRefitClientInfo, 0);					 // RefitCout
		Write16(pRefitClientInfo, 0);					 // MaxFlags
		Write16(pRefitClientInfo, 0);					 // WWeight
		Write16(pRefitClientInfo, 0);					 // SpeedWeight
		Write16(pRefitClientInfo, 0);					 // JetWeight
		Write16(pRefitClientInfo, 0);					 // SJetWeight
		Write16(pRefitClientInfo, 0);					 // AccuWeight
		Write32(pRefitClientInfo, 0);					 // ShapeRefitCount
		Write32(pRefitClientInfo, KartHeadRefitItemID);	 // KartHeadRefitItemID
		Write32(pRefitClientInfo, KartTailRefitItemID);	 // KartTailRefitItemID
		Write32(pRefitClientInfo, KartFlankRefitItemID); // KartFlankRefitItemID
		Write32(pRefitClientInfo, KartTireRefitItemID);	 // KartTireRefitItemID
		{												 // KartRefitExInfo
			BYTE *pKartRefitExInfo = pRefitClientInfo;
			Write16(pKartRefitExInfo, 0); // len

			if (ShapeSuitID == 0) {
				if (KartHeadRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTailRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // JetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartFlankRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // SJetRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTireRefitItemID > 0) {
					Write8(pKartRefitExInfo, 3); // AccuRefitStar
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}

				if (KartHeadRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTailRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // JetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartFlankRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // SJetAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
				if (KartTireRefitItemID > 0) {
					Write8(pKartRefitExInfo, 10); // AccuAddRatio
				}
				else {
					Write8(pKartRefitExInfo, 0);
				}
			}
			else {
				Write8(pKartRefitExInfo, 3); // SpeedRefitStar
				Write8(pKartRefitExInfo, 3); // JetRefitStar
				Write8(pKartRefitExInfo, 3); // SJetRefitStar
				Write8(pKartRefitExInfo, 3); // AccuRefitStar
				Write8(pKartRefitExInfo, 10); // SpeedAddRatio
				Write8(pKartRefitExInfo, 10); // JetAddRatio
				Write8(pKartRefitExInfo, 10); // SJetAddRatio
				Write8(pKartRefitExInfo, 10); // AccuAddRatio
			}

			len = pKartRefitExInfo - pRefitClientInfo;
			Set16(pRefitClientInfo, (WORD)len);
			pRefitClientInfo += len;
		}
		Write32(pRefitClientInfo, 0);					  // SecondRefitCount
		Write16(pRefitClientInfo, 0);					  // Speed2Weight
		Write16(pRefitClientInfo, 0);					  // DriftVecWeight
		Write16(pRefitClientInfo, 0);					  // AdditionalZSpeedWeight
		Write16(pRefitClientInfo, 0);					  // AntiCollisionWeight
		Write16(pRefitClientInfo, 0);					  // LuckyValue
		Write16(pRefitClientInfo, 0);					  // RefitLuckyValueMaxWeight
		Write32(pRefitClientInfo, ShapeSuitID);			  // ShapeSuitID
		Write8(pRefitClientInfo, LegendSuitLevel);		  // LegendSuitLevel
		Write32(pRefitClientInfo, LegendSuitLevelChoice); // LegendSuitLevelChoice
		Write32(pRefitClientInfo, ShapeLegendSuitID);	  // ShapeLegendSuitID

		len = pRefitClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 316, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	RoomNode *Room = GetRoom(Client->RoomID);
	aItemStatus[0].ItemID = KartID;
	if (ShapeSuitID == 0)
	{
		Client->SkinID = 0;
		aItemStatus[0].ItemID = KartID;
	}
	else
	{
		Client->OrgKartId = KartID;
		Client->SkinID = ShapeSuitID;
		aItemStatus[0].ItemID = ShapeSuitID;
	}

	aItemStatus[0].NewStatus = 1;

	if (!Room)
	{
		// NotifyChangeItemStatus(Client, Client->Uin, 2, aItemStatus);
	}
	else
	{
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyChangeItemStatus(RoomClient, Client->Uin, 1, aItemStatus);
				// ResponseSaveShapeRefit4(RoomClient, KartID);
			}
		}
	}
}
void ResponseSaveShapeRefit3(ClientNode *Client, int KartID, int ShapeSuitID)
{
	// 新增皮肤处理
	if (Client->KartID == KartID)
	{
		ItemStatus *aItemStatus = (ItemStatus *)malloc(sizeof(ItemStatus) * 2);
		if (ShapeSuitID != 0)
		{
			aItemStatus[0].ItemID = KartID;
			aItemStatus[0].NewStatus = 0;
			aItemStatus[1].ItemID = ShapeSuitID;
			aItemStatus[1].NewStatus = 1;
			Client->SkinID = ShapeSuitID;
			Client->OrgKartId = KartID;
			ClientNode *temp = GetClient(Client->Uin);
			if (temp)
			{
				temp->SkinID = ShapeSuitID;
				temp->OrgKartId = KartID;
			}
			else
			{
				Client->SkinID = ShapeSuitID;
				Client->OrgKartId = KartID;
			}
		}
		else
		{
			aItemStatus[0].ItemID = Client->SkinID;
			aItemStatus[0].NewStatus = 0;
			aItemStatus[1].ItemID = KartID;
			aItemStatus[1].NewStatus = 1;
			Client->SkinID = 0;
			Client->OrgKartId = 0;
			ClientNode *temp = GetClient(Client->Uin);

			if (temp)
			{
				temp->SkinID = 0;
				temp->OrgKartId = 0;
			}
			else
			{
				Client->SkinID = 0;
				Client->OrgKartId = 0;
			}
		}

		RoomNode *Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			// NotifyChangeItemStatus(Client, Client->Uin, 2, aItemStatus);
		}
		else
		{
			for (char i = 0; i < 6; i++)
			{
				ClientNode *RoomClient = Room->Player[i];
				if (RoomClient && RoomClient != Client)
				{
					NotifyChangeItemStatus(RoomClient, Client->Uin, 2, aItemStatus);
				}
			}
		}
	}
	// 到这里皮肤就处理完了
	int suitid = 0;
	suitid = ShapeSuitID;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT LegendSuitLevel = 0;
	UINT LegendSuitLevelChoice = 0;
	UINT ShapeLegendSuitID = 0;
	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;
	/*sql = "SELECT ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID FROM KartRefit WHERE Uin = ? AND KartID=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			ShapeSuitID = sqlite3_column_int(stmt, 0);
			LegendSuitLevel = sqlite3_column_int(stmt, 1);
			LegendSuitLevelChoice = sqlite3_column_int(stmt, 2);
			ShapeLegendSuitID = sqlite3_column_int(stmt, 3);
		}
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;*/
	int result;
	sql = "UPDATE KartRefit SET ShapeSuitID=? WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, ShapeSuitID);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		sqlite3_bind_int(stmt, 3, KartID);
		result = sqlite3_step(stmt);
		Z_INFO("结果:%d", result);
	}
	else
	{
		Z_ERROR("皮肤改装错误");
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write16(p, 0);		// ResultID
	Write8(p, 0);		// ReasonLen
	Write32(p, KartID); // KartItemID
	{					// KartRefitExInfo
		BYTE *pKartRefitExInfo = p;
		Write16(pKartRefitExInfo, 0); // len

		Write8(pKartRefitExInfo, 0); // SpeedRefitStar
		Write8(pKartRefitExInfo, 0); // JetRefitStar
		Write8(pKartRefitExInfo, 0); // SJetRefitStar
		Write8(pKartRefitExInfo, 0); // AccuRefitStar
		Write8(pKartRefitExInfo, 0); // SpeedAddRatio
		Write8(pKartRefitExInfo, 0); // JetAddRatio
		Write8(pKartRefitExInfo, 0); // SJetAddRatio
		Write8(pKartRefitExInfo, 0); // AccuAddRatio

		len = pKartRefitExInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 4);  // RefitItemNum
	Write32(p, 0); // RefitItemID[]
	Write32(p, 0); // RefitItemID[]
	Write32(p, 0); // RefitItemID[]
	Write32(p, 0); // RefitItemID[]

	Write32(p, ShapeSuitID); // ShapeSuitID
	{						 // RefitClientInfo
		BYTE *pRefitClientInfo = p;
		Write16(pRefitClientInfo, 0); // len

		Write32(pRefitClientInfo, Client->Uin); // Uin
		Write32(pRefitClientInfo, KartID);		// KartId
		Write32(pRefitClientInfo, 0);			// RefitCout
		Write16(pRefitClientInfo, 0);			// MaxFlags
		Write16(pRefitClientInfo, 0);			// WWeight
		Write16(pRefitClientInfo, 0);			// SpeedWeight
		Write16(pRefitClientInfo, 0);			// JetWeight
		Write16(pRefitClientInfo, 0);			// SJetWeight
		Write16(pRefitClientInfo, 0);			// AccuWeight
		Write32(pRefitClientInfo, 0);			// ShapeRefitCount
		Write32(pRefitClientInfo, 0);			// KartHeadRefitItemID
		Write32(pRefitClientInfo, 0);			// KartTailRefitItemID
		Write32(pRefitClientInfo, 0);			// KartFlankRefitItemID
		Write32(pRefitClientInfo, 0);			// KartTireRefitItemID
		{										// KartRefitExInfo
			BYTE *pKartRefitExInfo = pRefitClientInfo;
			Write16(pKartRefitExInfo, 0); // len

			Write8(pKartRefitExInfo, 0); // SpeedRefitStar
			Write8(pKartRefitExInfo, 0); // JetRefitStar
			Write8(pKartRefitExInfo, 0); // SJetRefitStar
			Write8(pKartRefitExInfo, 0); // AccuRefitStar
			Write8(pKartRefitExInfo, 0); // SpeedAddRatio
			Write8(pKartRefitExInfo, 0); // JetAddRatio
			Write8(pKartRefitExInfo, 0); // SJetAddRatio
			Write8(pKartRefitExInfo, 0); // AccuAddRatio

			len = pKartRefitExInfo - pRefitClientInfo;
			Set16(pRefitClientInfo, (WORD)len);
			pRefitClientInfo += len;
		}
		Write32(pRefitClientInfo, 0);					  // SecondRefitCount
		Write16(pRefitClientInfo, 0);					  // Speed2Weight
		Write16(pRefitClientInfo, 0);					  // DriftVecWeight
		Write16(pRefitClientInfo, 0);					  // AdditionalZSpeedWeight
		Write16(pRefitClientInfo, 0);					  // AntiCollisionWeight
		Write16(pRefitClientInfo, 0);					  // LuckyValue
		Write16(pRefitClientInfo, 0);					  // RefitLuckyValueMaxWeight
		Write32(pRefitClientInfo, ShapeSuitID);			  // ShapeSuitID
		Write8(pRefitClientInfo, LegendSuitLevel);		  // LegendSuitLevel
		Write32(pRefitClientInfo, LegendSuitLevelChoice); // LegendSuitLevelChoice
		Write32(pRefitClientInfo, ShapeLegendSuitID);	  // ShapeLegendSuitID

		len = pRefitClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 316, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NoitfyDobagStatus(ClientNode *Client)
{
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "SELECT ItemID,DoBagStatus FROM Item WHERE Uin = ? and DoBagStatus>0";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{

			DoBagItems[Client->Uin][sqlite3_column_int(stmt, 0)] = new DoBagItem{sqlite3_column_int(stmt, 0), (USHORT)sqlite3_column_int(stmt, 1)};
		}
	}
}
void RequestDoBagItemShowOperate(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	USHORT OptType = Read16(Body);	// 7上2 下 5 放在最后 6 放在最后 4 下移一味 3 上移
 	USHORT ShowType = Read16(Body); // 1
	USHORT TabType = Read16(Body);	// 0  Tab
	int ItemID = Read32(Body);		// 124679 置顶道具ID
	int ItemNum = Read32(Body);
	// int[] ItemID[] = Read32(p);

	int *ItemIDList = new int[ItemNum];
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	if (ItemID > 0)
	{
		if (OptType == 7)
		{
			DoBagItem *Temp = DoBagItems[Client->Uin][ItemID];
			if (!Temp)
			{
				const char *sql = NULL;
				sqlite3_stmt *stmt = NULL;
				int result;
				sql = "update Item set DoBagStatus = ? where Uin = ? and ItemID = ?";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ShowType);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					sqlite3_bind_int(stmt, 3, ItemID);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				DoBagItems[Client->Uin][ItemID] = new DoBagItem{ItemID, ShowType};
			}
			else
			{
				Temp->Status += ShowType;
				const char *sql = NULL;
				sqlite3_stmt *stmt = NULL;
				int result;
				sql = "update Item set DoBagStatus = ? where Uin = ? and ItemID = ?";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Temp->Status);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					sqlite3_bind_int(stmt, 3, ItemID);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
		}
		else if (OptType == 2)
		{
			DoBagItem *Temp = DoBagItems[Client->Uin][ItemID];
			if (Temp)
			{
				Temp->Status -= ShowType;
				const char *sql = NULL;
				sqlite3_stmt *stmt = NULL;
				int result;
				sql = "update Item set DoBagStatus = ? where Uin = ? and ItemID = ?";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Temp->Status);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					sqlite3_bind_int(stmt, 3, ItemID);
					result = sqlite3_step(stmt);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
		}
	}
	std::map<UINT, DoBagItem *> ClientDoBag = DoBagItems[Client->Uin];
	std::vector<UINT> StarItem;
	std::vector<UINT> TopItem;
	std::map<UINT, DoBagItem *>::iterator iter;
	for (iter = ClientDoBag.begin(); iter != ClientDoBag.end(); iter++)
	{
		DoBagItem *Temp = iter->second;
		if (Temp->Status == 1)
		{
			TopItem.push_back(Temp->IitemID);
		}
		else if (Temp->Status == 2)
		{
			StarItem.push_back(Temp->IitemID);
		}
		else if (Temp->Status == 3)
		{
			TopItem.push_back(Temp->IitemID);
			StarItem.push_back(Temp->IitemID);
		}
	}
	Write16(p, 0);		  // ResultID
	Write32(p, Uin);	  // Uin
	Write16(p, OptType);  // OptType
	Write16(p, ShowType); // ShowType
	Write16(p, TabType);  // TabType
	Write32(p, 1);		  // HasBagItemShowInfo 有背包道具显示信息

	for (size_t i = 0; i < 1; i++)
	{ // BagItemShowInfo
		BYTE *pBagItemShowInfo = p;
		Write16(pBagItemShowInfo, 0); // len

		Write32(pBagItemShowInfo, 6); // TabNum
		for (size_t i = 0; i < 6; i++)
		{ // BagTabItemTopInfo
			BYTE *pBagTabItemTopInfo = pBagItemShowInfo;
			Write16(pBagTabItemTopInfo, 0); // len

			Write32(pBagTabItemTopInfo, TopItem.size()); // ItemNum
			// Write32(pBagTabItemTopInfo, 0); //ItemID[]
			for (size_t i = 0; i < TopItem.size(); i++)
			{
				Write32(pBagTabItemTopInfo, TopItem[i]); // ItemID[]
			}
			len = pBagTabItemTopInfo - pBagItemShowInfo;
			Set16(pBagItemShowInfo, (WORD)len);
			pBagItemShowInfo += len;
		}

		Write32(pBagItemShowInfo, StarItem.size()); // StarItemNum  星形道具数量
		for (size_t i = 0; i < StarItem.size(); i++)
		{
			Write32(pBagItemShowInfo, StarItem[i]);
		}
		// StarItemID[] 星形道具ID列表
		Write32(pBagItemShowInfo, 9999); // TabTopItemNumMax 选项卡置顶道具数最大值
		Write32(pBagItemShowInfo, 9999); // StarItemNumMax 星形项目数最大值

		len = pBagItemShowInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0);					// Para0
	Write32(p, ClientDoBag.size()); // ItemNum
	Write32(p, 0);					// ItemID[]

	len = p - buf;
	SendToClient(Client, 28412, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	if (OptType==5|| OptType == 6 || OptType == 4 || OptType == 3) {
		if (GetItemType(ItemID) == EAIT_PET && ShowType == 2)
		{ // 星标宠物进化
			NotifyPetEvolution(Client);
		}
		if (GetItemMess(ItemID)== "车 头")
		{ // 星标宠物进化
			if (OptType == 5 || OptType == 6) {
				ItemID = 0;
			}
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "UPDATE KartRefit SET KartHeadRefitItemID=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ItemID);
				sqlite3_bind_int(stmt, 2, 0);
				sqlite3_bind_int(stmt, 3, 0);
				sqlite3_bind_int(stmt, 4, Client->Uin);
				sqlite3_bind_int(stmt, 5, Client->KartID);
				result = sqlite3_step(stmt);
				Z_INFO("结果:%d", result);
			}
			else
			{
				Z_ERROR("皮肤改装错误");
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseSaveShapeRefit4(Client, Client->KartID);
		}
		if (GetItemMess(ItemID) == "车 尾")
		{ // 星标宠物进化
			if (OptType == 5 || OptType == 6) {
				ItemID = 0;
			}
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "UPDATE KartRefit SET KartTailRefitItemID=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ItemID);
				sqlite3_bind_int(stmt, 2, 0);
				sqlite3_bind_int(stmt, 3, 0);
				sqlite3_bind_int(stmt, 4, Client->Uin);
				sqlite3_bind_int(stmt, 5, Client->KartID);
				result = sqlite3_step(stmt);
				Z_INFO("结果:%d", result);
			}
			else
			{
				Z_ERROR("皮肤改装错误");
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseSaveShapeRefit4(Client, Client->KartID);
		}
		if (GetItemMess(ItemID) == "车 翼")
		{ // 星标宠物进化
			if (OptType == 5 || OptType == 6) {
				ItemID = 0;
			}
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "UPDATE KartRefit SET KartFlankRefitItemID=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ItemID);
				sqlite3_bind_int(stmt, 2, 0);
				sqlite3_bind_int(stmt, 3, 0);
				sqlite3_bind_int(stmt, 4, Client->Uin);
				sqlite3_bind_int(stmt, 5, Client->KartID);
				result = sqlite3_step(stmt);
				Z_INFO("结果:%d", result);
			}
			else
			{
				Z_ERROR("皮肤改装错误");
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseSaveShapeRefit4(Client, Client->KartID);
		}
		if (GetItemMess(ItemID) == "车 轮")
		{ // 星标宠物进化
			if (OptType == 5 || OptType == 6) {
				ItemID = 0;
			}
			const char* sql = NULL;
			sqlite3_stmt* stmt = NULL;
			int result;
			sql = "UPDATE KartRefit SET KartTireRefitItemID=?,ShapeSuitID=?,ShapeLegendSuitID=? WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, ItemID);
				sqlite3_bind_int(stmt, 2, 0);
				sqlite3_bind_int(stmt, 3, 0);
				sqlite3_bind_int(stmt, 4, Client->Uin);
				sqlite3_bind_int(stmt, 5, Client->KartID);
				result = sqlite3_step(stmt);
				Z_INFO("结果:%d", result);
			}
			else
			{
				Z_ERROR("皮肤改装错误");
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseSaveShapeRefit4(Client, Client->KartID);
		}
	}
	
}

void RequestCheckName(ClientNode *Client, BYTE *Body, DWORD BodyLen)
{
	UINT Uin = Read32(Body);  // Uni
	UINT Time = Read32(Body); // Time
	char *NewNickName = (char *)Body;
	sqlite3_stmt *stmt;
	if (sqlite3_prepare_v2(PlayerDB, "Select * From BaseInfo Where NickName = ?", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_text(stmt, 1, NewNickName, strlen(NewNickName), SQLITE_TRANSIENT);

		if (sqlite3_step(stmt) != SQLITE_ROW)
		{
			ResponseCheckName(Client, NewNickName, "恭喜你可以使用这个名字！");
		}
		else
		{
			ResponseCheckName(Client, NewNickName, "名称已被使用！");
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
}
void ResponseCheckName(ClientNode *Client, const char *NewNickName, const char *Reason)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len = 0;
	Write16(p, 0); // ResultID

	memcpy(p, NewNickName, MaxNickName); // m_szNickName[]
	p += MaxNickName;

	len = strlen(Reason);
	Write8(p, (BYTE)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;

	len = p - buf;
	SendToClient(Client, 133, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}

void RequestModifyNickname(ClientNode *Client, BYTE *Body, DWORD BodyLen)
{
	std::string Reason = "";

	UINT Uin = Read32(Body);  // Uin
	UINT Time = Read32(Body); // Time

	char *NewModifyNickName = ReadString(Body);
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "SELECT COUNT(*) FROM Item WHERE Uin=? AND ItemID=10595;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		result = sqlite3_step(stmt);

		if (result == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0)
		{

			int ModifyNameCard = 10595; // ID ModifyCard
			int ItemNum = PlayerDB_GetItemNum(Uin, ModifyNameCard);
			PlayerDB_UseItem(Client->Uin, ModifyNameCard, 1);			 //- Item Database
			NotifyUseItemResult(Client, ModifyNameCard, 1, ItemNum - 1); // - Item RealTime InGame

			sql = "SELECT COUNT(*) FROM BaseInfo WHERE NickName=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_text(stmt, 1, NewModifyNickName, strlen(NewModifyNickName), SQLITE_TRANSIENT);
				result = sqlite3_step(stmt);

				if (result == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0)
				{
					// New nickname already exists in database
					Reason = "456";
					return;
				}
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sql = "UPDATE BaseInfo SET NickName=?  WHERE Uin=? ;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_text(stmt, 1, NewModifyNickName, strlen(NewModifyNickName), SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 2, Client->Uin);
				result = sqlite3_step(stmt);
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sql = "UPDATE EngageInfo SET NickName1=?  WHERE EngageUin1=? ;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_text(stmt, 1, NewModifyNickName, strlen(NewModifyNickName), SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 2, Client->Uin);
				result = sqlite3_step(stmt);
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sql = "UPDATE EngageInfo SET NickName2=?  WHERE EngageUin2=? ;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_text(stmt, 1, NewModifyNickName, strlen(NewModifyNickName), SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 2, Client->Uin);
				result = sqlite3_step(stmt);
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}

			sqlite3_finalize(stmt);
			stmt = NULL;
			ResponseModifyNickname(Client, NewModifyNickName, "重命名成功！");
			return;
		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}

	// If Dont Have ModifyCar
	Reason = "123";
}
void ResponseModifyNickname(ClientNode *Client, const char *NewModifyNickName, const char *Reason)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 0); // ResultID

	memcpy(p, NewModifyNickName, MaxNickName); // m_szNickName[]
	p += MaxNickName;

	len = strlen(Reason);
	Write8(p, (BYTE)len); // ReasonLen
	memcpy(p, Reason, len);
	p += len;

	len = p - buf;
	SendToClient(Client, 154, buf, len, Client->RoomID, FE_ROOM, Client->ConnID, Response);
}

void NotifyClientUpdateMoneyExp(ClientNode *Client)
{ // Update Character Info
	BYTE buf[8192]{};
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	{						 // CurMoneyExp
		BYTE *pCurMoneyExp = p;
		Write16(pCurMoneyExp, 0); // len

		Write32(pCurMoneyExp, Client->Money);		// Money
		Write32(pCurMoneyExp, Client->SuperMoney);	// SuperMoney
		Write32(pCurMoneyExp, Client->Experience);	// Experience
		Write32(pCurMoneyExp, Client->Charm);		// Charm
		Write32(pCurMoneyExp, Client->DanceExp);	// DanceExp
		Write32(pCurMoneyExp, Client->Coupons);		// Coupons
		Write32(pCurMoneyExp, Client->Admiration);	// Admiration
		Write32(pCurMoneyExp, Client->LuckMoney);	// LuckMoney
		Write32(pCurMoneyExp, Client->TeamWorkExp); // TeamWorkExp
		Write32(pCurMoneyExp, AchievePointSum);		// AchievePoint
#ifndef ZingSpeed
		Write32(pCurMoneyExp, 0); // ConsumeScore
		Write32(pCurMoneyExp, 0); // HistoryConsumeScore
		Write32(pCurMoneyExp, 0); // LadderMatchAchievePoint
		Write32(pCurMoneyExp, 0); // SpeedBean
		Write32(pCurMoneyExp, 0); // SpeedCoin
#endif							  // !ZingSpeed

		len = pCurMoneyExp - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // Reason

	len = p - buf;
	SendToClient(Client, 559, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdateMoney(ClientNode *Client, int SuperMoney, int Coupons, int LuckMoney, UINT SpeedCoin)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	{						 // CurrentMoney
		BYTE *pCurrentMoney = p;
		Write16(pCurrentMoney, 0); // len

		Write32(pCurrentMoney, 0);			// Money
		Write32(pCurrentMoney, SuperMoney); // SuperMoney
		Write32(pCurrentMoney, Coupons);	// Coupons
		Write32(pCurrentMoney, LuckMoney);	// LuckMoney
		Write32(pCurrentMoney, 0);			// ConsumeScore
		Write32(pCurrentMoney, 0);			// HistoryConsumeScore
		Write32(pCurrentMoney, SpeedCoin);	// SpeedCoin

		len = pCurrentMoney - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // IncMoney
		BYTE *pIncMoney = p;
		Write16(pIncMoney, 0); // len

		Write32(pIncMoney, 0);			// Money
		Write32(pIncMoney, SuperMoney); // SuperMoney
		Write32(pIncMoney, Coupons);	// Coupons
		Write32(pIncMoney, LuckMoney);	// LuckMoney
		Write32(pIncMoney, 0);			// ConsumeScore
		Write32(pIncMoney, 0);			// HistoryConsumeScore
		Write32(pIncMoney, SpeedCoin);	// SpeedCoin

		len = pIncMoney - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // Flag

	len = p - buf;
	SendToClient(Client, 528, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyClientUpdateMoney2(ClientNode *Client)
{
	// 更新客户端货币消息
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	DWORD Uin = Client->Uin;
	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;
	UINT Money = 0;
	UINT SuperMoney = 0;
	UINT Coupons = 0;
	UINT LuckMoney = 0;
	UINT SpeedCoin = 0;
	sql = "SELECT Money,SuperMoney,Coupons,LuckMoney,SpeedCoin FROM BaseInfo WHERE Uin=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Money = sqlite3_column_int(stmt, 0);
			SuperMoney = sqlite3_column_int(stmt, 1); // 点券
			Coupons = sqlite3_column_int(stmt, 2);	  // 消费卷
			LuckMoney = sqlite3_column_int(stmt, 3);  // 幸运币
			SpeedCoin = sqlite3_column_int(stmt, 4);  // 飞车币
		}
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	Write32(p, Client->Uin); // Uin
	{						 // CurrentMoney
		BYTE *pCurrentMoney = p;
		Write16(pCurrentMoney, 0); // len

		Write32(pCurrentMoney, Money);		// Money
		Write32(pCurrentMoney, SuperMoney); // SuperMoney  点券
		Write32(pCurrentMoney, Coupons);	// Coupons		  消费卷
		Write32(pCurrentMoney, LuckMoney);	// LuckMoney  幸运币
		Write32(pCurrentMoney, 0);			// ConsumeScore
		Write32(pCurrentMoney, 0);			// HistoryConsumeScore
		Write32(pCurrentMoney, SpeedCoin);	// SpeedCoin  飞车币

		len = pCurrentMoney - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // IncMoney
		BYTE *pIncMoney = p;
		Write16(pIncMoney, 0); // len

		Write32(pIncMoney, Money);		// Money
		Write32(pIncMoney, SuperMoney); // SuperMoney 点券
		Write32(pIncMoney, Coupons);	// Coupons		消费卷
		Write32(pIncMoney, LuckMoney);	// LuckMoney  幸运币
		Write32(pIncMoney, 0);			// ConsumeScore
		Write32(pIncMoney, 0);			// HistoryConsumeScore
		Write32(pIncMoney, SpeedCoin);	// SpeedCoin  飞车币

		len = pIncMoney - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // Flag

	len = p - buf;
	SendToClient(Client, 528, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestReportCommonEvent(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR EventType = Read8(p);
	int EventID = Read32(p);
	UCHAR ParaNum = Read8(p);
	// int[] Para[100] = Read32(p);
	/*if (EventID==125) {
		NotifyBorderGameCondition(Client);
	}*/
}

void RequestChangeTeam(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT NewTeamID = Read8(p);
	Client->TeamID = NewTeamID;
	ResponseChangeTeam(Client);
	RoomNode *Room = GetRoom(Client->RoomID);
	for (char i = 0; i < Room->CurrentPlayerNum; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		// 通知其它玩家
		for (char i2 = 0; i2 < Room->CurrentPlayerNum; i2++)
		{
			ClientNode *RoomClient = Room->Player[i2];
			if (RoomClient && RoomClient != Client)
			{

				NotifyEnterRoom(RoomClient, Room, Client, i);
				NotifyEnterRoom(Client, Room, RoomClient, i2);
			}
		}
	}
}

void ResponseChangeTeam(ClientNode *Client)
{
	int n = 1;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			   // ResultID
	Write8(p, Client->TeamID); // NewTeamID
	Write8(p, 0);			   // ReasonLen
	Write8(p, 0);			   // LoverRaceOrPro

	len = p - buf;
	SendToClient(Client, 128, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestOpenBox(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	int n = 1;
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int KeyID = 0;
	int Num = 0;
	int TreasureBoxID = Read32(p);
	{ // Key
		BYTE *pKey = p;
		len = Read16(pKey);

		int KeyType = Read32(pKey);
		for (size_t i = 0; i < n; i++)
		{ // KeyEntry
			BYTE *pKeyEntry = pKey;
			len = Read16(pKeyEntry);

			KeyID = Read32(pKeyEntry);
			Num = Read32(pKeyEntry);

			pKey += len;
		}
		UINT IsNotConsume = Read8(pKey);
		// Read8(pKey);

		p += len;
	}
	UINT SpecialIdx = Read8(p);
	USHORT MutiOpenNum = Read16(p);
	USHORT OpenBoxSrcType = Read16(p);
	UCHAR SelectAwardNum = Read8(p);
	int ItemIDNum = 1;
	for (size_t i = 0; i < n; i++)
	{ // AwardIndexInfo
		BYTE *pAwardIndexInfo = p;
		len = Read16(pAwardIndexInfo);

		UCHAR AwardIdx = Read8(pAwardIndexInfo);
		ItemIDNum = Read32(pAwardIndexInfo);
		if (ItemIDNum == 0)
		{
			ItemIDNum = 1;
		}
		p += len;
	}
	if (TreasureBoxID == 65736)
	{ // 紫钻
		RequestChangeVip(Client, Body, BodyLen, 65736);
		ResponseStartGame(Client, "/<a>/C01开通紫钻成功");
		NotifyClientVipFlag(Client);
		return;
	}
	else if (TreasureBoxID == 62787)
	{ // 魅力
		RequestChangeVip(Client, Body, BodyLen, 62787);
		ResponseStartGame(Client, "/<a>/C01魅力提升");
		NotifyClientUpdateConsumeVipRealteInfo(Client);
		return;
	}
	char FilePath[MAX_PATH];
	sprintf_s(FilePath, ".\\Gift\\%d.yml", TreasureBoxID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);
	if (isFileExists)
	{
		YAML::Node GiftConfig = YAML::LoadFile(FilePath)["Items"];
		UINT type = YAML::LoadFile(FilePath)["Type"].as<INT>();
		if (type == 1 || type == 2)
		{
			int Itemnum = 1;
			if (type == 2)
			{
				Itemnum = GiftConfig.size();
			}
			Itemnum *= ItemIDNum;
			ItemInfo *aItemStatus = (ItemInfo *)malloc(sizeof(ItemInfo) * Itemnum);
			for (int i = 0; i < Itemnum; i++)
			{
				int randomID;
				std::random_device rd;
				std::mt19937 eng(rd());
				std::uniform_int_distribution<int> dis(1, GiftConfig.size());
				randomID = dis(eng);
				YAML::Node Items = GiftConfig[randomID - 1];
				if (type == 2)
				{
					YAML::Node Items = GiftConfig[i];
				}
				aItemStatus[i].ItemID = Items["ItemId"].as<INT>();
				aItemStatus[i].ItemNum = Items["ItemNum"].as<INT>();
				aItemStatus[i].AvailPeriod = Items["AvailPeriod"].as<INT>();
				aItemStatus[i].ItemType = Items["ItemType"].as<INT>();

				ItemInfo Iteminsos[3000];
				ItemInfo Item = PlayerDB_AddItem(Client->Uin, aItemStatus[i].ItemID, aItemStatus[i].ItemNum, aItemStatus[i].AvailPeriod, false);
				Iteminsos[0].ItemID = aItemStatus[i].ItemID;
				Iteminsos[0].ItemNum = Item.ItemNum;
				Iteminsos[0].AvailPeriod = Item.AvailPeriod;
				Iteminsos[0].Status = Item.Status;
				Iteminsos[0].ObtainTime = Item.ObtainTime;
				Iteminsos[0].OtherAttribute = Item.OtherAttribute;
				Iteminsos[0].ItemType = Item.ItemType;
				NotifyClientAddItem(Client, 1, Iteminsos);
			}
			ResponseOpenBox(Client, TreasureBoxID, aItemStatus, Itemnum, ItemIDNum);
			int RefitItemNum = 0;
			sqlite3_stmt *stmt = NULL;
			// GetItemNum
			if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, TreasureBoxID);

				if (sqlite3_step(stmt) == SQLITE_ROW)
					RefitItemNum = sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			RefitItemNum -= ItemIDNum;
			if (RefitItemNum <= 0)
			{ // Delete Item
				if (TreasureBoxID == 10020)
				{
					return;
				}
				if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, TreasureBoxID);
					sqlite3_step(stmt);
				}
			}
			else
			{
				if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemIDNum);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					sqlite3_bind_int(stmt, 3, TreasureBoxID);
					sqlite3_step(stmt);
				}
			}
			NotifyUseItemResult(Client, TreasureBoxID, ItemIDNum, RefitItemNum);
			sqlite3_finalize(stmt);
			stmt = NULL;
			if (KeyID > 0 && Num > 0)
			{
				RefitItemNum = 0;
				// GetItemNum
				if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KeyID);

					if (sqlite3_step(stmt) == SQLITE_ROW)
						RefitItemNum = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				RefitItemNum -= Num;
				if (RefitItemNum <= 0)
				{ // Delete Item
					if (TreasureBoxID == 10020)
					{
						return;
					}
					if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
					{
						sqlite3_bind_int(stmt, 1, Client->Uin);
						sqlite3_bind_int(stmt, 2, KeyID);
						sqlite3_step(stmt);
					}
				}
				else
				{
					if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
					{
						sqlite3_bind_int(stmt, 1, Num);
						sqlite3_bind_int(stmt, 2, Client->Uin);
						sqlite3_bind_int(stmt, 3, KeyID);
						sqlite3_step(stmt);
					}
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			NotifyUseItemResult(Client, KeyID, Num, RefitItemNum);
		}
		else if (type == 3)
		{
			YAML::Node Config = YAML::LoadFile(FilePath);
			ListInfo *aItemStatus = (ListInfo *)malloc(sizeof(ListInfo) * 100);
			bool falg = true;
			int n = 0;
			int rate = 0;
			while (falg)
			{
				std::string num = std::to_string(n + 1);
				if (Config["List" + num])
				{
					rate += Config["List" + num]["Rate"].as<INT>();
					aItemStatus[n].Rate = rate;
					aItemStatus[n].No = n + 1;
					n++;
				}
				else
				{
					falg = false;
				}
			}
			ItemInfo *aItem = (ItemInfo *)malloc(sizeof(ItemInfo) * ItemIDNum);
			for (int q = 0; q < ItemIDNum; q++)
			{
				int randomID;
				std::random_device rd;
				std::mt19937 eng(rd());
				std::uniform_int_distribution<int> dis(1, rate);
				randomID = dis(eng);
				for (int i = 0; i < n; i++)
				{
					if (aItemStatus[i].Rate >= randomID)
					{
						std::string NO = std::to_string(aItemStatus[i].No);
						YAML::Node ItemList = Config["List" + NO]["Items"];
						int size = ItemList.size();
						int randomItem;
						std::random_device rd;
						std::mt19937 eng(rd());
						std::uniform_int_distribution<int> dis(1, size);
						randomItem = dis(eng);
						YAML::Node Items = ItemList[randomItem - 1];
						aItem[q].ItemID = Items["ItemId"].as<INT>();
						aItem[q].ItemNum = Items["ItemNum"].as<INT>();
						aItem[q].AvailPeriod = Items["AvailPeriod"].as<INT>();
						aItem[q].ItemType = Items["ItemType"].as<INT>();
						ItemInfo Iteminsos[3000];
						ItemInfo Item = PlayerDB_AddItem(Client->Uin, aItem[q].ItemID, aItem[q].ItemNum, aItem[q].AvailPeriod, false);
						Iteminsos[0].ItemID = aItem[q].ItemID;
						Iteminsos[0].ItemNum = Item.ItemNum;
						Iteminsos[0].AvailPeriod = Item.AvailPeriod;
						Iteminsos[0].Status = Item.Status;
						Iteminsos[0].ObtainTime = Item.ObtainTime;
						Iteminsos[0].OtherAttribute = Item.OtherAttribute;
						Iteminsos[0].ItemType = Item.ItemType;
						NotifyClientAddItem(Client, 1, Iteminsos);
						break;
					}
				}
			}
			ResponseOpenBox(Client, TreasureBoxID, aItem, ItemIDNum, ItemIDNum);
			int RefitItemNum = 0;
			sqlite3_stmt *stmt = NULL;
			// GetItemNum
			if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, TreasureBoxID);

				if (sqlite3_step(stmt) == SQLITE_ROW)
					RefitItemNum = sqlite3_column_int(stmt, 0);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			RefitItemNum -= ItemIDNum;
			if (RefitItemNum <= 0)
			{ // Delete Item
				if (TreasureBoxID == 10020)
				{
					return;
				}
				if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, TreasureBoxID);
					sqlite3_step(stmt);
				}
			}
			else
			{
				if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ItemIDNum);
					sqlite3_bind_int(stmt, 2, Client->Uin);
					sqlite3_bind_int(stmt, 3, TreasureBoxID);
					sqlite3_step(stmt);
				}
			}
			NotifyUseItemResult(Client, TreasureBoxID, ItemIDNum, RefitItemNum);
			sqlite3_finalize(stmt);
			stmt = NULL;
			if (KeyID > 0 && Num > 0)
			{
				RefitItemNum = 0;
				// GetItemNum
				if (sqlite3_prepare_v2(PlayerDB, "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, Client->Uin);
					sqlite3_bind_int(stmt, 2, KeyID);

					if (sqlite3_step(stmt) == SQLITE_ROW)
						RefitItemNum = sqlite3_column_int(stmt, 0);
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
				RefitItemNum -= ItemIDNum;
				if (RefitItemNum <= 0)
				{ // Delete Item
					if (TreasureBoxID == 10020)
					{
						return;
					}
					if (sqlite3_prepare_v2(PlayerDB, "DELETE FROM Item  WHERE Uin=? AND ItemID=?;", -1, &stmt, NULL) == SQLITE_OK)
					{
						sqlite3_bind_int(stmt, 1, Client->Uin);
						sqlite3_bind_int(stmt, 2, KeyID);
						sqlite3_step(stmt);
					}
				}
				else
				{
					if (sqlite3_prepare_v2(PlayerDB, "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;", -1, &stmt, NULL) == SQLITE_OK)
					{
						sqlite3_bind_int(stmt, 1, Num);
						sqlite3_bind_int(stmt, 2, Client->Uin);
						sqlite3_bind_int(stmt, 3, KeyID);
						sqlite3_step(stmt);
					}
				}
				sqlite3_finalize(stmt);
				stmt = NULL;
			}
			NotifyUseItemResult(Client, KeyID, Num, RefitItemNum);
		}
		else
		{
			ResponseStartGame(Client, "无效的类型");
		}
	}
	else
	{
		ResponseStartGame(Client, "未配置的道具");
	}
}
void ResponseOpenBox(ClientNode *Client, int TreasureBoxID, ItemInfo *aItemStatus, int itemNum, int ItemIDNum)
{
	int n = itemNum;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	{						 // TreasureBoxToClient
		BYTE *pTreasureBoxToClient = p;
		Write16(pTreasureBoxToClient, 0); // len

		Write32(pTreasureBoxToClient, TreasureBoxID); // BoxID
		Write32(pTreasureBoxToClient, 0);			  // BoxType
		{											  // Key
			BYTE *pKey = pTreasureBoxToClient;
			Write16(pKey, 0); // len

			Write32(pKey, 0); // KeyType
			for (size_t i = 0; i < n; i++)
			{ // KeyEntry
				BYTE *pKeyEntry = pKey;
				Write16(pKeyEntry, 0); // len

				Write32(pKeyEntry, 0); // KeyID
				Write32(pKeyEntry, 0); // Num

				len = pKeyEntry - pKey;
				Set16(pKey, (WORD)len);
				pKey += len;
			}
			Write8(pKey, 0); // IsNotConsume

			len = pKey - pTreasureBoxToClient;
			Set16(pTreasureBoxToClient, (WORD)len);
			pTreasureBoxToClient += len;
		}
		{ // AwardEntry
			BYTE *pAwardEntry = pTreasureBoxToClient;
			Write16(pAwardEntry, 0); // len

			Write32(pAwardEntry, 0); // Exp
			Write32(pAwardEntry, 0); // Money
			Write32(pAwardEntry, 0); // SuperMoney
			Write16(pAwardEntry, n); // ItemNum
			for (size_t i = 0; i < n; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardEntry;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, aItemStatus[i].ItemID);		// ItemID
				Write32(pItemInfo, aItemStatus[i].ItemNum);		// ItemNum
				Write32(pItemInfo, aItemStatus[i].AvailPeriod); // AvailPeriod
				Write8(pItemInfo, 0);							// Status
				Write32(pItemInfo, 0);							// ObtainTime
				Write32(pItemInfo, 0);							// OtherAttribute
				Write16(pItemInfo, aItemStatus[i].ItemType);	// ItemType

				len = pItemInfo - pAwardEntry;
				Set16(pAwardEntry, (WORD)len);
				pAwardEntry += len;
			}
			Write32(pAwardEntry, 0); // Coupons
			Write32(pAwardEntry, 0); // GuildPoint
			Write32(pAwardEntry, 0); // LuckMoney
			Write8(pAwardEntry, 0);	 // ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardEntry;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardEntry;
				Set16(pAwardEntry, (WORD)len);
				pAwardEntry += len;
			}
			Write32(pAwardEntry, 0); // SpeedCoin

			len = pAwardEntry - pTreasureBoxToClient;
			Set16(pTreasureBoxToClient, (WORD)len);
			pTreasureBoxToClient += len;
		}

		len = pTreasureBoxToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);		   // ReasonLen
	Write32(p, ItemIDNum); // Reserve
	Write32(p, 0);		   // MaxLuckVaue
	Write32(p, 0);		   // LuckValue
	Write32(p, 0);		   // TotalLuckValue
	Write32(p, 0);		   // OpenDate
	Write32(p, 0);		   // CloseDate
	Write32(p, 0);		   // OpenTime
	Write32(p, 0);		   // CloseTime

	len = p - buf;
	SendToClient(Client, 168, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseOpenSignBox(ClientNode *Client)
{
	int n = 1;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write8(p, 0);  // ReasonLen
	Write32(p, 0); // Uin
	Write32(p, 0); // BoxLevel
	Write8(p, 0);  // HasAward
	for (size_t i = 0; i < n; i++)
	{ // AwardInfo
		BYTE *pAwardInfo = p;
		Write16(pAwardInfo, 0); // len

		Write32(pAwardInfo, 0); // Exp
		Write32(pAwardInfo, 0); // Money
		Write32(pAwardInfo, 0); // SuperMoney
		Write16(pAwardInfo, 0); // ItemNum
		for (size_t i = 0; i < n; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAwardInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 0); // ItemID
			Write32(pItemInfo, 0); // ItemNum
			Write32(pItemInfo, 0); // AvailPeriod
			Write8(pItemInfo, 0);  // Status
			Write32(pItemInfo, 0); // ObtainTime
			Write32(pItemInfo, 0); // OtherAttribute
			Write16(pItemInfo, 0); // ItemType

			len = pItemInfo - pAwardInfo;
			Set16(pAwardInfo, (WORD)len);
			pAwardInfo += len;
		}
		Write32(pAwardInfo, 0); // Coupons
		Write32(pAwardInfo, 0); // GuildPoint
		Write32(pAwardInfo, 0); // LuckMoney
		Write8(pAwardInfo, 0);	// ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAwardInfo;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAwardInfo;
			Set16(pAwardInfo, (WORD)len);
			pAwardInfo += len;
		}
		Write32(pAwardInfo, 0); // SpeedCoin

		len = pAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24041, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseTowerOpenBox(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int n = 1;
	Write16(p, 0); // ResultID
	Write32(p, 0); // Uin
	Write8(p, 0);  // LevelID
	Write8(p, 0);  // Index
	Write32(p, 0); // hasAward
	for (size_t i = 0; i < n; i++)
	{ // Award
		BYTE *pAward = p;
		Write16(pAward, 0); // len

		Write32(pAward, 0); // Exp
		Write32(pAward, 0); // Money
		Write32(pAward, 0); // SuperMoney
		Write16(pAward, 0); // ItemNum
		for (size_t i = 0; i < n; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAward;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 0); // ItemID
			Write32(pItemInfo, 0); // ItemNum
			Write32(pItemInfo, 0); // AvailPeriod
			Write8(pItemInfo, 0);  // Status
			Write32(pItemInfo, 0); // ObtainTime
			Write32(pItemInfo, 0); // OtherAttribute
			Write16(pItemInfo, 0); // ItemType

			len = pItemInfo - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); // Coupons
		Write32(pAward, 0); // GuildPoint
		Write32(pAward, 0); // LuckMoney
		Write8(pAward, 0);	// ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAward;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); // SpeedCoin

		len = pAward - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24331, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestChangeIdentity(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	const unsigned char* pMsg13199 = EHexToByte("000001010000006360D0000632C400A00120C043E40181587800000000000000000000007449");
	const unsigned char* pMsg1311171 = EHexToByte("000001010000007540D0000E220443B00160C502E47BE55FF81C310E0000000000000000F59D");
	const unsigned char* pMsg1311170 = EHexToByte("000000010000007540D0000E220443B00160C502E47BE55FF81C110E00000000000000002029");
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT SetOrUnset = Read8(Body);
	UINT IsExRightFlag = Read8(Body);
	UINT IdentityFlag = Read32(Body);
	UINT BeginBit = Read32(Body);
	UINT EndBit = Read32(Body);
	if (IdentityFlag == 117)
	{
		if (SetOrUnset==0) {
			SendToClient(Client, 131, pMsg1311170, 38, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
		}
		else if (SetOrUnset == 1) {
			SendToClient(Client, 131, pMsg1311171, 38, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
		}
		
		return;
	}
	if (IdentityFlag == 99)
	{
		SendToClient(Client, 131, pMsg13199, 38, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
		return;
	}
	ResponseChangeIdentity(Client, SetOrUnset, IsExRightFlag, IdentityFlag);
}

void ResponseChangeIdentity(ClientNode* Client, char SetOrUnset, char IsExRightFlag, UINT IdentityFlag)
{
	BYTE buf[39] = { 0 };
	
	if (SetOrUnset == 0)
	{
		memcpy(buf, "\x00\x00\x00\x01\x00\x00\x00\x78\x00\x50\x00\x01\x02\x00\x00\xa0\x00\x20\x80\x03\xa4\x01\x00\x40\x10\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 38);
	}
	else if(IdentityFlag == 0x78)
	{
		memcpy(buf, "\x00\x00\x01\x01\x00\x00\x00\x78\x00\x50\x00\x01\x02\x00\x00\xa0\x00\x20\x80\x03\xa4\x01\x00\x40\x10\x00\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 38);
	}
	else if (IdentityFlag == 0x2)
	{
		memcpy(buf, "\x00\x00\x01\x01\x00\x00\x00\x02\x00\x50\x00\x01\x06\x00\x00\xa0\x00\x20\x80\x03\xa4\x01\x00\x40\x10\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 38);
	}
	else {
		BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write8(p, SetOrUnset); //SetOrUnset 0x0重置
	Write8(p, IsExRightFlag); //IsExRightFlag
	Write32(p, IdentityFlag); //IdentityFlag 0x78关小喷灯 0x02关宝石
	Write32(p, 0); //Identity
	Write8(p, 0); //ExRightFlag[]

	len = p - buf;
	}
	

	SendToClient(Client, 131, (BYTE*)buf, 38, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyAntiBot(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, 1);			 // Type
	Write16(p, 2);			 // BuffLen

	len = p - buf;
	SendToClient(Client, 538, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void AckAntiBot(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	short BuffLen = Read16(p);
	NotifyAntiBot(Client);
}
void RequestWorldOnlineInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseWorldOnlineInfo(Client);
}

void RequestEnterPersonalGarden(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR RoomType = Read8(p);
	UINT RoomKey = Read32(p);
	ResponseEnterPersonalGarden(Client);
}

void ResponseEnterPersonalGarden(ClientNode *Client)
{
	int n = 1;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultId
	Write32(p, 0); // Uin
	Write32(p, 0); // RoomKey
	Write16(p, 0); // ChatRoomID
	{			   // AddrInfo
		BYTE *pAddrInfo = p;
		Write16(pAddrInfo, 0); // len

		Write32(pAddrInfo, 0); // svrId
		Write32(pAddrInfo, 0); // svrIP
		Write32(pAddrInfo, 0); // tcpPortNum
		Write32(pAddrInfo, 0); // tcpPort[]
		Write32(pAddrInfo, 0); // udpPortNum
		Write32(pAddrInfo, 0); // udpPort[]
		Write8(pAddrInfo, 0);  // HasMultiIPInfo
		for (size_t i = 0; i < n; i++)
		{ // MultiIPInfo
			BYTE *pMultiIPInfo = pAddrInfo;
			Write16(pMultiIPInfo, 0); // len

			Write8(pMultiIPInfo, 0);  // IPCount
			Write32(pMultiIPInfo, 0); // ServerIP[]
			Write8(pMultiIPInfo, 0);  // DomainLen

			len = pMultiIPInfo - pAddrInfo;
			Set16(pAddrInfo, (WORD)len);
			pAddrInfo += len;
		}

		len = pAddrInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 20137, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
// void RequestQuickRPC(ClientNode* Client, BYTE* Body, size_t BodyLen)
//{
//	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
//	printf("RequestQuickRPC	Hex:%s\n", pMsg);*/
//
//	size_t len;
//	//Head
//	len = Read16(Body);
//	UINT Uin = Read32(Body);
//	UINT Time = Read32(Body);
//	BYTE Version = Read8(Body);
//	BYTE Compress = Read8(Body);
//	BYTE OptLength = Read8(Body);
//	//Body
//	len = Read16(Body);
//	USHORT MsgLen = Read16(Body);
//	std::string pMsg((char*)Body);
//
//	//printf("ResponseQuickRPC Uin:%d Version:%d Compress:%d OptLength:%d MsgLen:%d\n", Uin, Version, Compress, OptLength, MsgLen);
// #ifdef DEBUG
//	printf("ResponseQuickRPC:%s\n", pMsg.data());
// #endif
//
//
//
//	BYTE buf[8192]; BYTE* p = buf;
//
//	{ //Head
//		BYTE* pHead = p;
//		Write16(pHead, 0); //len
//
//		Write32(pHead, Uin); //Uin
//		Write32(pHead, Time); //Time
//		Write8(pHead, Version); //Version
//		Write8(pHead, Compress); //Compress
//		Write8(pHead, OptLength); //OptLength
//
//		len = pHead - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//	{ //Body
//		BYTE* pBody = p;
//		Write16(pBody, 0); //len
//
//		Write16(pBody, MsgLen); //MsgLen
//		Write16(pBody, 0); //len
//
//
//		/*len = pMsg.length();
//		memcpy(pBody, pMsg.data(), len);*/
//
//		len = pBody - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//	len = p - buf;
//	SendToClient(Client, 24251, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
//
// }
template <typename T>
T z_get(BYTE* b) {
	return *(T*)(b);
}
template <typename T>
void z_set(BYTE* b, T val) {
	*(T*)(b) = val;
}
void MyResponseQuickRPC(ClientNode* Client, BYTE* Msg, size_t MsgLen) {
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	{ //Head
		BYTE* pHead = p;
		Write16(pHead, 0); //len

		Write32(pHead, Client->Uin); //Uin
		Write32(pHead, (DWORD)time(nullptr)); //Time
		Write8(pHead, 0); //Version
		Write8(pHead, 0); //Compress
		Write8(pHead, 0); //OptLength

		len = pHead - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //Body
		BYTE* pBody = p;
		Write16(pBody, 0); //len
		Write16(pBody, MsgLen); //MsgLen
		memcpy(pBody, Msg, MsgLen);
		pBody += MsgLen;

		len = pBody - p;
		Set16(p, (WORD)len);
		p += len;
	}
	len = p - buf;
	SendToClient(Client, 24251, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestQuickRPC(ClientNode* Client, BYTE* Body, size_t BodyLen)
{

	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("RequestQuickRPC	Hex:%s\n", pMsg);*/

	size_t len;
	// Head
	len = Read16(Body);
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	BYTE Version = Read8(Body);
	BYTE Compress = Read8(Body);
	BYTE OptLength = Read8(Body);
	// Body
	len = Read16(Body);
	USHORT MsgLen = Read16(Body);
	// std::string pMsg((char *)Body);
	BYTE* msg = Body;
	std::string cmd((char*)msg);
	//Z_FATAL("debug, lineno=%d\n", __LINE__);
	double ping;
	{ // resolve msg
		int i = 0;
		for (; i < MsgLen - 1; i++) {
			if (msg[i] == 0 && msg[i + 1] == 0) {
				break;
			}
		}
		//Z_FATAL("debug, lineno=%d\n", __LINE__);
		// assert find i
		i += 2; // 0x0000
		i += 1; // 0xF6
		ping = z_get<double>(msg + i);
#ifdef DEBUG
		Z_FATAL("debug, lineno=%d, i=%d, ping=%d\n", __LINE__, i, (int)ping);

#endif // DEBUG

	}
	double pong;
	pong = ping;

	BYTE* response = NULL;
	std::string resp = "";
	//Z_FATAL("debug, lineno=%d\n", __LINE__);
	if (cmd == "Co.CheJian_QueryOrderInfo") {
		resp = "4469737061746368436F526573706F6E73650000F60000000000804540F60000000000000000FD00000000";
		//Z_FATAL("debug, lineno=%d\n", __LINE__);
	}
	else if (cmd == "Co.CarLibrary_OpenMainDlg") {
		// 自己替换bytes
		resp = "4469737061746368436F526573706F6E73650000F60000000000003340F60000000000000000FD00000364FA18436F6C6C65637456616C7565496E4C6576656C546F74616CF6000000000088C340FA0C436F6C6C65637456616C7565F6000000000075BF40FA07506F73496E666FFD000000C6F6000000000000F03FF60000000040DBF240F60000000000000040F600000000C027F240F60000000000000840F60000000000C5D340F60000000000001040F6000000000041D140F60000000000001440F60000000010E0F840F60000000000001840F600000000305CF740F60000000000001C40F600000000F06CF840F60000000000002040F60000000090FBF440F60000000000002240F60000000020C2F040F60000000000002440F600000000601DFC40F60000000000002640F600000000A027F240FA1A486973746F727948696768657374436F6C6C65637456616C7565F6000000000075BF40FA0554316E756DF60000000000001040FA0554326E756DF6000000000000F03FFA0554336E756DF60000000000000000FA09466C616756616C7565FD00000036F60000000000003D40F6000000000000F03FF60000000000003B40F6000000000000F03FF60000000000003C40F6000000000000F03FFA07537569746E756DF6000000000000F03FFA0742617365436667FD0000010CFA0E416C6C6F77506C61636554506F73FD0000005AF6000000000000F03FF60000000000001C40F60000000000000040F60000000000002040F60000000000000840F60000000000002240F60000000000001040F60000000000002440F60000000000001440F60000000000002640FA09536E756D546F74616CF60000000000805D40FA144D6178436F6C6C65637456616C75654C6576656CF60000000000002440FA0C537569746E756D546F74616CF60000000000004340FA0A54326E756D546F74616CF60000000000805140FA0A54316E756D546F74616CF60000000000004940FA09416E756D546F74616CF60000000000F88140FA0A54336E756D546F74616CF60000000000002240FA11436F6C6C65637456616C75654C6576656CF60000000000002040FA04416E756DF60000000000003040FA0A466F7262696453686F77F6000000000000F03FFA056F74686572F60000000000004240FA0753686F774E756DF60000000000002640FA13436F6C6C65637456616C7565496E4C6576656CF6000000000075BF40FA04536E756DF600000000000010403500";
		//Z_FATAL("debug, lineno=%d\n", __LINE__);
	}
	else if (cmd == "Co.MapChallenge_GetOpenResultID") {
		resp = "4469737061746368436F526573706F6E73650000F60000000000003240F600000000000000000000";
	}
	else if (cmd == "Co.MapChallenge_OpenMainDlg") {
		// 自己替换bytes
		resp = "4469737061746368436F526573706F6E73650000F60000000000003340F60000000000000000FD000005B6FA09436C6F736554696D65F60000C0BF6728D941FA104861734D61705265636F726444617461F8FA076D6170696E666FFD0000051FF6000000000000F03FFD0000012EFA054D61704944F60000000000B07840FA0D4D61705265636F726454696D65F600000000F019F740FA0A417373657373496E666FFD00000072FA034E756DF6000000000000F03FFA0A41737365737354797065F60000000000003740FA034D7367FA19CDEAB3C9B1C8C8FCB2A2CAB9D3C3574357C5E77B74637DB4CEFA074D736747616D65FA11CAB9D3C3574357C5E77B74637DB4CEA3BAFA08697366696E697368F60000000000000000FA0853746172496E666FFD00000074F6000000000000F03FFD0000001EFB0CF60000000000000000FA086261736574696D65F600000000F001FB40F60000000000000040FD00000016FB0CF60000000000000000FB0EF600000000F007FA40F60000000000000840FD00000016FB0CF60000000000000000FB0EF600000000F00DF940F60000000000000040FD000000B9FB03F60000000000E06F40FB04F60000000070F3F640FB05FD00000029FB06F6000000000000F03FFB07F60000000000003740FB08FB09FB0AFB0BFB0CF60000000000000000FB0DFD0000006CF6000000000000F03FFD00000016FB0CF60000000000000000FB0EF60000000070DBFA40F60000000000000040FD00000016FB0CF60000000000000000FB0EF60000000070E1F940F60000000000000840FD00000016FB0CF60000000000000000FB0EF60000000070E7F840F60000000000000840FD000000B9FB03F60000000000006540FB04F6000000000000F0BFFB05FD00000029FB06F6000000000000F03FFB07F60000000000003740FB08FB09FB0AFB0BFB0CF60000000000000000FB0DFD0000006CF6000000000000F03FFD00000016FB0CF60000000000000000FB0EF600000000C0440941F60000000000000040FD00000016FB0CF60000000000000000FB0EF600000000C0C70841F60000000000000840FD00000016FB0CF60000000000000000FB0EF600000000C04A0841F60000000000001040FD000000B9FB03F60000000000007040FB04F600000000802BF940FB05FD00000029FB06F6000000000000F03FFB07F60000000000003740FB08FB09FB0AFB0BFB0CF60000000000000000FB0DFD0000006CF6000000000000F03FFD00000016FB0CF60000000000000000FB0EF6000000008013FD40F60000000000000040FD00000016FB0CF60000000000000000FB0EF6000000008019FC40F60000000000000840FD00000016FB0CF60000000000000000FB0EF600000000801FFB40F60000000000001440FD000000B9FB03F60000000000606D40FB04F6000000000000F0BFFB05FD00000029FB06F6000000000000F03FFB07F60000000000003740FB08FB09FB0AFB0BFB0CF60000000000000000FB0DFD0000006CF6000000000000F03FFD00000016FB0CF60000000000000000FB0EF600000000804F0241F60000000000000040FD00000016FB0CF60000000000000000FB0EF60000000080D20141F60000000000000840FD00000016FB0CF60000000000000000FB0EF60000000080550141F60000000000001840FD000000B9FB03F60000000000C07440FB04F600000000A0DCF340FB05FD00000029FB06F6000000000000F03FFB07F60000000000003740FB08FB09FB0AFB0BFB0CF60000000000000000FB0DFD0000006CF6000000000000F03FFD00000016FB0CF60000000000000000FB0EF600000000A0C4F740F60000000000000040FD00000016FB0CF60000000000000000FB0EF600000000A0CAF640F60000000000000840FD00000016FB0CF60000000000000000FB0EF600000000A0D0F540FA0F526566726573684C65667444617973F60000000000001C40FA0742617365436667FD00000014FA094368616E6E656C4944F60000000000000840FA084F70656E54696D65F6000000201926D941FA08537461724E756D73F600000000000000003531";
		//Z_FATAL("debug, lineno=%d\n", __LINE__);
	}
	else if (cmd == "Co.CarLibraryOp") {
		resp = "4469737061746368436F526573706F6E73650000F60000000000004B40F60000000000409F40001A";
	}
	else if (cmd == "Co.CarLibrary_GetCollect") {
		resp = "4469737061746368436F526573706F6E73650000F60000000000804A40F6000000000000894000A2";
	}
	else if ("Co.ProcessionLicense_OpenMainDlg") {
		resp = "4469737061746368436F526573706F6E73650000F60000000000003440F60000000000000000FD00000AE6FA0B7765656B6578705F6D6178F6000000000082B440FA09726571626C6576656CF60000000000000040FA06736C6576656CF60000000000001440FA0D736C6576656C5F657870637572F600000000007CAA40FA0742617365436667FD00000056FA134D61784C6576656C5F536D616C6C4C6576656CF60000000000001440FA115765656B4D6178546F74616C53636F7265F60000000000409F40FA114D61784C6576656C5F4269674C6576656CF60000000000001840FA08506F776572436667FD000002F7F60000000000609240FD0000006BFA0B73696D706C655F64657363FA18B8C4D7B0D0D2D4CBD6B5C9FAD0A7B5C8BCB6C9CFCFDE2B31FA046E616D65FA12B8C4D7B0D0D2D4CBD6B5C9CFCFDECCD8C8A8FA0464657363FA22B8C4D7B0D0D2D4CBD6B5C9FAD0A7B5C8BCB6C9CFCFDE2B31A3A8B1E4CEAA3136A3A9F60000000000549240FD00000023FB09FA0FBBF1B5C3B9A4D7CACCE1C9FD313025FB0BFA08B9A4D7CACCD8C8A8FB0DFB0FF60000000000649240FD00000043FB09FA14C2E4BAF3BBF1B5C3BCAFC6F8D0A7C2CACCE1C9FDFB0BFA0CBCAFC6F8D0A7C2CACCD8C8A8FB0DFA17C2E4BAF3BBF1B5C3BCAFC6F8D0A7C2CACCE1C9FD313525F60000000000589240FD0000006EFB09FA14B1DFBEB3B6D4BED6BBF1B5C3B6EECDE2C8D9D3FEFB0BFA0CB1DFBEB3C8D9D3FECCD8C8A8FB0DFA42C3BFBED6B1DFBEB3B1C8C8FCBBF1B5C3B6EECDE2B5E3C8D9D3FEA3ACCFEDCADCB6E0B1B6BCD3B3C928B3F5BCB62B35A3ACB8DFBCB62B3130A3ACD7A8D2B52B313529F600000000004C9240FD0000008CFB09FA14C3BFC8D5CFDEB6A8B4CECAFDBEADD1E9BCD3B3C9FB0BFA08BEADD1E9CCD8C8A8FB0DFA64C3BFC8D5CFDEB6A8C7B03135BED6C8CED2E2B1C8C8FCA3ACC3BFBED6B6EECDE2BBF1B5C3247B6164646578705F6669786E756D7DBEADD1E928CEB4CDEAB3C9BBF1B5C3247B6164646578705F6669786E756D5F6E6F7466696E69736865647DBEADD1E929F600000000005C9240FD0000004EFB09FA16C3BFC8D5C5C5CEBBCAD7B4CECAA7B0DCB2BBB5F4B7D6FB0BFA10C5C5CEBBC8FCB2BBB5F4B7D6CCD8C8A8FB0DFA1CC3BFC8D5C5C5CEBBC8FCB6D4BED6CAD7B4CECAA7B0DCB2BBB5F4B7D6F60000000000509240FD0000007CFB09FA14C3BFD6DCCFDEB6A8B4CECAFDBACFB3C9B1A3BBA4FB0BFA0CB1A6CAAFBACFB3C9CCD8C8A8FB0DFA50C3BFD6DCCFDEB6A8C9FAD0A733B4CEA3ACB1A6CAAFBACFB3C9CAA7B0DCCAB1A3ACBFC9B1A3C1F431B8F6D3C3C0B4BACFB3C9B5C4B5CDB5C8BCB6B1A6CAAFA3A8D3C5CFC8CAB9D3C3D7A3B8A3CAAFA3A9FA0E726571626C6576656C5F696E666FFD00000517FA075461736B436667FD000001E5F6000000000000F03FFD0000007EFA0A6D5F75635461736B4944F60000000000DAAE40FA126D5F75695461736B46696E6973684E756D73F60000000000003E40FA06537461747573F60000000000000040FA146D5F7569416C726561647941776172644E756D73F60000000000003E40FA116D5F756941776172644D6F76654E756D73F60000000000C08240F60000000000000040FD00000037FB22F600000000006FB040FB23F60000000000001440FB24F60000000000000040FB25F60000000000001440FB26F60000000000C08240F60000000000000840FD00000037FB22F60000000000DCAE40FB23F60000000000003940FB24F60000000000000040FB25F60000000000003940FB26F60000000000C08240F60000000000001040FD00000037FB22F60000000000DEAE40FB23F60000000000002E40FB24F60000000000000040FB25F60000000000002E40FB26F60000000000C08240F60000000000001440FD00000037FB22F60000000000E0AE40FB23F60000000000008940FB24F60000000000000040FB25F60000000000008940FB26F60000000000C08240F60000000000001840FD00000037FB22F60000000000E2AE40FB23F60000000000002E40FB24F60000000000000040FB25F60000000000002E40FB26F60000000000C08240FA046C636667FD00000319FB21FD0000019EF6000000000000F03FFD00000037FB22F60000000000DAAE40FB23F60000000000003E40FB24F60000000000000040FB25F60000000000003E40FB26F60000000000C08240F60000000000000040FD00000037FB22F600000000006FB040FB23F60000000000001440FB24F60000000000000040FB25F60000000000001440FB26F60000000000C08240F60000000000000840FD00000037FB22F60000000000DCAE40FB23F60000000000003940FB24F60000000000000040FB25F60000000000003940FB26F60000000000C08240F60000000000001040FD00000037FB22F60000000000DEAE40FB23F60000000000002E40FB24F60000000000000040FB25F60000000000002E40FB26F60000000000C08240F60000000000001440FD00000037FB22F60000000000E0AE40FB23F60000000000008940FB24F60000000000000040FB25F60000000000008940FB26F60000000000C08240F60000000000001840FD00000037FB22F60000000000E2AE40FB23F60000000000002E40FB24F60000000000000040FB25F60000000000002E40FB26F60000000000C08240FA05706F776572FD00000024F6000000000000F03FF60000000000609240F60000000000000040F600000000004C9240FA0D737461725F657870746F74616CF6000000000094D140FA0B737461725F657870636667FD00000087F6000000000000F03FFD00000015FA0A75705F6E656564657870F6000000000020AC40F60000000000000040FD0000000BFB2BF6000000000020AC40F60000000000000840FD0000000BFB2BF6000000000020AC40F60000000000001040FD0000000BFB2BF6000000000020AC40F60000000000001440FD0000000BFB2BF6000000000020AC40FA0F706F696E745F7765656B6C696D6974F6000000000082B440FA12436F6D6D6F6E4177617264426F7854797065F60000000000C05C40FA0D6164646578705F6669786E756DF60000000000E08540FA196164646578705F6669786E756D5F6E6F7466696E6973686564F60000000000807140FA056C696D6974FD00000014FA09757365726C6576656CF60000000000005940FA0B506F776572537461747573FD000001AFF60000000000609240FD00000055FA046E756D73F60000000000002640FA0C706F7765725F626C6576656CF60000000000000040FB0BFB0CFB09FB0AFA06737461747573F6000000000000F03FFA076D61786E756D73F6000000000000F0BFFB0DFB0EF60000000000549240FD00000022FB34F60000000000001040FB0BFB10FB09FB0FFB35F60000000000000000FB0DFB0FF60000000000649240FD00000022FB34F60000000000001840FB0BFB12FB09FB11FB35F60000000000000000FB0DFB13F60000000000589240FD00000022FB34F60000000000001440FB0BFB15FB09FB14FB35F60000000000000000FB0DFB16F600000000004C9240FD00000038FB33F60000000000000040FB34F6000000000000F03FFB0BFB18FB09FB17FB35F6000000000000F03FFB36F60000000000002E40FB0DFB19F600000000005C9240FD00000022FB34F60000000000000840FB0BFB1BFB09FB1AFB35F60000000000000000FB0DFB1CF60000000000509240FD00000038FB33F60000000000000000FB34F6000000000000F03FFB0BFB1EFB09FB1DFB35F6000000000000F03FFB36F60000000000000840FB0DFB1FFA077765656B657870F60000000000005A40FA06626C6576656CF60000000000000040FA0E736C6576656C5F6578706E656564F6000000000020AC400000";

	}
	//Z_FATAL("debug, lineno=%d\n", __LINE__);
#ifdef DEBUG
	Z_FATAL("resp=%s\n", resp.data());
	Z_FATAL("msg=%s\n", (char*)msg);
	Z_FATAL("cmd=%s\n", cmd.data());
#endif // DEBUG

	

	// Z_FATAL("cmd=%s, ping=pong=%d, resp=%s\n", cmd.c_str(), ping, resp.c_str());
	// printf("cmd=%s, ping=pong=%d, resp=%s\n", cmd.c_str(), ping, resp.c_str());

	if (resp != "") {
		response = EHexToByte(resp.data());
		int len_of_response = resp.size() / 2;
		// replace pong
		int i = 0;
		for (; i < len_of_response; i++) {
			if (response[i] == 0 && response[i + 1] == 0) {
				break;
			}
		}
		// assert find i
		i += 2; // 0x0000
		i += 1; // 0xF6
		z_set<double>(response + i, pong);

		MyResponseQuickRPC(Client, response, len_of_response);
	}
	return;



	// printf("ResponseQuickRPC Uin:%d Version:%d Compress:%d OptLength:%d MsgLen:%d\n", Uin, Version, Compress, OptLength, MsgLen);
#ifdef DEBUG
	// printf("ResponseQuickRPC:%s\n", pMsg.data());
#endif
	}



void RequestGetKeyOfBox(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	char Type = Read8(p);
	int ID = Read32(p);
	ResponseGetKeyOfBox(Client, Type, ID);
}

void ResponseGetKeyOfBox(ClientNode *Client, UINT Type, UINT ID)
{
	char FilePath[MAX_PATH];
	sprintf_s(FilePath, ".\\Gift\\%d.yml", ID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);
	int n = 0;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	if (!isFileExists)
	{
		ResponseStartGame(Client, "未配置钥匙yml");
		return;
	}

	YAML::Node config = YAML::LoadFile(FilePath)["KeyList"];

	n = config.size();
	if (n > 1)
	{
		Type = 1;
	}
	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write8(p, Type);		 // Type
	Write32(p, ID);			 // ID
	Write8(p, n);			 // OpenNum
	int count = 0;
	for (size_t i = 0; i < n; i++)
	{ // OpenEntry
		BYTE *pOpenEntry = p;
		Write16(pOpenEntry, 0); // len

		Write32(pOpenEntry, n); // Num
		for (size_t i = 0; i < n; i++)
		{ // IDNum
			BYTE *pIDNum = pOpenEntry;
			Write16(pIDNum, 0); // len

			Write32(pIDNum, config[count]["Key"].as<INT>()); // ID
			Write32(pIDNum, config[count]["Num"].as<INT>()); // Num
			len = pIDNum - pOpenEntry;
			Set16(pOpenEntry, (WORD)len);
			pOpenEntry += len;
		}

		len = pOpenEntry - p;
		Set16(p, (WORD)len);
		p += len;
		count++;
	}

	len = p - buf;
	SendToClient(Client, 189, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetBoxItems(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int TreasureBoxID = Read32(p);
	UCHAR Contex = Read8(p);
	ResponseGetBoxItems(Client);
}

void ResponseGetBoxItems(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int n = 1;
	Write16(p, 0); // ResultID
	Write32(p, 0); // Uin
	{			   // TreasureBoxToClient
		BYTE *pTreasureBoxToClient = p;
		Write16(pTreasureBoxToClient, 0); // len

		Write32(pTreasureBoxToClient, 0); // BoxID
		Write32(pTreasureBoxToClient, 0); // BoxType
		{								  // Key
			BYTE *pKey = pTreasureBoxToClient;
			Write16(pKey, 0); // len

			Write32(pKey, 0); // KeyType
			for (size_t i = 0; i < n; i++)
			{ // KeyEntry
				BYTE *pKeyEntry = pKey;
				Write16(pKeyEntry, 0); // len

				Write32(pKeyEntry, 0); // KeyID
				Write32(pKeyEntry, 0); // Num

				len = pKeyEntry - pKey;
				Set16(pKey, (WORD)len);
				pKey += len;
			}
			Write8(pKey, 0); // IsNotConsume

			len = pKey - pTreasureBoxToClient;
			Set16(pTreasureBoxToClient, (WORD)len);
			pTreasureBoxToClient += len;
		}
		{ // AwardEntry
			BYTE *pAwardEntry = pTreasureBoxToClient;
			Write16(pAwardEntry, 0); // len

			Write32(pAwardEntry, 0); // Exp
			Write32(pAwardEntry, 0); // Money
			Write32(pAwardEntry, 0); // SuperMoney
			Write16(pAwardEntry, 0); // ItemNum
			for (size_t i = 0; i < n; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardEntry;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 0); // ItemID
				Write32(pItemInfo, 0); // ItemNum
				Write32(pItemInfo, 0); // AvailPeriod
				Write8(pItemInfo, 0);  // Status
				Write32(pItemInfo, 0); // ObtainTime
				Write32(pItemInfo, 0); // OtherAttribute
				Write16(pItemInfo, 0); // ItemType

				len = pItemInfo - pAwardEntry;
				Set16(pAwardEntry, (WORD)len);
				pAwardEntry += len;
			}
			Write32(pAwardEntry, 0); // Coupons
			Write32(pAwardEntry, 0); // GuildPoint
			Write32(pAwardEntry, 0); // LuckMoney
			Write8(pAwardEntry, 0);	 // ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardEntry;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardEntry;
				Set16(pAwardEntry, (WORD)len);
				pAwardEntry += len;
			}
			Write32(pAwardEntry, 0); // SpeedCoin

			len = pAwardEntry - pTreasureBoxToClient;
			Set16(pTreasureBoxToClient, (WORD)len);
			pTreasureBoxToClient += len;
		}

		len = pTreasureBoxToClient - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // maxSelectNum
	Write8(p, 0);  // Contex
	Write32(p, 0); // OpenDate
	Write32(p, 0); // CloseDate
	Write32(p, 0); // OpenTime
	Write32(p, 0); // CloseTime

	len = p - buf;
	SendToClient(Client, 190, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetNewHandCritBoxAwardCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ItemID = Read32(p);
	ResponseGetNewHandCritBoxAwardCfg(Client, ItemID);
}

void ResponseGetNewHandCritBoxAwardCfg(ClientNode *Client, int itemID)
{
	char FilePath[MAX_PATH];
	sprintf_s(FilePath, ".\\Gift\\%d.yml", itemID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);
	YAML::Node GiftConfig;
	if (isFileExists)
	{
		GiftConfig = YAML::LoadFile(FilePath);
		if (GiftConfig["Type"].as<INT>() != 3)
		{
			return;
		}
	}
	else
	{
		return;
	}
	ListInfo *aItemStatus = (ListInfo *)malloc(sizeof(ListInfo) * 100);
	bool falg = true;
	int n = 0;
	int rate = 0;
	while (falg)
	{
		std::string num = std::to_string(n + 1);
		if (GiftConfig["List" + num])
		{
			rate += GiftConfig["List" + num]["Rate"].as<INT>();
			aItemStatus[n].Rate = rate;
			aItemStatus[n].No = n + 1;
			n++;
		}
		else
		{
			falg = false;
		}
	}
	INT ItemIDNum = 0;
	for (int i = 0; i < n; i++)
	{
		std::string NO = std::to_string(aItemStatus[i].No);
		ItemIDNum += GiftConfig["List" + NO]["Items"].size();
	}
	if (ItemIDNum > 8)
	{
		ItemIDNum = 8;
	}
	int count = 0;

	ItemInfo *aItem = (ItemInfo *)malloc(sizeof(ItemInfo) * ItemIDNum);
	for (int i = 0; i < 3; i++)
	{
		if (count >= 8)
		{
			break;
		}
		std::string num = std::to_string(aItemStatus[i].No);
		YAML::Node Config = GiftConfig["List" + num]["Items"];
		for (int j = 0; j < Config.size(); j++)
		{
			if (count >= 8)
			{
				break;
			}
			aItem[count].ItemID = Config[j]["ItemId"].as<UINT>();
			aItem[count].ItemNum = Config[j]["ItemNum"].as<UINT>();
			aItem[count].ItemType = Config[j]["ItemType"].as<UINT>();
			aItem[count].AvailPeriod = Config[j]["AvailPeriod"].as<INT>();
			count++;
		}
	}
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultId
	Write32(p, Client->Uin); // Uin
	Write16(p, ItemIDNum);	 // ItemNum
	for (size_t i = 0; i < 8; i++)
	{ // LottoryAwardItem
		BYTE *pLottoryAwardItem = p;
		Write16(pLottoryAwardItem, 0); // len

		Write32(pLottoryAwardItem, aItem[i].ItemID);	  // ItemID
		Write32(pLottoryAwardItem, aItem[i].ItemNum);	  // Num
		Write32(pLottoryAwardItem, aItem[i].AvailPeriod); // Period
		Write16(pLottoryAwardItem, aItem[i].ItemType);	  // ItemType

		len = pLottoryAwardItem - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // MultiExtraAwardItem
		BYTE *pMultiExtraAwardItem = p;
		Write16(pMultiExtraAwardItem, 0); // len

		Write32(pMultiExtraAwardItem, aItem[0].ItemID);		 // ItemID
		Write32(pMultiExtraAwardItem, aItem[0].ItemNum);	 // Num
		Write32(pMultiExtraAwardItem, aItem[0].AvailPeriod); // Period
		Write16(pMultiExtraAwardItem, aItem[0].ItemType);	 // ItemType

		len = pMultiExtraAwardItem - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 10);		// LuckValue
	Write32(p, 100);	// MaxLuckValue
	Write32(p, itemID); // ItemID

	len = p - buf;
	SendToClient(Client, 313, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetCommodityExchangeItems(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
printf("Request_171	Hex:%s\n", pMsg);*/
	// Hex:00002713 025C3F31 00000190 0000
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	int ExchangeID = Read32(Body);

	Write16(p, 0);			// ResultID
	Write32(p, Uin);		// Uin
	Write32(p, ExchangeID); // ExchangeID
	Write16(p, 1);			// ItemNum
	for (size_t i = 0; i < 1; i++)
	{ // ItemInfo  兑换所需道具信息
		BYTE *pItemInfo = p;
		Write16(pItemInfo, 0); // len

		Write32(pItemInfo, 95456); // ItemID
		Write8(pItemInfo, 0);	   // ItemType
		Write32(pItemInfo, 1);	   // NeedQuantity
		Write32(pItemInfo, 1);	   // Quantity

		len = pItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // ReasonLen
	Write32(p, 0); // ConsumeFlag
	Write8(p, 0);  // ProduceItemNum
	for (size_t i = 0; i < 0; i++)
	{ // ProduceItemInfo
		BYTE *pProduceItemInfo = p;
		Write16(pProduceItemInfo, 0); // len

		Write32(pProduceItemInfo, 0); // ItemID
		Write32(pProduceItemInfo, 0); // ItemNum
		Write32(pProduceItemInfo, 0); // AvailPeriod
		Write8(pProduceItemInfo, 0);  // Status
		Write32(pProduceItemInfo, 0); // ObtainTime
		Write32(pProduceItemInfo, 0); // OtherAttribute
		Write16(pProduceItemInfo, 0); // ItemType

		len = pProduceItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 171, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void ResponseGetCommodityExchangeItems(ClientNode *Client)
{
	int n = 1;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, 1);			 // ExchangeID
	Write16(p, n);			 // ItemNum
	for (size_t i = 0; i < n; i++)
	{ // ItemInfo
		BYTE *pItemInfo = p;
		Write16(pItemInfo, 0); // len

		Write32(pItemInfo, 10020); // ItemID
		Write8(pItemInfo, 1);	   // ItemType
		Write32(pItemInfo, 100);   // NeedQuantity
		Write32(pItemInfo, 100);   // Quantity

		len = pItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // ReasonLen
	Write32(p, 1); // ConsumeFlag
	Write8(p, n);  // ProduceItemNum
	for (size_t i = 0; i < n; i++)
	{ // ProduceItemInfo
		BYTE *pProduceItemInfo = p;
		Write16(pProduceItemInfo, 0); // len

		Write32(pProduceItemInfo, 10020); // ItemID
		Write32(pProduceItemInfo, 1);	  // ItemNum
		Write32(pProduceItemInfo, -1);	  // AvailPeriod
		Write8(pProduceItemInfo, 0);	  // Status
		Write32(pProduceItemInfo, 0);	  // ObtainTime
		Write32(pProduceItemInfo, 0);	  // OtherAttribute
		Write16(pProduceItemInfo, 1);	  // ItemType

		len = pProduceItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 171, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestExchange(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ExchangeID = Read32(p);
	int ExchangeID1 = Read32(p);
	int ExchangeID2 = Read32(p);
	ResponseExchange(Client, Time, ExchangeID);
}
int generateRandomNumber()
{
	srand(time(NULL));
	return rand() % 16;
}
void ResponseExchange(ClientNode *Client, UINT Time, int ExchangeID) // 幸运币道具抽奖项
{
#ifdef DEBUG
	Z_INFO("Request_160	ExchangeID:%d\n", ExchangeID);
#endif

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	Write16(p, 0); // ResultID
	Write8(p, 0);  // ReasonLen
	Write8(p, 1);  // LuckAwardNum
	// 奖品道具获得
	int awardPara[] = {12525, 24640, 12720, 129287, 19615, 19614, 19300, 19299, 21319, 21901, 21907, 22097, 22795, 23234, 23374, 24146};
	char FilePath[MAX_PATH];
	sprintf_s(FilePath, ".\\Gift\\%d.yml", ExchangeID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);
	int z = 0;
	if (!isFileExists)
	{
		ResponseStartGame(Client, "未配置");
		return;
	}
	if (ExchangeID == 92 || ExchangeID == 93 || ExchangeID == 94)
	{
		z = generateRandomNumber();
	}
	else
	{
		ResponseStartGame(Client, "兑换成功");
	}
	YAML::Node GiftConfig = YAML::LoadFile(FilePath)["Items"];

	{ // LuckAwardList
		BYTE *pLuckAwardList = p;
		Write16(pLuckAwardList, 0); // len

		Write8(pLuckAwardList, 7);									 // AwardType
		Write32(pLuckAwardList, GiftConfig[z]["ItemId"].as<UINT>()); // AwardPara
		ItemInfo Items[3000];
		ItemInfo Item = PlayerDB_AddItem(Client->Uin, GiftConfig[z]["ItemId"].as<UINT>(), GiftConfig[z]["ItemNum"].as<UINT>(), GiftConfig[z]["AvailPeriod"].as<INT>(), false);
		Items[0].ItemID = GiftConfig[z]["ItemId"].as<UINT>();
		Items[0].ItemNum = Item.ItemNum;
		Items[0].AvailPeriod = Item.AvailPeriod;
		Items[0].Status = Item.Status;
		Items[0].ObtainTime = Item.ObtainTime;
		Items[0].OtherAttribute = Item.OtherAttribute;
		Items[0].ItemType = Item.ItemType;
		NotifyClientAddItem(Client, 1, Items);
		Write8(pLuckAwardList, z); // awardPosition
		Write8(pLuckAwardList, 0); // ReasonLen

		Write32(pLuckAwardList, GiftConfig[z]["ItemNum"].as<UINT>());	 // ItemNum
		Write32(pLuckAwardList, GiftConfig[z]["AvailPeriod"].as<INT>()); // AvailPeriod

		len = pLuckAwardList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 160, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestGetExchangeDesc(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	bool ExchangeNum = Read8(p);
	// int[] ExchangeIDs[] = Read32(p);
	ResponseGetExchangeDesc(Client);
}

void ResponseGetExchangeDesc(ClientNode *Client)
{
	int n = 1;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, 0); // ExchangeNum
	for (size_t i = 0; i < n; i++)
	{ // Desc
		BYTE *pDesc = p;
		Write16(pDesc, 0); // len

		Write32(pDesc, 0); // ExchangeID
		Write16(pDesc, 0); // DescLen

		len = pDesc - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 936, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetLuckAwardList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ExchangeID = Read32(p);
	ResponseGetLuckAwardList(Client, Time, ExchangeID);
}

void ResponseGetLuckAwardList(ClientNode *Client, UINT Time, int ExchangeID) // 幸运币道具抽奖显示
{
	int n = 16;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, Time);		 // Time
	Write32(p, ExchangeID);	 // ExchangeID
	Write8(p, n);			 // AwardNum

	// 奖品道具显示
	int awardPara[] = {12525, 24640, 12720, 129287, 19615, 19614, 19300, 19299, 21319, 21901, 21907, 22097, 22795, 23234, 23374, 24146};
	char FilePath[MAX_PATH];
	sprintf_s(FilePath, ".\\Gift\\%d.yml", ExchangeID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);
	if (!isFileExists)
	{
		ResponseStartGame(Client, "未配置");
		return;
	}
	YAML::Node GiftConfig = YAML::LoadFile(FilePath)["Items"];
	for (size_t i = 0; i < n; i++)
	{ // LuckAwardList
		BYTE *pLuckAwardList = p;
		Write16(pLuckAwardList, 0); // len

		Write8(pLuckAwardList, 7);										 // AwardType
		Write32(pLuckAwardList, GiftConfig[i]["ItemId"].as<UINT>());	 // AwardPara
		Write8(pLuckAwardList, i);										 // AwardPostion
		Write8(pLuckAwardList, 0);										 // AwardDesLen
		Write32(pLuckAwardList, GiftConfig[i]["ItemNum"].as<UINT>());	 // ItemNum
		Write32(pLuckAwardList, GiftConfig[i]["AvailPeriod"].as<INT>()); // AvailPeriod

		len = pLuckAwardList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 172, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestInvitePlayer(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	bool Type = Read8(p);
	if (GetClient(DstUin))
	{
		NotifyBeInvited(GetClient(DstUin), Client);
	}
	else
	{
		ResponseStartGame(Client, "该玩家不在线");
	}

	ResponseInvitePlayer(Client, DstUin, Type);
}

void ResponseInvitePlayer(ClientNode *Client, int dstuin, bool type)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int n = 5;
	Write16(p, 0);		// ResultID
	Write32(p, dstuin); // DstUin
	memcpy(p, "", MaxNickName);
	p += MaxNickName;
	Write8(p, type); // Type
	// RequestEnterOutdoor2(Client, dstuin - 1);
}
void RequestC2GGetPlayerStatus(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT Num = Read16(p);
	ItemStatus* aItemStatus = (ItemStatus*)malloc(sizeof(ItemStatus) * Num);
	for (size_t i = 0; i < Num; i++) {
		aItemStatus[i].ItemID = Read32(p);
	}
	ResponseC2GGetPlayerStatus(Client,Num, aItemStatus);
}

void ResponseC2GGetPlayerStatus(ClientNode *Client, USHORT Num, ItemStatus* aItemStatus)
{
	int n = 0;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, Num);			 // Num
	for (size_t i = 0; i < Num; i++)
	{ // PlayerStatus
		BYTE *pPlayerStatus = p;
		Write16(pPlayerStatus, 0); // len

		Write32(pPlayerStatus, aItemStatus[i].ItemID); // Uin
		if (GetClient(aItemStatus[i].ItemID)) {
			Write8(pPlayerStatus, 1);	   // IsOnLine
		}
		else {
			Write8(pPlayerStatus, 0);	   // IsOnLine
		}
		
		Write32(pPlayerStatus, 1);	   // PlayerStatus
		Write16(pPlayerStatus, 1);	   // ServerType
		Write8(pPlayerStatus, 1);	   // GVoiceState

		len = pPlayerStatus - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24053, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
/*void RequestGetKartTransformersInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT KartTransID = Read32(p);
	UINT KartID = Read32(p);
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int status;
	if (KartTransID == 96462) {//神影
		KartID = 34393;
	}
	else if (KartTransID == 95405) {//雷诺
		KartID = 12720;
	}
	sql = "SELECT ShapeSuitID FROM KartRefit WHERE Uin = ? AND KartID = ?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartID);
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			if (KartTransID == 96462) {//神影
				if (sqlite3_column_int(stmt, 0) == 96462) {
					status = 1;
				}
				else {
					status = 0;
				}
			}
			else if (KartTransID == 95405) {//雷诺

				if (sqlite3_column_int(stmt, 0) == 95405) {
					status = 1;
				}
				else {
					status = 0;
				}
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (KartTransID == 96462) {//神影
		if (status == 1) {
			ResponseSaveShapeRefit3(Client, KartID, 0);
			ResponseStartGame(Client, "剑尊神影已经取消激活");
		}
		else {
			ResponseSaveShapeRefit3(Client, KartID, 96462);
			ResponseStartGame(Client, "剑尊神影已经激活");
		}
	}
	else if (KartTransID == 95405) {//雷诺

		if (status == 1) {
			ResponseSaveShapeRefit3(Client, KartID, 0);
			ResponseStartGame(Client, "千机变雷诺已经取消激活");
		}
		else {
			ResponseSaveShapeRefit3(Client, KartID, 95405);
			ResponseStartGame(Client, "千机变雷诺已经激活");
		}
	}
}*/
void RequestGetKartTransformersInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT KartTransID = Read32(p);
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int type = 0; // 1 神影 2 雷诺
	INT KartID;
	INT Status;
	ItemStatus *aItemStatus = (ItemStatus *)malloc(sizeof(ItemStatus) * 2);
	sql = "SELECT ItemID,Status FROM ITEM WHERE Uin = ? AND ITEMID = ?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, KartTransID);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{

			KartID = sqlite3_column_int(stmt, 0);
			Status = sqlite3_column_int(stmt, 1);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (KartTransID == 96462)
	{ // 神影

		if (Status == 0)
		{
			ResponseStartGame(Client, "剑尊神影已经激活");
			aItemStatus[0].ItemID = 34393;
			aItemStatus[0].NewStatus = 0;
		}
		else
		{
			ResponseStartGame(Client, "剑尊神影已经取消激活");
			aItemStatus[0].ItemID = 34393;
			aItemStatus[0].NewStatus = 1;
		}
	}
	else if (KartTransID == 95405)
	{ // 雷诺
		if (Status == 0)
		{
			ResponseStartGame(Client, "千机变雷诺已经激活");
			aItemStatus[0].ItemID = 12720;
			aItemStatus[0].NewStatus = 0;
		}
		else
		{
			ResponseStartGame(Client, "千机变雷诺已经取消激活");
			aItemStatus[0].ItemID = 12720;
			aItemStatus[0].NewStatus = 1;
		}
	}
	sql = "UPDATE ITEM SET STATUS = ? WHERE Uin=? and ItemID = ?";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		if (Status == 0)
		{
			sqlite3_bind_int(stmt, 1, 1);

			aItemStatus[1].ItemID = KartID;
			aItemStatus[1].NewStatus = 1;
			ResponseChangeItemStatus(Client, 2, aItemStatus);
		}
		else if (Status == 1)
		{
			sqlite3_bind_int(stmt, 1, 0);
			aItemStatus[1].ItemID = KartID;
			aItemStatus[1].NewStatus = 0;
			ResponseChangeItemStatus(Client, 2, aItemStatus);
		}
		sqlite3_bind_int(stmt, 2, Client->Uin);
		sqlite3_bind_int(stmt, 3, KartID);
		result = sqlite3_step(stmt);
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}

	sqlite3_finalize(stmt);
	stmt = NULL;
}

void ResponseGetKartTransformersInfo(ClientNode *Client, UINT KartTransID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	int n = 4;
	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write32(p, KartTransID); // KartTransID
	Write16(p, 5);			 // NameLen
	Write32(p, 5);			 // CostItemID
	Write16(p, 5);			 // OneCostNums
	Write16(p, 5);			 // FiveCostNums
	Write16(p, 5);			 // PartNums
	for (size_t i = 0; i < n; i++)
	{ // KartTransPart
		BYTE *pKartTransPart = p;
		Write16(pKartTransPart, 0); // len

		Write16(pKartTransPart, 0);	 // IndexID
		Write16(pKartTransPart, 0);	 // NameLen
		Write16(pKartTransPart, 10); // DescLen
		Write16(pKartTransPart, 10); // MaxLevel
		Write16(pKartTransPart, 10); // UserLevel

		len = pKartTransPart - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // KartTransAward
		BYTE *pKartTransAward = p;
		Write16(pKartTransAward, 0); // len

		Write32(pKartTransAward, 0); // Exp
		Write32(pKartTransAward, 0); // Money
		Write32(pKartTransAward, 0); // SuperMoney
		Write16(pKartTransAward, n); // ItemNum
		for (size_t i = 0; i < n; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pKartTransAward;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 10020); // ItemID
			Write32(pItemInfo, 1);	   // ItemNum
			Write32(pItemInfo, -1);	   // AvailPeriod
			Write8(pItemInfo, 0);	   // Status
			Write32(pItemInfo, 0);	   // ObtainTime
			Write32(pItemInfo, 0);	   // OtherAttribute
			Write16(pItemInfo, 0);	   // ItemType

			len = pItemInfo - pKartTransAward;
			Set16(pKartTransAward, (WORD)len);
			pKartTransAward += len;
		}
		Write32(pKartTransAward, 0); // Coupons
		Write32(pKartTransAward, 0); // GuildPoint
		Write32(pKartTransAward, 0); // LuckMoney
		Write8(pKartTransAward, n);	 // ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pKartTransAward;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pKartTransAward;
			Set16(pKartTransAward, (WORD)len);
			pKartTransAward += len;
		}
		Write32(pKartTransAward, 0); // SpeedCoin

		len = pKartTransAward - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 28331, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestCompositeSkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int SkillStoneID = Read32(p);
	int LuckyStoneID = Read32(p);
	int BlessStoneID = Read32(p);
	// ResponseCompositeSkillStone(Client,  SkillStoneID, LuckyStoneID,BlessStoneID);
}

// void ResponseCompositeSkillStone(ClientNode* Client,int SkillStoneID,int LuckyStoneID,int BlessStoneID)
//{
//	BYTE buf[8192]; BYTE* p = buf; size_t len;
//	Write32(p, 0); //ResultID
//	Write32(p, 0); //ReasonLen
//	Write32(p, SkillStoneID); //SkillStoneID
//	Write32(p, LuckyStoneID); //LuckyStoneID
//	Write32(p, BlessStoneID); //BlessStoneID
//	Write32(p, 1); //DecSkillStoneNum
//	Write32(p, 0); //GenStillStoneID
//	Write32(p, 0); //ExperienceCompensation
//	Write8(p, 0); //LeftSkillStoneNumOnFailed_ModifyType
//
//	len = p - buf;
//	SendToClient(Client, 905, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
// }
void ResponseCompositeSkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	// Request_905     Hex:0000271408E1E196000055E00000000000000000F036
	// Request_905     Hex:0000271408E5F0F4000055E00000563C00005639708C
	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("Request_905	Hex:%s\n", pMsg);*/
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	LPUSyntheticStoneDef USyntheticStoneDef = reinterpret_cast<LPUSyntheticStoneDef>(Body);

	UINT Uin = DWORDSWAP(USyntheticStoneDef->Uin);
	UINT SkillStoneID = DWORDSWAP(USyntheticStoneDef->SkillStoneID);
	UINT LuckyStoneID = DWORDSWAP(USyntheticStoneDef->LuckyStoneID);
	UINT BlessStoneID = DWORDSWAP(USyntheticStoneDef->BlessStoneID);
	unsigned int ItemRarityProbability = 0;
	UINT GenSkillStoneID = 0;
	UINT Lv = 0;
	UINT ProbabilityList[] = {
		80, // 1-2
		60, // 2-3
		20, // 3-4
		10, // 4-5
		5,	// 5-6
	};
	if (SkillStoneID >= 1)
	{
		if (!NotifyUseItem(Client, Client->Uin, SkillStoneID, 4))
		{
			ResponseStartGame(Client, "客户端数据异常...");
			return;
		}
	}
	if (LuckyStoneID >= 1)
	{
		if (!NotifyUseItem(Client, Client->Uin, LuckyStoneID, 1))
		{
			ResponseStartGame(Client, "客户端数据异常...");
			return;
		}
		ItemRarityProbability += 2; // 使用幸运宝石 +百分之2 成功率
	}
	if (BlessStoneID >= 1)
	{
		if (!NotifyUseItem(Client, Client->Uin, BlessStoneID, 1))
		{
			ResponseStartGame(Client, "客户端数据异常...");
			return;
		}
		ItemRarityProbability += 3; // 使用祝福宝石 +百分之3 成功率
	}
	switch (SkillStoneID)
	{
	case 21983:
		GenSkillStoneID = 21984;
		Lv = 1;
		break; // 重生宝珠LV1
	case 21984:
		GenSkillStoneID = 21985;
		Lv = 2;
		break; // 重生宝珠LV2
	case 21985:
		GenSkillStoneID = 21986;
		Lv = 3;
		break; // 重生宝珠LV3
	case 21986:
		GenSkillStoneID = 21987;
		Lv = 4;
		break; // 重生宝珠LV4
	case 21987:
		GenSkillStoneID = 21988;
		Lv = 5;
		break; // 重生宝珠LV5

	case 22037:
		GenSkillStoneID = 22038;
		Lv = 1;
		break; // 攻击之钻LV1
	case 22038:
		GenSkillStoneID = 22039;
		Lv = 2;
		break; // 攻击之钻LV2
	case 22039:
		GenSkillStoneID = 22040;
		Lv = 3;
		break; // 攻击之钻LV3
	case 22040:
		GenSkillStoneID = 22041;
		Lv = 4;
		break; // 攻击之钻LV4
	case 22041:
		GenSkillStoneID = 22042;
		Lv = 5;
		break; // 攻击之钻LV5

	case 22043:
		GenSkillStoneID = 22044;
		Lv = 1;
		break; // 复制之钻LV1
	case 22044:
		GenSkillStoneID = 22045;
		Lv = 2;
		break; // 复制之钻LV2
	case 22045:
		GenSkillStoneID = 22046;
		Lv = 3;
		break; // 复制之钻LV3
	case 22046:
		GenSkillStoneID = 22047;
		Lv = 4;
		break; // 复制之钻LV4
	case 22047:
		GenSkillStoneID = 22048;
		Lv = 5;
		break; // 复制之钻LV5

	case 22031:
		GenSkillStoneID = 22032;
		Lv = 1;
		break; // 闪电之钻LV1
	case 22032:
		GenSkillStoneID = 22033;
		Lv = 2;
		break; // 闪电之钻LV2
	case 22033:
		GenSkillStoneID = 22034;
		Lv = 3;
		break; // 闪电之钻LV3
	case 22034:
		GenSkillStoneID = 22035;
		Lv = 4;
		break; // 闪电之钻LV4
	case 22035:
		GenSkillStoneID = 22036;
		Lv = 5;
		break; // 闪电之钻LV5

	case 22025:
		GenSkillStoneID = 22026;
		Lv = 1;
		break; // 复仇之钻LV1
	case 22026:
		GenSkillStoneID = 22027;
		Lv = 2;
		break; // 复仇之钻LV2
	case 22027:
		GenSkillStoneID = 22028;
		Lv = 3;
		break; // 复仇之钻LV3
	case 22028:
		GenSkillStoneID = 22029;
		Lv = 4;
		break; // 复仇之钻LV4
	case 22029:
		GenSkillStoneID = 22030;
		Lv = 5;
		break; // 复仇之钻LV5

	case 22013:
		GenSkillStoneID = 22014;
		Lv = 1;
		break; // 后发宝珠LV1
	case 22014:
		GenSkillStoneID = 22015;
		Lv = 2;
		break; // 后发宝珠LV2
	case 22015:
		GenSkillStoneID = 22016;
		Lv = 3;
		break; // 后发宝珠LV3
	case 22016:
		GenSkillStoneID = 22017;
		Lv = 4;
		break; // 后发宝珠LV4
	case 22017:
		GenSkillStoneID = 22018;
		Lv = 5;
		break; // 后发宝珠LV5

	case 21989:
		GenSkillStoneID = 21990;
		Lv = 1;
		break; // 坚韧宝珠LV1
	case 21990:
		GenSkillStoneID = 21991;
		Lv = 2;
		break; // 坚韧宝珠LV2
	case 21991:
		GenSkillStoneID = 21992;
		Lv = 3;
		break; // 坚韧宝珠LV3
	case 21992:
		GenSkillStoneID = 21993;
		Lv = 4;
		break; // 坚韧宝珠LV4
	case 21993:
		GenSkillStoneID = 21994;
		Lv = 5;
		break; // 坚韧宝珠LV5

	case 21977:
		GenSkillStoneID = 21978;
		Lv = 1;
		break; // 效率宝珠LV1
	case 21978:
		GenSkillStoneID = 21979;
		Lv = 2;
		break; // 效率宝珠LV2
	case 21979:
		GenSkillStoneID = 21980;
		Lv = 3;
		break; // 效率宝珠LV3
	case 21980:
		GenSkillStoneID = 21981;
		Lv = 4;
		break; // 效率宝珠LV4
	case 21981:
		GenSkillStoneID = 21982;
		Lv = 5;
		break; // 效率宝珠LV5

	default:
		++GenSkillStoneID;
		Lv = 5;
		break;
	}
	unsigned short status = 0;
	unsigned int ExperienceCompensation = 0;		  // 补偿的经验
	ItemRarityProbability += ProbabilityList[Lv - 1]; // 宝石合成成功概率
	RandomInt RandObj;
	RandObj.set(0, 100);
	unsigned int RandomCalue = RandObj.get();
	if (RandomCalue <= ItemRarityProbability)
	{
		// 合成成功处理 逻辑
		status = 0;
		ItemInfo Items = PlayerDB_AddItem(Client->Uin, GenSkillStoneID, 1, 0, false); // 数据库添加道具
		NotifyClientAddItem(Client, 1, &Items);										  // 通知客户添加道具
	}
	else
	{
		status = 1;
		GenSkillStoneID = 0;
		ExperienceCompensation = 2000;
	}
	Write16(p, status);					// ResultID  status
	Write8(p, 0);						// ReasonLen
	Write32(p, SkillStoneID);			// SkillStoneID	技能石ID
	Write32(p, LuckyStoneID);			// LuckyStoneID  幸运
	Write32(p, BlessStoneID);			// BlessStoneID  祝福
	Write32(p, 4);						// DecSkillStoneNum 减去的宝石数量
	Write32(p, GenSkillStoneID);		// GenSkillStoneID 合成的宝石ID
	Write32(p, ExperienceCompensation); // ExperienceCompensation补偿的经验
	Write8(p, 0);						// LeftSkillStoneNumOnFailed_ModifyType 祝福类型

	len = p - buf;
	SendToClient(Client, 905, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
bool NotifyUseItem(ClientNode *Client, UINT CUin, UINT ItemID, int UseItemNum)
{
	const UINT Uin = CUin;
	const UINT ItemId = ItemID;
	const int uItemNum = UseItemNum;
	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;

	int ItemNum = 0;
	// 获取道具数量
	sql = "SELECT ItemNum from Item WHERE Uin=? AND ItemID=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, ItemId);
		if (sqlite3_step(stmt) == SQLITE_ROW)
			ItemNum = sqlite3_column_int(stmt, 0);
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	if (ItemNum >= 1)
	{
		// 扣除道具
		ItemNum -= uItemNum;
		if (ItemNum <= 0)
		{ // Delete Item
			sql = "DELETE FROM Item  WHERE Uin=? AND ItemID=?;";
			if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, ItemId);
				sqlite3_step(stmt);
			}
		}
		else
		{
			sql = "UPDATE Item SET ItemNum=ItemNum-? WHERE Uin = ? AND ItemID = ?;";
			if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, uItemNum);
				sqlite3_bind_int(stmt, 2, Uin);
				sqlite3_bind_int(stmt, 3, ItemId);
				sqlite3_step(stmt);
			}
		}
		sqlite3_finalize(stmt);
		stmt = nullptr;
		// 通知客户端扣除道具
		NotifyUseItemResult(Client, ItemId, uItemNum, ItemNum);
		return true;
	}
	return false;
}
void RequestGetMagicHouseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{

	/*char* pMsg = EByteToHexUA2((const unsigned char*)Body, BodyLen);
	printf("ResponseGetMagicHouseInfo	Hex:%s\n", pMsg);*/

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT MagicHouseID = Read32(Body);
	// printf("ResponseGetMagicHouseInfo MagicHouseID:%d\n", MagicHouseID);

	Write32(p, Uin);		// Uin
	Write16(p, 0);			// ResultID
	Write8(p, 1);			// OpenStat
	Write8(p, 0);			// ReceiveAllAward
	Write32(p, 1678084039); // OpenStartTime  开放开始时间
	Write32(p, 3382237639); // OpenEndTime    打开结束时间
	Write16(p, 12);			// AwardNums
	for (size_t i = 0; i < 12; i++)
	{ // AwardItemInfo
		BYTE *pAwardItemInfo = p;
		Write16(pAwardItemInfo, 0); // len

		Write32(pAwardItemInfo, 98263); // ItemID
		Write32(pAwardItemInfo, 1);		// ItemNums
		Write32(pAwardItemInfo, -1);	// AvailPeriod
		Write16(pAwardItemInfo, 0);		// Received    被承认的
		Write16(pAwardItemInfo, i);		// AwardIndex
		Write16(pAwardItemInfo, 0);		// ItemType

		len = pAwardItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 5);			  // CostCallOnItemNums  限时折扣张数 开启需要的道具数量
	Write32(p, MagicHouseID); // MagicHouseID
	Write32(p, 0);			  // TimeRemain 折扣多少秒后消失
	Write32(p, 0);			  // SaveItemNum //福利 总共节省召唤卡 张数
	Write8(p, 1);			  // TipsFlag 未知 不是1 会出现 福利提示
	Write32(p, 5);			  // CostCallOnItemNumsBeforDiscount 本次参与需要的召唤卡数量
	Write32(p, 83073);		  // CallOnItemID

	len = p - buf;
	SendToClient(Client, 24470, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void ResponseGetMagicHouseInfo(ClientNode *Client, USHORT MagicHouseID)
{
	int n = 12;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write8(p, 0);			 // OpenStat
	Write8(p, 0);			 // ReceiveAllAward
	Write32(p, 0);			 // OpenStartTime
	Write32(p, 0);			 // OpenEndTime
	Write16(p, n);			 // AwardNums

	int MagicCar[] = {12525, 24640, 12720, 129287, 19615, 19614, 19300, 19299, 21319, 21901, 21907, 22097};

	for (size_t i = 0; i < sizeof(MagicCar) / sizeof(MagicCar[0]); i++)
	{ // AwardItemInfo
		BYTE *pAwardItemInfo = p;
		Write16(pAwardItemInfo, 0); // len

		Write32(pAwardItemInfo, MagicCar[i]); // ItemID
		Write32(pAwardItemInfo, 1);			  // ItemNums
		Write32(pAwardItemInfo, -1);		  // AvailPeriod
		Write16(pAwardItemInfo, 0);			  // Received   奖励是否已被领取
		Write16(pAwardItemInfo, i);			  // AwardIndex  代表奖励索引
		Write16(pAwardItemInfo, 3);			  // ItemType 奖励物品的类型

		len = pAwardItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 100); // CostCallOnItemNums
	// 小橘子神秘宝盒
	Write16(p, 1);		// MagicHouseID
	Write32(p, 100);	// TimeRemain
	Write32(p, 100);	// SaveItemNum
	Write8(p, 100);		// TipsFlag
	Write32(p, 10);		// CostCallOnItemNumsBeforDiscount
	Write32(p, 127748); // CallOnItemID

	len = p - buf;
	SendToClient(Client, 24470, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetRankedMatchShowInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UCHAR Type = Read8(p);
	ResponseGetRankedMatchShowInfo(Client);
}
void ResponseGetRankedMatchShowInfo(ClientNode *Client)
{
	int n = 0;
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	if (buf == nullptr)
	{
		return;
	}
	UINT SeasonID = 0;
	UINT ClientVersion =  Client->ClientVersion;
	if (Client->ClientVersion == 18812) {
		SeasonID = 36;
	}
	else if (Client->ClientVersion >= 18812) {
		SeasonID = 37;
	}
	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write32(p, SeasonID);			 // SeasonID
	Write16(p, 1);			 // DailyOpenTimeInfoNums
	for (size_t i = 0; i < 1; i++)
	{ // OpenTimeInfos
		BYTE *pOpenTimeInfos = p;
		Write16(pOpenTimeInfos, 0); // len

		Write32(pOpenTimeInfos, 0); // OpenTime
		Write32(pOpenTimeInfos, 0); // CloseTime

		len = pOpenTimeInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // SeasonOpenDate
	Write32(p, 0); // SeasonCloseDate
	{// RankedMatchGradeInfo
		BYTE *pRankedMatchGradeInfo = p;
		Write16(pRankedMatchGradeInfo, 0); // len

		Write32(pRankedMatchGradeInfo, 27);	  // GradeLevel
		Write32(pRankedMatchGradeInfo, 0);	  // ChildLevel
		Write32(pRankedMatchGradeInfo, 100); // GradeScore
		Write32(pRankedMatchGradeInfo, 100); // MaxGradeScore
		Write32(pRankedMatchGradeInfo, 100); // TotalScore

		len = pRankedMatchGradeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // NextLevelReceAwardInfo
		BYTE *pNextLevelReceAwardInfo = p;
		Write16(pNextLevelReceAwardInfo, 0); // len

		{ // AwardInfo
			BYTE *pAwardInfo = pNextLevelReceAwardInfo;
			Write16(pAwardInfo, 0); // len

			Write32(pAwardInfo, 100); // Exp
			Write32(pAwardInfo, 100); // Money
			Write32(pAwardInfo, 100); // SuperMoney
			Write16(pAwardInfo, 1);	  // ItemNum
			for (size_t i = 0; i < 1; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 10020); // ItemID
				Write32(pItemInfo, 1);	   // ItemNum
				Write32(pItemInfo, -1);	   // AvailPeriod
				Write8(pItemInfo, 0);	   // Status
				Write32(pItemInfo, 0);	   // ObtainTime
				Write32(pItemInfo, 0);	   // OtherAttribute
				Write16(pItemInfo, 0);	   // ItemType

				len = pItemInfo - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0);	  // Coupons
			Write32(pAwardInfo, 100); // GuildPoint
			Write32(pAwardInfo, 10);  // LuckMoney
			Write8(pAwardInfo, 0);	  // ExtendInfoNum
			for (size_t i = 0; i < 0; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardInfo;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 2000); // SpeedCoin

			len = pAwardInfo - pNextLevelReceAwardInfo;
			Set16(pNextLevelReceAwardInfo, (WORD)len);
			pNextLevelReceAwardInfo += len;
		}
		Write32(pNextLevelReceAwardInfo, 1);   // GradeLevel
		Write32(pNextLevelReceAwardInfo, 1);   // ChildLevel
		Write32(pNextLevelReceAwardInfo, 100); // FinishMatchNums

		len = pNextLevelReceAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // SeasonAwardInfo
		BYTE *pSeasonAwardInfo = p;
		Write16(pSeasonAwardInfo, 0); // len

		Write32(pSeasonAwardInfo, 10); // Exp
		Write32(pSeasonAwardInfo, 10); // Money
		Write32(pSeasonAwardInfo, 10); // SuperMoney
		Write16(pSeasonAwardInfo, 1);  // ItemNum
		for (size_t i = 0; i < 1; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pSeasonAwardInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 10020); // ItemID
			Write32(pItemInfo, 1);	   // ItemNum
			Write32(pItemInfo, -1);	   // AvailPeriod
			Write8(pItemInfo, 0);	   // Status
			Write32(pItemInfo, 0);	   // ObtainTime
			Write32(pItemInfo, 0);	   // OtherAttribute
			Write16(pItemInfo, 0);	   // ItemType

			len = pItemInfo - pSeasonAwardInfo;
			Set16(pSeasonAwardInfo, (WORD)len);
			pSeasonAwardInfo += len;
		}
		Write32(pSeasonAwardInfo, 0);	// Coupons
		Write32(pSeasonAwardInfo, 100); // GuildPoint
		Write32(pSeasonAwardInfo, 10);	// LuckMoney
		Write8(pSeasonAwardInfo, 0);	// ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pSeasonAwardInfo;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pSeasonAwardInfo;
			Set16(pSeasonAwardInfo, (WORD)len);
			pSeasonAwardInfo += len;
		}
		Write32(pSeasonAwardInfo, 100); // SpeedCoin

		len = pSeasonAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // AnotherSeasonAwardInfo
		BYTE *pAnotherSeasonAwardInfo = p;
		Write16(pAnotherSeasonAwardInfo, 0); // len

		Write32(pAnotherSeasonAwardInfo, 10);  // Exp
		Write32(pAnotherSeasonAwardInfo, 100); // Money
		Write32(pAnotherSeasonAwardInfo, 10);  // SuperMoney
		Write16(pAnotherSeasonAwardInfo, 1);   // ItemNum
		for (size_t i = 0; i < 1; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAnotherSeasonAwardInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 10020); // ItemID
			Write32(pItemInfo, 1);	   // ItemNum
			Write32(pItemInfo, -1);	   // AvailPeriod
			Write8(pItemInfo, 0);	   // Status
			Write32(pItemInfo, 0);	   // ObtainTime
			Write32(pItemInfo, 0);	   // OtherAttribute
			Write16(pItemInfo, 0);	   // ItemType

			len = pItemInfo - pAnotherSeasonAwardInfo;
			Set16(pAnotherSeasonAwardInfo, (WORD)len);
			pAnotherSeasonAwardInfo += len;
		}
		Write32(pAnotherSeasonAwardInfo, 0); // Coupons
		Write32(pAnotherSeasonAwardInfo, 0); // GuildPoint
		Write32(pAnotherSeasonAwardInfo, 0); // LuckMoney
		Write8(pAnotherSeasonAwardInfo, 0);	 // ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAnotherSeasonAwardInfo;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAnotherSeasonAwardInfo;
			Set16(pAnotherSeasonAwardInfo, (WORD)len);
			pAnotherSeasonAwardInfo += len;
		}
		Write32(pAnotherSeasonAwardInfo, 200); // SpeedCoin

		len = pAnotherSeasonAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // SeasonMapNums
	for (size_t i = 0; i < n; i++)
	{ // MapInfos
		BYTE *pMapInfos = p;
		Write16(pMapInfos, 0); // len

		Write16(pMapInfos, 0); // GradeMapNums
		for (size_t i = 0; i < 0; i++)
		{ // MapInfo
			BYTE *pMapInfo = pMapInfos;
			Write16(pMapInfo, 0); // len

			Write16(pMapInfo, 0); // GradeLevel
			Write16(pMapInfo, 0); // MapNums
			Write32(pMapInfo, 0); // MapArray[]
			Write16(pMapInfo, 0); // SuperRaceMapNums
			Write32(pMapInfo, 0); // SuperRaceMapArray[]
			Write16(pMapInfo, 0); // CarFactoryMapNums
			Write32(pMapInfo, 0); // CarFactoryMapArray[]
			Write16(pMapInfo, 0); // NewMapNums
			Write32(pMapInfo, 0); // NewMapArray[]

			len = pMapInfo - pMapInfos;
			Set16(pMapInfos, (WORD)len);
			pMapInfos += len;
		}

		len = pMapInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // PlayerSeasonInfoNums
	for (size_t i = 0; i < n; i++)
	{ // PlayerSeasonInfos
		BYTE *pPlayerSeasonInfos = p;
		Write16(pPlayerSeasonInfos, 0); // len

		Write16(pPlayerSeasonInfos, 0); // SeasonID
		Write16(pPlayerSeasonInfos, 0); // GradeLevel
		Write8(pPlayerSeasonInfos, 0);	// ChildLevel
		Write16(pPlayerSeasonInfos, 0); // HighGradeLevel
		Write8(pPlayerSeasonInfos, 0);	// HighChildLevel
		Write8(pPlayerSeasonInfos, 0);	// SeasonInfoProcesedTag
		Write8(pPlayerSeasonInfos, 0);	// SeasonComputeInfoTipTag
		Write16(pPlayerSeasonInfos, 0); // SeasonInheritTag
		Write8(pPlayerSeasonInfos, 0);	// SeasonInheritTipTag
		Write16(pPlayerSeasonInfos, 0); // SpeedGodNums
		Write32(pPlayerSeasonInfos, 0); // SpeedRoundNums
		Write32(pPlayerSeasonInfos, 0); // SpeedFinishRoundNums
		Write32(pPlayerSeasonInfos, 0); // TotalRankIndex
		Write32(pPlayerSeasonInfos, 0); // ContiFinishRoundNums
		Write32(pPlayerSeasonInfos, 0); // SpeedKillAllRoundNums
		Write32(pPlayerSeasonInfos, 0); // PropRoundNums
		Write32(pPlayerSeasonInfos, 0); // PropFinishRoundNums
		Write32(pPlayerSeasonInfos, 0); // ContiFinishPropRoundNums
		Write32(pPlayerSeasonInfos, 0); // PropKillAllRoundNums
		Write8(pPlayerSeasonInfos, 0);	// LastRoundIndex
		Write32(pPlayerSeasonInfos, 0); // OpenDate
		Write32(pPlayerSeasonInfos, 0); // CloseDate
		Write32(pPlayerSeasonInfos, 0); // HistroyContFinishRoundNums

		len = pPlayerSeasonInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // RankedMatchPromoInfo
		BYTE *pRankedMatchPromoInfo = p;
		Write16(pRankedMatchPromoInfo, 0); // len

		Write8(pRankedMatchPromoInfo, 0);  // PromotionMatch
		Write32(pRankedMatchPromoInfo, 0); // PromotionRoundNums
		Write32(pRankedMatchPromoInfo, 0); // PromotionSuccessRoundNums
		Write32(pRankedMatchPromoInfo, 0); // PromotionAlreadySuccessRoundNums
		Write32(pRankedMatchPromoInfo, 0); // PromotionRecordTag
		Write32(pRankedMatchPromoInfo, 0); // CurrentPromotionRoundNums
		Write32(pRankedMatchPromoInfo, 0); // PromoFailedMinusScore
		Write32(pRankedMatchPromoInfo, 0); // PromoSuccAddScore
		Write8(pRankedMatchPromoInfo, 0);  // PromoDirectly
		Write8(pRankedMatchPromoInfo, 0);  // GradeNoNeedPromo
		Write8(pRankedMatchPromoInfo, 0);  // RankedMemoryNoNeedPromo
		Write8(pRankedMatchPromoInfo, 0);  // PromotionExtraScore
		Write8(pRankedMatchPromoInfo, 0);  // PromtRaceFailedDefendFlag

		len = pRankedMatchPromoInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // InhertGradeTag
	{			  // GradeInheritInfo
		BYTE *pGradeInheritInfo = p;
		Write16(pGradeInheritInfo, 0); // len

		Write16(pGradeInheritInfo, 1); // OriginalGradeLevel
		Write16(pGradeInheritInfo, 1); // OriginalChildLevel
		Write16(pGradeInheritInfo, 2); // InheritedGradeLevel
		Write16(pGradeInheritInfo, 2); // InheritedChildLevel
		Write16(pGradeInheritInfo, 2); // InheritedLowLimitGradeLevel
		Write16(pGradeInheritInfo, 2); // InheritedLowLimitChildLevel

		len = pGradeInheritInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // RewardSeasonAward
	{			  // LastSeasonAwardInfo
		BYTE *pLastSeasonAwardInfo = p;
		Write16(pLastSeasonAwardInfo, 0); // len

		Write32(pLastSeasonAwardInfo, 0); // Exp
		Write32(pLastSeasonAwardInfo, 0); // Money
		Write32(pLastSeasonAwardInfo, 0); // SuperMoney
		Write16(pLastSeasonAwardInfo, 0); // ItemNum
		for (size_t i = 0; i < n; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pLastSeasonAwardInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 0); // ItemID
			Write32(pItemInfo, 0); // ItemNum
			Write32(pItemInfo, 0); // AvailPeriod
			Write8(pItemInfo, 0);  // Status
			Write32(pItemInfo, 0); // ObtainTime
			Write32(pItemInfo, 0); // OtherAttribute
			Write16(pItemInfo, 0); // ItemType

			len = pItemInfo - pLastSeasonAwardInfo;
			Set16(pLastSeasonAwardInfo, (WORD)len);
			pLastSeasonAwardInfo += len;
		}
		Write32(pLastSeasonAwardInfo, 0); // Coupons
		Write32(pLastSeasonAwardInfo, 0); // GuildPoint
		Write32(pLastSeasonAwardInfo, 0); // LuckMoney
		Write8(pLastSeasonAwardInfo, 0);  // ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pLastSeasonAwardInfo;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pLastSeasonAwardInfo;
			Set16(pLastSeasonAwardInfo, (WORD)len);
			pLastSeasonAwardInfo += len;
		}
		Write32(pLastSeasonAwardInfo, 0); // SpeedCoin

		len = pLastSeasonAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // AnotherLastSeasonAwardInfo
		BYTE *pAnotherLastSeasonAwardInfo = p;
		Write16(pAnotherLastSeasonAwardInfo, 0); // len

		Write32(pAnotherLastSeasonAwardInfo, 0); // Exp
		Write32(pAnotherLastSeasonAwardInfo, 0); // Money
		Write32(pAnotherLastSeasonAwardInfo, 0); // SuperMoney
		Write16(pAnotherLastSeasonAwardInfo, 0); // ItemNum
		for (size_t i = 0; i < n; i++)
		{ // ItemInfo
			BYTE *pItemInfo = pAnotherLastSeasonAwardInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 0); // ItemID
			Write32(pItemInfo, 0); // ItemNum
			Write32(pItemInfo, 0); // AvailPeriod
			Write8(pItemInfo, 0);  // Status
			Write32(pItemInfo, 0); // ObtainTime
			Write32(pItemInfo, 0); // OtherAttribute
			Write16(pItemInfo, 0); // ItemType

			len = pItemInfo - pAnotherLastSeasonAwardInfo;
			Set16(pAnotherLastSeasonAwardInfo, (WORD)len);
			pAnotherLastSeasonAwardInfo += len;
		}
		Write32(pAnotherLastSeasonAwardInfo, 0); // Coupons
		Write32(pAnotherLastSeasonAwardInfo, 0); // GuildPoint
		Write32(pAnotherLastSeasonAwardInfo, 0); // LuckMoney
		Write8(pAnotherLastSeasonAwardInfo, 0);	 // ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pAnotherLastSeasonAwardInfo;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pAnotherLastSeasonAwardInfo;
			Set16(pAnotherLastSeasonAwardInfo, (WORD)len);
			pAnotherLastSeasonAwardInfo += len;
		}
		Write32(pAnotherLastSeasonAwardInfo, 0); // SpeedCoin

		len = pAnotherLastSeasonAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // DailyAwardInfo
		BYTE *pDailyAwardInfo = p;
		Write16(pDailyAwardInfo, 0); // len

		Write16(pDailyAwardInfo, 0); // ScoreInc
		Write16(pDailyAwardInfo, 0); // DailyScoreInc
		Write8(pDailyAwardInfo, 0);	 // DailyAwardGot
		Write32(pDailyAwardInfo, 0); // SuperMoney
		Write32(pDailyAwardInfo, 0); // CouponMoney

		len = pDailyAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // CreateRoom
	Write8(p, 0);  // RemindLengnedSpeedGodTip
	Write8(p, 0);  // DisableSwitchGameSvr
	Write8(p, 0);  // InheritFromOldSysTag
	Write32(p, 0); // OldSysGrade
	Write32(p, 0); // WeeklyAwardRankCoinNums
	Write32(p, 0); // WeeklyAwardRankCoinLimit
	Write32(p, 0); // CurrentRankCoinNums
	Write32(p, 0); // RankTaskNum
	for (size_t i = 0; i < n; i++)
	{ // RankTaskCfgWithCurrentStatus
		BYTE *pRankTaskCfgWithCurrentStatus = p;
		Write16(pRankTaskCfgWithCurrentStatus, 0); // len

		{ // Cfg
			BYTE *pCfg = pRankTaskCfgWithCurrentStatus;
			Write16(pCfg, 0); // len

			Write32(pCfg, 0); // DescLen
			Write32(pCfg, 0); // DailyNum
			Write32(pCfg, 0); // Score
			Write32(pCfg, 0); // TaskInfoNum
			for (size_t i = 0; i < n; i++)
			{ // TaskInfo
				BYTE *pTaskInfo = pCfg;
				Write16(pTaskInfo, 0); // len

				Write32(pTaskInfo, 0); // TaskConditionNum
				for (size_t i = 0; i < n; i++)
				{ // TaskCondition
					BYTE *pTaskCondition = pTaskInfo;
					Write16(pTaskCondition, 0); // len

					Write32(pTaskCondition, 0); // Type
					Write32(pTaskCondition, 0); // Condition

					len = pTaskCondition - pTaskInfo;
					Set16(pTaskInfo, (WORD)len);
					pTaskInfo += len;
				}

				len = pTaskInfo - pCfg;
				Set16(pCfg, (WORD)len);
				pCfg += len;
			}
			Write32(pCfg, 0); // RankedCoinNums
			Write32(pCfg, 0); // DailyExtraTaskID

			len = pCfg - pRankTaskCfgWithCurrentStatus;
			Set16(pRankTaskCfgWithCurrentStatus, (WORD)len);
			pRankTaskCfgWithCurrentStatus += len;
		}
		Write32(pRankTaskCfgWithCurrentStatus, 0); // CurrentFinishNum

		len = pRankTaskCfgWithCurrentStatus - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // FeiShiDescLen
	Write32(p, 0); // FeiShiID
	Write32(p, 0); // ShopItemNums
	for (size_t i = 0; i < n; i++)
	{ // ShopItemInfos
		BYTE *pShopItemInfos = p;
		Write16(pShopItemInfos, 0); // len

		Write8(pShopItemInfos, 0);	// ItemLabelType
		Write32(pShopItemInfos, 0); // ItemID

		len = pShopItemInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	// WriteString(p, 0); //SeasonName[]
	Write32(p, 0); // BackgroundSceneId
	Write32(p, 0); // BackgroundSceneId
	{			   // RankedMatchSeasonChallengeInfo
		BYTE *pRankedMatchSeasonChallengeInfo = p;
		Write16(pRankedMatchSeasonChallengeInfo, 0); // len

		Write32(pRankedMatchSeasonChallengeInfo, 0); // SeasonStarNums
		Write32(pRankedMatchSeasonChallengeInfo, 0); // SeasonTaskNums
		for (size_t i = 0; i < n; i++)
		{ // RankedMatchSeasonTaskInfos
			BYTE *pRankedMatchSeasonTaskInfos = pRankedMatchSeasonChallengeInfo;
			Write16(pRankedMatchSeasonTaskInfos, 0); // len

			Write32(pRankedMatchSeasonTaskInfos, 0); // TaskIndex
			Write32(pRankedMatchSeasonTaskInfos, 0); // TaskType
			Write32(pRankedMatchSeasonTaskInfos, 0); // FinshiNeedValue
			Write32(pRankedMatchSeasonTaskInfos, 0); // CurrentProcessValue
			Write8(pRankedMatchSeasonTaskInfos, 0);	 // FinishOrNot

			len = pRankedMatchSeasonTaskInfos - pRankedMatchSeasonChallengeInfo;
			Set16(pRankedMatchSeasonChallengeInfo, (WORD)len);
			pRankedMatchSeasonChallengeInfo += len;
		}
		{ // RankedMatchSeasonChallengeHonorDefendInfo
			BYTE *pRankedMatchSeasonChallengeHonorDefendInfo = pRankedMatchSeasonChallengeInfo;
			Write16(pRankedMatchSeasonChallengeHonorDefendInfo, 0); // len

			Write32(pRankedMatchSeasonChallengeHonorDefendInfo, 0); // NeedStarNums
			Write32(pRankedMatchSeasonChallengeHonorDefendInfo, 0); // AwardHonorDefendNums
			Write32(pRankedMatchSeasonChallengeHonorDefendInfo, 0); // DefendAddSocre

			len = pRankedMatchSeasonChallengeHonorDefendInfo - pRankedMatchSeasonChallengeInfo;
			Set16(pRankedMatchSeasonChallengeInfo, (WORD)len);
			pRankedMatchSeasonChallengeInfo += len;
		}
		Write8(pRankedMatchSeasonChallengeInfo, 0);	 // FinishNewTask
		Write32(pRankedMatchSeasonChallengeInfo, 0); // CurrentDefendHonorNums
		Write32(pRankedMatchSeasonChallengeInfo, 0); // UiDefendHonorLimitNums

		len = pRankedMatchSeasonChallengeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // RaceTipType
	Write8(p, 0); // NewSeasonBegain
	Write8(p, 0); // HaveSeasonMapChallengeInfo
	for (size_t i = 0; i < n; i++)
	{ // RankedMatchSeasonNewMapInfos
		BYTE *pRankedMatchSeasonNewMapInfos = p;
		Write16(pRankedMatchSeasonNewMapInfos, 0); // len

		Write32(pRankedMatchSeasonNewMapInfos, 0); // SeasonID
		Write32(pRankedMatchSeasonNewMapInfos, 0); // OpenDate
		Write32(pRankedMatchSeasonNewMapInfos, 0); // CloseDate
		Write32(pRankedMatchSeasonNewMapInfos, 0); // OpenTime
		Write32(pRankedMatchSeasonNewMapInfos, 0); // CloseTime
		Write16(pRankedMatchSeasonNewMapInfos, 0); // NewMapNums
		for (size_t i = 0; i < n; i++)
		{ // SeasonNewMapInfos
			BYTE *pSeasonNewMapInfos = pRankedMatchSeasonNewMapInfos;
			Write16(pSeasonNewMapInfos, 0); // len

			Write32(pSeasonNewMapInfos, 0); // MapID
			Write32(pSeasonNewMapInfos, 0); // MapIndex
			Write32(pSeasonNewMapInfos, 0); // MaxFinishiTime
			Write16(pSeasonNewMapInfos, 0); // ChallengeLevelNums
			for (size_t i = 0; i < n; i++)
			{ // ChallengeInfos
				BYTE *pChallengeInfos = pSeasonNewMapInfos;
				Write16(pChallengeInfos, 0); // len

				Write16(pChallengeInfos, 0); // ChallengeLevel
				Write32(pChallengeInfos, 0); // FinishTime
				Write8(pChallengeInfos, 0);	 // FinishOrNot
				Write32(pChallengeInfos, 0); // AddRankedScore
				Write32(pChallengeInfos, 0); // AddRankedCoinNums

				len = pChallengeInfos - pSeasonNewMapInfos;
				Set16(pSeasonNewMapInfos, (WORD)len);
				pSeasonNewMapInfos += len;
			}
			Write8(pSeasonNewMapInfos, 0); // PromotionExtraScore

			len = pSeasonNewMapInfos - pRankedMatchSeasonNewMapInfos;
			Set16(pRankedMatchSeasonNewMapInfos, (WORD)len);
			pRankedMatchSeasonNewMapInfos += len;
		}

		len = pRankedMatchSeasonNewMapInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // NewSeasonMapAddDate
	Write8(p, 0);  // HaveTippedSuperRaceGuard
	Write16(p, 0); // PromoAwardNums
	for (size_t i = 0; i < n; i++)
	{ // RankedMatchPromAwardInfos
		BYTE *pRankedMatchPromAwardInfos = p;
		Write16(pRankedMatchPromAwardInfos, 0); // len

		Write32(pRankedMatchPromAwardInfos, 0); // GradeLevel
		Write32(pRankedMatchPromAwardInfos, 0); // ChildLevel
		{										// AwardInfo
			BYTE *pAwardInfo = pRankedMatchPromAwardInfos;
			Write16(pAwardInfo, 0); // len

			Write32(pAwardInfo, 0); // Exp
			Write32(pAwardInfo, 0); // Money
			Write32(pAwardInfo, 0); // SuperMoney
			Write16(pAwardInfo, 0); // ItemNum
			for (size_t i = 0; i < n; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 0); // ItemID
				Write32(pItemInfo, 0); // ItemNum
				Write32(pItemInfo, 0); // AvailPeriod
				Write8(pItemInfo, 0);  // Status
				Write32(pItemInfo, 0); // ObtainTime
				Write32(pItemInfo, 0); // OtherAttribute
				Write16(pItemInfo, 0); // ItemType

				len = pItemInfo - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // Coupons
			Write32(pAwardInfo, 0); // GuildPoint
			Write32(pAwardInfo, 0); // LuckMoney
			Write8(pAwardInfo, 0);	// ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardInfo;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // SpeedCoin

			len = pAwardInfo - pRankedMatchPromAwardInfos;
			Set16(pRankedMatchPromAwardInfos, (WORD)len);
			pRankedMatchPromAwardInfos += len;
		}
		Write8(pRankedMatchPromAwardInfos, 0);	// HaveAwarded
		Write32(pRankedMatchPromAwardInfos, 0); // AwardNeedFinishNums

		len = pRankedMatchPromAwardInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // SeasonAwardNums
	for (size_t i = 0; i < n; i++)
	{ // RankedMatchSeasonAwardInfos
		BYTE *pRankedMatchSeasonAwardInfos = p;
		Write16(pRankedMatchSeasonAwardInfos, 0); // len

		Write32(pRankedMatchSeasonAwardInfos, 0); // GradeLevel
		Write32(pRankedMatchSeasonAwardInfos, 0); // ChildLevel
		{										  // AwardInfo
			BYTE *pAwardInfo = pRankedMatchSeasonAwardInfos;
			Write16(pAwardInfo, 0); // len

			Write32(pAwardInfo, 0); // Exp
			Write32(pAwardInfo, 0); // Money
			Write32(pAwardInfo, 0); // SuperMoney
			Write16(pAwardInfo, 0); // ItemNum
			for (size_t i = 0; i < n; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 0); // ItemID
				Write32(pItemInfo, 0); // ItemNum
				Write32(pItemInfo, 0); // AvailPeriod
				Write8(pItemInfo, 0);  // Status
				Write32(pItemInfo, 0); // ObtainTime
				Write32(pItemInfo, 0); // OtherAttribute
				Write16(pItemInfo, 0); // ItemType

				len = pItemInfo - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // Coupons
			Write32(pAwardInfo, 0); // GuildPoint
			Write32(pAwardInfo, 0); // LuckMoney
			Write8(pAwardInfo, 0);	// ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardInfo;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // SpeedCoin

			len = pAwardInfo - pRankedMatchSeasonAwardInfos;
			Set16(pRankedMatchSeasonAwardInfos, (WORD)len);
			pRankedMatchSeasonAwardInfos += len;
		}

		len = pRankedMatchSeasonAwardInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // CurrentHighGradeLevel
	Write32(p, 0); // CurrentHighChildLevel
	Write16(p, 0); // SpecialTipInfoLen
	Write16(p, 0); // SeasonNewMapNums
	Write32(p, 0); // SeasonNewMapID[]
	Write8(p, 0);  // IsTriggerInheritedLowLimit
	Write8(p, 0);  // InheritedLowLimitContentLens
	Write8(p, 0);  // IsInheritedLowLimitUpdate
	Write8(p, 0);  // InheritedLowLimitUpdateContentLens
	Write8(p, 0);  // RankedMatchMissionTaskNums
	for (size_t i = 0; i < n; i++)
	{ // RankedMatchMissionTask
		BYTE *pRankedMatchMissionTask = p;
		Write16(pRankedMatchMissionTask, 0); // len

		Write32(pRankedMatchMissionTask, 0); // SeasonID
		Write8(pRankedMatchMissionTask, 0);	 // SeasonTaskNums
		Write32(pRankedMatchMissionTask, 0); // SeasonTask[]
		Write8(pRankedMatchMissionTask, 0);	 // RankTaskNums
		Write32(pRankedMatchMissionTask, 0); // RankTask[]

		len = pRankedMatchMissionTask - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // RankedMatchSuperRacePartLevelLimit
	Write16(p, 0); // NextGradeAndChildLevelSeasonTask
	Write16(p, 0); // CurrentSeasonTotalPartiNum
	Write8(p, 0);  // SeasonMileStoneCfgNum
	for (size_t i = 0; i < n; i++)
	{ // SeasonMileStoneCfg
		BYTE *pSeasonMileStoneCfg = p;
		Write16(pSeasonMileStoneCfg, 0); // len

		Write16(pSeasonMileStoneCfg, 0); // SeasonID
		Write16(pSeasonMileStoneCfg, 0); // TotalPartiNums
		Write16(pSeasonMileStoneCfg, 0); // IntervalNum
		Write8(pSeasonMileStoneCfg, 0);	 // AwardNumCfgNums
		Write16(pSeasonMileStoneCfg, 0); // AwardNumCfg[]

		len = pSeasonMileStoneCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // SeasonMileStoneAwardNum
	Write8(p, 0);  // TestDriveKartCfgNum
	for (size_t i = 0; i < n; i++)
	{ // KartfactoryTestDriveCfg
		BYTE *pKartfactoryTestDriveCfg = p;
		Write16(pKartfactoryTestDriveCfg, 0); // len

		Write16(pKartfactoryTestDriveCfg, 0); // SeasonID
		Write32(pKartfactoryTestDriveCfg, 0); // TestDriveTaskID
		Write8(pKartfactoryTestDriveCfg, 0);  // TestDriveKartNum
		Write32(pKartfactoryTestDriveCfg, 0); // TestDirivekartID[]
		Write8(pKartfactoryTestDriveCfg, 0);  // KartShowIntervalTime
		Write32(pKartfactoryTestDriveCfg, 0); // PlayerTestKartItemID[]
		Write8(pKartfactoryTestDriveCfg, 0);  // TestKartRefitLevel
		Write16(pKartfactoryTestDriveCfg, 0); // UnLockSuitNum[]

		len = pKartfactoryTestDriveCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); // TestDriveKartIdx
	Write32(p, 0); // TipContiUseTestKart
	Write8(p, 0);  // MileStoneAddTip
	Write8(p, 0);  // Type
	Write8(p, 0);  // HasRankedMemoryData
	for (size_t i = 0; i < n; i++)
	{ // RankedMatchMemoryData
		BYTE *pRankedMatchMemoryData = p;
		Write16(pRankedMatchMemoryData, 0); // len

		Write32(pRankedMatchMemoryData, 0); // RegisterDiffDayNum
		Write32(pRankedMatchMemoryData, 0); // FirstParticipateSeasonID
		Write32(pRankedMatchMemoryData, 0); // FirstTopGradeSeasonID
		Write32(pRankedMatchMemoryData, 0); // TopGrade
		Write32(pRankedMatchMemoryData, 0); // TopChild
		Write8(pRankedMatchMemoryData, 0);	// TopNum

		len = pRankedMatchMemoryData - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HasRankedSpeedFlashData
	for (size_t i = 0; i < n; i++)
	{ // RankedSpeedFlash
		BYTE *pRankedSpeedFlash = p;
		Write16(pRankedSpeedFlash, 0); // len

		Write16(pRankedSpeedFlash, 0); // SeasonID
		Write32(pRankedSpeedFlash, 0); // AwardItemID
		Write8(pRankedSpeedFlash, 0);  // PerAwardNum
		Write8(pRankedSpeedFlash, 0);  // PerRoundAwardNumMax
		Write8(pRankedSpeedFlash, 0);  // WeeklyAwardNumMax
		Write16(pRankedSpeedFlash, 0); // SeasonAwardNumMax
		Write8(pRankedSpeedFlash, 0);  // N2OAwardProb
		Write8(pRankedSpeedFlash, 0);  // JetAwardProb
		Write8(pRankedSpeedFlash, 0);  // CDTime
		Write8(pRankedSpeedFlash, 0);  // WeeklyHasAwardNum
		Write16(pRankedSpeedFlash, 0); // SeasonHasAwardNum

		len = pRankedSpeedFlash - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0);  // CanPlayPinnacleBattle
	Write32(p, 0); // SavedExtraScore
	Write32(p, 0); // AnonymousModeStartScore
	Write16(p, 0); // SeasonHonerAwardNums
	for (size_t i = 0; i < n; i++)
	{ // SeasonHonerAwardInfo
		BYTE *pSeasonHonerAwardInfo = p;
		Write16(pSeasonHonerAwardInfo, 0); // len

		Write16(pSeasonHonerAwardInfo, 0); // GradeLevel
		Write8(pSeasonHonerAwardInfo, 0);  // Level
		Write16(pSeasonHonerAwardInfo, 0); // RankRange[]
		Write16(pSeasonHonerAwardInfo, 0); // SeasonAppelationType
		{								   // AwardInfo
			BYTE *pAwardInfo = pSeasonHonerAwardInfo;
			Write16(pAwardInfo, 0); // len

			Write32(pAwardInfo, 0); // Exp
			Write32(pAwardInfo, 0); // Money
			Write32(pAwardInfo, 0); // SuperMoney
			Write16(pAwardInfo, 0); // ItemNum
			for (size_t i = 0; i < n; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAwardInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 0); // ItemID
				Write32(pItemInfo, 0); // ItemNum
				Write32(pItemInfo, 0); // AvailPeriod
				Write8(pItemInfo, 0);  // Status
				Write32(pItemInfo, 0); // ObtainTime
				Write32(pItemInfo, 0); // OtherAttribute
				Write16(pItemInfo, 0); // ItemType

				len = pItemInfo - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // Coupons
			Write32(pAwardInfo, 0); // GuildPoint
			Write32(pAwardInfo, 0); // LuckMoney
			Write8(pAwardInfo, 0);	// ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ // ExtendInfoAward
				BYTE *pExtendInfoAward = pAwardInfo;
				Write16(pExtendInfoAward, 0); // len

				Write16(pExtendInfoAward, 0); // Key
				Write32(pExtendInfoAward, 0); // AddValue
				Write32(pExtendInfoAward, 0); // TotalValue
				Write8(pExtendInfoAward, 0);  // ShowPriority
				Write16(pExtendInfoAward, 0); // BuyNeedScore
				Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

				len = pExtendInfoAward - pAwardInfo;
				Set16(pAwardInfo, (WORD)len);
				pAwardInfo += len;
			}
			Write32(pAwardInfo, 0); // SpeedCoin

			len = pAwardInfo - pSeasonHonerAwardInfo;
			Set16(pSeasonHonerAwardInfo, (WORD)len);
			pSeasonHonerAwardInfo += len;
		}
		Write8(pSeasonHonerAwardInfo, 0); // Difficulty

		len = pSeasonHonerAwardInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // LastSeasonTopHonerInfoIdx
	Write8(p, 0);  // HasFengJianSunRenAwardData
	for (size_t i = 0; i < n; i++)
	{ // RankedMatchFengJianSunRenAwardData
		BYTE *pRankedMatchFengJianSunRenAwardData = p;
		Write16(pRankedMatchFengJianSunRenAwardData, 0); // len

		Write32(pRankedMatchFengJianSunRenAwardData, 0); // FragmentItemID
		Write32(pRankedMatchFengJianSunRenAwardData, 0); // FragmentItemNum
		Write32(pRankedMatchFengJianSunRenAwardData, 0); // RankedScoreNum
		Write32(pRankedMatchFengJianSunRenAwardData, 0); // RankedCoinItemID
		Write32(pRankedMatchFengJianSunRenAwardData, 0); // RankedCoinNum
		Write32(pRankedMatchFengJianSunRenAwardData, 0); // RankedFactoryItemID
		Write32(pRankedMatchFengJianSunRenAwardData, 0); // RankedFactoryNum
		Write32(pRankedMatchFengJianSunRenAwardData, 0); // SuperMoneyNum

		len = pRankedMatchFengJianSunRenAwardData - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // RankedCoinGradeCfgNum
	for (size_t i = 0; i < n; i++)
	{ // RankedCoinGradeCfg
		BYTE *pRankedCoinGradeCfg = p;
		Write16(pRankedCoinGradeCfg, 0); // len

		Write16(pRankedCoinGradeCfg, 0); // GradeLevel
		Write32(pRankedCoinGradeCfg, 0); // RankCoinWeeklyLimit

		len = pRankedCoinGradeCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // WeeklyAwardFactoryCoinLimit

	len = p - buf;
	SendToClient(Client, 25000, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyReplaceProp(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 1); //PlayerNum
	for (size_t i = 0; i < 1; i++)
	{ //RacerReplacePropInfo
		BYTE* pRacerReplacePropInfo = p;
		Write16(pRacerReplacePropInfo, 0); //len

		WriteUn(pRacerReplacePropInfo, Client->Uin); //Uin
		for (size_t i = 0; i < 1; i++)
		{ //ReplaceProp
			BYTE* pReplaceProp = pRacerReplacePropInfo;
			Write16(pReplaceProp, 0); //len

			Write32(pReplaceProp, 0); //ReplaceFlag
			Write32(pReplaceProp, 1); //PropIndex
			Write16(pReplaceProp, 34); //PropID

			len = pReplaceProp - pRacerReplacePropInfo;
			Set16(pRacerReplacePropInfo, (WORD)len);
			pRacerReplacePropInfo += len;
		}

		len = pRacerReplacePropInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //ItemID

	len = p - buf;
	SendToClient(Client, 525, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestReportDrift(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;
	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int DriftFlag = Read32(p);
	int AccelerateFuelInc = Read32(p);
	bool OnlyAccelSelfFue = Read8(p);
	UINT StartLapTime = Read32(p);
	UINT CurLapTime = Read32(p);
	bool maxFuel = false;
	Client->DriftCount++;
	Client->Fuel += AccelerateFuelInc;
	


	if (Client->Fuel >= 1000) {//判断是否挤满
		maxFuel = true;
		Client->Fuel = 0;
	}

	if (Client->GetSkillID(4) || (Client->KartID == 19618)) {//雷蛇特性
	//if (Client->GetSkillID(4)) {//雷蛇特性
		if (maxFuel)
		{
			if (Client->getPropNum >= 2) {
				//Client->PropList[Client->getPropNum - 1] = 34;
				ResponseReportDrift(Client, 777, 1000);
#ifdef DEBUG
				Z_INFO("当前道具栏数量%d", Client->getPropNum);
#endif
				return;
			}
		}
	}
	
	if (maxFuel) {
		if (Client->getPropNum >= 2) {
			Client->getPropNum = 2;
		}
		else {
			Client->getPropNum++;
		}
		//Client->PropList[Client->getPropNum - 1] = 1;
	}
	
	
#ifdef DEBUG
	Z_INFO("当前道具栏数量%d", Client->getPropNum);
#endif
	
	if (Client->SubGameMode == 2 && Client->BaseGameMode == 1)
	{ // 组队模式
		Client->TeamFuel += (AccelerateFuelInc / 5) + 1;
		if (Client->TeamFuel >= 1000)
		{
			int JetID = 2002; // 默认普通氮气
			ResponseGetPropS(Client, JetID);
			NotifyReportDrift(Client, JetID, Client->Uin, Client->TeamFuel);
			Client->TeamFuel = 0;
			RoomNode *Room = GetRoom(Client->RoomID);
			if (Room)
			{
				for (int i = 0; i < 6; i++)
				{
					ClientNode *RoomClient = Room->Player[i];
					if (RoomClient && RoomClient != Client)
					{
						ResponseGetPropS(RoomClient, JetID);
						NotifyReportDrift(RoomClient, JetID, RoomClient->Uin, RoomClient->TeamFuel);
						RoomClient->TeamFuel = 0;
					}
				}
			}
			/*if (Client->CollectedN2O % 7 == 0) {
				JetID = 777;
			}*/
			// NotifyReportDrift(Client, JetID, Client->Uin, Client->TeamFuel);
		}
		else
		{
			NotifyReportDrift(Client, 1, Client->Uin, Client->TeamFuel);
			RoomNode *Room = GetRoom(Client->RoomID);
			if (Room)
			{
				for (int i = 0; i < 6; i++)
				{
					ClientNode *RoomClient = Room->Player[i];
					if (RoomClient && RoomClient != Client)
					{
						RoomClient->TeamFuel = Client->TeamFuel;
						NotifyReportDrift(RoomClient, 1, RoomClient->Uin, RoomClient->TeamFuel);
					}
				}
			}
		}
	}
	if (Client->GetSkillID(3)||(Client->KartID == 129099&& Client->SkinID == 127616))
	//if (Client->GetSkillID(3))
	{ // 国服玉麒麟

		if (maxFuel)
		{
			// 挤满一个氮气
			Client->CollectedN2O++;
			if (Client->KartSkillID==Client->PetSkillID&& Client->KartSkillID>0) {
				int JetID = 1; // 默认普通氮气
				if (Client->CollectedN2O % 4 == 0)
				{
					//Client->PropList[0] = 777;
					JetID = 777;
				}
				ResponseReportDrift(Client, JetID, 250);
			}
			else {
				int JetID = 1; // 默认普通氮气
				if (Client->CollectedN2O % 6 == 0)
				{
					//Client->PropList[0] = 777;
					JetID = 777;
				}
				if (Client->CollectedN2O % 6 == 0) {
					ResponseReportDrift(Client, JetID, 100);
				}
				else {
					ResponseReportDrift(Client, JetID, 180);
				}
			}
			
		}
	}
	if (Client->GetSkillID(6))
	{ // 手游星煌
		if (maxFuel &&Client->useDQ > 6) {//第一次进入星蝶形态氮气升级
			ResponseReportDrift(Client, 777, 0);
		}
	}
	
}
void ResponseGetSuperN2O2(ClientNode* Client)
{
	if (Client->getPropNum >= 2) {
		return;
	}
	else {
		Client->getPropNum++;
		//Client->PropList[Client->getPropNum - 1] = 34;
	}
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	Write16(p, 0); //ResultID
	Write32(p, Client->Uin); //Uin
	Write16(p, 34); //PropID
	//34 433紫色氮气 2002蓝色氮气 71火球 176 190其他氮气 555火焰氮气 
	//777玉麒麟专属超级氮气
	Write32(p, 1); //PropIndex
	Write8(p, 0); //GetPropType
	Write32(p, 0); //AwardItemID
	Write8(p, 0); //ReasonLen
	Write16(p, 0); //PropPositionNO
	Write32(p, 0); //ActIDForClient
#ifndef ZingSpeed
	Write8(p, 1); //GetPropSecType
	Write8(p, 0); //PropPosIdx
	Write32(p, 0); //NPCUin
	Write8(p, 0); //ParaNum
	//Write32(p, 0); //ParaList[]
#endif
	len = p - buf;
	SendToClient(Client, 124, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyReportDrift(ClientNode *Client, UINT PropID, UINT DriftPlayerUin, UINT TeamAccelerateFule)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, TeamAccelerateFule); // TeamAccelerateFule
	Write32(p, DriftPlayerUin);		// DriftPlayerUin
	Write16(p, PropID);				// PropID
	Write32(p, 0);					// PropIndex

	len = p - buf;
	SendToClient(Client, 521, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyTeamN2OProgressInc(ClientNode *Client, int PropID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 100);		 // TeamN2OProgressInc
	Write32(p, 100);		 // TeamN2OProgress
	Write32(p, Client->Uin); // Uin
	Write32(p, 0);			 // PropIndex
	Write16(p, PropID);		 // PropID
	Write32(p, 1000);		 // TeamN2OMaxProgress

	len = p - buf;
	SendToClient(Client, 1207, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestUseProp(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short PropID = Read16(p);
	UINT DstUin = Read32(p);
	int CurrentPosition = Read32(p);
	 CurrentPosition = Read32(p);
	 CurrentPosition = Read32(p);
	int PropIndex = Read32(p);
	short NewID = Read16(p);
	char IsClearProp = Read8(p);
	bool DstType = Read8(p);
	UCHAR CurrentSelfRank = Read8(p);
	UCHAR CurrentTeammateRank = Read8(p);
	UCHAR Position = Read8(p);
	ResponseUseProp(Client);
}

void ResponseUseProp(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	WriteUn(p, Client->Uin); //Uin
	Write16(p, 34); //PropID
	WriteUn(p, Client->Uin); //DstUin
	Write32(p, 0); //PropPosition[]
	Write32(p, 0); //PropPosition[]
	Write32(p, 0); //PropPosition[]
	Write32(p, 0); //PropIndex
	Write8(p, 0); //DstType
	Write8(p, 0); //Position
	Write8(p, 0); //ParaNum
	Write32(p, 0); //ParaList[]
	Write8(p, 0); //PropSecType
	Write8(p, 0); //PropPosIdx
	Write8(p, 0); //Status
	Write32(p, 0); //NPCUin
	Write8(p, 0); //UsePropProtoMode
	Write8(p, 0); //SubPropNum
	for (size_t i = 0; i < 0; i++)
	{ //SubProp
		BYTE* pSubProp = p;
		Write16(pSubProp, 0); //len

		Write16(pSubProp, 0); //SubPropID
		Write32(pSubProp, 0); //Duration
		Write32(pSubProp, 0); //UseInterval

		len = pSubProp - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); //SubPropID
	Write8(p, 0); //OtherPropPosNum
	for (size_t i = 0; i < 0; i++)
	{ //OtherPropPos
		BYTE* pOtherPropPos = p;
		Write16(pOtherPropPos, 0); //len

		Write32(pOtherPropPos, 0); //Idx
		Write32(pOtherPropPos, 0); //Postion[]

		len = pOtherPropPos - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 125, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseReportDrift(ClientNode *Client, int PropID, int SuperJetKartFuelInc)
{

	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);					 // ResultID
	Write16(p, PropID);				 // PropID
	Write32(p, Client->DriftCount);					 // PropIndex
	Write32(p, Client->Fuel);		 // CurrentAccelerateFuel
	Write8(p, 0);					 // ReasonLen
	Write32(p, SuperJetKartFuelInc); // SuperJetKartFuelInc

	len = p - buf;
	SendToClient(Client, 119, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestReportSkillStoneTakeEffect(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int StoneID = Read32(p);
	NotifyOtherSkillStoneTakeEffect(Client, StoneID);
}

void NotifyOtherSkillStoneTakeEffect(ClientNode *Client, int StoneID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, StoneID);	 // StoneID

	len = p - buf;
	SendToClient(Client, 910, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyPlayerLimitedWakeKartInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, 1);			 // uilimitInfoNums
	for (size_t i = 0; i < 1; i++)
	{ // wakeKartLimitedItemToKartInfos
		BYTE *pwakeKartLimitedItemToKartInfos = p;
		Write16(pwakeKartLimitedItemToKartInfos, 0); // len

		Write32(pwakeKartLimitedItemToKartInfos, 10020); // LimitedItemID
		Write32(pwakeKartLimitedItemToKartInfos, 12720); // KartID

		len = pwakeKartLimitedItemToKartInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 16072, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetGameEndBoxAward(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT Value = Read32(p);
	ItemInfo Items[1];
	ItemInfo ItemIn = EndGame[Client->Uin];
	ItemInfo Item = PlayerDB_AddItem(Client->Uin, ItemIn.ItemID, ItemIn.ItemNum, ItemIn.AvailPeriod, false);
	Items[0].ItemID = ItemIn.ItemID;
	Items[0].ItemNum = Item.ItemNum;
	Items[0].AvailPeriod = Item.AvailPeriod;
	Items[0].Status = Item.Status;
	Items[0].ObtainTime = Item.ObtainTime;
	Items[0].OtherAttribute = Item.OtherAttribute;
	Items[0].ItemType = Item.ItemType;
	NotifyClientAddItem(Client, 1, Items);
}
void ResponseGetGameEndBoxAward(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;
	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write32(p, 2);
	len = p - buf;

	SendToClient(Client, 24756, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyGenGameEndBoxAward(ClientNode *Client)
{
	YAML::Node OverItem = YAML::LoadFile(".\\Gift\\overItem.yml");
	ItemInfo EndItems[3];
	if (OverItem["ON"].as<UINT>() == 1)
	{
		UINT temp = 0;
		YAML::Node ItemList = OverItem["Items"];
		for (size_t i = 0; i < 3; i++)
		{
			int randomID;
			std::random_device rd;
			std::mt19937 eng(rd());
			std::uniform_int_distribution<int> dis(1, ItemList.size());
			randomID = dis(eng);
			while (randomID == temp && temp > 0)
			{
				randomID = dis(eng);
			}
			EndItems[i].ItemID = ItemList[randomID - 1]["ItemId"].as<INT>();
			EndItems[i].ItemNum = ItemList[randomID - 1]["ItemNum"].as<INT>();
			EndItems[i].AvailPeriod = ItemList[randomID - 1]["AvailPeriod"].as<INT>();
			EndItems[i].ItemType = ItemList[randomID - 1]["ItemType"].as<INT>();
			temp = randomID;
		}
	}
	else
	{
		return;
	}
	EndGame[Client->Uin] = EndItems[0];
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	// typedef struct ENDBOXAWARD // 库定义数据类型结构
	//{
	//	WORD ItemID;
	//	WORD ItemNum;
	//	WORD AvailPeriod;
	//	UCHAR Status;
	//	WORD ObtainTime;
	//	WORD OtherAttribute;
	//	WORD ItemType;

	//};
	// const ENDBOXAWARD EndBoxAward[] =
	//{
	//	//ItemID
	//	//ItemNum
	//	//AvailPeriod
	//   //Status
	//	//ObtainTime
	//	//OtherAttribute
	//	//ItemType
	//	{13916,1,0,0,0,0,1},//春节金宝箱
	//	{21983,1,0,0,0,0,1},
	//	{65025,1,0,0,0,0,1}

	//};
	Write32(p, Client->Uin); // Uin
	Write16(p, 3);			 // ItemNum
	for (size_t i = 0; i < 3; i++)
	{ // ItemInfo
		BYTE *pItemInfo = p;
		Write16(pItemInfo, 0); // len

		Write32(pItemInfo, EndItems[i].ItemID);		 // ItemID
		Write32(pItemInfo, EndItems[i].ItemNum);	 // ItemNum
		Write32(pItemInfo, EndItems[i].AvailPeriod); // AvailPeriod
		Write8(pItemInfo, 0);						 // Status
		Write32(pItemInfo, 0);						 // ObtainTime
		Write32(pItemInfo, 0);						 // OtherAttribute
		Write16(pItemInfo, EndItems[i].ItemType);	 // ItemType

		len = pItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 98); // DescLen
	memcpy(p, "恭喜你获得幸运卡牌/C033选1/C00的机会，点击卡牌即可领奖!", 98);
	p += 98;
	len = p - buf;
	SendToClient(Client, 24755, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestGetPlayerNonSaleItemInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetPlayerNonSaleItemInfo(Client);
}

void ResponseGetPlayerNonSaleItemInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultId
	Write32(p, Client->Uin); // Uin
	{						 // ClientPlayerNonSaleItemInfoList
		BYTE *pClientPlayerNonSaleItemInfoList = p;
		Write16(pClientPlayerNonSaleItemInfoList, 0); // len

		Write32(pClientPlayerNonSaleItemInfoList, 1); // ItemNum
		for (size_t i = 0; i < 1; i++)
		{ // PlayerNonSaleItemInfo
			BYTE *pPlayerNonSaleItemInfo = pClientPlayerNonSaleItemInfoList;
			Write16(pPlayerNonSaleItemInfo, 0); // len

			Write32(pPlayerNonSaleItemInfo, 10020); // ItemID
			Write32(pPlayerNonSaleItemInfo, 0);		// FocusCount
			Write32(pPlayerNonSaleItemInfo, 0);		// FocusTime

			len = pPlayerNonSaleItemInfo - pClientPlayerNonSaleItemInfoList;
			Set16(pClientPlayerNonSaleItemInfoList, (WORD)len);
			pClientPlayerNonSaleItemInfoList += len;
		}

		len = pClientPlayerNonSaleItemInfoList - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 20125, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestPlayerAction(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short ActionType = Read16(p);
	int Para0 = Read32(p);
	int Para1 = Read32(p);
	int Para2 = Read32(p);
	int Para3 = Read32(p);
	int Para4 = Read32(p);
	// char* Para0 = ReadString(p);
	// char* Para1 = ReadString(p);
	ResponsePlayerAction(Client, ActionType);
}

void ResponsePlayerAction(ClientNode *Client, short ActionType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write16(p, ActionType);			 // ActionType
	Write32(p, 0);			 // Para0
	Write32(p, 0);			 // Para1
	Write32(p, 0);			 // Para2
	Write32(p, 0);			 // Para3
	Write32(p, 0);			 // Para4
	Write8(p, 0);			 // Para0[]
	Write8(p, 0);			 // Para1[]

	len = p - buf;
	SendToClient(Client, 24640, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetWareHouseStatus(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetWareHouseStatus(Client);
}

void ResponseGetWareHouseStatus(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write8(p, 1);	   // Status
	Write16(p, 99999); // MaxSize

	len = p - buf;
	SendToClient(Client, 420, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetWareHouseItemList(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetWareHouseItemList(Client);
}

void ResponseGetWareHouseItemList(ClientNode *Client)
{
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	ItemInfo Items[200];
	size_t ItemNum = 0;
	bool HasMoreInfo = false;

	sql = "SELECT ItemID,ItemNum,AvailPeriod,Status,ObtainTime,OtherAttribute  FROM Item WHERE Uin = ? and WareHouseStatus = 1";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	if (stmt)
	{
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			Items[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
			Items[ItemNum].ItemNum = sqlite3_column_int(stmt, 1);
			Items[ItemNum].AvailPeriod = sqlite3_column_int(stmt, 2);
			Items[ItemNum].Status = sqlite3_column_int(stmt, 3);
			Items[ItemNum].ObtainTime = sqlite3_column_int(stmt, 4);
			Items[ItemNum].OtherAttribute = sqlite3_column_int(stmt, 5);
			Items[ItemNum].ItemType = 0;
			ItemNum++;
		}
	}
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, ItemNum); // ItemNum
	for (size_t i = 0; i < ItemNum; i++)
	{ // WareHouseItems
		BYTE *pWareHouseItems = p;
		Write16(pWareHouseItems, 0); // len

		Write32(pWareHouseItems, Items[i].ItemID);		   // ItemID
		Write32(pWareHouseItems, Items[i].ItemNum);		   // ItemNum
		Write32(pWareHouseItems, Items[i].AvailPeriod);	   // AvailPeriod
		Write8(pWareHouseItems, Items[i].Status);		   // Status
		WriteUn(pWareHouseItems, Items[i].ObtainTime);	   // ObtainTime
		Write32(pWareHouseItems, Items[i].OtherAttribute); // OtherAttribute
		Write16(pWareHouseItems, Items[i].ItemType);	   // ItemType

		len = pWareHouseItems - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HasMoreInfo

	len = p - buf;
	SendToClient(Client, 421, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestDoWareHouseOperation(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ItemID = Read32(p);
	bool OpType = Read8(p); // 1 取出 0 放入
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	sql = "UPDATE Item SET WareHouseStatus=? WHERE Uin=? AND ItemID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, OpType == 1 ? 0 : 1);
		sqlite3_bind_int(stmt, 2, Client->Uin);
		sqlite3_bind_int(stmt, 3, ItemID);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	ResponseWareHouseOperation(Client, ItemID, OpType);
}

void ResponseWareHouseOperation(ClientNode *Client, int ItemID, bool OpType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);	   // ResultID
	Write8(p, OpType); // OpType
	{				   // ItemInfo
		BYTE *pItemInfo = p;
		Write16(pItemInfo, 0); // len

		Write32(pItemInfo, ItemID); // ItemID
		Write32(pItemInfo, 1);		// ItemNum
		Write32(pItemInfo, -1);		// AvailPeriod
		Write8(pItemInfo, 0);		// Status
		WriteUn(pItemInfo, 0);		// ObtainTime
		Write32(pItemInfo, 0);		// OtherAttribute
		Write16(pItemInfo, 0);		// ItemType

		len = pItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 422, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestDoWareHouseOperationBatch(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int ItemNum = Read32(p);
	ItemStatus Items[200];
	for (int i = 0; i < ItemNum; i++)
	{
		Items[i].ItemID = Read32(p);
	}
	bool OpType = Read8(p);
	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	for (int i = 0; i < ItemNum; i++)
	{
		sql = "UPDATE Item SET WareHouseStatus=? WHERE Uin=? AND ItemID=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, OpType == 1 ? 0 : 1);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			sqlite3_bind_int(stmt, 3, Items[i].ItemID);
			result = sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		ResponseWareHouseOperation(Client, Items[i].ItemID, OpType);
	}
	// ResponseWareHouseOperationBatch(Client, ItemNum, Items, OpType);
}

void ResponseWareHouseOperationBatch(ClientNode *Client, int ItemNun, ItemStatus *Items, bool OpType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write8(p, OpType);		 // OpType
	Write32(p, ItemNun);	 // SuccessItemNum
	for (size_t i = 0; i < ItemNun; i++)
	{ // SuccessPlayerDBItemInfo
		BYTE *pSuccessPlayerDBItemInfo = p;
		Write16(pSuccessPlayerDBItemInfo, 0); // len

		Write32(pSuccessPlayerDBItemInfo, Items[i].ItemID); // ItemID
		Write32(pSuccessPlayerDBItemInfo, 1);				// ItemNum
		Write32(pSuccessPlayerDBItemInfo, -1);				// AvailPeriod
		Write8(pSuccessPlayerDBItemInfo, 0);				// Status
		WriteUn(pSuccessPlayerDBItemInfo, 0);				// ObtainTime
		Write32(pSuccessPlayerDBItemInfo, 0);				// OtherAttribute
		Write16(pSuccessPlayerDBItemInfo, 0);				// ItemType

		len = pSuccessPlayerDBItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); // FailItemNum
	for (size_t i = 0; i < 0; i++)
	{ // FailBatchOperationInfo
		BYTE *pFailBatchOperationInfo = p;
		Write16(pFailBatchOperationInfo, 0); // len

		Write16(pFailBatchOperationInfo, 0); // ResultID
		{									 // ItemInfo
			BYTE *pItemInfo = pFailBatchOperationInfo;
			Write16(pItemInfo, 0); // len

			Write32(pItemInfo, 0); // ItemID
			Write32(pItemInfo, 0); // ItemNum
			Write32(pItemInfo, 0); // AvailPeriod
			Write8(pItemInfo, 0);  // Status
			WriteUn(pItemInfo, 0); // ObtainTime
			Write32(pItemInfo, 0); // OtherAttribute
			Write16(pItemInfo, 0); // ItemType

			len = pItemInfo - pFailBatchOperationInfo;
			Set16(pFailBatchOperationInfo, (WORD)len);
			pFailBatchOperationInfo += len;
		}

		len = pFailBatchOperationInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 21036, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestKartRefitAttrTransfer(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int SrcKartID = Read32(p);
	int DstKartID = Read32(p);
	UCHAR ItemType = Read8(p); // 转移项 0 悬挂 1 引擎 2燃料 3 点火 4 进气 8 防护
	UCHAR Type = Read8(p);	   // 0 转移 1 确定转移
	ResponseRefitKartAttrTransfer(Client, SrcKartID, DstKartID, ItemType, Type);
}

void ResponseRefitKartAttrTransfer(ClientNode *Client, int SrcKartID, int DstKartID, UCHAR ItemType, UCHAR Type)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	UINT SrcBeforeValue = 0; // 原车先值
	UINT SrcAfterValue = 0;	 // 转移后变成0

	UINT DstBeforeValue = 0; // 目标车的值
	UINT DstAfterValue = 0;	 // 等于原车当前改装值

	UINT SrcBeforeRefitCount = 0; // 原车先改装数
	UINT SrcAfterRefitCount = 0;  // 转移后变成0

	UINT DstAfterRefitCount = 0;  // 目标车改装数
	UINT DstBeforeRefitCount = 0; // 等于原车当前改装值
	short SrcRefitCout = 0;
	
	const char *sql = nullptr;
	sqlite3_stmt *stmt = nullptr;
	sql = "SELECT KartID,RefitCout,MaxFlags,SJetWeight,AccuWeight,JetWeight,WWeight,SpeedWeight,AntiCollisionWeight,SJetWeightRefitCout, AccuWeightRefitCout, JetWeightRefitCout, WWeightRefitCout, SpeedWeightRefitCout, AntiCollisionWeightRefitCout FROM KartRefit WHERE Uin = ? AND KartID=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, SrcKartID);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			SrcBeforeRefitCount = sqlite3_column_int(stmt, 1);
			SrcAfterRefitCount = sqlite3_column_int(stmt, 1);
			if (ItemType == 0)
			{ // 悬挂
				SrcBeforeValue = sqlite3_column_int(stmt, 6);
				DstAfterValue = sqlite3_column_int(stmt, 6);
				SrcRefitCout = sqlite3_column_int(stmt, 13);
			}
			else if (ItemType == 1)
			{ // 引擎
				SrcBeforeValue = sqlite3_column_int(stmt, 7);
				DstAfterValue = sqlite3_column_int(stmt, 7);
				SrcRefitCout = sqlite3_column_int(stmt, 14);
			}
			else if (ItemType == 2)
			{ // 燃料
				SrcBeforeValue = sqlite3_column_int(stmt, 5);
				DstAfterValue = sqlite3_column_int(stmt, 5);
				SrcRefitCout = sqlite3_column_int(stmt, 12);
			}
			else if (ItemType == 3)
			{ // 点火
				SrcBeforeValue = sqlite3_column_int(stmt, 3);
				DstAfterValue = sqlite3_column_int(stmt, 3);
				SrcRefitCout = sqlite3_column_int(stmt, 10);
			}
			else if (ItemType == 4)
			{ // 进气
				SrcBeforeValue = sqlite3_column_int(stmt, 4);
				DstAfterValue = sqlite3_column_int(stmt, 4);
				SrcRefitCout = sqlite3_column_int(stmt, 11);
			}
			else if (ItemType == 8)
			{ // 防护
				SrcBeforeValue = sqlite3_column_int(stmt, 8);
				DstAfterValue = sqlite3_column_int(stmt, 8);
				SrcRefitCout = sqlite3_column_int(stmt, 15);
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	sql = "SELECT KartID,RefitCout,MaxFlags,SJetWeight,AccuWeight,JetWeight,WWeight,SpeedWeight,AntiCollisionWeight FROM KartRefit WHERE Uin = ? AND KartID=?;";
	if (sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, nullptr) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, DstKartID);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			DstAfterRefitCount = sqlite3_column_int(stmt, 1);
			DstBeforeRefitCount = sqlite3_column_int(stmt, 1);
			if (ItemType == 0)
			{ // 悬挂
				DstBeforeValue = sqlite3_column_int(stmt, 6);
			}
			else if (ItemType == 1)
			{ // 引擎
				DstBeforeValue = sqlite3_column_int(stmt, 7);
			}
			else if (ItemType == 2)
			{ // 燃料
				DstBeforeValue = sqlite3_column_int(stmt, 5);
			}
			else if (ItemType == 3)
			{ // 点火
				DstBeforeValue = sqlite3_column_int(stmt, 3);
			}
			else if (ItemType == 4)
			{ // 进气
				DstBeforeValue = sqlite3_column_int(stmt, 4);
			}
			else if (ItemType == 8)
			{ // 防护
				DstBeforeValue = sqlite3_column_int(stmt, 8);
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = nullptr;
	Write32(p, Client->Uin);		 // Uin
	Write16(p, 0);					 // ResultID
	Write16(p, 0);					 // ReasonLen
	Write32(p, SrcKartID);			 // SrcKartID
	Write32(p, DstKartID);			 // DstKartID
	Write8(p, ItemType);			 // ItemType //转移项 0 悬挂 1 引擎 2燃料 3 点火 4 进气 8 防护
	Write8(p, Type);				 // Type // 0 转移 1 确定转移
	Write8(p, SrcBeforeValue);		 // SrcBeforeValue
	Write8(p, SrcAfterValue);		 // SrcAfterValue
	Write32(p, SrcAfterRefitCount);	 // SrcRefitCount
	Write32(p, DstAfterRefitCount);	 // DstRefitCount
	Write16(p, 0);					 // SrcMaxFlags
	Write16(p, 0);					 // DstMaxFlags
	Write8(p, DstBeforeValue);		 // DstBeforeValue
	Write8(p, DstAfterValue);		 // DstAfterValue
	Write32(p, SrcBeforeRefitCount); // SrcBeforeRefitCount
	Write32(p, DstBeforeRefitCount); // DstBeforeRefitCount

	len = p - buf;
	SendToClient(Client, 1208, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
	int result;
	const char *sql1 = nullptr;
	const char *sql2 = nullptr;

	if (Type == 1)
	{ // 确定转移
		if (ItemType == 0)
		{ // 悬挂
			sql1 = "UPDATE KartRefit SET WWeight=0,RefitCout = RefitCout-WWeightRefitCout,WWeightRefitCout=0 WHERE Uin=? AND KartID=?;";
			sql2 = "UPDATE KartRefit SET WWeight=?,RefitCout = RefitCout+?,WWeightRefitCout=? WHERE Uin=? AND KartID=?;";
		}
		else if (ItemType == 1)
		{ // 引擎
			sql1 = "UPDATE KartRefit SET SpeedWeight=0,RefitCout = RefitCout-SpeedWeightRefitCout,SpeedWeightRefitCout=0 WHERE Uin=? AND KartID=?;";
			sql2 = "UPDATE KartRefit SET SpeedWeight=?,RefitCout = RefitCout+?,SpeedWeightRefitCout=? WHERE Uin=? AND KartID=?;";
		}
		else if (ItemType == 2)
		{ // 燃料
			sql1 = "UPDATE KartRefit SET JetWeight=0,RefitCout = RefitCout-JetWeightRefitCout,JetWeightRefitCout=0 WHERE Uin=? AND KartID=?;";
			sql2 = "UPDATE KartRefit SET JetWeight=?,RefitCout = RefitCout+?,JetWeightRefitCout=? WHERE Uin=? AND KartID=?;";
		}
		else if (ItemType == 3)
		{ // 点火
			sql1 = "UPDATE KartRefit SET SJetWeight=0,RefitCout = RefitCout-SJetWeightRefitCout,SJetWeightRefitCout=0 WHERE Uin=? AND KartID=?;";
			sql2 = "UPDATE KartRefit SET SJetWeight=?,RefitCout = RefitCout+?,SJetWeightRefitCout=? WHERE Uin=? AND KartID=?;";
		}
		else if (ItemType == 4)
		{ // 进气
			sql1 = "UPDATE KartRefit SET AccuWeight=0,RefitCout = RefitCout-AccuWeightRefitCout,AccuWeightRefitCout=0 WHERE Uin=? AND KartID=?;";
			sql2 = "UPDATE KartRefit SET AccuWeight=?,RefitCout = RefitCout+?,AccuWeightRefitCout=? WHERE Uin=? AND KartID=?;";
		}
		else if (ItemType == 8)
		{ // 防护
			sql1 = "UPDATE KartRefit SET AntiCollisionWeight=0,RefitCout = RefitCout-AntiCollisionWeightRefitCout,AntiCollisionWeightRefitCout=0 WHERE Uin=? AND KartID=?;";
			sql2 = "UPDATE KartRefit SET AntiCollisionWeight=?,RefitCout = RefitCout+?,AntiCollisionWeightRefitCout=? WHERE Uin=? AND KartID=?;";
		}
		result = sqlite3_prepare_v2(PlayerDB, sql1, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, SrcKartID);
			result = sqlite3_step(stmt);
			Z_INFO("结果:%d", result);
		}
		else
		{
			Z_ERROR("皮肤改装错误");
		}
		sqlite3_finalize(stmt);
		stmt = NULL;

		result = sqlite3_prepare_v2(PlayerDB, sql2, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, DstAfterValue);
			sqlite3_bind_int(stmt, 2, SrcRefitCout);
			sqlite3_bind_int(stmt, 3, SrcRefitCout);
			sqlite3_bind_int(stmt, 4, Client->Uin);
			sqlite3_bind_int(stmt, 5, DstKartID);
			result = sqlite3_step(stmt);
			Z_INFO("结果:%d", result);
		}
		else
		{
			Z_ERROR("皮肤改装错误");
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
}

void RequestGetKartRefitAttrTransferInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetRefitKartAttrTransferInfo(Client);
}

void ResponseGetRefitKartAttrTransferInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	struct RefitKartAttrTransferInfo // 转移卡消耗配置
	{
		UINT ItemID;
		UINT MinWeight;
	};
	RefitKartAttrTransferInfo RefitKartAttrTransferInfoCFG[] =
		{
			{96593, 0},
			{96594, 11},
			{96595, 20},
			{96596, 26},
		};
	struct RefitKartAttrTransferLevelInfo
	{
		UINT Level;
		UINT TheotyRefitCount;
		UINT TransferDecProp[5];
		UINT TransferCardNum;
	};

	RefitKartAttrTransferLevelInfo RefitKartAttrTransferLevelInfoCFG[] =
		{
			{0, 0, {10000, 0, 0, 0, 0}, 1},
			{1, 1, {10000, 0, 0, 0, 0}, 1},
			{2, 2, {10000, 0, 0, 0, 0}, 1},
			{3, 3, {10000, 0, 0, 0, 0}, 1},
			{4, 4, {10000, 0, 0, 0, 0}, 1},
			{5, 5, {10000, 0, 0, 0, 0}, 1},
			{6, 6, {10000, 0, 0, 0, 0}, 1},
			{7, 11, {8000, 2000, 0, 0, 0}, 1},
			{8, 16, {5000, 2000, 2000, 0, 0}, 1},
			{9, 26, {3000, 2000, 3000, 0, 0}, 1},
			{10, 36, {3000, 2000, 2000, 1000, 2000}, 2},
			{11, 47, {2046, 2000, 3500, 1000, 1454}, 2},
			{12, 64, {1556, 2000, 3500, 1000, 1944}, 2},
			{13, 89, {1193, 2000, 3500, 1000, 2307}, 2},
			{14, 114, {970, 2000, 3500, 1000, 2530}, 2},
			{16, 179, {597, 2000, 3500, 1000, 1903}, 2},
			{17, 279, {408, 2000, 3000, 1000, 3592}, 2},
			{18, 379, {310, 2000, 3000, 1000, 2690}, 2},
			{19, 479, {250, 2000, 3000, 1000, 2750}, 2},
			{20, 579, {2888, 1033, 1000, 1000, 4079}, 1},
			{21, 679, {2485, 623, 1500, 2000, 3392}, 1},
			{22, 790, {2151, 506, 3000, 2000, 2343}, 1},
			{23, 915, {1869, 427, 2000, 2000, 3704}, 1},
			{24, 1057, {1625, 366, 1500, 1500, 5009}, 1},
			{25, 1224, {2830, 616, 1500, 1500, 3554}, 2},
			{26, 1424, {2443, 559, 1500, 1500, 3998}, 2},
			{27, 1674, {2086, 531, 2500, 1500, 3383}, 2},
			{28, 2007, {1746, 603, 2500, 1500, 3651}, 2},
			{29, 2507, {1403, 1149, 2500, 1500, 3448}, 2},

		};
	struct CarListNumInfo
	{
		UINT CarTypeNum;
		UINT KartType[5];
	};
	CarListNumInfo CarListNumInfoCFG[] =
		{
			{3, {1539, 1556, 1548}},
			{3, {1543, 1557, 1558}},
			{5, {1540, 1541, 1546, 1547, 1565}},
		};
	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	{						 // RefitTransferCfg
		BYTE *pRefitTransferCfg = p;
		Write16(pRefitTransferCfg, 0);																			   // len
		Write8(pRefitTransferCfg, sizeof(RefitKartAttrTransferInfoCFG) / sizeof(RefitKartAttrTransferInfoCFG[0])); // CardNum
		for (size_t i = 0; i < sizeof(RefitKartAttrTransferInfoCFG) / sizeof(RefitKartAttrTransferInfoCFG[0]); i++)
		{ // TransferCard
			BYTE *pTransferCard = pRefitTransferCfg;
			Write16(pTransferCard, 0); // len

			Write32(pTransferCard, RefitKartAttrTransferInfoCFG[i].ItemID);	   // ItemID
			Write32(pTransferCard, RefitKartAttrTransferInfoCFG[i].MinWeight); // MinWeight

			len = pTransferCard - pRefitTransferCfg;
			Set16(pRefitTransferCfg, (WORD)len);
			pRefitTransferCfg += len;
		}
		Write8(pRefitTransferCfg, sizeof(RefitKartAttrTransferLevelInfoCFG) / sizeof(RefitKartAttrTransferLevelInfoCFG[0])); // LevelNum
		for (size_t i = 0; i < sizeof(RefitKartAttrTransferLevelInfoCFG) / sizeof(RefitKartAttrTransferLevelInfoCFG[0]); i++)
		{ // TransferLevel
			BYTE *pTransferLevel = pRefitTransferCfg;
			Write16(pTransferLevel, 0); // len

			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].Level);			  // Level
			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].TheotyRefitCount);	  // TheotyRefitCount
			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].TransferDecProp[0]); // TransferDecProp[]
			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].TransferDecProp[1]); // TransferDecProp[]
			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].TransferDecProp[2]); // TransferDecProp[]
			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].TransferDecProp[3]); // TransferDecProp[]
			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].TransferDecProp[4]); // TransferDecProp[]
			Write32(pTransferLevel, RefitKartAttrTransferLevelInfoCFG[i].TransferCardNum);	  // TransferCardNum

			len = pTransferLevel - pRefitTransferCfg;
			Set16(pRefitTransferCfg, (WORD)len);
			pRefitTransferCfg += len;
		}
		Write8(pRefitTransferCfg, sizeof(CarListNumInfoCFG) / sizeof(CarListNumInfoCFG[0])); // CarListNum
		for (size_t i = 0; i < sizeof(CarListNumInfoCFG) / sizeof(CarListNumInfoCFG[0]); i++)
		{ // TransferCar
			BYTE *pTransferCar = pRefitTransferCfg;
			Write16(pTransferCar, 0); // len

			Write8(pTransferCar, CarListNumInfoCFG[i].CarTypeNum); // CarTypeNum
			for (int j = 0; j < CarListNumInfoCFG[i].CarTypeNum; j++)
			{
				Write32(pTransferCar, CarListNumInfoCFG[i].KartType[j]); // KartType[]
			}
			len = pTransferCar - pRefitTransferCfg;
			Set16(pRefitTransferCfg, (WORD)len);
			pRefitTransferCfg += len;
		}
		Write16(pRefitTransferCfg, 0); // DescLen

		len = pRefitTransferCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 1214, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyTaskAward(ClientNode *Client, UINT TaskID, UINT TaskWindowType)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write32(p, TaskID);		 // TaskID
	{						 // TaskAward
		BYTE *pTaskAward = p;
		Write16(pTaskAward, 0); // len

		Write32(pTaskAward, 9999); // AwardMoney
		Write32(pTaskAward, 9999); // TotalMoney
		Write32(pTaskAward, 9999); // AwardSuperMoney
		Write32(pTaskAward, 9999); // TotalSuperMoney
		Write32(pTaskAward, 9999); // AwardExp
		Write32(pTaskAward, 9999); // TotalExp
		Write32(pTaskAward, 1);	   // AwardItemNum
		for (size_t i = 0; i < 1; i++)
		{ // AwardItemInfo
			BYTE *pAwardItemInfo = pTaskAward;
			Write16(pAwardItemInfo, 0); // len

			Write32(pAwardItemInfo, 12720); // ItemID
			Write32(pAwardItemInfo, 1);		// ItemNum
			Write32(pAwardItemInfo, -1);	// AvailPeriod
			Write8(pAwardItemInfo, 0);		// Status
			WriteUn(pAwardItemInfo, 0);		// ObtainTime
			Write32(pAwardItemInfo, 0);		// OtherAttribute
			Write16(pAwardItemInfo, 0);		// ItemType

			len = pAwardItemInfo - pTaskAward;
			Set16(pTaskAward, (WORD)len);
			pTaskAward += len;
		}
		Write32(pTaskAward, 0); // AwardCoupons
		Write32(pTaskAward, 0); // TotalCoupons
		Write32(pTaskAward, 0); // AwardLuckMoney
		Write32(pTaskAward, 0); // TotalLuckMoney
		Write32(pTaskAward, 0); // AwardTeamWorkExp
		Write32(pTaskAward, 0); // TotalTeamWorkExp
		Write32(pTaskAward, 0); // AwardWlCash
		Write32(pTaskAward, 0); // AwardWlCrystal
		Write32(pTaskAward, 0); // AwardWlDegree
		Write32(pTaskAward, 0); // ChumCircleActivity
		Write32(pTaskAward, 0); // GuildScore
		Write16(pTaskAward, 0); // AppellcationID
		Write16(pTaskAward, 0); // ScoreJingJi
		Write8(pTaskAward, 0);	// ExtendInfoNum
		for (size_t i = 0; i < 0; i++)
		{ // ExtendInfoAward
			BYTE *pExtendInfoAward = pTaskAward;
			Write16(pExtendInfoAward, 0); // len

			Write16(pExtendInfoAward, 0); // Key
			Write32(pExtendInfoAward, 0); // AddValue
			Write32(pExtendInfoAward, 0); // TotalValue
			Write8(pExtendInfoAward, 0);  // ShowPriority
			Write16(pExtendInfoAward, 0); // BuyNeedScore
			Write8(pExtendInfoAward, 0);  // OneMatchMaxNum

			len = pExtendInfoAward - pTaskAward;
			Set16(pTaskAward, (WORD)len);
			pTaskAward += len;
		}
		Write32(pTaskAward, 0); // LiveExp
		Write32(pTaskAward, 0); // AwardSpeedCoin
		Write32(pTaskAward, 0); // TotalSpeedCoin

		len = pTaskAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0);				// NextTaskID
	Write16(p, 0);				// Type
	Write32(p, TaskWindowType); // TaskWindowType
	Write8(p, 0);				// TaskTipsLen
	Write8(p, 0);				// ParaNum
	Write32(p, 0);				// ParaList[]

	len = p - buf;
	SendToClient(Client, 537, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyUpdateAchieveProgress(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 1);			 // AchieveNum
	for (size_t i = 0; i < 1; i++)
	{ // Achieve
		BYTE *pAchieve = p;
		Write16(pAchieve, 0); // len

		Write16(pAchieve, 1); // ID
		Write8(pAchieve, 1);  // ShowType
		Write8(pAchieve, 0);  // LogicType
		Write8(pAchieve, 0);  // Status
		Write16(pAchieve, 0); // Progress
		Write16(pAchieve, 0); // TotalProgress
		Write32(pAchieve, 0); // FinTime
		Write16(pAchieve, 0); // AchieveValue
		Write32(pAchieve, 0); // AchieveFlagType

		len = pAchieve - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 759, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RemoveAllClothes(ClientNode* Client) {
	short ItemNum = 0;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	ItemStatus* aItemStatus = (ItemStatus*)malloc(sizeof(ItemStatus) * 50);
	sql = "SELECT ItemID FROM ITEM WHERE Uin = ? AND STATUS = 1";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			if (GetItemType(sqlite3_column_int(stmt, 0))== EAIT_CLOTHES) {
				ItemNum++;
				aItemStatus[ItemNum].ItemID = sqlite3_column_int(stmt, 0);
				aItemStatus[ItemNum].NewStatus = 0;
				PlayDB_UpdateItemStatus(Client->Uin, sqlite3_column_int(stmt, 0), 0);
			}
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	ResponseChangeItemStatus(Client, ItemNum, aItemStatus);
	// ResponseSaveShapeRefit4(Client,Client->KartID);
	do
	{ // 普通房间
		RoomNode* Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			break;
		}
		for (char i = 0; i < 6; i++)
		{
			ClientNode* RoomClient = Room->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
			}
		}
	} while (false);
	do
	{ // 休闲区
		ChatRoomNode* ChatRoom = GetChatRoom(Client->ChatRoomID);
		if (!ChatRoom)
		{
			return;
		}
		for (char i = 0; i < 99; i++)
		{
			ClientNode* RoomClient = ChatRoom->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyChangeItemStatus(RoomClient, Client->Uin, ItemNum, aItemStatus);
			}
		}
	} while (false);
	free(aItemStatus);
}
