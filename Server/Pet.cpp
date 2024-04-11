#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Message.h"
#include "Pet.h"

#include <map>
#include <sqlite/sqlite3.h>
std::map<UINT, UINT> Levels;
std::map<UINT, UINT> PetStrengs;
std::map<UINT, UINT> PetHuanHuas;

extern sqlite3 *PlayerDB;

void LoadPetLevelConfig()
{
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\Pet.yml");

		for (size_t i = 0; i < Config["Levels"].size(); i++)
		{
			YAML::Node Level = Config["Levels"][i];

			Levels[Level["Level"].as<UINT>()] = Level["Exp"].as<UINT>();
		}
		for (size_t i = 0; i < Config["PetStrengs"].size(); i++)
		{
			YAML::Node PetStreng = Config["PetStrengs"][i];
			PetStrengs[PetStreng["ID"].as<UINT>()] = PetStreng["ID"].as<UINT>();
		}
		for (size_t i = 0; i < Config["PetHuanHuas"].size(); i++)
		{
			YAML::Node PetHuanHua = Config["PetHuanHuas"][i];
			PetHuanHuas[PetHuanHua["ID"].as<UINT>()] = PetHuanHua["ID"].as<UINT>();
		}
		for (size_t i = 0; i < Config["PetAllStatus"].size(); i++)
		{
			YAML::Node PetAllStatus = Config["PetAllStatus"][i];
			PetStrengs[PetAllStatus["ID"].as<UINT>()] = PetAllStatus["ID"].as<UINT>();
			PetHuanHuas[PetAllStatus["ID"].as<UINT>()] = PetAllStatus["ID"].as<UINT>();
		}
	}
	catch (const std::exception &)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
		Z_ERROR("加载宠物升级配置出错\n");
		exit(0);
	}
	Z_INFO("宠物升级配置加载完成[%d]!\n", Levels.size());
}
PetInfo* GetPetItem(UINT PetID,UINT Uin)
{
	PetInfo* pPetItem = new PetInfo;
	pPetItem->ID = -1;
	
	
	sqlite3_stmt* stmt = NULL;
	if (sqlite3_prepare_v2(PlayerDB, "SELECT Name,Experience,PL,Status FROM Pet WHERE Uin=? AND PetId=?;", -1, &stmt, NULL) == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		sqlite3_bind_int(stmt, 2, PetID);

		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			pPetItem->ID = PetID;
			char PetName[23] = "";
			memcpy(PetName, (char*)sqlite3_column_text(stmt, 0), 23);
			sprintf_s(pPetItem->Name, PetName);

			pPetItem->Exp = sqlite3_column_int(stmt, 1);
			pPetItem->PL = sqlite3_column_int(stmt, 2);
			pPetItem->Level = GetPetLevel(pPetItem->Exp);
			pPetItem->MaxExp = GetPetMaxExp(pPetItem->Level);
			pPetItem->Status = sqlite3_column_int(stmt, 3);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	return pPetItem;
}
void GetPetInfo(ClientNode *Client)
{
	
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
		
	}
	else {
		sqlite3_stmt* stmt = NULL;
		bool hasPet = false;
		if (sqlite3_prepare_v2(PlayerDB, "SELECT Name,Experience,PL FROM Pet WHERE Uin=? AND PetId=?;", -1, &stmt, NULL) == SQLITE_OK)
		{
			sqlite3_bind_int(stmt, 1, Client->Uin);
			sqlite3_bind_int(stmt, 2, Client->Pet->ID);

			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				hasPet = true;
				char PetName[23] = "";
				memcpy(PetName, (char*)sqlite3_column_text(stmt, 0), 23);
				sprintf_s(Client->Pet->Name, PetName);

				Client->Pet->Exp = sqlite3_column_int(stmt, 1);
				Client->Pet->PL = sqlite3_column_int(stmt, 2);
				Client->Pet->Level = GetPetLevel(Client->Pet->Exp);
				Client->Pet->MaxExp = GetPetMaxExp(Client->Pet->Level);
				Client->Pet->Status = GetPetStatus(Client->Pet->Level, Client->Pet->ID);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		if (!hasPet)
		{
			// 宠物信息
			if (sqlite3_prepare_v2(PlayerDB, "INSERT INTO Pet(Uin,PetId,Name,Experience,PL) VALUES (?,?,?,?,?);", -1, &stmt, NULL) == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Client->Uin);
				sqlite3_bind_int(stmt, 2, Client->Pet->ID);
				sqlite3_bind_text(stmt, 3, "神宠", strlen("神宠"), SQLITE_TRANSIENT);
				sqlite3_bind_int(stmt, 4, 238966);
				sqlite3_bind_int(stmt, 5, 0);
				sqlite3_step(stmt);
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
			GetPetInfo(Client);
		}
	}
	
}

UINT GetPetLevel(UINT Exp)
{
	UINT Level = 0;
	for (size_t i = 1; i <= 30; i++)
	{
		if (Exp < Levels[i])
		{
			break;
		}
		Level++;
	}
	return Level;
}

UINT GetPetStatus(UINT Level,UINT PetID)
{
	UINT Status = 1;
	if (Level >= 30)
	{
		Status = 3;
		if (PetHuanHuas[PetID]) {
			Status = 4;
		}
		else {
			PetHuanHuas.erase(PetID);
		}
		if (PetStrengs[PetID]) {
			Status = 4;
		}
		else {
			PetStrengs.erase(PetID);
		}
	}
	else if (Level >= 20)
	{
		Status = 2;
	}
	return Status;
}

UINT GetPetMaxExp(UINT Level)
{
	if (Level < 30)
	{
		Level += 1;
	}
	return Levels[Level];
}

YAML::Node GetPetConfig(UINT PetID)
{
	char FilePath[MAX_PATH];
	YAML::Node Config;

	sprintf_s(FilePath, ".\\Pet\\%d.yml", PetID);
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);

	if (isFileExists)
	{
		Config = YAML::LoadFile(FilePath);
	}
	else
	{
#ifdef DEBUG
		Z_ERROR("缺少 Pet %d 的配置文件\n", PetID);
#endif
	}

	return Config;
}

void RequestUpdatePetName(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);

	char *PetName = (char *)p;
	if (getGameConfig()["MySQL"] && getGameConfig()["MySQL"]["Open"] && getGameConfig()["MySQL"]["Open"].as<UINT>() == 1)
	{
	}
	else {
		sqlite3_stmt* stmt = NULL;
		if (sqlite3_prepare_v2(PlayerDB, "UPDATE Pet SET Name=? WHERE Uin = ? AND PetId = ?;", -1, &stmt, NULL) == SQLITE_OK)
		{
			sqlite3_bind_text(stmt, 1, PetName, strlen(PetName), SQLITE_TRANSIENT);
			sqlite3_bind_int(stmt, 2, Client->Uin);
			sqlite3_bind_int(stmt, 3, Client->Pet->ID);
			sqlite3_step(stmt);
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
	}
	
	ResponseUpdatePetName(Client);
}

void ResponseUpdatePetName(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // Result

	len = p - buf;
	SendToClient(Client, 178, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestChangePetEquipStatus(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int PetID = Read32(p);
	int NewStatus = Read8(p);
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "UPDATE Pet SET Status=? WHERE Uin=? and PetID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK) {
		sqlite3_bind_int(stmt, 1, NewStatus);
		sqlite3_bind_int(stmt, 2, Uin);
		sqlite3_bind_int(stmt, 3, PetID);
		result = sqlite3_step(stmt);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	ResponseChangePetEquipStatus(Client, NewStatus);
	NotifyBroadcastPetEvolution(Client, PetID, NewStatus);
}

void ResponseChangePetEquipStatus(ClientNode *Client, int NewStatus)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0); // ResultID
	Write8(p, NewStatus);  // NewStatus
	Write16(p, 0); // ReasonLen

	len = p - buf;
	SendToClient(Client, 15398, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyAddPet(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	INT n = 1;
	Write32(p, 0); // Uin
	{			   // PetInfo
		BYTE *pPetInfo = p;
		Write16(pPetInfo, 0); // len

		Write32(pPetInfo, 0); // Uin
		Write32(pPetInfo, 0); // PetId
		strcpy_s((char *)pPetInfo, 100, "神宠");
		pPetInfo += 100;
		// WriteString(pPetInfo, 0); //PetName[]
		Write8(pPetInfo, 0);  // IsProper
		Write16(pPetInfo, 0); // LevelLimit
		Write16(pPetInfo, 0); // Level
		Write32(pPetInfo, 0); // CurLevelExp
		Write32(pPetInfo, 0); // CurLevelMaxExp
		Write8(pPetInfo, 0);  // Status
		Write16(pPetInfo, 0); // PL
		Write16(pPetInfo, 0); // TotalPL
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

		len = pPetInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // PetPKInfo
		BYTE *pPetPKInfo = p;
		Write16(pPetPKInfo, 0); // len

		Write32(pPetPKInfo, 0); // PetId
		Write8(pPetPKInfo, 0);	// PetType
		Write16(pPetPKInfo, 0); // FightLevel
		Write16(pPetPKInfo, 0); // Star
		Write32(pPetPKInfo, 0); // CurLevelExp
		Write32(pPetPKInfo, 0); // CurLevelTotalExp
		Write16(pPetPKInfo, 0); // ExtraPotentialByStar[]
		Write32(pPetPKInfo, 0); // SkillNum
		for (size_t i = 0; i < n; i++)
		{ // Skill
			BYTE *pSkill = pPetPKInfo;
			Write16(pSkill, 0); // len

			Write32(pSkill, 0); // SkillID
			Write16(pSkill, 0); // SkillLevel
			Write8(pSkill, 0);	// SkillType
			Write8(pSkill, 0);	// IsCanLevelUp
			Write16(pSkill, 0); // LevelUpFightLevel
			Write16(pSkill, 0); // AttachLen

			len = pSkill - pPetPKInfo;
			Set16(pPetPKInfo, (WORD)len);
			pPetPKInfo += len;
		}
		Write32(pPetPKInfo, 0); // FightPower
		Write32(pPetPKInfo, 0); // Attribute[]
		Write8(pPetPKInfo, 0);	// IsTopFightLevel
		Write32(pPetPKInfo, 0); // HPIncByStatus
		Write8(pPetPKInfo, 0);	// HPIncPercent
		Write8(pPetPKInfo, 0);	// Status
		Write32(pPetPKInfo, 0); // MaxFightPower
		Write16(pPetPKInfo, 0); // AttrMotivateLevel[]

		len = pPetPKInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 586, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyPetEvolution(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Result
	Write16(p, 1); //PreStatus
	{ //PetInfo
		BYTE* pPetInfo = p;
		Write16(pPetInfo, 0); //len

		Write32(pPetInfo, Client->Uin); //Uin
		Write32(pPetInfo, Client->Pet->ID); //PetId
		WriteString(pPetInfo, Client->Pet->Name); //PetName[]
		Write8(pPetInfo, 1); //IsProper
		Write16(pPetInfo, Client->Pet->Level); //LevelLimit
		Write16(pPetInfo, Client->Pet->Level); //Level
		Write32(pPetInfo, Client->Pet->Exp); //CurLevelExp
		Write32(pPetInfo, Client->Pet->MaxExp); //CurLevelMaxExp
		Write8(pPetInfo, Client->Pet->Status); //Status
		Write16(pPetInfo, Client->Pet->PL); //PL
		Write16(pPetInfo, 100); //TotalPL
		YAML::Node PetSkills = GetPetConfig(Client->Pet->ID);
		if (PetSkills.size() < Client->Pet->Status) {
			Write16(pPetInfo, 0); //PetSkillNum
		}
		else {
			Write16(pPetInfo, PetSkills.size()); //PetSkillNum
			for (size_t i = 0; i < 0; i++)
			{ //PetSkillList
				BYTE* pPetSkillList = pPetInfo;
				Write16(pPetSkillList, 0); //len

				Write16(pPetSkillList, PetSkills[i]["SkillID"].as<UINT>()); //SkillID
				Write8(pPetSkillList, i + 1); //Status
				Write8(pPetSkillList, 1); //Active
				Write32(pPetSkillList, PetSkills[i]["Value"].as<float>() * Client->Pet->Level); //Value

				len = pPetSkillList - pPetInfo;
				Set16(pPetInfo, (WORD)len);
				pPetInfo += len;
			}

		}
		Write32(pPetInfo, 0); //StrengLevel
		Write8(pPetInfo, 0); //TopStatus
		{ //CommonInfo
			BYTE* pCommonInfo = pPetInfo;
			Write16(pCommonInfo, 0); //len

			Write32(pCommonInfo, 0); //HuanHuaLevel
			Write8(pCommonInfo, 0); //CanHuanHua

			len = pCommonInfo - pPetInfo;
			Set16(pPetInfo, (WORD)len);
			pPetInfo += len;
		}

		len = pPetInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); //MsgLen

	len = p - buf;
	SendToClient(Client, 584, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestC2GGetWizardBaseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	USHORT DstWizardID = Read16(p);
	bool Reason = Read8(p);
	ResponseC2GGetWizardBaseInfo(Client);
}

void ResponseC2GGetWizardBaseInfo(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write32(p, Client->Uin); // DstUin
	Write16(p, 62844);		 // DstWizardID
	Write16(p, 1);			 // Num
	for (size_t i = 0; i < 1; i++)
	{ // PlayerDBWizardBaseInfo
		BYTE *pPlayerDBWizardBaseInfo = p;
		Write16(pPlayerDBWizardBaseInfo, 0); // len

		{ // WizardBaseInfo
			BYTE *pWizardBaseInfo = pPlayerDBWizardBaseInfo;
			Write16(pWizardBaseInfo, 0); // len

			Write16(pWizardBaseInfo, 62844);		// WizardID
			Write16(pWizardBaseInfo, 1);			// WizardType
			WriteString(pWizardBaseInfo, "肖经理"); // NickName[]
			Write32(pWizardBaseInfo, 0);			// Experience
			Write8(pWizardBaseInfo, 0);				// Level
			Write8(pWizardBaseInfo, 0);				// Order
			Write32(pWizardBaseInfo, 0);			// Health
			Write8(pWizardBaseInfo, 0);				// IsRare
			Write8(pWizardBaseInfo, 0);				// Intimacy
			Write8(pWizardBaseInfo, 0);				// IsHuanHua
			Write8(pWizardBaseInfo, 0);				// IsShow
			Write8(pWizardBaseInfo, 0);				// IsCarry
			Write8(pWizardBaseInfo, 0);				// MaxSkillNum
			{										// WizardSkillInfoList
				BYTE *pWizardSkillInfoList = pWizardBaseInfo;
				Write16(pWizardSkillInfoList, 0); // len

				Write8(pWizardSkillInfoList, 0); // SkillNum
				for (size_t i = 0; i < 0; i++)
				{ // SkillInfo
					BYTE *pSkillInfo = pWizardSkillInfoList;
					Write16(pSkillInfo, 0); // len

					Write8(pSkillInfo, 0); // SkillId
					Write8(pSkillInfo, 0); // SkillLevel

					len = pSkillInfo - pWizardSkillInfoList;
					Set16(pWizardSkillInfoList, (WORD)len);
					pWizardSkillInfoList += len;
				}

				len = pWizardSkillInfoList - pWizardBaseInfo;
				Set16(pWizardBaseInfo, (WORD)len);
				pWizardBaseInfo += len;
			}
			Write8(pWizardBaseInfo, 1);	 // Status
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
	Write8(p, 0); // Reason

	len = p - buf;
	SendToClient(Client, 11526, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyBroadcastPetEvolution(ClientNode* Client,UINT PetID,UINT Status)
{//Lupin
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	{ //PetInfo
		BYTE* pPetInfo = p;
		Write16(pPetInfo, 0); //len
		Write32(pPetInfo, Client->Uin); //Uin
		Write32(pPetInfo, PetID); //PetId
		memcpy(pPetInfo, "神宠", 23);
		pPetInfo += 23;
		Write8(pPetInfo, 0); //IsProper
		Write16(pPetInfo, 30); //Level
		Write8(pPetInfo, Status); //Status
		Write16(pPetInfo, 0); //PetSkillNum

		Write32(pPetInfo, 10); //StrengLevel

		len = pPetInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;

	SendToClient(Client, 587, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyPetStrengCfgInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //StrengCardId
	Write32(p, PetStrengs.size()); //PetStrengNum
	std::map<UINT, UINT>::iterator iter;
	for (iter = PetStrengs.begin(); iter != PetStrengs.end(); iter++)
	{ //PetStrengInfos
		BYTE* pPetStrengInfos = p;
		Write16(pPetStrengInfos, 0); //len

		Write32(pPetStrengInfos, iter->second); //Petid
		Write32(pPetStrengInfos, 0); //LevelNum
		for (size_t i = 0; i < 0; i++)
		{ //PetStrengLevels
			BYTE* pPetStrengLevels = pPetStrengInfos;
			Write16(pPetStrengLevels, 0); //len

			Write32(pPetStrengLevels, i+1); //Level
			Write32(pPetStrengLevels, 0); //SuccProb
			Write32(pPetStrengLevels, 0); //ConsCardNum
			Write32(pPetStrengLevels, 0); //ConsSuperMoney
			Write32(pPetStrengLevels, 3); //SkillNum
			for (size_t i = 0; i < 3; i++)
			{ //SkillInfos
				BYTE* pSkillInfos = pPetStrengLevels;
				Write16(pSkillInfos, 0); //len

				Write16(pSkillInfos, i+1); //SkillID
				Write8(pSkillInfos, 0); //Status
				Write8(pSkillInfos, 1); //Active
				Write32(pSkillInfos, 10); //Value

				len = pSkillInfos - pPetStrengLevels;
				Set16(pPetStrengLevels, (WORD)len);
				pPetStrengLevels += len;
			}
			Write8(pPetStrengLevels, 0); //Type
			Write32(pPetStrengLevels, 0); //HuanHuaItemPeriod
			Write32(pPetStrengLevels, 0); //BeginDate
			Write32(pPetStrengLevels, 0); //BeginTime
			Write32(pPetStrengLevels, 0); //BonusProb
			Write32(pPetStrengLevels, 0); //BonusProbBeginDate
			Write32(pPetStrengLevels, 0); //BonusProbBeginTime
			Write32(pPetStrengLevels, 0); //BonusProbEndDate
			Write32(pPetStrengLevels, 0); //BonusProbEndTime
			Write32(pPetStrengLevels, 0); //BaojiBeginNum
			Write32(pPetStrengLevels, 0); //BaojiInc

			len = pPetStrengLevels - pPetStrengInfos;
			Set16(pPetStrengInfos, (WORD)len);
			pPetStrengInfos += len;
		}
		Write32(pPetStrengInfos, 0); //HuanHuaItemID

		len = pPetStrengInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //HasMoreInfo

	len = p - buf;
	SendToClient(Client, 20100, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyPetStrengCfgMoreInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //PetStrengNum
	for (size_t i = 0; i < 0; i++)
	{ //PetStrengInfos
		BYTE* pPetStrengInfos = p;
		Write16(pPetStrengInfos, 0); //len

		Write32(pPetStrengInfos, 0); //Petid
		Write32(pPetStrengInfos, 0); //LevelNum
		for (size_t i = 0; i < 0; i++)
		{ //PetStrengLevels
			BYTE* pPetStrengLevels = pPetStrengInfos;
			Write16(pPetStrengLevels, 0); //len

			Write32(pPetStrengLevels, 0); //Level
			Write32(pPetStrengLevels, 0); //SuccProb
			Write32(pPetStrengLevels, 0); //ConsCardNum
			Write32(pPetStrengLevels, 0); //ConsSuperMoney
			Write32(pPetStrengLevels, 0); //SkillNum
			for (size_t i = 0; i < 0; i++)
			{ //SkillInfos
				BYTE* pSkillInfos = pPetStrengLevels;
				Write16(pSkillInfos, 0); //len

				Write16(pSkillInfos, 0); //SkillID
				Write8(pSkillInfos, 0); //Status
				Write8(pSkillInfos, 0); //Active
				Write32(pSkillInfos, 0); //Value

				len = pSkillInfos - pPetStrengLevels;
				Set16(pPetStrengLevels, (WORD)len);
				pPetStrengLevels += len;
			}
			Write8(pPetStrengLevels, 0); //Type
			Write32(pPetStrengLevels, 0); //HuanHuaItemPeriod
			Write32(pPetStrengLevels, 0); //BeginDate
			Write32(pPetStrengLevels, 0); //BeginTime
			Write32(pPetStrengLevels, 0); //BonusProb
			Write32(pPetStrengLevels, 0); //BonusProbBeginDate
			Write32(pPetStrengLevels, 0); //BonusProbBeginTime
			Write32(pPetStrengLevels, 0); //BonusProbEndDate
			Write32(pPetStrengLevels, 0); //BonusProbEndTime
			Write32(pPetStrengLevels, 0); //BaojiBeginNum
			Write32(pPetStrengLevels, 0); //BaojiInc

			len = pPetStrengLevels - pPetStrengInfos;
			Set16(pPetStrengInfos, (WORD)len);
			pPetStrengInfos += len;
		}
		Write32(pPetStrengInfos, 0); //HuanHuaItemID

		len = pPetStrengInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //HasNext

	len = p - buf;
	SendToClient(Client, 28228, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyPetHuanHuaCfgInfo(ClientNode* Client)
{
	BYTE buf[8192*4]; BYTE* p = buf; size_t len;

	Write32(p, 0); //StrengCardId
	Write32(p, PetHuanHuas.size()); //PetStrengNum
	std::map<UINT, UINT>::iterator iter;
	for (iter = PetHuanHuas.begin(); iter != PetHuanHuas.end(); iter++)
	{ //PetStrengInfos
		BYTE* pPetStrengInfos = p;
		Write16(pPetStrengInfos, 0); //len

		Write32(pPetStrengInfos, iter->second); //Petid
		Write32(pPetStrengInfos, 0); //LevelNum
		for (size_t i = 0; i < 0; i++)
		{ //PetStrengLevels
			BYTE* pPetStrengLevels = pPetStrengInfos;
			Write16(pPetStrengLevels, 0); //len

			Write32(pPetStrengLevels, i+1); //Level
			Write32(pPetStrengLevels, 0); //SuccProb
			Write32(pPetStrengLevels, 0); //ConsCardNum
			Write32(pPetStrengLevels, 0); //ConsSuperMoney
			Write32(pPetStrengLevels, 3); //SkillNum
			for (size_t i = 0; i < 3; i++)
			{ //SkillInfos
				BYTE* pSkillInfos = pPetStrengLevels;
				Write16(pSkillInfos, 0); //len

				Write16(pSkillInfos, i+1); //SkillID
				Write8(pSkillInfos, 0); //Status
				Write8(pSkillInfos, 1); //Active
				Write32(pSkillInfos, 10); //Value

				len = pSkillInfos - pPetStrengLevels;
				Set16(pPetStrengLevels, (WORD)len);
				pPetStrengLevels += len;
			}
			Write8(pPetStrengLevels, 0); //Type
			Write32(pPetStrengLevels, 0); //HuanHuaItemPeriod
			Write32(pPetStrengLevels, 0); //BeginDate
			Write32(pPetStrengLevels, 0); //BeginTime
			Write32(pPetStrengLevels, 0); //BonusProb
			Write32(pPetStrengLevels, 0); //BonusProbBeginDate
			Write32(pPetStrengLevels, 0); //BonusProbBeginTime
			Write32(pPetStrengLevels, 0); //BonusProbEndDate
			Write32(pPetStrengLevels, 0); //BonusProbEndTime
			Write32(pPetStrengLevels, 0); //BaojiBeginNum
			Write32(pPetStrengLevels, 0); //BaojiInc

			len = pPetStrengLevels - pPetStrengInfos;
			Set16(pPetStrengInfos, (WORD)len);
			pPetStrengInfos += len;
		}
		Write32(pPetStrengInfos, 0); //HuanHuaItemID

		len = pPetStrengInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24073, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetPKPetInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT DstUin = Read32(p);
	char Flag = Read8(p);
	int PetId = Read32(p);
	ResponseGetPKPetInfo(Client, DstUin, Flag, PetId);
}

void ResponseGetPKPetInfo(ClientNode* Client, UINT DstUin, char Flag, int PetId)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write32(p, 0); //Time
	Write16(p, 0); //ResultID
	Write16(p, 0); //ReasonLen
	Write32(p, DstUin); //DstUin
	Write8(p, Flag); //Flag
	Write32(p, 0); //IDs[]
	Write16(p, 0); //PKPetNum
	for (size_t i = 0; i < 0; i++)
	{ //PKPetInfo
		BYTE* pPKPetInfo = p;
		Write16(pPKPetInfo, 0); //len

		Write32(pPKPetInfo, 0); //PetId
		Write8(pPKPetInfo, 0); //PetType
		Write16(pPKPetInfo, 0); //FightLevel
		Write16(pPKPetInfo, 0); //Star
		Write32(pPKPetInfo, 0); //CurLevelExp
		Write32(pPKPetInfo, 0); //CurLevelTotalExp
		Write16(pPKPetInfo, 0); //ExtraPotentialByStar[]
		Write32(pPKPetInfo, 0); //SkillNum
		for (size_t i = 0; i < 0; i++)
		{ //Skill
			BYTE* pSkill = pPKPetInfo;
			Write16(pSkill, 0); //len

			Write32(pSkill, 0); //SkillID
			Write16(pSkill, 0); //SkillLevel
			Write8(pSkill, 0); //SkillType
			Write8(pSkill, 0); //IsCanLevelUp
			Write16(pSkill, 0); //LevelUpFightLevel
			Write16(pSkill, 0); //AttachLen

			len = pSkill - pPKPetInfo;
			Set16(pPKPetInfo, (WORD)len);
			pPKPetInfo += len;
		}
		Write32(pPKPetInfo, 0); //FightPower
		Write32(pPKPetInfo, 0); //Attribute[]
		Write8(pPKPetInfo, 0); //IsTopFightLevel
		Write32(pPKPetInfo, 0); //HPIncByStatus
		Write8(pPKPetInfo, 0); //HPIncPercent
		Write8(pPKPetInfo, 0); //Status
		Write32(pPKPetInfo, 0); //MaxFightPower
		Write16(pPKPetInfo, 0); //AttrMotivateLevel[]

		len = pPKPetInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //TotalFightPower
	Write32(p, 0); //CaptainPetID
	Write32(p, 0); //RankedMatchLevel
	Write32(p, 0); //RankedMatchScore
	Write32(p, 0); //RankedMatchRank
	Write8(p, 0); //HasMoreInfo
	Write32(p, 0); //WeeklyPetStarRefreshFreeLimit
	Write32(p, 0); //WeeklyPetStarRefreshFreeCnt
	Write8(p, 0); //HasPkArrayInfo
	for (size_t i = 0; i < 0; i++)
	{ //PKArrayInfo
		BYTE* pPKArrayInfo = p;
		Write16(pPKArrayInfo, 0); //len

		Write8(pPKArrayInfo, 0); //UsingArrayID
		Write8(pPKArrayInfo, 0); //ArrayNum
		for (size_t i = 0; i < 0; i++)
		{ //PetPkArrayInfo
			BYTE* pPetPkArrayInfo = pPKArrayInfo;
			Write16(pPetPkArrayInfo, 0); //len

			Write8(pPetPkArrayInfo, 0); //ArrayID
			Write8(pPetPkArrayInfo, 0); //Level
			Write32(pPetPkArrayInfo, 0); //CaptainPetID
			Write32(pPetPkArrayInfo, 0); //TotalFightPower
			Write32(pPetPkArrayInfo, 0); //IDs[]

			len = pPetPkArrayInfo - pPKArrayInfo;
			Set16(pPKArrayInfo, (WORD)len);
			pPKArrayInfo += len;
		}

		len = pPKArrayInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 21133, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestPetTrialInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT TeamID = Read16(p);
	ResponsePetTrialInfo(Client, TeamID);
}

void ResponsePetTrialInfo(ClientNode* Client, USHORT TeamID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID
	Write8(p, 0); //TrialNum
	for (size_t i = 0; i < 0; i++)
	{ //PetTrialInfos
		BYTE* pPetTrialInfos = p;
		Write16(pPetTrialInfos, 0); //len

		Write8(pPetTrialInfos, 0); //TrialName[]
		Write8(pPetTrialInfos, 0); //WeekDayNum
		WriteUn(pPetTrialInfos, 0); //aucWeekDays[]
		Write8(pPetTrialInfos, 0); //TrialLevelNum
		for (size_t i = 0; i < 0; i++)
		{ //TrialLevelInfos
			BYTE* pTrialLevelInfos = pPetTrialInfos;
			Write16(pTrialLevelInfos, 0); //len

			Write8(pTrialLevelInfos, 0); //LevelID
			Write32(pTrialLevelInfos, 0); //MonsterCaptainID
			Write8(pTrialLevelInfos, 0); //MonsterName[]
			Write32(pTrialLevelInfos, 0); //MonsterPower
			Write32(pTrialLevelInfos, 0); //MinFightPower
			Write8(pTrialLevelInfos, 0); //ArrayID
			Write8(pTrialLevelInfos, 0); //MonsterNum
			for (size_t i = 0; i < 0; i++)
			{ //MonsterInfo
				BYTE* pMonsterInfo = pTrialLevelInfos;
				Write16(pMonsterInfo, 0); //len

				Write32(pMonsterInfo, 0); //MonsterItemID
				Write8(pMonsterInfo, 0); //SeatID
				Write8(pMonsterInfo, 0); //FormLevel
				Write16(pMonsterInfo, 0); //SkillNums
				for (size_t i = 0; i < 0; i++)
				{ //NormalSkillInfos
					BYTE* pNormalSkillInfos = pMonsterInfo;
					Write16(pNormalSkillInfos, 0); //len

					Write16(pNormalSkillInfos, 0); //SkillID
					Write16(pNormalSkillInfos, 0); //SkillLevel

					len = pNormalSkillInfos - pMonsterInfo;
					Set16(pMonsterInfo, (WORD)len);
					pMonsterInfo += len;
				}
				Write16(pMonsterInfo, 0); //AngerSkillNums
				for (size_t i = 0; i < 0; i++)
				{ //AngerSkillInfos
					BYTE* pAngerSkillInfos = pMonsterInfo;
					Write16(pAngerSkillInfos, 0); //len

					Write16(pAngerSkillInfos, 0); //SkillID
					Write16(pAngerSkillInfos, 0); //SkillLevel

					len = pAngerSkillInfos - pMonsterInfo;
					Set16(pMonsterInfo, (WORD)len);
					pMonsterInfo += len;
				}
				Write16(pMonsterInfo, 0); //SpecialAngerSkillNums
				for (size_t i = 0; i < 0; i++)
				{ //SpecialAngerSkillInfos
					BYTE* pSpecialAngerSkillInfos = pMonsterInfo;
					Write16(pSpecialAngerSkillInfos, 0); //len

					Write16(pSpecialAngerSkillInfos, 0); //SkillID
					Write16(pSpecialAngerSkillInfos, 0); //SkillLevel

					len = pSpecialAngerSkillInfos - pMonsterInfo;
					Set16(pMonsterInfo, (WORD)len);
					pMonsterInfo += len;
				}
				Write8(pMonsterInfo, 0); //AttrNum
				Write32(pMonsterInfo, 0); //FinalAttr[]

				len = pMonsterInfo - pTrialLevelInfos;
				Set16(pTrialLevelInfos, (WORD)len);
				pTrialLevelInfos += len;
			}
			{ //FirstAward
				BYTE* pFirstAward = pTrialLevelInfos;
				Write16(pFirstAward, 0); //len

				Write8(pFirstAward, 0); //AwardItemNum
				for (size_t i = 0; i < 0; i++)
				{ //AwardItemInfo
					BYTE* pAwardItemInfo = pFirstAward;
					Write16(pAwardItemInfo, 0); //len

					Write32(pAwardItemInfo, 0); //ItemID
					Write32(pAwardItemInfo, 0); //ItemNums
					Write32(pAwardItemInfo, 0); //AvailPeriod

					len = pAwardItemInfo - pFirstAward;
					Set16(pFirstAward, (WORD)len);
					pFirstAward += len;
				}

				len = pFirstAward - pTrialLevelInfos;
				Set16(pTrialLevelInfos, (WORD)len);
				pTrialLevelInfos += len;
			}
			{ //BaseAward
				BYTE* pBaseAward = pTrialLevelInfos;
				Write16(pBaseAward, 0); //len

				Write8(pBaseAward, 0); //AwardItemNum
				for (size_t i = 0; i < 0; i++)
				{ //BaseAwardItems
					BYTE* pBaseAwardItems = pBaseAward;
					Write16(pBaseAwardItems, 0); //len

					Write32(pBaseAwardItems, 0); //ItemID
					Write32(pBaseAwardItems, 0); //MinNum
					Write32(pBaseAwardItems, 0); //MaxNum

					len = pBaseAwardItems - pBaseAward;
					Set16(pBaseAward, (WORD)len);
					pBaseAward += len;
				}

				len = pBaseAward - pTrialLevelInfos;
				Set16(pTrialLevelInfos, (WORD)len);
				pTrialLevelInfos += len;
			}
			Write8(pTrialLevelInfos, 0); //TrialState
			Write8(pTrialLevelInfos, 0); //PerfectRoudNum
			Write8(pTrialLevelInfos, 0); //StarNum
			Write8(pTrialLevelInfos, 0); //Label
			Write8(pTrialLevelInfos, 0); //FirstAwardState
			Write8(pTrialLevelInfos, 0); //ArrayLevel

			len = pTrialLevelInfos - pPetTrialInfos;
			Set16(pPetTrialInfos, (WORD)len);
			pPetTrialInfos += len;
		}
		Write8(pPetTrialInfos, 0); //BossType
		Write8(pPetTrialInfos, TeamID); //TeamID
		Write8(pPetTrialInfos, 0); //IsOpen

		len = pPetTrialInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write16(p, 0); //LeftChallengeNum
	Write16(p, 0); //LeftSpecialChallengeNum

	len = p - buf;
	SendToClient(Client, 21150, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetPKPetCfg(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	Write32(p, Uin); //Uin
	Write16(p, 0); //ResultID
	Write16(p, 0); //ReasonLen
	{ //PKPetCfg
		BYTE* pPKPetCfg = p;
		Write16(pPKPetCfg, 0); //len

		Write8(pPKPetCfg, 0); //ExpItemNum
		for (size_t i = 0; i < 0; i++)
		{ //ExpItemInfo
			BYTE* pExpItemInfo = pPKPetCfg;
			Write16(pExpItemInfo, 0); //len

			Write32(pExpItemInfo, 0); //ExpItemID
			Write32(pExpItemInfo, 0); //PetCoinNum

			len = pExpItemInfo - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		Write16(pPKPetCfg, 0); //StarLevelNum
		for (size_t i = 0; i < 0; i++)
		{ //IncStarItemInfo
			BYTE* pIncStarItemInfo = pPKPetCfg;
			Write16(pIncStarItemInfo, 0); //len

			Write32(pIncStarItemInfo, 0); //Level
			Write32(pIncStarItemInfo, 0); //ItemID
			Write32(pIncStarItemInfo, 0); //ItemNum
			Write32(pIncStarItemInfo, 0); //PetCoinNum
			Write8(pIncStarItemInfo, 0); //Probability

			len = pIncStarItemInfo - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		Write16(pPKPetCfg, 0); //NormalSkillLevelNum
		for (size_t i = 0; i < 0; i++)
		{ //IncNormalSkillItemInfo
			BYTE* pIncNormalSkillItemInfo = pPKPetCfg;
			Write16(pIncNormalSkillItemInfo, 0); //len

			Write32(pIncNormalSkillItemInfo, 0); //Level
			Write32(pIncNormalSkillItemInfo, 0); //ItemID
			Write32(pIncNormalSkillItemInfo, 0); //ItemNum
			Write32(pIncNormalSkillItemInfo, 0); //PetCoinNum
			Write8(pIncNormalSkillItemInfo, 0); //Probability

			len = pIncNormalSkillItemInfo - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		Write16(pPKPetCfg, 0); //PowerSkillLevelNum
		for (size_t i = 0; i < 0; i++)
		{ //IncPowerSkillItemInfo
			BYTE* pIncPowerSkillItemInfo = pPKPetCfg;
			Write16(pIncPowerSkillItemInfo, 0); //len

			Write32(pIncPowerSkillItemInfo, 0); //Level
			Write32(pIncPowerSkillItemInfo, 0); //ItemID
			Write32(pIncPowerSkillItemInfo, 0); //ItemNum
			Write32(pIncPowerSkillItemInfo, 0); //PetCoinNum
			Write8(pIncPowerSkillItemInfo, 0); //Probability

			len = pIncPowerSkillItemInfo - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		Write16(pPKPetCfg, 0); //PetCfgNum
		for (size_t i = 0; i < 0; i++)
		{ //PetCfg
			BYTE* pPetCfg = pPKPetCfg;
			Write16(pPetCfg, 0); //len

			Write32(pPetCfg, 0); //PetID
			Write8(pPetCfg, 0); //ProfType
			Write8(pPetCfg, 0); //Rarity
			Write32(pPetCfg, 0); //SkillNum
			for (size_t i = 0; i < 0; i++)
			{ //Skill
				BYTE* pSkill = pPetCfg;
				Write16(pSkill, 0); //len

				Write32(pSkill, 0); //SkillID
				Write16(pSkill, 0); //TackEffectLevel
				Write16(pSkill, 0); //AttachLen

				len = pSkill - pPetCfg;
				Set16(pPetCfg, (WORD)len);
				pPetCfg += len;
			}
			Write32(pPetCfg, 0); //FightPower
			Write32(pPetCfg, 0); //Attribute[]
			Write8(pPetCfg, 0); //HasSpecialPowerSkill
			Write32(pPetCfg, 0); //MaxFightPower

			len = pPetCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		Write16(pPKPetCfg, 0); //PetSkillCfgNum
		for (size_t i = 0; i < 0; i++)
		{ //PetSkillCfg
			BYTE* pPetSkillCfg = pPKPetCfg;
			Write16(pPetSkillCfg, 0); //len

			Write32(pPetSkillCfg, 0); //SkillID
			Write8(pPetSkillCfg, 0); //Type
			Write16(pPetSkillCfg, 0); //SkillStar

			len = pPetSkillCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		{ //SkillWakeUpCfg
			BYTE* pSkillWakeUpCfg = pPKPetCfg;
			Write16(pSkillWakeUpCfg, 0); //len

			Write32(pSkillWakeUpCfg, 0); //ItemID
			Write32(pSkillWakeUpCfg, 0); //ItemNum
			Write16(pSkillWakeUpCfg, 0); //FightLevel

			len = pSkillWakeUpCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		for (size_t i = 0; i < 0; i++)
		{ //AttrPotentailCfg
			BYTE* pAttrPotentailCfg = pPKPetCfg;
			Write16(pAttrPotentailCfg, 0); //len

			Write8(pAttrPotentailCfg, 0); //ProfType
			for (size_t i = 0; i < 0; i++)
			{ //BaseAttrPotential
				BYTE* pBaseAttrPotential = pAttrPotentailCfg;
				Write16(pBaseAttrPotential, 0); //len

				Write8(pBaseAttrPotential, 0); //Rarity
				Write16(pBaseAttrPotential, 0); //PotentialAttr[]

				len = pBaseAttrPotential - pAttrPotentailCfg;
				Set16(pAttrPotentailCfg, (WORD)len);
				pAttrPotentailCfg += len;
			}

			len = pAttrPotentailCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		for (size_t i = 0; i < 0; i++)
		{ //StarPotentailCfg
			BYTE* pStarPotentailCfg = pPKPetCfg;
			Write16(pStarPotentailCfg, 0); //len

			Write8(pStarPotentailCfg, 0); //Rarity
			Write16(pStarPotentailCfg, 0); //StarAttr[]

			len = pStarPotentailCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		for (size_t i = 0; i < 0; i++)
		{ //AttrCfg
			BYTE* pAttrCfg = pPKPetCfg;
			Write16(pAttrCfg, 0); //len

			Write8(pAttrCfg, 0); //ProfType
			for (size_t i = 0; i < 0; i++)
			{ //BaseAttr
				BYTE* pBaseAttr = pAttrCfg;
				Write16(pBaseAttr, 0); //len

				Write8(pBaseAttr, 0); //Rarity
				Write16(pBaseAttr, 0); //BaseAttr[]

				len = pBaseAttr - pAttrCfg;
				Set16(pAttrCfg, (WORD)len);
				pAttrCfg += len;
			}

			len = pAttrCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		{ //EmbattleCfg
			BYTE* pEmbattleCfg = pPKPetCfg;
			Write16(pEmbattleCfg, 0); //len

			Write32(pEmbattleCfg, 0); //HealthAddition[]
			Write32(pEmbattleCfg, 0); //FrontRowDefenseAddition[]
			Write32(pEmbattleCfg, 0); //BackRowAttackAddition[]
			Write32(pEmbattleCfg, 0); //DamageTreatAddition[]
			Write32(pEmbattleCfg, 0); //EmbattlePetNum

			len = pEmbattleCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		Write16(pPKPetCfg, 0); //AttrMotivateLevelNum
		for (size_t i = 0; i < 0; i++)
		{ //AttrMotivateCfg
			BYTE* pAttrMotivateCfg = pPKPetCfg;
			Write16(pAttrMotivateCfg, 0); //len

			Write32(pAttrMotivateCfg, 0); //Level
			Write32(pAttrMotivateCfg, 0); //ItemID
			Write32(pAttrMotivateCfg, 0); //ItemNum
			Write32(pAttrMotivateCfg, 0); //PetCoinNum
			Write32(pAttrMotivateCfg, 0); //AttrInc

			len = pAttrMotivateCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		Write16(pPKPetCfg, 0); //StarRefreshLevelNum
		for (size_t i = 0; i < 0; i++)
		{ //StarRefreshCfg
			BYTE* pStarRefreshCfg = pPKPetCfg;
			Write16(pStarRefreshCfg, 0); //len

			Write32(pStarRefreshCfg, 0); //Level
			Write32(pStarRefreshCfg, 0); //ItemID
			Write32(pStarRefreshCfg, 0); //ItemNum
			Write32(pStarRefreshCfg, 0); //PetCoinNum
			Write8(pStarRefreshCfg, 0); //Probability

			len = pStarRefreshCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}
		{ //BattleArrayCfg
			BYTE* pBattleArrayCfg = pPKPetCfg;
			Write16(pBattleArrayCfg, 0); //len

			Write8(pBattleArrayCfg, 0); //InitArrayID
			Write8(pBattleArrayCfg, 0); //ArrayCfgNum
			for (size_t i = 0; i < 0; i++)
			{ //PetPkArrayCfg
				BYTE* pPetPkArrayCfg = pBattleArrayCfg;
				Write16(pPetPkArrayCfg, 0); //len

				Write8(pPetPkArrayCfg, 0); //ArrayID
				Write8(pPetPkArrayCfg, 0); //NameLen
				Write16(pPetPkArrayCfg, 0); //DescLen
				Write8(pPetPkArrayCfg, 0); //LevelCfgNum
				for (size_t i = 0; i < 0; i++)
				{ //ArrayLevelCfg
					BYTE* pArrayLevelCfg = pPetPkArrayCfg;
					Write16(pArrayLevelCfg, 0); //len

					Write8(pArrayLevelCfg, 0); //Level
					Write8(pArrayLevelCfg, 0); //CostItemCfgNum
					for (size_t i = 0; i < 0; i++)
					{ //CostItem
						BYTE* pCostItem = pArrayLevelCfg;
						Write16(pCostItem, 0); //len

						Write32(pCostItem, 0); //ItemID
						Write32(pCostItem, 0); //ConsumeNum

						len = pCostItem - pArrayLevelCfg;
						Set16(pArrayLevelCfg, (WORD)len);
						pArrayLevelCfg += len;
					}
					Write8(pArrayLevelCfg, 0); //ArraySeatNum
					for (size_t i = 0; i < 0; i++)
					{ //ArraySeatCfg
						BYTE* pArraySeatCfg = pArrayLevelCfg;
						Write16(pArraySeatCfg, 0); //len

						Write8(pArraySeatCfg, 0); //SeatID
						Write8(pArraySeatCfg, 0); //SeatEffectNum
						for (size_t i = 0; i < 0; i++)
						{ //SeatEffect
							BYTE* pSeatEffect = pArraySeatCfg;
							Write16(pSeatEffect, 0); //len

							Write8(pSeatEffect, 0); //ArraySeatEffectType
							Write8(pSeatEffect, 0); //ParaNum
							Write32(pSeatEffect, 0); //Para[]
							Write32(pSeatEffect, 0); //PetAttrEffectFactor[]
							Write32(pSeatEffect, 0); //MaxPetAttrEffectFactor

							len = pSeatEffect - pArraySeatCfg;
							Set16(pArraySeatCfg, (WORD)len);
							pArraySeatCfg += len;
						}
						Write8(pArraySeatCfg, 0); //RecomPetType

						len = pArraySeatCfg - pArrayLevelCfg;
						Set16(pArrayLevelCfg, (WORD)len);
						pArrayLevelCfg += len;
					}

					len = pArrayLevelCfg - pPetPkArrayCfg;
					Set16(pPetPkArrayCfg, (WORD)len);
					pPetPkArrayCfg += len;
				}

				len = pPetPkArrayCfg - pBattleArrayCfg;
				Set16(pBattleArrayCfg, (WORD)len);
				pBattleArrayCfg += len;
			}
			Write32(pBattleArrayCfg, 0); //BeginDate
			Write32(pBattleArrayCfg, 0); //EndDate
			Write32(pBattleArrayCfg, 0); //OpenTime
			Write32(pBattleArrayCfg, 0); //CloseTime

			len = pBattleArrayCfg - pPKPetCfg;
			Set16(pPKPetCfg, (WORD)len);
			pPKPetCfg += len;
		}

		len = pPKPetCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //HasMoreInfo

	len = p - buf;
	SendToClient(Client, 21140, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetPetInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT PetNum = Read16(p);
	int PetIDList[200];
	for (size_t i = 0; i < PetNum;i++) {
		PetIDList[i] = Read32(p);
	} 
	

}

void ResponseGetPetInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Uin
	Write16(p, 0); //PetNum
	for (size_t i = 0; i < 0; i++)
	{ //PetInfo
		BYTE* pPetInfo = p;
		Write16(pPetInfo, 0); //len

		Write32(pPetInfo, 0); //Uin
		Write32(pPetInfo, 0); //PetId
		WriteString(pPetInfo, 0); //PetName[]
		Write8(pPetInfo, 0); //IsProper
		Write16(pPetInfo, 0); //LevelLimit
		Write16(pPetInfo, 0); //Level
		Write32(pPetInfo, 0); //CurLevelExp
		Write32(pPetInfo, 0); //CurLevelMaxExp
		Write8(pPetInfo, 0); //Status
		Write16(pPetInfo, 0); //PL
		Write16(pPetInfo, 0); //TotalPL
		Write16(pPetInfo, 0); //PetSkillNum
		for (size_t i = 0; i < 0; i++)
		{ //PetSkillList
			BYTE* pPetSkillList = pPetInfo;
			Write16(pPetSkillList, 0); //len

			Write16(pPetSkillList, 0); //SkillID
			Write8(pPetSkillList, 0); //Status
			Write8(pPetSkillList, 0); //Active
			Write32(pPetSkillList, 0); //Value

			len = pPetSkillList - pPetInfo;
			Set16(pPetInfo, (WORD)len);
			pPetInfo += len;
		}
		Write32(pPetInfo, 0); //StrengLevel
		Write8(pPetInfo, 0); //TopStatus
		{ //CommonInfo
			BYTE* pCommonInfo = pPetInfo;
			Write16(pCommonInfo, 0); //len

			Write32(pCommonInfo, 0); //HuanHuaLevel
			Write8(pCommonInfo, 0); //CanHuanHua

			len = pCommonInfo - pPetInfo;
			Set16(pPetInfo, (WORD)len);
			pPetInfo += len;
		}

		len = pPetInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24528, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetPetShowInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	int PetID = Read32(p);
	short GetAllPetShowInfo = Read16(p);
	ResponseGetPetShowInfo(Client, PetID);
}

void ResponseGetPetShowInfo(ClientNode* Client, UINT PETID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	if (PETID == 0) {
		Write16(p, 1); //PetNums
		for (size_t i = 0; i < 1; i++)
		{ //PetShowInfos
			BYTE* pPetShowInfos = p;
			Write16(pPetShowInfos, 0); //len

			{ //BaseInfo
				BYTE* pBaseInfo = pPetShowInfos;
				Write16(pBaseInfo, 0); //len

				Write32(pBaseInfo, 75772); //PetID
				Write32(pBaseInfo, 3); //FullLevel
				Write32(pBaseInfo, 10); //StrengthFullLevel
				Write16(pBaseInfo, 1); //CanRide
				Write16(pBaseInfo, 2); //RidePetType
				Write16(pBaseInfo, 3); //BaseInfoNums
				for (size_t i = 0; i < 3; i++)
				{ //ShowBaseInfos
					BYTE* pShowBaseInfos = pBaseInfo;
					Write16(pShowBaseInfos, 0); //len

					Write32(pShowBaseInfos, i+1); //SkillID
					Write32(pShowBaseInfos, 10); //SKillFullLevelValue
					Write32(pShowBaseInfos, 10); //StrengthFullLevelValue

					len = pShowBaseInfos - pBaseInfo;
					Set16(pBaseInfo, (WORD)len);
					pBaseInfo += len;
				}

				len = pBaseInfo - pPetShowInfos;
				Set16(pPetShowInfos, (WORD)len);
				pPetShowInfos += len;
			}
			{ //OpenNewSkillInfo
				BYTE* pOpenNewSkillInfo = pPetShowInfos;
				Write16(pOpenNewSkillInfo, 0); //len

				Write32(pOpenNewSkillInfo, 75772); //PetID
				Write16(pOpenNewSkillInfo, 2); //OpenSkillNums
				for (size_t i = 0; i < 2; i++)
				{ //OpenSKillInfos
					BYTE* pOpenSKillInfos = pOpenNewSkillInfo;
					Write16(pOpenSKillInfos, 0); //len

					Write32(pOpenSKillInfos, 5); //OpenLevel
					Write16(pOpenSKillInfos, 15); //SkillID
					Write32(pOpenSKillInfos, 15); //AddValue

					len = pOpenSKillInfos - pOpenNewSkillInfo;
					Set16(pOpenNewSkillInfo, (WORD)len);
					pOpenNewSkillInfo += len;
				}

				len = pOpenNewSkillInfo - pPetShowInfos;
				Set16(pPetShowInfos, (WORD)len);
				pPetShowInfos += len;
			}

			len = pPetShowInfos - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 0); //HasMoreInfo
	}
	else {
		Write16(p, 1); //PetNums
		for (size_t i = 0; i < 1; i++)
		{ //PetShowInfos
			BYTE* pPetShowInfos = p;
			Write16(pPetShowInfos, 0); //len

			{ //BaseInfo
				BYTE* pBaseInfo = pPetShowInfos;
				Write16(pBaseInfo, 0); //len

				Write32(pBaseInfo, 75772); //PetID
				Write32(pBaseInfo, 3); //FullLevel
				Write32(pBaseInfo, 10); //StrengthFullLevel
				Write16(pBaseInfo, 1); //CanRide
				Write16(pBaseInfo, 2); //RidePetType
				Write16(pBaseInfo, 3); //BaseInfoNums
				for (size_t i = 0; i < 3; i++)
				{ //ShowBaseInfos
					BYTE* pShowBaseInfos = pBaseInfo;
					Write16(pShowBaseInfos, 0); //len

					Write32(pShowBaseInfos, i + 1); //SkillID
					Write32(pShowBaseInfos, 10); //SKillFullLevelValue
					Write32(pShowBaseInfos, 10); //StrengthFullLevelValue

					len = pShowBaseInfos - pBaseInfo;
					Set16(pBaseInfo, (WORD)len);
					pBaseInfo += len;
				}

				len = pBaseInfo - pPetShowInfos;
				Set16(pPetShowInfos, (WORD)len);
				pPetShowInfos += len;
			}
			{ //OpenNewSkillInfo
				BYTE* pOpenNewSkillInfo = pPetShowInfos;
				Write16(pOpenNewSkillInfo, 0); //len

				Write32(pOpenNewSkillInfo, 75772); //PetID
				Write16(pOpenNewSkillInfo, 2); //OpenSkillNums
				for (size_t i = 0; i < 2; i++)
				{ //OpenSKillInfos
					BYTE* pOpenSKillInfos = pOpenNewSkillInfo;
					Write16(pOpenSKillInfos, 0); //len

					Write32(pOpenSKillInfos, 5); //OpenLevel
					Write16(pOpenSKillInfos, 15); //SkillID
					Write32(pOpenSKillInfos, 15); //AddValue

					len = pOpenSKillInfos - pOpenNewSkillInfo;
					Set16(pOpenNewSkillInfo, (WORD)len);
					pOpenNewSkillInfo += len;
				}

				len = pOpenNewSkillInfo - pPetShowInfos;
				Set16(pPetShowInfos, (WORD)len);
				pPetShowInfos += len;
			}

			len = pPetShowInfos - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 0); //HasMoreInfo
	}
	

	len = p - buf;
	SendToClient(Client, 24426, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void ResponseGetPetShowMoreInfo(ClientNode* Client, UINT PETID)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, 0); //Uin
	Write16(p, 0); //PetNums
	for (size_t i = 0; i < 0; i++)
	{ //PetShowInfos
		BYTE* pPetShowInfos = p;
		Write16(pPetShowInfos, 0); //len

		{ //BaseInfo
			BYTE* pBaseInfo = pPetShowInfos;
			Write16(pBaseInfo, 0); //len

			Write32(pBaseInfo, 0); //PetID
			Write32(pBaseInfo, 0); //FullLevel
			Write32(pBaseInfo, 0); //StrengthFullLevel
			Write16(pBaseInfo, 0); //CanRide
			Write16(pBaseInfo, 0); //RidePetType
			Write16(pBaseInfo, 0); //BaseInfoNums
			for (size_t i = 0; i < 0; i++)
			{ //ShowBaseInfos
				BYTE* pShowBaseInfos = pBaseInfo;
				Write16(pShowBaseInfos, 0); //len

				Write32(pShowBaseInfos, 0); //SkillID
				Write32(pShowBaseInfos, 0); //SKillFullLevelValue
				Write32(pShowBaseInfos, 0); //StrengthFullLevelValue

				len = pShowBaseInfos - pBaseInfo;
				Set16(pBaseInfo, (WORD)len);
				pBaseInfo += len;
			}

			len = pBaseInfo - pPetShowInfos;
			Set16(pPetShowInfos, (WORD)len);
			pPetShowInfos += len;
		}
		{ //OpenNewSkillInfo
			BYTE* pOpenNewSkillInfo = pPetShowInfos;
			Write16(pOpenNewSkillInfo, 0); //len

			Write32(pOpenNewSkillInfo, 0); //PetID
			Write16(pOpenNewSkillInfo, 0); //OpenSkillNums
			for (size_t i = 0; i < 0; i++)
			{ //OpenSKillInfos
				BYTE* pOpenSKillInfos = pOpenNewSkillInfo;
				Write16(pOpenSKillInfos, 0); //len

				Write32(pOpenSKillInfos, 0); //OpenLevel
				Write16(pOpenSKillInfos, 0); //SkillID
				Write32(pOpenSKillInfos, 0); //AddValue

				len = pOpenSKillInfos - pOpenNewSkillInfo;
				Set16(pOpenNewSkillInfo, (WORD)len);
				pOpenNewSkillInfo += len;
			}

			len = pOpenNewSkillInfo - pPetShowInfos;
			Set16(pPetShowInfos, (WORD)len);
			pPetShowInfos += len;
		}

		len = pPetShowInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //HasNext

	len = p - buf;
	SendToClient(Client, 24527, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestGetPetSkillList(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	ResponseGetPetSkillList(Client);
}

void ResponseGetPetSkillList(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write8(p, 1); //PetNum
	for (size_t i = 0; i < 1; i++)
	{ //PetSkills
		BYTE* pPetSkills = p;
		Write16(pPetSkills, 0); //len

		Write32(pPetSkills, 104739); //PetID
		Write8(pPetSkills, 1); //SkillNum
		Write16(pPetSkills, 0); //PetSkillIDs[]
		len = pPetSkills - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 440, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void NotifyClientLoveBabyInfoWhenLogIn(ClientNode* Client)
{
	if (Client->Engage->UIN==0) {
		return;
	}
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 1); //HaveLoveBaby
	Write16(p, 1); //BeNoticed
	{ //LoveBabyInfo
		BYTE* pLoveBabyInfo = p;
		Write16(pLoveBabyInfo, 0); //len

		Write32(pLoveBabyInfo, Client->Engage->BabyOneID); //LoveBabyID
		Write32(pLoveBabyInfo, Client->Uin); //MasterLoveUin
		Write32(pLoveBabyInfo, Client->Engage->UIN); //ViceLoveUin
		Write32(pLoveBabyInfo, 100); //GrowValue
		Write32(pLoveBabyInfo, 100); //MentalValue
		Write32(pLoveBabyInfo, 100); //BabyGrowLevel
		Write32(pLoveBabyInfo, 5); //BabyProfileLevel
		Write32(pLoveBabyInfo, 0); //BabyStrengthLevel
		Write32(pLoveBabyInfo, 0); //BabyPLValue
		Write32(pLoveBabyInfo, 0); //BabyOrgPLValue
		WriteString(pLoveBabyInfo, Client->Engage->BabyName1,16); //LoveBabyNickName[]
		Write16(pLoveBabyInfo, 0); //BabyStat
		Write16(pLoveBabyInfo, 0); //RemainingActiveDay
		Write16(pLoveBabyInfo, Client->Engage->BackGroundID); //BackGroundID
		Write16(pLoveBabyInfo, Client->Engage->BabyOneGender); //LoverBabyGender
		Write16(pLoveBabyInfo, 0); //MentalTotalValue
		Write32(pLoveBabyInfo, 0); //NextGrowLevelGrowValue
		Write32(pLoveBabyInfo, Client->Engage->BabyOneID); //LoverBabyItemID
		Write16(pLoveBabyInfo, Client->Engage->BabyOneEquipStat); //EquipStat
		Write32(pLoveBabyInfo, 0); //BabySkillNums
		for (size_t i = 0; i < 0; i++)
		{ //PetSkillList
			BYTE* pPetSkillList = pLoveBabyInfo;
			Write16(pPetSkillList, 0); //len

			Write16(pPetSkillList, 1); //SkillID
			Write8(pPetSkillList, 1); //Status
			Write8(pPetSkillList, 1); //Active
			Write32(pPetSkillList, 1); //Value

			len = pPetSkillList - pLoveBabyInfo;
			Set16(pLoveBabyInfo, (WORD)len);
			pLoveBabyInfo += len;
		}
		Write16(pLoveBabyInfo, 0); //TreatMentalValue
		Write32(pLoveBabyInfo, 0); //DailyLikeNum
		Write32(pLoveBabyInfo, 0); //TotalLikeNum
		Write32(pLoveBabyInfo, 0); //DailyLearningNum
		Write32(pLoveBabyInfo, 0); //DaliyLearningLimtNum
		Write32(pLoveBabyInfo, 0); //AlreadyLikeNums
		Write32(pLoveBabyInfo, 0); //RemainingLikeNums
		Write16(pLoveBabyInfo, 5); //BabyTransferdProfileLevel
		Write16(pLoveBabyInfo, 0); //HaveBirthPresent

		len = pLoveBabyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //SecondLoveBabyInfo
		BYTE* pSecondLoveBabyInfo = p;
		Write16(pSecondLoveBabyInfo, 0); //len

		Write32(pSecondLoveBabyInfo, Client->Engage->BabyTwoID); //LoveBabyID
		Write32(pSecondLoveBabyInfo, Client->Uin); //MasterLoveUin
		Write32(pSecondLoveBabyInfo, Client->Engage->UIN); //ViceLoveUin
		Write32(pSecondLoveBabyInfo, 100); //GrowValue
		Write32(pSecondLoveBabyInfo, 100); //MentalValue
		Write32(pSecondLoveBabyInfo, 100); //BabyGrowLevel
		Write32(pSecondLoveBabyInfo, 5); //BabyProfileLevel
		Write32(pSecondLoveBabyInfo, 0); //BabyStrengthLevel
		Write32(pSecondLoveBabyInfo, 0); //BabyPLValue
		Write32(pSecondLoveBabyInfo, 0); //BabyOrgPLValue
		WriteString(pSecondLoveBabyInfo, Client->Engage->BabyName2,16); //LoveBabyNickName[]
		Write16(pSecondLoveBabyInfo, 0); //BabyStat
		Write16(pSecondLoveBabyInfo, 0); //RemainingActiveDay
		Write16(pSecondLoveBabyInfo, Client->Engage->BackGroundID); //BackGroundID
		Write16(pSecondLoveBabyInfo, Client->Engage->BabyTwoGender); //LoverBabyGender
		Write16(pSecondLoveBabyInfo, 0); //MentalTotalValue
		Write32(pSecondLoveBabyInfo, 0); //NextGrowLevelGrowValue
		Write32(pSecondLoveBabyInfo, Client->Engage->BabyTwoID); //LoverBabyItemID
		Write16(pSecondLoveBabyInfo, Client->Engage->BabyTwoEquipStat); //EquipStat
		Write32(pSecondLoveBabyInfo, 0); //BabySkillNums
		for (size_t i = 0; i < 0; i++)
		{ //PetSkillList
			BYTE* pPetSkillList = pSecondLoveBabyInfo;
			Write16(pPetSkillList, 0); //len

			Write16(pPetSkillList, 0); //SkillID
			Write8(pPetSkillList, 0); //Status
			Write8(pPetSkillList, 0); //Active
			Write32(pPetSkillList, 0); //Value

			len = pPetSkillList - pSecondLoveBabyInfo;
			Set16(pSecondLoveBabyInfo, (WORD)len);
			pSecondLoveBabyInfo += len;
		}
		Write16(pSecondLoveBabyInfo, 0); //TreatMentalValue
		Write32(pSecondLoveBabyInfo, 0); //DailyLikeNum
		Write32(pSecondLoveBabyInfo, 0); //TotalLikeNum
		Write32(pSecondLoveBabyInfo, 0); //DailyLearningNum
		Write32(pSecondLoveBabyInfo, 0); //DaliyLearningLimtNum
		Write32(pSecondLoveBabyInfo, 0); //AlreadyLikeNums
		Write32(pSecondLoveBabyInfo, 0); //RemainingLikeNums
		Write16(pSecondLoveBabyInfo, 5); //BabyTransferdProfileLevel
		Write16(pSecondLoveBabyInfo, 0); //HaveBirthPresent

		len = pSecondLoveBabyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //LoverBabyCfgInfo
		BYTE* pLoverBabyCfgInfo = p;
		Write16(pLoverBabyCfgInfo, 0); //len

		Write32(pLoverBabyCfgInfo, 0); //LikeAddMentalValue
		Write32(pLoverBabyCfgInfo, 0); //DailyLikeAddMentalValue
		Write32(pLoverBabyCfgInfo, 0); //DailyBeLikedNumsToGetCoupon
		Write32(pLoverBabyCfgInfo, 0); //AwardCouponNums
		Write32(pLoverBabyCfgInfo, 0); //LearningAddGrowValue
		Write32(pLoverBabyCfgInfo, 0); //LearningTime

		len = pLoverBabyCfgInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //SecondLoverBabyCfgInfo
		BYTE* pSecondLoverBabyCfgInfo = p;
		Write16(pSecondLoverBabyCfgInfo, 0); //len

		Write32(pSecondLoverBabyCfgInfo, 0); //LikeAddMentalValue
		Write32(pSecondLoverBabyCfgInfo, 0); //DailyLikeAddMentalValue
		Write32(pSecondLoverBabyCfgInfo, 0); //DailyBeLikedNumsToGetCoupon
		Write32(pSecondLoverBabyCfgInfo, 0); //AwardCouponNums
		Write32(pSecondLoverBabyCfgInfo, 0); //LearningAddGrowValue
		Write32(pSecondLoverBabyCfgInfo, 0); //LearningTime

		len = pSecondLoverBabyCfgInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); //HaveRaisedSecondBaby
	Write8(p, 0); //EquipBabyStatus
	Write16(p, 0); //RemainingDanceNums
	Write16(p, 0); //RemainingMaimengNums
	Write16(p, 0); //MaxDanceNums
	Write16(p, 0); //MaxMaimengNums

	len = p - buf;
	SendToClient(Client, 24370, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestGetLoveBabyInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body;
	WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	UINT Flag = Read32(p);
	ResponseGetLoveBabyInfo(Client);
}

void ResponseGetLoveBabyInfo(ClientNode* Client)
{
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write32(p, 0);			 // DailyFeedLimitNums
	Write32(p, 0);			 // DailyShowerLimitNums
	Write32(p, 0);			 // DailyFunLimitNums
	Write32(p, 0);			 // DailyFeedNums
	Write32(p, 0);			 // DailyShowerNums
	Write32(p, 0);			 // DailyFunNums
	Write32(p, 0);			 // ShowerAddMentalValue
	Write32(p, 0);			 // FunAddMentalValue
	Write32(p, 0);			 // FeedAddGrowValue
	Write32(p, 0);			 // DailyLikeBabyNum
	Write32(p, 0);			 // RemainingLearningTime
	{						 // LoveBabyInfo
		BYTE* pLoveBabyInfo = p;
		Write16(pLoveBabyInfo, 0); // len

		Write32(pLoveBabyInfo, Client->Engage->BabyOneID);		 // LoveBabyID
		Write32(pLoveBabyInfo, Client->Uin); // MasterLoveUin
		Write32(pLoveBabyInfo, Client->Engage->UIN); // ViceLoveUin
		Write32(pLoveBabyInfo, 100);		 // GrowValue
		Write32(pLoveBabyInfo, 100);		 // MentalValue
		Write32(pLoveBabyInfo, 100);			 // BabyGrowLevel
		Write32(pLoveBabyInfo, 5);			 // BabyProfileLevel
		Write32(pLoveBabyInfo,0);			 // BabyStrengthLevel
		Write32(pLoveBabyInfo, 0);			 // BabyPLValue
		Write32(pLoveBabyInfo, 0);			 // BabyOrgPLValue
		WriteString(pLoveBabyInfo, Client->Engage->BabyName1,16);	 // LoveBabyNickName[]
		Write16(pLoveBabyInfo, 0);	   // BabyStat
		Write16(pLoveBabyInfo, 0);	   // RemainingActiveDay
		Write16(pLoveBabyInfo, Client->Engage->BackGroundID);	   // BackGroundID
		Write16(pLoveBabyInfo, Client->Engage->BabyOneGender);	   // LoverBabyGender
		Write16(pLoveBabyInfo, 100);	   // MentalTotalValue
		Write32(pLoveBabyInfo, 100);	   // NextGrowLevelGrowValue
		Write32(pLoveBabyInfo, Client->Engage->BabyOneID); // LoverBabyItemID
		Write16(pLoveBabyInfo, Client->Engage->BabyOneEquipStat);	   // EquipStat
		Write32(pLoveBabyInfo, 0);	   // BabySkillNums
		for (size_t i = 0; i < 0; i++)
		{ // PetSkillList
			BYTE* pPetSkillList = pLoveBabyInfo;
			Write16(pPetSkillList, 0); // len

			Write16(pPetSkillList, 0); // SkillID
			Write8(pPetSkillList, 0);  // Status
			Write8(pPetSkillList, 0);  // Active
			Write32(pPetSkillList, 0); // Value

			len = pPetSkillList - pLoveBabyInfo;
			Set16(pLoveBabyInfo, (WORD)len);
			pLoveBabyInfo += len;
		}
		Write16(pLoveBabyInfo, 0); // TreatMentalValue
		Write32(pLoveBabyInfo, 0); // DailyLikeNum
		Write32(pLoveBabyInfo, 0); // TotalLikeNum
		Write32(pLoveBabyInfo, 0); // DailyLearningNum
		Write32(pLoveBabyInfo, 0); // DaliyLearningLimtNum
		Write32(pLoveBabyInfo, 0); // AlreadyLikeNums
		Write32(pLoveBabyInfo, 0); // RemainingLikeNums
		Write16(pLoveBabyInfo, 5); // BabyTransferdProfileLevel
		Write16(pLoveBabyInfo, 0); // HaveBirthPresent

		len = pLoveBabyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // SecondBabyInfo
		BYTE* pSecondBabyInfo = p;
		Write16(pSecondBabyInfo, 0); // len

		Write32(pSecondBabyInfo, 0); // DailyFeedLimitNums
		Write32(pSecondBabyInfo, 0); // DailyShowerLimitNums
		Write32(pSecondBabyInfo, 0); // DailyFunLimitNums
		Write32(pSecondBabyInfo, 0); // DailyFeedNums
		Write32(pSecondBabyInfo, 0); // DailyShowerNums
		Write32(pSecondBabyInfo, 0); // DailyFunNums
		Write32(pSecondBabyInfo, 0); // ShowerAddMentalValue
		Write32(pSecondBabyInfo, 0); // FunAddMentalValue
		Write32(pSecondBabyInfo, 0); // FeedAddGrowValue
		Write32(pSecondBabyInfo, 0); // DailyLikeBabyNum
		Write32(pSecondBabyInfo, 0); // RemainingLearningTime
		{							 // LoveBabyInfo
			BYTE* pLoveBabyInfo = pSecondBabyInfo;
			Write16(pLoveBabyInfo, 0); // len

			Write32(pLoveBabyInfo, Client->Engage->BabyTwoID); // LoveBabyID
			Write32(pLoveBabyInfo, Client->Uin); // MasterLoveUin
			Write32(pLoveBabyInfo, Client->Engage->UIN); // ViceLoveUin
			Write32(pLoveBabyInfo, 5);		 // GrowValue
			Write32(pLoveBabyInfo, 5);		 // MentalValue
			Write32(pLoveBabyInfo, 100);			 // BabyGrowLevel
			Write32(pLoveBabyInfo, 5);			 // BabyProfileLevel
			Write32(pLoveBabyInfo, 5);			 // BabyStrengthLevel
			Write32(pLoveBabyInfo, 0); // BabyPLValue
			Write32(pLoveBabyInfo, 0); // BabyOrgPLValue
			WriteString(pLoveBabyInfo, Client->Engage->BabyName2, 16);	 // LoveBabyNickName[]
			Write16(pLoveBabyInfo, 0); // BabyStat
			Write16(pLoveBabyInfo, 0); // RemainingActiveDay
			Write16(pLoveBabyInfo, Client->Engage->BackGroundID); // BackGroundID
			Write16(pLoveBabyInfo, Client->Engage->BabyTwoGender); // LoverBabyGender
			Write16(pLoveBabyInfo, 0); // MentalTotalValue
			Write32(pLoveBabyInfo, 0); // NextGrowLevelGrowValue
			Write32(pLoveBabyInfo, Client->Engage->BabyTwoID); // LoverBabyItemID
			Write16(pLoveBabyInfo, Client->Engage->BabyTwoEquipStat); // EquipStat
			Write32(pLoveBabyInfo, 0); // BabySkillNums
			for (size_t i = 0; i < 0; i++)
			{ // PetSkillList
				BYTE* pPetSkillList = pLoveBabyInfo;
				Write16(pPetSkillList, 0); // len

				Write16(pPetSkillList, 0); // SkillID
				Write8(pPetSkillList, 0);  // Status
				Write8(pPetSkillList, 0);  // Active
				Write32(pPetSkillList, 0); // Value

				len = pPetSkillList - pLoveBabyInfo;
				Set16(pLoveBabyInfo, (WORD)len);
				pLoveBabyInfo += len;
			}
			Write16(pLoveBabyInfo, 0); // TreatMentalValue
			Write32(pLoveBabyInfo, 0); // DailyLikeNum
			Write32(pLoveBabyInfo, 0); // TotalLikeNum
			Write32(pLoveBabyInfo, 0); // DailyLearningNum
			Write32(pLoveBabyInfo, 0); // DaliyLearningLimtNum
			Write32(pLoveBabyInfo, 0); // AlreadyLikeNums
			Write32(pLoveBabyInfo, 0); // RemainingLikeNums
			Write16(pLoveBabyInfo, 5); // BabyTransferdProfileLevel
			Write16(pLoveBabyInfo, 0); // HaveBirthPresent

			len = pLoveBabyInfo - pSecondBabyInfo;
			Set16(pSecondBabyInfo, (WORD)len);
			pSecondBabyInfo += len;
		}

		len = pSecondBabyInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, Client->Engage->BabyOneEquipStat+ Client->Engage->BabyTwoEquipStat);  // EquipBaby
	Write16(p, 0); // ActivedExtralSkillNums
	for (size_t i = 0; i < 0; i++)
	{ // BabyExtralSkillInfos
		BYTE* pBabyExtralSkillInfos = p;
		Write16(pBabyExtralSkillInfos, 0); // len

		Write8(pBabyExtralSkillInfos, i);  // BabySeq
		Write16(pBabyExtralSkillInfos, 1); // SkillID

		len = pBabyExtralSkillInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 24352, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestChangeLoverBabyEquipStatus(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short EquipStatus = Read16(p);//1号上 0 一号下 2 二号上 
	ResponseChangeLoverBabyEquipStatus(Client, EquipStatus);
}

void ResponseChangeLoverBabyEquipStatus(ClientNode* Client, short EquipStatus)
{
	if (EquipStatus == 0) {
		if (Client->Engage->BabyOneEquipStat == 1) {
			Client->Engage->BabyOneEquipStat = 0;
		}
		else if (Client->Engage->BabyTwoEquipStat == 1) {
			Client->Engage->BabyTwoEquipStat = 0;
		}
	}
	else if (EquipStatus == 1) {
		Client->Engage->BabyOneEquipStat = 1;
		Client->Engage->BabyTwoEquipStat = 0;
	}
	else if (EquipStatus == 2) {
		Client->Engage->BabyOneEquipStat = 0;
		Client->Engage->BabyTwoEquipStat = 1;
	}
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResutID

	Write16(p, EquipStatus); //EquipStat



	len = p - buf;
	SendToClient(Client, 24386, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestChangeCarryWizard(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT Num = Read16(p);
	for (size_t i = 0; i < Num; i++)
	{ //WizardCarry
		BYTE* pWizardCarry = p;
		len = Read16(pWizardCarry);

		USHORT WizardID = Read16(pWizardCarry);
		bool Carry = Read8(pWizardCarry);
		ResponseChangeCarryWizard(Client, WizardID, Carry);
		p += len;
	}

}

void ResponseChangeCarryWizard(ClientNode* Client, USHORT WizardID, bool Carry)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write16(p, 0); //ResultID
	Write16(p, 1); //Num
	for (size_t i = 0; i < 1; i++)
	{ //WizardCarry
		BYTE* pWizardCarry = p;
		Write16(pWizardCarry, 0); //len

		Write16(pWizardCarry, WizardID); //WizardID
		Write8(pWizardCarry, Carry); //Carry

		len = pWizardCarry - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 11506, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}