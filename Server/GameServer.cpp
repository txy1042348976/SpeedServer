#include "Message.h"

#include <stdio.h>
#include <iostream>
#include <time.h>
#include <map>
#include "yaml-cpp/yaml.h"
#include <random>
#include <filesystem>
#include "LibIconv/iconv.h"
#include "Database.h"
#include "GameServer.h"
#include "Item.h"
#include "sqlite/sqlite3.h"
#include <algorithm>
#pragma comment(lib, "DllIconv/libiconv.lib")
struct DstInfo
{
	USHORT PlayerID;
	UINT Uin;
};
DstInfo aDstInfo[6];
extern sqlite3 *PlayerDB;
std::map<UINT, KartItem *> KartItems;
extern std::map<long, RoomNode*> Rooms;
const char *UITitleDispInfo_ADText;
extern std::vector<KartAltas*> KartAltasList; // 车库列表
YAML::Node KartSkillConfig = YAML::LoadFile(".\\Config\\KartSkill.yml");
YAML::Node ServerConfig = YAML::LoadFile(".\\Config\\config.yml");
BYTE BorderOver[187];
BYTE RankedOver[402];
BYTE taskaward[128] = { 0 };
BYTE jump24238[91];
BYTE jump24255[91];
void RequestGetAllWakeKartInfo(ClientNode* Client, BYTE* Body, size_t BodyLen)
{
	BYTE* p = Body; WORD len;

	UINT Uin = Read32(p);
	ResponseGetAllWakeKartInfo(Client);
}

void ResponseGetAllWakeKartInfo(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 8); //WakeKartInfoNums
	const UINT RefitItemCfg[] = { 116739 ,127524 ,127616 ,19618,18765,124685,109350, 24146 };
	for (size_t i = 0; i < 8; i++)
	{ //WakeKartInfos
		BYTE* pWakeKartInfos = p;
		Write16(pWakeKartInfos, 0); //len

		Write32(pWakeKartInfos, Client->Uin); //Uin
		Write32(pWakeKartInfos, 10020); //KartID
		Write32(pWakeKartInfos, 0); //KartType
		WriteUn(pWakeKartInfos, 0); //critProgress
		Write16(pWakeKartInfos, 0); //CanExchange
		WriteUn(pWakeKartInfos, 0); //isCanCrit
		WriteUn(pWakeKartInfos, 1); //isWaked
		WriteUn(pWakeKartInfos, 0); //isExchanged
		WriteUn(pWakeKartInfos, 10); //currentWakeLevel
		WriteUn(pWakeKartInfos, 0); //wakeType
		WriteUn(pWakeKartInfos, 0); //wakeKartType
		Write16(pWakeKartInfos, 0); //JetAddValue
		Write16(pWakeKartInfos, 0); //SpeedAddValue
		Write16(pWakeKartInfos, 0); //SJetAddValue
		Write16(pWakeKartInfos, 0); //AccAddValue
		Write16(pWakeKartInfos, 0); //NextLevelJetAddValue
		Write16(pWakeKartInfos, 0); //NextLevelAccAddValue
		Write16(pWakeKartInfos, 0); //NextLevelSpeedAddValue
		Write16(pWakeKartInfos, 0); //NextLevelSJetAddValue
		Write16(pWakeKartInfos, 0); //CostSuperMoney
		Write16(pWakeKartInfos, 0); //CostWakeCoupons
		Write16(pWakeKartInfos, 0); //NeedItemNums
		Write32(pWakeKartInfos, 0); //NeedItemID[]

		len = pWakeKartInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //NoteInfo[]

	len = p - buf;
	SendToClient(Client, 16068, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void loadKartBoatInfo(BYTE*& pKartPhysParam) {
	WORD len;
	Write8(pKartPhysParam, 1); // HasBoatParam
	for (size_t i = 0; i < 1; i++)
	{ // BoatParam
		BYTE* pBoatParam = pKartPhysParam;
		Write16(pBoatParam, 0); // len

		Write32(pBoatParam, 0); // DirKeyForceParamC
		Write32(pBoatParam, 0); // DirKeyForceParamD
		Write32(pBoatParam, 0); // DTFLimitAngle
		Write32(pBoatParam, 0); // DTFRatio

		Write32(pBoatParam, 10000); // UpKeyVecForce
		Write32(pBoatParam, 2000); // UpKeyVecForceParamA
		Write32(pBoatParam, -800); // UpKeyVecForceParamB
		Write32(pBoatParam, 0); // LRKeyVecForce

		Write32(pBoatParam, 2000); // LRKeyVecForceParamA
		Write32(pBoatParam, 70); // LRKeyVecForceParamB
		Write32(pBoatParam, 1000); // VecForce
		Write32(pBoatParam, 0); // VecForceParamA

		Write32(pBoatParam, 300); // VecForceParamB
		Write32(pBoatParam, 250); // BoatShape[]
		Write32(pBoatParam, 250); // BoatShape[]
		Write32(pBoatParam, 1000); // BoatShape[]

		Write32(pBoatParam, 3100); // WecEffect
		Write32(pBoatParam, 4500); // BannerKeyTwist
		Write32(pBoatParam, 0); // BannerKeyTwistParamA
		Write32(pBoatParam, 1000); // BannerKeyTwistParamB

		Write32(pBoatParam, 5000); // SuaiJianTwist
		Write32(pBoatParam, 600); // BoatHeighRatio
		Write32(pBoatParam, 5200); // BannerTwist
		Write32(pBoatParam, 1300); // BannerTwistParamA

		Write32(pBoatParam, 4000); // DirUpKeyForce
		Write32(pBoatParam, 3500); // DirUpKeyForceParamA
		Write32(pBoatParam, 0); // DirUpKeyForceParamB
		Write32(pBoatParam, 0); // DirKeyForce

		Write32(pBoatParam, 0); // DirKeyForceParamA
		Write32(pBoatParam, 1000000); // DirKeyForceParamB
		Write32(pBoatParam, 350000); // AddNO2PerSec
		Write32(pBoatParam, 375); // BoatAirValue
		Write32(pBoatParam, 3000); // ifSlidFricForce

		len = pBoatParam - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}
	typedef struct BoatForwardAccel // 库定义数据类型结构
	{
		UINT Key;
		UINT Value;
	} *LPREFITITEMWEIGHTTYPE;
	const static BoatForwardAccel BoatForwardAccelCfg[] =
	{
		{0, 5100},
		{5000, 5100},
		{5700, 5600},
		{9000, 6100},
		{9300, 6600},
		{11000, 7000},
		{11800, 7100},
		{15000, 9300},
		{16000, 9300},
		{20000, 9350},
		{25000, 9300},
		{30000, 9300},
		{35000, 9300},
	};
	Write8(pKartPhysParam, 13); // BoatForwardAccelNum//集气
	for (size_t i = 0; i < 13; i++)
	{ // BoatForwardAccel
		BYTE* pBoatForwardAccel = pKartPhysParam;
		Write16(pBoatForwardAccel, 0); // len

		Write32(pBoatForwardAccel, BoatForwardAccelCfg->Key); // Key
		Write32(pBoatForwardAccel, BoatForwardAccelCfg->Value); // Value

		len = pBoatForwardAccel - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}

	Write32(pKartPhysParam, 0); // DirUpKeyForceParamA
	Write32(pKartPhysParam, 0); // DirUpKeyForceParamB
}
DWORD WINAPI OnSkill5(PVOID Info) {
	ClientNode* Client = (ClientNode*)Info;
	while (true) {
		if (Client->StartGameTime == false) {
			return 0;
		}
		Sleep(125);
		if (Client->StartGameTime == false) {
			return 0;
		}
		ResponseReportDrift(Client, 1, 10);
		Client->CollectedN2O += 10;
		if (Client->CollectedN2O >= 1000) {
			Client->CollectedN2O = 0;
			if (Client->getPropNum >= 1) {
				ResponseReportDrift(Client, 777, 0);
			}
			else {
				ResponseGetPropS(Client, 777);
			}
		}
		
	}
	return 0;
}
DWORD WINAPI SetGameTime(PVOID Info) {
	ClientNode* Client = (ClientNode*)Info;
	while(true) {
		if (Client->StartGameTime == false) {
			return 0;
		}
		Sleep(1000);
		if (Client->StartGameTime == false) {
			return 0;
		}
		Client->GameTime++;
		if (Client->KartSkillID==7&& Client->PetSkillID == 7) {
			if (Client->GameTime == 60) {
					Client->KartStatus = 1;
			}
		}
		else {
			if (Client->GetSkillID(7)) {
				if (Client->GameTime == 30) {
				
					
					Client->KartStatus = 1;
				}

			}
		}
		
	}
	return 0;
}
DWORD WINAPI XingDieTime(PVOID Info) {
	ClientNode* Client = (ClientNode*)Info;
	if (Client->IsFinish == true) {
		return 0;
	}
	ResponseReportDrift(Client, 777, 999);
	NotifyUseProp(Client, Client->Uin, Client->Uin, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, {}, 0, {}, 0, 0, 0, 0, 0);
	if (Client->getPropNum<2) {
		ResponseGetPropS(Client, 777);
	}
	for (size_t i = 0; i < 30; i++) {
		Sleep(500);
		ResponseReportDrift(Client, 777, -33);
	}
	//Sleep(15000);
	Client->KartStatus = 0;
	ResponseReportDrift(Client, 777, 1000);
	return 0;
}
void NotifyRaceOver(ClientNode *Client, RoomNode *Room);
void NotifyRaceOverNew(ClientNode *Client);
DWORD WINAPI OnSingleJump(PVOID Client)
{
	while (1)
	{
		Sleep(30000);//模拟30秒跳一次
		if (((ClientNode*)Client)->TaskID == 0xFFFE && ((ClientNode*)Client)->IsLogin)
		{
			BYTE* p = jump24238;
			Write32(p, ((ClientNode*)Client)->Uin);
			SendToClient((ClientNode*)Client, 24238, jump24238, 91, ((ClientNode*)Client)->ConnID, FE_PLAYER, ((ClientNode*)Client)->ConnID, Notify);

		}
		if (((ClientNode*)Client)->TaskID == 0xFFFF && ((ClientNode*)Client)->IsLogin)
		{
			//跳关
			SendToClient(((ClientNode*)Client), 24255, jump24255, 91, ((ClientNode*)Client)->ConnID, FE_PLAYER, ((ClientNode*)Client)->ConnID, Notify);
		}
		if (((ClientNode*)Client)->TaskID == 0)
		{
			((ClientNode*)Client)->JumpThread = 0;
			break;
		}
	}
	return 0;
}
DWORD WINAPI OnSingleOver(PVOID Client)
{

	if (((ClientNode*)Client)->TaskID == 0xFFFE)
	{
		((ClientNode*)Client)->TaskID = 0;
		((ClientNode*)Client)->IsFinish = FALSE;
		SendToClient((ClientNode*)Client, 24238, (const BYTE*)"\x00\x00\x27\x11\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x1e\x00\x00\x00\x00\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x21\x00\x00\x00\x00\x00\x00\x00\x21\x00\x00\x00\x00\x00\x00", 72, ((ClientNode*)Client)->ConnID, FE_PLAYER, ((ClientNode*)Client)->ConnID, Notify);
		Sleep(2000);
		SendToClient((ClientNode*)Client, 514, (const BYTE*)"\x00\x00\x00\x00", 4, ((ClientNode*)Client)->GameID, FE_GAMELOGIC, ((ClientNode*)Client)->ConnID, Notify);
	}
	else if (((ClientNode*)Client)->TaskID == 0xFFFF)
	{
		((ClientNode*)Client)->TaskID = 0;
		((ClientNode*)Client)->IsFinish = FALSE;
		const char* buf6 = "\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x52\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1a\x60\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x2c\x00\x00\x27\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x1a\x60\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x50\x75";
		SendToClient((ClientNode*)Client, 24255, (const BYTE*)buf6, 161, ((ClientNode*)Client)->ConnID, FE_PLAYER, ((ClientNode*)Client)->ConnID, Notify);
		Sleep(2000);
		SendToClient((ClientNode*)Client, 514, (const BYTE*)"\x00\x00\x00\x00\x00\x00\x00\x00", 8, ((ClientNode*)Client)->GameID, FE_GAMELOGIC, ((ClientNode*)Client)->ConnID, Notify);

		const char* buf7 = "\x00\x00\x00\x01\x00\x2e\x00\x01\x06\x00\xb4\xf3\xbc\xd2\xd2\xbb\xc6\xf0\xcc\xf4\xd5\xbd\xbc\xab\xcf\xde\xa3\xa1\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x64\x70\x64\x1c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0a\x00\x08\x47\x7e\x1d\x00\x00\x00\x00\x01\x00\x11\x00\x00\x00\x00\x00\x04\x01\x01\x00\x08\x11\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x0f\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x0f\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x17\x31\x2d\x31\x38\x37\x32\x34\x37\x33\x37\x36\x37\x2d\x31\x36\x38\x35\x30\x38\x37\x32\x36\x30\x00\x00\x00\x00\x00\x00\x00\x00\x00\x72\x69";
		SendToClient((ClientNode*)Client, 103, (BYTE*)buf7, 207, ((ClientNode*)Client)->ServerID, FE_GAMESVRD, ((ClientNode*)Client)->ConnID, Response);

	}


	return 0;
}
void LoadKartConfig()
{

	std::filesystem::path path(".\\Kart");
	if (exists(path))
	{																	  // 目录存在
		auto begin = std::filesystem::recursive_directory_iterator(path); // 获取文件系统迭代器
		auto end = std::filesystem::recursive_directory_iterator();		  // end迭代器
		for (auto &it = begin; it != end; it++)
		{
			auto &entry = *it;
			if (std::filesystem::is_regular_file(entry))
			{ // 是否是普通文件
				auto &PathInfo = entry.path();
				if (PathInfo.extension() == ".yml")
				{
					auto path = PathInfo.string();
					const YAML::Node uKartItem = YAML::LoadFile(path);
					if (true)
					{
						KartItem *LKartItem = new KartItem;
						LKartItem->KartID = uKartItem["KartID"].as<UINT>();
						const YAML::Node KartSkillInfo = uKartItem["KartSkill"];
						const YAML::Node GrooveInfo = uKartItem["GrooveInfo"];
						{
							if (GrooveInfo)
							{
								LKartItem->GrooveInfoCount = (BYTE)GrooveInfo.size();
								for (size_t i = 0; i < LKartItem->GrooveInfoCount; i++)
								{
									LKartItem->GrooveInfo[i].ID = GrooveInfo[i]["ID"].as<UINT>();
									LKartItem->GrooveInfo[i].Type = GrooveInfo[i]["Type"].as<UINT>();
								}
							}
							else
							{
								LKartItem->GrooveInfo[0].ID=1;
								LKartItem->GrooveInfo[0].Type=0;
								LKartItem->GrooveInfo[1].ID = 2;
								LKartItem->GrooveInfo[1].Type = 0;
								LKartItem->GrooveInfo[2].ID = 3;
								LKartItem->GrooveInfo[2].Type = 0;
								LKartItem->GrooveInfo[3].ID = 4;
								LKartItem->GrooveInfo[3].Type = 1;
								LKartItem->GrooveInfo[4].ID = 5;
								LKartItem->GrooveInfo[4].Type = 1;
							}
						}
						{
							if (KartSkillInfo)
							{
								LKartItem->Skill.PID = KartSkillInfo["PID"].as<WORD>();
								LKartItem->Skill.IsOpen = KartSkillInfo["IsOpen"].as<bool>();
								LKartItem->Skill.Progress = KartSkillInfo["Progress"].as<UINT>();
							}
							else
							{
								LKartItem->Skill.PID = 34;
								LKartItem->Skill.IsOpen = true;
								LKartItem->Skill.Progress = 1000;
							}
						}
						const YAML::Node KartPhysInfo = uKartItem["PhysInfo"];
						{
							LKartItem->PhysInfoCount = (BYTE)KartPhysInfo.size();
							for (size_t i = 0; i < LKartItem->PhysInfoCount; i++)
							{
								LKartItem->PhysInfo[i].PID = KartPhysInfo[i]["PID"].as<UINT>();
								LKartItem->PhysInfo[i].AffectID = KartPhysInfo[i]["AffectID"].as<UINT>();
								LKartItem->PhysInfo[i].Param = KartPhysInfo[i]["Param"].as<UINT>();
								LKartItem->PhysInfo[i].LifeTime = KartPhysInfo[i]["LifeTime"].as<UINT>();
							}
						}
						const YAML::Node KartCompensateParam = uKartItem["CompensateParam"];
						{
							LKartItem->CompensateParamCount = (BYTE)KartCompensateParam.size();
							for (size_t i = 0; i < LKartItem->CompensateParamCount; i++)
							{
								LKartItem->CompensateParam[i].Percent = KartCompensateParam[i]["Percent"].as<UINT>();
								LKartItem->CompensateParam[i].Value = KartCompensateParam[i]["Value"].as<UINT>();
							}
						}
						const YAML::Node KartCollisionBoxParam = uKartItem["CollisionBoxParam"];
						{
							LKartItem->CollisionBoxParam.BoxLength = KartCollisionBoxParam["BoxLength"].as<int>();
							LKartItem->CollisionBoxParam.BoxWidth = KartCollisionBoxParam["BoxWidth"].as<int>();
							LKartItem->CollisionBoxParam.BoxHeight = KartCollisionBoxParam["BoxHeight"].as<int>();
							LKartItem->CollisionBoxParam.AdjustOffset = KartCollisionBoxParam["AdjustOffset"].as<int>();
							LKartItem->CollisionBoxParam.AdjustPickGap = KartCollisionBoxParam["AdjustPickGap"].as<int>();
							LKartItem->CollisionBoxParam.AdjustRemoteOffset = KartCollisionBoxParam["AdjustRemoteOffset"].as<int>();
							LKartItem->CollisionBoxParam.AdjustBevelOffset = KartCollisionBoxParam["AdjustBevelOffset"].as<int>();
							LKartItem->CollisionBoxParam.ControllerOffset = KartCollisionBoxParam["ControllerOffset"].as<int>();
						}
						if (uKartItem["KartSkatePara"])
						{
							const YAML::Node KartSkatePara = uKartItem["KartSkatePara"];
							{
								LKartItem->SkatePara.TrackSpeed = KartSkatePara["TrackSpeed"].as<int>();
								LKartItem->SkatePara.SkateSpeedCoef_AirBigJet = KartSkatePara["SkateSpeedCoef_AirBigJet"].as<int>();
								LKartItem->SkatePara.SkateSpeedCoef_Jump = KartSkatePara["SkateSpeedCoef_Jump"].as<int>();
								LKartItem->SkatePara.SkateSpeedCoef_AirJetPoint = KartSkatePara["SkateSpeedCoef_AirJetPoint"].as<int>();
								LKartItem->SkatePara.SkateSpeedCoef_MaxForce = KartSkatePara["SkateSpeedCoef_MaxForce"].as<int>();
								LKartItem->SkatePara.OntrackAccRef = KartSkatePara["OntrackAccRef"].as<int>();
								LKartItem->SkatePara.TrackAccRef = KartSkatePara["TrackAccRef"].as<int>();
								LKartItem->SkatePara.DashSkill = KartSkatePara["DashSkill"].as<int>();
							}
						}
						else
						{
							LKartItem->SkatePara.TrackSpeed = 242000;
							LKartItem->SkatePara.SkateSpeedCoef_AirBigJet = -200;
							LKartItem->SkatePara.SkateSpeedCoef_Jump = -190;
							LKartItem->SkatePara.SkateSpeedCoef_AirJetPoint = 5000;
							LKartItem->SkatePara.SkateSpeedCoef_MaxForce = 40000;
							LKartItem->SkatePara.OntrackAccRef = 190000;
							LKartItem->SkatePara.TrackAccRef = 250000;
							LKartItem->SkatePara.DashSkill = 1000;
						}

						const YAML::Node KartCarToCarCollisionParam = uKartItem["CarToCarCollisionParam"];
						{
							LKartItem->CarToCarCollisionParam.CollisionRadius = KartCarToCarCollisionParam["CollisionRadius"].as<int>();
							LKartItem->CarToCarCollisionParam.CollisionLength = KartCarToCarCollisionParam["CollisionLength"].as<int>();
							LKartItem->CarToCarCollisionParam.CollisionWidth = KartCarToCarCollisionParam["CollisionWidth"].as<int>();
							LKartItem->CarToCarCollisionParam.CollisionHeight = KartCarToCarCollisionParam["CollisionHeight"].as<int>();
							LKartItem->CarToCarCollisionParam.CollisionLostCtrlTime = KartCarToCarCollisionParam["CollisionLostCtrlTime"].as<int>();
						}
						LKartItem->BaseTurnRate = uKartItem["BaseTurnRate"].as<UINT>();
						LKartItem->VolatileTurnRate = uKartItem["VolatileTurnRate"].as<UINT>();
						LKartItem->MaxTurnSpeed = uKartItem["MaxTurnSpeed"].as<UINT>();
						LKartItem->MinTurnSpeed = uKartItem["MinTurnSpeed"].as<UINT>();
						LKartItem->MaxAccuTime = uKartItem["MaxAccuTime"].as<UINT>();
						LKartItem->BaseAccuRate = uKartItem["BaseAccuRate"].as<UINT>();
						LKartItem->MaxAffectSpeed = uKartItem["MaxAffectSpeed"].as<UINT>();
						LKartItem->Gravity = uKartItem["Gravity"].as<int>();
						LKartItem->AdditionalLocalZSpeed = uKartItem["AdditionalLocalZSpeed"].as<int>();
						LKartItem->StartVec = uKartItem["StartVec"].as<UINT>();
						LKartItem->EndVecFist = uKartItem["EndVecFist"].as<UINT>();
						LKartItem->EndVecSecon = uKartItem["EndVecSecon"].as<UINT>();
						LKartItem->DirKeyForce = uKartItem["DirKeyForce"].as<UINT>();
						LKartItem->DirKeyTwist = uKartItem["DirKeyTwist"].as<UINT>();
						LKartItem->BannerTwist = uKartItem["BannerTwist"].as<UINT>();
						LKartItem->BannerKeyTwist = uKartItem["BannerKeyTwist"].as<UINT>();
						LKartItem->BannerVecForce = uKartItem["BannerVecForce"].as<UINT>();
						LKartItem->BannerHeadForce = uKartItem["BannerHeadForce"].as<UINT>();
						LKartItem->SlidFricForce = uKartItem["SlidFricForce"].as<UINT>();
						LKartItem->RollFricForce = uKartItem["RollFricForce"].as<UINT>();
						LKartItem->StartWec = uKartItem["StartWec"].as<UINT>();
						LKartItem->MaxWec = uKartItem["MaxWec"].as<UINT>();
						LKartItem->SuaiJianTwist = uKartItem["SuaiJianTwist"].as<UINT>();
						LKartItem->DirUpKeyForce = uKartItem["DirUpKeyForce"].as<UINT>();
						const YAML::Node KartAccelStatus = uKartItem["AccelStatus"];
						{
							LKartItem->AccelStatusCount = (BYTE)KartAccelStatus.size();
							for (size_t i = 0; i < LKartItem->AccelStatusCount; i++)
							{
								LKartItem->AccelStatus[i] = KartAccelStatus[i].as<UINT>();
							}
						}
						const YAML::Node KartForwardAccel = uKartItem["ForwardAccel"];
						{
							LKartItem->ForwardAccelNum = (BYTE)KartForwardAccel.size();
							for (size_t i = 0; i < LKartItem->ForwardAccelNum; i++)
							{
								LKartItem->ForwardAccel[i].Key = KartForwardAccel[i]["Key"].as<int>();
								LKartItem->ForwardAccel[i].Value = KartForwardAccel[i]["Value"].as<int>();
							}
						}
						const YAML::Node KartForwardDecel = uKartItem["ForwardDecel"];
						{
							LKartItem->ForwardDecelNum = (BYTE)KartForwardDecel.size();
							for (size_t i = 0; i < LKartItem->ForwardDecelNum; i++)
							{
								LKartItem->ForwardDecel[i].Key = KartForwardDecel[i]["Key"].as<int>();
								LKartItem->ForwardDecel[i].Value = KartForwardDecel[i]["Value"].as<int>();
							}
						}
						const YAML::Node KartBackwardAccel = uKartItem["BackwardAccel"];
						{
							LKartItem->BackwardAccelNum = (BYTE)KartBackwardAccel.size();
							for (size_t i = 0; i < LKartItem->BackwardAccelNum; i++)
							{
								LKartItem->BackwardAccel[i].Key = KartBackwardAccel[i]["Key"].as<int>();
								LKartItem->BackwardAccel[i].Value = KartBackwardAccel[i]["Value"].as<int>();
							}
						}
						const YAML::Node KartBackwardDecel = uKartItem["BackwardDecel"];
						{
							LKartItem->BackwardDecelNum = (BYTE)KartBackwardDecel.size();
							for (size_t i = 0; i < LKartItem->BackwardDecelNum; i++)
							{
								LKartItem->BackwardDecel[i].Key = KartBackwardDecel[i]["Key"].as<int>();
								LKartItem->BackwardDecel[i].Value = KartBackwardDecel[i]["Value"].as<int>();
							}
						}
						// KartItems[LKartItem->KartID] = LKartItem;
						UINT KartID;
						KartID = atof(PathInfo.filename().string().c_str());
						KartItems[KartID] = LKartItem;
					}
				}
			}
		}
	}
	Z_INFO("赛车文件加载完成[%d]!\n", KartItems.size());
}
KartItem *GetKartItem(UINT KartID)
{
	KartItem *pKartItem = KartItems[KartID];
	return pKartItem;
}
void WriteKartPhysParamOld(BYTE *&p, UINT Uin, UINT KartID, UINT PetID, UINT PetLevel, UINT SkinID)
{
	WORD len;
	BYTE *pKartPhysParam = p;
	Write16(pKartPhysParam, 0); // len

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
	int ShapeRefitCount = 0;
	int KartHeadRefitItemID = 0;
	int KartTailRefitItemID = 0;
	int KartFlankRefitItemID = 0;
	int KartTireRefitItemID = 0;

	int CrashCompensatePower = 0;
	int ActiveLevel = 0;
	if (Uin > 10000) // Not NPC
	{
		UINT RefitNO = getGameConfig()["Global"]["Refit"]["RefitNO"].as<UINT>();
		if (RefitNO == 1)
		{
			RefitNO = -1;
			YAML::Node Karts;
			YAML::Node ContainUin = getGameConfig()["Global"]["Refit"]["ContainUin"];
			for (size_t i = 0; i < ContainUin.size(); i++)
			{
				YAML::Node Uins = ContainUin[i];
				if (Uins["Uin"].as<INT>() == -1)
				{
					// RefitNO = 1;
					Karts = Uins["ContainKart"];
				}
				else if (Uins["Uin"].as<INT>() == Uin)
				{
					// RefitNO = 1;
					Karts = Uins["ContainKart"];
				}
			}
			if (Karts.size() > 0)
			{
				for (size_t i = 0; i < Karts.size(); i++)
				{
					YAML::Node Kart = Karts[i];
					if (Kart["KartID"].as<INT>() == -1)
					{
						RefitNO = 1;
					}
					else if (Kart["KartID"].as<INT>() == KartID)
					{
						RefitNO = 1;
					}
				}
			}
		}

		if (RefitNO == 1)
		{
			WWeight = getGameConfig()["Global"]["Refit"]["WWeight"].as<LONG>();
			SpeedWeight = getGameConfig()["Global"]["Refit"]["SpeedWeight"].as<LONG>();
			JetWeight = getGameConfig()["Global"]["Refit"]["JetWeight"].as<LONG>();
			SJetWeight = getGameConfig()["Global"]["Refit"]["SJetWeight"].as<LONG>();
			AccuWeight = getGameConfig()["Global"]["Refit"]["AccuWeight"].as<LONG>();
			Speed2Weight = getGameConfig()["Global"]["Refit"]["Speed2Weight"].as<LONG>();
			DriftVecWeight = getGameConfig()["Global"]["Refit"]["DriftVecWeight"].as<LONG>();
			AdditionalZSpeedWeight = getGameConfig()["Global"]["Refit"]["AdditionalZSpeedWeight"].as<LONG>();
			AntiCollisionWeight = getGameConfig()["Global"]["Refit"]["AntiCollisionWeight"].as<LONG>();
		}
		else
		{
			sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID,ActiveLevel  FROM KartRefit WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, KartID);
				if (sqlite3_step(stmt) == SQLITE_ROW)
				{
					RefitCout = sqlite3_column_int(stmt, 0);
					MaxFlags = sqlite3_column_int(stmt, 1);
					WWeight = sqlite3_column_int(stmt, 2);
					SpeedWeight = sqlite3_column_int(stmt, 3);
					JetWeight = sqlite3_column_int(stmt, 4);
					SJetWeight = sqlite3_column_int(stmt, 5);
					AccuWeight = sqlite3_column_int(stmt, 6);
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
					ActiveLevel = sqlite3_column_int(stmt, 23);

					ShapeRefitCount = sqlite3_column_int(stmt, 7);
					KartHeadRefitItemID = sqlite3_column_int(stmt, 8);
					KartTailRefitItemID = sqlite3_column_int(stmt, 9);
					KartFlankRefitItemID = sqlite3_column_int(stmt, 10);
					KartTireRefitItemID = sqlite3_column_int(stmt, 11);
				}
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		UINT ForbiddenKartID[] = { 129287, 129099, 127438, 126532, 126347, 124679, 123514, 123499, 122196, 120646, 119067, 119059, 119058, 117935, 116734, 115166, 115160, 112177, 112176, 109563, 109560, 109299, 107371, 107370, 106676, 106675, 106674, 106673, 105425, 104129, 102225, 102224, 102223, 102219, 101885, 100049, 100048, 98830, 98829, 98263, 96785, 94844, 94843, 94842, 94841, 94838, 91957, 90945, 89936, 88865, 86998, 85949, 85948, 85947, 85946, 85942, 84869, 83042, 82249, 81293, 80542, 80161, 78296, 78292, 77236, 75808, 74363, 74362, 73697, 72741, 71588, 71452, 70047, 68643, 66600, 65864, 65109, 63702, 63431, 63429, 62772, 60430, 34851, 33789, 33788, 33258, 32819, 32407, 32406, 31570, 30808, 28779, 28110, 27598, 26931, 26144, 25717, 24924, 24410, 24146, 23980, 23378, 23063, 23015, 22526, 22097, 21961, 21593, 21319, 21086, 20471, 20244, 19951, 19618, 19298, 18767, 18179, 17668, 17354, 16572, 15456, 14107, 14106, 13902, 13703, 12948, 11376 };
		bool hasActiveKart = false;
		for (size_t i = 0; i < 127; i++) {
			if (KartID == ForbiddenKartID[i]) {
				hasActiveKart = true;
				continue;
			}
		}
		if (hasActiveKart == false) {
			ActiveLevel = 1;
		}
		if (PetID > 0&&ActiveLevel>0)
		{
			YAML::Node PetSkills = GetPetConfig(PetID);

			for (size_t i = 0; i < PetSkills.size(); i++)
			{
				YAML::Node PetSkill = PetSkills[i];

				switch (PetSkill["SkillID"].as<UINT>())
				{
				case 2:
					CrashCompensatePower = PetSkill["Value"].as<float>() * PetLevel;
					break;
				}
			}
		}
	}
	UINT RefitCard = 0;
	sql = "SELECT Status,ItemID FROM ITEM WHERE Uin=? AND (ItemID=120258 or ItemID=120259)";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{

			if (sqlite3_column_int(stmt, 0) == 120259) {
				RefitCard = 2;
			}
			else {
				RefitCard = sqlite3_column_int(stmt, 0);
			}


		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (RefitCard == 1) {
		SpeedWeight = 29;
		JetWeight = 29;
		SJetWeight = 29;
		AccuWeight = 29;
		AntiCollisionWeight = 29;

	}
	else if (RefitCard == 2) {
		WWeight = 29;
		SpeedWeight = 29;
		JetWeight = 29;
		SJetWeight = 29;
		AccuWeight = 29;
		Speed2Weight = 29;
		DriftVecWeight = 29;
		AdditionalZSpeedWeight = 29;
		AntiCollisionWeight = 29;



	}
	char FilePath[MAX_PATH];
	UINT loadID = KartID;
	if (SkinID > 0)
	{

		sprintf_s(FilePath, ".\\Kart\\%d.yml", SkinID);
		loadID = SkinID;
	}
	else
	{
		sprintf_s(FilePath, ".\\Kart\\%d.yml", KartID);
	}
	bool isFileExists;
	struct stat buffer;
	isFileExists = (stat(FilePath, &buffer) == 0);
	if (!isFileExists)
	{
		sprintf_s(FilePath, ".\\Kart\\%d.yml", 12720);
	}
	// sprintf_s(FilePath, ".\\Kart\\%d.yml", KartID);
	YAML::Node config = YAML::LoadFile(FilePath);

	Write32(pKartPhysParam, loadID);
	
	YAML::Node PhysInfo = config["PhysInfo"];
	BYTE* pPhysInfoCount = pKartPhysParam;
	UINT PhysInfoSize = (BYTE)PhysInfo.size();
	Write8(pKartPhysParam, 0); // PhysInfoCount
	UINT ParamJetWeightRes = 0;
	UINT LifeTimeJetWeightRes = 0;
	UINT ParamSJetWeightRes = 0;
	UINT LifeTimeSJetWeightRes = 0;
	if (JetWeight <= 10) {
		ParamJetWeightRes = JetWeight * 10;
		LifeTimeJetWeightRes = JetWeight * 8;
	}
	else if (JetWeight <= 20) {
		ParamJetWeightRes = 100 + ((JetWeight - 10) * 6);
		LifeTimeJetWeightRes = 80 + ((JetWeight - 10) * 3);
	}
	else {
		ParamJetWeightRes = 160 + ((JetWeight - 20) * 4);
		LifeTimeJetWeightRes = 120 + ((JetWeight - 20) * 2);
	}
	if (SJetWeight <= 10) {
		ParamSJetWeightRes = SJetWeight * 10;
		LifeTimeSJetWeightRes = SJetWeight * 4;
	}
	else if (JetWeight <= 20) {
		ParamSJetWeightRes = 100 + ((SJetWeight - 10) * 6);
		LifeTimeSJetWeightRes = 40 + ((SJetWeight - 10) * 3);
	}
	else {
		ParamSJetWeightRes = 160 + ((SJetWeight - 20) * 4);
		LifeTimeSJetWeightRes = 100;
	}
	bool Has777 = true;
	bool Has34 = true;
	for (size_t i = 0; i < PhysInfo.size(); i++)
	{
		{ // PhysInfo
			BYTE *pPhysInfo = pKartPhysParam;
			Write16(pPhysInfo, 0); // len

			int PID = PhysInfo[i]["PID"].as<int>();
			int AffectID = PhysInfo[i]["AffectID"].as<int>();
			int Param = PhysInfo[i]["Param"].as<int>();
			int LifeTime = PhysInfo[i]["LifeTime"].as<int>();
			// TODO: 应该限制最大值, 但懒得做限制, 因为不同赛车等级是不同的, 留给后人去折腾吧!
			switch (PID)
			{
			case 1: // 燃料
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			case 2002: // 燃料
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			case 2001: // 点火
				Param += ParamSJetWeightRes;
				LifeTime += LifeTimeSJetWeightRes;
				break;
			case 99999: // 进气
				Param += 300 * AccuWeight;
				break;
			case 777: // 紫色氮气
				Has777 = false;
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			case 34: // 紫色氮气
				Has34 = false;
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			default:
				break;
			}
#ifdef DEBUG
			Z_INFO("当前 PID:%d\n", PID);
#endif
			Write32(pPhysInfo, PID);
			Write32(pPhysInfo, AffectID);
			Write32(pPhysInfo, Param);
			Write32(pPhysInfo, LifeTime);
			len = pPhysInfo - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}	
	if (GetClient(Uin)&&(GetClient(Uin)->GetSkillID(0)|| (GetClient(Uin)->KartID == 129099 && GetClient(Uin)->SkinID == 127616)|| GetClient(Uin)->KartID == 19618)) {
		if(Has777){ //紫色超级氮气
			PhysInfoSize += 1;
			BYTE* pPhysInfo = pKartPhysParam;
			Write16(pPhysInfo, 0); //len
			Write32(pPhysInfo, 777);//玉麒麟专属超级喷 必须写入这个数据才能显示专属氮气条
			Write32(pPhysInfo, 100);
			Write32(pPhysInfo, 6500);
			Write32(pPhysInfo, 3500);
			len = pPhysInfo - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
		if (Has34) { //紫色超级氮气
			PhysInfoSize += 1;
			BYTE* pPhysInfo = pKartPhysParam;
			Write16(pPhysInfo, 0); //len
			Write32(pPhysInfo, 34);
			Write32(pPhysInfo, 100);
			Write32(pPhysInfo, 6500);
			Write32(pPhysInfo, 3500);
			len = pPhysInfo - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}
	Set8(pPhysInfoCount, (WORD)PhysInfoSize);
	YAML::Node CompensateParam = config["CompensateParam"];
	Write8(pKartPhysParam, (BYTE)CompensateParam.size()); // CompensateParamCount
	for (size_t i = 0; i < CompensateParam.size(); i++)
	{
		{ // CompensateParam
			BYTE *pCompensateParam = pKartPhysParam;
			Write16(pCompensateParam, 0); // len

			Write32(pCompensateParam, CompensateParam[i]["Percent"].as<int>());
			Write32(pCompensateParam, CompensateParam[i]["Value"].as<int>());

			len = pCompensateParam - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	{ // CollisionBoxParam
		YAML::Node CollisionBoxParam = config["CollisionBoxParam"];
		BYTE *pCollisionBoxParam = pKartPhysParam;
		Write16(pCollisionBoxParam, 0); // len

		Write32(pCollisionBoxParam, CollisionBoxParam["BoxLength"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["BoxWidth"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["BoxHeight"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustOffset"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustPickGap"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustRemoteOffset"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["AdjustBevelOffset"].as<int>());
		Write32(pCollisionBoxParam, CollisionBoxParam["ControllerOffset"].as<int>());

		len = pCollisionBoxParam - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}

	{ // CarToCarCollisionParam
		YAML::Node CarToCarCollisionParam = config["CarToCarCollisionParam"];
		BYTE *pCarToCarCollisionParam = pKartPhysParam;
		Write16(pCarToCarCollisionParam, 0); // len

		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionRadius"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionLength"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionWidth"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionHeight"].as<int>());
		Write32(pCarToCarCollisionParam, CarToCarCollisionParam["CollisionLostCtrlTime"].as<int>()); 

		len = pCarToCarCollisionParam - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}

	Write32(pKartPhysParam, config["BaseTurnRate"].as<int>());
	Write32(pKartPhysParam, config["VolatileTurnRate"].as<int>());
	Write32(pKartPhysParam, config["MaxTurnSpeed"].as<int>());
	Write32(pKartPhysParam, config["MinTurnSpeed"].as<int>());
	Write32(pKartPhysParam, config["MaxAccuTime"].as<int>());
	Write32(pKartPhysParam, config["BaseAccuRate"].as<int>());
	Write32(pKartPhysParam, config["MaxAffectSpeed"].as<int>());
	Write32(pKartPhysParam, config["Gravity"].as<int>() + (10 * WWeight) );  //悬挂
	Write32(pKartPhysParam, config["AdditionalLocalZSpeed"].as<int>() - (10 * AdditionalZSpeedWeight));// 平衡
	Write32(pKartPhysParam, config["StartVec"].as<int>()+ DriftVecWeight);
	Write32(pKartPhysParam, config["EndVecFist"].as<int>() + DriftVecWeight);
	Write32(pKartPhysParam, config["EndVecSecon"].as<int>() + DriftVecWeight);
	Write32(pKartPhysParam, config["DirKeyForce"].as<int>());
	Write32(pKartPhysParam, config["DirKeyTwist"].as<int>());
	Write32(pKartPhysParam, config["BannerTwist"].as<int>());
	Write32(pKartPhysParam, config["BannerKeyTwist"].as<int>());
	Write32(pKartPhysParam, config["BannerVecForce"].as<int>());
	Write32(pKartPhysParam, config["BannerHeadForce"].as<int>());
	Write32(pKartPhysParam, config["SlidFricForce"].as<int>());
	Write32(pKartPhysParam, config["RollFricForce"].as<int>());
	Write32(pKartPhysParam, config["StartWec"].as<int>());
	Write32(pKartPhysParam, config["MaxWec"].as<int>());
	Write32(pKartPhysParam, config["SuaiJianTwist"].as<int>());
	Write32(pKartPhysParam, config["DirUpKeyForce"].as<int>());

	YAML::Node AccelStatus = config["AccelStatus"];
	Write8(pKartPhysParam, (BYTE)AccelStatus.size()); // AccelStatusCount
	for (size_t i = 0; i < AccelStatus.size(); i++)
	{
		{ // m_astKartPhysParam[].m_aiAccelStatus[]
			Write32(pKartPhysParam, AccelStatus[i].as<int>());
		}
	}

	YAML::Node ForwardAccel = config["ForwardAccel"];
	Write8(pKartPhysParam, (BYTE)ForwardAccel.size()); // ForwardAccelNum
	for (size_t i = 0; i < ForwardAccel.size(); i++)
	{
		{ // ForwardAccel
			BYTE *pForwardAccel = pKartPhysParam;
			Write16(pForwardAccel, 0); // len

			int Key = ForwardAccel[i]["Key"].as<int>();
			int Value = ForwardAccel[i]["Value"].as<int>();
			if (i >= 7 && i <= 9)
			{
				// TODO: 引擎; 未限制最大值!
				Value += 35 * SpeedWeight;	// 引擎
				Value += 8 * Speed2Weight; // 推进
			}
			Write32(pForwardAccel, Key);
			Write32(pForwardAccel, Value);

			len = pForwardAccel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	YAML::Node ForwardDecel = config["ForwardDecel"];
	Write8(pKartPhysParam, (BYTE)ForwardDecel.size()); // ForwardDecelNum
	for (size_t i = 0; i < ForwardDecel.size(); i++)
	{
		{ // ForwardDecel
			BYTE *pForwardDecel = pKartPhysParam;
			Write16(pForwardDecel, 0); // len

			Write32(pForwardDecel, ForwardDecel[i]["Key"].as<int>());
			Write32(pForwardDecel, ForwardDecel[i]["Value"].as<int>());

			len = pForwardDecel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	YAML::Node BackwardAccel = config["BackwardAccel"];
	Write8(pKartPhysParam, (BYTE)BackwardAccel.size()); // BackwardAccelNum
	for (size_t i = 0; i < BackwardAccel.size(); i++)
	{
		{ // BackwardAccel
			BYTE *pBackwardAccel = pKartPhysParam;
			Write16(pBackwardAccel, 0); // len

			Write32(pBackwardAccel, BackwardAccel[i]["Key"].as<int>());
			Write32(pBackwardAccel, BackwardAccel[i]["Value"].as<int>());

			len = pBackwardAccel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	YAML::Node BackwardDecel = config["BackwardDecel"];
	Write8(pKartPhysParam, (BYTE)BackwardDecel.size()); // BackwardDecelNum
	for (size_t i = 0; i < BackwardDecel.size(); i++)
	{
		{ // BackwardDecel
			BYTE *pBackwardDecel = pKartPhysParam;
			Write16(pBackwardDecel, 0); // len

			Write32(pBackwardDecel, BackwardDecel[i]["Key"].as<int>());
			Write32(pBackwardDecel, BackwardDecel[i]["Value"].as<int>());

			len = pBackwardDecel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	Write32(pKartPhysParam, CrashCompensatePower + AntiCollisionWeight * 10); // CrashCompensatePower
	Write32(pKartPhysParam, 0);					   // DefenseRate
	Write32(pKartPhysParam, Uin);				   // Uin
	Write32(pKartPhysParam, 0xFFFFFFFF);					   // SpeedupCardGenRate
	//Write32(pKartPhysParam, 0);					   // ExtraFuncFlag
	UINT skillHide = 0;
	if (getGameConfig()["KartExMode"] && getGameConfig()["KartExMode"].as<UINT>() == 1 && GetClient(Uin) && GetClient(Uin)->GetSkillID(8))
	{
		skillHide = 4;
	}
	Write32(pKartPhysParam, 0xF8 | skillHide | 0 | 0);// skillhide | skilln2o | skillview
	if (GetItemGameType(loadID) == 2)
	{
		Write8(pKartPhysParam, 1); // HasSkatePara
		for (size_t i = 0; i < 1; i++)
		{ // hasSkatePara
			BYTE *phasSkatePara = pKartPhysParam;
			Write16(phasSkatePara, 0); // len
			if (config["KartSkatePara"])
			{
				YAML::Node KartSkatePara = config["KartSkatePara"];
				Write32(phasSkatePara, KartSkatePara["TrackSpeed"].as<int>());				   // TrackSpeed //轨道墙壁速度
				Write32(phasSkatePara, KartSkatePara["SkateSpeedCoef_AirBigJet"].as<int>());   // SkateSpeedCoef_AirBigJet
				Write32(phasSkatePara, KartSkatePara["SkateSpeedCoef_Jump"].as<int>());		   // SkateSpeedCoef_Jump
				Write32(phasSkatePara, KartSkatePara["SkateSpeedCoef_AirJetPoint"].as<int>()); // SkateSpeedCoef_AirJetPoint
				Write32(phasSkatePara, KartSkatePara["SkateSpeedCoef_MaxForce"].as<int>());	   // SkateSpeedCoef_MaxForce
				Write32(phasSkatePara, KartSkatePara["OntrackAccRef"].as<int>());			   // OntrackAccRef //轨道集气
				Write32(phasSkatePara, KartSkatePara["TrackAccRef"].as<int>());				   // TrackAccRef // 墙壁
				Write32(phasSkatePara, KartSkatePara["DashSkill"].as<int>());				   // DashSkill
			}
			else
			{
				Write32(phasSkatePara, 250000); // TrackSpeed //轨道墙壁速度
				Write32(phasSkatePara, 0);		// SkateSpeedCoef_AirBigJet
				Write32(phasSkatePara, 0);		// SkateSpeedCoef_Jump
				Write32(phasSkatePara, 0);		// SkateSpeedCoef_AirJetPoint
				Write32(phasSkatePara, 0);		// SkateSpeedCoef_MaxForce
				Write32(phasSkatePara, 350000); // OntrackAccRef //轨道集气
				Write32(phasSkatePara, 350000); // TrackAccRef // 墙壁
				Write32(phasSkatePara, 100000); // DashSkill
			}

			len = phasSkatePara - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}
	else
	{
		Write8(pKartPhysParam, 0); // HasSkatePara
	}

	/*
m_astKartPhysParam[].m_hasSkatePara[].m_iTrackSpeed
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_AirBigJet
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_Jump
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_AirJetPoint
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_MaxForce
m_astKartPhysParam[].m_hasSkatePara[].m_iOntrackAccRef
m_astKartPhysParam[].m_hasSkatePara[].m_iTrackAccRef
m_astKartPhysParam[].m_hasSkatePara[].m_iDashSkill
		*/

	Write8(pKartPhysParam, 0); // HasBattleModeSkillInfoList
	/*
m_astKartPhysParam[].m_astBattleModeSkillInfoList[].m_bSkillNum
m_astKartPhysParam[].m_astBattleModeSkillInfoList[].m_astSkillInfo[].m_shSkillId
m_astKartPhysParam[].m_astBattleModeSkillInfoList[].m_astSkillInfo[].m_bSkillLevel
		*/

	// #ifndef ZingSpeed
	Write32(pKartPhysParam, 200); // EnergyConvert
	Write16(pKartPhysParam, 0); // SuperN2ORate
	Write16(pKartPhysParam, AntiCollisionWeight*5); // CollisionLoseRate 防护
	Write8(pKartPhysParam, 0);	// NotInterruptDrift

	Write8(pKartPhysParam, 0); // AddWeightNum

	Write8(pKartPhysParam, 0); // WeightNum

	Write8(pKartPhysParam, 0); // HasAntiCollisionCfg
	/*
m_astKartPhysParam[].m_astAntiCollistionCfg[].m_uchNum
m_astKartPhysParam[].m_astAntiCollistionCfg[].m_aiPosition[]
		*/

	loadKartBoatInfo(pKartPhysParam);

	len = pKartPhysParam - p;
	Set16(p, len);
	p += len;
}
void WriteKartPhysParam(BYTE *&p, UINT Uin, UINT KartID, UINT PetID, UINT PetLevel, UINT SkinID)
{
	WORD len;
	BYTE *pKartPhysParam = p;
	Write16(pKartPhysParam, 0); // len

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
	int ShapeRefitCount = 0;
	int KartHeadRefitItemID = 0;
	int KartTailRefitItemID = 0;
	int KartFlankRefitItemID = 0;
	int KartTireRefitItemID = 0;
	int ActiveLevel = 0;
	int CrashCompensatePower = 0;
	if (Uin > 10000) // Not NPC
	{
		UINT RefitNO = getGameConfig()["Global"]["Refit"]["RefitNO"].as<UINT>();
		if (RefitNO == 1)
		{
			RefitNO = -1;
			YAML::Node Karts;
			YAML::Node ContainUin = getGameConfig()["Global"]["Refit"]["ContainUin"];
			for (size_t i = 0; i < ContainUin.size(); i++)
			{
				YAML::Node Uins = ContainUin[i];
				if (Uins["Uin"].as<INT>() == -1)
				{
					// RefitNO = 1;
					Karts = Uins["ContainKart"];
				}
				else if (Uins["Uin"].as<INT>() == Uin)
				{
					// RefitNO = 1;
					Karts = Uins["ContainKart"];
				}
			}
			if (Karts.size() > 0)
			{
				for (size_t i = 0; i < Karts.size(); i++)
				{
					YAML::Node Kart = Karts[i];
					if (Kart["KartID"].as<INT>() == -1)
					{
						RefitNO = 1;
					}
					else if (Kart["KartID"].as<INT>() == KartID)
					{
						RefitNO = 1;
					}
				}
			}
		}
		if (RefitNO == 1)
		{
			WWeight = getGameConfig()["Global"]["Refit"]["WWeight"].as<LONG>();
			SpeedWeight = getGameConfig()["Global"]["Refit"]["SpeedWeight"].as<LONG>();
			JetWeight = getGameConfig()["Global"]["Refit"]["JetWeight"].as<LONG>();
			SJetWeight = getGameConfig()["Global"]["Refit"]["SJetWeight"].as<LONG>();
			AccuWeight = getGameConfig()["Global"]["Refit"]["AccuWeight"].as<LONG>();
			Speed2Weight = getGameConfig()["Global"]["Refit"]["Speed2Weight"].as<LONG>();
			DriftVecWeight = getGameConfig()["Global"]["Refit"]["DriftVecWeight"].as<LONG>();
			AdditionalZSpeedWeight = getGameConfig()["Global"]["Refit"]["AdditionalZSpeedWeight"].as<LONG>();
			AntiCollisionWeight = getGameConfig()["Global"]["Refit"]["AntiCollisionWeight"].as<LONG>();
		}
		else
		{
			sql = "SELECT RefitCout,MaxFlags,WWeight,SpeedWeight,JetWeight,SJetWeight,AccuWeight,ShapeRefitCount,KartHeadRefitItemID,KartTailRefitItemID,KartFlankRefitItemID,KartTireRefitItemID,SecondRefitCount,Speed2Weight,DriftVecWeight,AdditionalZSpeedWeight,AntiCollisionWeight,LuckyValue,RefitLuckyValueMaxWeight,ShapeSuitID,LegendSuitLevel,LegendSuitLevelChoice,ShapeLegendSuitID,ActiveLevel  FROM KartRefit WHERE Uin=? AND KartID=?;";
			result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
			if (result == SQLITE_OK)
			{
				sqlite3_bind_int(stmt, 1, Uin);
				sqlite3_bind_int(stmt, 2, KartID);
				if (sqlite3_step(stmt) == SQLITE_ROW)
				{
					RefitCout = sqlite3_column_int(stmt, 0);
					MaxFlags = sqlite3_column_int(stmt, 1);
					WWeight = sqlite3_column_int(stmt, 2);
					SpeedWeight = sqlite3_column_int(stmt, 3);
					JetWeight = sqlite3_column_int(stmt, 4);
					SJetWeight = sqlite3_column_int(stmt, 5);
					AccuWeight = sqlite3_column_int(stmt, 6);
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
					ActiveLevel = sqlite3_column_int(stmt, 23);

					ShapeRefitCount = sqlite3_column_int(stmt, 7);
					KartHeadRefitItemID = sqlite3_column_int(stmt, 8);
					KartTailRefitItemID = sqlite3_column_int(stmt, 9);
					KartFlankRefitItemID = sqlite3_column_int(stmt, 10);
					KartTireRefitItemID = sqlite3_column_int(stmt, 11);
				}
			}
			else
			{
				Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
			}
			sqlite3_finalize(stmt);
			stmt = NULL;
		}
		UINT ForbiddenKartID[] = { 129287, 129099, 127438, 126532, 126347, 124679, 123514, 123499, 122196, 120646, 119067, 119059, 119058, 117935, 116734, 115166, 115160, 112177, 112176, 109563, 109560, 109299, 107371, 107370, 106676, 106675, 106674, 106673, 105425, 104129, 102225, 102224, 102223, 102219, 101885, 100049, 100048, 98830, 98829, 98263, 96785, 94844, 94843, 94842, 94841, 94838, 91957, 90945, 89936, 88865, 86998, 85949, 85948, 85947, 85946, 85942, 84869, 83042, 82249, 81293, 80542, 80161, 78296, 78292, 77236, 75808, 74363, 74362, 73697, 72741, 71588, 71452, 70047, 68643, 66600, 65864, 65109, 63702, 63431, 63429, 62772, 60430, 34851, 33789, 33788, 33258, 32819, 32407, 32406, 31570, 30808, 28779, 28110, 27598, 26931, 26144, 25717, 24924, 24410, 24146, 23980, 23378, 23063, 23015, 22526, 22097, 21961, 21593, 21319, 21086, 20471, 20244, 19951, 19618, 19298, 18767, 18179, 17668, 17354, 16572, 15456, 14107, 14106, 13902, 13703, 12948, 11376 };
		bool hasActiveKart = false;
		for (size_t i = 0; i < 127; i++) {
			if (KartID == ForbiddenKartID[i]) {
				hasActiveKart = true;
				continue;
			}
		}
		if (hasActiveKart == false) {
			ActiveLevel = 1;
		}

		if (PetID > 0&&ActiveLevel>0)
		{
			YAML::Node PetSkills = GetPetConfig(PetID);

			for (size_t i = 0; i < PetSkills.size(); i++)
			{
				YAML::Node PetSkill = PetSkills[i];

				switch (PetSkill["SkillID"].as<UINT>())
				{
				case 2:
					CrashCompensatePower = PetSkill["Value"].as<float>() * PetLevel;
					break;
				}
			}
		}
	}
	UINT RefitCard = 0;
	sql = "SELECT Status,ItemID FROM ITEM WHERE Uin=? AND (ItemID=120258 or ItemID=120259)";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Uin);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			
			if (sqlite3_column_int(stmt, 0)== 120259) {
				RefitCard = 2;
			}
			else {
				RefitCard = sqlite3_column_int(stmt, 0);
			}


		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (RefitCard == 1) {
			SpeedWeight = 29;
			JetWeight = 29;
			SJetWeight = 29;
			AccuWeight = 29;
			AntiCollisionWeight = 29;
		
	}else if(RefitCard == 2) {
		WWeight = 29;
		SpeedWeight = 29;
		JetWeight = 29;
		SJetWeight = 29;
		AccuWeight = 29;
		Speed2Weight = 29;
		DriftVecWeight = 29;
		AdditionalZSpeedWeight = 29;
		AntiCollisionWeight = 29;



	}
	UINT loadID = 0;
	if (SkinID > 0)
	{
		loadID = SkinID;
	}
	else
	{
		loadID = KartID;
	}

	KartItem *KartItem = GetKartItem(loadID);
	if (!KartItem)
	{
		WriteKartPhysParamOld(p, Uin, KartID, PetID, PetLevel, SkinID);
		return;
	}
	// char FilePath[MAX_PATH];
	// if (SkinID > 0) {
	//
	//	sprintf_s(FilePath, ".\\Kart\\%d.yml", SkinID);
	// }
	// else{
	//	sprintf_s(FilePath, ".\\Kart\\%d.yml", KartID);
	// }
	////sprintf_s(FilePath, ".\\Kart\\%d.yml", KartID);
	// YAML::Node config = YAML::LoadFile(FilePath);

	Write32(pKartPhysParam, loadID);
	BYTE PhysInfoNum = KartItem->PhysInfoCount;
	BYTE* pPhysInfoCount = pKartPhysParam;
	Write8(pKartPhysParam, 0); // PhysInfoCount
	UINT ParamJetWeightRes = 0;
	UINT LifeTimeJetWeightRes = 0;
	UINT ParamSJetWeightRes = 0;
	UINT LifeTimeSJetWeightRes = 0;
	if (JetWeight < 10) {
		ParamJetWeightRes = JetWeight * 8;
		LifeTimeJetWeightRes = JetWeight * 4;
	}else if (JetWeight < 20) {
		ParamJetWeightRes = 80 +((JetWeight-10) * 6);
		LifeTimeJetWeightRes = 60+((JetWeight - 10) * 3);
	}
	else{
		ParamJetWeightRes = 160 + ((JetWeight - 20) * 4);
		LifeTimeJetWeightRes = 120 + ((JetWeight - 20) * 2);
	}
	if (SJetWeight <= 10) {
		ParamSJetWeightRes = SJetWeight * 10;
		LifeTimeSJetWeightRes = SJetWeight * 4;
	}
	else if (JetWeight <= 20) {
		ParamSJetWeightRes = 100 + ((SJetWeight - 10) * 6);
		LifeTimeSJetWeightRes = 60 + ((SJetWeight - 10) * 3);
	}
	else {
		ParamSJetWeightRes = 160 + ((SJetWeight - 20) * 4);
		//LifeTimeSJetWeightRes = 120 + ((SJetWeight - 20) * 2);
	}
	bool Has777 = true;
	bool Has34 = true;
	for (size_t i = 0; i < PhysInfoNum; i++)
	{
		{ // PhysInfo
			BYTE *pPhysInfo = pKartPhysParam;
			Write16(pPhysInfo, 0); // len

			int PID = KartItem->PhysInfo[i].PID;
			int AffectID = KartItem->PhysInfo[i].AffectID;
			int Param = KartItem->PhysInfo[i].Param;
			int LifeTime = KartItem->PhysInfo[i].LifeTime;
			// TODO: 应该限制最大值, 但懒得做限制, 因为不同赛车等级是不同的, 留给后人去折腾吧!
			switch (PID)
			{
			case 1: // 燃料
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			case 2002: // 燃料
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			case 2001: // 点火
				Param += ParamSJetWeightRes;
				LifeTime += LifeTimeSJetWeightRes;
				break;
			case 99999: // 进气
				Param += 300 * AccuWeight;
				break;
			case 777: // 紫色氮气
				Has777 = false;
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			case 34: // 紫色氮气
				Has34 = false;
				Param += ParamJetWeightRes;
				LifeTime += LifeTimeJetWeightRes;
				break;
			default:
				break;
			}
#ifdef DEBUG
			Z_INFO("当前 PID:%d\n", PID);
#endif
			Write32(pPhysInfo, PID);
			Write32(pPhysInfo, AffectID);
			Write32(pPhysInfo, Param);
			Write32(pPhysInfo, LifeTime);
			len = pPhysInfo - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}
	if (GetClient(Uin) && (GetClient(Uin)->GetSkillID(0) || (GetClient(Uin)->KartID == 129099 && GetClient(Uin)->SkinID == 127616) || GetClient(Uin)->KartID == 19618)) {
		if (Has777) { //紫色超级氮气
			PhysInfoNum += 1;
			BYTE* pPhysInfo = pKartPhysParam;
			Write16(pPhysInfo, 0); //len
			Write32(pPhysInfo, 777);//玉麒麟专属超级喷 必须写入这个数据才能显示专属氮气条
			Write32(pPhysInfo, 100);
			Write32(pPhysInfo, 6500);
			Write32(pPhysInfo, 3500);
			len = pPhysInfo - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
		if (Has34) { //紫色超级氮气
			PhysInfoNum += 1;
			BYTE* pPhysInfo = pKartPhysParam;
			Write16(pPhysInfo, 0); //len
			Write32(pPhysInfo, 34);
			Write32(pPhysInfo, 100);
			Write32(pPhysInfo, 6500);
			Write32(pPhysInfo, 3500);
			len = pPhysInfo - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}
	Set8(pPhysInfoCount, (WORD)PhysInfoNum);
	BYTE CompensateParamCount = KartItem->CompensateParamCount;
	Write8(pKartPhysParam, CompensateParamCount); // CompensateParamCount
	for (size_t i = 0; i < CompensateParamCount; i++)
	{
		{ // CompensateParam
			BYTE *pCompensateParam = pKartPhysParam;
			Write16(pCompensateParam, 0); // len

			Write32(pCompensateParam, KartItem->CompensateParam[i].Percent);
			Write32(pCompensateParam, KartItem->CompensateParam[i].Value);

			len = pCompensateParam - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	{ // CollisionBoxParam
		BYTE *pCollisionBoxParam = pKartPhysParam;
		Write16(pCollisionBoxParam, 0); // len

		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.BoxLength);
		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.BoxWidth);
		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.BoxHeight);
		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.AdjustOffset);
		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.AdjustPickGap);
		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.AdjustRemoteOffset);
		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.AdjustBevelOffset);
		Write32(pCollisionBoxParam, KartItem->CollisionBoxParam.ControllerOffset);

		len = pCollisionBoxParam - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}

	{ // CarToCarCollisionParam
		BYTE *pCarToCarCollisionParam = pKartPhysParam;
		Write16(pCarToCarCollisionParam, 0); // len
		Write32(pCarToCarCollisionParam, KartItem->CarToCarCollisionParam.CollisionRadius);
		Write32(pCarToCarCollisionParam, KartItem->CarToCarCollisionParam.CollisionLength);
		Write32(pCarToCarCollisionParam, KartItem->CarToCarCollisionParam.CollisionWidth);
		Write32(pCarToCarCollisionParam, KartItem->CarToCarCollisionParam.CollisionHeight);
		Write32(pCarToCarCollisionParam, KartItem->CarToCarCollisionParam.CollisionLostCtrlTime);

		len = pCarToCarCollisionParam - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}
	// Write32(pKartPhysParam, 10000);//低速转向 add
	// Write32(pKartPhysParam, 10000);//高速转向 add
	Write32(pKartPhysParam, KartItem->BaseTurnRate);						   // 基本周转率
	Write32(pKartPhysParam, KartItem->VolatileTurnRate);					   // 不稳定周转率
	Write32(pKartPhysParam, KartItem->MaxTurnSpeed);						   // 最大转弯速度
	Write32(pKartPhysParam, KartItem->MinTurnSpeed);						   // 最小转弯速度
	Write32(pKartPhysParam, KartItem->MaxAccuTime);							   // 最大准确时间
	Write32(pKartPhysParam, KartItem->BaseAccuRate);						   // 基本准确率
	Write32(pKartPhysParam, KartItem->MaxAffectSpeed);						   // 最大影响速度
	Write32(pKartPhysParam, KartItem->Gravity + (10 * WWeight) ); // TODO: 平衡; 未限制最大值! 悬挂
	Write32(pKartPhysParam, KartItem->AdditionalLocalZSpeed - (10 * AdditionalZSpeedWeight)); // 附加本地速度// TODO: 平衡
	Write32(pKartPhysParam, KartItem->StartVec + DriftVecWeight);							   // 启动视频
	Write32(pKartPhysParam, KartItem->EndVecFist + DriftVecWeight);
	Write32(pKartPhysParam, KartItem->EndVecSecon + DriftVecWeight);
	Write32(pKartPhysParam, KartItem->DirKeyForce);
	Write32(pKartPhysParam, KartItem->DirKeyTwist);
	Write32(pKartPhysParam, KartItem->BannerTwist);
	Write32(pKartPhysParam, KartItem->BannerKeyTwist );
	Write32(pKartPhysParam, KartItem->BannerVecForce);
	Write32(pKartPhysParam, KartItem->BannerHeadForce);
	// Write32(pKartPhysParam, 10000);//摩擦值 add
	Write32(pKartPhysParam, KartItem->SlidFricForce + (9 * WWeight));
	Write32(pKartPhysParam, KartItem->RollFricForce + (3 * WWeight));
	Write32(pKartPhysParam, KartItem->StartWec);
	// Write32(pKartPhysParam, 10000);//转向助力 add
	Write32(pKartPhysParam, KartItem->MaxWec);
	Write32(pKartPhysParam, KartItem->SuaiJianTwist);
	Write32(pKartPhysParam, KartItem->DirUpKeyForce);

	BYTE AccelStatusCount = KartItem->AccelStatusCount;
	Write8(pKartPhysParam, AccelStatusCount); // AccelStatusCount
	for (size_t i = 0; i < AccelStatusCount; i++)
	{
		{ // m_astKartPhysParam[].m_aiAccelStatus[]
			Write32(pKartPhysParam, KartItem->AccelStatus[i]);
		}
	}

	BYTE ForwardAccelNum = KartItem->ForwardAccelNum;
	Write8(pKartPhysParam, ForwardAccelNum); // ForwardAccelNum
	for (size_t i = 0; i < ForwardAccelNum; i++)
	{
		{ // ForwardAccel
			BYTE *pForwardAccel = pKartPhysParam;
			Write16(pForwardAccel, 0); // len

			int Key = KartItem->ForwardAccel[i].Key;
			int Value = KartItem->ForwardAccel[i].Value;
			if (i >= 7 && i <= 9)
			{
				// TODO: 引擎; 未限制最大值!
				Value += 35 * SpeedWeight;
				Value += 8 * Speed2Weight; // 推进
			}
			Write32(pForwardAccel, Key);
			Write32(pForwardAccel, Value);

			len = pForwardAccel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	BYTE ForwardDecelNum = KartItem->ForwardDecelNum;
	Write8(pKartPhysParam, ForwardDecelNum); // ForwardDecelNum
	for (size_t i = 0; i < ForwardDecelNum; i++)
	{
		{ // ForwardDecel
			BYTE *pForwardDecel = pKartPhysParam;
			Write16(pForwardDecel, 0); // len

			Write32(pForwardDecel, KartItem->ForwardDecel[i].Key);
			Write32(pForwardDecel, KartItem->ForwardDecel[i].Value);

			len = pForwardDecel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	BYTE BackwardAccelNum = KartItem->BackwardAccelNum;
	Write8(pKartPhysParam, BackwardAccelNum); // BackwardAccelNum
	for (size_t i = 0; i < BackwardAccelNum; i++)
	{
		{ // BackwardAccel
			BYTE *pBackwardAccel = pKartPhysParam;
			Write16(pBackwardAccel, 0); // len

			Write32(pBackwardAccel, KartItem->BackwardAccel[i].Key);
			Write32(pBackwardAccel, KartItem->BackwardAccel[i].Value);

			len = pBackwardAccel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}

	BYTE BackwardDecelNum = KartItem->BackwardDecelNum;
	Write8(pKartPhysParam, BackwardDecelNum); // BackwardDecelNum
	for (size_t i = 0; i < BackwardDecelNum; i++)
	{
		{ // BackwardDecel
			BYTE *pBackwardDecel = pKartPhysParam;
			Write16(pBackwardDecel, 0); // len

			Write32(pBackwardDecel, KartItem->BackwardDecel[i].Key);
			Write32(pBackwardDecel, KartItem->BackwardDecel[i].Value);

			len = pBackwardDecel - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}
	Write32(pKartPhysParam, CrashCompensatePower + AntiCollisionWeight * 10); // CrashCompensatePower
	Write32(pKartPhysParam, 0);					   // DefenseRate
	Write32(pKartPhysParam, Uin);				   // Uin
	Write32(pKartPhysParam, 0xFFFFFFFF);					   // SpeedupCardGenRate
	//Write32(pKartPhysParam, 0);					   // ExtraFuncFlag
	UINT skillHide = 0;
	if (getGameConfig()["KartExMode"] && getGameConfig()["KartExMode"].as<UINT>() == 1 && GetClient(Uin) && GetClient(Uin)->GetSkillID(8))
	{
		skillHide = 4;
	}
	Write32(pKartPhysParam, 0xF8 | skillHide | 0 | 0);// skillhide | skilln2o | skillview
	if (GetItemGameType(loadID) == 2)
	{
		Write8(pKartPhysParam, 1); // HasSkatePara
		for (size_t i = 0; i < 1; i++)
		{ // hasSkatePara
			BYTE *phasSkatePara = pKartPhysParam;
			Write16(phasSkatePara, 0);												// len
			Write32(phasSkatePara, KartItem->SkatePara.TrackSpeed);					// TrackSpeed //轨道墙壁速度
			Write32(phasSkatePara, KartItem->SkatePara.SkateSpeedCoef_AirBigJet);	// SkateSpeedCoef_AirBigJet
			Write32(phasSkatePara, KartItem->SkatePara.SkateSpeedCoef_Jump);		// SkateSpeedCoef_Jump
			Write32(phasSkatePara, KartItem->SkatePara.SkateSpeedCoef_AirJetPoint); // SkateSpeedCoef_AirJetPoint
			Write32(phasSkatePara, KartItem->SkatePara.SkateSpeedCoef_MaxForce);	// SkateSpeedCoef_MaxForce
			Write32(phasSkatePara, KartItem->SkatePara.OntrackAccRef);				// OntrackAccRef //轨道集气
			Write32(phasSkatePara, KartItem->SkatePara.TrackAccRef);				// TrackAccRef // 墙壁
			Write32(phasSkatePara, KartItem->SkatePara.DashSkill);					// DashSkill

			len = phasSkatePara - pKartPhysParam;
			Set16(pKartPhysParam, (WORD)len);
			pKartPhysParam += len;
		}
	}
	else
	{
		Write8(pKartPhysParam, 0); // HasSkatePara
	}

	/*
m_astKartPhysParam[].m_hasSkatePara[].m_iTrackSpeed
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_AirBigJet
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_Jump
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_AirJetPoint
m_astKartPhysParam[].m_hasSkatePara[].m_iSkateSpeedCoef_MaxForce
m_astKartPhysParam[].m_hasSkatePara[].m_iOntrackAccRef
m_astKartPhysParam[].m_hasSkatePara[].m_iTrackAccRef
m_astKartPhysParam[].m_hasSkatePara[].m_iDashSkill
		*/

	Write8(pKartPhysParam, 0); // HasBattleModeSkillInfoList
	for (size_t i = 0; i < 0; i++)
	{ // BattleModeSkillInfoList
		BYTE *pBattleModeSkillInfoList = pKartPhysParam;
		Write16(pBattleModeSkillInfoList, 0); // len

		Write8(pBattleModeSkillInfoList, 0); // SkillNum
		for (size_t i = 0; i < 0; i++)
		{ // SkillInfo
			BYTE *pSkillInfo = pBattleModeSkillInfoList;
			Write16(pSkillInfo, 0); // len

			Write16(pSkillInfo, 0); // SkillId
			Write8(pSkillInfo, 0);	// SkillLevel

			len = pSkillInfo - pBattleModeSkillInfoList;
			Set16(pBattleModeSkillInfoList, (WORD)len);
			pBattleModeSkillInfoList += len;
		}

		len = pBattleModeSkillInfoList - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}
	Write32(pKartPhysParam, 200); // EnergyConvert
	Write16(pKartPhysParam, 0); // SuperN2ORate
	Write16(pKartPhysParam, AntiCollisionWeight*5); // CollisionLoseRate 防护
	Write8(pKartPhysParam, 0);	// NotInterruptDrift

	Write8(pKartPhysParam, 0); // AddWeightNum

	Write8(pKartPhysParam, 0); // WeightNum

	Write8(pKartPhysParam, 0); // HasAntiCollisionCfg
	for (size_t i = 0; i < 0; i++)
	{ // AntiCollistionCfg
		BYTE *pAntiCollistionCfg = pKartPhysParam;
		Write16(pAntiCollistionCfg, 0); // len

		Write8(pAntiCollistionCfg, 0);	// Num
		Write32(pAntiCollistionCfg, 0); // Position[]
		Write32(pAntiCollistionCfg, 0); // Position[]
		Write32(pAntiCollistionCfg, 0); // Position[]

		len = pAntiCollistionCfg - pKartPhysParam;
		Set16(pKartPhysParam, (WORD)len);
		pKartPhysParam += len;
	}
	loadKartBoatInfo(pKartPhysParam);
	

	len = pKartPhysParam - p;
	Set16(p, len);
	p += len;
}
void NotifyUpdateKartPhysparam(ClientNode *Client, UINT KartID)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;
	KartItem *KartItem = GetKartItem(KartID);
	BYTE PhysInfoNum = KartItem->PhysInfoCount;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write32(p, KartID);		 // KartID
	Write8(p, PhysInfoNum);	 // PhysInfoCount
	for (size_t i = 0; i < PhysInfoNum; i++)
	{
		int PID = KartItem->PhysInfo[i].PID;
		int AffectID = KartItem->PhysInfo[i].AffectID / 10;
		int Param = KartItem->PhysInfo[i].Param / 10;
		int LifeTime = KartItem->PhysInfo[i].LifeTime / 10;
		BYTE *pPhysInfo = p;
		Write16(pPhysInfo, 0); // len

		Write32(pPhysInfo, PID);	  // PID
		Write32(pPhysInfo, AffectID); // AffectID
		Write32(pPhysInfo, Param);	  // Param
		Write32(pPhysInfo, LifeTime); // LifeTime

		len = pPhysInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	// 返回值未知
	// SendToClient(Client, 24451, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void WINAPI OnBegin(RoomNode *Room)
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

void WINAPI OnCountDown3(RoomNode *Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient)
		{
			//RoomClient = GetClient(RoomClient->Uin);
			if (RoomClient->StartGameTime == false) {
				continue;
			}
			NotifyGenGameEndBoxAward(RoomClient);
			if (Room->SceneID == 400)
			{
				NotifyChatGameOver(RoomClient, Room->BaseGameMode, Room->SubGameMode);
				NotifyGameOver(RoomClient, 0, 0, NULL, 0);
				RequestEnterOutdoor2(RoomClient, Room->ID, 4);
			}
			else if(RoomClient->matchQueueType>=1)
			{
				RoomClient->matchQueueType = 0;
				NotifyGameOver(RoomClient, 1, 0, NULL, 0);
			}
			else
			{
				NotifyGameOver(RoomClient, 0, 0, NULL, 0);
			}

			/*YAML::Node OverItem = YAML::LoadFile(".\\Gift\\overItem.yml");
			if (OverItem["ON"].as<UINT>()==1) {
				int randomID;
				{
					std::random_device rd;
					std::mt19937 eng(rd());
					std::uniform_int_distribution<int> dis(1, 100);
					randomID = dis(eng);
				}
				if (randomID<= OverItem["Rate"].as<UINT>()) {
					YAML::Node ItemList = OverItem["Items"];
					int randomID;
					std::random_device rd;
					std::mt19937 eng(rd());
					std::uniform_int_distribution<int> dis(1, ItemList.size());
					randomID = dis(eng);
					ItemInfo Items[3000];
					ItemInfo Item = PlayerDB_AddItem(RoomClient->Uin, ItemList[randomID-1]["ItemId"].as<INT>(), ItemList[randomID - 1]["ItemNum"].as<INT>(), ItemList[randomID - 1]["AvailPeriod"].as<INT>(), false);
					Items[0].ItemID = ItemList[randomID - 1]["ItemId"].as<INT>();
					Items[0].ItemNum = Item.ItemNum;
					Items[0].AvailPeriod = Item.AvailPeriod;
					Items[0].Status = Item.Status;
					Items[0].ObtainTime = Item.ObtainTime;
					Items[0].OtherAttribute = Item.OtherAttribute;
					Items[0].ItemType = Item.ItemType;
					NotifyClientAddItem(RoomClient, 1, Items);

					ResponseStartGame(RoomClient, "恭喜你在本局比赛掉落道具");
				}

			}*/
		}
	}
}
void WINAPI OnCountDown2(RoomNode *Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient)
		{
			//Room->PlayerReal[i] = getBakClient(RoomClient);
			//NotifyRaceOver(RoomClient, Room);
			NotifyRaceOverNew(RoomClient);
		}
	}
	// 经验结算等待时间 day 20220419
	INT wait = getGameConfig()["ResultWait"].as<INT>();
	CreateRoomTimer(Room, wait, OnCountDown3);
}
void WINAPI OnCountDown(RoomNode *Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient)
		{
			NotifyRaceShow(RoomClient);
		}
	}
	CreateRoomTimer(Room, 5000, OnCountDown2);
}
bool OpenSkill(ClientNode* Client,UINT SkillID) {
	if (SkillID == 0 || SkillID == 3 || SkillID == 4 || SkillID == 8 || SkillID == 1 || SkillID == 2|| SkillID == 6 || SkillID == 7) {
		return true;
	}
	else if (Client->SkillStoneList[0] == 0 && Client->SkillStoneList[1] == 0 && Client->SkillStoneList[2] == 0 && Client->SkillStoneList[3] == 0 && Client->SkillStoneList[4] == 0) {
		return true;
	}
	else {
		return false;
	}
}
void ClientNode::SetSkillID()
{

	
		if (getGameConfig()["KartExMode"] && getGameConfig()["KartExMode"].as<UINT>() == 1) {
			if (this->KartID == this->OrgKartId && this->SkinID > 0)
			{
				if (KartSkillConfig[this->SkinID])
				{
					this->KartSkillID = KartSkillConfig[this->SkinID].as<UINT>();
				}
				else if (KartSkillConfig[this->KartID])
				{
					this->KartSkillID = KartSkillConfig[this->KartID].as<UINT>();
				}
			}
			else
			{
				if (KartSkillConfig[this->KartID])
				{
					this->KartSkillID = KartSkillConfig[this->KartID].as<UINT>();
				}
			}
			if (KartSkillConfig[this->Pet->ID]) {
				this->PetSkillID = KartSkillConfig[this->Pet->ID].as<UINT>();
			}
	
	}
}
bool ClientNode::GetSkillID(UINT SkillID)
{
	if (this->IsFinish == true) {
		return false;
	}
	if (OpenSkill(this,SkillID)) {
		if (SkillID > 0) {
			if (SkillID == 7 && this->KartSkillID != 2 && this->KartSkillID != 6) {
				if (this->KartSkillID == SkillID) {
					return true;
				}
				else if (this->PetSkillID == SkillID) {
					return true;
				}
				else {
					return false;
				}
			}
			else if (SkillID == 8) {
				if (this->KartSkillID == SkillID) {
					return true;
				}
				else if (this->PetSkillID == SkillID) {
					return true;
				}
				else {
					return false;
				}
			}
			else {
				if (this->KartSkillID == SkillID) {
					return true;
				}
				else if (this->PetSkillID == SkillID && this->KartSkillID == 0) {
					return true;
				}
				else {
					return false;
				}
			}

		}
		else {
			if (this->KartSkillID > 0 || this->PetSkillID > 0) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}
void NotifyGameBegin(ClientNode* Client, ClientNode* Player[])
{


	BYTE buf[8192] = { 0 };
	BYTE* p = buf;
	size_t len = 0;
	ClientNode* Players[6] = { 0 };

	char CurrentPlayers = 0;
	if (Player)
	{
		for (char i = 0; i < 6; i++)
		{
			if (Player[i])
			{
				Players[CurrentPlayers] = Player[i];
				CurrentPlayers++;
			}
		}
	}
	else
	{
		Players[0] = Client;
		CurrentPlayers = 1;
	}

	Write32(p, 20000);		   // NormalLoadTime
	Write32(p, Client->MapID); // MapID
	// MapHash[]
	memset(p, 0, 32);
	p += 32;

	Write32(p, Client->GameID);		  // GameID
	Write32(p, 0x77af7a30); // Seed

	Write8(p, CurrentPlayers);
	// m_aunRaceTrackOrders[MAX_MUCH_SEATNUMINROOM]
	int* aRaceTrackOrders = (int*)p;
	for (char i = 0; i < MAX_MUCH_SEATNUMINROOM; i++)
	{
		Write32(p, 0);
	}
	// 起跑线并排的玩家
	int index[] = { 2, 3, 1, 4, 0, 5 };
	for (char i = 0; i < CurrentPlayers; i++)
	{
		Set32((BYTE*)&aRaceTrackOrders[index[i]], Players[i]->Uin);
	}
	//Set32((BYTE*)&aRaceTrackOrders[index[CurrentPlayers]], 1);
	Write16(p, Client->TotalRound);								   // TotalMapRound
	Write32(p, 0);												   // PropUpdateInterval
	Write32(p, PlayerDB_GetMapRecord(Client->Uin, Client->MapID)); // Record

	Write8(p, 0); // NianShouTypeNum

	UINT BUFFID = 0;
	UINT BHID = Client->KartID;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	sql = "SELECT ItemID  FROM ITEM WHERE Uin=? AND (ItemID=19753 or ItemID=18554) AND Status = 1";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			BUFFID = sqlite3_column_int(stmt, 0);

		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	sql = "SELECT ItemID  FROM ITEM WHERE Uin=? AND ItemID=19755 AND Status = 1";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		if (sqlite3_step(stmt) == SQLITE_ROW)
		{
			int randomID;
			std::random_device rd;
			std::mt19937 eng(rd());
			std::uniform_int_distribution<int> dis(1, KartAltasList.size());
			randomID = dis(eng);
			BHID = KartAltasList[randomID]->ID;

		}
	}
	else
	{
		Z_ERROR("%s\n", sqlite3_errmsg(PlayerDB));
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	Write8(p, CurrentPlayers); // SyncCarNum
	if (getGameConfig()["Global"]["SkillStoneUin"])
	{
		INT Uin = getGameConfig()["Global"]["SkillStoneUin"].as<INT>();
		if (Uin == -1)
		{
			NotifyUpdateKartSkillStoneInfo3(Client, Client->KartID, Client->KartID);
		}
		else if (Client->Uin == Uin)
		{
			NotifyUpdateKartSkillStoneInfo3(Client, Client->KartID, Client->KartID);
		}
		else {
			NotifyUpdateKartSkillStoneInfo4(Client, Client->KartID, Client->KartID, BUFFID);
		}
	}
	else {
		NotifyUpdateKartSkillStoneInfo4(Client, Client->KartID, Client->KartID, BUFFID);
	}
	Client->SetSkillID();
	for (char i = 0; i < CurrentPlayers; i++)
	{ // SyncCar
		BYTE* pSyncCar = p;
		Write16(pSyncCar, 0); // len
		// 新增判断皮肤
		Write32(pSyncCar, Players[i]->Uin); // PlayerUin
		if (Players[i]==Client) {
			Write32(pSyncCar, BHID);
		}
		else {
			Write32(pSyncCar, Players[i]->KartID);
		}
		
		Write8(pSyncCar, 0); // HasStoneInfo

		len = pSyncCar - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 1); // ReportDataFlag

	Write8(p, 5); // CheckDataNum
	int BeginIdx[] = { 7,19,30,38,41 };
	int EndIdx[] = { 16,27,37,40,44 };
	for (size_t i = 0; i < 5; i++)
	{ // MapCheckDiv
		BYTE* pMapCheckDiv = p;
		Write16(pMapCheckDiv, 0); // len

		Write16(pMapCheckDiv, BeginIdx[i]);  // BeginIdx
		Write16(pMapCheckDiv, EndIdx[i]); // EndIdx

		len = pMapCheckDiv - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // P2PMode
	Write8(p, 0); // TcpFrequence

	Write8(p, 50); // MultiInfoLen
	memcpy(p, "00000001649ef5ad0402b404010101649ef5ad222d20b0024275e80fa8a59dbd3e0355b936ffdb65c783a302db7ec18b724a", 50);
	p += 50;

	Write8(p, 1); // FeedBackEnabled

	Write8(p, 8); // SpeedSectionNum
	Write16(p, 0x0124);
	Write16(p, 0x012e);
	Write16(p, 0x0138);
	Write16(p, 0x0142);
	Write16(p, 0x014c);
	Write16(p, 0x0156);
	Write16(p, 0x0160);
	Write16(p, 0x016f);

	Write8(p, 8); // NormalSpeedSectionNum
	Write16(p, 0x00c8);    //NormalSpeedSectionLowerBound[]);
	Write16(p, 0x00dc);
	Write16(p, 0x00f0);
	Write16(p, 0x0104);
	Write16(p, 0x0118);
	Write16(p, 0x012c);
	Write16(p, 0x0140);
	Write16(p, 0x0154);

	Write8(p, 0); // MemCheckInfoNum

	Write8(p, 4);	 // ExtraInfoInterval
	Write16(p, -1);	 // OffsetThreshold
	Write32(p, 200); // SpeedRatioThreshold1
	Write32(p, 200); // SpeedRatioThreshold2
	Write32(p, 1000);	 // HideTaskId
	Write8(p, 1);	 // HideTaskType
	Write32(p, 0);	 // HideTaskParam1

	Write8(p, 0); // ForceReportCPNum
	// m_au16ForceReportCP[]

	Write8(p, 0); // CliReserveFlag

	char EnableAntiDriftCheat = 1; // 反卡漂
	Write8(p, EnableAntiDriftCheat);  // EnableAntiDriftCheat
	if (EnableAntiDriftCheat)
	{ // AntiDriftCheatPara
		BYTE* pAntiDriftCheatPara = p;
		Write16(pAntiDriftCheatPara, 0); // len

		Write32(pAntiDriftCheatPara, 0);   // MaxDriftHistoryTime
		Write32(pAntiDriftCheatPara, 0);   // MinTimeInterval
		Write32(pAntiDriftCheatPara, 0);   // MaxTimeInterval
		Write32(pAntiDriftCheatPara, 220); // NormalThreshold
		Write32(pAntiDriftCheatPara, 15);  // JetThreshold
		Write32(pAntiDriftCheatPara, 0);   // JetInterval
		Write32(pAntiDriftCheatPara, 0);   // OneSideSlidingInterval

		len = pAntiDriftCheatPara - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // HasCrashModePara

	Write32(p, 0); // FizzStarTaskId
	Write8(p, 0);  // FizzStarTaskType
	Write32(p, 0); // FizzStarTaskParam1
	Write32(p, 0); // LDMRecordID
	Write32(p, 0x00884301); // GameSeq

	Write8(p, CurrentPlayers); // PlayerNums
	for (size_t i = 0; i < CurrentPlayers; i++)
	{ // GameBeginPlayerInfo
		BYTE* pGameBeginPlayerInfo = p;
		Write16(pGameBeginPlayerInfo, 0); // len

		Write32(pGameBeginPlayerInfo, Players[i]->Uin); // Uin
		if (Players[i]->Uin == Client->Uin)
		{
			Write32(pGameBeginPlayerInfo, 77308);
		}
		else
		{
			Write32(pGameBeginPlayerInfo, 0);// ChumCircleID
		}
		Write8(pGameBeginPlayerInfo, 0); // SkillNums
		Write32(pGameBeginPlayerInfo, 0); // WorldEscapeTaskID
		{ // ProfessionLicenseInfo
			BYTE* pProfessionLicenseInfo = pGameBeginPlayerInfo;
			Write16(pProfessionLicenseInfo, 0); // len

			Write8(pProfessionLicenseInfo, 6); // BigLevel
			Write8(pProfessionLicenseInfo, 5); // SmallLevel

			len = pProfessionLicenseInfo - pGameBeginPlayerInfo;
			Set16(pGameBeginPlayerInfo, (WORD)len);
			pGameBeginPlayerInfo += len;
		}
		Write8(pGameBeginPlayerInfo, 0); // ParaNum

		Write32(pGameBeginPlayerInfo, 0); // StarsFightingStar
		Write8(pGameBeginPlayerInfo, 0);  // ParaNum
		Write32(pGameBeginPlayerInfo, 0); // ParaNum[]

		len = pGameBeginPlayerInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // PlayeCheerNums

	Write8(p, 0); // MapNum

	Write32(p, 0); // SpecialMapId
	RoomNode* TempRoom = GetRoom(Client->RoomID);
	if (!TempRoom) {
		TempRoom = GetRoom(Client->BorderRoomID);
	}
	if (TempRoom && TempRoom->OpenNpc==true) {
		
		Write8(p, TempRoom->NPCNum);//NPC
		typedef struct NORMARSPEED {
			//MaxSpeed
			//MinSpeed
			//MaxPlusForce
			//MinPlusForce
			//MaxTurnSpeed
			//MinTurnSpeed
			//MaxDist
			//MinDist
			UINT MaxSpeed;
			UINT MinSpeed;
			UINT MaxPlusForce;
			UINT MinPlusForce;
			UINT MaxTurnSpeed;
			UINT MinTurnSpeed;
			UINT MaxDist;
			UINT MinDist;


		};

		const static NORMARSPEED Normspeed[] = {
			// ItemWeightLevel
			// MinWeight
			// MaxWeight
			// ExpAward
			{55, 10, 5, 0, 5, 1, 99999, 200},
			{60, 20, 10, 0, 5, 1, 200, 0},
			{70, 20, 10, 5, 5, 1, 0, -100},
			{80, 25, 20, 10, 5, 1, -100, -99999}

		};

		typedef struct GPNUM {
			//GpID
			//Probability
			//MaxDist
			//MinDist
			UINT GpID;
			UINT Probability;
			UINT MaxDist;
			UINT MinDist;

		};
		const static GPNUM GpNum[] = {
			{8, 0, 999990, 9999},
			{10, 8000, 999, 20},
			{1, 5000, 20, -20},
			{2, 5000, -20, -100}

		};
		
		
		for (size_t i = 0; i < TempRoom->NPCNum; i++)
		{ //NPCInfo
			if (TempRoom->NPCPlayer[i] == NULL) {
				NpcBaseInfo* Npc = RoomNcpInfo();
				Npc->NpcID = i + 1;
				int randomID;
				std::random_device rd;
				std::mt19937 eng(rd());
				std::uniform_int_distribution<int> dis(Npc->MinLevel, Npc->MaxLevel);
				randomID = dis(eng);
				Npc->Level = randomID;
				TempRoom->NPCPlayer[i] = Npc;
			}
			BYTE* pNPCInfo = p;
			Write16(pNPCInfo, 0); //len
			Write32(pNPCInfo, TempRoom->NPCPlayer[i]->NpcID); //NpcID
			memcpy(pNPCInfo, TempRoom->NPCPlayer[i]->RoomName.c_str(), 17); // RoomName
			pNPCInfo += 17;
			Write32(pNPCInfo, 0);  //NPC车队
			Write32(pNPCInfo, 0);
			Write32(pNPCInfo, 0);
			Write32(pNPCInfo, 0);
			Write8(pNPCInfo, 0);
			{ //NpcItemInfo
				BYTE* pNpcItemInfo = pNPCInfo;
				Write16(pNpcItemInfo, 0); //len

				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->RoleID); //RoleID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->HairID); //HairID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->FaceID); //FaceID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->GlassID); //GlassID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->CoatID); //CoatID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->GloveID); //GloveID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->TrousersID); //TrousersID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->PersonarID); //PersonarID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->TattooID); //TattooID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->WingID); //WingID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->DecoratorID); //DecoratorID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->KartID); //KartID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->ColorID); //ColorID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->TrackID); //TrackID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->EffectID); //EffectID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->BrandID); //BrandID
				Write32(pNpcItemInfo, TempRoom->NPCPlayer[i]->TailID); //TailID

				len = pNpcItemInfo - pNPCInfo;
				Set16(pNPCInfo, (WORD)len);
				pNPCInfo += len;
			}
			Write32(pNPCInfo, 0); //GpSkillPeriod
			Write32(pNPCInfo, 10000); //BaseValue
			{ //DefaultSpeedPara
				BYTE* pDefaultSpeedPara = pNPCInfo;
				Write16(pDefaultSpeedPara, 0); //len

				Write32(pDefaultSpeedPara, 1250); //MaxSpeed
				Write32(pDefaultSpeedPara, 600); //MinSpeed
				Write32(pDefaultSpeedPara, 1000); //MaxPlusForce
				Write32(pDefaultSpeedPara, 600); //MinPlusForce
				Write32(pDefaultSpeedPara, 1000); //MaxTurnSpeed
				Write32(pDefaultSpeedPara, 600); //MinTurnSpeed
				Write32(pDefaultSpeedPara, 1000); //MaxDist
				Write32(pDefaultSpeedPara, 600); //MinDist
				len = pDefaultSpeedPara - pNPCInfo;
				Set16(pNPCInfo, (WORD)len);
				pNPCInfo += len;
			}
			Write8(pNPCInfo, sizeof(Normspeed) / sizeof(Normspeed[0]));
			//Write8(pNPCInfo, 0);//NormalSpeedNum
			for (size_t i = 0; i < sizeof(Normspeed) / sizeof(Normspeed[0]); i++)
			{ //NormalSpeedPara
				BYTE* pNormalSpeedPara = pNPCInfo;
				Write16(pNormalSpeedPara, 0); //len

				Write32(pNormalSpeedPara, Normspeed[i].MaxSpeed); //MaxSpeed
				Write32(pNormalSpeedPara, Normspeed[i].MinSpeed); //MinSpeed
				Write32(pNormalSpeedPara, Normspeed[i].MinPlusForce); //MaxPlusForce
				Write32(pNormalSpeedPara, Normspeed[i].MinPlusForce); //MinPlusForce
				Write32(pNormalSpeedPara, Normspeed[i].MaxTurnSpeed); //MaxTurnSpeed
				Write32(pNormalSpeedPara, Normspeed[i].MinTurnSpeed); //MinTurnSpeed
				Write32(pNormalSpeedPara, Normspeed[i].MaxDist); //MaxDist
				Write32(pNormalSpeedPara, Normspeed[i].MinDist); //MinDist

				len = pNormalSpeedPara - pNPCInfo;
				Set16(pNPCInfo, (WORD)len);
				pNPCInfo += len;
			}
			Write8(pNPCInfo, sizeof(GpNum) / sizeof(GpNum[0])); //GpNum
			//Write8(pNPCInfo, 0);
			for (size_t i = 0; i < sizeof(GpNum) / sizeof(GpNum[0]); i++)
			{ //GpPara
				BYTE* pGpPara = pNPCInfo;
				Write16(pGpPara, 0); //len

				Write32(pGpPara, GpNum[i].GpID); //GpID
				Write32(pGpPara, GpNum[i].Probability); //Probability
				Write32(pGpPara, GpNum[i].MaxDist); //MaxDist
				Write32(pGpPara, GpNum[i].MinDist); //MinDist

				len = pGpPara - pNPCInfo;
				Set16(pNPCInfo, (WORD)len);
				pNPCInfo += len;
			}
			Write16(pNPCInfo, TempRoom->NPCPlayer[i]->Level); //NPCAILevel
			Write16(pNPCInfo, 0); //TeamID

			len = pNPCInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

	}
	else {
		Write8(p, 0);//NPC
	}
	Write8(p, 0); // MapNum
	if (TempRoom && TempRoom->OpenNpc == true) {
		for (size_t i = 0; i < 20;i++) {
			if (TempRoom->NPCNum>=i+1) {
				Write32(p, i+1);
			}
			else {
				Write32(p, 0);
			}
		}
		


	}
	else {
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);
		Write32(p, 0);

	}
	Write8(p, 0); // PlayerNum


	int k = 0;
	Write8(p, 0); // HasTowerInfo


	Write8(p, 0); // HasWeRelayGameBeginInfo

	Write8(p, 0);		  // ChangeCar
	Write32(p, 0x0124b17e); // GameSeqIDHigh
	Write32(p, 2938796198); // GameSeqIDLow
	Write32(p, 0);		  // KubiBigCoinReplaceItem
	Write32(p, 0);		  // TimerChallengeJumpLevel
	Write8(p, 0);		  // ShadowRunDelay
	Write16(p, 0);		  // ShadowCatchUpContinuesTime

	Write8(p, 0); // ArrestPlayerNums


	Write16(p, 0x000a); // MonitorCheckPointBegin
	Write16(p, 0x000b); // MonitorCheckPointEnd

	Write8(p, 1);  // MonitorLapCnt
	Write8(p, 0);  // GameType
	Write8(p, 0);  // PointID
	Write16(p, 0); // BaseMode
	Write16(p, 0); // SubMode
	Write8(p, 0);  // GameType
	Write16(p, 0); // ReportPosSpan

	Write16(p, 0); // PropID
	Write32(p, 0); // PropIndex
	Write16(p, 0); // PropNum

	Write8(p, 0); // BaseGameModeEx
	Write8(p, 0); // ParaNum
	Write16(p, 0); // ParaNum[]

	Write8(p, 0); // MapCheckpointFileIndex

	Write8(p, 0); // HasTimerChallenge2ndGameBeginInfo
	/*
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_iPlayerNum
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_uiUin
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iJumplevel
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iWeekHighestTongGuanNum
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iBonusJumplevel
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_astPlayerJumpInfo[].m_iHistoryMaxTongGuanNum
m_astTimerChallenge2ndGameBeginInfo[].m_stJumpInfo.m_iJumpLevel
m_astTimerChallenge2ndGameBeginInfo[].m_uiBeginUin
m_astTimerChallenge2ndGameBeginInfo[].m_bReverse
	*/

	Write8(p, 0); // HasGameStageInfo
	/*
m_stGameStageInfo[].m_u8StageIndex
m_stGameStageInfo[].m_u8TotalStageNum
	*/
	Write8(p, 0); // CarCollectInfoNum


	Write16(p, 2560); // ReportAntiCollisionDataTime
	Write32(p, 0);	   // Duration
	Write32(p, 0);	   // BeginCDTime
	Write32(p, 0);	   // PropInteval
	Write32(p, 0x00000f00);	   // MoveFreq

	Write8(p, 0); // HaveGameLogicTask


	Write16(p, 0); // RankedMatchBegainTipInfoType

	Write8(p, 0); // BegainTipLen

	Write8(p, 0); // BuffBum


	Write8(p, 0); // MapChallengeInfoNum


	Write8(p, 0); // IsChangeCar

	Write8(p, 0); // AnonymousMode

	Write8(p, 0); // IsTimeShuttleGen

	Write8(p, 0); // HalloweenDdventureInfoNum


	Write8(p, 0); // MaxPropEffectNum
	Write8(p, 0); // IsCrossNoCollision

	Write8(p, 3); // RecordCheckCondNum
	{ //RecordCheckConds
		BYTE* pRecordCheckConds = p;
		Write16(pRecordCheckConds, 0); //len

		Write16(pRecordCheckConds, 0); //CheckRelation
		Write8(pRecordCheckConds, 2); //ConditionNum
		for (size_t i = 0; i < 1; i++)
		{ //CheckConditions
			BYTE* pCheckConditions = pRecordCheckConds;
			Write16(pCheckConditions, 0); //len

			Write16(pCheckConditions, 0); //CheckType
			Write8(pCheckConditions, 5); //CheckParaNum
			Write32(pCheckConditions, 0x00000258); //IntParas[]
			Write32(pCheckConditions, 0xffffffff); //IntParas[]
			Write32(pCheckConditions, 0xffffffff); //IntParas[]
			Write32(pCheckConditions, 0xffffffff); //IntParas[]
			Write32(pCheckConditions, 0x00000bb8); //IntParas[]

			len = pCheckConditions - pRecordCheckConds;
			Set16(pRecordCheckConds, (WORD)len);
			pRecordCheckConds += len;
		}
		for (size_t i = 0; i < 1; i++)
		{ //CheckConditions
			BYTE* pCheckConditions = pRecordCheckConds;
			Write16(pCheckConditions, 0); //len

			Write16(pCheckConditions, 0); //CheckType
			Write8(pCheckConditions, 5); //CheckParaNum
			Write32(pCheckConditions, 0x0000012c); //IntParas[]
			Write32(pCheckConditions, 0xffffffff); //IntParas[]
			Write32(pCheckConditions, 0x00000000); //IntParas[]
			Write32(pCheckConditions, 0x000003e8); //IntParas[]
			Write32(pCheckConditions, 0x000003e8); //IntParas[]

			len = pCheckConditions - pRecordCheckConds;
			Set16(pRecordCheckConds, (WORD)len);
			pRecordCheckConds += len;
		}
		len = pRecordCheckConds - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //RecordCheckConds
		BYTE* pRecordCheckConds = p;
		Write16(pRecordCheckConds, 0); //len

		Write16(pRecordCheckConds, 1); //CheckRelation
		Write8(pRecordCheckConds, 2); //ConditionNum
		for (size_t i = 0; i < 1; i++)
		{ //CheckConditions
			BYTE* pCheckConditions = pRecordCheckConds;
			Write16(pCheckConditions, 0); //len

			Write16(pCheckConditions, 1); //CheckType
			Write8(pCheckConditions, 5); //CheckParaNum
			Write32(pCheckConditions, 0); //IntParas[]
			Write32(pCheckConditions, 0x00000032); //IntParas[]
			Write32(pCheckConditions, 0xffffffff); //IntParas[]
			Write32(pCheckConditions, 0xffffffff); //IntParas[]
			Write32(pCheckConditions, 0); //IntParas[]

			len = pCheckConditions - pRecordCheckConds;
			Set16(pRecordCheckConds, (WORD)len);
			pRecordCheckConds += len;
		}
		for (size_t i = 0; i < 1; i++)
		{ //CheckConditions
			BYTE* pCheckConditions = pRecordCheckConds;
			Write16(pCheckConditions, 0); //len

			Write16(pCheckConditions, 1); //CheckType
			Write8(pCheckConditions, 6); //CheckParaNum
			Write32(pCheckConditions, 0);
			Write32(pCheckConditions, 0x00009c40);
			Write32(pCheckConditions, 0xffffffff);
			Write32(pCheckConditions, 0xffffffff);
			Write32(pCheckConditions, 0x00002710);
			Write32(pCheckConditions, 0);
			len = pCheckConditions - pRecordCheckConds;
			Set16(pRecordCheckConds, (WORD)len);
			pRecordCheckConds += len;
		}
		len = pRecordCheckConds - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //RecordCheckConds
		BYTE* pRecordCheckConds = p;
		Write16(pRecordCheckConds, 0); //len

		Write16(pRecordCheckConds, 0); //CheckRelation
		Write8(pRecordCheckConds, 1); //ConditionNum
		for (size_t i = 0; i < 1; i++)
		{ //CheckConditions
			BYTE* pCheckConditions = pRecordCheckConds;
			Write16(pCheckConditions, 0); //len

			Write16(pCheckConditions, 2); //CheckType
			Write8(pCheckConditions, 0); //CheckParaNum

			len = pCheckConditions - pRecordCheckConds;
			Set16(pRecordCheckConds, (WORD)len);
			pRecordCheckConds += len;
		}

		len = pRecordCheckConds - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0x000007b3); // GameBeginSwitchFlag
	Write8(p, 0);  // TriggerVeggieDogTask
	Write8(p, 0); // HasQSpeedCrystalInfo
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write16(p, 0);
	Write8(p, 0);


	len = p - buf;
	SendToClient(Client, 509, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyGameNPCInfo(ClientNode* Client)
{
	RoomNode* TempRoom = GetRoom(Client->RoomID);
	if (!TempRoom) {
		TempRoom = GetRoom(Client->BorderRoomID);
	}
	if (TempRoom && TempRoom->NPCNum>0&& TempRoom->OpenNpc==true) {
		BYTE buf[8192]; BYTE* p = buf; size_t len;

		Write32(p, Client->Uin); //HostUin
		Write8(p, TempRoom->NPCNum); //NPCNum
		for (size_t i = 0; i < TempRoom->NPCNum; i++)
		{ //NPCPos
			BYTE* pNPCPos = p;
			Write16(pNPCPos, 0); //len

			Write32(pNPCPos, i+1); //NpcID
			Write16(pNPCPos, 0); //StartPos
			Write16(pNPCPos, TempRoom->NPCPlayer[i]->Level); //NPCAILevel

			len = pNPCPos - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 0); //StageIndex

		len = p - buf;
		SendToClient(Client, 784, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	}

	
}
void NotifyKartPhysParam(ClientNode *Client, ClientNode *Player[])
{
	ClientNode *Players[6] = {nullptr};
	char CurrentPlayers = 0;
	if (Player)
	{
		for (char i = 0; i < 6; i++)
		{
			if (Player[i])
			{
				Players[CurrentPlayers] = Player[i];
				CurrentPlayers++;
			}
		}
	}
	else
	{
		Players[0] = Client;
		CurrentPlayers = 1;
	}

	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, CurrentPlayers); // KartNum
	for (char i = 0; i < CurrentPlayers; i++)
	{ // KartPhysParam
		try
		{
			ClientNode *Temp = Players[i];
			if (Temp->SkinID > 0 && Temp->KartID == Temp->OrgKartId)
			{
				WriteKartPhysParam(p, Temp->Uin, Temp->KartID, Temp->Pet->ID, Temp->Pet->Level, Temp->SkinID);
			}
			else
			{
				WriteKartPhysParam(p, Temp->Uin, Temp->KartID, Temp->Pet->ID, Temp->Pet->Level, 0);
			}
			// WriteKartPhysParam(p, Players[i]->Uin, Players[i]->KartID, Players[i]->Pet->ID, Players[i]->Pet->Level);
		}
		catch (const std::exception &)
		{
			return;
		}
	}

	Write16(p, 65535); // SuperN2ORate

	len = p - buf;
	SendToClient(Client, 551, buf, len, Client->GameID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestPrepareReady(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
#ifndef ZingSpeed
	// int MapID = Read32(Body);
	// char MapHash[32] = ReadString(Body);
	// UCHAR SingleGameRestartFlag = Read8(Body);
#endif

	if (Client->RoomID)
	{
		Client->IsReady = TRUE;
		RoomNode *Room = GetRoom(Client->RoomID);
		if (!Room)
		{
			return;
		}
		BOOL All = TRUE;
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient)
			{
				if (RoomClient->IsReady == FALSE)
				{
					All = FALSE;
					break;
				}
			}
		}
		// 跳舞时间 day 20220419

		INT wait = getGameConfig()["DanceWait"].as<INT>();
		if (All || Room->SceneID == 400)
		{
			CreateRoomTimer(Room, wait, OnBegin);
		}
	}
}
void RequestPrepareReadyMath(ClientNode *Client)
{
	UINT Uin = Client->Uin;
	UINT Time = 0;
#ifndef ZingSpeed
	// int MapID = Read32(Body);
	// char MapHash[32] = ReadString(Body);
	// UCHAR SingleGameRestartFlag = Read8(Body);
#endif

	if (Client->RoomID)
	{
		RoomNode *Room = GetRoom(Client->RoomID);
		if (!Room || Room->BaseGameMode==19)
		{
			return;
		}
		for (char i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
		}
		// 跳舞时间 day 20220419
		INT wait = ServerConfig["Server"]["DanceWait"].as<INT>();
		CreateRoomTimer(Room, wait, OnBegin);
	}
}
void NotifyRaceBegin(ClientNode *Client)
{
	if (Client->isRaceBegin == true)
	{
		return;
	}
	Client->isRaceBegin = true;
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, 9000); // CountDownTime
	Write16(p, 0);	  // DelayTime
#ifndef ZingSpeed
	Write8(p, 0);  // StageIndex
	Write32(p, 0); // RaceCountDownTime
	Write32(p, 0); // RaceCountDownDis
	Write8(p, 0);  // UseNewCountDownTime
	Write32(p, 0); // NewCountDownTime
	Write32(p, (DWORD)time(nullptr)); // ServerSecond
	Write32(p, 0); // ServerMicroSecond
#endif

	len = p - buf;
	SendToClient(Client, 511, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyPlayerFinishRace(ClientNode *Client,UINT Uin, bool NewRecord, int FinTime)
{

	BYTE buf[8192]{};
	BYTE *p = buf;
	size_t len;

	Write32(p, Uin); // Uin
	Write8(p, NewRecord);	 // NewRecord
	Write32(p, FinTime);	 // FinTime

	len = p - buf;
	SendToClient(Client, 522, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
	NotifyFeedBackData(Client);
}
void WINAPI OnBorderCountDown2(RoomNode* Room)
{
	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient)
		{
			RoomClient->TaskID = 0;
			RoomClient->BorderRoomID = 0;
			RoomClient->IsFinish = 0;
			SendToClient(RoomClient, 514, (const BYTE*)"\x00\x00\x00\x00", 4, RoomClient->GameID, FE_GAMELOGIC, RoomClient->ConnID, Notify);
		}
		Room->Player[i] = NULL;
		Room->CurrentPlayerNum = 0;
	}

	Rooms.erase(Room->ID);
	delete Room;//清理边境房间
}
void WINAPI OnBorderCountDown(RoomNode* Room)
{

	for (char i = 0; i < 6; i++)
	{
		ClientNode* RoomClient = Room->Player[i];
		if (RoomClient && RoomClient->TaskID == 0xFFFD)
		{
			//边境结算
			BYTE* p = BorderOver;
			p += 2;
			Write32(p, RoomClient->Uin);
			SendToClient(RoomClient, 542, BorderOver, 187, RoomClient->ConnID, FE_PLAYER, RoomClient->ConnID, Notify);
		}
		else if (RoomClient && RoomClient->TaskID == 0xFFFC)
		{
			//排位结算
			BYTE* p = RankedOver;
			p += 3;
			Write32(p, RoomClient->Uin);
			SendToClient(RoomClient, 513, RankedOver, 402, RoomClient->ConnID, FE_PLAYER, RoomClient->ConnID, Notify);
		}
	}
	CreateRoomTimer(Room, 16000, OnBorderCountDown2);
}

void RequestReportCurrentInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	WORD len;
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	UINT CurrentState = Read32(Body);
	Vector3 Position = {};
	// m_aiCurrentPosition[3]
	//Position.X=Read32(Body);
	//Position.Y = Read32(Body);
	//Position.Z = Read32(Body);
	Read32(Body);
	Read32(Body);
	Read32(Body);
	BOOL IsFinish = FALSE;
	BYTE PassedCheckPointNum = Read8(Body);
	for (size_t i = 0; i < PassedCheckPointNum; i++)
	{
		WORD PassedCheckPointID = Read16(Body);
		// printf("%d\t", PassedCheckPointID);
		if (Client->Loop)
		{
			if (Client->EndCheckPoint != 0 && PassedCheckPointID > Client->EndCheckPoint) // 大于终点的则可能在走近道
			{
				// printf("continue\t");
				continue;
			}
			if (PassedCheckPointID == 0 && Client->PassedCheckPoint > 4)
			{
				Client->Round++;
				// Client->PassedCheckPointPropS.clear();
				if (Client->Round >= Client->TotalRound)
				{
					IsFinish = TRUE;
				}
			}
			// 修复阿拉丁冲线不结算 day 20220419
			else if (Client->PassedCheckPoint == 0 && PassedCheckPointID > 4)
			// else if (Client->PassedCheckPoint == 0 && PassedCheckPointID > 4 && Client->MapID != 373)
			{
				Client->Round--;
			}
		}
		else
		{
			if (PassedCheckPointID == Client->EndCheckPoint)
			{
				Client->Round++;
				// Client->PassedCheckPointPropS.clear();
				if (Client->Round >= Client->TotalRound)
				{
					IsFinish = TRUE;
				}
			}
		}
		Client->PassedCheckPoint = PassedCheckPointID;
	}

	UINT LapTime = Read32(Body);

	BYTE AccVerifyDataNum = Read8(Body);
	for (size_t i = 0; i < AccVerifyDataNum; i++)
	{
		/*
m_astAccelVerifyData[].m_unType
m_astAccelVerifyData[].m_ushTotalCount
m_astAccelVerifyData[].m_unMaxDurTime
m_astAccelVerifyData[].m_ushOverlayCount
		*/
		Body += Get16(Body);
	}

	BYTE MsgSequence = Read8(Body);

	BYTE ExtraDataNum = Read8(Body);
	for (size_t i = 0; i < ExtraDataNum; i++)
	{
		/*
m_astExtraData[].stSkillStoneTriggerData.m_uTriggerTotalCount
m_astExtraData[].stSkillStoneTriggerData.m_uCheckData1
m_astExtraData[].stSkillStoneTriggerData.m_uCheckData2
m_astExtraData[].m_stClientReportSelfErr.m_iRemoveFailedCount
m_astExtraData[].m_stClientReportSelfErr.m_iNo2CheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iFrictionCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iGPTargetCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iPowerCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iStoneCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iHashCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iTencentCarCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iUseIlleagleNo2Count
m_astExtraData[].m_stClientReportSelfErr.m_iListAffectGPsCheckErrCount
m_astExtraData[].m_stClientReportSelfErr.m_iGPTargetInvalidErrCount
m_astExtraData[].m_stClientReportSelfErr.m_aiErrData[ENMCRET_MAX]
m_astExtraData[].m_stClientReportSeflTag.m_iPowerCheckTag
m_astExtraData[].m_stClientReportSeflTag.m_iNo2CheckTag
m_astExtraData[].m_stClientReportSeflTag.m_iTencentCarCheckTag
m_astExtraData[].m_stClientReportSeflTag.m_iGPTargetInvalidCheckTag
m_astExtraData[].m_u32JetCombinationFlag
m_astExtraData[].m_unMaxSpeed
m_astExtraData[].m_u32CollisionNumOnLastCheckPoint
m_astExtraData[].m_uiTotalDrift
m_astExtraData[].m_bySpeedSectionNum
m_astExtraData[].m_auSpeedSectionAccTime[]
m_astExtraData[].m_byNormalSpeedSectionNum
m_astExtraData[].m_auNormalSpeedSectionAccTime[]
m_astExtraData[].m_u8MemCheckResultNum
m_astExtraData[].m_au32MemData[]
m_astExtraData[].m_auClientCheckErr[ECCET_MAX]
m_astExtraData[].m_auClientCheckErrTag[ECCETT_MAX]
m_astExtraData[].m_u8UpdateCount
m_astExtraData[].m_u8OffsetOverThresholdCount
m_astExtraData[].m_u32OffsetOverThresholdSum
m_astExtraData[].m_u16MaxOffsetSpeed
m_astExtraData[].m_u16AvgOffsetSpeed
m_astExtraData[].m_u16MaxRealSpeed
m_astExtraData[].m_u16AvgRealSpeed
m_astExtraData[].m_u32MaxSpeedRatio
m_astExtraData[].m_u16SpeedRatioOverThresholdCount
m_astExtraData[].m_u32MaxSpeedRatio2
m_astExtraData[].m_u16SpeedRatioOverThresholdCount2
m_astExtraData[].m_u32FBSkillRebornTime
m_astExtraData[].m_u32FBSkillChargeTime
m_astExtraData[].m_uiTotalPerpetualNight
m_astExtraData[].m_iRPCSFlag
m_astExtraData[].m_u32CollisionPlayerNum
m_astExtraData[].m_u32CollisionNumOnPlayer
m_astExtraData[].m_iCodeItegrity
m_astExtraData[].m_iSendLaPkg
m_astExtraData[].m_iSendDataToSvr
m_astExtraData[].m_aiAbnormalData[4]
m_astExtraData[].m_stLevelInfo.m_cLowFPSRatio
m_astExtraData[].m_stLevelInfo.m_cAveFPS
m_astExtraData[].m_stLevelInfo.m_iMemUsed
m_astExtraData[].m_stLevelInfo.m_iVMemUsed
m_astExtraData[].m_stLevelInfo.m_iGameSetting
m_astExtraData[].m_uiPositionErrorCount
m_astExtraData[].m_uiDropToDeathSurfaceNums
m_astExtraData[].m_uiCollideNumsInN2OStat
m_astExtraData[].m_uiBeyondPlayerNums
		*/
		Body += Get16(Body);
	}

	BYTE N2OChangeEventNum = Read8(Body);
	for (size_t i = 0; i < N2OChangeEventNum; i++)
	{
		/*
m_astN2OChangeEvent[].m_u16EventID
m_astN2OChangeEvent[].m_u32BeginLapTime
m_astN2OChangeEvent[].m_u16LastingTime
m_astN2OChangeEvent[].m_u8EventType
m_astN2OChangeEvent[].m_i16N2OChange
		*/
		Body += Get16(Body);
	}

	UINT Flag = Read32(Body);

#ifndef ZingSpeed
	BYTE HasCrashModeData = Read8(Body);
	if (HasCrashModeData)
	{
		// m_astCrashModeData[].m_u8CurEggNum
		Body += Get16(Body);
	}

	BYTE HasPointChallengeData = Read8(Body);
	if (HasPointChallengeData)
	{
		/*
m_astPointChallengeData[].m_uiEatCoin
m_astPointChallengeData[].m_uiBeforeTime
m_astPointChallengeData[].m_uiBeforeLong
m_astPointChallengeData[].m_uiCollectN2
m_astPointChallengeData[].m_uiShuangpen
m_astPointChallengeData[].m_uiKongpen
m_astPointChallengeData[].m_uiLuodipen
m_astPointChallengeData[].m_uiNPCRunAfterTime
m_astPointChallengeData[].m_uiLeiYinCrazyJet
		*/
		Body += Get16(Body);
	}

	UINT SkateCoinNum = Read32(Body);
	for (size_t i = 0; i < SkateCoinNum; i++)
	{
		/*
m_astSkateCoinData[].m_uiCoinID
m_astSkateCoinData[].m_uiPlayerPosX
m_astSkateCoinData[].m_uiPlayerPosY
m_astSkateCoinData[].m_uiPlayerPosZ
m_u32SkateComboAwardCoinNum
m_astSkateComboAwardCoinData[].m_ushComboNum
m_astSkateComboAwardCoinData[].m_ushCoinNum
		*/
		Body += Get16(Body);
	}

	UINT SkateComboAwardCoinNum = Read32(Body);
	for (size_t i = 0; i < SkateComboAwardCoinNum; i++)
	{
		/*
m_astSkateComboAwardCoinData[].m_ushComboNum
m_astSkateComboAwardCoinData[].m_ushCoinNum
		*/
		Body += Get16(Body);
	}

	UINT SkateDoubleJumpCount = Read32(Body);
	UINT CurrMapID = Read32(Body);

	BYTE HasGameDetailRecord = Read8(Body);
	if (HasGameDetailRecord)
	{
		BYTE* pGameDetailRecord = Body;
		len = Read16(pGameDetailRecord);

		UINT Uin = Read32(pGameDetailRecord);
		char* NickName = ReadString(pGameDetailRecord, 17);
		int KartId = Read32(pGameDetailRecord);
		UINT FinTime = Read32(pGameDetailRecord);
		UINT HighestSpeed = Read32(pGameDetailRecord);
		UINT AverageSpeed = Read32(pGameDetailRecord);
		UINT CollisionCount = Read32(pGameDetailRecord);
		UINT N2OGatherCount = Read32(pGameDetailRecord);
		UINT JetTime = Read32(pGameDetailRecord);
		UINT ChaoJiQiBu = Read32(pGameDetailRecord);
		UINT ZhuangQiangDianPen = Read32(pGameDetailRecord);
		UINT DriftCount = Read32(pGameDetailRecord);
		UINT SmallJetCount = Read32(pGameDetailRecord);
		UINT DoubleJetCount = Read32(pGameDetailRecord);
		UINT FlyJetCount = Read32(pGameDetailRecord);
		UINT FallJetCount = Read32(pGameDetailRecord);
		UINT DuanWeiQiBu = Read32(pGameDetailRecord);
		UINT DuanWeiDoubleJetCount = Read32(pGameDetailRecord);
		UINT LinkJetCount = Read32(pGameDetailRecord);
		UINT WCJetCount = Read32(pGameDetailRecord);
		UINT CeShenDriftCount = Read32(pGameDetailRecord);
		UINT ShuaiWeiDriftCount = Read32(pGameDetailRecord);
		UINT CWWJetCount = Read32(pGameDetailRecord);
		UINT XiaoGuaCount = Read32(pGameDetailRecord);
		UINT teamID = Read32(pGameDetailRecord);
		UINT FastCornersNum = Read32(pGameDetailRecord);
		USHORT DiffCornerNum = Read16(pGameDetailRecord);
		UINT FastCornerDetailInfo = Read32(pGameDetailRecord);
		USHORT WSingleJetCount = Read16(pGameDetailRecord);
		USHORT WCWJetCount = Read16(pGameDetailRecord);
		USHORT CWWJetCount1 = Read16(pGameDetailRecord);
		USHORT CeShenJetCount = Read16(pGameDetailRecord);
		USHORT ShuaiWeiJetCount = Read16(pGameDetailRecord);
		short RankedMatchGrade = Read16(pGameDetailRecord);
		USHORT DuanWeiPiaoYi = Read16(pGameDetailRecord);
		USHORT DuanWeiJet = Read16(pGameDetailRecord);
		USHORT OutOfTurnQuickly = Read16(pGameDetailRecord);
		USHORT Launch = Read16(pGameDetailRecord);
		USHORT HitWaterDrift = Read16(pGameDetailRecord);
		USHORT OutWaterJet = Read16(pGameDetailRecord);
		USHORT N2DriftContinue = Read16(pGameDetailRecord);
		USHORT WaveFlyJet = Read16(pGameDetailRecord);
		USHORT WorldID = Read16(pGameDetailRecord);
		UCHAR Npc = Read8(pGameDetailRecord);
		UINT GameSeq = Read32(pGameDetailRecord);
		UINT Time = Read32(pGameDetailRecord);

		Body += len;
		Client->HighestSpeed= HighestSpeed;
		Client->CollisionCount = CollisionCount;
		Client->N2OGatherCount = N2OGatherCount;
		Client->DriftCount = DriftCount;

		//Body += Get16(Body);
	}

	BYTE CurrentInfoFlag = Read8(Body);
	UINT TotalAccelerateFuel = Read32(Body);
	UINT DstNPCID = Read32(Body);
	UINT DistanceToEnd = Read32(Body);
	UINT GansterProcess = Read32(Body);
	UINT TotalGangsterPKCount = Read32(Body);

	BYTE HasTowerChallengeData = Read8(Body);
	if (HasTowerChallengeData)
	{
		/*
m_astTowerChallengeData[].m_uiEatCoin
m_astTowerChallengeData[].m_uiCollisionCount
m_astTowerChallengeData[].m_uiDriftCount
m_astTowerChallengeData[].m_cKartStatus
m_astTowerChallengeData[].m_cTrackElementNum
m_astTowerChallengeData[].m_astTriggerInfo[].m_cType
m_astTowerChallengeData[].m_astTriggerInfo[].m_cInitCheckPoint
m_astTowerChallengeData[].m_astTriggerInfo[].m_cPlayerTriggerCheckPoint
		*/
		Body += Get16(Body);
	}

	BYTE CurrentSelfRank = Read8(Body);
	BYTE CurrentTeammateRank = Read8(Body);

	BYTE HasP2PStatusData = Read8(Body);
	if (HasP2PStatusData)
	{
		/*
m_astP2PStatusData[].m_uiUin
m_astP2PStatusData[].m_ucServerUDPLoginStaus
m_astP2PStatusData[].m_ucPlayerNum
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiUin
m_astP2PStatusData[].m_astP2PStatus2Player[].m_ucP2PConnectState
m_astP2PStatusData[].m_astP2PStatus2Player[].m_ucP2PCanPass
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiP2PNetSpeed
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiNetSpeed
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiNotPassStateCount
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiSendDataCount[ENMSDM_MAX]
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiRecvDataCount[ENMSDM_MAX]
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uiServerRecvDataCount[ENMSDM_MAX]
m_astP2PStatusData[].m_astP2PStatus2Player[].m_ushRecvPkgCount
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astRecvPkgInfo[].m_uiPkgID
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astRecvPkgInfo[].m_uiRecvTime
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astRecvPkgInfo[].m_uiDistanceDiff
m_astP2PStatusData[].m_astP2PStatus2Player[].m_uchDetailCount
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astDetailInfo[].m_ucType
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astDetailInfo[].m_uiMsgID
m_astP2PStatusData[].m_astP2PStatus2Player[].m_astDetailInfo[].m_uiCount
m_astP2PStatusData[].m_ushSendPkgCount
m_astP2PStatusData[].m_astSendPkgInfo[].m_uiPkgID
m_astP2PStatusData[].m_astSendPkgInfo[].m_uiSendTime
m_astP2PStatusData[].m_astSendPkgInfo[].m_uchUDPStatus
m_astP2PStatusData[].m_astSendPkgInfo[].m_uchSendDataMethod
m_astP2PStatusData[].m_uiRaceBeginTime
m_astP2PStatusData[].m_uiLagCnt
m_astP2PStatusData[].m_uiLagCntGlobal
m_astP2PStatusData[].m_uiTickInLevel
		*/
		Body += Get16(Body);
	}

	UINT DistanceToFirstRacer = Read32(Body);
	UINT TimerChallengeRecoverNum = Read32(Body);
	UINT CoinNum = Read32(Body);

	BYTE ClientPlayerNum = Read8(Body);
	for (size_t i = 0; i < ClientPlayerNum; i++)
	{
		/*
m_astClientGameInfo[].m_uiUin
m_astClientGameInfo[].m_uchDistanceNum
m_astClientGameInfo[].m_astDistanceToEndInfo[].m_uiTimeFromRaceBegin
m_astClientGameInfo[].m_astDistanceToEndInfo[].m_uiDistanceToEnd
m_astClientGameInfo[].m_uchCollisionNum
m_astClientGameInfo[].m_astClientCollisionInfo[].m_uiTimeFromRaceBegin
		*/
		Body += Get16(Body);
	}

	BYTE RankedMatchSpeedKingBuffer = Read8(Body);
	UINT DistanceTotal = Read32(Body);
	BYTE IsHitRecordContion = Read8(Body);
	USHORT HangSpan = Read16(Body);
	BYTE CurRound = Read8(Body);
	BYTE IncRound = Read8(Body);

	if (Client->MapCompatibilityMode)
	{
		IsFinish = FALSE;
		if (DistanceToEnd == 0)
		{
			IsFinish = TRUE;
		}
	}
#endif
#ifdef DEBUG
	Z_INFO("圈数:%d\n", Client->Round);
#endif
	if (IsFinish || (DstNPCID==0&&Client->IsFinish == true))
	{
		UINT FinshUin = DstNPCID > 0? DstNPCID :Client->Uin;
		Client->IsFinish = false;

		if (((Client->RoomID != 0 || Client->BorderRoomID != 0) && Client->FinishTime == 0) || (Client->matchQueueType > 0 && Client->FinishTime == 0)||(DstNPCID>0))
		{
			
			// NotifyPlayerFinishRace(Client, NewRecord, Client->FinishTime);
			
			RoomNode *Room = GetRoom(Client->RoomID);
			if (!Room&& !GetRoom(Client->BorderRoomID))
			{
				
				if (Client->RoomID == 9999)
				{
					NotifySingleRaceFinish(Client);
					NotifySingleRaceOver(Client);
					NotifySingleGameOver(Client);
					// NotifyKickFromGame(Client, "挑战完成", 0);
				}
				return;
			}
			if (!Room) {
				Room = GetRoom(Client->BorderRoomID);
			}
			Client->FinishTime = LapTime * 10;
			bool NewRecord = NewRecord = PlayerDB_SetMapRecord(FinshUin, Client->MapID, Client->FinishTime);
			if (DstNPCID >0) {
				Room->NPCPlayer[DstNPCID - 1]->FinishTime = LapTime * 10;
			}
			else {
				Client->FinishTime = LapTime * 10;
			}
			if (Client->TaskID == 0xFFFF || Client->TaskID == 0xFFFE) {
				//闯关
				Client->matchQueueType = 0;
				HANDLE over = CreateThread(NULL, 0, OnSingleOver, Client, 0, NULL);
				if (over)
				{
					CloseHandle(over);
				}
			}
			else if (Client->BorderRoomID != 0 && Client->FinishTime == 0 && Client->TaskID == 0xFFFD || Client->TaskID == 0xFFFC)
			{
				Client->matchQueueType = 0;
				RoomNode* BorderRoom = GetRoom(Client->BorderRoomID);
				if (!BorderRoom)
				{
					return;
				}
				int PlayerNum = 0;
				for (char i = 0; i < 6; i++)
				{
					ClientNode* RoomClient = BorderRoom->Player[i];
					if (RoomClient)
					{
						PlayerNum++;
						if (Room->isFinsh == true) {
							NotifyPlayerFinishRace(RoomClient, FinshUin, NewRecord, Client->FinishTime);
						}
						else {
							NotifyCountDown(RoomClient, FinshUin, NewRecord, Client->FinishTime);
						}
					}
				}
				if (Room->isFinsh == true) {

				}
				else {
					Room->isFinsh = true;
				}
				if (!BorderRoom->Timer)
				{
					if (PlayerNum > 1)
					{
						CreateRoomTimer(BorderRoom, 10000, OnBorderCountDown);//如果玩家大于1等待10秒后进入结算界面
					}
					else
					{
						CreateRoomTimer(BorderRoom, 10000, OnBorderCountDown);
					}

				}
			}
			else {
				int PlayerNum = 0;
				for (char i = 0; i < 6; i++)
				{
					ClientNode* RoomClient = Room->Player[i];
					if (RoomClient)
					{
						PlayerNum++;
						if (Room->isFinsh==true) {
							NotifyPlayerFinishRace(RoomClient, FinshUin,NewRecord, Client->FinishTime);
						}
						else {
							NotifyCountDown(RoomClient, FinshUin, NewRecord, Client->FinishTime);
						}
						
						
					}
				}
				if (Room->isFinsh == true) {
					
				}
				else {
					Room->isFinsh = true;
				}
				if (!Room->Timer)
				{
					if (PlayerNum > 1)
					{
						CreateRoomTimer(Room, 10000, OnCountDown);
					}
					else
					{
						if (NewRecord)
						{
							CreateRoomTimer(Room, 10000, OnCountDown);
						}
						else
						{
							// day 20230420
							CreateRoomTimer(Room, 10000, OnCountDown);
							// CreateRoomTimer(Room, 1, OnCountDown);
						}
					}
				}
			}
			
		}
		
		NotifySpecialActivityInfo(Client);
	}
}

void NotifyMuchPlayerRaceOver(ClientNode *Client)
{
	YAML::Node RaceAward = getGameConfig()["RaceAward"];
	// Race over get award
	UINT Experience = RaceAward["Experience"].as<UINT>(); // 经验
	UINT Money = RaceAward["Money"].as<UINT>();			  // 酷币
	UINT SuperMoney = RaceAward["SuperMoney"].as<UINT>(); // 点券
	UINT Coupons = RaceAward["Coupons"].as<UINT>();		  // 消费券
	UINT LuckMoney = RaceAward["LuckMoney"].as<UINT>();	  // 幸运币

	Client->Experience += Experience;
	Client->Money += Money;
	Client->SuperMoney += SuperMoney;
	Client->Coupons += Coupons;
	Client->LuckMoney += LuckMoney;
	PlayerDB_UpdateMoneyExp(Client);
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write16(p, 1); // BaseGameMode
	Write16(p, 1); // SubGameMode
	Write8(p, 1);  // CurrentPlayerNum
	for (size_t i = 0; i < 1; i++)
	{ // RaceScore
		BYTE *pRaceScore = p;
		Write16(pRaceScore, 0); // len

		WriteUn(pRaceScore, Client->Uin);		 // Uin
		WriteUn(pRaceScore, Client->FinishTime); // FinTime
		Write32(pRaceScore, 0);					 // BorderGameType
		Write32(pRaceScore, 999);				 // ExpInc
		Write32(pRaceScore, 999);				 // TotalExp
		Write32(pRaceScore, 999);				 // SuperMoneyInc
		Write32(pRaceScore, 999);				 // TotalSuperMoney
		Write32(pRaceScore, 999);				 // PersonalHonor
		Write32(pRaceScore, 999);				 // TotalPersonalHonor
		Write32(pRaceScore, 999);				 // NationalHonor
		Write32(pRaceScore, 999);				 // TotalNationalHonor
		Write32(pRaceScore, 999);				 // GuildPointInc
		Write32(pRaceScore, 999);				 // GuildPointSpecialInc
		Write8(pRaceScore, 0);					 // TeamID
		Write8(pRaceScore, 0);					 // AwardNum
		Write32(pRaceScore, 0);					 // SpecialAward[]
		Write8(pRaceScore, 1);					 // VipMemberFreeFlag
		Write8(pRaceScore, 0);					 // IsTeamMatch
		Write32(pRaceScore, 999);				 // ZhanXunInc
		Write32(pRaceScore, 999);				 // ZhanXunTotal
		Write32(pRaceScore, 999);				 // TeamHonourInc
		Write32(pRaceScore, 999);				 // MedalInc
		Write32(pRaceScore, 999);				 // SeasonZhanXunTotal
		Write32(pRaceScore, 0);					 // ReduceReturnRate
		Write32(pRaceScore, 0);					 // ReduceReturnAvailPeriod
		Write32(pRaceScore, 0);					 // DetailRecordID
		Write32(pRaceScore, 0);					 // GuildID
		Write8(pRaceScore, 0);					 // GuildName[]
		Write32(pRaceScore, 999);				 // GMMScoreInc
		Write8(pRaceScore, 1);					 // HasRankedMatchInfo
		for (size_t i = 0; i < 0; i++)
		{ // RankedMatchScore
			BYTE *pRankedMatchScore = pRaceScore;
			Write16(pRankedMatchScore, 0); // len

			{ // RankedMatchLastGradeInfo
				BYTE *pRankedMatchLastGradeInfo = pRankedMatchScore;
				Write16(pRankedMatchLastGradeInfo, 0); // len

				Write32(pRankedMatchLastGradeInfo, 27);	 // GradeLevel
				Write32(pRankedMatchLastGradeInfo, 999); // ChildLevel
				Write32(pRankedMatchLastGradeInfo, 999); // GradeScore
				Write32(pRankedMatchLastGradeInfo, 999); // MaxGradeScore
				Write32(pRankedMatchLastGradeInfo, 999); // TotalScore

				len = pRankedMatchLastGradeInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			{ // RankedMatchGradeInfo
				BYTE *pRankedMatchGradeInfo = pRankedMatchScore;
				Write16(pRankedMatchGradeInfo, 0); // len

				Write32(pRankedMatchGradeInfo, 27);	 // GradeLevel
				Write32(pRankedMatchGradeInfo, 999); // ChildLevel
				Write32(pRankedMatchGradeInfo, 999); // GradeScore
				Write32(pRankedMatchGradeInfo, 999); // MaxGradeScore
				Write32(pRankedMatchGradeInfo, 999); // TotalScore

				len = pRankedMatchGradeInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			{ // RankedMatchPromoInfo
				BYTE *pRankedMatchPromoInfo = pRankedMatchScore;
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

				len = pRankedMatchPromoInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			{ // RankedMatchComputeInfo
				BYTE *pRankedMatchComputeInfo = pRankedMatchScore;
				Write16(pRankedMatchComputeInfo, 0); // len

				Write32(pRankedMatchComputeInfo, 100);	// RankAddScore
				Write32(pRankedMatchComputeInfo, 100);	// FinishAddScore
				Write32(pRankedMatchComputeInfo, 100);	// KillAllAddScore
				Write32(pRankedMatchComputeInfo, 100);	// GradeProtectAddScore
				Write32(pRankedMatchComputeInfo, 100);	// GradeAddScore
				Write32(pRankedMatchComputeInfo, 0);	// ProtectAddScoreGradeLimit
				Write32(pRankedMatchComputeInfo, 0);	// DailyExtraAddScore
				Write32(pRankedMatchComputeInfo, 100);	// AwardRankCoinNums
				Write32(pRankedMatchComputeInfo, 1000); // WeeklyAwardRankCoinNums
				Write32(pRankedMatchComputeInfo, 9999); // WeeklyAwardRankCoinLimit
				Write32(pRankedMatchComputeInfo, 0);	// AwardRankCoinNumsByRank
				Write32(pRankedMatchComputeInfo, 0);	// AwardRankCoinNumsByFinish
				Write32(pRankedMatchComputeInfo, 0);	// BanMapExtraAddScore
				Write32(pRankedMatchComputeInfo, 0);	// TeamRankAddScore
				Write32(pRankedMatchComputeInfo, 0);	// AwardRankCoinNumsByTeamRank
				Write32(pRankedMatchComputeInfo, 0);	// AwardRankCoinNumsByTask
				Write8(pRankedMatchComputeInfo, 0);		// UseDoubleScoreItem
				Write8(pRankedMatchComputeInfo, 0);		// UserNotDecreaseItem
				Write8(pRankedMatchComputeInfo, 0);		// FinishSeasonChallengeTaskOrNot
				Write32(pRankedMatchComputeInfo, 0);	// FinishedSeasonChallengeTaskIndex
				Write32(pRankedMatchComputeInfo, 0);	// SeasonChallengeTaskProtectAddScore
				Write8(pRankedMatchComputeInfo, 0);		// NotDecreaseReason
				Write8(pRankedMatchComputeInfo, 0);		// HaveSpeedKingInfo
				for (size_t i = 0; i < 0; i++)
				{ // RankedMatchSpeedKingInfos
					BYTE *pRankedMatchSpeedKingInfos = pRankedMatchComputeInfo;
					Write16(pRankedMatchSpeedKingInfos, 0); // len

					Write8(pRankedMatchSpeedKingInfos, 0);	// WinOrNot
					WriteUn(pRankedMatchSpeedKingInfos, 0); // UiAwardSpeedKingEnergyItemNums
					Write32(pRankedMatchSpeedKingInfos, 0); // WeeklyAwardSpeedKingEnergyItemNums
					Write32(pRankedMatchSpeedKingInfos, 0); // WeeklyAwardSpeedKingEnergyItemLimitNums

					len = pRankedMatchSpeedKingInfos - pRankedMatchComputeInfo;
					Set16(pRankedMatchComputeInfo, (WORD)len);
					pRankedMatchComputeInfo += len;
				}
				Write8(pRankedMatchComputeInfo, 0); // HaveExtraComputeInfo
				for (size_t i = 0; i < 0; i++)
				{ // RankedMatchExtracComputeInfos
					BYTE *pRankedMatchExtracComputeInfos = pRankedMatchComputeInfo;
					Write16(pRankedMatchExtracComputeInfos, 0); // len

					Write32(pRankedMatchExtracComputeInfos, 0); // ExtraComputeType
					Write32(pRankedMatchExtracComputeInfos, 0); // AwardItemNums
					Write32(pRankedMatchExtracComputeInfos, 0); // WeekylyAwardItemNums
					Write32(pRankedMatchExtracComputeInfos, 0); // WeeklyAwardItemLimitNums
					Write8(pRankedMatchExtracComputeInfos, 0);	// HaveGameLogicTask
					for (size_t i = 0; i < 0; i++)
					{ // RakedMatchGamelogicTaskInfo
						BYTE *pRakedMatchGamelogicTaskInfo = pRankedMatchExtracComputeInfos;
						Write16(pRakedMatchGamelogicTaskInfo, 0); // len

						Write32(pRakedMatchGamelogicTaskInfo, 0); // GameLogicTaskType
						Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskFinishCondValue
						Write8(pRakedMatchGamelogicTaskInfo, 0);  // JudgeRule
						Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskAddGradeScore
						Write8(pRakedMatchGamelogicTaskInfo, 0);  // GradeScoreRangeNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); // GradeScoreRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0);  // TaskFinishCondValueNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskFinishCondValueRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0);  // TaskAddGradeScoreNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskAddGradeScoreRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0);  // FailedAddGradeScoreNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); // FailedAddGradeScoreRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0);  // FinishTask
						Write32(pRakedMatchGamelogicTaskInfo, 0); // ExtraFinishTaskAddScore

						len = pRakedMatchGamelogicTaskInfo - pRankedMatchExtracComputeInfos;
						Set16(pRankedMatchExtracComputeInfos, (WORD)len);
						pRankedMatchExtracComputeInfos += len;
					}
					Write8(pRankedMatchExtracComputeInfos, 0); // Win
					Write8(pRankedMatchExtracComputeInfos, 0); // NormalPlayerOrNot
					Write8(pRankedMatchExtracComputeInfos, 0); // LeadRolePlayerNums

					len = pRankedMatchExtracComputeInfos - pRankedMatchComputeInfo;
					Set16(pRankedMatchComputeInfo, (WORD)len);
					pRankedMatchComputeInfo += len;
				}
				Write32(pRankedMatchComputeInfo, 0); // SeasonNewMapAddScore
				Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeBaseScore
				Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeContiWinExtrScore
				Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeRegretDefeatScore
				Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeRespectableOpponentScore
				Write8(pRankedMatchComputeInfo, 0);	 // SpeedGodChallengeAddScore
				Write8(pRankedMatchComputeInfo, 0);	 // SpeedGodChallengeAddCoin
				Write8(pRankedMatchComputeInfo, 0);	 // SpeedFlashAddFactoryCoin
				Write8(pRankedMatchComputeInfo, 0);	 // PinnacleBattleAddScore
				Write8(pRankedMatchComputeInfo, 0);	 // PromotionExtraScore
				Write32(pRankedMatchComputeInfo, 0); // SavedExtraScore
				Write32(pRankedMatchComputeInfo, 0); // DailyExtraTaskID
				Write8(pRankedMatchComputeInfo, 0);	 // SuitActiveAddScore
				Write8(pRankedMatchComputeInfo, 0);	 // SpeedGodChallengeType
				Write8(pRankedMatchComputeInfo, 0);	 // SuitActiveEffectID

				len = pRankedMatchComputeInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			Write8(pRankedMatchScore, 0); // HavePromotionExtraScoreInfo
			for (size_t i = 0; i < 0; i++)
			{ // PromotionExtraScoreInfo
				BYTE *pPromotionExtraScoreInfo = pRankedMatchScore;
				Write16(pPromotionExtraScoreInfo, 0); // len

				Write8(pPromotionExtraScoreInfo, 0);  // PromotionMatch
				Write32(pPromotionExtraScoreInfo, 0); // PromotionRoundNums
				Write32(pPromotionExtraScoreInfo, 0); // PromotionSuccessRoundNums
				Write32(pPromotionExtraScoreInfo, 0); // PromotionAlreadySuccessRoundNums
				Write32(pPromotionExtraScoreInfo, 0); // PromotionRecordTag
				Write32(pPromotionExtraScoreInfo, 0); // CurrentPromotionRoundNums
				Write32(pPromotionExtraScoreInfo, 0); // PromoFailedMinusScore
				Write32(pPromotionExtraScoreInfo, 0); // PromoSuccAddScore
				Write8(pPromotionExtraScoreInfo, 0);  // PromoDirectly
				Write8(pPromotionExtraScoreInfo, 0);  // GradeNoNeedPromo
				Write8(pPromotionExtraScoreInfo, 0);  // RankedMemoryNoNeedPromo
				Write8(pPromotionExtraScoreInfo, 0);  // PromotionExtraScore
				Write8(pPromotionExtraScoreInfo, 0);  // PromtRaceFailedDefendFlag

				len = pPromotionExtraScoreInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}

			len = pRankedMatchScore - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}
		Write32(pRaceScore, 999); // MoneyInc
		Write32(pRaceScore, 999); // TotalMoney
		Write32(pRaceScore, 0);	  // DistanceToEnd
		Write16(pRaceScore, 999); // Score
		Write8(pRaceScore, 0);	  // ParaNum
		Write32(pRaceScore, 0);	  // ParaList[]
		Write8(pRaceScore, 0);	  // HasRankedMatchArenaInfo
		for (size_t i = 0; i < 0; i++)
		{ // RankedMatchArenaScore
			BYTE *pRankedMatchArenaScore = pRaceScore;
			Write16(pRankedMatchArenaScore, 0); // len

			Write8(pRankedMatchArenaScore, 0);	// Win
			Write8(pRankedMatchArenaScore, 0);	// Rank
			Write8(pRankedMatchArenaScore, 0);	// TaskScore
			Write32(pRankedMatchArenaScore, 0); // AwardRankCoinNums
			{									// RankedMatchLastGradeInfo
				BYTE *pRankedMatchLastGradeInfo = pRankedMatchArenaScore;
				Write16(pRankedMatchLastGradeInfo, 0); // len

				Write32(pRankedMatchLastGradeInfo, 0); // GradeLevel
				Write32(pRankedMatchLastGradeInfo, 0); // ChildLevel
				Write32(pRankedMatchLastGradeInfo, 0); // GradeScore
				Write32(pRankedMatchLastGradeInfo, 0); // MaxGradeScore
				Write32(pRankedMatchLastGradeInfo, 0); // TotalScore

				len = pRankedMatchLastGradeInfo - pRankedMatchArenaScore;
				Set16(pRankedMatchArenaScore, (WORD)len);
				pRankedMatchArenaScore += len;
			}
			{ // RankedMatchGradeInfo
				BYTE *pRankedMatchGradeInfo = pRankedMatchArenaScore;
				Write16(pRankedMatchGradeInfo, 0); // len

				Write32(pRankedMatchGradeInfo, 0); // GradeLevel
				Write32(pRankedMatchGradeInfo, 0); // ChildLevel
				Write32(pRankedMatchGradeInfo, 0); // GradeScore
				Write32(pRankedMatchGradeInfo, 0); // MaxGradeScore
				Write32(pRankedMatchGradeInfo, 0); // TotalScore

				len = pRankedMatchGradeInfo - pRankedMatchArenaScore;
				Set16(pRankedMatchArenaScore, (WORD)len);
				pRankedMatchArenaScore += len;
			}
			{ // RankedMatchPromoInfo
				BYTE *pRankedMatchPromoInfo = pRankedMatchArenaScore;
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

				len = pRankedMatchPromoInfo - pRankedMatchArenaScore;
				Set16(pRankedMatchArenaScore, (WORD)len);
				pRankedMatchArenaScore += len;
			}
			Write8(pRankedMatchArenaScore, 0);	// WinNum
			Write16(pRankedMatchArenaScore, 0); // CurrentScore
			Write8(pRankedMatchArenaScore, 0);	// FinishScore
			Write16(pRankedMatchArenaScore, 0); // AwardRankScore
			Write8(pRankedMatchArenaScore, 0);	// AwardRankCoinNumsByRank
			Write8(pRankedMatchArenaScore, 0);	// AwardRankCoinNumsByFinish
			Write32(pRankedMatchArenaScore, 0); // WeeklyAwardRankCoinNums
			Write32(pRankedMatchArenaScore, 0); // WeeklyAwardRankCoinLimit
			Write32(pRankedMatchArenaScore, 0); // AwardRankCoinNumsByTask

			len = pRankedMatchArenaScore - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}
		Write8(pRaceScore, 0); // ProfessionLicenseAwardNum
		for (size_t i = 0; i < 0; i++)
		{ // ProfessionLicenseAward
			BYTE *pProfessionLicenseAward = pRaceScore;
			Write16(pProfessionLicenseAward, 0); // len

			Write32(pProfessionLicenseAward, 0); // PointInc
			Write32(pProfessionLicenseAward, 0); // PointLevelCur
			Write32(pProfessionLicenseAward, 0); // PointLevelMax
			Write16(pProfessionLicenseAward, 0); // IncResultID
			Write8(pProfessionLicenseAward, 0);	 // ShowQiPao
			Write32(pProfessionLicenseAward, 0); // PointWeekTotal
			Write32(pProfessionLicenseAward, 0); // PointWeekCur
			{									 // ProfessionLicenseInfo
				BYTE *pProfessionLicenseInfo = pProfessionLicenseAward;
				Write16(pProfessionLicenseInfo, 0); // len

				Write8(pProfessionLicenseInfo, 6); // BigLevel
				Write8(pProfessionLicenseInfo, 5); // SmallLevel

				len = pProfessionLicenseInfo - pProfessionLicenseAward;
				Set16(pProfessionLicenseAward, (WORD)len);
				pProfessionLicenseAward += len;
			}
			Write8(pProfessionLicenseAward, 0);	 // SwitchShowExtraAddPoint
			Write16(pProfessionLicenseAward, 0); // BeginOffsetSeconds

			len = pProfessionLicenseAward - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}
		Write8(pRaceScore, 0); // HaveRankedMatchSpeedKingInfo
		for (size_t i = 0; i < 0; i++)
		{ // RankedMatchSpeedKingScoreInfo
			BYTE *pRankedMatchSpeedKingScoreInfo = pRaceScore;
			Write16(pRankedMatchSpeedKingScoreInfo, 0); // len

			Write32(pRankedMatchSpeedKingScoreInfo, 0); // AwardSpeedKingEnergyNums
			Write8(pRankedMatchSpeedKingScoreInfo, 0);	// Win

			len = pRankedMatchSpeedKingScoreInfo - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}
		{ // RaceResult
			BYTE *pRaceResult = pRaceScore;
			Write16(pRaceResult, 0); // len

			Write8(pRaceResult, 0); // PLExpPowerValid
			Write8(pRaceResult, 0); // PLHonorPowerValid
			Write8(pRaceResult, 0); // ExpMultiNum
			Write8(pRaceResult, 0); // HonorMultiNum

			len = pRaceResult - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}
		Write8(pRaceScore, 0); // HaveRankedMatchExtraInfo
		for (size_t i = 0; i < 0; i++)
		{ // RankedMatchExtraScoreInfo
			BYTE *pRankedMatchExtraScoreInfo = pRaceScore;
			Write16(pRankedMatchExtraScoreInfo, 0); // len

			Write32(pRankedMatchExtraScoreInfo, 0); // AwardItemNums
			Write8(pRankedMatchExtraScoreInfo, 0);	// Win

			len = pRankedMatchExtraScoreInfo - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}
		Write16(pRaceScore, 0); // Status
		Write8(pRaceScore, 0);	// HasAward
		for (size_t i = 0; i < 0; i++)
		{ // Award
			BYTE *pAward = pRaceScore;
			Write16(pAward, 0); // len

			Write32(pAward, 0); // Exp
			Write32(pAward, 0); // Money
			Write32(pAward, 0); // SuperMoney
			Write16(pAward, 0); // ItemNum
			for (size_t i = 0; i < 0; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAward;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 0); // ItemID
				Write32(pItemInfo, 0); // ItemNum
				Write32(pItemInfo, 0); // AvailPeriod
				Write8(pItemInfo, 0);  // Status
				WriteUn(pItemInfo, 0); // ObtainTime
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
			for (size_t i = 0; i < 0; i++)
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

			len = pAward - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}
		Write8(pRaceScore, 0); // HasYuLeJiaNianHuaComputeData
		for (size_t i = 0; i < 0; i++)
		{ // YuLeJiaNianHuaComputeData
			BYTE *pYuLeJiaNianHuaComputeData = pRaceScore;
			Write16(pYuLeJiaNianHuaComputeData, 0); // len

			Write32(pYuLeJiaNianHuaComputeData, 0); // AwardScore
			Write8(pYuLeJiaNianHuaComputeData, 0);	// Recommend
			Write8(pYuLeJiaNianHuaComputeData, 0);	// ScoreDouble
			Write8(pYuLeJiaNianHuaComputeData, 0);	// GuaJi

			len = pYuLeJiaNianHuaComputeData - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}

		len = pRaceScore - p;
		Set16(p, (WORD)len);
		p += len;
	}
	WriteUn(p, 0); // AwardTime
	Write8(p, 0);  // KickOffPlayerNumber
	for (size_t i = 0; i < 0; i++)
	{ // KickOffPlayer
		BYTE *pKickOffPlayer = p;
		Write16(pKickOffPlayer, 0); // len

		Write32(pKickOffPlayer, 0);		// Uin
		WriteString(pKickOffPlayer, 0); // NickName[]
		Write8(pKickOffPlayer, 0);		// Gender
		Write8(pKickOffPlayer, 0);		// TeamID
		Write8(pKickOffPlayer, 0);		// Country

		len = pKickOffPlayer - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // TeamNum
	for (size_t i = 0; i < 0; i++)
	{ // TeamScore
		BYTE *pTeamScore = p;
		Write16(pTeamScore, 0); // len

		Write8(pTeamScore, 0);	// TeamID
		Write32(pTeamScore, 0); // TeamScore
		Write8(pTeamScore, 0);	// TeamMinRank
		Write8(pTeamScore, 0);	// CooperNum
		Write8(pTeamScore, 0);	// IncCooper
		Write8(pTeamScore, 0);	// CurCooper

		len = pTeamScore - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // LeavePlayerNum
	for (size_t i = 0; i < 0; i++)
	{ // LeaveGamePlayer
		BYTE *pLeaveGamePlayer = p;
		Write16(pLeaveGamePlayer, 0); // len

		Write32(pLeaveGamePlayer, 0); // Uin
		Write8(pLeaveGamePlayer, 0);  // TeamID

		len = pLeaveGamePlayer - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // IsLastRound

	len = p - buf;
	SendToClient(Client, 1721, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyMatchRaceOver(ClientNode *Client)
{
	BYTE buf[8192];
	BYTE *p = buf;
	size_t len;

	Write32(p, 0); // AwardTime
	for (size_t i = 0; i < 0; i++)
	{ // RaceScore
		BYTE *pRaceScore = p;
		Write16(pRaceScore, 0); // len

		Write32(pRaceScore, Client->Uin);			 // Uin
		Write32(pRaceScore, Client->FinishTime);	 // FinTime
		Write32(pRaceScore, Client->matchQueueType); // GameType
		Write32(pRaceScore, 0);						 // PointInc
		Write32(pRaceScore, 0);						 // TotalPoint
		Write32(pRaceScore, 0);						 // ExpInc
		Write32(pRaceScore, 0);						 // TotalExp
		Write32(pRaceScore, 0);						 // SuperMoneyInc
		Write32(pRaceScore, 0);						 // TotalSuperMoney
		Write32(pRaceScore, 0);						 // CoolMoneyInc
		Write32(pRaceScore, 0);						 // TotalCoolMoney
		Write32(pRaceScore, 0);						 // PropPoint
		Write32(pRaceScore, 0);						 // RacePoint
		Write32(pRaceScore, 0);						 // PRPoint
		Write32(pRaceScore, 0);						 // DistanceToEnd
		Write32(pRaceScore, 0);						 // DetailRecordID

		len = pRaceScore - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // InfoLen
	Write8(p, 0); // KickOffPlayerNumber
	for (size_t i = 0; i < 0; i++)
	{ // KickOffPlayer
		BYTE *pKickOffPlayer = p;
		Write16(pKickOffPlayer, 0); // len

		Write32(pKickOffPlayer, 0);		// Uin
		WriteString(pKickOffPlayer, 0); // NickName[]
		Write8(pKickOffPlayer, 0);		// Gender
		Write8(pKickOffPlayer, 0);		// TeamID
		Write8(pKickOffPlayer, 0);		// Country

		len = pKickOffPlayer - p;
		Set16(p, (WORD)len);
		p += len;
	}
	WriteUn(p, 1); // hasGameFrameInfo
	for (size_t i = 0; i < 1; i++)
	{ // GameFrameInfo
		BYTE *pGameFrameInfo = p;
		Write16(pGameFrameInfo, 0); // len

		Write32(pGameFrameInfo, Client->Uin);	// Uin
		Write32(pGameFrameInfo, Client->MapID); // MapID
		Write16(pGameFrameInfo, 0);				// FrameNum
		for (size_t i = 0; i < 0; i++)
		{ // FrameInfo
			BYTE *pFrameInfo = pGameFrameInfo;
			Write16(pFrameInfo, 0); // len

			Write16(pFrameInfo, 0); // CheckPointID
			Write32(pFrameInfo, 0); // Num
			for (size_t i = 0; i < 0; i++)
			{ // FrameList
				BYTE *pFrameList = pFrameInfo;
				Write16(pFrameList, 0); // len

				WriteUn(pFrameList, 0); // TimeElaps
				WriteUn(pFrameList, 0); // fInPosX[]
				WriteUn(pFrameList, 0); // fInPosY[]
				WriteUn(pFrameList, 0); // fInPosZ[]
				WriteUn(pFrameList, 0); // fStepX[]
				WriteUn(pFrameList, 0); // fStepY[]
				WriteUn(pFrameList, 0); // fStepZ[]
				WriteUn(pFrameList, 0); // fCarMat[]
				WriteUn(pFrameList, 0); // fOutPosX[]
				WriteUn(pFrameList, 0); // fOutPosY[]
				WriteUn(pFrameList, 0); // fOutPosZ[]
				WriteUn(pFrameList, 0); // fAccTime[]
				WriteUn(pFrameList, 0); // fFrameTime[]
				Write8(pFrameList, 0);	// SetPos
				Write8(pFrameList, 0);	// Collision

				len = pFrameList - pFrameInfo;
				Set16(pFrameInfo, (WORD)len);
				pFrameInfo += len;
			}

			len = pFrameInfo - pGameFrameInfo;
			Set16(pGameFrameInfo, (WORD)len);
			pGameFrameInfo += len;
		}

		len = pGameFrameInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 564, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifySingleGameOver(ClientNode *Client)
{
	Client->MapID = 0;
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 0); // ResultID

	len = p - buf;
	SendToClient(Client, 534, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
const static unsigned char *pMsg533 = EHexToByte("00002711001300000049600000000000000005000000000001E09E007400000000000F4B9F000000000000199600000000001309BA00000001001900003379000000030000000000646C807D00000000000000000000000000320000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
void NotifySingleRaceOver(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	// Write32(p, 0); //Uin
	//{ //TaskStatusInfo
	//	BYTE* pTaskStatusInfo = p;
	//	Write16(pTaskStatusInfo, 0); //len

	//	Write32(pTaskStatusInfo, 0); //TaskID
	//	Write8(pTaskStatusInfo, 0); //Status
	//	Write32(pTaskStatusInfo, 0); //ProgressRate
	//	Write32(pTaskStatusInfo, 0); //Other
	//	Write32(pTaskStatusInfo, 0); //LastUpdateTime

	//	len = pTaskStatusInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	// Write32(p, 0); //FinTime
	//{ //TaskAward
	//	BYTE* pTaskAward = p;
	//	Write16(pTaskAward, 0); //len

	//	Write32(pTaskAward, 0); //AwardMoney
	//	Write32(pTaskAward, 0); //TotalMoney
	//	Write32(pTaskAward, 0); //AwardSuperMoney
	//	Write32(pTaskAward, 0); //TotalSuperMoney
	//	Write32(pTaskAward, 0); //AwardExp
	//	Write32(pTaskAward, 0); //TotalExp
	//	Write32(pTaskAward, 0); //AwardItemNum
	//	for (size_t i = 0; i < n; i++)
	//	{ //AwardItemInfo
	//		BYTE* pAwardItemInfo = pTaskAward;
	//		Write16(pAwardItemInfo, 0); //len

	//		Write32(pAwardItemInfo, 0); //ItemID
	//		Write32(pAwardItemInfo, 0); //ItemNum
	//		Write32(pAwardItemInfo, 0); //AvailPeriod
	//		Write8(pAwardItemInfo, 0); //Status
	//		WriteUn(pAwardItemInfo, 0); //ObtainTime
	//		Write32(pAwardItemInfo, 0); //OtherAttribute
	//		Write16(pAwardItemInfo, 0); //ItemType

	//		len = pAwardItemInfo - pTaskAward;
	//		Set16(pTaskAward, (WORD)len);
	//		pTaskAward += len;
	//	}
	//	Write32(pTaskAward, 0); //AwardCoupons
	//	Write32(pTaskAward, 0); //TotalCoupons
	//	Write32(pTaskAward, 0); //AwardLuckMoney
	//	Write32(pTaskAward, 0); //TotalLuckMoney
	//	Write32(pTaskAward, 0); //AwardTeamWorkExp
	//	Write32(pTaskAward, 0); //TotalTeamWorkExp
	//	Write32(pTaskAward, 0); //AwardWlCash
	//	Write32(pTaskAward, 0); //AwardWlCrystal
	//	Write32(pTaskAward, 0); //AwardWlDegree
	//	Write32(pTaskAward, 0); //ChumCircleActivity
	//	Write32(pTaskAward, 0); //GuildScore
	//	Write16(pTaskAward, 0); //AppellcationID
	//	Write16(pTaskAward, 0); //ScoreJingJi
	//	Write8(pTaskAward, 0); //ExtendInfoNum
	//	for (size_t i = 0; i < n; i++)
	//	{ //ExtendInfoAward
	//		BYTE* pExtendInfoAward = pTaskAward;
	//		Write16(pExtendInfoAward, 0); //len

	//		Write16(pExtendInfoAward, 0); //Key
	//		Write32(pExtendInfoAward, 0); //AddValue
	//		Write32(pExtendInfoAward, 0); //TotalValue
	//		Write8(pExtendInfoAward, 0); //ShowPriority
	//		Write16(pExtendInfoAward, 0); //BuyNeedScore
	//		Write8(pExtendInfoAward, 0); //OneMatchMaxNum

	//		len = pExtendInfoAward - pTaskAward;
	//		Set16(pTaskAward, (WORD)len);
	//		pTaskAward += len;
	//	}
	//	Write32(pTaskAward, 0); //LiveExp
	//	Write32(pTaskAward, 0); //AwardSpeedCoin
	//	Write32(pTaskAward, 0); //TotalSpeedCoin

	//	len = pTaskAward - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	// Write32(p, 0); //ProfessionTime
	// Write8(p, 0); //ProfessionIDStrLen
	// Write8(p, 0); //HaveRankedMatchSeasonMapChallengeInfo
	// for (size_t i = 0; i < n; i++)
	//{ //RankedMatchSeasonMapChallengeInfo
	//	BYTE* pRankedMatchSeasonMapChallengeInfo = p;
	//	Write16(pRankedMatchSeasonMapChallengeInfo, 0); //len

	//	Write32(pRankedMatchSeasonMapChallengeInfo, 0); //MapID
	//	Write32(pRankedMatchSeasonMapChallengeInfo, 0); //MapIndex
	//	Write32(pRankedMatchSeasonMapChallengeInfo, 0); //MaxFinishiTime
	//	Write16(pRankedMatchSeasonMapChallengeInfo, 0); //ChallengeLevelNums
	//	for (size_t i = 0; i < n; i++)
	//	{ //ChallengeInfos
	//		BYTE* pChallengeInfos = pRankedMatchSeasonMapChallengeInfo;
	//		Write16(pChallengeInfos, 0); //len

	//		Write16(pChallengeInfos, 0); //ChallengeLevel
	//		Write32(pChallengeInfos, 0); //FinishTime
	//		Write8(pChallengeInfos, 0); //FinishOrNot
	//		Write32(pChallengeInfos, 0); //AddRankedScore
	//		Write32(pChallengeInfos, 0); //AddRankedCoinNums

	//		len = pChallengeInfos - pRankedMatchSeasonMapChallengeInfo;
	//		Set16(pRankedMatchSeasonMapChallengeInfo, (WORD)len);
	//		pRankedMatchSeasonMapChallengeInfo += len;
	//	}
	//	Write8(pRankedMatchSeasonMapChallengeInfo, 0); //PromotionExtraScore

	//	len = pRankedMatchSeasonMapChallengeInfo - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}
	// Write8(p, 0); //TriigerRankedMatchPromotion

	// len = p - buf;
	SendToClient(Client, 533, pMsg533, 152, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
}
void NotifySingleRaceFinish(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 0); // ResultID

	len = p - buf;
	SendToClient(Client, 532, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyCountDown(ClientNode *Client, UINT WinnerUin, bool WinnerNewRecord, UINT FinTime)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;
	Write32(p, 10000);			// CountDownTime
	Write32(p, WinnerUin);		// WinnerUin
	Write8(p, WinnerNewRecord); // WinnerNewRecord
	Write32(p, FinTime);		// FinTime

	len = p - buf;
	SendToClient(Client, 512, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
	NotifyFeedBackData(Client);
}

void NotifyRaceShow(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, 1000); // RaceShowTime
	Write8(p, 0);	  // HasFBInfo
					  /*
				  m_stFBInfo[].m_bFBResult
				  m_stFBInfo[].m_uiKiller
					  */

#ifndef ZingSpeed
	Write8(p, 0);  // WinTeamID
	Write32(p, 0); // EventID

	Write32(p, 0); // ParaNum
				   // m_auiPara[]

#endif

	len = p - buf;
	SendToClient(Client, 515, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}
int *GetScoreSort(RoomNode *Room);
int *GetScoreSort(RoomNode *Room)
{
	return 0;
	/*int index = 0;
	int *Scoresort = new int[6];

	for (size_t i = 0; i < Room->TotalPlayerNum; i++)
	{
		ClientNode *Player = &Room->PlayerReal[i];
		if (Player)
		{
			Scoresort[index] = i;
			index++;
		}
	}

	for (int i = 0; i < index; ++i)
	{
		bool isSwap = false;
		for (int j = index - 1; j > i; --j)
		{
			if (Room->PlayerReal[Scoresort[j]].FinishTime <= 0 && Room->PlayerReal[Scoresort[j - 1]].FinishTime <= 0)
			{
				if (Room->PlayerReal[Scoresort[j]].Round > Room->PlayerReal[Scoresort[j - 1]].Round)
				{
					std::swap(Scoresort[j], Scoresort[j - 1]);
					isSwap = true;
				}
				else if (Room->PlayerReal[Scoresort[j]].PassedCheckPoint > Room->PlayerReal[Scoresort[j - 1]].PassedCheckPoint)
				{
					std::swap(Scoresort[j], Scoresort[j - 1]);
					isSwap = true;
				}
			}
			else if (Room->PlayerReal[Scoresort[j]].FinishTime > 0 && Room->PlayerReal[Scoresort[j]].FinishTime <= Room->PlayerReal[Scoresort[j - 1]].FinishTime)
			{
				std::swap(Scoresort[j], Scoresort[j - 1]);
				isSwap = true;
			}
		}

		if (isSwap == false)
		{
			break;
		}
	}

	return Scoresort;*/
}
void NotifyRaceOverNew(ClientNode* Client)
{// Race over get award
	
	
	struct PlayerRaceInfo
	{
		UINT Uin;
		//UINT Round;
		UINT FinishTime;
	};
	RoomNode* room = GetRoom(Client->RoomID);
	if (Client->matchQueueType>0) {
		room =  GetRoom(Client->BorderRoomID);
	}
	room->startStatus = 1;
	UINT PlayerNum = 0;
	int Honor = 0;
	int SpeedCoin = 0;
	//mMySQL msql;

	PlayerRaceInfo ArrPlayerRaceInfo[12] = {};
	for (int i = 0; i < 6; i++)
	{
		PlayerRaceInfo tmp;
		tmp.Uin = -1;
		tmp.FinishTime = -1;
		ArrPlayerRaceInfo[i] = tmp;

		if (room->Player[i])
		{
			//完成时间如果是0还是会作为结算成绩进行排名
			//改为-1就是未完成了
			if (room->Player[i]->FinishTime == 0)room->Player[i]->FinishTime = -1;

			ArrPlayerRaceInfo[i].Uin = room->Player[i]->Uin;
			//ArrPlayerRaceInfo[i].Round = room->Player[i]->Round;
			ArrPlayerRaceInfo[i].FinishTime = room->Player[i]->FinishTime;

			PlayerNum++;
		}
	}
	for (int i = 0; i < 6; i++)
	{
		PlayerRaceInfo tmp;
		tmp.Uin = -1;
		tmp.FinishTime = -1;
		ArrPlayerRaceInfo[i+6] = tmp;

		if (room->NPCPlayer[i])
		{
			//完成时间如果是0还是会作为结算成绩进行排名
			//改为-1就是未完成了
			if (room->NPCPlayer[i]->FinishTime == 0)room->NPCPlayer[i]->FinishTime = -1;

			ArrPlayerRaceInfo[i + 6].Uin = room->NPCPlayer[i]->NpcID;
			//ArrPlayerRaceInfo[i].Round = room->Player[i]->Round;
			ArrPlayerRaceInfo[i + 6].FinishTime = room->NPCPlayer[i]->FinishTime;

			PlayerNum++;
		}
	}
	auto cmp_PlayerRaceInfo = [](PlayerRaceInfo& a, PlayerRaceInfo& b) -> bool
	{
		if (a.FinishTime== b.FinishTime) {
			return  a.Uin < b.Uin;
		}
		else {
			return  a.FinishTime < b.FinishTime;
		}
		
	};

	//名次排序
	std::sort(ArrPlayerRaceInfo, ArrPlayerRaceInfo + 12, cmp_PlayerRaceInfo);
	BYTE buf[8192];
	BYTE* p = buf;
	size_t len;
	const char* sql = NULL;
	sqlite3_stmt* stmt = NULL;
	int result;
	int TotalExp = 0;
	Write8(p, PlayerNum); //CurrentPlayerNum
	for (int i = 0; i < PlayerNum; i++)
	{ //RaceScore
		if (ArrPlayerRaceInfo[i].Uin == Client->Uin) {
			UINT Experience = getGameConfig()["RaceAward"]["Experience"].as<UINT>(); // 经验
			UINT Money = getGameConfig()["RaceAward"]["Money"].as<UINT>();			 // 酷币
			UINT SuperMoney = getGameConfig()["RaceAward"]["SuperMoney"].as<UINT>(); // 点券
			UINT Coupons = getGameConfig()["RaceAward"]["Coupons"].as<UINT>();		 // 消费券
			UINT LuckMoney = getGameConfig()["RaceAward"]["LuckMoney"].as<UINT>();	 // 幸运币

			Client->Experience += Experience;
			Client->Money += Money;
			Client->SuperMoney += SuperMoney;
			Client->Coupons += Coupons;
			Client->LuckMoney += LuckMoney;
			PlayerDB_UpdateMoneyExp(Client);
		}
		BYTE* pRaceScore = p;
		Write16(pRaceScore, 0); //len

		Write32(pRaceScore, ArrPlayerRaceInfo[i].Uin); //Uin
		Write32(pRaceScore, ArrPlayerRaceInfo[i].FinishTime); //FinTime
		Write32(pRaceScore, 0); //TP
		Write32(pRaceScore, 0); //MoneyInc		增加酷币
		Write32(pRaceScore, 0); //TotalMoney
		Write32(pRaceScore, 60 + 60 / (i + 1)); //ExpInc			增加经验
		sql = "SELECT Experience  FROM BaseInfo  WHERE Uin=?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, ArrPlayerRaceInfo[i].Uin);
			result = sqlite3_step(stmt);
			if (result == SQLITE_ROW)
			{
				TotalExp = sqlite3_column_int(stmt, 0);
			}
		}
		sqlite3_finalize(stmt);
		stmt = NULL;
		Write32(pRaceScore, TotalExp); //TotalExp

		Write8(pRaceScore, 10); // AwardNum
		// m_iSpecialAward[]
		//Write32(pRaceScore, 1); // m_iSpecialAward QQ网吧
		//Write32(pRaceScore, 2); // m_iSpecialAward 双倍经验
		Write32(pRaceScore, 3); // m_iSpecialAward 紫钻55
		//Write32(pRaceScore, 4); // m_iSpecialAward 好友加成
		//Write32(pRaceScore, 5); // m_iSpecialAward 装备加成
		Write32(pRaceScore, 6); // m_iSpecialAward 宠物加成
		Write32(pRaceScore, 7); // m_iSpecialAward 刷新纪录
		Write32(pRaceScore, 8); // m_iSpecialAward 金牌网吧
		//Write32(pRaceScore, 9); // m_iSpecialAward 蓝钻20
		//Write32(pRaceScore, 10); // m_iSpecialAward 雪球
		//Write32(pRaceScore, 11); // m_iSpecialAward 经验减半
		//Write32(pRaceScore, 12); // m_iSpecialAward 双倍酷比
		//Write32(pRaceScore, 13); // m_iSpecialAward 车队加成
		//Write32(pRaceScore, 14); // m_iSpecialAward 师徒加成
		//Write32(pRaceScore, 15); // m_iSpecialAward 挑战加成
		Write32(pRaceScore, 16); // m_iSpecialAward 活跃加成
		//Write32(pRaceScore, 17); // m_iSpecialAward 舞蹈加成
		//Write32(pRaceScore, 18); // m_iSpecialAward 网吧5
		//Write32(pRaceScore, 19); // m_iSpecialAward 网吧10
		//Write32(pRaceScore, 20); // m_iSpecialAward 网吧15
		//Write32(pRaceScore, 21); // m_iSpecialAward 网吧20
		//Write32(pRaceScore, 22); // m_iSpecialAward 会员20
		//Write32(pRaceScore, 23); // m_iSpecialAward 车队福利
		//Write32(pRaceScore, 24); // m_iSpecialAward 连续登录
		//Write32(pRaceScore, 26); // m_iSpecialAward 宠物经验
		Write32(pRaceScore, 26); // m_iSpecialAward 伴侣加成
		//Write32(pRaceScore, 28); // m_iSpecialAward 管家20
		//Write32(pRaceScore, 29); // m_iSpecialAward 黄钻加成
		Write32(pRaceScore, 30); // m_iSpecialAward 车神加成
		Write32(pRaceScore, 31); // m_iSpecialAward 皇族加成
		//Write32(pRaceScore, 32); // m_iSpecialAward 四倍经验
		//Write32(pRaceScore, 33); // m_iSpecialAward 精灵技能
		Write32(pRaceScore, 34); // m_iSpecialAward 车队贵族
		Write32(pRaceScore, 35); // m_iSpecialAward 情侣贵族
		//Write32(pRaceScore, 36); // m_iSpecialAward 金币加成
		//Write32(pRaceScore, 37); // m_iSpecialAward 天天升级
		//Write32(pRaceScore, 38); // m_iSpecialAward 勋章加成
		//Write32(pRaceScore, 39); // m_iSpecialAward 经验三倍
		//Write32(pRaceScore, 40); // m_iSpecialAward 经验四倍
		//Write32(pRaceScore, 41); // m_iSpecialAward 经验6倍
		//Write32(pRaceScore, 42); // m_iSpecialAward 经验8倍
		//Write32(pRaceScore, 48); // m_iSpecialAward 经验10倍
		//Write32(pRaceScore, 49); // m_iSpecialAward 经验12倍
		//Write32(pRaceScore, 50); // m_iSpecialAward 经验15倍

		Write32(pRaceScore, 0); //TeamWorkExpInc
		Write32(pRaceScore, 0); //PropPoint
		Write32(pRaceScore, 0); //PropPointAddExp
		Write32(pRaceScore, 0); //LuckyMatchPointInc
		Write32(pRaceScore, 0); //LuckyMatchPointTotal
		Write32(pRaceScore, 0); //LuckyMatchScoreInc
		Write32(pRaceScore, 0); //LuckyMatchScoreTotal
		Write32(pRaceScore, 0); //LuckMoneyInc
		Write32(pRaceScore, 0); //LuckMoneyTotal
		Write32(pRaceScore, 0); //GuildScoreInc

		Write8(pRaceScore, 0); //CrazyPropAchieveNum
		//m_aiCrazyPropAchieve[]

		Write32(pRaceScore, 0); //IncWlMatchScore
		Write32(pRaceScore, 0); //IncWlDegree
		Write8(pRaceScore, 0); //IncItemNumByWl
		/*
m_astRaceScore[].m_astIncItemInfoByWl[].m_shGenderItemNum
m_astRaceScore[].m_astIncItemInfoByWl[].m_aiItemID[EGT_MAX]
m_astRaceScore[].m_astIncItemInfoByWl[].m_shItemNum
m_astRaceScore[].m_astIncItemInfoByWl[].m_iAvailPeriod
m_astRaceScore[].m_astIncItemInfoByWl[].m_usBroadcastType
m_astRaceScore[].m_astIncItemInfoByWl[].m_usBroadcastFlag
m_astRaceScore[].m_astIncItemInfoByWl[].m_ucShowPriority
m_astRaceScore[].m_astIncItemInfoByWl[].m_uchItemType
m_astRaceScore[].m_astIncItemInfoByWl[].m_uchIgnoreItem
m_astRaceScore[].m_astIncItemInfoByWl[].m_uiLimitPara
m_astRaceScore[].m_astIncItemInfoByWl[].m_ucSpecialAwardCtrlType
m_astRaceScore[].m_astIncItemInfoByWl[].m_ucAwardCtrlType
		*/

		Write32(pRaceScore, 0); //WlMutiplyCard
		Write32(pRaceScore, 0); //SkateCoinInc
		Write32(pRaceScore, 0); //SkateCoinTotal
		Write32(pRaceScore, 0); //SkateCoinHistoryTotal
		Write32(pRaceScore, 0); //TotalCoupons


		Write8(pRaceScore, 0); //ChallengeCheer
		/*
m_astRaceScore[].m_stChallengeCheer[].m_ushGuildCheerValueInc
m_astRaceScore[].m_stChallengeCheer[].m_ushCheerValueInc
m_astRaceScore[].m_stChallengeCheer[].m_uiTotalCheerValue
m_astRaceScore[].m_stChallengeCheer[].m_ushCouponsInc
m_astRaceScore[].m_stChallengeCheer[].m_iGuildScoreInc
m_astRaceScore[].m_stChallengeCheer[].m_iSuperMoneyInc
		*/

		Write32(pRaceScore, 0); //LoveValue
		Write32(pRaceScore, 0); //SkateCoinEmperorBonus
		Write32(pRaceScore, 0); //DetailRecordID

		Write8(pRaceScore, 0); //HasGangsterResult
		/*
m_astRaceScore[].m_astGangsterResult[].m_cAbnormalFalg
m_astRaceScore[].m_astGangsterResult[].m_uiGangsterBlood
m_astRaceScore[].m_astGangsterResult[].m_uiGangsterTotalScore
m_astRaceScore[].m_astGangsterResult[].m_uiGangsterIndentity
m_astRaceScore[].m_astGangsterResult[].m_uiFragmentInc
m_astRaceScore[].m_astGangsterResult[].m_uiKillTimes
m_astRaceScore[].m_astGangsterResult[].m_uiDeadTimes
m_astRaceScore[].m_astGangsterResult[].m_ucIsMultiple
m_astRaceScore[].m_astGangsterResult[].m_ucIsLimited
m_astRaceScore[].m_astGangsterResult[].m_iScoreInc
m_astRaceScore[].m_astGangsterResult[].m_iTotalScore
m_astRaceScore[].m_astGangsterResult[].m_iLoseStreakCount
		*/
		{ // EquippedActiveKartInfo
			BYTE* pEquippedActiveKartInfo = pRaceScore;
			Write16(pEquippedActiveKartInfo, 0); // len

			Write8(pEquippedActiveKartInfo, 1);	 // HaveActiveInfo
			Write32(pEquippedActiveKartInfo, Client->KartID); // KartID
			Write32(pEquippedActiveKartInfo, 10); // ActiveLevel

			len = pEquippedActiveKartInfo - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}


		Write8(pRaceScore, 0); //HasWeRelayRaceOverInfo
		/*
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Score
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32ScoreInc
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Supermoney
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Coupons
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32SuperN2O
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreInc
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreLevelNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreLevelLimit
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiSupermoneyLimit
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiSupermoneyNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiCouponsLimit
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiCouponsNow
	m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiLuckAwardMul
		*/

		Write8(pRaceScore, 0); //HaveSkatePropRaceInfo
		/*
	m_astRaceScore[].m_stSkatePropRaceInfo[].m_uiScoreAdd
	m_astRaceScore[].m_stSkatePropRaceInfo[].m_uiScoreTotal
		*/

		Write8(pRaceScore, 0); //IsNewBox


		Write8(pRaceScore, 0); //HaveArrestScoreInfo
		/*
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiScoreInGame
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiScoreByGold
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiGoldMoney
	m_astRaceScore[].m_stArrestScoreInfo[].m_uiArrestNum
	m_astRaceScore[].m_stArrestScoreInfo[].m_uchResultType
	m_astRaceScore[].m_stArrestScoreInfo[].m_bTeamID
		*/

		Write8(pRaceScore, 0); //HasRankedMatchInfo
		for (size_t i = 0; i < 0; i++)
		{ //RankedMatchScore
			BYTE* pRankedMatchScore = pRaceScore;
			Write16(pRankedMatchScore, 0); //len

			{ //RankedMatchLastGradeInfo
				BYTE* pRankedMatchLastGradeInfo = pRankedMatchScore;
				Write16(pRankedMatchLastGradeInfo, 0); //len

				Write32(pRankedMatchLastGradeInfo, 0); //GradeLevel
				Write32(pRankedMatchLastGradeInfo, 0); //ChildLevel
				Write32(pRankedMatchLastGradeInfo, 0); //GradeScore
				Write32(pRankedMatchLastGradeInfo, 0); //MaxGradeScore
				Write32(pRankedMatchLastGradeInfo, 0); //TotalScore

				len = pRankedMatchLastGradeInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			{ //RankedMatchGradeInfo
				BYTE* pRankedMatchGradeInfo = pRankedMatchScore;
				Write16(pRankedMatchGradeInfo, 0); //len

				Write32(pRankedMatchGradeInfo, 0); //GradeLevel
				Write32(pRankedMatchGradeInfo, 0); //ChildLevel
				Write32(pRankedMatchGradeInfo, 0); //GradeScore
				Write32(pRankedMatchGradeInfo, 0); //MaxGradeScore
				Write32(pRankedMatchGradeInfo, 0); //TotalScore

				len = pRankedMatchGradeInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			{ //RankedMatchPromoInfo
				BYTE* pRankedMatchPromoInfo = pRankedMatchScore;
				Write16(pRankedMatchPromoInfo, 0); //len

				Write8(pRankedMatchPromoInfo, 0); //PromotionMatch
				Write32(pRankedMatchPromoInfo, 0); //PromotionRoundNums
				Write32(pRankedMatchPromoInfo, 0); //PromotionSuccessRoundNums
				Write32(pRankedMatchPromoInfo, 0); //PromotionAlreadySuccessRoundNums
				Write32(pRankedMatchPromoInfo, 0); //PromotionRecordTag
				Write32(pRankedMatchPromoInfo, 0); //CurrentPromotionRoundNums
				Write32(pRankedMatchPromoInfo, 0); //PromoFailedMinusScore
				Write32(pRankedMatchPromoInfo, 0); //PromoSuccAddScore
				Write8(pRankedMatchPromoInfo, 0); //PromoDirectly
				Write8(pRankedMatchPromoInfo, 0); //GradeNoNeedPromo
				Write8(pRankedMatchPromoInfo, 0); //RankedMemoryNoNeedPromo
				Write8(pRankedMatchPromoInfo, 0); //PromotionExtraScore
				Write8(pRankedMatchPromoInfo, 0); //PromtRaceFailedDefendFlag

				len = pRankedMatchPromoInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			{ //RankedMatchComputeInfo
				BYTE* pRankedMatchComputeInfo = pRankedMatchScore;
				Write16(pRankedMatchComputeInfo, 0); //len

				Write32(pRankedMatchComputeInfo, 0); //RankAddScore
				Write32(pRankedMatchComputeInfo, 0); //FinishAddScore
				Write32(pRankedMatchComputeInfo, 0); //KillAllAddScore
				Write32(pRankedMatchComputeInfo, 0); //GradeProtectAddScore
				Write32(pRankedMatchComputeInfo, 0); //GradeAddScore
				Write32(pRankedMatchComputeInfo, 0); //ProtectAddScoreGradeLimit
				Write32(pRankedMatchComputeInfo, 0); //DailyExtraAddScore
				Write32(pRankedMatchComputeInfo, 0); //AwardRankCoinNums
				Write32(pRankedMatchComputeInfo, 0); //WeeklyAwardRankCoinNums
				Write32(pRankedMatchComputeInfo, 0); //WeeklyAwardRankCoinLimit
				Write32(pRankedMatchComputeInfo, 0); //AwardRankCoinNumsByRank
				Write32(pRankedMatchComputeInfo, 0); //AwardRankCoinNumsByFinish
				Write32(pRankedMatchComputeInfo, 0); //BanMapExtraAddScore
				Write32(pRankedMatchComputeInfo, 0); //TeamRankAddScore
				Write32(pRankedMatchComputeInfo, 0); //AwardRankCoinNumsByTeamRank
				Write32(pRankedMatchComputeInfo, 0); //AwardRankCoinNumsByTask
				Write8(pRankedMatchComputeInfo, 0); //UseDoubleScoreItem
				Write8(pRankedMatchComputeInfo, 0); //UserNotDecreaseItem
				Write8(pRankedMatchComputeInfo, 0); //FinishSeasonChallengeTaskOrNot
				Write32(pRankedMatchComputeInfo, 0); //FinishedSeasonChallengeTaskIndex
				Write32(pRankedMatchComputeInfo, 0); //SeasonChallengeTaskProtectAddScore
				Write8(pRankedMatchComputeInfo, 0); //NotDecreaseReason
				Write8(pRankedMatchComputeInfo, 0); //HaveSpeedKingInfo
				for (size_t i = 0; i < 0; i++)
				{ //RankedMatchSpeedKingInfos
					BYTE* pRankedMatchSpeedKingInfos = pRankedMatchComputeInfo;
					Write16(pRankedMatchSpeedKingInfos, 0); //len

					Write8(pRankedMatchSpeedKingInfos, 0); //WinOrNot
					WriteUn(pRankedMatchSpeedKingInfos, 0); //UiAwardSpeedKingEnergyItemNums
					Write32(pRankedMatchSpeedKingInfos, 0); //WeeklyAwardSpeedKingEnergyItemNums
					Write32(pRankedMatchSpeedKingInfos, 0); //WeeklyAwardSpeedKingEnergyItemLimitNums

					len = pRankedMatchSpeedKingInfos - pRankedMatchComputeInfo;
					Set16(pRankedMatchComputeInfo, (WORD)len);
					pRankedMatchComputeInfo += len;
				}
				Write8(pRankedMatchComputeInfo, 0); //HaveExtraComputeInfo
				for (size_t i = 0; i < 0; i++)
				{ //RankedMatchExtracComputeInfos
					BYTE* pRankedMatchExtracComputeInfos = pRankedMatchComputeInfo;
					Write16(pRankedMatchExtracComputeInfos, 0); //len

					Write32(pRankedMatchExtracComputeInfos, 0); //ExtraComputeType
					Write32(pRankedMatchExtracComputeInfos, 0); //AwardItemNums
					Write32(pRankedMatchExtracComputeInfos, 0); //WeekylyAwardItemNums
					Write32(pRankedMatchExtracComputeInfos, 0); //WeeklyAwardItemLimitNums
					Write8(pRankedMatchExtracComputeInfos, 0); //HaveGameLogicTask
					for (size_t i = 0; i < 0; i++)
					{ //RakedMatchGamelogicTaskInfo
						BYTE* pRakedMatchGamelogicTaskInfo = pRankedMatchExtracComputeInfos;
						Write16(pRakedMatchGamelogicTaskInfo, 0); //len

						Write32(pRakedMatchGamelogicTaskInfo, 0); //GameLogicTaskType
						Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskFinishCondValue
						Write8(pRakedMatchGamelogicTaskInfo, 0); //JudgeRule
						Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskAddGradeScore
						Write8(pRakedMatchGamelogicTaskInfo, 0); //GradeScoreRangeNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); //GradeScoreRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0); //TaskFinishCondValueNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskFinishCondValueRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0); //TaskAddGradeScoreNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskAddGradeScoreRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0); //FailedAddGradeScoreNum
						Write32(pRakedMatchGamelogicTaskInfo, 0); //FailedAddGradeScoreRange[]
						Write8(pRakedMatchGamelogicTaskInfo, 0); //FinishTask
						Write32(pRakedMatchGamelogicTaskInfo, 0); //ExtraFinishTaskAddScore

						len = pRakedMatchGamelogicTaskInfo - pRankedMatchExtracComputeInfos;
						Set16(pRankedMatchExtracComputeInfos, (WORD)len);
						pRankedMatchExtracComputeInfos += len;
					}
					Write8(pRankedMatchExtracComputeInfos, 0); //Win
					Write8(pRankedMatchExtracComputeInfos, 0); //NormalPlayerOrNot
					Write8(pRankedMatchExtracComputeInfos, 0); //LeadRolePlayerNums

					len = pRankedMatchExtracComputeInfos - pRankedMatchComputeInfo;
					Set16(pRankedMatchComputeInfo, (WORD)len);
					pRankedMatchComputeInfo += len;
				}
				Write32(pRankedMatchComputeInfo, 0); //SeasonNewMapAddScore
				Write8(pRankedMatchComputeInfo, 0); //SeasonChallengeBaseScore
				Write8(pRankedMatchComputeInfo, 0); //SeasonChallengeContiWinExtrScore
				Write8(pRankedMatchComputeInfo, 0); //SeasonChallengeRegretDefeatScore
				Write8(pRankedMatchComputeInfo, 0); //SeasonChallengeRespectableOpponentScore
				Write8(pRankedMatchComputeInfo, 0); //SpeedGodChallengeAddScore
				Write8(pRankedMatchComputeInfo, 0); //SpeedGodChallengeAddCoin
				Write8(pRankedMatchComputeInfo, 0); //SpeedFlashAddFactoryCoin
				Write8(pRankedMatchComputeInfo, 0); //PinnacleBattleAddScore
				Write8(pRankedMatchComputeInfo, 0); //PromotionExtraScore
				Write32(pRankedMatchComputeInfo, 0); //SavedExtraScore
				Write32(pRankedMatchComputeInfo, 0); //DailyExtraTaskID
				Write8(pRankedMatchComputeInfo, 0); //SuitActiveAddScore
				Write8(pRankedMatchComputeInfo, 0); //SpeedGodChallengeType
				Write8(pRankedMatchComputeInfo, 0); //SuitActiveEffectID

				len = pRankedMatchComputeInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}
			Write8(pRankedMatchScore, 0); //HavePromotionExtraScoreInfo
			for (size_t i = 0; i < 0; i++)
			{ //PromotionExtraScoreInfo
				BYTE* pPromotionExtraScoreInfo = pRankedMatchScore;
				Write16(pPromotionExtraScoreInfo, 0); //len

				Write8(pPromotionExtraScoreInfo, 0); //PromotionMatch
				Write32(pPromotionExtraScoreInfo, 0); //PromotionRoundNums
				Write32(pPromotionExtraScoreInfo, 0); //PromotionSuccessRoundNums
				Write32(pPromotionExtraScoreInfo, 0); //PromotionAlreadySuccessRoundNums
				Write32(pPromotionExtraScoreInfo, 0); //PromotionRecordTag
				Write32(pPromotionExtraScoreInfo, 0); //CurrentPromotionRoundNums
				Write32(pPromotionExtraScoreInfo, 0); //PromoFailedMinusScore
				Write32(pPromotionExtraScoreInfo, 0); //PromoSuccAddScore
				Write8(pPromotionExtraScoreInfo, 0); //PromoDirectly
				Write8(pPromotionExtraScoreInfo, 0); //GradeNoNeedPromo
				Write8(pPromotionExtraScoreInfo, 0); //RankedMemoryNoNeedPromo
				Write8(pPromotionExtraScoreInfo, 0); //PromotionExtraScore
				Write8(pPromotionExtraScoreInfo, 0); //PromtRaceFailedDefendFlag

				len = pPromotionExtraScoreInfo - pRankedMatchScore;
				Set16(pRankedMatchScore, (WORD)len);
				pRankedMatchScore += len;
			}

			len = pRankedMatchScore - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}

		Write8(pRaceScore, 0); //HaveCrazyChaseScoreInfo
		/*
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uiUin
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_shRole
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uiScore
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uchScoreNum
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_uchScoreType
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_iValue
	m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_iScore
		*/
		Write8(pRaceScore, 0); //TeamID

		Write8(pRaceScore, 0); //HasRankedMatchArenaInfo
		/*
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucWin
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucRank
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucTaskScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iAwardRankCoinNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiGradeLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiChildLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiMaxGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiTotalScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiGradeLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiChildLevel
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiMaxGradeScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiTotalScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromotionMatch
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionSuccessRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionAlreadySuccessRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionRecordTag
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiCurrentPromotionRoundNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromoFailedMinusScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromoSuccAddScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromoDirectly
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucGradeNoNeedPromo
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_bRankedMemoryNoNeedPromo
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uchPromotionExtraScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromtRaceFailedDefendFlag
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucWinNum
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_sCurrentScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucFinishScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_usAwardRankScore
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucAwardRankCoinNumsByRank
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucAwardRankCoinNumsByFinish
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iWeeklyAwardRankCoinNums
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iWeeklyAwardRankCoinLimit
	m_astRaceScore[].m_stRankedMatchArenaScore[].m_iAwardRankCoinNumsByTask
		*/


		Write32(pRaceScore, 0); //DistanceToEnd
		Write8(pRaceScore, 0); //ShortDistancWinPoint
		Write8(pRaceScore, 0); //Status


		Write8(pRaceScore, 0); //HaveRankedMatchSpeedKingInfo
		/*
	m_astRaceScore[].m_stRankedMatchSpeedKingScoreInfo[].m_uiAwardSpeedKingEnergyNums
	m_astRaceScore[].m_stRankedMatchSpeedKingScoreInfo[].m_ucWin
		*/


		Write8(pRaceScore, 0); //ProfessionLicenseAwardNum
		/*
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointInc
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointLevelCur
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointLevelMax
		m_astRaceScore[].m_stProfessionLicenseAward[].m_ushIncResultID
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uchShowQiPao
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointWeekTotal
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointWeekCur
		m_astRaceScore[].m_stProfessionLicenseAward[].m_stProfessionLicenseInfo.m_uchBigLevel
		m_astRaceScore[].m_stProfessionLicenseAward[].m_stProfessionLicenseInfo.m_uchSmallLevel
		m_astRaceScore[].m_stProfessionLicenseAward[].m_uchSwitchShowExtraAddPoint
		m_astRaceScore[].m_stProfessionLicenseAward[].m_ushBeginOffsetSeconds
		*/

		{ //RaceResult
			BYTE* pRaceResult = pRaceScore;
			Write16(pRaceResult, 0); //len

			Write8(pRaceResult, 0); //PLExpPowerValid
			Write8(pRaceResult, 0); //PLHonorPowerValid
			Write8(pRaceResult, 0); //ExpMultiNum
			Write8(pRaceResult, 0); //HonorMultiNum

			len = pRaceResult - pRaceScore;
			Set16(pRaceScore, (WORD)len);
			pRaceScore += len;
		}

		Write8(pRaceScore, 0); //HaveRankedMatchExtraInfo
		/*
		m_astRaceScore[].m_stRankedMatchExtraScoreInfo[].m_uiAwardItemNums
		m_astRaceScore[].m_stRankedMatchExtraScoreInfo[].m_ucWin
		*/

		Write8(pRaceScore, 0); //HasYuLeJiaNianHuaComputeData
		/*
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_iAwardScore
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchRecommend
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchScoreDouble
		m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchGuaJi
		*/

		len = pRaceScore - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //WinTeamID
	Write32(p, 0); //WinType
	Write32(p, 0); //AwardTime
	Write8(p, 0); //LuckyMatchResult
	Write8(p, 0); //LuckyMatchType


	Write8(p, 1); //PlayerNumber
	{ //HideTaskResult
		BYTE* pHideTaskResult = p;
		Write16(pHideTaskResult, 0); //len

		Write32(pHideTaskResult, Client->Uin); //Uin
		Write32(pHideTaskResult, 1000); //TaskId
		Write8(pHideTaskResult, 1); //HideTaskFinishResult

		len = pHideTaskResult - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); //HasLadderMatchResult
	/*
m_stLDMRaceResult[].m_u8PlayerNum
m_stLDMRaceResult[].m_stResult[].m_u32Uin
m_stLDMRaceResult[].m_stResult[].m_u32Score
m_stLDMRaceResult[].m_stResult[].m_i32ScoreInc
m_stLDMRaceResult[].m_stResult[].m_u32NextGradeScore
m_stLDMRaceResult[].m_stResult[].m_u8Grade
m_stLDMRaceResult[].m_stResult[].m_u8BeforeGrade
m_stLDMRaceResult[].m_stResult[].m_stRank.m_u8RankType
m_stLDMRaceResult[].m_stResult[].m_stRank.m_u8RankValue
m_stLDMRaceResult[].m_stResult[].m_stEstimateRank.m_u8RankType
m_stLDMRaceResult[].m_stResult[].m_stEstimateRank.m_u8RankValue
	*/

	Write8(p, 0); //NeedFreshTutor
	Write8(p, 0); //hasMedalGameInfo
	//m_medalGameInfo[].m_curMedal

#ifndef ZingSpeed
	Write8(p, 0); //KickOffPlayerNumber
	/*
m_stKickOffPlayer[].m_u32Uin
m_stKickOffPlayer[].m_szNickName[17]
m_stKickOffPlayer[].m_bGender
m_stKickOffPlayer[].m_bTeamID
m_stKickOffPlayer[].m_bCountry
	*/

	Write8(p, 0); //EliminatedRacerNum
	/*
m_astEliminatedRacer[].m_unUin
m_astEliminatedRacer[].m_szRacerName[17]
	*/

	Write8(p, 0); //TeamNum
	/*
m_astTeamScore[].m_ucTeamID
m_astTeamScore[].m_iTeamScore
m_astTeamScore[].m_ucTeamMinRank
m_astTeamScore[].m_ucCooperNum
m_astTeamScore[].m_ucIncCooper
m_astTeamScore[].m_ucCurCooper
	*/

	Write8(p, 0); //NPCRacerNum
	for (size_t i = 0; i < 0; i++)
	{ //ArrestNPCResultInfo
		BYTE* pArrestNPCResultInfo = p;
		Write16(pArrestNPCResultInfo, 0); //len

		Write32(pArrestNPCResultInfo, i+1); //NpcID
		WriteString(pArrestNPCResultInfo, "严斌", MaxNickName); //NPCName[]
		Write8(pArrestNPCResultInfo, 0); //ResultType

		len = pArrestNPCResultInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	/*
m_astArrestNPCResultInfo[].m_iNpcID
m_astArrestNPCResultInfo[].m_szNPCName[17]
m_astArrestNPCResultInfo[].m_uchResultType
	*/

	Write8(p, 0); //hasGameFrameInfo
	/*
m_astGameFrameInfo[].m_uiUin
m_astGameFrameInfo[].m_iMapID
m_astGameFrameInfo[].m_ushFrameNum
m_astGameFrameInfo[].m_astFrameInfo[].m_ushCheckPointID
m_astGameFrameInfo[].m_astFrameInfo[].m_uiNum
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_unTimeElaps
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fCarMat[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fAccTime[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fFrameTime[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_ucSetPos
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_ucCollision
	*/

	Write8(p, 0); //hasDesperateEscapeTrophyInfo
	/*
m_astDesperateEscapeTrophyInfo[].m_uchNum
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiSrcUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchTrophyType
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchStatus
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchHasRefit
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_uiUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartId
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iRefitCout
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shMaxFlags
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shWWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSpeedWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shJetWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSJetWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAccuWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartHeadRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartTailRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartFlankRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartTireRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iSecondRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSpeed2Weight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shDriftVecWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAdditionalZSpeedWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAntiCollisionWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shLuckyValue
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shRefitLuckyValueMaxWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeSuitID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_ucLegendSuitLevel
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iLegendSuitLevelChoice
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeLegendSuitID
	*/

	Write8(p, 0); //HasGameStageInfo
	/*
m_stGameStageInfo[].m_u8StageIndex
m_stGameStageInfo[].m_u8TotalStageNum
	*/
#endif

	len = p - buf;
	SendToClient(Client, 513, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}
void NotifyRaceOver(ClientNode *Client, RoomNode *Room)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	// Race over get award
	UINT Experience = getGameConfig()["RaceAward"]["Experience"].as<UINT>(); // 经验
	UINT Money = getGameConfig()["RaceAward"]["Money"].as<UINT>();			 // 酷币
	UINT SuperMoney = getGameConfig()["RaceAward"]["SuperMoney"].as<UINT>(); // 点券
	UINT Coupons = getGameConfig()["RaceAward"]["Coupons"].as<UINT>();		 // 消费券
	UINT LuckMoney = getGameConfig()["RaceAward"]["LuckMoney"].as<UINT>();	 // 幸运币

	Client->Experience += Experience;
	Client->Money += Money;
	Client->SuperMoney += SuperMoney;
	Client->Coupons += Coupons;
	Client->LuckMoney += LuckMoney;
	PlayerDB_UpdateMoneyExp(Client);
	Room->startStatus = 1;
	// ResponseStartGame(Client, "你在游戏中获得了新的道具");
	int *Scoresort = Room->TotalPlayerNum != 1 ? GetScoreSort(Room) : NULL;

	std::cout << Scoresort << std::endl;

	Write8(p, Room->TotalPlayerNum); // CurrentPlayerNum
	for (size_t i = 0; i < Room->TotalPlayerNum; i++)
	{
		{ // RaceScore
			BYTE *pRaceScore = p;
			Write16(pRaceScore, 0); // len
			int index = Scoresort ? Scoresort[i] : i;
			ClientNode Player = *Room->Player[index];
			
			Write32(pRaceScore, Player.Uin); // Uin
			if (Player.FinishTime == 0)
				Player.FinishTime = -1;
			Write32(pRaceScore, Player.FinishTime); // FinTime
			Write32(pRaceScore, 1);					 // TP
			Write32(pRaceScore, Money);				 // MoneyInc
			Write32(pRaceScore, Player.Money);		 // TotalMoney
			Write32(pRaceScore, Experience);		 // ExpInc
			Write32(pRaceScore, Player.Experience); // TotalExp

			Write8(pRaceScore, 3); // AwardNum
			// m_iSpecialAward[]
			Write32(pRaceScore, 5); // m_iSpecialAward
			Write32(pRaceScore, 23); // m_iSpecialAward
			Write32(pRaceScore, 30); // m_iSpecialAward

			Write32(pRaceScore, 0);					// TeamWorkExpInc
			Write32(pRaceScore, 0);					// PropPoint
			Write32(pRaceScore, 0);					// PropPointAddExp
			Write32(pRaceScore, 0);					// LuckyMatchPointInc
			Write32(pRaceScore, 0);					// LuckyMatchPointTotal
			Write32(pRaceScore, 0);					// LuckyMatchScoreInc
			Write32(pRaceScore, 0);					// LuckyMatchScoreTotal
			Write32(pRaceScore, LuckMoney);			// LuckMoneyInc
			Write32(pRaceScore, Player.LuckMoney); // LuckMoneyTotal
			Write32(pRaceScore, 0);					// GuildScoreInc

			Write8(pRaceScore, 0); // CrazyPropAchieveNum
			// m_aiCrazyPropAchieve[]

			Write32(pRaceScore, 0); // IncWlMatchScore
			Write32(pRaceScore, 0); // IncWlDegree
			Write8(pRaceScore, 0);	// IncItemNumByWl
			/*
	m_astRaceScore[].m_astIncItemInfoByWl[].m_shGenderItemNum
	m_astRaceScore[].m_astIncItemInfoByWl[].m_aiItemID[EGT_MAX]
	m_astRaceScore[].m_astIncItemInfoByWl[].m_shItemNum
	m_astRaceScore[].m_astIncItemInfoByWl[].m_iAvailPeriod
	m_astRaceScore[].m_astIncItemInfoByWl[].m_usBroadcastType
	m_astRaceScore[].m_astIncItemInfoByWl[].m_usBroadcastFlag
	m_astRaceScore[].m_astIncItemInfoByWl[].m_ucShowPriority
	m_astRaceScore[].m_astIncItemInfoByWl[].m_uchItemType
	m_astRaceScore[].m_astIncItemInfoByWl[].m_uchIgnoreItem
	m_astRaceScore[].m_astIncItemInfoByWl[].m_uiLimitPara
	m_astRaceScore[].m_astIncItemInfoByWl[].m_ucSpecialAwardCtrlType
	m_astRaceScore[].m_astIncItemInfoByWl[].m_ucAwardCtrlType
			*/

			Write32(pRaceScore, 0); // WlMutiplyCard
			Write32(pRaceScore, 0); // SkateCoinInc
			Write32(pRaceScore, 0); // SkateCoinTotal
			Write32(pRaceScore, 0); // SkateCoinHistoryTotal
			Write32(pRaceScore, 0); // TotalCoupons

			Write8(pRaceScore, 0); // ChallengeCheer
			/*
	m_astRaceScore[].m_stChallengeCheer[].m_ushGuildCheerValueInc
	m_astRaceScore[].m_stChallengeCheer[].m_ushCheerValueInc
	m_astRaceScore[].m_stChallengeCheer[].m_uiTotalCheerValue
	m_astRaceScore[].m_stChallengeCheer[].m_ushCouponsInc
	m_astRaceScore[].m_stChallengeCheer[].m_iGuildScoreInc
	m_astRaceScore[].m_stChallengeCheer[].m_iSuperMoneyInc
			*/

			Write32(pRaceScore, 0); // LoveValue
			Write32(pRaceScore, 0); // SkateCoinEmperorBonus
			Write32(pRaceScore, 0); // DetailRecordID

			Write8(pRaceScore, 0); // HasGangsterResult
			/*
	m_astRaceScore[].m_astGangsterResult[].m_cAbnormalFalg
	m_astRaceScore[].m_astGangsterResult[].m_uiGangsterBlood
	m_astRaceScore[].m_astGangsterResult[].m_uiGangsterTotalScore
	m_astRaceScore[].m_astGangsterResult[].m_uiGangsterIndentity
	m_astRaceScore[].m_astGangsterResult[].m_uiFragmentInc
	m_astRaceScore[].m_astGangsterResult[].m_uiKillTimes
	m_astRaceScore[].m_astGangsterResult[].m_uiDeadTimes
	m_astRaceScore[].m_astGangsterResult[].m_ucIsMultiple
	m_astRaceScore[].m_astGangsterResult[].m_ucIsLimited
	m_astRaceScore[].m_astGangsterResult[].m_iScoreInc
	m_astRaceScore[].m_astGangsterResult[].m_iTotalScore
	m_astRaceScore[].m_astGangsterResult[].m_iLoseStreakCount
			*/

			{ // EquippedActiveKartInfo
				BYTE *pEquippedActiveKartInfo = pRaceScore;
				Write16(pEquippedActiveKartInfo, 0); // len

				Write8(pEquippedActiveKartInfo, 1);	 // HaveActiveInfo
				Write32(pEquippedActiveKartInfo, Client->KartID); // KartID
				Write32(pEquippedActiveKartInfo, 10); // ActiveLevel

				len = pEquippedActiveKartInfo - pRaceScore;
				Set16(pRaceScore, (WORD)len);
				pRaceScore += len;
			}

			Write8(pRaceScore, 0); // HasWeRelayRaceOverInfo
			/*
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Score
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32ScoreInc
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Supermoney
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32Coupons
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_stSumStageInfo.m_u32SuperN2O
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreInc
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreNow
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreLevelNow
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiWeRelayScoreLevelLimit
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiSupermoneyLimit
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiSupermoneyNow
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiCouponsLimit
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiCouponsNow
		m_astRaceScore[].m_stWeRelayRaceOverInfo[].m_uiLuckAwardMul
			*/

			Write8(pRaceScore, 0); // HaveSkatePropRaceInfo
			/*
		m_astRaceScore[].m_stSkatePropRaceInfo[].m_uiScoreAdd
		m_astRaceScore[].m_stSkatePropRaceInfo[].m_uiScoreTotal
			*/

			Write8(pRaceScore, 1); // IsNewBox

			Write8(pRaceScore, 0); // HaveArrestScoreInfo
			/*
		m_astRaceScore[].m_stArrestScoreInfo[].m_uiScoreInGame
		m_astRaceScore[].m_stArrestScoreInfo[].m_uiScoreByGold
		m_astRaceScore[].m_stArrestScoreInfo[].m_uiGoldMoney
		m_astRaceScore[].m_stArrestScoreInfo[].m_uiArrestNum
		m_astRaceScore[].m_stArrestScoreInfo[].m_uchResultType
		m_astRaceScore[].m_stArrestScoreInfo[].m_bTeamID
			*/

			Write8(pRaceScore, 1); // HasRankedMatchInfo
			for (size_t i = 0; i < 1; i++)
			{ // RankedMatchScore
				BYTE *pRankedMatchScore = pRaceScore;
				Write16(pRankedMatchScore, 0); // len

				{ // RankedMatchLastGradeInfo
					BYTE *pRankedMatchLastGradeInfo = pRankedMatchScore;
					Write16(pRankedMatchLastGradeInfo, 0); // len

					Write32(pRankedMatchLastGradeInfo, 27);	  // GradeLevel
					Write32(pRankedMatchLastGradeInfo, 9999); // ChildLevel
					Write32(pRankedMatchLastGradeInfo, 9999); // GradeScore
					Write32(pRankedMatchLastGradeInfo, 9999); // MaxGradeScore
					Write32(pRankedMatchLastGradeInfo, 9999); // TotalScore

					len = pRankedMatchLastGradeInfo - pRankedMatchScore;
					Set16(pRankedMatchScore, (WORD)len);
					pRankedMatchScore += len;
				}
				{ // RankedMatchGradeInfo
					BYTE *pRankedMatchGradeInfo = pRankedMatchScore;
					Write16(pRankedMatchGradeInfo, 0); // len

					Write32(pRankedMatchGradeInfo, 27);	  // GradeLevel
					Write32(pRankedMatchGradeInfo, 9999); // ChildLevel
					Write32(pRankedMatchGradeInfo, 9999); // GradeScore
					Write32(pRankedMatchGradeInfo, 9999); // MaxGradeScore
					Write32(pRankedMatchGradeInfo, 9999); // TotalScore

					len = pRankedMatchGradeInfo - pRankedMatchScore;
					Set16(pRankedMatchScore, (WORD)len);
					pRankedMatchScore += len;
				}
				{ // RankedMatchPromoInfo
					BYTE *pRankedMatchPromoInfo = pRankedMatchScore;
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

					len = pRankedMatchPromoInfo - pRankedMatchScore;
					Set16(pRankedMatchScore, (WORD)len);
					pRankedMatchScore += len;
				}
				{ // RankedMatchComputeInfo
					BYTE *pRankedMatchComputeInfo = pRankedMatchScore;
					Write16(pRankedMatchComputeInfo, 0); // len

					Write32(pRankedMatchComputeInfo, 0); // RankAddScore
					Write32(pRankedMatchComputeInfo, 0); // FinishAddScore
					Write32(pRankedMatchComputeInfo, 0); // KillAllAddScore
					Write32(pRankedMatchComputeInfo, 0); // GradeProtectAddScore
					Write32(pRankedMatchComputeInfo, 0); // GradeAddScore
					Write32(pRankedMatchComputeInfo, 0); // ProtectAddScoreGradeLimit
					Write32(pRankedMatchComputeInfo, 0); // DailyExtraAddScore
					Write32(pRankedMatchComputeInfo, 0); // AwardRankCoinNums
					Write32(pRankedMatchComputeInfo, 0); // WeeklyAwardRankCoinNums
					Write32(pRankedMatchComputeInfo, 0); // WeeklyAwardRankCoinLimit
					Write32(pRankedMatchComputeInfo, 0); // AwardRankCoinNumsByRank
					Write32(pRankedMatchComputeInfo, 0); // AwardRankCoinNumsByFinish
					Write32(pRankedMatchComputeInfo, 0); // BanMapExtraAddScore
					Write32(pRankedMatchComputeInfo, 0); // TeamRankAddScore
					Write32(pRankedMatchComputeInfo, 0); // AwardRankCoinNumsByTeamRank
					Write32(pRankedMatchComputeInfo, 0); // AwardRankCoinNumsByTask
					Write8(pRankedMatchComputeInfo, 0);	 // UseDoubleScoreItem
					Write8(pRankedMatchComputeInfo, 0);	 // UserNotDecreaseItem
					Write8(pRankedMatchComputeInfo, 0);	 // FinishSeasonChallengeTaskOrNot
					Write32(pRankedMatchComputeInfo, 0); // FinishedSeasonChallengeTaskIndex
					Write32(pRankedMatchComputeInfo, 0); // SeasonChallengeTaskProtectAddScore
					Write8(pRankedMatchComputeInfo, 0);	 // NotDecreaseReason
					Write8(pRankedMatchComputeInfo, 0);	 // HaveSpeedKingInfo
					for (size_t i = 0; i < 1; i++)
					{ // RankedMatchSpeedKingInfos
						BYTE *pRankedMatchSpeedKingInfos = pRankedMatchComputeInfo;
						Write16(pRankedMatchSpeedKingInfos, 0); // len

						Write8(pRankedMatchSpeedKingInfos, 1);	// WinOrNot
						WriteUn(pRankedMatchSpeedKingInfos, 0); // UiAwardSpeedKingEnergyItemNums
						Write32(pRankedMatchSpeedKingInfos, 0); // WeeklyAwardSpeedKingEnergyItemNums
						Write32(pRankedMatchSpeedKingInfos, 0); // WeeklyAwardSpeedKingEnergyItemLimitNums

						len = pRankedMatchSpeedKingInfos - pRankedMatchComputeInfo;
						Set16(pRankedMatchComputeInfo, (WORD)len);
						pRankedMatchComputeInfo += len;
					}
					Write8(pRankedMatchComputeInfo, 1); // HaveExtraComputeInfo
					for (size_t i = 0; i < 1; i++)
					{ // RankedMatchExtracComputeInfos
						BYTE *pRankedMatchExtracComputeInfos = pRankedMatchComputeInfo;
						Write16(pRankedMatchExtracComputeInfos, 0); // len

						Write32(pRankedMatchExtracComputeInfos, 0); // ExtraComputeType
						Write32(pRankedMatchExtracComputeInfos, 0); // AwardItemNums
						Write32(pRankedMatchExtracComputeInfos, 0); // WeekylyAwardItemNums
						Write32(pRankedMatchExtracComputeInfos, 0); // WeeklyAwardItemLimitNums
						Write8(pRankedMatchExtracComputeInfos, 0);	// HaveGameLogicTask
						for (size_t i = 0; i < 1; i++)
						{ // RakedMatchGamelogicTaskInfo
							BYTE *pRakedMatchGamelogicTaskInfo = pRankedMatchExtracComputeInfos;
							Write16(pRakedMatchGamelogicTaskInfo, 0); // len

							Write32(pRakedMatchGamelogicTaskInfo, 0); // GameLogicTaskType
							Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskFinishCondValue
							Write8(pRakedMatchGamelogicTaskInfo, 0);  // JudgeRule
							Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskAddGradeScore
							Write8(pRakedMatchGamelogicTaskInfo, 0);  // GradeScoreRangeNum
							Write32(pRakedMatchGamelogicTaskInfo, 0); // GradeScoreRange[]
							Write8(pRakedMatchGamelogicTaskInfo, 0);  // TaskFinishCondValueNum
							Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskFinishCondValueRange[]
							Write8(pRakedMatchGamelogicTaskInfo, 0);  // TaskAddGradeScoreNum
							Write32(pRakedMatchGamelogicTaskInfo, 0); // TaskAddGradeScoreRange[]
							Write8(pRakedMatchGamelogicTaskInfo, 0);  // FailedAddGradeScoreNum
							Write32(pRakedMatchGamelogicTaskInfo, 0); // FailedAddGradeScoreRange[]
							Write8(pRakedMatchGamelogicTaskInfo, 0);  // FinishTask
							Write32(pRakedMatchGamelogicTaskInfo, 0); // ExtraFinishTaskAddScore

							len = pRakedMatchGamelogicTaskInfo - pRankedMatchExtracComputeInfos;
							Set16(pRankedMatchExtracComputeInfos, (WORD)len);
							pRankedMatchExtracComputeInfos += len;
						}
						Write8(pRankedMatchExtracComputeInfos, 0); // Win
						Write8(pRankedMatchExtracComputeInfos, 0); // NormalPlayerOrNot
						Write8(pRankedMatchExtracComputeInfos, 0); // LeadRolePlayerNums

						len = pRankedMatchExtracComputeInfos - pRankedMatchComputeInfo;
						Set16(pRankedMatchComputeInfo, (WORD)len);
						pRankedMatchComputeInfo += len;
					}
					Write32(pRankedMatchComputeInfo, 0); // SeasonNewMapAddScore
					Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeBaseScore
					Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeContiWinExtrScore
					Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeRegretDefeatScore
					Write8(pRankedMatchComputeInfo, 0);	 // SeasonChallengeRespectableOpponentScore
					Write8(pRankedMatchComputeInfo, 0);	 // SpeedGodChallengeAddScore
					Write8(pRankedMatchComputeInfo, 0);	 // SpeedGodChallengeAddCoin
					Write8(pRankedMatchComputeInfo, 0);	 // SpeedFlashAddFactoryCoin
					Write8(pRankedMatchComputeInfo, 0);	 // PinnacleBattleAddScore
					Write8(pRankedMatchComputeInfo, 0);	 // PromotionExtraScore
					Write32(pRankedMatchComputeInfo, 0); // SavedExtraScore
					Write32(pRankedMatchComputeInfo, 0); // DailyExtraTaskID
					Write8(pRankedMatchComputeInfo, 0);	 // SuitActiveAddScore
					Write8(pRankedMatchComputeInfo, 0);	 // SpeedGodChallengeType
					Write8(pRankedMatchComputeInfo, 0);	 // SuitActiveEffectID

					len = pRankedMatchComputeInfo - pRankedMatchScore;
					Set16(pRankedMatchScore, (WORD)len);
					pRankedMatchScore += len;
				}
				Write8(pRankedMatchScore, 1); // HavePromotionExtraScoreInfo
				for (size_t i = 0; i < 1; i++)
				{ // PromotionExtraScoreInfo
					BYTE *pPromotionExtraScoreInfo = pRankedMatchScore;
					Write16(pPromotionExtraScoreInfo, 0); // len

					Write8(pPromotionExtraScoreInfo, 0);  // PromotionMatch
					Write32(pPromotionExtraScoreInfo, 0); // PromotionRoundNums
					Write32(pPromotionExtraScoreInfo, 0); // PromotionSuccessRoundNums
					Write32(pPromotionExtraScoreInfo, 0); // PromotionAlreadySuccessRoundNums
					Write32(pPromotionExtraScoreInfo, 0); // PromotionRecordTag
					Write32(pPromotionExtraScoreInfo, 0); // CurrentPromotionRoundNums
					Write32(pPromotionExtraScoreInfo, 0); // PromoFailedMinusScore
					Write32(pPromotionExtraScoreInfo, 0); // PromoSuccAddScore
					Write8(pPromotionExtraScoreInfo, 0);  // PromoDirectly
					Write8(pPromotionExtraScoreInfo, 0);  // GradeNoNeedPromo
					Write8(pPromotionExtraScoreInfo, 0);  // RankedMemoryNoNeedPromo
					Write8(pPromotionExtraScoreInfo, 0);  // PromotionExtraScore
					Write8(pPromotionExtraScoreInfo, 0);  // PromtRaceFailedDefendFlag

					len = pPromotionExtraScoreInfo - pRankedMatchScore;
					Set16(pRankedMatchScore, (WORD)len);
					pRankedMatchScore += len;
				}

				len = pRankedMatchScore - pRaceScore;
				Set16(pRaceScore, (WORD)len);
				pRaceScore += len;
			}

			Write8(pRaceScore, 0); // HaveCrazyChaseScoreInfo
			/*
		m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uiUin
		m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_shRole
		m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uiScore
		m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_uchScoreNum
		m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_uchScoreType
		m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_iValue
		m_astRaceScore[].m_astCrazyChaseScoreInfo[].m_astScoreInfo[].m_iScore
			*/
			Write8(pRaceScore, 0); // TeamID

			Write8(pRaceScore, 0); // HasRankedMatchArenaInfo
			/*
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucWin
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucRank
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucTaskScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_iAwardRankCoinNums
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiGradeLevel
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiChildLevel
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiGradeScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiMaxGradeScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchLastGradeInfo.m_uiTotalScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiGradeLevel
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiChildLevel
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiGradeScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiMaxGradeScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchGradeInfo.m_uiTotalScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromotionMatch
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionRoundNums
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionSuccessRoundNums
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionAlreadySuccessRoundNums
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromotionRecordTag
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiCurrentPromotionRoundNums
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromoFailedMinusScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uiPromoSuccAddScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromoDirectly
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucGradeNoNeedPromo
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_bRankedMemoryNoNeedPromo
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_uchPromotionExtraScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_stRankedMatchPromoInfo.m_ucPromtRaceFailedDefendFlag
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucWinNum
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_sCurrentScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucFinishScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_usAwardRankScore
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucAwardRankCoinNumsByRank
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_ucAwardRankCoinNumsByFinish
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_iWeeklyAwardRankCoinNums
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_iWeeklyAwardRankCoinLimit
		m_astRaceScore[].m_stRankedMatchArenaScore[].m_iAwardRankCoinNumsByTask
			*/

			Write32(pRaceScore, 0); // DistanceToEnd
			Write8(pRaceScore, 0);	// ShortDistancWinPoint
			Write8(pRaceScore, 0);	// Status

			Write8(pRaceScore, 0); // HaveRankedMatchSpeedKingInfo
			/*
		m_astRaceScore[].m_stRankedMatchSpeedKingScoreInfo[].m_uiAwardSpeedKingEnergyNums
		m_astRaceScore[].m_stRankedMatchSpeedKingScoreInfo[].m_ucWin
			*/

			Write8(pRaceScore, 0); // ProfessionLicenseAwardNum
			/*
			m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointInc
			m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointLevelCur
			m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointLevelMax
			m_astRaceScore[].m_stProfessionLicenseAward[].m_ushIncResultID
			m_astRaceScore[].m_stProfessionLicenseAward[].m_uchShowQiPao
			m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointWeekTotal
			m_astRaceScore[].m_stProfessionLicenseAward[].m_uiPointWeekCur
			m_astRaceScore[].m_stProfessionLicenseAward[].m_stProfessionLicenseInfo.m_uchBigLevel
			m_astRaceScore[].m_stProfessionLicenseAward[].m_stProfessionLicenseInfo.m_uchSmallLevel
			m_astRaceScore[].m_stProfessionLicenseAward[].m_uchSwitchShowExtraAddPoint
			m_astRaceScore[].m_stProfessionLicenseAward[].m_ushBeginOffsetSeconds
			*/

			{ // RaceResult
				BYTE *pRaceResult = pRaceScore;
				Write16(pRaceResult, 0); // len

				Write8(pRaceResult, 0); // PLExpPowerValid
				Write8(pRaceResult, 0); // PLHonorPowerValid
				Write8(pRaceResult, 0); // ExpMultiNum
				Write8(pRaceResult, 0); // HonorMultiNum

				len = pRaceResult - pRaceScore;
				Set16(pRaceScore, (WORD)len);
				pRaceScore += len;
			}

			Write8(pRaceScore, 0); // HaveRankedMatchExtraInfo
			/*
			m_astRaceScore[].m_stRankedMatchExtraScoreInfo[].m_uiAwardItemNums
			m_astRaceScore[].m_stRankedMatchExtraScoreInfo[].m_ucWin
			*/

			Write8(pRaceScore, 0); // HasYuLeJiaNianHuaComputeData
			/*
			m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_iAwardScore
			m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchRecommend
			m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchScoreDouble
			m_astRaceScore[].m_stYuLeJiaNianHuaComputeData[].m_uchGuaJi
			*/

			len = pRaceScore - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	delete[] Scoresort;

	Write8(p, 0);  // WinTeamID
	Write32(p, 0); // WinType
	Write32(p, 0); // AwardTime
	Write8(p, 0);  // LuckyMatchResult
	Write8(p, 0);  // LuckyMatchType

	Write8(p, 1); // PlayerNumber
	{			  // HideTaskResult
		BYTE *pHideTaskResult = p;
		Write16(pHideTaskResult, 0); // len

		Write32(pHideTaskResult, Client->Uin); // Uin
		Write32(pHideTaskResult, 0);		   // TaskId
		Write8(pHideTaskResult, 0);			   // HideTaskFinishResult

		len = pHideTaskResult - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0); // HasLadderMatchResult
	/*
m_stLDMRaceResult[].m_u8PlayerNum
m_stLDMRaceResult[].m_stResult[].m_u32Uin
m_stLDMRaceResult[].m_stResult[].m_u32Score
m_stLDMRaceResult[].m_stResult[].m_i32ScoreInc
m_stLDMRaceResult[].m_stResult[].m_u32NextGradeScore
m_stLDMRaceResult[].m_stResult[].m_u8Grade
m_stLDMRaceResult[].m_stResult[].m_u8BeforeGrade
m_stLDMRaceResult[].m_stResult[].m_stRank.m_u8RankType
m_stLDMRaceResult[].m_stResult[].m_stRank.m_u8RankValue
m_stLDMRaceResult[].m_stResult[].m_stEstimateRank.m_u8RankType
m_stLDMRaceResult[].m_stResult[].m_stEstimateRank.m_u8RankValue
	*/

	Write8(p, 0); // NeedFreshTutor
	Write8(p, 0); // hasMedalGameInfo
				  // m_medalGameInfo[].m_curMedal

#ifndef ZingSpeed
	Write8(p, 0); // KickOffPlayerNumber
	/*
m_stKickOffPlayer[].m_u32Uin
m_stKickOffPlayer[].m_szNickName[17]
m_stKickOffPlayer[].m_bGender
m_stKickOffPlayer[].m_bTeamID
m_stKickOffPlayer[].m_bCountry
	*/

	Write8(p, 0); // EliminatedRacerNum
	/*
m_astEliminatedRacer[].m_unUin
m_astEliminatedRacer[].m_szRacerName[17]
	*/

	Write8(p, 0); // TeamNum
	/*
m_astTeamScore[].m_ucTeamID
m_astTeamScore[].m_iTeamScore
m_astTeamScore[].m_ucTeamMinRank
m_astTeamScore[].m_ucCooperNum
m_astTeamScore[].m_ucIncCooper
m_astTeamScore[].m_ucCurCooper
	*/

	Write8(p, 0); // NPCRacerNum
	/*
m_astArrestNPCResultInfo[].m_iNpcID
m_astArrestNPCResultInfo[].m_szNPCName[17]
m_astArrestNPCResultInfo[].m_uchResultType
	*/

	Write8(p, 0); // hasGameFrameInfo
	/*
m_astGameFrameInfo[].m_uiUin
m_astGameFrameInfo[].m_iMapID
m_astGameFrameInfo[].m_ushFrameNum
m_astGameFrameInfo[].m_astFrameInfo[].m_ushCheckPointID
m_astGameFrameInfo[].m_astFrameInfo[].m_uiNum
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_unTimeElaps
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fInPosZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fStepZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fCarMat[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosX[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosY[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fOutPosZ[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fAccTime[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_fFrameTime[]
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_ucSetPos
m_astGameFrameInfo[].m_astFrameInfo[].m_astFrameList[].m_ucCollision
	*/

	Write8(p, 0); // hasDesperateEscapeTrophyInfo
	/*
m_astDesperateEscapeTrophyInfo[].m_uchNum
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiSrcUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchTrophyType
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uiRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchStatus
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_uchHasRefit
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_uiUin
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartId
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iRefitCout
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shMaxFlags
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shWWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSpeedWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shJetWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSJetWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAccuWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartHeadRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartTailRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartFlankRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iKartTireRefitItemID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iSecondRefitCount
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shSpeed2Weight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shDriftVecWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAdditionalZSpeedWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shAntiCollisionWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shLuckyValue
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_shRefitLuckyValueMaxWeight
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeSuitID
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_ucLegendSuitLevel
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iLegendSuitLevelChoice
m_astDesperateEscapeTrophyInfo[].m_astInfo[].m_astKartRefitInfo[].m_iShapeLegendSuitID
	*/

	Write8(p, 0); // HasGameStageInfo
				  /*
			  m_stGameStageInfo[].m_u8StageIndex
			  m_stGameStageInfo[].m_u8TotalStageNum
				  */
#endif

	len = p - buf;
	SendToClient(Client, 513, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyGameOver(ClientNode *Client, USHORT LeaveGameType, BYTE ParaNum, int *ParaList, USHORT ReturnHall)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 0); // ResultID

#ifndef ZingSpeed
	Write8(p, 0);			   // StageIndex
	Write8(p, ReturnHall);	   // ReturnHall
	Write8(p, 0);			   // WaitEnterRoom
	Write16(p, LeaveGameType); // LeaveGameType
	Write8(p, ParaNum);		   // ParaNum
	for (size_t i = 0; i < ParaNum; i++)
	{
		// m_aiParaList[]
		Write32(p, ParaList[i]);
	}
#endif

	len = p - buf;
	SendToClient(Client, 514, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
	NotifySpecialActivityInfo(Client);
	NotifyUpdateKartSkillStoneInfo2(Client, Client->KartID, Client->KartID);
	Client->StartGameTime = false;
}

void RequestTransferByTCP(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	char Ver = Read8(Body);
	int Seq = Read32(Body);
	char DstNum = Read8(Body);


	for (char i = 0; i < DstNum; i++)
	{
		BYTE* pDstInfo = Body;
		USHORT len = Read16(pDstInfo);

		aDstInfo[i].PlayerID = Read16(pDstInfo);
		aDstInfo[i].Uin = Read32(pDstInfo);

		Body += len;
	}
	USHORT BuffLen = Read16(Body);
	RoomNode* Room;
	if (Client->TaskID == 0xFFFC || Client->TaskID == 0xFFFD)
		Room = GetRoom(Client->BorderRoomID);
	else
		Room = GetRoom(Client->RoomID);
	if (!Room)
	{
		return;
	}
	for (char i = 0; i < DstNum; i++)
	{
		for (char i2 = 0; i2 < 6; i2++)
		{
			ClientNode* RoomClient = Room->Player[i2];
			if (RoomClient)
			{
				if (RoomClient->Uin == aDstInfo[i].Uin)
				{
					if (RoomClient->IsReady)
					{
						NotifyTranferByTCP(RoomClient, Client->Uin, Client->ConnID, Seq, Body, BuffLen,Ver);
					}
					break;
				}
			}
		}
	}
}
void NotifyTranferByTCP(ClientNode *Client, UINT SrcUin, USHORT SrcPlayerID, int Seq, BYTE *Buff, int Bufflen, char Ver)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, SrcUin);
	Write16(p, SrcPlayerID);
	Write8(p, 0); // Ver
	Write32(p, Seq);
	Write16(p, Bufflen);
	memcpy(p, Buff, Bufflen);
	p += Bufflen;

	len = p - buf;
	SendToClient(Client, 560, buf, len, SrcPlayerID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyAddPropBySkillStone(ClientNode *Client, int StoneSkillType, short PropID, int PropIndex)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;
	Write32(p, StoneSkillType);
	Write16(p, PropID);
	Write32(p, PropIndex);
	len = p - buf;
	SendToClient(Client, 908, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyFeedBackData(ClientNode* Client)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	{ //FeedBack
		BYTE* pFeedBack = p;
		Write16(pFeedBack, 0); //len

		Write32(pFeedBack, 1); //Rank
		Write8(pFeedBack, 1); //Level
		{ //CurRecord
			BYTE* pCurRecord = pFeedBack;
			Write16(pCurRecord, 0); //len

			Write32(pCurRecord, Client->MapID); //MapID
			Write32(pCurRecord, Client->FinishTime); //FinTime
			Write32(pCurRecord, Client->HighestSpeed); //HighestSpeed
			Write32(pCurRecord, Client->DriftCount); //DriftCount
			Write32(pCurRecord, Client->N2OGatherCount); //N2OGatherCount
			Write32(pCurRecord, Client->CollisionCount); //CollisionCount

			len = pCurRecord - pFeedBack;
			Set16(pFeedBack, (WORD)len);
			pFeedBack += len;
		}
		{ //BestRecord
			BYTE* pBestRecord = pFeedBack;
			Write16(pBestRecord, 0); //len

			Write32(pBestRecord, 0); //MapID
			Write32(pBestRecord, 0); //FinTime
			Write32(pBestRecord, 0); //HighestSpeed
			Write32(pBestRecord, 0); //DriftCount
			Write32(pBestRecord, 0); //N2OGatherCount
			Write32(pBestRecord, 0); //CollisionCount

			len = pBestRecord - pFeedBack;
			Set16(pFeedBack, (WORD)len);
			pFeedBack += len;
		}
		Write32(pFeedBack, 100); //GameEvaluation
		Write32(pFeedBack, 100); //PlayerEvaluation
		Write32(pFeedBack, 100); //KartEvaluation
		Write32(pFeedBack, 0); //RefitTryOutIncTime
		Write8(pFeedBack, 0); //WeightNum
		Write32(pFeedBack, Client->Uin); //Uin
		Write32(pFeedBack, 1); //Rank
		Write8(pFeedBack, 0); //HaveGameLogicTask
		for (size_t i = 0; i < 0; i++)
		{ //RakedMatchGamelogicTaskInfo
			BYTE* pRakedMatchGamelogicTaskInfo = pFeedBack;
			Write16(pRakedMatchGamelogicTaskInfo, 0); //len

			Write32(pRakedMatchGamelogicTaskInfo, 0); //GameLogicTaskType
			Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskFinishCondValue
			Write8(pRakedMatchGamelogicTaskInfo, 0); //JudgeRule
			Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskAddGradeScore
			Write8(pRakedMatchGamelogicTaskInfo, 0); //GradeScoreRangeNum
			Write32(pRakedMatchGamelogicTaskInfo, 0); //GradeScoreRange[]
			Write8(pRakedMatchGamelogicTaskInfo, 0); //TaskFinishCondValueNum
			Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskFinishCondValueRange[]
			Write8(pRakedMatchGamelogicTaskInfo, 0); //TaskAddGradeScoreNum
			Write32(pRakedMatchGamelogicTaskInfo, 0); //TaskAddGradeScoreRange[]
			Write8(pRakedMatchGamelogicTaskInfo, 0); //FailedAddGradeScoreNum
			Write32(pRakedMatchGamelogicTaskInfo, 0); //FailedAddGradeScoreRange[]
			Write8(pRakedMatchGamelogicTaskInfo, 0); //FinishTask
			Write32(pRakedMatchGamelogicTaskInfo, 0); //ExtraFinishTaskAddScore

			len = pRakedMatchGamelogicTaskInfo - pFeedBack;
			Set16(pFeedBack, (WORD)len);
			pFeedBack += len;
		}

		len = pFeedBack - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //MapChallengeInfoNum
	for (size_t i = 0; i < 0; i++)
	{ //MapChallengeInfo
		BYTE* pMapChallengeInfo = p;
		Write16(pMapChallengeInfo, 0); //len

		Write8(pMapChallengeInfo, 0); //StarInfoNum
		for (size_t i = 0; i < 0; i++)
		{ //MapChallengeStarInfo
			BYTE* pMapChallengeStarInfo = pMapChallengeInfo;
			Write16(pMapChallengeStarInfo, 0); //len

			Write32(pMapChallengeStarInfo, 0); //Time
			Write8(pMapChallengeStarInfo, 0); //Status

			len = pMapChallengeStarInfo - pMapChallengeInfo;
			Set16(pMapChallengeInfo, (WORD)len);
			pMapChallengeInfo += len;
		}
		{ //AssessInfo
			BYTE* pAssessInfo = pMapChallengeInfo;
			Write16(pAssessInfo, 0); //len

			{ //MapChallenge_AssessInfo
				BYTE* pMapChallenge_AssessInfo = pAssessInfo;
				Write16(pMapChallenge_AssessInfo, 0); //len

				Write8(pMapChallenge_AssessInfo, 0); //Type
				Write16(pMapChallenge_AssessInfo, 0); //Count

				len = pMapChallenge_AssessInfo - pAssessInfo;
				Set16(pAssessInfo, (WORD)len);
				pAssessInfo += len;
			}
			Write8(pAssessInfo, 0); //Status
			Write16(pAssessInfo, 0); //MsgLen

			len = pAssessInfo - pMapChallengeInfo;
			Set16(pMapChallengeInfo, (WORD)len);
			pMapChallengeInfo += len;
		}
		Write32(pMapChallengeInfo, 0); //MapID

		len = pMapChallengeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 935, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestReportFeedBack(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	// printf("我复位了");
	int randomID;
	std::random_device rd;
	std::mt19937 eng(rd());
	std::uniform_int_distribution<int> dis(1, 100);
	randomID = dis(eng);

	/*if (KartSkill[Client->KartID]) {
		if (KartSkill[Client->KartID].as<INT>() == 3) {
			if (randomID >= 90) {
				ResponseGetPropS(Client, 1);
			}
		}

	}*/

	BYTE *p = Body;
	WORD len;
	int n = 1;
	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	{ // FeedBack
		BYTE *pFeedBack = p;
		len = Read16(pFeedBack);

		UINT TotalHitPoint = Read32(pFeedBack);
		UINT ComboHitPoint = Read32(pFeedBack);
		bool AssessNum = Read8(pFeedBack);
		for (size_t i = 0; i < n; i++)
		{ // Assess
			BYTE *pAssess = pFeedBack;
			len = Read16(pAssess);

			bool Type = Read8(pAssess);
			USHORT Count = Read16(pAssess);

			pFeedBack += len;
		}

		p += len;
	}
	
}
void NotifyUseProp(ClientNode* Client, UINT Uin, UINT DstUin, UINT ProdID, UINT ProdIndex, int CurrentPosition1, int CurrentPosition2, int CurrentPosition3, short NewID, BYTE DstType, int ItemID, int StoneSkillType, UINT LapTime, std::vector<UINT> DstUins, BYTE PropUseMode, std::vector<UINT> QueryUins, UCHAR Position , int Status,UINT PropPosIdx, UINT PropSecType, UINT SubPropID)
{
	BYTE buf[8192] = { 0 };
	BYTE* p = buf;
	size_t len = 0;
	UINT OtherPropPosNum = 0;
	Write32(p, Uin);	// Uin
	Write16(p, ProdID);			// PropID
	Write32(p, DstUin);			// DstUin
	Write32(p, CurrentPosition1);				// PropPosition[]
	Write32(p, CurrentPosition2);				// PropPosition[]
	Write32(p, CurrentPosition3);				// PropPosition[]
	Write32(p, ProdIndex);		// PropIndex
	Write16(p, NewID);				// NewID
	Write32(p, ItemID);				// ItemID
	Write32(p, StoneSkillType);				// StoneSkillType
	Write32(p, LapTime);				// LapTime
	Write8(p, DstUins.size());				// DstUinNum
	for (size_t i = 0; i < DstUins.size(); i++) {
		Write32(p, DstUins[i]);				// DstUin[]
	}
	Write8(p, PropUseMode);				// PropUseMode
	Write8(p, QueryUins.size());				// QueryUinNum
	for (size_t i = 0; i < QueryUins.size(); i++) {
		Write32(p, QueryUins[i]);				// DstUin[]
	}
	Write8(p, DstType);				// DstType
	Write8(p, Position);				// Position
	Write8(p, 0);				// ParaNum
	//Write32(p, 0);				// ParaList[]
	Write8(p, Status);				// Status
	Write8(p, PropPosIdx);				// PropPosIdx
	Write8(p, PropSecType);				// PropSecType
	Write8(p, 0);				// PassedCheckPointNum
	//Write16(p, 0);				// PassedCheckPointIDs[]
	Write16(p, SubPropID);				// SubPropID
	Write8(p, OtherPropPosNum); // OtherPropPosNum
	for (size_t i = 0; i < OtherPropPosNum; i++)
	{ // OtherPropPos
		BYTE* pOtherPropPos = p;
		Write16(pOtherPropPos, 0); // len

		Write32(pOtherPropPos, 0); // Idx
		Write32(pOtherPropPos, 0); // Postion[]

		len = pOtherPropPos - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 517, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestUseProp2(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	short PropID = Read16(Body);
	UINT DstUin = Read32(Body);
	int CurrentPosition1 = Read32(Body);
	int CurrentPosition2 = Read32(Body);
	int CurrentPosition3 = Read32(Body);
	//// m_aiCurrentPosition[3]
	//Body += 3 * 4;

	int PropIndex = Read32(Body);
	short NewID = Read16(Body);
	char IsClearProp = Read8(Body);
	BYTE DstType = Read8(Body);
	int MyUseItemID = Read32(Body);
	int DstUseItemID = Read32(Body);
	BYTE IsConvertMode = Read8(Body);
	int StoneSkillType = Read32(Body);
	UINT LapTime = Read32(Body);
	BYTE DstUinNum = Read8(Body);
	std::vector<UINT> DstUins;
	for (size_t i = 0; i < DstUinNum; i++) {
		DstUins.insert(DstUins.end(), Read32(Body));
	}
	BYTE PropUseMode = Read8(Body);
	BYTE QueryUinNum = Read8(Body);
	std::vector<UINT> QueryUins;
	for (size_t i = 0; i < QueryUinNum; i++) {
		QueryUins.insert(QueryUins.end(), Read32(Body));
	}
	UCHAR CurrentSelfRank = Read8(Body);
	UCHAR CurrentTeammateRank = Read8(Body);
	UCHAR Position = Read8(Body);
	char PropSecType = Read8(Body);
	UCHAR PropPosIdx = Read8(Body);
	UINT NPCUin = Read32(Body);
	UCHAR UsePropProtoMode = Read8(Body);
	short SubPropID = Read16(Body);
	UCHAR OtherPropPosNum = Read8(Body);
	Client->useDQ++;
	Client->getPropNum -= 1;
	Client->NumPropsInHand -= 1;
    //Client->PropList[Client->getPropNum - 1] = 0;
#ifdef DEBUG
	Z_INFO("当前道具栏数量%d", Client->getPropNum);
#endif
	if (Client->GetSkillID(6)) {//星煌特性
		if (Client->useDQ == 1) {//起步第一个氮气强化
			NotifyUseProp(Client, Client->Uin, Client->Uin, 777, PropIndex, CurrentPosition1, CurrentPosition2, CurrentPosition3,NewID,DstType,0,StoneSkillType,LapTime,DstUins,PropUseMode,QueryUins,Position,0,PropPosIdx,PropSecType,SubPropID);
			PropID = 777;
		}
		if (Client->useDQ > 6) {//前六个氮气之后
			ResponseReportDrift(Client, 777, 0);
		}
		if (Client->KartStatus == 1) {//星蝶形态超级氮气
			
			NotifyUseProp(Client, Client->Uin, Client->Uin, 34, PropIndex, CurrentPosition1, CurrentPosition2, CurrentPosition3, NewID, DstType, 0, StoneSkillType, LapTime, DstUins, PropUseMode, QueryUins, Position, 0, PropPosIdx, PropSecType, SubPropID);
			PropID = 34;
		}
		if (Client->KartStatus==0) {//星茧形态增加能量
			Client->CollectedN2O += 1;//星蝶能量
			if (Client->CollectedN2O==6) {
				ResponseReportDrift(Client, 1, 170);
			}
			else {
				ResponseReportDrift(Client, 1, 166);
			}
			
		}
		if (Client->CollectedN2O==6) {//进入星蝶形态
			Client->CollectedN2O = 0;
			Client->KartStatus = 1;
			PropID = 34;
			HANDLE XingDie = CreateThread(NULL, 0, XingDieTime, Client, 0, NULL);
			if (XingDie)
			{
				CloseHandle(XingDie);
			}
		}
		
	}
	if (Client->GetSkillID(7))
	{ // 起步特性
		if (Client->useDQ == 1) {
			ResponseGetPropS(Client, 1);
		}
		if (Client->KartStatus == 0) {
			PropID = 34;
			NotifyUseProp(Client, Client->Uin, Client->Uin, 34, PropIndex, CurrentPosition1, CurrentPosition2, CurrentPosition3, NewID, DstType, 0, StoneSkillType, LapTime, DstUins, PropUseMode, QueryUins, Position, 0, PropPosIdx, PropSecType, SubPropID);
		}
	}
	if (PropID == 777)
	{
		if (Client->GetSkillID(1)|| Client->GetSkillID(2)|| Client->GetSkillID(4) || Client->GetSkillID(5)||Client->KartID==19618) {
			NotifyUseProp(Client, Client->Uin, Client->Uin, 34, PropIndex, CurrentPosition1, CurrentPosition2, CurrentPosition3, NewID, DstType, 0, StoneSkillType, LapTime, DstUins, PropUseMode, QueryUins, Position, 0, PropPosIdx, PropSecType, SubPropID);
			PropID = 34;
		}
		
		if (Client->GetSkillID(2)) {
			Client->kartPow = 0;
		}
		
	}

#ifdef DEBUG
	Z_INFO("属性ID:%d, 目标用户ID:%d, 属性索引:%d, 宝石技能类型:%d\n", PropID, DstUin, PropIndex, StoneSkillType);
#endif
	RoomNode *Room = GetRoom(Client->RoomID);
	if (!Room) {
		Room = GetRoom(Client->BorderRoomID);
	}
	if (Room)
	{
		for (int i = 0; i < 6; i++)
		{
			ClientNode *RoomClient = Room->Player[i];
			if (RoomClient && RoomClient != Client)
			{
				NotifyUseProp(RoomClient, Uin,DstUin, PropID, PropIndex, CurrentPosition1, CurrentPosition2, CurrentPosition3, NewID, DstType, 0, StoneSkillType, LapTime, DstUins, PropUseMode, QueryUins, Position, 0, PropPosIdx, PropSecType, SubPropID);
			}
		}
	}
	if (Client->StoneMode == false) {
		return;
	}
	bool hasCS = false;
	UINT CSID = 0;
	for (size_t i = 0; i < 5; i++) {
		if (Client->SkillStoneList[i] == 21983 || Client->SkillStoneList[i] == 21984 || Client->SkillStoneList[i] == 21985 || Client->SkillStoneList[i] == 21986 || Client->SkillStoneList[i] == 21987 || Client->SkillStoneList[i] == 21988) {
			hasCS = true;
			CSID = Client->SkillStoneList[i];
		}
	}
	
	if (!Room)
		return;
	if (!hasCS) {
		return;
	}

	if (CSID == 21983&& Client->UseCountUpperlimit >= 1) {//1级重生
		return;

	}else if (CSID == 21984 && Client->UseCountUpperlimit >= 2) {//2级重生
		return;
	}
	else if (CSID == 21985 && Client->UseCountUpperlimit >= 4) {//3级重生
		return;
	}
	else if (CSID == 21986 && Client->UseCountUpperlimit >= 6) {//4级重生
		return;
	}
	else if (CSID == 21987 && Client->UseCountUpperlimit >= 8) {//5级重生
		return;
	}
	else if (CSID == 21988 && Client->UseCountUpperlimit >= 10) {//6级重生
		return;
	}

	if (Client->getPropNum >= 2) {
		return;
	}
	if (Client->N2OCountRequiredForReborn > 0)
	{ // 冷却
		Client->N2OCountRequiredForReborn--;
	}
	else
	{
		int Prob = 380;
		if ((rand() % 1000) + Prob > 1000)
		{

			if (hasCS) {
				Client->getPropNum++;
			}
			//Client->PropList[Client->getPropNum - 1] = 1;
			// 发动 重生
			NotifyAddPropBySkillStone(Client, PropID, 1, PropIndex);
			Client->N2OCountRequiredForReborn = 2; // TODO：在yml中配置
			Client->UseCountUpperlimit++;
		}
	}
}
void NotifySkillStoneKartMoreInfo(ClientNode *Client, size_t num);
void NotifySkillStoneKartMoreInfo(ClientNode *Client, size_t num)
{
	BYTE *buf = new BYTE[81920];
	BYTE *p = buf;
	size_t len = 0;

	if (buf == nullptr)
	{
		// 分配空间失败的处理
		return;
	}

	Write32(p, Client->Uin); // Uin
	int StoneKartNum = 0;
	int StoneGrooveNums = 0;
	bool HasMoreInfo = false;
	BYTE *pStoneKartNum = p;
	Write32(p, 0); // StoneKartNum

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	sql = "SELECT KartID,ID  FROM KartStoneGroove WHERE Uin = ? LIMIT ?,2000;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		if (stmt == nullptr)
		{
			// 空指针处理
			delete[] buf;
			return;
		}
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, num);
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{ // KartStoneGrooveInfo
			if (StoneGrooveNums > 1800)
			{
				HasMoreInfo = true;
				break;
			}
			StoneKartNum++;
			BYTE *pKartStoneGrooveInfo = p;
			Write16(pKartStoneGrooveInfo, 0); // len

			int KartID = sqlite3_column_int(stmt, 0);
			int ID = sqlite3_column_int(stmt, 1);
			Write32(pKartStoneGrooveInfo, KartID);

			BYTE *pStoneGrooveNum = pKartStoneGrooveInfo;
			Write32(pKartStoneGrooveInfo, 0); // StoneGrooveNum
			{
				int StoneGrooveNum = 0;
				const char *sql = NULL;
				sqlite3_stmt *stmt = NULL;
				int result;
				sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					if (stmt == nullptr)
					{
						// 空指针处理
						delete[] buf;
						return;
					}
					sqlite3_bind_int(stmt, 1, ID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{ // StoneGrooveInfo
						StoneGrooveNum++;

						BYTE *pStoneGrooveInfo = pKartStoneGrooveInfo;
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
				StoneGrooveNums += StoneGrooveNum;
			}
			len = pKartStoneGrooveInfo - p;
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
	Set32(pStoneKartNum, StoneKartNum);

	Write8(p, HasMoreInfo); // HasNext

#ifdef DEBUG
	Z_INFO("更高赛车技能宝珠信息 个数: %d 更多信息: %d 赛车宝石个数: %d 赛车槽数: %d \n", num, HasMoreInfo, StoneKartNum, StoneGrooveNums);
#endif

	len = p - buf;
	SendToClient(Client, 28416, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	if (HasMoreInfo)
	{
		NotifySkillStoneKartMoreInfo(Client, StoneKartNum + num);
	}

	// 释放堆空间
	delete[] buf;
}
void NotifySkillStoneKartInfo(ClientNode *Client)
{
	BYTE *buf = new BYTE[81920];
	BYTE *p = buf;
	size_t len = 0;

	if (buf == nullptr)
	{
		// 分配空间失败的处理
		return;
	}

	Write32(p, Client->Uin); // Uin
	int StoneKartNum = 0;
	int StoneGrooveNums = 0;
	bool HasMoreInfo = false;
	BYTE *pStoneKartNum = p;
	Write32(p, 0); // StoneKartNum
	{
		const char *sql = NULL;
		sqlite3_stmt *stmt = NULL;
		int result;
		sql = "SELECT KartID,ID  FROM KartStoneGroove WHERE Uin = ?;";
		result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
		if (result == SQLITE_OK)
		{
			if (stmt == nullptr)
			{
				// 空指针处理
				delete[] buf;
				return;
			}
			sqlite3_bind_int(stmt, 1, Client->Uin);
			while (sqlite3_step(stmt) == SQLITE_ROW)
			{ // KartStoneGrooveInfo
				if (StoneGrooveNums > 1800)
				{
					HasMoreInfo = true;
					break;
				}
				StoneKartNum++;
				BYTE *pKartStoneGrooveInfo = p;
				Write16(pKartStoneGrooveInfo, 0); // len

				int KartID = sqlite3_column_int(stmt, 0);
				int ID = sqlite3_column_int(stmt, 1);
				Write32(pKartStoneGrooveInfo, KartID);

				BYTE *pStoneGrooveNum = pKartStoneGrooveInfo;
				Write32(pKartStoneGrooveInfo, 0); // StoneGrooveNum
				{
					int StoneGrooveNum = 0;
					const char *sql = NULL;
					sqlite3_stmt *stmt = NULL;
					int result;
					sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
					result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
					if (result == SQLITE_OK)
					{
						if (stmt == nullptr)
						{
							// 空指针处理
							delete[] buf;
							return;
						}
						sqlite3_bind_int(stmt, 1, ID);
						while (sqlite3_step(stmt) == SQLITE_ROW)
						{ // StoneGrooveInfo
							StoneGrooveNum++;

							BYTE *pStoneGrooveInfo = pKartStoneGrooveInfo;
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
					StoneGrooveNums += StoneGrooveNum;
				}
				len = pKartStoneGrooveInfo - p;
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
		Set32(pStoneKartNum, StoneKartNum);
	}

	Write8(p, HasMoreInfo); // HasMoreInfo

#ifdef DEBUG
	Z_INFO("更高赛车技能宝珠信息 更多信息: %d 宝石个数: %d 槽位: %d \n", HasMoreInfo, StoneKartNum, StoneGrooveNums);
#endif
	len = p - buf;
	SendToClient(Client, 228, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	if (HasMoreInfo)
	{
		NotifySkillStoneKartMoreInfo(Client, StoneKartNum);
	}

	// 释放堆空间
	delete[] buf;
}

void NotifyOtherKartStoneInfo(ClientNode *Client)
{
	Client->StoneMode = true;
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;
	int ID = -1;
	sql = "SELECT ID  FROM KartStoneGroove  WHERE Uin=? AND KartID=?;";
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		sqlite3_bind_int(stmt, 1, Client->Uin);
		sqlite3_bind_int(stmt, 2, Client->KartID);
		result = sqlite3_step(stmt);
		if (result == SQLITE_ROW)
		{
			ID = sqlite3_column_int(stmt, 0);
		}
	}
	sqlite3_finalize(stmt);
	stmt = NULL;
	if (true)
	{
		return;
	}

	Write32(p, 1); // OtherStoneKartNum
	{			   // KartStoneGrooveInfo
		BYTE *paKartStoneGrooveInfo = p;
		Write16(paKartStoneGrooveInfo, 0); // len

		Write32(paKartStoneGrooveInfo, Client->Uin); // Uin
		{											 // KartStoneGrooveInfo
			BYTE *pKartStoneGrooveInfo = paKartStoneGrooveInfo;
			Write16(pKartStoneGrooveInfo, 0); // len

			Write32(pKartStoneGrooveInfo, Client->KartID); // KartID
			BYTE *pStoneGrooveNum = pKartStoneGrooveInfo;
			Write32(pKartStoneGrooveInfo, 0); // StoneGrooveNum
			{
				int StoneGrooveNum = 0;
				const char *sql = NULL;
				sqlite3_stmt *stmt = NULL;
				int result;
				sql = "SELECT StoneUseOccaType,SkillStoneID  FROM KartStone WHERE ID = ?;";
				result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
				if (result == SQLITE_OK)
				{
					sqlite3_bind_int(stmt, 1, ID);
					while (sqlite3_step(stmt) == SQLITE_ROW)
					{ // StoneGrooveInfo
						StoneGrooveNum++;

						BYTE *pStoneGrooveInfo = pKartStoneGrooveInfo;
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
			}

			len = pKartStoneGrooveInfo - paKartStoneGrooveInfo;
			Set16(paKartStoneGrooveInfo, (WORD)len);
			paKartStoneGrooveInfo += len;
		}

		len = paKartStoneGrooveInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 907, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestGetProp(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	
	
#ifdef DEBUG
	Z_INFO("当前道具栏数量%d", Client->getPropNum);
#endif
	BYTE *p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	short PropPositionNO = Read16(p);
	bool CurrentRank = Read8(p);
	int CurrentPosition1 = Read32(p);
	int CurrentPosition2 = Read32(p);
	int CurrentPosition3 = Read32(p);
	int PreviousInterval = Read32(p);
	char GetPropType = Read8(p);
	bool TeammateRank = Read8(p);
	char PointArrayIdx = Read8(p);
	UINT LapTime = Read32(p);
	char GetPropSecType = Read8(p);
	UCHAR PropPosIdx = Read8(p);
	UINT NPCUin = Read32(p);
	UCHAR ParaNum = Read8(p);
	// UINT ParaList[] = Read32(p);
	ResponseGetProp(Client);
}
void ResponseGetProp(ClientNode *Client)
{
	if (Client->getPropNum >= 2)
	{
		return;
	}
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	static WORD PropIDs[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 15, 16, 18, 21, 25, 27, 41, 43, 44, 45, 46, 47, 49, 50, 51, 53};
	static int Index = 0;
	Index++;
	if (Index >= sizeof(PropIDs) / sizeof(WORD))
	{
		Index = 0;
	}
	Z_INFO("%d\n", Index);
	if (Client->getPropNum >= 2) {
		return;
	}
	else {
		Client->NumPropsInHand += 1;
		Client->getPropNum++;
		//Client->PropList[Client->getPropNum - 1] = PropIDs[Index];
	}
	Write16(p, 0);				// ResultID
	Write32(p, Client->Uin);	// Uin
	Write16(p, PropIDs[Index]); // PropID
	Write32(p, 0);				// PropIndex
	Write8(p, 0);				// GetPropType
	Write32(p, 0);				// AwardItemID
	Write8(p, 0);				// ReasonLen
	Write16(p, 0);				// PropPositionNO
	Write32(p, 0);				// ActIDForClient
#ifndef ZingSpeed
	Write8(p, 0);  // GetPropSecType
	Write8(p, 0);  // PropPosIdx
	Write32(p, 0); // NPCUin
	Write8(p, 0);  // ParaNum
	Write32(p, 0); // ParaList[]
#endif

	len = p - buf;
	SendToClient(Client, 124, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void ResponseGetPropS(ClientNode *Client, int index)
{
	if (Client->getPropNum>=2) {
		return;
	}
	else {
		Client->getPropNum++;
		//Client->PropList[Client->getPropNum-1] = index;
	}
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	// static WORD PropIDs[] = { 1,2,3,4,5,6,7,8,9,10,15,16,18,21,25,27,41,43,44,45,46,47,49,50,51,53 };
	// static int Index = 0;
	// printf("%d\n", Index);

	Write16(p, 0);			 // ResultID
	Write32(p, Client->Uin); // Uin
	Write16(p, index);		 // PropID
	Write32(p, 0);			 // PropIndex
	Write8(p, 0);			 // GetPropType
	Write32(p, 0);			 // AwardItemID
	Write8(p, 0);			 // ReasonLen
	Write16(p, 0);			 // PropPositionNO
	Write32(p, 0);			 // ActIDForClient
#ifndef ZingSpeed
	Write8(p, 0);  // GetPropSecType
	Write8(p, 0);  // PropPosIdx
	Write32(p, 0); // NPCUin
	Write8(p, 0);  // ParaNum
	Write32(p, 0); // ParaList[]
#endif

	len = p - buf;
	SendToClient(Client, 124, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyMsgBox(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write8(p, 0); // MsgType
	{			  // MsgBox
		BYTE *pMsgBox = p;
		Write16(pMsgBox, 0); // len

		{ // OnlineTaskMsg
			BYTE *pOnlineTaskMsg = pMsgBox;
			Write16(pOnlineTaskMsg, 0); // len

			Write8(pOnlineTaskMsg, 0);	// MsgType
			Write32(pOnlineTaskMsg, 0); // OpenDate
			Write32(pOnlineTaskMsg, 0); // OpenTime
			Write32(pOnlineTaskMsg, 0); // CloseDate
			Write32(pOnlineTaskMsg, 0); // CloseTime
			Write8(pOnlineTaskMsg, 0);	// IsToday
			Write8(pOnlineTaskMsg, 0);	// URLLen
			Write8(pOnlineTaskMsg, 0);	// WeekLimit

			len = pOnlineTaskMsg - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ // VipPages
			BYTE *pVipPages = pMsgBox;
			Write16(pVipPages, 0); // len

			Write8(pVipPages, 0); // URLLen1
			Write8(pVipPages, 0); // URLLen2
			Write8(pVipPages, 0); // URLLen3

			len = pVipPages - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ // ActivitiesAd
			BYTE *pActivitiesAd = pMsgBox;
			Write16(pActivitiesAd, 0); // len

			Write8(pActivitiesAd, 0); // URLLen

			len = pActivitiesAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ // MatchSchedule
			BYTE *pMatchSchedule = pMsgBox;
			Write16(pMatchSchedule, 0); // len

			Write8(pMatchSchedule, 0); // URLLen

			len = pMatchSchedule - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ // LoadingAd
			BYTE *pLoadingAd = pMsgBox;
			Write16(pLoadingAd, 0); // len

			Write8(pLoadingAd, 0); // Ver
			Write8(pLoadingAd, 0); // URLLen

			len = pLoadingAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ // TaskAdvAd
			BYTE *pTaskAdvAd = pMsgBox;
			Write16(pTaskAdvAd, 0); // len

			Write8(pTaskAdvAd, 0); // URLLen

			len = pTaskAdvAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ // LoginAd
			BYTE *pLoginAd = pMsgBox;
			Write16(pLoginAd, 0); // len

			Write8(pLoginAd, 0); // URLLen

			len = pLoginAd - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}
		{ // T3Ad
			BYTE *pT3Ad = pMsgBox;
			Write16(pT3Ad, 0); // len

			Write8(pT3Ad, 0); // URLLen1
			Write8(pT3Ad, 0); // URLLen2
			Write8(pT3Ad, 0); // URLLen3
			Write8(pT3Ad, 0); // URLLen4

			len = pT3Ad - pMsgBox;
			Set16(pMsgBox, (WORD)len);
			pMsgBox += len;
		}

		len = pMsgBox - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 596, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Notify);
}

void NotifySvrConfig(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, 5000); // GuildScoreThreshold

	{ // LuckyMatchCfg
		BYTE *pLuckyMatchCfg = p;
		Write16(pLuckyMatchCfg, 0); // len

		const char Time[50] = "";
		memcpy(pLuckyMatchCfg, Time, 50);
		pLuckyMatchCfg += 50;

		const char Award[50] = "";
		memcpy(pLuckyMatchCfg, Award, 50);
		pLuckyMatchCfg += 50;

		const char StartCondition[50] = "";
		memcpy(pLuckyMatchCfg, StartCondition, 50);
		pLuckyMatchCfg += 50;

		Write32(pLuckyMatchCfg, 0); // LuckyMoney

		const char MatchTitle[50] = "";
		memcpy(pLuckyMatchCfg, MatchTitle, 50);
		pLuckyMatchCfg += 50;

		len = pLuckyMatchCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // MsgStoneSysCfg
		BYTE *pMsgStoneSysCfg = p;
		Write16(pMsgStoneSysCfg, 0); // len

		Write8(pMsgStoneSysCfg, 1); // StoneSkillTakeEffect

		{ // SkillStoneCfg
			BYTE *pSkillStoneCfg = pMsgStoneSysCfg;
			Write16(pSkillStoneCfg, 0); // len

			BYTE *pSkillStoneNum = pSkillStoneCfg;
			Write32(pSkillStoneCfg, 0); // SkillStoneNum
			try
			{
				YAML::Node Config = YAML::LoadFile(".\\Config\\SingleSkillStoneCfg.yml");
				size_t Num = Config.size();
				for (size_t i = 0; i < Num; i++)
				{
					YAML::Node SingleSkillStoneCfg = Config[i];
					{ // SingleSkillStoneCfg
						BYTE *pSingleSkillStoneCfg = pSkillStoneCfg;
						Write16(pSingleSkillStoneCfg, 0); // len

						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["StoneID"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["ActiveCondVal1"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["UseCountUpperlimit"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["ActiveSuccessProb"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["CoolTime"].as<int>());
						Write32(pSingleSkillStoneCfg, SingleSkillStoneCfg["GenResultVal1"].as<int>());

						len = pSingleSkillStoneCfg - pSkillStoneCfg;
						Set16(pSkillStoneCfg, (WORD)len);
						pSkillStoneCfg += len;
					}
				}
				Set32(pSkillStoneNum, Num);
			}
			catch (const std::exception &)
			{
				return;
			}

			len = pSkillStoneCfg - pMsgStoneSysCfg;
			Set16(pMsgStoneSysCfg, (WORD)len);
			pMsgStoneSysCfg += len;
		}

		Write32(pMsgStoneSysCfg, 0); // Kart2StoneGrooveNum
		/*
m_stMsgStoneSysCfg.m_astKart2StoneGrooveCfg[].m_iKartID
m_stMsgStoneSysCfg.m_astKart2StoneGrooveCfg[].m_iInitStoneGrooveNum
m_stMsgStoneSysCfg.m_astKart2StoneGrooveCfg[].m_aiStoneUseOccasionType[MAX_KART_STONE_GROOVE_NUM]
		*/

		len = pMsgStoneSysCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // UITitleDispInfo
		BYTE *pUITitleDispInfo = p;
		Write16(pUITitleDispInfo, 0); // len

		struct
		{
			bool GameName : 1;
			bool Version : 1;
			bool WorldName : 1;
			bool Ad : 1;
			bool NickName : 1;
			bool Unknown1 : 1;
			bool Unknown2 : 1;
			bool Unknown3 : 1;
		} DisplayBitMap{0, 0, 1, 1, 1, 1, 1, 1};
		memcpy(pUITitleDispInfo, &DisplayBitMap, sizeof(DisplayBitMap));

		std::string text = ServerConfig["Server"]["GameName"].as<std::string>();
		size_t len1 = text.size();
		text += "】【6.03-6.04在线送童趣斑斓戒指+A车风火轮+ 10888点券+6倍开启! ";
		const char *ANSI = text.data();

		UITitleDispInfo_ADText = ANSI;

		pUITitleDispInfo += sizeof(DisplayBitMap); // 加上字符串长度和结尾符的长度
		{										   // ADText
			BYTE *pADText = pUITitleDispInfo;
			Write16(pADText, 0); // len

			len = strlen(UITitleDispInfo_ADText);
			Write8(pADText, (BYTE)len); // TextLen
			memcpy(pADText, UITitleDispInfo_ADText, len);
			pADText += len;

			len = pADText - pUITitleDispInfo;
			Set16(pUITitleDispInfo, (WORD)len);
			pUITitleDispInfo += len;
		}
		len = pUITitleDispInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // LoaderTipInfo
		BYTE *pLoaderTipInfo = p;
		Write16(pLoaderTipInfo, 0); // len

		Write32(pLoaderTipInfo, 0); // Version
		Write8(pLoaderTipInfo, 0);	// TipsNum
		// m_stLoaderTipInfo.m_astLoaderTipText[].m_u8TextLen

		len = pLoaderTipInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // MailSysAD
		BYTE *pMailSysAD = p;
		Write16(pMailSysAD, 0); // len

		const char *ADText = "MailMC";
		len = strlen(ADText);
		Write16(pMailSysAD, (WORD)len); // TextLen
		memcpy(pMailSysAD, ADText, len);
		pMailSysAD += len;

		len = pMailSysAD - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // CheatReportCfg
		BYTE *pCheatReportCfg = p;
		Write16(pCheatReportCfg, 0); // len

		Write8(pCheatReportCfg, 1); // ReplayDayNum
		Write8(pCheatReportCfg, 1); // IsReportCheatEnable

		len = pCheatReportCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	UINT MaintainableKartNum = 0;
	Write8(p, MaintainableKartNum); // MaintainableKartNum
	// m_aiMaintainableKartID[]

	Write32(p, 1); // CommonBitSwitch
	Write8(p, 1);  // QQKart2ButtonLen

	{ // GuildEnlistCfg
		BYTE *pGuildEnlistCfg = p;
		Write16(pGuildEnlistCfg, 0); // len

		Write8(pGuildEnlistCfg, 0); // MinUnloginDay
		Write8(pGuildEnlistCfg, 0); // CdDay
		Write8(pGuildEnlistCfg, 0); // MaxAwardTimeOneDay

		len = pGuildEnlistCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, true); // CloseQT
	Write32(p, 0);	 // QTCommonRoomID
	Write8(p, 0);	 // WlFetionDayLimit
	Write8(p, 1);	 // ShowShopIcon
	Write32(p, 0);	 // TiroRecommendMapNum
	// m_aiTiroRecommendMapIdList[]
	Write16(p, 0); // BoxMutiOpenLimit
	Write8(p, 1);  // HallButtonShow
	Write8(p, 0);  // HallButtonShowUrlLen
	Write32(p, 0); // EndlessModeBaseScorePerLevel
#ifndef ZingSpeed
	Write32(p, 0); // GetShopNonSaleInfoInterval
	{			   // ShuttleRandMap
		BYTE *pShuttleRandMap = p;
		Write16(pShuttleRandMap, 0); // len

		Write16(pShuttleRandMap, 0); // BeginNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //BeginMapList
			BYTE* pBeginMapList = pShuttleRandMap;
			Write16(pBeginMapList, 0); //len

			Write32(pBeginMapList, 0); //MapID
			Write16(pBeginMapList, 0); //BeginIdx
			Write16(pBeginMapList, 0); //EndIdx
			Write8(pBeginMapList, 0); //Reverse
			Write16(pBeginMapList, 0); //BeginCheckPointIdx

			len = pBeginMapList - pShuttleRandMap;
			Set16(pShuttleRandMap, (WORD)len);
			pShuttleRandMap += len;
		}
		*/
		Write16(pShuttleRandMap, 0); // MidNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //MidMapList
			BYTE* pMidMapList = pShuttleRandMap;
			Write16(pMidMapList, 0); //len

			Write32(pMidMapList, 0); //MapID
			Write16(pMidMapList, 0); //BeginIdx
			Write16(pMidMapList, 0); //EndIdx
			Write8(pMidMapList, 0); //Reverse
			Write16(pMidMapList, 0); //BeginCheckPointIdx

			len = pMidMapList - pShuttleRandMap;
			Set16(pShuttleRandMap, (WORD)len);
			pShuttleRandMap += len;
		}
		*/
		Write16(pShuttleRandMap, 0); // EndNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //EndMapList
			BYTE* pEndMapList = pShuttleRandMap;
			Write16(pEndMapList, 0); //len

			Write32(pEndMapList, 0); //MapID
			Write16(pEndMapList, 0); //BeginIdx
			Write16(pEndMapList, 0); //EndIdx
			Write8(pEndMapList, 0); //Reverse
			Write16(pEndMapList, 0); //BeginCheckPointIdx

			len = pEndMapList - pShuttleRandMap;
			Set16(pShuttleRandMap, (WORD)len);
			pShuttleRandMap += len;
		}
		*/
		len = pShuttleRandMap - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // HasWeeklyRecommendModeClientInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //WeeklyRecommendModeClientInfo
		BYTE* pWeeklyRecommendModeClientInfo = p;
		Write16(pWeeklyRecommendModeClientInfo, 0); //len

		Write32(pWeeklyRecommendModeClientInfo, 0); //OpenDate
		Write32(pWeeklyRecommendModeClientInfo, 0); //CloseDate
		Write32(pWeeklyRecommendModeClientInfo, 0); //OpenTime
		Write32(pWeeklyRecommendModeClientInfo, 0); //CloseTime
		Write8(pWeeklyRecommendModeClientInfo, 0); //WeekLimitNum
		Write8(pWeeklyRecommendModeClientInfo, 0); //WeekLimits[]
		Write32(pWeeklyRecommendModeClientInfo, 0); //DesktopSpecialShowFlag
		Write16(pWeeklyRecommendModeClientInfo, 0); //BaseMode
		Write16(pWeeklyRecommendModeClientInfo, 0); //SubMode
		Write32(pWeeklyRecommendModeClientInfo, 0); //SpecialFlag
		Write(pWeeklyRecommendModeClientInfo, 0); //AwardTriggerDesc[]
		Write8(pWeeklyRecommendModeClientInfo, 0); //AwardNum
		Write32(pWeeklyRecommendModeClientInfo, 0); //AwardItemID[]

		len = pWeeklyRecommendModeClientInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0);	   // ReportWeeklyRecommendModeTriggerInfoInterval
	Write32(p, 0); // MaxEnergy
	Write32(p, 0);	   // N2ORatio
	Write32(p, 0);	   // PlayerNumRatio[]
	Write32(p, 1);	   // IsOpenBuf
	Write8(p, 1);	   // CouponTreasureGameButtonShow
	Write8(p, 1);	   // NewYearFinancingShow
	Write32(p, 0);	   // GuildTeamSignatureReportNumsLimit
	Write32(p, 0);	   // GetRelationItemMaxTimeout
	Write32(p, 0);	   // ClientWaitGameBeginTimeout

#endif

	len = p - buf;
	SendToClient(Client, 800, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifySvrConfig2(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write8(p, 0); // HaveTopMapRecord
	/*
	{ //TopMapRecordConfig
		BYTE* pTopMapRecordConfig = p;
		Write16(pTopMapRecordConfig, 0); //len

		Write32(pTopMapRecordConfig, 0); //Version
		Write32(pTopMapRecordConfig, 0); //RecordDisplayLimit
		Write8(pTopMapRecordConfig, 0); //RecordNumber
		for (size_t i = 0; i < n; i++)
		{ //Record
			BYTE* pRecord = pTopMapRecordConfig;
			Write16(pRecord, 0); //len

			Write32(pRecord, 0); //MapId
			Write32(pRecord, 0); //Record
			Write32(pRecord, 0); //Uin
			Write8(pRecord, 0); //WorldId
			Write8(pRecord, 0); //NameLength
			Write8(pRecord, 0); //UrlLength

			len = pRecord - pTopMapRecordConfig;
			Set16(pTopMapRecordConfig, (WORD)len);
			pTopMapRecordConfig += len;
		}

		len = pTopMapRecordConfig - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // HaveKartRoomPushInfo

	/* { // KartRoomPushInfo
		BYTE *pKartRoomPushInfo = p;
		Write16(pKartRoomPushInfo, 0); // len

		Write32(pKartRoomPushInfo, 0); // Version
		Write16(pKartRoomPushInfo, 1); // PushItemNumber
		for (size_t i = 0; i < 1; i++)
		{ // PushItem
			BYTE *pPushItem = pKartRoomPushInfo;
			Write16(pPushItem, 0); // len

			Write32(pPushItem, 10020); // KartId
			Write8(pPushItem, 0);	   // TextLength

			len = pPushItem - pKartRoomPushInfo;
			Set16(pKartRoomPushInfo, (WORD)len);
			pKartRoomPushInfo += len;
		}

		len = pKartRoomPushInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}*/

	{ // ClickStreamCfg
		BYTE *pClickStreamCfg = p;
		Write16(pClickStreamCfg, 0); // len

		Write8(pClickStreamCfg, 2); // ReportMode

		Write8(pClickStreamCfg, 3); // ContextNum
		Write8(pClickStreamCfg, 1);
		Write8(pClickStreamCfg, 2);
		Write8(pClickStreamCfg, 3);

		Write32(pClickStreamCfg, 100); // MaxNum

		len = pClickStreamCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 1); // IsGuildPKOpen
	Write8(p, 1); // IsEightYearsOpen
	Write8(p, 1); // HaveSkatePropGameLogicCfg

	{ // SkatePropGameLogicCfg
		BYTE *pSkatePropGameLogicCfg = p;
		Write16(pSkatePropGameLogicCfg, 0); // len

		Write32(pSkatePropGameLogicCfg, 0); // PropAddN2ONum
		Write32(pSkatePropGameLogicCfg, 0); // N2OSpeed
		Write32(pSkatePropGameLogicCfg, 0); // ZanAddScoreNum
		Write32(pSkatePropGameLogicCfg, 0); // GetSpecialPropAddCoinNum
		Write32(pSkatePropGameLogicCfg, 0); // SkatePropHitCD
		Write32(pSkatePropGameLogicCfg, 0); // SkatePropHit_CoinSubNum
		Write32(pSkatePropGameLogicCfg, 0); // MaxOneGameLogicGetScoreNum

		len = pSkatePropGameLogicCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write16(p, 0); // SkatePropShoeNum
	// Write32(p, 0); //SkatePropShoeList[]

	Write16(p, 0); // SkatePropShoeNumForMultiGame
	// Write32(p, 0); //SkatePropShoeListForMultiGame[]

	Write32(p, 1); // AddAngleTime

	const int EMBM_MAX = 47;
	for (size_t i = 0; i < EMBM_MAX; i++)
	{ // ClassicMapInfo
		BYTE *pClassicMapInfo = p;
		Write16(pClassicMapInfo, 0); // len

		Write32(pClassicMapInfo, 0); // MapNums
		// Write32(pClassicMapInfo, 0); //MapIDS[]

		len = pClassicMapInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 0);  // OpenPopAds
	Write8(p, 0);  // IsCloseSaveKeyTransInfo
	Write8(p, 0);  // IsCloseSuperMoneySecondPsw
	Write32(p, 0); // NewMapNum

	for (size_t i = 0; i < 0; i++)
	{ // NewMapTipsCfg
		BYTE *pNewMapTipsCfg = p;
		Write16(pNewMapTipsCfg, 0); // len

		Write32(pNewMapTipsCfg, 104); // Mapid
		Write16(pNewMapTipsCfg, 0);	  // WordLen

		len = pNewMapTipsCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	for (size_t i = 0; i < 16; i++)
	{
		Write8(p, 0); // ClientSwitch[]
	}

	Write8(p, 1); // OpenPwdRedEnvelop
	Write8(p, 1); // IsUseTCLS
	Write8(p, 0); // IsPropTeamMatchShow

	Write32(p, 0); // CfgNums
	/*
	for (size_t i = 0; i < 1; i++)
	{ //CommonItemTopListInfos
		BYTE* pCommonItemTopListInfos = p;
		Write16(pCommonItemTopListInfos, 0); //len

		Write32(pCommonItemTopListInfos, 75827); //ItemID

		Write16(pCommonItemTopListInfos, 1); //ExtendInfoNums
		Write32(pCommonItemTopListInfos, 0); //ExtendInfoForItemGene[]

		Write16(pCommonItemTopListInfos, 1); //UpdateTopListNums
		Write32(pCommonItemTopListInfos, 0); //TopListID[]

		Write32(pCommonItemTopListInfos, 0); //ActiveDate
		Write32(pCommonItemTopListInfos, 0); //InActiveDate
		Write32(pCommonItemTopListInfos, 0); //ActiveTime
		Write32(pCommonItemTopListInfos, 0); //InActiveTime
		Write8(pCommonItemTopListInfos, 1); //UpdateTopList
		Write32(pCommonItemTopListInfos, 0); //ExtendInfoForLastUpdateTime
		Write32(pCommonItemTopListInfos, 0); //DailyTopListExtendInfoID
		Write32(pCommonItemTopListInfos, 0); //UpdateDate

		len = pCommonItemTopListInfos - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 1);  // IsShowBoardAllareaBubble
	Write8(p, 0);  // RemindClearSuperMoneyLeftDays
	Write8(p, 0);  // RemindClearSuperMoneyLeftHours
	Write32(p, 0); // RemindClearSuperMoneyThreshold
	Write16(p, 0); // RemindClearSuperMoneyInterval
	Write32(p, 0); // HPJMaxAccFuel
	Write32(p, 0); // HPJWildStatusAccelParam
	Write8(p, 0);  // SwitchNum
	Write8(p, 1);  // UseSign3
	Write8(p, 1);  // OpenRankedMatchSpeedKing
	Write8(p, 0);  // OpenLimitMapCollect
	Write8(p, 0);  // HasMoreInfo

	len = p - buf;
	SendToClient(Client, 822, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
//void NotifySvrConfig3(ClientNode* Client)
//{
//	BYTE buf[28192] = { 0 };
//	BYTE* p = buf;
//	size_t len = 0;
//	typedef struct CPhysInfo {
//
//		UINT PID;
//		UINT AffectID;
//		UINT Param;
//		UINT LifeTime;
//
//	};
//	typedef struct COMPENSATEPARAMCOUNT {
//
//		UINT Percent;
//		UINT Value;
//
//	};
//	typedef struct NPCFORWRDACCEL {
//
//		UINT Key;
//		UINT Value;
//	};
//	typedef struct NPCFORWRDDECEL {
//
//		UINT Key;
//		UINT Value;
//	};
//
//
//	typedef struct NPCBACKWARDACCEL {
//
//		UINT Key;
//		UINT Value;
//	};
//	typedef struct NPCBACKWARDDECEL {
//
//		UINT Key;
//		UINT Value;
//	};
//	const static CPhysInfo NPCKartPhysPara[] = {
//		{1,100,6000,3200},
//		{2001,100,6000,580},
//		{2002,100,6000,4100},
//		{99999,100,83000,5000},
//
//	};
//
//	const static COMPENSATEPARAMCOUNT CompensateParamCount[] = {
//		{2,1010},
//		{10,1010},
//		{20,1020},
//		{60,1080},
//		{120,1160},
//		{240,1200},
//		{900,1200},
//	};
//	const static UINT NPCAccelStatusCount[]{
//
//	1200,1200,800,300,2000,1900,
//
//	};
//	const static NPCFORWRDACCEL ForwardAccel[]{
//		{0,4000},
//		{5000,4000},
//		{5700,4500},
//		{9000,5000},
//		{9300,5500},
//		{11000,5800},
//		{11000,5800},
//		{15000,6300},
//		{16000,6300},
//		{20000,6300},
//		{25000,6300},
//		{30000,6300},
//		{35000,6300},
//	};
//	const static NPCFORWRDDECEL ForwardDecel[]{
//
//		 {0,-8000},
//		{2000,-7000},
//		{4000,-6000},
//		{6000,-5000},
//		{18000,-4000},
//
//	};
//	const static NPCBACKWARDACCEL BackwardAccel[]{
//
//		 {0,-5000},
//		{3000,-5000},
//		{6000,-5000},
//		{10000,-5000},
//		{12000,-5500},
//
//	};
//	const static NPCBACKWARDDECEL BackwardDecel[]{
//
//		 {0,16000},
//		{2000,16000},
//		{4000,16000},
//		{6000,15000},
//		{10000,15000},
//
//	};
//	{ //NPCKartPhysPara
//		BYTE* pNPCKartPhysPara = p;
//		Write16(pNPCKartPhysPara, 0); //len
//
//		Write32(pNPCKartPhysPara, 0); //KartID
//		Write8(pNPCKartPhysPara, sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0])); //PhysInfoCount
//		for (size_t i = 0; i < sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0]); i++)
//		{ //PhysInfo
//			BYTE* pPhysInfo = pNPCKartPhysPara;
//			Write16(pPhysInfo, 0); //len
//
//			Write32(pPhysInfo, NPCKartPhysPara[i].PID); //PID
//			Write32(pPhysInfo, NPCKartPhysPara[i].AffectID); //AffectID
//			Write32(pPhysInfo, NPCKartPhysPara[i].Param); //Param
//			Write32(pPhysInfo, NPCKartPhysPara[i].LifeTime); //LifeTime
//
//			len = pPhysInfo - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
//		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
//		{ //CompensateParam
//			BYTE* pCompensateParam = pNPCKartPhysPara;
//			Write16(pCompensateParam, 0); //len
//
//			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
//			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value
//
//			len = pCompensateParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		{ //CollisionBoxParam
//			BYTE* pCollisionBoxParam = pNPCKartPhysPara;
//			Write16(pCollisionBoxParam, 0); //len
//
//			Write32(pCollisionBoxParam, 300); //BoxLength
//			Write32(pCollisionBoxParam, 170); //BoxWidth
//			Write32(pCollisionBoxParam, 240); //BoxHeight
//			Write32(pCollisionBoxParam, 240); //AdjustOffset
//			Write32(pCollisionBoxParam, 200); //AdjustPickGap
//			Write32(pCollisionBoxParam, 190); //AdjustRemoteOffset
//			Write32(pCollisionBoxParam, 200); //AdjustBevelOffset
//			Write32(pCollisionBoxParam, -30); //ControllerOffset
//
//			len = pCollisionBoxParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		{ //CarToCarCollisionParam
//			BYTE* pCarToCarCollisionParam = pNPCKartPhysPara;
//			Write16(pCarToCarCollisionParam, 0); //len
//
//			Write32(pCarToCarCollisionParam, 300); //CollisionRadius
//			Write32(pCarToCarCollisionParam, 300); //CollisionLength
//			Write32(pCarToCarCollisionParam, 200); //CollisionWidth
//			Write32(pCarToCarCollisionParam, 250); //CollisionHeight
//			Write32(pCarToCarCollisionParam, 100000); //CollisionLostCtrlTime
//
//			len = pCarToCarCollisionParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write32(pNPCKartPhysPara, 1000); //BaseTurnRate
//		Write32(pNPCKartPhysPara, 1100); //VolatileTurnRate
//		Write32(pNPCKartPhysPara, 12000); //MaxTurnSpeed
//		Write32(pNPCKartPhysPara, 5700); //MinTurnSpeed
//		Write32(pNPCKartPhysPara, 700); //MaxAccuTime
//		Write32(pNPCKartPhysPara, 150); //BaseAccuRate
//		Write32(pNPCKartPhysPara, 19000); //MaxAffectSpeed
//		Write32(pNPCKartPhysPara, -15000); //Gravity
//		Write32(pNPCKartPhysPara, -2400); //AdditionalLocalZSpeed
//		Write32(pNPCKartPhysPara, 18000); //StartVec
//		Write32(pNPCKartPhysPara, 50000); //EndVecFist
//		Write32(pNPCKartPhysPara, 16000); //EndVecSecon
//		Write32(pNPCKartPhysPara, 1500); //DirKeyForce
//		Write32(pNPCKartPhysPara, 5900); //DirKeyTwist
//		Write32(pNPCKartPhysPara, 4200); //BannerTwist
//		Write32(pNPCKartPhysPara, 6000); //BannerKeyTwist
//		Write32(pNPCKartPhysPara, 6000); //BannerVecForce
//		Write32(pNPCKartPhysPara, 5500); //BannerHeadForce
//		Write32(pNPCKartPhysPara, 2600); //SlidFricForce
//		Write32(pNPCKartPhysPara, 1000); //RollFricForce
//		Write32(pNPCKartPhysPara, 800); //StartWec
//		Write32(pNPCKartPhysPara, 3500); //MaxWec
//		Write32(pNPCKartPhysPara, 5000); //SuaiJianTwist
//		Write32(pNPCKartPhysPara, 10000); //DirUpKeyForce
//		Write8(pNPCKartPhysPara, 6); //AccelStatusCount
//		for (size_t i = 0; i < 6; i++) {
//			Write32(pNPCKartPhysPara, NPCAccelStatusCount[i]); //AccelStatus[]
//		}
//		Write8(pNPCKartPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
//		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
//		{ //ForwardAccel
//			BYTE* pForwardAccel = pNPCKartPhysPara;
//			Write16(pForwardAccel, 0); //len
//
//			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
//			Write32(pForwardAccel, ForwardAccel[i].Value); //Value
//
//			len = pForwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(ForwardDecel) / sizeof(ForwardDecel[0])); //ForwardDecelNum
//		for (size_t i = 0; i < sizeof(ForwardDecel) / sizeof(ForwardDecel[0]); i++)
//		{ //ForwardDecel
//			BYTE* pForwardDecel = pNPCKartPhysPara;
//			Write16(pForwardDecel, 0); //len
//
//			Write32(pForwardDecel, ForwardDecel[i].Key); //Key
//			Write32(pForwardDecel, ForwardDecel[i].Value); //Value
//
//			len = pForwardDecel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
//		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
//		{ //BackwardAccel
//			BYTE* pBackwardAccel = pNPCKartPhysPara;
//			Write16(pBackwardAccel, 0); //len
//
//			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
//			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value
//
//			len = pBackwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(BackwardDecel) / sizeof(BackwardDecel[0])); //BackwardDecelNum
//		for (size_t i = 0; i < sizeof(BackwardDecel) / sizeof(BackwardDecel[0]); i++)
//		{ //BackwardDecel
//			BYTE* pBackwardDecel = pNPCKartPhysPara;
//			Write16(pBackwardDecel, 0); //len
//
//			Write32(pBackwardDecel, BackwardDecel[i].Key); //Key
//			Write32(pBackwardDecel, BackwardDecel[i].Value); //Value
//
//			len = pBackwardDecel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write32(pNPCKartPhysPara, 0); //CrashCompensatePower
//		Write32(pNPCKartPhysPara, 0); //DefenseRate
//		Write32(pNPCKartPhysPara, 0); //Uin
//		Write32(pNPCKartPhysPara, 0); //SpeedupCardGenRate
//		Write32(pNPCKartPhysPara, 0); //ExtraFuncFlag
//		Write8(pNPCKartPhysPara, 0); //SkatePara
//		/*for (size_t i = 0; i < 0; i++)
//		{ //hasSkatePara
//			BYTE* phasSkatePara = pNPCKartPhysPara;
//			Write16(phasSkatePara, 0); //len
//
//			Write32(phasSkatePara, 0); //TrackSpeed
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirBigJet
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_Jump
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirJetPoint
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_MaxForce
//			Write32(phasSkatePara, 0); //OntrackAccRef
//			Write32(phasSkatePara, 0); //TrackAccRef
//			Write32(phasSkatePara, 0); //DashSkill
//
//			len = phasSkatePara - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //HasBattleModeSkillInfoList
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BattleModeSkillInfoList
//			BYTE* pBattleModeSkillInfoList = pNPCKartPhysPara;
//			Write16(pBattleModeSkillInfoList, 0); //len
//
//			Write8(pBattleModeSkillInfoList, 0); //SkillNum
//			for (size_t i = 0; i < 0; i++)
//			{ //SkillInfo
//				BYTE* pSkillInfo = pBattleModeSkillInfoList;
//				Write16(pSkillInfo, 0); //len
//
//				Write16(pSkillInfo, 0); //SkillId
//				Write8(pSkillInfo, 0); //SkillLevel
//
//				len = pSkillInfo - pBattleModeSkillInfoList;
//				Set16(pBattleModeSkillInfoList, (WORD)len);
//				pBattleModeSkillInfoList += len;
//			}
//
//			len = pBattleModeSkillInfoList - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write32(pNPCKartPhysPara, 0); //EnergyConvert
//		Write16(pNPCKartPhysPara, 0); //SuperN2ORate
//		Write16(pNPCKartPhysPara, 0); //CollisionLoseRate
//		Write8(pNPCKartPhysPara, 0); //NotInterruptDrift
//		Write8(pNPCKartPhysPara, 0); //AddWeightNum
//		Write8(pNPCKartPhysPara, 0); //WeightNum
//		Write8(pNPCKartPhysPara, 0); //HasAntiCollisionCfg
//		/*for (size_t i = 0; i < 0; i++)
//		{ //AntiCollistionCfg
//			BYTE* pAntiCollistionCfg = pNPCKartPhysPara;
//			Write16(pAntiCollistionCfg, 0); //len
//
//			Write8(pAntiCollistionCfg, 0); //Num
//			Write32(pAntiCollistionCfg, 0); //Position[]
//
//			len = pAntiCollistionCfg - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //HasBoatParam
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BoatParam
//			BYTE* pBoatParam = pNPCKartPhysPara;
//			Write16(pBoatParam, 0); //len
//
//			Write32(pBoatParam, 0); //DirKeyForceParamC
//			Write32(pBoatParam, 0); //DirKeyForceParamD
//			Write32(pBoatParam, 0); //DTFLimitAngle
//			Write32(pBoatParam, 0); //DTFRatio
//			Write32(pBoatParam, 0); //UpKeyVecForce
//			Write32(pBoatParam, 0); //UpKeyVecForceParamA
//			Write32(pBoatParam, 0); //UpKeyVecForceParamB
//			Write32(pBoatParam, 0); //LRKeyVecForce
//			Write32(pBoatParam, 0); //LRKeyVecForceParamA
//			Write32(pBoatParam, 0); //LRKeyVecForceParamB
//			Write32(pBoatParam, 0); //VecForce
//			Write32(pBoatParam, 0); //VecForceParamA
//			Write32(pBoatParam, 0); //VecForceParamB
//			Write32(pBoatParam, 0); //BoatShape[]
//			Write32(pBoatParam, 0); //WecEffect
//			Write32(pBoatParam, 0); //BannerKeyTwist
//			Write32(pBoatParam, 0); //BannerKeyTwistParamA
//			Write32(pBoatParam, 0); //BannerKeyTwistParamB
//			Write32(pBoatParam, 0); //SuaiJianTwist
//			Write32(pBoatParam, 0); //BoatHeighRatio
//			Write32(pBoatParam, 0); //BannerTwist
//			Write32(pBoatParam, 0); //BannerTwistParamA
//			Write32(pBoatParam, 0); //DirUpKeyForce
//			Write32(pBoatParam, 0); //DirUpKeyForceParamA
//			Write32(pBoatParam, 0); //DirUpKeyForceParamB
//			Write32(pBoatParam, 0); //DirKeyForce
//			Write32(pBoatParam, 0); //DirKeyForceParamA
//			Write32(pBoatParam, 0); //DirKeyForceParamB
//			Write32(pBoatParam, 0); //AddNO2PerSec
//			Write32(pBoatParam, 0); //BoatAirValue
//			WriteUn(pBoatParam, 0); //ifSlidFricForce
//
//			len = pBoatParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //BoatForwardAccelNum
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BoatForwardAccel
//			BYTE* pBoatForwardAccel = pNPCKartPhysPara;
//			Write16(pBoatForwardAccel, 0); //len
//
//			Write32(pBoatForwardAccel, 0); //Key
//			Write32(pBoatForwardAccel, 0); //Value
//
//			len = pBoatForwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamA
//		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamB
//
//		len = pNPCKartPhysPara - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//	const int MAX_NPC_AI_LEVEL_NUM = 12;
//	typedef struct NPCKARTAIPHYSPARA {
//
//		UINT NpcN2OIntervalTime;
//		UINT NpcTriggerN2ORate;
//		UINT NpcRestCheckTime;
//		UINT NpcFixVelocity;
//		UINT NpcFixVelocityRate;
//		UINT NpcRunDisHulanThreshold;
//		UINT NpcSJetRate;
//		UINT NpcTurnAjustDisHulanRate;
//
//
//	};
//	const static NPCKARTAIPHYSPARA NPCKartAIPhysPara[]{
//		{18000,0,1950,105,100,1,20,100},
//		{16000,0,1900,110,100,1,20,100},
//		{12000,100,1850,115,100,1,30,100},
//		{11000,100,1800,120,100,1,30,30},
//		{10000,100,1750,125,100,1,100,100},
//		{9000,100,1700,135,100,1,100,100},
//		{7000,100,1700,135,100,1,100,100},
//		{6000,100,1700,140,100,1,100,100},
//		{6000,100,1700,145,100,1,100,100},
//		{5500,100,1700,150,100,1,100,100},
//		{5500,100,1700,160,100,1,100,100},
//		{5500,100,1700,160,100,1,100,100},
//	};
//
//	for (size_t i = 0; i < 12; i++)
//	{ //NPCKartAIPhysPara
//		BYTE* pNPCKartAIPhysPara = p;
//		Write16(pNPCKartAIPhysPara, 0); //len
//
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcN2OIntervalTime); //NpcN2OIntervalTime
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcTriggerN2ORate); //NpcTriggerN2ORate
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcRestCheckTime); //NpcRestCheckTime
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcFixVelocity); //NpcFixVelocity
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcFixVelocityRate); //NpcFixVelocityRate
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcRunDisHulanThreshold); //NpcRunDisHulanThreshold
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcSJetRate); //NpcSJetRate
//		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcTurnAjustDisHulanRate); //NpcTurnAjustDisHulanRate
//		Write8(pNPCKartAIPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
//		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
//		{ //ForwardAccel
//			BYTE* pForwardAccel = pNPCKartAIPhysPara;
//			Write16(pForwardAccel, 0); //len
//
//			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
//			Write32(pForwardAccel, ForwardAccel[i].Value); //Value
//
//			len = pForwardAccel - pNPCKartAIPhysPara;
//			Set16(pNPCKartAIPhysPara, (WORD)len);
//			pNPCKartAIPhysPara += len;
//		}
//		Write8(pNPCKartAIPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
//		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
//		{ //BackwardAccel
//			BYTE* pBackwardAccel = pNPCKartAIPhysPara;
//			Write16(pBackwardAccel, 0); //len
//
//			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
//			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value
//
//			len = pBackwardAccel - pNPCKartAIPhysPara;
//			Set16(pNPCKartAIPhysPara, (WORD)len);
//			pNPCKartAIPhysPara += len;
//		}
//		Write32(pNPCKartAIPhysPara, 25); //StraightLenThreshold
//		Write32(pNPCKartAIPhysPara, 80); //NpcDriftRate
//		Write8(pNPCKartAIPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
//		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
//		{ //CompensateParam
//			BYTE* pCompensateParam = pNPCKartAIPhysPara;
//			Write16(pCompensateParam, 0); //len
//
//			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
//			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value
//
//			len = pCompensateParam - pNPCKartAIPhysPara;
//			Set16(pNPCKartAIPhysPara, (WORD)len);
//			pNPCKartAIPhysPara += len;
//		}
//
//		len = pNPCKartAIPhysPara - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//	//2
//	{ //NPCKartPhysPara
//		BYTE* pNPCKartPhysPara = p;
//		Write16(pNPCKartPhysPara, 0); //len
//
//		Write32(pNPCKartPhysPara, 0); //KartID
//		Write8(pNPCKartPhysPara, sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0])); //PhysInfoCount
//		for (size_t i = 0; i < sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0]); i++)
//		{ //PhysInfo
//			BYTE* pPhysInfo = pNPCKartPhysPara;
//			Write16(pPhysInfo, 0); //len
//
//			Write32(pPhysInfo, NPCKartPhysPara[i].PID); //PID
//			Write32(pPhysInfo, NPCKartPhysPara[i].AffectID); //AffectID
//			Write32(pPhysInfo, NPCKartPhysPara[i].Param); //Param
//			Write32(pPhysInfo, NPCKartPhysPara[i].LifeTime); //LifeTime
//
//			len = pPhysInfo - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
//		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
//		{ //CompensateParam
//			BYTE* pCompensateParam = pNPCKartPhysPara;
//			Write16(pCompensateParam, 0); //len
//
//			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
//			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value
//
//			len = pCompensateParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		{ //CollisionBoxParam
//			BYTE* pCollisionBoxParam = pNPCKartPhysPara;
//			Write16(pCollisionBoxParam, 0); //len
//
//			Write32(pCollisionBoxParam, 300); //BoxLength
//			Write32(pCollisionBoxParam, 170); //BoxWidth
//			Write32(pCollisionBoxParam, 240); //BoxHeight
//			Write32(pCollisionBoxParam, 240); //AdjustOffset
//			Write32(pCollisionBoxParam, 200); //AdjustPickGap
//			Write32(pCollisionBoxParam, 190); //AdjustRemoteOffset
//			Write32(pCollisionBoxParam, 200); //AdjustBevelOffset
//			Write32(pCollisionBoxParam, -30); //ControllerOffset
//
//			len = pCollisionBoxParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		{ //CarToCarCollisionParam
//			BYTE* pCarToCarCollisionParam = pNPCKartPhysPara;
//			Write16(pCarToCarCollisionParam, 0); //len
//
//			Write32(pCarToCarCollisionParam, 300); //CollisionRadius
//			Write32(pCarToCarCollisionParam, 300); //CollisionLength
//			Write32(pCarToCarCollisionParam, 200); //CollisionWidth
//			Write32(pCarToCarCollisionParam, 250); //CollisionHeight
//			Write32(pCarToCarCollisionParam, 100000); //CollisionLostCtrlTime
//
//			len = pCarToCarCollisionParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write32(pNPCKartPhysPara, 1000); //BaseTurnRate
//		Write32(pNPCKartPhysPara, 1100); //VolatileTurnRate
//		Write32(pNPCKartPhysPara, 12000); //MaxTurnSpeed
//		Write32(pNPCKartPhysPara, 5700); //MinTurnSpeed
//		Write32(pNPCKartPhysPara, 700); //MaxAccuTime
//		Write32(pNPCKartPhysPara, 150); //BaseAccuRate
//		Write32(pNPCKartPhysPara, 19000); //MaxAffectSpeed
//		Write32(pNPCKartPhysPara, -15000); //Gravity
//		Write32(pNPCKartPhysPara, -2400); //AdditionalLocalZSpeed
//		Write32(pNPCKartPhysPara, 18000); //StartVec
//		Write32(pNPCKartPhysPara, 50000); //EndVecFist
//		Write32(pNPCKartPhysPara, 16000); //EndVecSecon
//		Write32(pNPCKartPhysPara, 1500); //DirKeyForce
//		Write32(pNPCKartPhysPara, 5900); //DirKeyTwist
//		Write32(pNPCKartPhysPara, 4200); //BannerTwist
//		Write32(pNPCKartPhysPara, 6000); //BannerKeyTwist
//		Write32(pNPCKartPhysPara, 6000); //BannerVecForce
//		Write32(pNPCKartPhysPara, 5500); //BannerHeadForce
//		Write32(pNPCKartPhysPara, 2600); //SlidFricForce
//		Write32(pNPCKartPhysPara, 1000); //RollFricForce
//		Write32(pNPCKartPhysPara, 800); //StartWec
//		Write32(pNPCKartPhysPara, 3500); //MaxWec
//		Write32(pNPCKartPhysPara, 5000); //SuaiJianTwist
//		Write32(pNPCKartPhysPara, 10000); //DirUpKeyForce
//		Write8(pNPCKartPhysPara, 6); //AccelStatusCount
//		for (size_t i = 0; i < 6; i++) {
//			Write32(pNPCKartPhysPara, NPCAccelStatusCount[i]); //AccelStatus[]
//		}
//		Write8(pNPCKartPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
//		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
//		{ //ForwardAccel
//			BYTE* pForwardAccel = pNPCKartPhysPara;
//			Write16(pForwardAccel, 0); //len
//
//			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
//			Write32(pForwardAccel, ForwardAccel[i].Value); //Value
//
//			len = pForwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(ForwardDecel) / sizeof(ForwardDecel[0])); //ForwardDecelNum
//		for (size_t i = 0; i < sizeof(ForwardDecel) / sizeof(ForwardDecel[0]); i++)
//		{ //ForwardDecel
//			BYTE* pForwardDecel = pNPCKartPhysPara;
//			Write16(pForwardDecel, 0); //len
//
//			Write32(pForwardDecel, ForwardDecel[i].Key); //Key
//			Write32(pForwardDecel, ForwardDecel[i].Value); //Value
//
//			len = pForwardDecel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
//		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
//		{ //BackwardAccel
//			BYTE* pBackwardAccel = pNPCKartPhysPara;
//			Write16(pBackwardAccel, 0); //len
//
//			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
//			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value
//
//			len = pBackwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(BackwardDecel) / sizeof(BackwardDecel[0])); //BackwardDecelNum
//		for (size_t i = 0; i < sizeof(BackwardDecel) / sizeof(BackwardDecel[0]); i++)
//		{ //BackwardDecel
//			BYTE* pBackwardDecel = pNPCKartPhysPara;
//			Write16(pBackwardDecel, 0); //len
//
//			Write32(pBackwardDecel, BackwardDecel[i].Key); //Key
//			Write32(pBackwardDecel, BackwardDecel[i].Value); //Value
//
//			len = pBackwardDecel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write32(pNPCKartPhysPara, 0); //CrashCompensatePower
//		Write32(pNPCKartPhysPara, 0); //DefenseRate
//		Write32(pNPCKartPhysPara, 0); //Uin
//		Write32(pNPCKartPhysPara, 0); //SpeedupCardGenRate
//		Write32(pNPCKartPhysPara, 0); //ExtraFuncFlag
//		Write8(pNPCKartPhysPara, 0); //SkatePara
//		/*for (size_t i = 0; i < 0; i++)
//		{ //hasSkatePara
//			BYTE* phasSkatePara = pNPCKartPhysPara;
//			Write16(phasSkatePara, 0); //len
//
//			Write32(phasSkatePara, 0); //TrackSpeed
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirBigJet
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_Jump
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirJetPoint
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_MaxForce
//			Write32(phasSkatePara, 0); //OntrackAccRef
//			Write32(phasSkatePara, 0); //TrackAccRef
//			Write32(phasSkatePara, 0); //DashSkill
//
//			len = phasSkatePara - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //HasBattleModeSkillInfoList
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BattleModeSkillInfoList
//			BYTE* pBattleModeSkillInfoList = pNPCKartPhysPara;
//			Write16(pBattleModeSkillInfoList, 0); //len
//
//			Write8(pBattleModeSkillInfoList, 0); //SkillNum
//			for (size_t i = 0; i < 0; i++)
//			{ //SkillInfo
//				BYTE* pSkillInfo = pBattleModeSkillInfoList;
//				Write16(pSkillInfo, 0); //len
//
//				Write16(pSkillInfo, 0); //SkillId
//				Write8(pSkillInfo, 0); //SkillLevel
//
//				len = pSkillInfo - pBattleModeSkillInfoList;
//				Set16(pBattleModeSkillInfoList, (WORD)len);
//				pBattleModeSkillInfoList += len;
//			}
//
//			len = pBattleModeSkillInfoList - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write32(pNPCKartPhysPara, 0); //EnergyConvert
//		Write16(pNPCKartPhysPara, 0); //SuperN2ORate
//		Write16(pNPCKartPhysPara, 0); //CollisionLoseRate
//		Write8(pNPCKartPhysPara, 0); //NotInterruptDrift
//		Write8(pNPCKartPhysPara, 0); //AddWeightNum
//		Write8(pNPCKartPhysPara, 0); //WeightNum
//		Write8(pNPCKartPhysPara, 0); //HasAntiCollisionCfg
//		/*for (size_t i = 0; i < 0; i++)
//		{ //AntiCollistionCfg
//			BYTE* pAntiCollistionCfg = pNPCKartPhysPara;
//			Write16(pAntiCollistionCfg, 0); //len
//
//			Write8(pAntiCollistionCfg, 0); //Num
//			Write32(pAntiCollistionCfg, 0); //Position[]
//
//			len = pAntiCollistionCfg - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //HasBoatParam
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BoatParam
//			BYTE* pBoatParam = pNPCKartPhysPara;
//			Write16(pBoatParam, 0); //len
//
//			Write32(pBoatParam, 0); //DirKeyForceParamC
//			Write32(pBoatParam, 0); //DirKeyForceParamD
//			Write32(pBoatParam, 0); //DTFLimitAngle
//			Write32(pBoatParam, 0); //DTFRatio
//			Write32(pBoatParam, 0); //UpKeyVecForce
//			Write32(pBoatParam, 0); //UpKeyVecForceParamA
//			Write32(pBoatParam, 0); //UpKeyVecForceParamB
//			Write32(pBoatParam, 0); //LRKeyVecForce
//			Write32(pBoatParam, 0); //LRKeyVecForceParamA
//			Write32(pBoatParam, 0); //LRKeyVecForceParamB
//			Write32(pBoatParam, 0); //VecForce
//			Write32(pBoatParam, 0); //VecForceParamA
//			Write32(pBoatParam, 0); //VecForceParamB
//			Write32(pBoatParam, 0); //BoatShape[]
//			Write32(pBoatParam, 0); //WecEffect
//			Write32(pBoatParam, 0); //BannerKeyTwist
//			Write32(pBoatParam, 0); //BannerKeyTwistParamA
//			Write32(pBoatParam, 0); //BannerKeyTwistParamB
//			Write32(pBoatParam, 0); //SuaiJianTwist
//			Write32(pBoatParam, 0); //BoatHeighRatio
//			Write32(pBoatParam, 0); //BannerTwist
//			Write32(pBoatParam, 0); //BannerTwistParamA
//			Write32(pBoatParam, 0); //DirUpKeyForce
//			Write32(pBoatParam, 0); //DirUpKeyForceParamA
//			Write32(pBoatParam, 0); //DirUpKeyForceParamB
//			Write32(pBoatParam, 0); //DirKeyForce
//			Write32(pBoatParam, 0); //DirKeyForceParamA
//			Write32(pBoatParam, 0); //DirKeyForceParamB
//			Write32(pBoatParam, 0); //AddNO2PerSec
//			Write32(pBoatParam, 0); //BoatAirValue
//			WriteUn(pBoatParam, 0); //ifSlidFricForce
//
//			len = pBoatParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //BoatForwardAccelNum
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BoatForwardAccel
//			BYTE* pBoatForwardAccel = pNPCKartPhysPara;
//			Write16(pBoatForwardAccel, 0); //len
//
//			Write32(pBoatForwardAccel, 0); //Key
//			Write32(pBoatForwardAccel, 0); //Value
//
//			len = pBoatForwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamA
//		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamB
//
//		len = pNPCKartPhysPara - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//	Write8(p, 12); // LevelNum
//
//	for (size_t i = 0; i < 12; i++)
//	{ // NPCKartAIPhysPara_NpcPlayer
//		BYTE* pNPCKartAIPhysPara_NpcPlayer = p;
//		Write16(pNPCKartAIPhysPara_NpcPlayer, 0); // len
//
//		{ // Param
//			BYTE* pParam = pNPCKartAIPhysPara_NpcPlayer;
//			Write16(pParam, 0); // len
//
//			Write32(pParam, 0); // NpcN2OIntervalTime
//			Write32(pParam, 0); // NpcTriggerN2ORate
//			Write32(pParam, 0); // NpcRestCheckTime
//			Write32(pParam, 0); // NpcFixVelocity
//			Write32(pParam, 0); // NpcFixVelocityRate
//			Write32(pParam, 0); // NpcRunDisHulanThreshold
//			Write32(pParam, 0); // NpcSJetRate
//			Write32(pParam, 0); // NpcTurnAjustDisHulanRate
//
//			Write8(pParam, 0); // ForwardAccelNum
//			/*
//			for (size_t i = 0; i < 0; i++)
//			{ //ForwardAccel
//				BYTE* pForwardAccel = pParam;
//				Write16(pForwardAccel, 0); //len
//
//				Write32(pForwardAccel, 0); //Key
//				Write32(pForwardAccel, 0); //Value
//
//				len = pForwardAccel - pParam;
//				Set16(pParam, (WORD)len);
//				pParam += len;
//			}
//			*/
//
//			Write8(pParam, 0); // BackwardAccelNum
//			/*
//			for (size_t i = 0; i < 1; i++)
//			{ //BackwardAccel
//				BYTE* pBackwardAccel = pParam;
//				Write16(pBackwardAccel, 0); //len
//
//				Write32(pBackwardAccel, 0); //Key
//				Write32(pBackwardAccel, 0); //Value
//
//				len = pBackwardAccel - pParam;
//				Set16(pParam, (WORD)len);
//				pParam += len;
//			}
//			*/
//			Write32(pParam, 0); // StraightLenThreshold
//			Write32(pParam, 0); // NpcDriftRate
//
//			Write8(pParam, 0); // CompensateParamCount
//			/*
//			for (size_t i = 0; i < 1; i++)
//			{ //CompensateParam
//				BYTE* pCompensateParam = pParam;
//				Write16(pCompensateParam, 0); //len
//
//				Write32(pCompensateParam, 0); //Percent
//				Write32(pCompensateParam, 0); //Value
//
//				len = pCompensateParam - pParam;
//				Set16(pParam, (WORD)len);
//				pParam += len;
//			}
//			*/
//
//			len = pParam - pNPCKartAIPhysPara_NpcPlayer;
//			Set16(pNPCKartAIPhysPara_NpcPlayer, (WORD)len);
//			pNPCKartAIPhysPara_NpcPlayer += len;
//		}
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // N2OStartTime
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // NpcN2OMaxCount
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // Dis2FirstPlay
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlTime
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostInterval
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlCount
//
//		len = pNPCKartAIPhysPara_NpcPlayer - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//	{ //NPCKartPhysPara
//		BYTE* pNPCKartPhysPara = p;
//		Write16(pNPCKartPhysPara, 0); //len
//
//		Write32(pNPCKartPhysPara, 0); //KartID
//		Write8(pNPCKartPhysPara, sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0])); //PhysInfoCount
//		for (size_t i = 0; i < sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0]); i++)
//		{ //PhysInfo
//			BYTE* pPhysInfo = pNPCKartPhysPara;
//			Write16(pPhysInfo, 0); //len
//
//			Write32(pPhysInfo, NPCKartPhysPara[i].PID); //PID
//			Write32(pPhysInfo, NPCKartPhysPara[i].AffectID); //AffectID
//			Write32(pPhysInfo, NPCKartPhysPara[i].Param); //Param
//			Write32(pPhysInfo, NPCKartPhysPara[i].LifeTime); //LifeTime
//
//			len = pPhysInfo - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
//		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
//		{ //CompensateParam
//			BYTE* pCompensateParam = pNPCKartPhysPara;
//			Write16(pCompensateParam, 0); //len
//
//			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
//			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value
//
//			len = pCompensateParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		{ //CollisionBoxParam
//			BYTE* pCollisionBoxParam = pNPCKartPhysPara;
//			Write16(pCollisionBoxParam, 0); //len
//
//			Write32(pCollisionBoxParam, 300); //BoxLength
//			Write32(pCollisionBoxParam, 170); //BoxWidth
//			Write32(pCollisionBoxParam, 240); //BoxHeight
//			Write32(pCollisionBoxParam, 240); //AdjustOffset
//			Write32(pCollisionBoxParam, 200); //AdjustPickGap
//			Write32(pCollisionBoxParam, 190); //AdjustRemoteOffset
//			Write32(pCollisionBoxParam, 200); //AdjustBevelOffset
//			Write32(pCollisionBoxParam, -30); //ControllerOffset
//
//			len = pCollisionBoxParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		{ //CarToCarCollisionParam
//			BYTE* pCarToCarCollisionParam = pNPCKartPhysPara;
//			Write16(pCarToCarCollisionParam, 0); //len
//
//			Write32(pCarToCarCollisionParam, 300); //CollisionRadius
//			Write32(pCarToCarCollisionParam, 300); //CollisionLength
//			Write32(pCarToCarCollisionParam, 200); //CollisionWidth
//			Write32(pCarToCarCollisionParam, 250); //CollisionHeight
//			Write32(pCarToCarCollisionParam, 100000); //CollisionLostCtrlTime
//
//			len = pCarToCarCollisionParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write32(pNPCKartPhysPara, 1000); //BaseTurnRate
//		Write32(pNPCKartPhysPara, 1100); //VolatileTurnRate
//		Write32(pNPCKartPhysPara, 12000); //MaxTurnSpeed
//		Write32(pNPCKartPhysPara, 5700); //MinTurnSpeed
//		Write32(pNPCKartPhysPara, 700); //MaxAccuTime
//		Write32(pNPCKartPhysPara, 150); //BaseAccuRate
//		Write32(pNPCKartPhysPara, 19000); //MaxAffectSpeed
//		Write32(pNPCKartPhysPara, -15000); //Gravity
//		Write32(pNPCKartPhysPara, -2400); //AdditionalLocalZSpeed
//		Write32(pNPCKartPhysPara, 18000); //StartVec
//		Write32(pNPCKartPhysPara, 50000); //EndVecFist
//		Write32(pNPCKartPhysPara, 16000); //EndVecSecon
//		Write32(pNPCKartPhysPara, 1500); //DirKeyForce
//		Write32(pNPCKartPhysPara, 5900); //DirKeyTwist
//		Write32(pNPCKartPhysPara, 4200); //BannerTwist
//		Write32(pNPCKartPhysPara, 6000); //BannerKeyTwist
//		Write32(pNPCKartPhysPara, 6000); //BannerVecForce
//		Write32(pNPCKartPhysPara, 5500); //BannerHeadForce
//		Write32(pNPCKartPhysPara, 2600); //SlidFricForce
//		Write32(pNPCKartPhysPara, 1000); //RollFricForce
//		Write32(pNPCKartPhysPara, 800); //StartWec
//		Write32(pNPCKartPhysPara, 3500); //MaxWec
//		Write32(pNPCKartPhysPara, 5000); //SuaiJianTwist
//		Write32(pNPCKartPhysPara, 10000); //DirUpKeyForce
//		Write8(pNPCKartPhysPara, 6); //AccelStatusCount
//		for (size_t i = 0; i < 6; i++) {
//			Write32(pNPCKartPhysPara, NPCAccelStatusCount[i]); //AccelStatus[]
//		}
//		Write8(pNPCKartPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
//		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
//		{ //ForwardAccel
//			BYTE* pForwardAccel = pNPCKartPhysPara;
//			Write16(pForwardAccel, 0); //len
//
//			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
//			Write32(pForwardAccel, ForwardAccel[i].Value); //Value
//
//			len = pForwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(ForwardDecel) / sizeof(ForwardDecel[0])); //ForwardDecelNum
//		for (size_t i = 0; i < sizeof(ForwardDecel) / sizeof(ForwardDecel[0]); i++)
//		{ //ForwardDecel
//			BYTE* pForwardDecel = pNPCKartPhysPara;
//			Write16(pForwardDecel, 0); //len
//
//			Write32(pForwardDecel, ForwardDecel[i].Key); //Key
//			Write32(pForwardDecel, ForwardDecel[i].Value); //Value
//
//			len = pForwardDecel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
//		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
//		{ //BackwardAccel
//			BYTE* pBackwardAccel = pNPCKartPhysPara;
//			Write16(pBackwardAccel, 0); //len
//
//			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
//			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value
//
//			len = pBackwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write8(pNPCKartPhysPara, sizeof(BackwardDecel) / sizeof(BackwardDecel[0])); //BackwardDecelNum
//		for (size_t i = 0; i < sizeof(BackwardDecel) / sizeof(BackwardDecel[0]); i++)
//		{ //BackwardDecel
//			BYTE* pBackwardDecel = pNPCKartPhysPara;
//			Write16(pBackwardDecel, 0); //len
//
//			Write32(pBackwardDecel, BackwardDecel[i].Key); //Key
//			Write32(pBackwardDecel, BackwardDecel[i].Value); //Value
//
//			len = pBackwardDecel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}
//		Write32(pNPCKartPhysPara, 0); //CrashCompensatePower
//		Write32(pNPCKartPhysPara, 0); //DefenseRate
//		Write32(pNPCKartPhysPara, 0); //Uin
//		Write32(pNPCKartPhysPara, 0); //SpeedupCardGenRate
//		Write32(pNPCKartPhysPara, 0); //ExtraFuncFlag
//		Write8(pNPCKartPhysPara, 0); //SkatePara
//		/*for (size_t i = 0; i < 0; i++)
//		{ //hasSkatePara
//			BYTE* phasSkatePara = pNPCKartPhysPara;
//			Write16(phasSkatePara, 0); //len
//
//			Write32(phasSkatePara, 0); //TrackSpeed
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirBigJet
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_Jump
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirJetPoint
//			Write32(phasSkatePara, 0); //SkateSpeedCoef_MaxForce
//			Write32(phasSkatePara, 0); //OntrackAccRef
//			Write32(phasSkatePara, 0); //TrackAccRef
//			Write32(phasSkatePara, 0); //DashSkill
//
//			len = phasSkatePara - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //HasBattleModeSkillInfoList
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BattleModeSkillInfoList
//			BYTE* pBattleModeSkillInfoList = pNPCKartPhysPara;
//			Write16(pBattleModeSkillInfoList, 0); //len
//
//			Write8(pBattleModeSkillInfoList, 0); //SkillNum
//			for (size_t i = 0; i < 0; i++)
//			{ //SkillInfo
//				BYTE* pSkillInfo = pBattleModeSkillInfoList;
//				Write16(pSkillInfo, 0); //len
//
//				Write16(pSkillInfo, 0); //SkillId
//				Write8(pSkillInfo, 0); //SkillLevel
//
//				len = pSkillInfo - pBattleModeSkillInfoList;
//				Set16(pBattleModeSkillInfoList, (WORD)len);
//				pBattleModeSkillInfoList += len;
//			}
//
//			len = pBattleModeSkillInfoList - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write32(pNPCKartPhysPara, 0); //EnergyConvert
//		Write16(pNPCKartPhysPara, 0); //SuperN2ORate
//		Write16(pNPCKartPhysPara, 0); //CollisionLoseRate
//		Write8(pNPCKartPhysPara, 0); //NotInterruptDrift
//		Write8(pNPCKartPhysPara, 0); //AddWeightNum
//		Write8(pNPCKartPhysPara, 0); //WeightNum
//		Write8(pNPCKartPhysPara, 0); //HasAntiCollisionCfg
//		/*for (size_t i = 0; i < 0; i++)
//		{ //AntiCollistionCfg
//			BYTE* pAntiCollistionCfg = pNPCKartPhysPara;
//			Write16(pAntiCollistionCfg, 0); //len
//
//			Write8(pAntiCollistionCfg, 0); //Num
//			Write32(pAntiCollistionCfg, 0); //Position[]
//
//			len = pAntiCollistionCfg - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //HasBoatParam
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BoatParam
//			BYTE* pBoatParam = pNPCKartPhysPara;
//			Write16(pBoatParam, 0); //len
//
//			Write32(pBoatParam, 0); //DirKeyForceParamC
//			Write32(pBoatParam, 0); //DirKeyForceParamD
//			Write32(pBoatParam, 0); //DTFLimitAngle
//			Write32(pBoatParam, 0); //DTFRatio
//			Write32(pBoatParam, 0); //UpKeyVecForce
//			Write32(pBoatParam, 0); //UpKeyVecForceParamA
//			Write32(pBoatParam, 0); //UpKeyVecForceParamB
//			Write32(pBoatParam, 0); //LRKeyVecForce
//			Write32(pBoatParam, 0); //LRKeyVecForceParamA
//			Write32(pBoatParam, 0); //LRKeyVecForceParamB
//			Write32(pBoatParam, 0); //VecForce
//			Write32(pBoatParam, 0); //VecForceParamA
//			Write32(pBoatParam, 0); //VecForceParamB
//			Write32(pBoatParam, 0); //BoatShape[]
//			Write32(pBoatParam, 0); //WecEffect
//			Write32(pBoatParam, 0); //BannerKeyTwist
//			Write32(pBoatParam, 0); //BannerKeyTwistParamA
//			Write32(pBoatParam, 0); //BannerKeyTwistParamB
//			Write32(pBoatParam, 0); //SuaiJianTwist
//			Write32(pBoatParam, 0); //BoatHeighRatio
//			Write32(pBoatParam, 0); //BannerTwist
//			Write32(pBoatParam, 0); //BannerTwistParamA
//			Write32(pBoatParam, 0); //DirUpKeyForce
//			Write32(pBoatParam, 0); //DirUpKeyForceParamA
//			Write32(pBoatParam, 0); //DirUpKeyForceParamB
//			Write32(pBoatParam, 0); //DirKeyForce
//			Write32(pBoatParam, 0); //DirKeyForceParamA
//			Write32(pBoatParam, 0); //DirKeyForceParamB
//			Write32(pBoatParam, 0); //AddNO2PerSec
//			Write32(pBoatParam, 0); //BoatAirValue
//			WriteUn(pBoatParam, 0); //ifSlidFricForce
//
//			len = pBoatParam - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write8(pNPCKartPhysPara, 0); //BoatForwardAccelNum
//		/*for (size_t i = 0; i < 0; i++)
//		{ //BoatForwardAccel
//			BYTE* pBoatForwardAccel = pNPCKartPhysPara;
//			Write16(pBoatForwardAccel, 0); //len
//
//			Write32(pBoatForwardAccel, 0); //Key
//			Write32(pBoatForwardAccel, 0); //Value
//
//			len = pBoatForwardAccel - pNPCKartPhysPara;
//			Set16(pNPCKartPhysPara, (WORD)len);
//			pNPCKartPhysPara += len;
//		}*/
//		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamA
//		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamB
//
//		len = pNPCKartPhysPara - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//	Write8(p, 10); // NPCLevelNum_RankedMatch
//	for (size_t i = 0; i < 10; i++)
//	{ // NPCKartAIPhysPara_NpcPlayer
//		BYTE* pNPCKartAIPhysPara_NpcPlayer = p;
//		Write16(pNPCKartAIPhysPara_NpcPlayer, 0); // len
//
//		{ // Param
//			BYTE* pParam = pNPCKartAIPhysPara_NpcPlayer;
//			Write16(pParam, 0); // len
//
//			Write32(pParam, 0); // NpcN2OIntervalTime
//			Write32(pParam, 0); // NpcTriggerN2ORate
//			Write32(pParam, 0); // NpcRestCheckTime
//			Write32(pParam, 0); // NpcFixVelocity
//			Write32(pParam, 0); // NpcFixVelocityRate
//			Write32(pParam, 0); // NpcRunDisHulanThreshold
//			Write32(pParam, 0); // NpcSJetRate
//			Write32(pParam, 0); // NpcTurnAjustDisHulanRate
//
//			Write8(pParam, 0); // ForwardAccelNum
//			/*
//			for (size_t i = 0; i < 0; i++)
//			{ //ForwardAccel
//				BYTE* pForwardAccel = pParam;
//				Write16(pForwardAccel, 0); //len
//
//				Write32(pForwardAccel, 0); //Key
//				Write32(pForwardAccel, 0); //Value
//
//				len = pForwardAccel - pParam;
//				Set16(pParam, (WORD)len);
//				pParam += len;
//			}
//			*/
//
//			Write8(pParam, 0); // BackwardAccelNum
//			/*
//			for (size_t i = 0; i < 1; i++)
//			{ //BackwardAccel
//				BYTE* pBackwardAccel = pParam;
//				Write16(pBackwardAccel, 0); //len
//
//				Write32(pBackwardAccel, 0); //Key
//				Write32(pBackwardAccel, 0); //Value
//
//				len = pBackwardAccel - pParam;
//				Set16(pParam, (WORD)len);
//				pParam += len;
//			}
//			*/
//			Write32(pParam, 0); // StraightLenThreshold
//			Write32(pParam, 0); // NpcDriftRate
//
//			Write8(pParam, 0); // CompensateParamCount
//			/*
//			for (size_t i = 0; i < 1; i++)
//			{ //CompensateParam
//				BYTE* pCompensateParam = pParam;
//				Write16(pCompensateParam, 0); //len
//
//				Write32(pCompensateParam, 0); //Percent
//				Write32(pCompensateParam, 0); //Value
//
//				len = pCompensateParam - pParam;
//				Set16(pParam, (WORD)len);
//				pParam += len;
//			}
//			*/
//
//			len = pParam - pNPCKartAIPhysPara_NpcPlayer;
//			Set16(pNPCKartAIPhysPara_NpcPlayer, (WORD)len);
//			pNPCKartAIPhysPara_NpcPlayer += len;
//		}
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // N2OStartTime
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // NpcN2OMaxCount
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // Dis2FirstPlay
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlTime
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostInterval
//		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlCount
//
//		len = pNPCKartAIPhysPara_NpcPlayer - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//
//	Write32(p, 0x0033BC27); // SwitchFlag1
//	Write16(p, 60);			// MaxQuickLerpThrehold
//	Write16(p, 10);			// QuickLerpStepCnts
//	Write16(p, 12);			// LerpSynccpFrequence
//
//	Write16(p, 3); // ClientItemOpenTypeNum
//	{			   // ClientItemOpenCfg
//		BYTE* pClientItemOpenCfg = p;
//		Write16(pClientItemOpenCfg, 0); // len
//
//		Write16(pClientItemOpenCfg, 5); // OpenType
//		Write8(pClientItemOpenCfg, 5);	// ItemNum
//		Write32(pClientItemOpenCfg, 0); // ItemList[]
//
//		len = pClientItemOpenCfg - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//	{ // ClientItemOpenCfg
//		BYTE* pClientItemOpenCfg = p;
//		Write16(pClientItemOpenCfg, 0); // len
//
//		Write16(pClientItemOpenCfg, 10); // OpenType
//		Write8(pClientItemOpenCfg, 10);	 // ItemNum
//		Write32(pClientItemOpenCfg, 0);	 // ItemList[]
//
//		len = pClientItemOpenCfg - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//	{ // ClientItemOpenCfg
//		BYTE* pClientItemOpenCfg = p;
//		Write16(pClientItemOpenCfg, 0); // len
//
//		Write16(pClientItemOpenCfg, 50); // OpenType
//		Write8(pClientItemOpenCfg, 50);	 // ItemNum
//		Write32(pClientItemOpenCfg, 0);	 // ItemList[]
//
//		len = pClientItemOpenCfg - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//
//
//	Write8(p, 0); // OtherNum
//	for (size_t i = 0; i < 0; i++)
//	{ // CollisionPowerOtherInfo
//		BYTE* pCollisionPowerOtherInfo = p;
//		Write16(pCollisionPowerOtherInfo, 0); // len
//
//		Write32(pCollisionPowerOtherInfo, 0);	// Power
//		Write32(pCollisionPowerOtherInfo, 100); // Ratio
//		Write32(pCollisionPowerOtherInfo, 0);	// Add
//
//		len = pCollisionPowerOtherInfo - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//	Write32(p, 400);  // CollisionMinPower
//	Write32(p, 2900); // ItemExtendLimit
//	Write32(p, 50);	  // ChattingTalkLevelLimit
//	Write32(p, 0);	  // ChattingSpecialOp
//
//	Write8(p, 0); // ForbidMapNum
//	// Write32(p, 0); //ForbidMap[]
//
//	Write32(p, 20000); // MaxBuySpeedNum
//
//	Write8(p, 0); // ClientItemIconNum
//
//	//for (size_t i = 0; i < n; i++)
//	//{ //ClientIconCfg
//	//	BYTE* pClientIconCfg = p;
//	//	Write16(pClientIconCfg, 0); //len
//
//	//	Write32(pClientIconCfg, 0); //ItemID
//	//	Write8(pClientIconCfg, 0); //IconPathLen
//
//	//	len = pClientIconCfg - p;
//	//	Set16(p, (WORD)len);
//	//	p += len;
//	//}
//
//
//	Write8(p, 0); // TDCBlackCarNum
//	// Write32(p, 0); //TDCBlackCar[]
//
//	Write8(p, 0); // TDCBlackCarTypeNum
//	// Write32(p, 0); //TDCBlackCarType[]
//
//	Write8(p, 0); // TDCBlackCarType2ndNum
//	// Write32(p, 0); //TDCBlackCarType2nd[]
//
//	{ // ClientFrameRateCfg
//		BYTE* pClientFrameRateCfg = p;
//		Write16(pClientFrameRateCfg, 0); // len
//		//Write32(pClientFrameRateCfg, 30000); //MinFrameRate
//		//Write32(pClientFrameRateCfg, 40000); //MaxFrameRate
//		//Write32(pClientFrameRateCfg, 10); //FrameStep
//		//Write32(pClientFrameRateCfg, 0); //CurrentFrameRate
//		if (getGameConfig()["LockFps"] && getGameConfig()["LockFps"].as<UINT>() == 1) {
//			Write32(pClientFrameRateCfg, 30000); //MinFrameRate
//			Write32(pClientFrameRateCfg, 40000); //MaxFrameRate
//			Write32(pClientFrameRateCfg, 10); //FrameStep
//			Write32(pClientFrameRateCfg, 40000); //CurrentFrameRate
//		}
//		else {
//			Write32(pClientFrameRateCfg, 0);	 // MinFrameRate
//			Write32(pClientFrameRateCfg, 60000); // MaxFrameRate
//			Write32(pClientFrameRateCfg, 10);	 // FrameStep
//			Write32(pClientFrameRateCfg, 0);	 // CurrentFrameRate
//		}
//		len = pClientFrameRateCfg - p;
//		Set16(p, (WORD)len);
//		p += len;
//	}
//
//	Write8(p, 16);	// ReportFrameRateValue
//	Write16(p, 10); // FrameOpSpan
//	Write8(p, 0);	// VideoSwitch
//
//	Write8(p, 0); // KartRefitCardTypeNum
//	Write32(p, 0);
//	Write32(p, 0);
//	Write32(p, 0);
//	Write8(p, 0);
//
//
//
//	len = p - buf;
//	SendToClient(Client, 25066, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
//}

void NotifySvrConfig3(ClientNode* Client)
{
	BYTE buf[28192] = { 0 };
	BYTE* p = buf;
	size_t len = 0;
	typedef struct CPhysInfo {

		UINT PID;
		UINT AffectID;
		UINT Param;
		UINT LifeTime;

	};
	typedef struct COMPENSATEPARAMCOUNT {

		UINT Percent;
		UINT Value;

	};
	typedef struct NPCFORWRDACCEL {

		UINT Key;
		UINT Value;
	};
	typedef struct NPCFORWRDDECEL {

		UINT Key;
		UINT Value;
	};


	typedef struct NPCBACKWARDACCEL {

		UINT Key;
		UINT Value;
	};
	typedef struct NPCBACKWARDDECEL {

		UINT Key;
		UINT Value;
	};
	const static CPhysInfo NPCKartPhysPara[] = {
		{1,100,6290,3370},
		{2001,100,6330,655},
		{2002,100,10600,4300},
		{99999,100,90300,5000},
		{777,100,6800,3300},
		{34,100,6800,3300},

	};

	const static COMPENSATEPARAMCOUNT CompensateParamCount[] = {
		{2,1010},
		{10,1010},
		{20,1020},
		{60,1080},
		{120,1160},
		{240,1200},
		{900,1200},
	};
	const static UINT NPCAccelStatusCount[]{

	1200,1200,800,300,2000,1900,

	};
	const static NPCFORWRDACCEL ForwardAccel[]{
			{0,4600},
			{5000,4700},
			{5700,5300},
			{9000,5800},
			{9300,6100},
			{11000,6600},
			{11800,6900},
			{15000,7800},
			{16000,7850},
			{20000,7800},
			{25000,7750},
			{30000,7750},
			{35000,7800},

			/*{0,30000},
			{5000,41000},
			{5700,42000},
			{9000,43000},
			{9300,44000},
			{11000,45800},
			{11000,46800},
			{15000,47800},
			{16000,48800},
			{20000,49800},
			{25000,50800},
			{30000,51800},
			{35000,52800},*/
	};
	const static NPCFORWRDDECEL ForwardDecel[]{

		 {0,-8000},
		{2000,-7000},
		{4000,-6000},
		{6000,-5000},
		{18000,-4000},

	};
	const static NPCBACKWARDACCEL BackwardAccel[]{

		 {0,-5000},
		{3000,-5000},
		{6000,-5000},
		{10000,-5000},
		{12000,-5500},

	};
	const static NPCBACKWARDDECEL BackwardDecel[]{

		 {0,16000},
		{2000,16000},
		{4000,16000},
		{6000,15000},
		{10000,15000},

	};
	{ //NPCKartPhysPara
		BYTE* pNPCKartPhysPara = p;
		Write16(pNPCKartPhysPara, 0); //len

		Write32(pNPCKartPhysPara, 0); //KartID
		Write8(pNPCKartPhysPara, sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0])); //PhysInfoCount
		for (size_t i = 0; i < sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0]); i++)
		{ //PhysInfo
			BYTE* pPhysInfo = pNPCKartPhysPara;
			Write16(pPhysInfo, 0); //len

			Write32(pPhysInfo, NPCKartPhysPara[i].PID); //PID
			Write32(pPhysInfo, NPCKartPhysPara[i].AffectID); //AffectID
			Write32(pPhysInfo, NPCKartPhysPara[i].Param); //Param
			Write32(pPhysInfo, NPCKartPhysPara[i].LifeTime); //LifeTime

			len = pPhysInfo - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
		{ //CompensateParam
			BYTE* pCompensateParam = pNPCKartPhysPara;
			Write16(pCompensateParam, 0); //len

			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value

			len = pCompensateParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		{ //CollisionBoxParam
			BYTE* pCollisionBoxParam = pNPCKartPhysPara;
			Write16(pCollisionBoxParam, 0); //len

			Write32(pCollisionBoxParam, 300); //BoxLength
			Write32(pCollisionBoxParam, 170); //BoxWidth
			Write32(pCollisionBoxParam, 240); //BoxHeight
			Write32(pCollisionBoxParam, 240); //AdjustOffset
			Write32(pCollisionBoxParam, 200); //AdjustPickGap
			Write32(pCollisionBoxParam, 190); //AdjustRemoteOffset
			Write32(pCollisionBoxParam, 200); //AdjustBevelOffset
			Write32(pCollisionBoxParam, -30); //ControllerOffset

			len = pCollisionBoxParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		{ //CarToCarCollisionParam
			BYTE* pCarToCarCollisionParam = pNPCKartPhysPara;
			Write16(pCarToCarCollisionParam, 0); //len

			Write32(pCarToCarCollisionParam, 300); //CollisionRadius
			Write32(pCarToCarCollisionParam, 300); //CollisionLength
			Write32(pCarToCarCollisionParam, 200); //CollisionWidth
			Write32(pCarToCarCollisionParam, 250); //CollisionHeight
			Write32(pCarToCarCollisionParam, 100000); //CollisionLostCtrlTime

			len = pCarToCarCollisionParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write32(pNPCKartPhysPara, 1130); //BaseTurnRate
		Write32(pNPCKartPhysPara, 1150); //VolatileTurnRate
		Write32(pNPCKartPhysPara, 12200); //MaxTurnSpeed
		Write32(pNPCKartPhysPara, 5700); //MinTurnSpeed
		Write32(pNPCKartPhysPara, 500); //MaxAccuTime
		Write32(pNPCKartPhysPara, 150); //BaseAccuRate
		Write32(pNPCKartPhysPara, 19000); //MaxAffectSpeed
		Write32(pNPCKartPhysPara, -15000); //Gravity
		Write32(pNPCKartPhysPara, -2740); //AdditionalLocalZSpeed
		Write32(pNPCKartPhysPara, 20000); //StartVec
		Write32(pNPCKartPhysPara, 50000); //EndVecFist
		Write32(pNPCKartPhysPara, 18000); //EndVecSecon
		Write32(pNPCKartPhysPara, 1500); //DirKeyForce
		Write32(pNPCKartPhysPara, 5900); //DirKeyTwist
		Write32(pNPCKartPhysPara, 4400); //BannerTwist
		Write32(pNPCKartPhysPara, 6200); //BannerKeyTwist
		Write32(pNPCKartPhysPara, 6200); //BannerVecForce
		Write32(pNPCKartPhysPara, 5500); //BannerHeadForce
		Write32(pNPCKartPhysPara, 2650); //SlidFricForce
		Write32(pNPCKartPhysPara, 1000); //RollFricForce
		Write32(pNPCKartPhysPara, 800); //StartWec
		Write32(pNPCKartPhysPara, 3500); //MaxWec
		Write32(pNPCKartPhysPara, 5000); //SuaiJianTwist
		Write32(pNPCKartPhysPara, 10240); //DirUpKeyForce
		Write8(pNPCKartPhysPara, 6); //AccelStatusCount
		for (size_t i = 0; i < 6; i++) {
			Write32(pNPCKartPhysPara, NPCAccelStatusCount[i]); //AccelStatus[]
		}
		Write8(pNPCKartPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
		{ //ForwardAccel
			BYTE* pForwardAccel = pNPCKartPhysPara;
			Write16(pForwardAccel, 0); //len

			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
			Write32(pForwardAccel, ForwardAccel[i].Value); //Value

			len = pForwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(ForwardDecel) / sizeof(ForwardDecel[0])); //ForwardDecelNum
		for (size_t i = 0; i < sizeof(ForwardDecel) / sizeof(ForwardDecel[0]); i++)
		{ //ForwardDecel
			BYTE* pForwardDecel = pNPCKartPhysPara;
			Write16(pForwardDecel, 0); //len

			Write32(pForwardDecel, ForwardDecel[i].Key); //Key
			Write32(pForwardDecel, ForwardDecel[i].Value); //Value

			len = pForwardDecel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
		{ //BackwardAccel
			BYTE* pBackwardAccel = pNPCKartPhysPara;
			Write16(pBackwardAccel, 0); //len

			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value

			len = pBackwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(BackwardDecel) / sizeof(BackwardDecel[0])); //BackwardDecelNum
		for (size_t i = 0; i < sizeof(BackwardDecel) / sizeof(BackwardDecel[0]); i++)
		{ //BackwardDecel
			BYTE* pBackwardDecel = pNPCKartPhysPara;
			Write16(pBackwardDecel, 0); //len

			Write32(pBackwardDecel, BackwardDecel[i].Key); //Key
			Write32(pBackwardDecel, BackwardDecel[i].Value); //Value

			len = pBackwardDecel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write32(pNPCKartPhysPara, 0); //CrashCompensatePower
		Write32(pNPCKartPhysPara, 0); //DefenseRate
		Write32(pNPCKartPhysPara, 0); //Uin
		Write32(pNPCKartPhysPara, 0); //SpeedupCardGenRate
		Write32(pNPCKartPhysPara, 0); //ExtraFuncFlag
		Write8(pNPCKartPhysPara, 0); //SkatePara
		/*for (size_t i = 0; i < 0; i++)
		{ //hasSkatePara
			BYTE* phasSkatePara = pNPCKartPhysPara;
			Write16(phasSkatePara, 0); //len

			Write32(phasSkatePara, 0); //TrackSpeed
			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirBigJet
			Write32(phasSkatePara, 0); //SkateSpeedCoef_Jump
			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirJetPoint
			Write32(phasSkatePara, 0); //SkateSpeedCoef_MaxForce
			Write32(phasSkatePara, 0); //OntrackAccRef
			Write32(phasSkatePara, 0); //TrackAccRef
			Write32(phasSkatePara, 0); //DashSkill

			len = phasSkatePara - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //HasBattleModeSkillInfoList
		/*for (size_t i = 0; i < 0; i++)
		{ //BattleModeSkillInfoList
			BYTE* pBattleModeSkillInfoList = pNPCKartPhysPara;
			Write16(pBattleModeSkillInfoList, 0); //len

			Write8(pBattleModeSkillInfoList, 0); //SkillNum
			for (size_t i = 0; i < 0; i++)
			{ //SkillInfo
				BYTE* pSkillInfo = pBattleModeSkillInfoList;
				Write16(pSkillInfo, 0); //len

				Write16(pSkillInfo, 0); //SkillId
				Write8(pSkillInfo, 0); //SkillLevel

				len = pSkillInfo - pBattleModeSkillInfoList;
				Set16(pBattleModeSkillInfoList, (WORD)len);
				pBattleModeSkillInfoList += len;
			}

			len = pBattleModeSkillInfoList - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write32(pNPCKartPhysPara, 0); //EnergyConvert
		Write16(pNPCKartPhysPara, 0); //SuperN2ORate
		Write16(pNPCKartPhysPara, 0); //CollisionLoseRate
		Write8(pNPCKartPhysPara, 0); //NotInterruptDrift
		Write8(pNPCKartPhysPara, 0); //AddWeightNum
		Write8(pNPCKartPhysPara, 0); //WeightNum
		Write8(pNPCKartPhysPara, 0); //HasAntiCollisionCfg
		/*for (size_t i = 0; i < 0; i++)
		{ //AntiCollistionCfg
			BYTE* pAntiCollistionCfg = pNPCKartPhysPara;
			Write16(pAntiCollistionCfg, 0); //len

			Write8(pAntiCollistionCfg, 0); //Num
			Write32(pAntiCollistionCfg, 0); //Position[]

			len = pAntiCollistionCfg - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //HasBoatParam
		/*for (size_t i = 0; i < 0; i++)
		{ //BoatParam
			BYTE* pBoatParam = pNPCKartPhysPara;
			Write16(pBoatParam, 0); //len

			Write32(pBoatParam, 0); //DirKeyForceParamC
			Write32(pBoatParam, 0); //DirKeyForceParamD
			Write32(pBoatParam, 0); //DTFLimitAngle
			Write32(pBoatParam, 0); //DTFRatio
			Write32(pBoatParam, 0); //UpKeyVecForce
			Write32(pBoatParam, 0); //UpKeyVecForceParamA
			Write32(pBoatParam, 0); //UpKeyVecForceParamB
			Write32(pBoatParam, 0); //LRKeyVecForce
			Write32(pBoatParam, 0); //LRKeyVecForceParamA
			Write32(pBoatParam, 0); //LRKeyVecForceParamB
			Write32(pBoatParam, 0); //VecForce
			Write32(pBoatParam, 0); //VecForceParamA
			Write32(pBoatParam, 0); //VecForceParamB
			Write32(pBoatParam, 0); //BoatShape[]
			Write32(pBoatParam, 0); //WecEffect
			Write32(pBoatParam, 0); //BannerKeyTwist
			Write32(pBoatParam, 0); //BannerKeyTwistParamA
			Write32(pBoatParam, 0); //BannerKeyTwistParamB
			Write32(pBoatParam, 0); //SuaiJianTwist
			Write32(pBoatParam, 0); //BoatHeighRatio
			Write32(pBoatParam, 0); //BannerTwist
			Write32(pBoatParam, 0); //BannerTwistParamA
			Write32(pBoatParam, 0); //DirUpKeyForce
			Write32(pBoatParam, 0); //DirUpKeyForceParamA
			Write32(pBoatParam, 0); //DirUpKeyForceParamB
			Write32(pBoatParam, 0); //DirKeyForce
			Write32(pBoatParam, 0); //DirKeyForceParamA
			Write32(pBoatParam, 0); //DirKeyForceParamB
			Write32(pBoatParam, 0); //AddNO2PerSec
			Write32(pBoatParam, 0); //BoatAirValue
			WriteUn(pBoatParam, 0); //ifSlidFricForce

			len = pBoatParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //BoatForwardAccelNum
		/*for (size_t i = 0; i < 0; i++)
		{ //BoatForwardAccel
			BYTE* pBoatForwardAccel = pNPCKartPhysPara;
			Write16(pBoatForwardAccel, 0); //len

			Write32(pBoatForwardAccel, 0); //Key
			Write32(pBoatForwardAccel, 0); //Value

			len = pBoatForwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamA
		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamB

		len = pNPCKartPhysPara - p;
		Set16(p, (WORD)len);
		p += len;
	}
	const int MAX_NPC_AI_LEVEL_NUM = 12;
	typedef struct NPCKARTAIPHYSPARA {

		UINT NpcN2OIntervalTime;
		UINT NpcTriggerN2ORate;
		UINT NpcRestCheckTime;
		UINT NpcFixVelocity;
		UINT NpcFixVelocityRate;
		UINT NpcRunDisHulanThreshold;
		UINT NpcSJetRate;
		UINT NpcTurnAjustDisHulanRate;


	};
	const static NPCKARTAIPHYSPARA NPCKartAIPhysPara[]{
		{18000,0,1950,105,100,1,20,100},
		{16000,0,1900,110,100,1,20,100},
		{12000,100,1850,115,100,1,30,100},
		{11000,100,1800,120,100,1,30,30},
		{10000,100,1750,125,100,1,100,100},
		{9000,100,1700,135,100,1,100,100},
		{7000,100,1700,135,100,1,100,100},
		{6000,100,1700,140,100,1,100,100},
		{6000,100,1700,145,100,1,100,100},
		{5500,100,1700,150,100,1,100,100},
		{5500,100,1700,160,100,1,100,100},
		{3000,500,1000,560,100,1,150,100},
		/*{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},
		{5500,500,1000,1000,100,1,150,100},*/

	};

	for (size_t i = 0; i < 12; i++)
	{ //NPCKartAIPhysPara
		BYTE* pNPCKartAIPhysPara = p;
		Write16(pNPCKartAIPhysPara, 0); //len

		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcN2OIntervalTime); //NpcN2OIntervalTime
		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcTriggerN2ORate); //NpcTriggerN2ORate
		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcRestCheckTime); //NpcRestCheckTime
		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcFixVelocity); //NpcFixVelocity
		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcFixVelocityRate); //NpcFixVelocityRate
		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcRunDisHulanThreshold); //NpcRunDisHulanThreshold
		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcSJetRate); //NpcSJetRate
		Write32(pNPCKartAIPhysPara, NPCKartAIPhysPara[i].NpcTurnAjustDisHulanRate); //NpcTurnAjustDisHulanRate
		Write8(pNPCKartAIPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
		{ //ForwardAccel
			BYTE* pForwardAccel = pNPCKartAIPhysPara;
			Write16(pForwardAccel, 0); //len

			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
			Write32(pForwardAccel, ForwardAccel[i].Value); //Value

			len = pForwardAccel - pNPCKartAIPhysPara;
			Set16(pNPCKartAIPhysPara, (WORD)len);
			pNPCKartAIPhysPara += len;
		}
		Write8(pNPCKartAIPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
		{ //BackwardAccel
			BYTE* pBackwardAccel = pNPCKartAIPhysPara;
			Write16(pBackwardAccel, 0); //len

			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value

			len = pBackwardAccel - pNPCKartAIPhysPara;
			Set16(pNPCKartAIPhysPara, (WORD)len);
			pNPCKartAIPhysPara += len;
		}
		Write32(pNPCKartAIPhysPara, 100); //StraightLenThreshold
		Write32(pNPCKartAIPhysPara, 100); //NpcDriftRate
		Write8(pNPCKartAIPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
		{ //CompensateParam
			BYTE* pCompensateParam = pNPCKartAIPhysPara;
			Write16(pCompensateParam, 0); //len

			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value

			len = pCompensateParam - pNPCKartAIPhysPara;
			Set16(pNPCKartAIPhysPara, (WORD)len);
			pNPCKartAIPhysPara += len;
		}

		len = pNPCKartAIPhysPara - p;
		Set16(p, (WORD)len);
		p += len;
	}

	//2
	{ //NPCKartPhysPara
		BYTE* pNPCKartPhysPara = p;
		Write16(pNPCKartPhysPara, 0); //len

		Write32(pNPCKartPhysPara, 0); //KartID
		Write8(pNPCKartPhysPara, sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0])); //PhysInfoCount
		for (size_t i = 0; i < sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0]); i++)
		{ //PhysInfo
			BYTE* pPhysInfo = pNPCKartPhysPara;
			Write16(pPhysInfo, 0); //len

			Write32(pPhysInfo, NPCKartPhysPara[i].PID); //PID
			Write32(pPhysInfo, NPCKartPhysPara[i].AffectID); //AffectID
			Write32(pPhysInfo, NPCKartPhysPara[i].Param); //Param
			Write32(pPhysInfo, NPCKartPhysPara[i].LifeTime); //LifeTime

			len = pPhysInfo - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
		{ //CompensateParam
			BYTE* pCompensateParam = pNPCKartPhysPara;
			Write16(pCompensateParam, 0); //len

			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value

			len = pCompensateParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		{ //CollisionBoxParam
			BYTE* pCollisionBoxParam = pNPCKartPhysPara;
			Write16(pCollisionBoxParam, 0); //len

			Write32(pCollisionBoxParam, 300); //BoxLength
			Write32(pCollisionBoxParam, 170); //BoxWidth
			Write32(pCollisionBoxParam, 240); //BoxHeight
			Write32(pCollisionBoxParam, 240); //AdjustOffset
			Write32(pCollisionBoxParam, 200); //AdjustPickGap
			Write32(pCollisionBoxParam, 190); //AdjustRemoteOffset
			Write32(pCollisionBoxParam, 200); //AdjustBevelOffset
			Write32(pCollisionBoxParam, -30); //ControllerOffset

			len = pCollisionBoxParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		{ //CarToCarCollisionParam
			BYTE* pCarToCarCollisionParam = pNPCKartPhysPara;
			Write16(pCarToCarCollisionParam, 0); //len

			Write32(pCarToCarCollisionParam, 300); //CollisionRadius
			Write32(pCarToCarCollisionParam, 300); //CollisionLength
			Write32(pCarToCarCollisionParam, 200); //CollisionWidth
			Write32(pCarToCarCollisionParam, 250); //CollisionHeight
			Write32(pCarToCarCollisionParam, 100000); //CollisionLostCtrlTime

			len = pCarToCarCollisionParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write32(pNPCKartPhysPara, 1130); //BaseTurnRate
		Write32(pNPCKartPhysPara, 1150); //VolatileTurnRate
		Write32(pNPCKartPhysPara, 12200); //MaxTurnSpeed
		Write32(pNPCKartPhysPara, 5700); //MinTurnSpeed
		Write32(pNPCKartPhysPara, 500); //MaxAccuTime
		Write32(pNPCKartPhysPara, 150); //BaseAccuRate
		Write32(pNPCKartPhysPara, 19000); //MaxAffectSpeed
		Write32(pNPCKartPhysPara, -15000); //Gravity
		Write32(pNPCKartPhysPara, -2740); //AdditionalLocalZSpeed
		Write32(pNPCKartPhysPara, 20000); //StartVec
		Write32(pNPCKartPhysPara, 50000); //EndVecFist
		Write32(pNPCKartPhysPara, 18000); //EndVecSecon
		Write32(pNPCKartPhysPara, 1500); //DirKeyForce
		Write32(pNPCKartPhysPara, 5900); //DirKeyTwist
		Write32(pNPCKartPhysPara, 4400); //BannerTwist
		Write32(pNPCKartPhysPara, 6200); //BannerKeyTwist
		Write32(pNPCKartPhysPara, 6200); //BannerVecForce
		Write32(pNPCKartPhysPara, 5500); //BannerHeadForce
		Write32(pNPCKartPhysPara, 2650); //SlidFricForce
		Write32(pNPCKartPhysPara, 1000); //RollFricForce
		Write32(pNPCKartPhysPara, 800); //StartWec
		Write32(pNPCKartPhysPara, 3500); //MaxWec
		Write32(pNPCKartPhysPara, 5000); //SuaiJianTwist
		Write32(pNPCKartPhysPara, 10240); //DirUpKeyForce
		Write8(pNPCKartPhysPara, 6); //AccelStatusCount
		for (size_t i = 0; i < 6; i++) {
			Write32(pNPCKartPhysPara, NPCAccelStatusCount[i]); //AccelStatus[]
		}
		Write8(pNPCKartPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
		{ //ForwardAccel
			BYTE* pForwardAccel = pNPCKartPhysPara;
			Write16(pForwardAccel, 0); //len

			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
			Write32(pForwardAccel, ForwardAccel[i].Value); //Value

			len = pForwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(ForwardDecel) / sizeof(ForwardDecel[0])); //ForwardDecelNum
		for (size_t i = 0; i < sizeof(ForwardDecel) / sizeof(ForwardDecel[0]); i++)
		{ //ForwardDecel
			BYTE* pForwardDecel = pNPCKartPhysPara;
			Write16(pForwardDecel, 0); //len

			Write32(pForwardDecel, ForwardDecel[i].Key); //Key
			Write32(pForwardDecel, ForwardDecel[i].Value); //Value

			len = pForwardDecel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
		{ //BackwardAccel
			BYTE* pBackwardAccel = pNPCKartPhysPara;
			Write16(pBackwardAccel, 0); //len

			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value

			len = pBackwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(BackwardDecel) / sizeof(BackwardDecel[0])); //BackwardDecelNum
		for (size_t i = 0; i < sizeof(BackwardDecel) / sizeof(BackwardDecel[0]); i++)
		{ //BackwardDecel
			BYTE* pBackwardDecel = pNPCKartPhysPara;
			Write16(pBackwardDecel, 0); //len

			Write32(pBackwardDecel, BackwardDecel[i].Key); //Key
			Write32(pBackwardDecel, BackwardDecel[i].Value); //Value

			len = pBackwardDecel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write32(pNPCKartPhysPara, 0); //CrashCompensatePower
		Write32(pNPCKartPhysPara, 0); //DefenseRate
		Write32(pNPCKartPhysPara, 0); //Uin
		Write32(pNPCKartPhysPara, 0); //SpeedupCardGenRate
		Write32(pNPCKartPhysPara, 0); //ExtraFuncFlag
		Write8(pNPCKartPhysPara, 0); //SkatePara
		/*for (size_t i = 0; i < 0; i++)
		{ //hasSkatePara
			BYTE* phasSkatePara = pNPCKartPhysPara;
			Write16(phasSkatePara, 0); //len

			Write32(phasSkatePara, 0); //TrackSpeed
			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirBigJet
			Write32(phasSkatePara, 0); //SkateSpeedCoef_Jump
			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirJetPoint
			Write32(phasSkatePara, 0); //SkateSpeedCoef_MaxForce
			Write32(phasSkatePara, 0); //OntrackAccRef
			Write32(phasSkatePara, 0); //TrackAccRef
			Write32(phasSkatePara, 0); //DashSkill

			len = phasSkatePara - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //HasBattleModeSkillInfoList
		/*for (size_t i = 0; i < 0; i++)
		{ //BattleModeSkillInfoList
			BYTE* pBattleModeSkillInfoList = pNPCKartPhysPara;
			Write16(pBattleModeSkillInfoList, 0); //len

			Write8(pBattleModeSkillInfoList, 0); //SkillNum
			for (size_t i = 0; i < 0; i++)
			{ //SkillInfo
				BYTE* pSkillInfo = pBattleModeSkillInfoList;
				Write16(pSkillInfo, 0); //len

				Write16(pSkillInfo, 0); //SkillId
				Write8(pSkillInfo, 0); //SkillLevel

				len = pSkillInfo - pBattleModeSkillInfoList;
				Set16(pBattleModeSkillInfoList, (WORD)len);
				pBattleModeSkillInfoList += len;
			}

			len = pBattleModeSkillInfoList - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write32(pNPCKartPhysPara, 0); //EnergyConvert
		Write16(pNPCKartPhysPara, 0); //SuperN2ORate
		Write16(pNPCKartPhysPara, 0); //CollisionLoseRate
		Write8(pNPCKartPhysPara, 0); //NotInterruptDrift
		Write8(pNPCKartPhysPara, 0); //AddWeightNum
		Write8(pNPCKartPhysPara, 0); //WeightNum
		Write8(pNPCKartPhysPara, 0); //HasAntiCollisionCfg
		/*for (size_t i = 0; i < 0; i++)
		{ //AntiCollistionCfg
			BYTE* pAntiCollistionCfg = pNPCKartPhysPara;
			Write16(pAntiCollistionCfg, 0); //len

			Write8(pAntiCollistionCfg, 0); //Num
			Write32(pAntiCollistionCfg, 0); //Position[]

			len = pAntiCollistionCfg - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //HasBoatParam
		/*for (size_t i = 0; i < 0; i++)
		{ //BoatParam
			BYTE* pBoatParam = pNPCKartPhysPara;
			Write16(pBoatParam, 0); //len

			Write32(pBoatParam, 0); //DirKeyForceParamC
			Write32(pBoatParam, 0); //DirKeyForceParamD
			Write32(pBoatParam, 0); //DTFLimitAngle
			Write32(pBoatParam, 0); //DTFRatio
			Write32(pBoatParam, 0); //UpKeyVecForce
			Write32(pBoatParam, 0); //UpKeyVecForceParamA
			Write32(pBoatParam, 0); //UpKeyVecForceParamB
			Write32(pBoatParam, 0); //LRKeyVecForce
			Write32(pBoatParam, 0); //LRKeyVecForceParamA
			Write32(pBoatParam, 0); //LRKeyVecForceParamB
			Write32(pBoatParam, 0); //VecForce
			Write32(pBoatParam, 0); //VecForceParamA
			Write32(pBoatParam, 0); //VecForceParamB
			Write32(pBoatParam, 0); //BoatShape[]
			Write32(pBoatParam, 0); //WecEffect
			Write32(pBoatParam, 0); //BannerKeyTwist
			Write32(pBoatParam, 0); //BannerKeyTwistParamA
			Write32(pBoatParam, 0); //BannerKeyTwistParamB
			Write32(pBoatParam, 0); //SuaiJianTwist
			Write32(pBoatParam, 0); //BoatHeighRatio
			Write32(pBoatParam, 0); //BannerTwist
			Write32(pBoatParam, 0); //BannerTwistParamA
			Write32(pBoatParam, 0); //DirUpKeyForce
			Write32(pBoatParam, 0); //DirUpKeyForceParamA
			Write32(pBoatParam, 0); //DirUpKeyForceParamB
			Write32(pBoatParam, 0); //DirKeyForce
			Write32(pBoatParam, 0); //DirKeyForceParamA
			Write32(pBoatParam, 0); //DirKeyForceParamB
			Write32(pBoatParam, 0); //AddNO2PerSec
			Write32(pBoatParam, 0); //BoatAirValue
			WriteUn(pBoatParam, 0); //ifSlidFricForce

			len = pBoatParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //BoatForwardAccelNum
		/*for (size_t i = 0; i < 0; i++)
		{ //BoatForwardAccel
			BYTE* pBoatForwardAccel = pNPCKartPhysPara;
			Write16(pBoatForwardAccel, 0); //len

			Write32(pBoatForwardAccel, 0); //Key
			Write32(pBoatForwardAccel, 0); //Value

			len = pBoatForwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamA
		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamB

		len = pNPCKartPhysPara - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 12); // LevelNum

	for (size_t i = 0; i < 12; i++)
	{ // NPCKartAIPhysPara_NpcPlayer
		BYTE* pNPCKartAIPhysPara_NpcPlayer = p;
		Write16(pNPCKartAIPhysPara_NpcPlayer, 0); // len

		{ // Param
			BYTE* pParam = pNPCKartAIPhysPara_NpcPlayer;
			Write16(pParam, 0); // len

			Write32(pParam, NPCKartAIPhysPara[i].NpcN2OIntervalTime); //NpcN2OIntervalTime
			Write32(pParam, NPCKartAIPhysPara[i].NpcTriggerN2ORate); //NpcTriggerN2ORate
			Write32(pParam, NPCKartAIPhysPara[i].NpcRestCheckTime); //NpcRestCheckTime
			Write32(pParam, NPCKartAIPhysPara[i].NpcFixVelocity); //NpcFixVelocity
			Write32(pParam, NPCKartAIPhysPara[i].NpcFixVelocityRate); //NpcFixVelocityRate
			Write32(pParam, NPCKartAIPhysPara[i].NpcRunDisHulanThreshold); //NpcRunDisHulanThreshold
			Write32(pParam, NPCKartAIPhysPara[i].NpcSJetRate); //NpcSJetRate
			Write32(pParam, NPCKartAIPhysPara[i].NpcTurnAjustDisHulanRate); //NpcTurnAjustDisHulanRate

			Write8(pParam, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); // ForwardAccelNum

			for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
			{ //ForwardAccel
				BYTE* pForwardAccel = pParam;
				Write16(pForwardAccel, 0); //len

				Write32(pForwardAccel, ForwardAccel[i].Key); //Key
				Write32(pForwardAccel, ForwardAccel[i].Value); //Value

				len = pForwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}


			Write8(pParam, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); // BackwardAccelNum

			for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
			{ //BackwardAccel
				BYTE* pBackwardAccel = pParam;
				Write16(pBackwardAccel, 0); //len

				Write32(pBackwardAccel, BackwardAccel[i].Key);
				Write32(pBackwardAccel, BackwardAccel[i].Value);

				len = pBackwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}

			Write32(pParam, 25); // StraightLenThreshold
			Write32(pParam, 100); // NpcDriftRate

			Write8(pParam, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); // CompensateParamCount

			for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
			{ //CompensateParam
				BYTE* pCompensateParam = pParam;
				Write16(pCompensateParam, 0); //len

				Write32(pCompensateParam, CompensateParamCount[i].Percent);
				Write32(pCompensateParam, CompensateParamCount[i].Value);

				len = pCompensateParam - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}


			len = pParam - pNPCKartAIPhysPara_NpcPlayer;
			Set16(pNPCKartAIPhysPara_NpcPlayer, (WORD)len);
			pNPCKartAIPhysPara_NpcPlayer += len;
		}
		Write32(pNPCKartAIPhysPara_NpcPlayer, 100); // N2OStartTime
		Write32(pNPCKartAIPhysPara_NpcPlayer, 1000); // NpcN2OMaxCount
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // Dis2FirstPlay
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlTime
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostInterval
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlCount

		len = pNPCKartAIPhysPara_NpcPlayer - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ //NPCKartPhysPara
		BYTE* pNPCKartPhysPara = p;
		Write16(pNPCKartPhysPara, 0); //len

		Write32(pNPCKartPhysPara, 0); //KartID
		Write8(pNPCKartPhysPara, sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0])); //PhysInfoCount
		for (size_t i = 0; i < sizeof(NPCKartPhysPara) / sizeof(NPCKartPhysPara[0]); i++)
		{ //PhysInfo
			BYTE* pPhysInfo = pNPCKartPhysPara;
			Write16(pPhysInfo, 0); //len

			Write32(pPhysInfo, NPCKartPhysPara[i].PID); //PID
			Write32(pPhysInfo, NPCKartPhysPara[i].AffectID); //AffectID
			Write32(pPhysInfo, NPCKartPhysPara[i].Param); //Param
			Write32(pPhysInfo, NPCKartPhysPara[i].LifeTime); //LifeTime

			len = pPhysInfo - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); //CompensateParamCount
		for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
		{ //CompensateParam
			BYTE* pCompensateParam = pNPCKartPhysPara;
			Write16(pCompensateParam, 0); //len

			Write32(pCompensateParam, CompensateParamCount[i].Percent); //Percent
			Write32(pCompensateParam, CompensateParamCount[i].Value); //Value

			len = pCompensateParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		{ //CollisionBoxParam
			BYTE* pCollisionBoxParam = pNPCKartPhysPara;
			Write16(pCollisionBoxParam, 0); //len

			Write32(pCollisionBoxParam, 300); //BoxLength
			Write32(pCollisionBoxParam, 170); //BoxWidth
			Write32(pCollisionBoxParam, 240); //BoxHeight
			Write32(pCollisionBoxParam, 240); //AdjustOffset
			Write32(pCollisionBoxParam, 200); //AdjustPickGap
			Write32(pCollisionBoxParam, 190); //AdjustRemoteOffset
			Write32(pCollisionBoxParam, 200); //AdjustBevelOffset
			Write32(pCollisionBoxParam, -30); //ControllerOffset

			len = pCollisionBoxParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		{ //CarToCarCollisionParam
			BYTE* pCarToCarCollisionParam = pNPCKartPhysPara;
			Write16(pCarToCarCollisionParam, 0); //len

			Write32(pCarToCarCollisionParam, 300); //CollisionRadius
			Write32(pCarToCarCollisionParam, 300); //CollisionLength
			Write32(pCarToCarCollisionParam, 200); //CollisionWidth
			Write32(pCarToCarCollisionParam, 250); //CollisionHeight
			Write32(pCarToCarCollisionParam, 100000); //CollisionLostCtrlTime

			len = pCarToCarCollisionParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write32(pNPCKartPhysPara, 1000); //BaseTurnRate
		Write32(pNPCKartPhysPara, 1100); //VolatileTurnRate
		Write32(pNPCKartPhysPara, 12000); //MaxTurnSpeed
		Write32(pNPCKartPhysPara, 5700); //MinTurnSpeed
		Write32(pNPCKartPhysPara, 700); //MaxAccuTime
		Write32(pNPCKartPhysPara, 150); //BaseAccuRate
		Write32(pNPCKartPhysPara, 19000); //MaxAffectSpeed
		Write32(pNPCKartPhysPara, -15000); //Gravity
		Write32(pNPCKartPhysPara, -2400); //AdditionalLocalZSpeed
		Write32(pNPCKartPhysPara, 18000); //StartVec
		Write32(pNPCKartPhysPara, 50000); //EndVecFist
		Write32(pNPCKartPhysPara, 16000); //EndVecSecon
		Write32(pNPCKartPhysPara, 1500); //DirKeyForce
		Write32(pNPCKartPhysPara, 5900); //DirKeyTwist
		Write32(pNPCKartPhysPara, 4200); //BannerTwist
		Write32(pNPCKartPhysPara, 6000); //BannerKeyTwist
		Write32(pNPCKartPhysPara, 6000); //BannerVecForce
		Write32(pNPCKartPhysPara, 5500); //BannerHeadForce
		Write32(pNPCKartPhysPara, 2600); //SlidFricForce
		Write32(pNPCKartPhysPara, 1000); //RollFricForce
		Write32(pNPCKartPhysPara, 800); //StartWec
		Write32(pNPCKartPhysPara, 3500); //MaxWec
		Write32(pNPCKartPhysPara, 5000); //SuaiJianTwist
		Write32(pNPCKartPhysPara, 10000); //DirUpKeyForce
		Write8(pNPCKartPhysPara, 6); //AccelStatusCount
		for (size_t i = 0; i < 6; i++) {
			Write32(pNPCKartPhysPara, NPCAccelStatusCount[i]); //AccelStatus[]
		}
		Write8(pNPCKartPhysPara, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); //ForwardAccelNum
		for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
		{ //ForwardAccel
			BYTE* pForwardAccel = pNPCKartPhysPara;
			Write16(pForwardAccel, 0); //len

			Write32(pForwardAccel, ForwardAccel[i].Key); //Key
			Write32(pForwardAccel, ForwardAccel[i].Value); //Value

			len = pForwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(ForwardDecel) / sizeof(ForwardDecel[0])); //ForwardDecelNum
		for (size_t i = 0; i < sizeof(ForwardDecel) / sizeof(ForwardDecel[0]); i++)
		{ //ForwardDecel
			BYTE* pForwardDecel = pNPCKartPhysPara;
			Write16(pForwardDecel, 0); //len

			Write32(pForwardDecel, ForwardDecel[i].Key); //Key
			Write32(pForwardDecel, ForwardDecel[i].Value); //Value

			len = pForwardDecel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); //BackwardAccelNum
		for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
		{ //BackwardAccel
			BYTE* pBackwardAccel = pNPCKartPhysPara;
			Write16(pBackwardAccel, 0); //len

			Write32(pBackwardAccel, BackwardAccel[i].Key); //Key
			Write32(pBackwardAccel, BackwardAccel[i].Value); //Value

			len = pBackwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write8(pNPCKartPhysPara, sizeof(BackwardDecel) / sizeof(BackwardDecel[0])); //BackwardDecelNum
		for (size_t i = 0; i < sizeof(BackwardDecel) / sizeof(BackwardDecel[0]); i++)
		{ //BackwardDecel
			BYTE* pBackwardDecel = pNPCKartPhysPara;
			Write16(pBackwardDecel, 0); //len

			Write32(pBackwardDecel, BackwardDecel[i].Key); //Key
			Write32(pBackwardDecel, BackwardDecel[i].Value); //Value

			len = pBackwardDecel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}
		Write32(pNPCKartPhysPara, 0); //CrashCompensatePower
		Write32(pNPCKartPhysPara, 0); //DefenseRate
		Write32(pNPCKartPhysPara, 0); //Uin
		Write32(pNPCKartPhysPara, 0); //SpeedupCardGenRate
		Write32(pNPCKartPhysPara, 0); //ExtraFuncFlag
		Write8(pNPCKartPhysPara, 0); //SkatePara
		/*for (size_t i = 0; i < 0; i++)
		{ //hasSkatePara
			BYTE* phasSkatePara = pNPCKartPhysPara;
			Write16(phasSkatePara, 0); //len

			Write32(phasSkatePara, 0); //TrackSpeed
			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirBigJet
			Write32(phasSkatePara, 0); //SkateSpeedCoef_Jump
			Write32(phasSkatePara, 0); //SkateSpeedCoef_AirJetPoint
			Write32(phasSkatePara, 0); //SkateSpeedCoef_MaxForce
			Write32(phasSkatePara, 0); //OntrackAccRef
			Write32(phasSkatePara, 0); //TrackAccRef
			Write32(phasSkatePara, 0); //DashSkill

			len = phasSkatePara - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //HasBattleModeSkillInfoList
		/*for (size_t i = 0; i < 0; i++)
		{ //BattleModeSkillInfoList
			BYTE* pBattleModeSkillInfoList = pNPCKartPhysPara;
			Write16(pBattleModeSkillInfoList, 0); //len

			Write8(pBattleModeSkillInfoList, 0); //SkillNum
			for (size_t i = 0; i < 0; i++)
			{ //SkillInfo
				BYTE* pSkillInfo = pBattleModeSkillInfoList;
				Write16(pSkillInfo, 0); //len

				Write16(pSkillInfo, 0); //SkillId
				Write8(pSkillInfo, 0); //SkillLevel

				len = pSkillInfo - pBattleModeSkillInfoList;
				Set16(pBattleModeSkillInfoList, (WORD)len);
				pBattleModeSkillInfoList += len;
			}

			len = pBattleModeSkillInfoList - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write32(pNPCKartPhysPara, 0); //EnergyConvert
		Write16(pNPCKartPhysPara, 0); //SuperN2ORate
		Write16(pNPCKartPhysPara, 0); //CollisionLoseRate
		Write8(pNPCKartPhysPara, 0); //NotInterruptDrift
		Write8(pNPCKartPhysPara, 0); //AddWeightNum
		Write8(pNPCKartPhysPara, 0); //WeightNum
		Write8(pNPCKartPhysPara, 0); //HasAntiCollisionCfg
		/*for (size_t i = 0; i < 0; i++)
		{ //AntiCollistionCfg
			BYTE* pAntiCollistionCfg = pNPCKartPhysPara;
			Write16(pAntiCollistionCfg, 0); //len

			Write8(pAntiCollistionCfg, 0); //Num
			Write32(pAntiCollistionCfg, 0); //Position[]

			len = pAntiCollistionCfg - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //HasBoatParam
		/*for (size_t i = 0; i < 0; i++)
		{ //BoatParam
			BYTE* pBoatParam = pNPCKartPhysPara;
			Write16(pBoatParam, 0); //len

			Write32(pBoatParam, 0); //DirKeyForceParamC
			Write32(pBoatParam, 0); //DirKeyForceParamD
			Write32(pBoatParam, 0); //DTFLimitAngle
			Write32(pBoatParam, 0); //DTFRatio
			Write32(pBoatParam, 0); //UpKeyVecForce
			Write32(pBoatParam, 0); //UpKeyVecForceParamA
			Write32(pBoatParam, 0); //UpKeyVecForceParamB
			Write32(pBoatParam, 0); //LRKeyVecForce
			Write32(pBoatParam, 0); //LRKeyVecForceParamA
			Write32(pBoatParam, 0); //LRKeyVecForceParamB
			Write32(pBoatParam, 0); //VecForce
			Write32(pBoatParam, 0); //VecForceParamA
			Write32(pBoatParam, 0); //VecForceParamB
			Write32(pBoatParam, 0); //BoatShape[]
			Write32(pBoatParam, 0); //WecEffect
			Write32(pBoatParam, 0); //BannerKeyTwist
			Write32(pBoatParam, 0); //BannerKeyTwistParamA
			Write32(pBoatParam, 0); //BannerKeyTwistParamB
			Write32(pBoatParam, 0); //SuaiJianTwist
			Write32(pBoatParam, 0); //BoatHeighRatio
			Write32(pBoatParam, 0); //BannerTwist
			Write32(pBoatParam, 0); //BannerTwistParamA
			Write32(pBoatParam, 0); //DirUpKeyForce
			Write32(pBoatParam, 0); //DirUpKeyForceParamA
			Write32(pBoatParam, 0); //DirUpKeyForceParamB
			Write32(pBoatParam, 0); //DirKeyForce
			Write32(pBoatParam, 0); //DirKeyForceParamA
			Write32(pBoatParam, 0); //DirKeyForceParamB
			Write32(pBoatParam, 0); //AddNO2PerSec
			Write32(pBoatParam, 0); //BoatAirValue
			WriteUn(pBoatParam, 0); //ifSlidFricForce

			len = pBoatParam - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write8(pNPCKartPhysPara, 0); //BoatForwardAccelNum
		/*for (size_t i = 0; i < 0; i++)
		{ //BoatForwardAccel
			BYTE* pBoatForwardAccel = pNPCKartPhysPara;
			Write16(pBoatForwardAccel, 0); //len

			Write32(pBoatForwardAccel, 0); //Key
			Write32(pBoatForwardAccel, 0); //Value

			len = pBoatForwardAccel - pNPCKartPhysPara;
			Set16(pNPCKartPhysPara, (WORD)len);
			pNPCKartPhysPara += len;
		}*/
		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamA
		Write32(pNPCKartPhysPara, 0); //DirUpKeyForceParamB

		len = pNPCKartPhysPara - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 10); // NPCLevelNum_RankedMatch
	for (size_t i = 0; i < 10; i++)
	{ // NPCKartAIPhysPara_NpcPlayer
		BYTE* pNPCKartAIPhysPara_NpcPlayer = p;
		Write16(pNPCKartAIPhysPara_NpcPlayer, 0); // len

		{ // Param
			BYTE* pParam = pNPCKartAIPhysPara_NpcPlayer;
			Write16(pParam, 0); // len

			Write32(pParam, NPCKartAIPhysPara[i].NpcN2OIntervalTime); //NpcN2OIntervalTime
			Write32(pParam, NPCKartAIPhysPara[i].NpcTriggerN2ORate); //NpcTriggerN2ORate
			Write32(pParam, NPCKartAIPhysPara[i].NpcRestCheckTime); //NpcRestCheckTime
			Write32(pParam, NPCKartAIPhysPara[i].NpcFixVelocity); //NpcFixVelocity
			Write32(pParam, NPCKartAIPhysPara[i].NpcFixVelocityRate); //NpcFixVelocityRate
			Write32(pParam, NPCKartAIPhysPara[i].NpcRunDisHulanThreshold); //NpcRunDisHulanThreshold
			Write32(pParam, NPCKartAIPhysPara[i].NpcSJetRate); //NpcSJetRate
			Write32(pParam, NPCKartAIPhysPara[i].NpcTurnAjustDisHulanRate); //NpcTurnAjustDisHulanRate

			Write8(pParam, sizeof(ForwardAccel) / sizeof(ForwardAccel[0])); // ForwardAccelNum

			for (size_t i = 0; i < sizeof(ForwardAccel) / sizeof(ForwardAccel[0]); i++)
			{ //ForwardAccel
				BYTE* pForwardAccel = pParam;
				Write16(pForwardAccel, 0); //len

				Write32(pForwardAccel, ForwardAccel[i].Key); //Key
				Write32(pForwardAccel, ForwardAccel[i].Value); //Value

				len = pForwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}


			Write8(pParam, sizeof(BackwardAccel) / sizeof(BackwardAccel[0])); // BackwardAccelNum

			for (size_t i = 0; i < sizeof(BackwardAccel) / sizeof(BackwardAccel[0]); i++)
			{ //BackwardAccel
				BYTE* pBackwardAccel = pParam;
				Write16(pBackwardAccel, 0); //len

				Write32(pBackwardAccel, BackwardAccel[i].Key);
				Write32(pBackwardAccel, BackwardAccel[i].Value);

				len = pBackwardAccel - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}

			Write32(pParam, 25); // StraightLenThreshold
			Write32(pParam, 100); // NpcDriftRate

			Write8(pParam, sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0])); // CompensateParamCount

			for (size_t i = 0; i < sizeof(CompensateParamCount) / sizeof(CompensateParamCount[0]); i++)
			{ //CompensateParam
				BYTE* pCompensateParam = pParam;
				Write16(pCompensateParam, 0); //len

				Write32(pCompensateParam, CompensateParamCount[i].Percent);
				Write32(pCompensateParam, CompensateParamCount[i].Value);

				len = pCompensateParam - pParam;
				Set16(pParam, (WORD)len);
				pParam += len;
			}


			len = pParam - pNPCKartAIPhysPara_NpcPlayer;
			Set16(pNPCKartAIPhysPara_NpcPlayer, (WORD)len);
			pNPCKartAIPhysPara_NpcPlayer += len;
		}
		Write32(pNPCKartAIPhysPara_NpcPlayer, 100); // N2OStartTime
		Write32(pNPCKartAIPhysPara_NpcPlayer, 1000); // NpcN2OMaxCount
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // Dis2FirstPlay
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlTime
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostInterval
		Write32(pNPCKartAIPhysPara_NpcPlayer, 0); // LostControlCount

		len = pNPCKartAIPhysPara_NpcPlayer - p;
		Set16(p, (WORD)len);
		p += len;
	}


	Write32(p, 0x0033BC27); // SwitchFlag1
	Write16(p, 60);			// MaxQuickLerpThrehold
	Write16(p, 10);			// QuickLerpStepCnts
	Write16(p, 12);			// LerpSynccpFrequence

	Write16(p, 3); // ClientItemOpenTypeNum
	{			   // ClientItemOpenCfg
		BYTE* pClientItemOpenCfg = p;
		Write16(pClientItemOpenCfg, 0); // len

		Write16(pClientItemOpenCfg, 5); // OpenType
		Write8(pClientItemOpenCfg, 5);	// ItemNum
		Write32(pClientItemOpenCfg, 0); // ItemList[]

		len = pClientItemOpenCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // ClientItemOpenCfg
		BYTE* pClientItemOpenCfg = p;
		Write16(pClientItemOpenCfg, 0); // len

		Write16(pClientItemOpenCfg, 10); // OpenType
		Write8(pClientItemOpenCfg, 10);	 // ItemNum
		Write32(pClientItemOpenCfg, 0);	 // ItemList[]

		len = pClientItemOpenCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ // ClientItemOpenCfg
		BYTE* pClientItemOpenCfg = p;
		Write16(pClientItemOpenCfg, 0); // len

		Write16(pClientItemOpenCfg, 50); // OpenType
		Write8(pClientItemOpenCfg, 50);	 // ItemNum
		Write32(pClientItemOpenCfg, 0);	 // ItemList[]

		len = pClientItemOpenCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}



	Write8(p, 0); // OtherNum
	for (size_t i = 0; i < 0; i++)
	{ // CollisionPowerOtherInfo
		BYTE* pCollisionPowerOtherInfo = p;
		Write16(pCollisionPowerOtherInfo, 0); // len

		Write32(pCollisionPowerOtherInfo, 0);	// Power
		Write32(pCollisionPowerOtherInfo, 100); // Ratio
		Write32(pCollisionPowerOtherInfo, 0);	// Add

		len = pCollisionPowerOtherInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 400);  // CollisionMinPower
	Write32(p, 2900); // ItemExtendLimit
	Write32(p, 50);	  // ChattingTalkLevelLimit
	Write32(p, 0);	  // ChattingSpecialOp

	Write8(p, 0); // ForbidMapNum
	// Write32(p, 0); //ForbidMap[]

	Write32(p, 20000); // MaxBuySpeedNum

	Write8(p, 0); // ClientItemIconNum

	//for (size_t i = 0; i < n; i++)
	//{ //ClientIconCfg
	//	BYTE* pClientIconCfg = p;
	//	Write16(pClientIconCfg, 0); //len

	//	Write32(pClientIconCfg, 0); //ItemID
	//	Write8(pClientIconCfg, 0); //IconPathLen

	//	len = pClientIconCfg - p;
	//	Set16(p, (WORD)len);
	//	p += len;
	//}


	Write8(p, 0); // TDCBlackCarNum
	// Write32(p, 0); //TDCBlackCar[]

	Write8(p, 0); // TDCBlackCarTypeNum
	// Write32(p, 0); //TDCBlackCarType[]

	Write8(p, 0); // TDCBlackCarType2ndNum
	// Write32(p, 0); //TDCBlackCarType2nd[]

	{ // ClientFrameRateCfg
		BYTE* pClientFrameRateCfg = p;
		Write16(pClientFrameRateCfg, 0); // len
		if (Client->Uin!=10001&&getGameConfig()["LockFps"] && getGameConfig()["LockFps"].as<UINT>() == 1) {
			Write32(pClientFrameRateCfg, 30000); //MinFrameRate
			Write32(pClientFrameRateCfg, 40000); //MaxFrameRate
			Write32(pClientFrameRateCfg, 10); //FrameStep
			Write32(pClientFrameRateCfg, 40000); //CurrentFrameRate
		}
		else {
			Write32(pClientFrameRateCfg, 0);	 // MinFrameRate
			Write32(pClientFrameRateCfg, 60000); // MaxFrameRate
			Write32(pClientFrameRateCfg, 10);	 // FrameStep
			Write32(pClientFrameRateCfg, 0);	 // CurrentFrameRate
		}

		len = pClientFrameRateCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 16);	// ReportFrameRateValue
	Write16(p, 10); // FrameOpSpan
	Write8(p, 0);	// VideoSwitch

	Write8(p, 0); // KartRefitCardTypeNum
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write8(p, 0);



	len = p - buf;
	SendToClient(Client, 25066, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);

	//delete [] buf;
}
void NotifySpeed2Cfg(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 0); // MapEffectMaskNum
	/*
	for (size_t i = 0; i < 0; i++)
	{ //MapEffectMask
		BYTE* pMapEffectMask = p;
		Write16(pMapEffectMask, 0); //len

		Write16(pMapEffectMask, 0); //MapID
		Write32(pMapEffectMask, 0); //EffectMask

		len = pMapEffectMask - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0);	 // Speed2ShopLimit
	Write32(p, 127); // OpenFlag

	len = p - buf;
	SendToClient(Client, 11358, buf, len, Client->ServerID, FE_GAMESVRD, Client->ConnID, Notify);
}

void NotifyTopUIItemInfo(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, Client->Uin); // Uin
	// ID
	{
		// 19	变身派对
		// 43	荣耀排位赛
		// 62	赛道随心造
		// 63	世界挑战赛
		// 67	双人冲顶赛
		// 81	狂野追逐
		// 90	暗夜大逃亡
		// 96	超能竞速赛
		// 103	疾风前传
		// 109	双人冲锋战
		// 120	龙晶大闯关
		// 127	极速对决
		// 134	车王新剧情
		// 143	互助接力赛
		// 153	年兽大乱斗
		// 155	雷诺嘉年华
		// 162	QS极速幻境
		// 171	绝命车王
		// 183	转世续缘
		// 190	跳跳派对
		// 197	时空捕手
		// 208	顺子大作战
		// 227	冰雪飞驰
		// 252	问天之约
		// 257	全明星大乱斗
		// 285	极速同行
		// 362以后未做测试 不知道是什么
	}
	UINT DisplayUIItemID[] = {43, 120};
	UINT ShowUINum = sizeof(DisplayUIItemID) / sizeof(UINT);
	Write16(p, ShowUINum); // Num
	for (size_t i = 0; i < ShowUINum; i++)
	{ // TopUIItemInfo
		BYTE *pTopUIItemInfo = p;
		Write16(pTopUIItemInfo, 0); // len

		Write8(pTopUIItemInfo, 0);					 // Type
		Write16(pTopUIItemInfo, DisplayUIItemID[i]); // ID
		Write16(pTopUIItemInfo, 0);					 // Tag
		Write16(pTopUIItemInfo, 0);					 // NameLen
		Write16(pTopUIItemInfo, 0);					 // PrompLen

		len = pTopUIItemInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	{ // RecommendModeInfo
		BYTE *pRecommendModeInfo = p;
		Write16(pRecommendModeInfo, 0); // len

		Write16(pRecommendModeInfo, 0); // ModeNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ModeInfo
			BYTE* pModeInfo = pRecommendModeInfo;
			Write16(pModeInfo, 0); //len

			Write16(pModeInfo, 0); //ModeID
			Write16(pModeInfo, 0); //Status
			Write32(pModeInfo, 0); //Privige
			Write16(pModeInfo, 0); //PrompLen

			len = pModeInfo - pRecommendModeInfo;
			Set16(pRecommendModeInfo, (WORD)len);
			pRecommendModeInfo += len;
		}
		*/
		len = pRecommendModeInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); // Count
	/*
	for (size_t i = 0; i < n; i++)
	{ //EntryList
		BYTE* pEntryList = p;
		Write16(pEntryList, 0); //len

		Write8(pEntryList, 0); //Type
		Write8(pEntryList, 0); //Status
		Write16(pEntryList, 0); //PromptLen
		Write8(pEntryList, 0); //Tag
		Write8(pEntryList, 0); //Icon
		Write8(pEntryList, 0); //Kind
		Write8(pEntryList, 0); //Privilige
		Write8(pEntryList, 0); //IsTodayOpen
		Write8(pEntryList, 0); //SetType
		Write8(pEntryList, 0); //EmbedType
		Write8(pEntryList, 0); //LoginWindow

		len = pEntryList - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // SetNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //EntrySet
		BYTE* pEntrySet = p;
		Write16(pEntrySet, 0); //len

		Write8(pEntrySet, 0); //Count
		for (size_t i = 0; i < n; i++)
		{ //EntryList
			BYTE* pEntryList = pEntrySet;
			Write16(pEntryList, 0); //len

			Write8(pEntryList, 0); //Type
			Write8(pEntryList, 0); //Status
			Write16(pEntryList, 0); //PromptLen
			Write8(pEntryList, 0); //Tag
			Write8(pEntryList, 0); //Icon
			Write8(pEntryList, 0); //Kind
			Write8(pEntryList, 0); //Privilige
			Write8(pEntryList, 0); //IsTodayOpen
			Write8(pEntryList, 0); //SetType
			Write8(pEntryList, 0); //EmbedType
			Write8(pEntryList, 0); //LoginWindow

			len = pEntryList - pEntrySet;
			Set16(pEntrySet, (WORD)len);
			pEntrySet += len;
		}

		len = pEntrySet - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // MouseOverNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //MouseOverTipsCfg
		BYTE* pMouseOverTipsCfg = p;
		Write16(pMouseOverTipsCfg, 0); //len

		Write16(pMouseOverTipsCfg, 0); //ID
		Write16(pMouseOverTipsCfg, 0); //PrompLen

		len = pMouseOverTipsCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 25116, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyRedPointInfo(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, Client->Uin); // Uin
	Write8(p, 0);			 // RedPointNum

	for (size_t i = 0; i < 0; i++)
	{ // RedPointInfo
		BYTE *pRedPointInfo = p;
		Write16(pRedPointInfo, 0); // len

		Write8(pRedPointInfo, i + 1); // Type
		Write8(pRedPointInfo, 1);	  // Status

		len = pRedPointInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 1); // All
	Write8(p, 0); // IsNeedPlayVideoGuid

	len = p - buf;
	SendToClient(Client, 25114, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void RequestGetActivityCenterInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseGetActivityCenterInfo(Client);
}
void ResponseGetActivityCenterInfo(ClientNode* Client)
{
	BYTE buf[8192] = { 0 }; BYTE* p = buf; size_t len = 0;

	Write32(p, Client->Uin); //Uin
	Write32(p, 0); //TotalActivity
	Write32(p, 0); //CurrentWeekActivity
	Write32(p, 0); //LastWeekActivity
	Write32(p, 0); //CurrentDayActivity
	Write32(p, 0); //ActivityAppellcation
	Write32(p, 0); //ActivityLevel
	Write32(p, 0); //NextLevelActivity
	{ //DailyActivityBox
		BYTE* pDailyActivityBox = p;
		Write16(pDailyActivityBox, 0); //len

		Write32(pDailyActivityBox, 0); //Day
		Write32(pDailyActivityBox, 0); //BoxNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ActivityBox
			BYTE* pActivityBox = pDailyActivityBox;
			Write16(pActivityBox, 0); //len

			Write32(pActivityBox, 0); //BoxLevel
			Write32(pActivityBox, 0); //BoxType
			Write32(pActivityBox, 0); //ActivityType
			Write32(pActivityBox, 0); //NeedActivityNum
			Write32(pActivityBox, 0); //BoxStatus
			Write32(pActivityBox, 0); //BoxID
			for (size_t i = 0; i < n; i++)
			{ //Award
				BYTE* pAward = pActivityBox;
				Write16(pAward, 0); //len

				Write32(pAward, 0); //Exp
				Write32(pAward, 0); //Money
				Write32(pAward, 0); //SuperMoney
				Write16(pAward, 0); //ItemNum
				for (size_t i = 0; i < n; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAward;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					WriteUn(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //Coupons
				Write32(pAward, 0); //GuildPoint
				Write32(pAward, 0); //LuckMoney
				Write8(pAward, 0); //ExtendInfoNum
				for (size_t i = 0; i < n; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAward;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //SpeedCoin

				len = pAward - pActivityBox;
				Set16(pActivityBox, (WORD)len);
				pActivityBox += len;
			}
			{ //PreviewAward
				BYTE* pPreviewAward = pActivityBox;
				Write16(pPreviewAward, 0); //len

				Write32(pPreviewAward, 0); //SuperMoney
				Write32(pPreviewAward, 0); //Coupons
				Write16(pPreviewAward, 0); //ItemNum
				Write32(pPreviewAward, 0); //ItemID[]

				len = pPreviewAward - pActivityBox;
				Set16(pActivityBox, (WORD)len);
				pActivityBox += len;
			}
			Write32(pActivityBox, 0); //PresetBoxID
			Write8(pActivityBox, 0); //HasActiveAward

			len = pActivityBox - pDailyActivityBox;
			Set16(pDailyActivityBox, (WORD)len);
			pDailyActivityBox += len;
		}
		*/
		len = pDailyActivityBox - p;
		Set16(p, (WORD)len);
		p += len;
	}
	{ //ActivityTaskCfg
		BYTE* pActivityTaskCfg = p;
		Write16(pActivityTaskCfg, 0); //len

		Write32(pActivityTaskCfg, 0); //TabNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //ActivityTaskTab
			BYTE* pActivityTaskTab = pActivityTaskCfg;
			Write16(pActivityTaskTab, 0); //len

			Write32(pActivityTaskTab, 0); //TabOrder
			Write32(pActivityTaskTab, 0); //TabType
			Write32(pActivityTaskTab, 0); //Show
			Write32(pActivityTaskTab, 0); //TaskNum
			for (size_t i = 0; i < n; i++)
			{ //ActivityTask
				BYTE* pActivityTask = pActivityTaskTab;
				Write16(pActivityTask, 0); //len

				Write32(pActivityTask, 0); //TaskID
				Write32(pActivityTask, 0); //ActivityAward
				Write16(pActivityTask, 0); //ChildTaskNum
				Write32(pActivityTask, 0); //ChildTaskID[]

				len = pActivityTask - pActivityTaskTab;
				Set16(pActivityTaskTab, (WORD)len);
				pActivityTaskTab += len;
			}

			len = pActivityTaskTab - pActivityTaskCfg;
			Set16(pActivityTaskCfg, (WORD)len);
			pActivityTaskCfg += len;
		}
		*/
		len = pActivityTaskCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write8(p, 0); //BCNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //BC
		BYTE* pBC = p;
		Write16(pBC, 0); //len

		Write16(pBC, 0); //ContentLen

		len = pBC - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, 0); //CurrentWeekActivityUsed
	{ //MultipleTimeCfg
		BYTE* pMultipleTimeCfg = p;
		Write16(pMultipleTimeCfg, 0); //len

		Write32(pMultipleTimeCfg, 0); //BeginTime
		Write32(pMultipleTimeCfg, 0); //EndTime
		Write16(pMultipleTimeCfg, 0); //MultiValue

		len = pMultipleTimeCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //CurrentWeekActivityForLotto
	Write8(p, 0); //LottoItemNum
	//Write32(p, 0); //LottoItemID[]

	len = p - buf;
	SendToClient(Client, 24029, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestC2GSign3Operate(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR OperateType = Read8(Body);

	ResponseC2GSign3Operate(Client, OperateType);
}
void ResponseC2GSign3Operate(ClientNode *Client, UCHAR OperateType)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID
	Write8(p, 0);			 // ReasonLen
	Write8(p, OperateType);	 // OperateType
	Write8(p, 0);			 // HasSign3Info
	/*
	for (size_t i = 0; i < n; i++)
	{ //Sign3Info
		BYTE* pSign3Info = p;
		Write16(pSign3Info, 0); //len

		Write32(pSign3Info, 0); //BeginDate
		Write32(pSign3Info, 0); //EndDate
		Write8(pSign3Info, 0); //SignNum
		Write8(pSign3Info, 0); //CanSign
		Write8(pSign3Info, 0); //AwardNum
		for (size_t i = 0; i < n; i++)
		{ //Sign2Award
			BYTE* pSign2Award = pSign3Info;
			Write16(pSign2Award, 0); //len

			{ //Award
				BYTE* pAward = pSign2Award;
				Write16(pAward, 0); //len

				Write32(pAward, 0); //Exp
				Write32(pAward, 0); //Money
				Write32(pAward, 0); //SuperMoney
				Write16(pAward, 0); //ItemNum
				for (size_t i = 0; i < n; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAward;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					WriteUn(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //Coupons
				Write32(pAward, 0); //GuildPoint
				Write32(pAward, 0); //LuckMoney
				Write8(pAward, 0); //ExtendInfoNum
				for (size_t i = 0; i < n; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAward;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAward;
					Set16(pAward, (WORD)len);
					pAward += len;
				}
				Write32(pAward, 0); //SpeedCoin

				len = pAward - pSign2Award;
				Set16(pSign2Award, (WORD)len);
				pSign2Award += len;
			}
			Write16(pSign2Award, 0); //AwardTimes
			Write8(pSign2Award, 0); //HighGrade
			Write8(pSign2Award, 0); //FuncDes

			len = pSign2Award - pSign3Info;
			Set16(pSign3Info, (WORD)len);
			pSign3Info += len;
		}
		Write32(pSign3Info, 0); //SpecailAwardBeginDate
		Write32(pSign3Info, 0); //SpecailAwardEndDate
		{ //SpecialAward
			BYTE* pSpecialAward = pSign3Info;
			Write16(pSpecialAward, 0); //len

			Write32(pSpecialAward, 0); //Exp
			Write32(pSpecialAward, 0); //Money
			Write32(pSpecialAward, 0); //SuperMoney
			Write16(pSpecialAward, 0); //ItemNum
			for (size_t i = 0; i < n; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pSpecialAward;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, 0); //ItemID
				Write32(pItemInfo, 0); //ItemNum
				Write32(pItemInfo, 0); //AvailPeriod
				Write8(pItemInfo, 0); //Status
				WriteUn(pItemInfo, 0); //ObtainTime
				Write32(pItemInfo, 0); //OtherAttribute
				Write16(pItemInfo, 0); //ItemType

				len = pItemInfo - pSpecialAward;
				Set16(pSpecialAward, (WORD)len);
				pSpecialAward += len;
			}
			Write32(pSpecialAward, 0); //Coupons
			Write32(pSpecialAward, 0); //GuildPoint
			Write32(pSpecialAward, 0); //LuckMoney
			Write8(pSpecialAward, 0); //ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ //ExtendInfoAward
				BYTE* pExtendInfoAward = pSpecialAward;
				Write16(pExtendInfoAward, 0); //len

				Write16(pExtendInfoAward, 0); //Key
				Write32(pExtendInfoAward, 0); //AddValue
				Write32(pExtendInfoAward, 0); //TotalValue
				Write8(pExtendInfoAward, 0); //ShowPriority
				Write16(pExtendInfoAward, 0); //BuyNeedScore
				Write8(pExtendInfoAward, 0); //OneMatchMaxNum

				len = pExtendInfoAward - pSpecialAward;
				Set16(pSpecialAward, (WORD)len);
				pSpecialAward += len;
			}
			Write32(pSpecialAward, 0); //SpeedCoin

			len = pSpecialAward - pSign3Info;
			Set16(pSign3Info, (WORD)len);
			pSign3Info += len;
		}
		Write8(pSign3Info, 0); //SpecialAwardState
		Write8(pSign3Info, 0); //IsSpecailAwardRemind
		Write8(pSign3Info, 0); //TipsLen
		Write32(pSign3Info, 0); //SpecialAwardID
		Write8(pSign3Info, 0); //LabelNum
		Write8(pSign3Info, 0); //GotLabelNum
		Write8(pSign3Info, 0); //FinalAwardState
		{ //FinalAward
			BYTE* pFinalAward = pSign3Info;
			Write16(pFinalAward, 0); //len

			Write32(pFinalAward, 0); //Exp
			Write32(pFinalAward, 0); //Money
			Write32(pFinalAward, 0); //SuperMoney
			Write16(pFinalAward, 0); //ItemNum
			for (size_t i = 0; i < n; i++)
			{ //ItemInfo
				BYTE* pItemInfo = pFinalAward;
				Write16(pItemInfo, 0); //len

				Write32(pItemInfo, 0); //ItemID
				Write32(pItemInfo, 0); //ItemNum
				Write32(pItemInfo, 0); //AvailPeriod
				Write8(pItemInfo, 0); //Status
				WriteUn(pItemInfo, 0); //ObtainTime
				Write32(pItemInfo, 0); //OtherAttribute
				Write16(pItemInfo, 0); //ItemType

				len = pItemInfo - pFinalAward;
				Set16(pFinalAward, (WORD)len);
				pFinalAward += len;
			}
			Write32(pFinalAward, 0); //Coupons
			Write32(pFinalAward, 0); //GuildPoint
			Write32(pFinalAward, 0); //LuckMoney
			Write8(pFinalAward, 0); //ExtendInfoNum
			for (size_t i = 0; i < n; i++)
			{ //ExtendInfoAward
				BYTE* pExtendInfoAward = pFinalAward;
				Write16(pExtendInfoAward, 0); //len

				Write16(pExtendInfoAward, 0); //Key
				Write32(pExtendInfoAward, 0); //AddValue
				Write32(pExtendInfoAward, 0); //TotalValue
				Write8(pExtendInfoAward, 0); //ShowPriority
				Write16(pExtendInfoAward, 0); //BuyNeedScore
				Write8(pExtendInfoAward, 0); //OneMatchMaxNum

				len = pExtendInfoAward - pFinalAward;
				Set16(pFinalAward, (WORD)len);
				pFinalAward += len;
			}
			Write32(pFinalAward, 0); //SpeedCoin

			len = pFinalAward - pSign3Info;
			Set16(pSign3Info, (WORD)len);
			pSign3Info += len;
		}
		Write32(pSign3Info, 0); //SeasonBeginDate
		Write32(pSign3Info, 0); //SeasonEndDate

		len = pSign3Info - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // SignNum
	Write8(p, 0); // HasAward
	/*
	for (size_t i = 0; i < n; i++)
	{ //Award
		BYTE* pAward = p;
		Write16(pAward, 0); //len

		Write32(pAward, 0); //Exp
		Write32(pAward, 0); //Money
		Write32(pAward, 0); //SuperMoney
		Write16(pAward, 0); //ItemNum
		for (size_t i = 0; i < n; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pAward;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); //Coupons
		Write32(pAward, 0); //GuildPoint
		Write32(pAward, 0); //LuckMoney
		Write8(pAward, 0); //ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pAward;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 0); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pAward;
			Set16(pAward, (WORD)len);
			pAward += len;
		}
		Write32(pAward, 0); //SpeedCoin

		len = pAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write8(p, 0); // LabelNum

	len = p - buf;
	SendToClient(Client, 1401, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestHyperSpaceJumboGetCfg(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UCHAR CfgType = Read8(Body);
	ResponseHyperSpaceJumboGetCfg(Client, CfgType);
}

void ResponseHyperSpaceJumboGetCfg(ClientNode* Client, UCHAR CfgType)
{
	if (CfgType ==17) {
		const char* buf = "\x00\x00\x27\x11\x00\x00\x00\x00\x00\x00\x00\x03\xa9\x80\x00\x00\x00\x00\x6a\x63\x00\x00\x5e\x8e\x00\x00\x7b\x56\x00\x00\x7e\x97\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x11\x00\x00\x00\x00\x00\x11\x00\x00\x00\x10\x00\x00\x00\x06\x00\x00\x00\x10\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x22\x94\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x22\x94\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x69\x6e";
		SendToClient(Client, 24202, (BYTE*)buf, 230, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
		return;
	}
	BYTE buf[8192] = { 0 };
	BYTE* p = buf;
	size_t len = 0;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID

	Write32(p, 0); // BeginTime
	// day 20220419 修复结算问题
	Write32(p, 240000); // EndTime
	// Write32(p, 480000); //EndTime

	Write16(p, 0); // MapNum
	// Write32(p, 0); //MapIDList[]

	const int MAX_HYPER_SPACE_JUMBO_BEST_ITEM_MUM = 30;
	int BestItemIDList[MAX_HYPER_SPACE_JUMBO_BEST_ITEM_MUM] = { 27235, 24206, 31574, 32407 };
	for (size_t i = 0; i < MAX_HYPER_SPACE_JUMBO_BEST_ITEM_MUM; i++)
	{
		Write32(p, BestItemIDList[i]); // BestItemIDList[]
	}

	Write32(p, 0);		// LeftOpenBoxNum
	Write8(p, CfgType); // CfgType
	Write32(p, 0);		// Para1

	Write16(p, 17); // ValueNum
	int Values[17] = {};
	for (size_t i = 0; i < 17; i++)
	{
		Write32(p, Values[i]); // Values[]
	}

	Write16(p, 0); // MapListNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //MapList
		BYTE* pMapList = p;
		Write16(pMapList, 0); //len

		Write32(pMapList, 0); //MapID
		Write8(pMapList, 0); //Para1

		len = pMapList - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write16(p, 0); // HighBoxAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //HighBoxAward
		BYTE* pHighBoxAward = p;
		Write16(pHighBoxAward, 0); //len

		Write32(pHighBoxAward, 0); //Exp
		Write32(pHighBoxAward, 0); //Money
		Write32(pHighBoxAward, 0); //SuperMoney
		Write16(pHighBoxAward, 0); //ItemNum
		for (size_t i = 0; i < n; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pHighBoxAward;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pHighBoxAward;
			Set16(pHighBoxAward, (WORD)len);
			pHighBoxAward += len;
		}
		Write32(pHighBoxAward, 0); //Coupons
		Write32(pHighBoxAward, 0); //GuildPoint
		Write32(pHighBoxAward, 0); //LuckMoney
		Write8(pHighBoxAward, 0); //ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pHighBoxAward;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 0); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pHighBoxAward;
			Set16(pHighBoxAward, (WORD)len);
			pHighBoxAward += len;
		}
		Write32(pHighBoxAward, 0); //SpeedCoin

		len = pHighBoxAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write16(p, 0); // LowBoxAwardNum
	/*
	for (size_t i = 0; i < n; i++)
	{ //LowBoxAward
		BYTE* pLowBoxAward = p;
		Write16(pLowBoxAward, 0); //len

		Write32(pLowBoxAward, 0); //Exp
		Write32(pLowBoxAward, 0); //Money
		Write32(pLowBoxAward, 0); //SuperMoney
		Write16(pLowBoxAward, 0); //ItemNum
		for (size_t i = 0; i < n; i++)
		{ //ItemInfo
			BYTE* pItemInfo = pLowBoxAward;
			Write16(pItemInfo, 0); //len

			Write32(pItemInfo, 0); //ItemID
			Write32(pItemInfo, 0); //ItemNum
			Write32(pItemInfo, 0); //AvailPeriod
			Write8(pItemInfo, 0); //Status
			WriteUn(pItemInfo, 0); //ObtainTime
			Write32(pItemInfo, 0); //OtherAttribute
			Write16(pItemInfo, 0); //ItemType

			len = pItemInfo - pLowBoxAward;
			Set16(pLowBoxAward, (WORD)len);
			pLowBoxAward += len;
		}
		Write32(pLowBoxAward, 0); //Coupons
		Write32(pLowBoxAward, 0); //GuildPoint
		Write32(pLowBoxAward, 0); //LuckMoney
		Write8(pLowBoxAward, 0); //ExtendInfoNum
		for (size_t i = 0; i < n; i++)
		{ //ExtendInfoAward
			BYTE* pExtendInfoAward = pLowBoxAward;
			Write16(pExtendInfoAward, 0); //len

			Write16(pExtendInfoAward, 0); //Key
			Write32(pExtendInfoAward, 0); //AddValue
			Write32(pExtendInfoAward, 0); //TotalValue
			Write8(pExtendInfoAward, 0); //ShowPriority
			Write16(pExtendInfoAward, 0); //BuyNeedScore
			Write8(pExtendInfoAward, 0); //OneMatchMaxNum

			len = pExtendInfoAward - pLowBoxAward;
			Set16(pLowBoxAward, (WORD)len);
			pLowBoxAward += len;
		}
		Write32(pLowBoxAward, 0); //SpeedCoin

		len = pLowBoxAward - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	Write32(p, 0); // Para2

	Write16(p, 0); // ValueNum2
	// Write32(p, 0); //Values2[]

	Write8(p, 0); // HasYuLeJiaNianHuaInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //YuLeJiaNianHuaInfo
		BYTE* pYuLeJiaNianHuaInfo = p;
		Write16(pYuLeJiaNianHuaInfo, 0); //len

		Write32(pYuLeJiaNianHuaInfo, 0); //Mode
		Write32(pYuLeJiaNianHuaInfo, 0); //Score
		Write8(pYuLeJiaNianHuaInfo, 0); //RankRangNum
		Write32(pYuLeJiaNianHuaInfo, 0); //RankRange[]
		Write8(pYuLeJiaNianHuaInfo, 0); //RankScoreNum
		Write32(pYuLeJiaNianHuaInfo, 0); //RankScore[]
		Write8(pYuLeJiaNianHuaInfo, 0); //InRecommend
		Write8(pYuLeJiaNianHuaInfo, 0); //InDoubleScore

		len = pYuLeJiaNianHuaInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 24202, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestTimerChallenge2ndMoreHallInfo(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	ResponseTimerChallenge2ndMoreHallInfo(Client);
}
void ResponseTimerChallenge2ndMoreHallInfo(ClientNode* Client)
{
	const char* buf = "\x00\x00\x27\x11\x00\x00\x00\x0b\x00\x00\x00\x78\x00\x00\x00\x55\x00\x00\x00\x78\x00\x00\x27\x66\x00\x00\x00\x01\x00\x00\x00\x0f\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x78\x00\x14\x00\x00\x00\x05\x00\x00\x00\x01\x00\x00\x00\x05\x01\x00\x00\x00\x24\x00\x00\x14\x00\x00\x00\x0a\x00\x00\x00\x02\x00\x00\x00\x0a\x01\x00\x00\x00\x47\x00\x00\x14\x00\x00\x00\x0f\x00\x00\x00\x03\x00\x00\x00\x0f\x01\x00\x00\x00\x71\x00\x00\x14\x00\x00\x00\x14\x00\x00\x00\x02\x00\x00\x00\x14\x01\x00\x00\x00\x9a\x00\x00\x14\x00\x00\x00\x19\x00\x00\x00\x07\x00\x00\x00\x19\x01\x00\x00\x00\xc6\x00\x00\x14\x00\x00\x00\x1e\x00\x00\x00\x08\x00\x00\x00\x1e\x01\x00\x00\x00\xf7\x00\x00\x14\x00\x00\x00\x23\x00\x00\x00\x05\x00\x00\x00\x23\x01\x00\x00\x01\x29\x00\x00\x14\x00\x00\x00\x28\x00\x00\x00\x05\x00\x00\x00\x28\x01\x00\x00\x01\x50\x00\x00\x14\x00\x00\x00\x2d\x00\x00\x00\x05\x00\x00\x00\x2d\x01\x00\x00\x01\x85\x00\x00\x14\x00\x00\x00\x32\x00\x00\x00\x05\x00\x00\x00\x32\x01\x00\x00\x01\xb4\x00\x00\x14\x00\x00\x00\x37\x00\x00\x00\x05\x00\x00\x00\x37\x01\x00\x00\x01\xd7\x00\x00\x14\x00\x00\x00\x3c\x00\x00\x00\x05\x00\x00\x00\x3c\x01\x00\x00\x02\x0d\x00\x00\x14\x00\x00\x00\x41\x00\x00\x00\x05\x00\x00\x00\x41\x01\x00\x00\x02\x37\x00\x00\x14\x00\x00\x00\x46\x00\x00\x00\x05\x00\x00\x00\x46\x01\x00\x00\x02\x53\x00\x00\x14\x00\x00\x00\x4b\x00\x00\x00\x05\x00\x00\x00\x4b\x01\x00\x00\x02\x8a\x00\x00\x14\x00\x00\x00\x50\x00\x00\x00\x03\x00\x00\x00\x50\x01\x00\x00\x02\xae\x00\x00\x14\x00\x00\x00\x55\x00\x00\x00\x03\x00\x00\x00\x55\x01\x00\x00\x02\xcb\x00\x00\x14\x00\x00\x00\x5a\x00\x00\x00\x04\x00\x00\x00\x5a\x01\x00\x00\x02\xf3\x00\x00\x14\x00\x00\x00\x5f\x00\x00\x00\x04\x00\x00\x00\x5f\x01\x00\x00\x03\x19\x00\x00\x14\x00\x00\x00\x64\x00\x00\x00\x03\x00\x00\x00\x64\x01\x00\x00\x03\x38\x00\x00\x14\x00\x00\x00\x69\x00\x00\x00\x00\x00\x00\x00\x69\x01\x00\x00\x03\x62\x00\x00\x14\x00\x00\x00\x6e\x00\x00\x00\x00\x00\x00\x00\x6e\x01\x00\x00\x03\x88\x00\x00\x14\x00\x00\x00\x73\x00\x00\x00\x00\x00\x00\x00\x73\x01\x00\x00\x03\xa8\x00\x00\x14\x00\x00\x00\x78\x00\x00\x00\x00\x00\x00\x00\x78\x01\x00\x00\x03\xda\x00\x00\x14\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x05\x00\x00\x14\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x10\x00\x00\x14\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x18\x00\x00\x14\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x1e\x00\x00\x14\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x2b\x00\x00\x14\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x31\x00\x00\x14\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x3a\x00\x00\x14\x00\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x41\x00\x00\x18\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x4f\x00\x00\x03\xe2\x00\x00\x18\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x57\x00\x00\x03\xea\x00\x00\x18\x00\x00\x00\x0d\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x60\x00\x00\x03\xf3\x00\x00\x18\x00\x00\x00\x0e\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x68\x00\x00\x03\xfb\x00\x00\x14\x00\x00\x00\x10\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x7a\x00\x00\x14\x00\x00\x00\x11\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x82\x00\x00\x14\x00\x00\x00\x12\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x8b\x00\x00\x14\x00\x00\x00\x13\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\x91\x00\x00\x14\x00\x00\x00\x15\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\xa3\x00\x00\x14\x00\x00\x00\x16\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\xaa\x00\x00\x14\x00\x00\x00\x17\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\xb3\x00\x00\x14\x00\x00\x00\x18\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x00\xbf\x00\x00\x18\x00\x00\x00\x1a\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\xd2\x00\x00\x04\x0a\x00\x00\x18\x00\x00\x00\x1b\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\xde\x00\x00\x04\x15\x00\x00\x18\x00\x00\x00\x1c\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\xe8\x00\x00\x04\x20\x00\x00\x18\x00\x00\x00\x1d\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\xf3\x00\x00\x04\x2b\x00\x00\x14\x00\x00\x00\x1f\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x02\x00\x00\x14\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x0c\x00\x00\x14\x00\x00\x00\x21\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x13\x00\x00\x14\x00\x00\x00\x22\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x20\x00\x00\x14\x00\x00\x00\x24\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x30\x00\x00\x14\x00\x00\x00\x25\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x37\x00\x00\x14\x00\x00\x00\x26\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x3f\x00\x00\x14\x00\x00\x00\x27\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x48\x00\x00\x18\x00\x00\x00\x29\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x01\x59\x00\x00\x04\x36\x00\x00\x18\x00\x00\x00\x2a\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x01\x6c\x00\x00\x04\x49\x00\x00\x18\x00\x00\x00\x2b\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x01\x72\x00\x00\x04\x50\x00\x00\x18\x00\x00\x00\x2c\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x01\x7d\x00\x00\x04\x5b\x00\x00\x14\x00\x00\x00\x2e\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x8c\x00\x00\x14\x00\x00\x00\x2f\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x96\x00\x00\x14\x00\x00\x00\x30\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\x9d\x00\x00\x14\x00\x00\x00\x31\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\xa7\x00\x00\x14\x00\x00\x00\x33\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\xbd\x00\x00\x14\x00\x00\x00\x34\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\xc8\x00\x00\x14\x00\x00\x00\x35\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\xcd\x00\x00\x14\x00\x00\x00\x36\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x01\xd3\x00\x00\x18\x00\x00\x00\x38\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x01\xe1\x00\x00\x04\x69\x00\x00\x18\x00\x00\x00\x39\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x01\xed\x00\x00\x04\x74\x00\x00\x18\x00\x00\x00\x3a\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x01\xfa\x00\x00\x04\x81\x00\x00\x18\x00\x00\x00\x3b\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\x01\x00\x00\x04\x88\x00\x00\x14\x00\x00\x00\x3d\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x13\x00\x00\x14\x00\x00\x00\x3e\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x1b\x00\x00\x14\x00\x00\x00\x3f\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x25\x00\x00\x14\x00\x00\x00\x40\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x2f\x00\x00\x14\x00\x00\x00\x42\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x3c\x00\x00\x14\x00\x00\x00\x43\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x40\x00\x00\x14\x00\x00\x00\x44\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x46\x00\x00\x14\x00\x00\x00\x45\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x4f\x00\x00\x18\x00\x00\x00\x47\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\x61\x00\x00\x04\xa0\x00\x00\x18\x00\x00\x00\x48\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\x68\x00\x00\x04\xa5\x00\x00\x18\x00\x00\x00\x49\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\x77\x00\x00\x04\xb4\x00\x00\x18\x00\x00\x00\x4a\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\x80\x00\x00\x04\xbd\x00\x00\x14\x00\x00\x00\x4c\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x92\x00\x00\x14\x00\x00\x00\x4d\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\x9b\x00\x00\x14\x00\x00\x00\x4e\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\xa2\x00\x00\x14\x00\x00\x00\x4f\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\xa9\x00\x00\x14\x00\x00\x00\x51\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\xb6\x00\x00\x14\x00\x00\x00\x52\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\xba\x00\x00\x14\x00\x00\x00\x53\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\xbf\x00\x00\x14\x00\x00\x00\x54\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\xc7\x00\x00\x18\x00\x00\x00\x56\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\xd6\x00\x00\x04\xca\x00\x00\x18\x00\x00\x00\x57\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\xdb\x00\x00\x04\xd0\x00\x00\x18\x00\x00\x00\x58\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\xe4\x00\x00\x04\xd9\x00\x00\x18\x00\x00\x00\x59\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x02\xe9\x00\x00\x04\xde\x00\x00\x14\x00\x00\x00\x5b\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x02\xfa\x00\x00\x14\x00\x00\x00\x5c\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x04\x00\x00\x14\x00\x00\x00\x5d\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x09\x00\x00\x14\x00\x00\x00\x5e\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x13\x00\x00\x14\x00\x00\x00\x60\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x1d\x00\x00\x14\x00\x00\x00\x61\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x21\x00\x00\x14\x00\x00\x00\x62\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x2a\x00\x00\x14\x00\x00\x00\x63\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x33\x00\x00\x18\x00\x00\x00\x65\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\x43\x00\x00\x04\xef\x00\x00\x18\x00\x00\x00\x66\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\x4b\x00\x00\x04\xf7\x00\x00\x18\x00\x00\x00\x67\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\x53\x00\x00\x04\xff\x00\x00\x18\x00\x00\x00\x68\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\x5a\x00\x00\x05\x08\x00\x00\x14\x00\x00\x00\x6a\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x69\x00\x00\x14\x00\x00\x00\x6b\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x70\x00\x00\x14\x00\x00\x00\x6c\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x7b\x00\x00\x14\x00\x00\x00\x6d\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x7f\x00\x00\x14\x00\x00\x00\x6f\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x8c\x00\x00\x14\x00\x00\x00\x70\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x93\x00\x00\x14\x00\x00\x00\x71\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\x97\x00\x00\x14\x00\x00\x00\x72\x00\x00\x00\x00\x00\x00\x00\x00\x01\x00\x00\x03\xa0\x00\x00\x18\x00\x00\x00\x74\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\xb5\x00\x00\x05\x15\x00\x00\x18\x00\x00\x00\x75\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\xc4\x00\x00\x05\x26\x00\x00\x18\x00\x00\x00\x76\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\xcb\x00\x00\x05\x2d\x00\x00\x18\x00\x00\x00\x77\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x03\xd1\x00\x00\x05\x34\x00\x01\x00\x06\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\xc2\x20\x45\x39\x35\x30\x33\x42\x46\x39\x44\x31\x37\x30\x43\x31\x32\x42\x33\x45\x42\x44\x44\x33\x38\x37\x34\x30\x42\x41\x41\x45\x33\x45\x00\x42\x68\x74\x74\x70\x3a\x2f\x2f\x64\x6f\x77\x6e\x2e\x71\x71\x2e\x63\x6f\x6d\x2f\x71\x71\x6b\x61\x72\x74\x2f\x66\x75\x6c\x6c\x2f\x54\x69\x6d\x65\x72\x43\x68\x61\x6c\x6c\x65\x6e\x67\x65\x52\x65\x63\x6f\x72\x64\x5f\x70\x75\x74\x6f\x6e\x67\x5f\x6c\x6f\x77\x2e\x73\x70\x64\x01\x00\x46\x00\x00\x00\x00\x00\x00\x27\x3a\x00\x00\x27\x77\x00\x00\x00\x00\x00\x00\x27\x42\x00\x00\x27\x30\x00\x00\x27\x43\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\xc6\xd5\xcd\xa8\xa1\xa4\xbc\xab\xcf\xde\xcc\xf4\xd5\xbd\xd5\xdf\x00\x00\x00\xc3\x20\x36\x43\x37\x32\x33\x39\x31\x43\x35\x39\x30\x34\x30\x46\x45\x31\x46\x41\x41\x32\x45\x37\x42\x31\x30\x36\x31\x41\x31\x32\x39\x32\x00\x43\x68\x74\x74\x70\x3a\x2f\x2f\x64\x6f\x77\x6e\x2e\x71\x71\x2e\x63\x6f\x6d\x2f\x71\x71\x6b\x61\x72\x74\x2f\x66\x75\x6c\x6c\x2f\x54\x69\x6d\x65\x72\x43\x68\x61\x6c\x6c\x65\x6e\x67\x65\x52\x65\x63\x6f\x72\x64\x5f\x70\x75\x74\x6f\x6e\x67\x5f\x68\x69\x67\x68\x2e\x73\x70\x64\x01\x00\x46\x00\x00\x00\x00\x00\x00\x27\x3a\x00\x00\x27\x77\x00\x00\x00\x00\x00\x00\x27\x42\x00\x00\x27\x30\x00\x00\x27\x43\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\xc6\xd5\xcd\xa8\xa1\xa4\xbc\xab\xcf\xde\xcc\xf4\xd5\xbd\xd5\xdf\x00\x00\x00\xc3\x20\x37\x37\x34\x31\x39\x46\x38\x41\x44\x38\x42\x46\x44\x41\x44\x34\x42\x44\x37\x35\x30\x31\x42\x34\x34\x41\x46\x44\x37\x45\x31\x39\x00\x43\x68\x74\x74\x70\x3a\x2f\x2f\x64\x6f\x77\x6e\x2e\x71\x71\x2e\x63\x6f\x6d\x2f\x71\x71\x6b\x61\x72\x74\x2f\x66\x75\x6c\x6c\x2f\x54\x69\x6d\x65\x72\x43\x68\x61\x6c\x6c\x65\x6e\x67\x65\x52\x65\x63\x6f\x72\x64\x5f\x63\x68\x65\x73\x68\x65\x6e\x5f\x6c\x6f\x77\x2e\x73\x70\x64\x01\x00\x46\x00\x00\x00\x00\x00\x00\x27\x3a\x00\x00\x27\x77\x00\x00\x00\x00\x00\x00\x27\x42\x00\x00\x27\x30\x00\x00\x27\x43\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\xb3\xb5\xc9\xf1\xa1\xa4\xbc\xab\xcf\xde\xcc\xf4\xd5\xbd\xd5\xdf\x00\x00\x00\xc4\x20\x35\x46\x42\x39\x45\x30\x33\x46\x32\x33\x34\x37\x46\x34\x33\x30\x34\x32\x37\x44\x36\x46\x41\x30\x33\x36\x45\x45\x38\x31\x38\x35\x00\x44\x68\x74\x74\x70\x3a\x2f\x2f\x64\x6f\x77\x6e\x2e\x71\x71\x2e\x63\x6f\x6d\x2f\x71\x71\x6b\x61\x72\x74\x2f\x66\x75\x6c\x6c\x2f\x54\x69\x6d\x65\x72\x43\x68\x61\x6c\x6c\x65\x6e\x67\x65\x52\x65\x63\x6f\x72\x64\x5f\x63\x68\x65\x73\x68\x65\x6e\x5f\x68\x69\x67\x68\x2e\x73\x70\x64\x01\x00\x46\x00\x00\x27\x26\x00\x00\x27\x29\x00\x00\x27\x7a\x00\x00\x00\x00\x00\x00\x27\x3b\x00\x00\x27\x3d\x00\x00\x27\x3c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x10\xb3\xb5\xc9\xf1\xa1\xa4\xbc\xab\xcf\xde\xcc\xf4\xd5\xbd\xd5\xdf\x00\x00\x00\x00\x00\x00\x00\x00\x50\x0f\x01\x00\x06\x00\x00\x00\x00\x03\x03\x59\x04\x00\x84\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\x00\x42\xc1\xf7\xbb\xf0\xc1\xfa\xbe\xa7\xb8\xb3\xd3\xe8\xc8\xfc\xb3\xb5\xb7\xc9\xd0\xd0\xb5\xc4\xc4\xdc\xc1\xa6\xa3\xac\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\x35\xc3\xeb\xa3\xac\xc0\xe4\xc8\xb4\xca\xb1\xbc\xe4\x31\x35\xc3\xeb\xa3\xac\xca\xb9\xd3\xc3\xb4\xce\xca\xfd\x31\xb4\xce\x01\x00\x00\x00\x00\x00\x00\x00\x09\x01\x00\x00\x00\x00\x00\x00\x00\x13\x88\x00\x00\x3a\x98\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\x4c\x76\x32\x00\x16\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\xb5\xc4\xc0\xe4\xc8\xb4\xca\xb1\xbc\xe4\x2d\x35\xc3\xeb\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x13\x88\x00\x00\x27\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\x4c\x76\x33\x00\x16\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\xb5\xc4\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\x2b\x32\xc3\xeb\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x1b\x58\x00\x00\x27\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\x4c\x76\x34\x00\x16\xc1\xd2\xd1\xe6\xb7\xc9\xd0\xd0\xb5\xc4\xca\xb9\xd3\xc3\xb4\xce\xca\xfd\x2b\x31\xb4\xce\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x1b\x58\x00\x00\x27\x10\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x69\x00\x00\x00\xc0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x69\x32\x00\x00\x00\x0a\x00\x08\xb8\xd6\xcc\xfa\xc0\xd7\xc5\xb5\x00\x27\xbf\xc9\xd7\xb0\xb1\xb8\xa1\xbe\xc2\xfa\xb8\xc4\xa1\xbf\x41\xbc\xb6\xc8\xfc\xb3\xb5\xb8\xd6\xcc\xfa\xc0\xd7\xc5\xb5\xbd\xf8\xd0\xd0\xb4\xf3\xb4\xb3\xb9\xd8\x01\x00\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x6c\x00\x00\x00\xc2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xc5\xf6\xd7\xb2\xbc\xaf\xc6\xf8\x00\x2a\xb7\xc7\xc6\xaf\xd2\xc6\xd7\xb4\xcc\xac\xcf\xc2\xa3\xac\xc3\xbf\xb4\xce\xd7\xb2\xc7\xbd\xa3\xac\x36\x30\x25\xb8\xc5\xc2\xca\xbb\xf1\xb5\xc3\x37\x30\x25\xbc\xaf\xc6\xf8\x01\x00\x00\x00\x00\x00\xc1\x00\x03\x00\x00\x00\x00\xc3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x84\x00\x00\x00\xc5\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0c\xc6\xf0\xb2\xbd\xb6\xee\xcd\xe2\xb5\xaa\xc6\xf8\x00\x3e\xc6\xf0\xb2\xbd\xc1\xa2\xbf\xcc\xbb\xf1\xb5\xc3\xd2\xbb\xb8\xf6\xb5\xaa\xc6\xf8\xa3\xac\xc7\xd2\x31\x30\xc3\xeb\xc4\xda\xb5\xaa\xc6\xf8\xd7\xee\xb8\xdf\xcb\xd9\xb6\xc8\x2b\x32\x2e\x35\x6b\x6d\x2f\x68\xa3\xac\xb5\xaa\xc6\xf8\xb6\xaf\xc1\xa6\x2b\x35\x01\x00\x00\x00\x00\x00\xc4\x00\x03\x00\x00\x00\x00\xc6\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x67\x00\x00\x00\xc8\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xb6\xaf\xc1\xa6\xcc\xe1\xc9\xfd\x00\x25\xc3\xbf\xb4\xce\xbd\xf8\xc8\xeb\xc6\xaf\xd2\xc6\xa3\xac\x35\x30\x25\xb8\xc5\xc2\xca\x37\xc3\xeb\xc4\xda\xb5\xaa\xc6\xf8\xb6\xaf\xc1\xa6\x2b\x31\x30\x01\x00\x00\x00\x00\x00\xc7\x00\x03\x00\x00\x00\x00\xc9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x58\x04\x00\xa3\x00\x00\x00\x05\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xc4\xdc\xc1\xbf\xca\xb1\xd6\xd3\x00\x61\xc1\xf7\xbb\xf0\xc1\xfa\xbe\xa7\xca\xb9\xc8\xfc\xb5\xc0\xc9\xcf\xc3\xc5\xb1\xe4\xb3\xc9\xcc\xd8\xca\xe2\xca\xb1\xd6\xd3\xc3\xc5\xa3\xac\xcd\xa8\xb9\xfd\xba\xf3\xb4\xb3\xb9\xd8\xca\xb1\xbc\xe4\xb6\xee\xcd\xe2\x2b\x31\x2e\x35\x73\xa3\xac\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\x36\xc3\xeb\xa3\xac\xc0\xe4\xc8\xb4\xca\xb1\xbc\xe4\x31\x35\xc3\xeb\xa3\xac\xca\xb9\xd3\xc3\xb4\xce\xca\xfd\x31\xb4\xce\x01\x00\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x00\x17\x70\x00\x00\x3a\x98\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x06\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc4\xdc\xc1\xbf\xca\xb1\xd6\xd3\x4c\x76\x32\x00\x16\xc4\xdc\xc1\xbf\xca\xb1\xd6\xd3\xb5\xc4\xc0\xe4\xc8\xb4\xca\xb1\xbc\xe4\x2d\x35\xc3\xeb\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x17\x70\x00\x00\x27\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x07\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc4\xdc\xc1\xbf\xca\xb1\xd6\xd3\x4c\x76\x33\x00\x16\xc4\xdc\xc1\xbf\xca\xb1\xd6\xd3\xb5\xc4\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\x2b\x33\xc3\xeb\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x23\x28\x00\x00\x27\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc4\xdc\xc1\xbf\xca\xb1\xd6\xd3\x4c\x76\x34\x00\x16\xc4\xdc\xc1\xbf\xca\xb1\xd6\xd3\xb5\xc4\xca\xb9\xd3\xc3\xb4\xce\xca\xfd\x2b\x31\xb4\xce\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x23\x28\x00\x00\x27\x10\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x69\x00\x00\x00\xca\x00\x00\x00\x00\x00\x00\x00\x00\x00\xec\x0d\xff\xff\xff\xff\x00\x08\xca\xc9\xbb\xea\xbd\xf0\xbc\xd7\x00\x27\xbf\xc9\xd7\xb0\xb1\xb8\xa1\xbe\xc2\xfa\xb8\xc4\xa1\xbf\x54\xbc\xb6\xc8\xfc\xb3\xb5\xca\xc9\xbb\xea\xbd\xf0\xbc\xd7\xbd\xf8\xd0\xd0\xb4\xf3\xb4\xb3\xb9\xd8\x01\x00\x00\x00\x00\x00\x00\x00\x09\x01\x00\x00\x00\x00\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5a\x00\x00\x00\xcc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xd6\xfa\xc1\xa6\xc6\xf0\xc5\xdc\x00\x18\xc3\xbf\xb0\xf4\xc6\xf0\xc5\xdc\xca\xb1\xa3\xac\xb4\xb3\xb9\xd8\xca\xb1\xbc\xe4\x2b\x31\xc3\xeb\x01\x00\x00\x00\x00\x00\xcb\x00\x03\x01\x00\x00\x00\xcd\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x79\x00\x00\x00\xcf\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xc5\xe7\xc9\xe4\xbc\xd3\xb3\xc9\x00\x37\xca\xb9\xd3\xc3\xcb\xab\xc5\xe7\xa3\xa8\xba\xac\x63\x77\x77\xa1\xa2\x77\x63\x77\xa3\xa9\xba\xf3\xa3\xac\xb4\xb3\xb9\xd8\xca\xb1\xbc\xe4\x2b\x31\xc3\xeb\xa3\xac\xa3\xa8\xc3\xbf\xb0\xf4\xc9\xcf\xcf\xde\x32\xb4\xce\xa3\xa9\x01\x00\x00\x00\x00\x00\xce\x00\x03\x01\x00\x00\x00\xd0\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x64\x00\x00\x00\xd2\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xbc\xab\xcb\xd9\xcc\xe1\xc9\xfd\x00\x22\xc3\xbf\x33\xb4\xce\xca\xb9\xd3\xc3\xb5\xaa\xc6\xf8\xa3\xac\x38\xc3\xeb\xc4\xda\xc6\xbd\xc5\xdc\xbc\xab\xcb\xd9\xc4\xdc\xc1\xa6\x2b\x38\x01\x00\x00\x00\x00\x00\xd1\x00\x03\x01\x00\x00\x00\xd3\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x03\x33\x04\x00\x97\x00\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xc1\xfa\xbe\xa7\xb4\xc5\xce\xfc\x00\x55\xc0\xa9\xb4\xf3\xb5\xc0\xbe\xdf\xca\xb0\xc8\xa1\xb7\xb6\xce\xa7\xb2\xa2\xc7\xd2\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\xc4\xda\xc3\xbf\xc3\xeb\xd4\xf6\xbc\xd3\x32\x30\x25\xb5\xaa\xc6\xf8\xa3\xac\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\x36\xc3\xeb\xa3\xac\xc0\xe4\xc8\xb4\xca\xb1\xbc\xe4\x31\x35\xc3\xeb\xa3\xac\xca\xb9\xd3\xc3\xb4\xce\xca\xfd\x31\xb4\xce\x01\x00\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x00\x17\x70\x00\x00\x3a\x98\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x0a\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc1\xfa\xbe\xa7\xb4\xc5\xce\xfc\x4c\x76\x32\x00\x16\xc1\xfa\xbe\xa7\xb4\xc5\xce\xfc\xb5\xc4\xc0\xe4\xc8\xb4\xca\xb1\xbc\xe4\x2d\x35\xc3\xeb\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x17\x70\x00\x00\x27\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x0b\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc1\xfa\xbe\xa7\xb4\xc5\xce\xfc\x4c\x76\x33\x00\x16\xc1\xfa\xbe\xa7\xb4\xc5\xce\xfc\xb5\xc4\xb3\xd6\xd0\xf8\xca\xb1\xbc\xe4\x2b\x33\xc3\xeb\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x23\x28\x00\x00\x27\x10\x01\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5b\x00\x00\x00\x0c\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x0b\xc1\xfa\xbe\xa7\xb4\xc5\xce\xfc\x4c\x76\x34\x00\x16\xc1\xfa\xbe\xa7\xb4\xc5\xce\xfc\xb5\xc4\xca\xb9\xd3\xc3\xb4\xce\xca\xfd\x2b\x31\xb4\xce\x01\x00\x00\x00\x00\x00\x00\x00\x03\x00\x00\x00\x00\x00\x00\x00\x00\x23\x28\x00\x00\x27\x10\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00\x04\x00\x5d\x00\x00\x00\xd4\x00\x00\x00\x00\x00\x00\x00\x00\x00\x43\xca\x00\x00\x00\x0a\x00\x06\xcc\xec\xbb\xfa\xcf\xdf\x00\x1d\xbf\xc9\xd7\xb0\xb1\xb8\x53\xbc\xb6\xc8\xfc\xb3\xb5\xcc\xec\xbb\xfa\xcf\xdf\xbd\xf8\xd0\xd0\xb4\xf3\xb4\xb3\xb9\xd8\x01\x00\x00\x00\x00\x00\x00\x00\x09\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x5e\x00\x00\x00\xd6\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xbc\xaf\xc6\xf8\xd4\xf6\xc7\xbf\x00\x1c\xc3\xbf\xb4\xce\xbd\xf8\xc8\xeb\xc6\xaf\xd2\xc6\xa3\xac\xc1\xa2\xbf\xcc\xbb\xf1\xb5\xc3\x38\x25\xbc\xaf\xc6\xf8\x01\x00\x00\x00\x00\x00\xd5\x00\x03\x00\x00\x00\x00\xd7\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x58\x00\x00\x00\xd9\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x07\x77\x63\x77\xbc\xab\xcb\xd9\x00\x17\xca\xb9\xd3\xc3\xb5\xaa\xc6\xf8\xbc\xb4\xbf\xc9\xb4\xef\xb5\xbd\x77\x63\x77\xcb\xd9\xb6\xc8\x01\x00\x00\x00\x00\x00\xd8\x00\x03\x00\x00\x00\x00\xda\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x74\x00\x00\x00\xdc\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\xb5\xaa\xc6\xf8\xc7\xbf\xbb\xaf\x00\x32\xbb\xf1\xb5\xc3\xb5\xaa\xc6\xf8\xba\xf3\xa3\xac\x32\x35\x25\xb5\xc4\xb8\xc5\xc2\xca\xb5\xaa\xc6\xf8\xd7\xee\xb8\xdf\xcb\xd9\xb6\xc8\x2b\x32\x2e\x35\x6b\x6d\x2f\x68\xa3\xac\xb3\xd6\xd0\xf8\x36\xc3\xeb\x01\x00\x00\x00\x00\x00\xdb\x00\x03\x00\x00\x00\x00\xdd\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x01\x02\x01\x00\x12\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x00\x00\x05\x00\x00\x00\x06\x00\x00\x00\x07\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\x0b\x00\x00\x00\x0d\x00\x00\x00\x0e\x00\x00\x00\x0f\x00\x00\x00\x12\x00\x00\x00\x13\x00\x00\x00\x11\x00\x00\x00\x16\x00\x00\x00\x17\x00\x00\x00\x18\x02\xa8\x00\x28\x00\x13\x00\x00\x00\x0a\x03\x00\x00\x00\x01\x00\x00\x00\x02\x00\x00\x00\x03\x00\x13\x00\x00\x00\x0b\x03\x00\x00\x00\x05\x00\x00\x00\x06\x00\x00\x00\x07\x00\x13\x00\x00\x00\x0c\x03\x00\x00\x00\x09\x00\x00\x00\x0a\x00\x00\x00\x0b\x00\x1f\x00\x00\x00\x0d\x06\x00\x00\x00\x0d\x00\x00\x00\x0e\x00\x00\x00\x0f\x00\x00\x00\x11\x00\x00\x00\x12\x00\x00\x00\x13\x00\x13\x00\x00\x00\x0e\x03\x00\x00\x00\x16\x00\x00\x00\x17\x00\x00\x00\x18\x00\x13\x00\x00\x00\x19\x03\x00\x00\x00\x19\x00\x00\x00\x1a\x00\x00\x00\x1b\x00\x1f\x00\x00\x00\x1a\x06\x00\x00\x00\x1d\x00\x00\x00\x1e\x00\x00\x00\x1f\x00\x00\x00\x21\x00\x00\x00\x22\x00\x00\x00\x23\x00\x13\x00\x00\x00\x1b\x03\x00\x00\x00\x25\x00\x00\x00\x26\x00\x00\x00\x27\x00\x13\x00\x00\x00\x1c\x03\x00\x00\x00\x2a\x00\x00\x00\x2b\x00\x00\x00\x2c\x00\x13\x00\x00\x00\x1d\x03\x00\x00\x00\x2e\x00\x00\x00\x2f\x00\x00\x00\x30\x00\x07\x00\x00\x00\x28\x00\x00\x1f\x00\x00\x00\x29\x06\x00\x00\x00\x32\x00\x00\x00\x33\x00\x00\x00\x34\x00\x00\x00\x37\x00\x00\x00\x38\x00\x00\x00\x39\x00\x13\x00\x00\x00\x2a\x03\x00\x00\x00\x3a\x00\x00\x00\x3b\x00\x00\x00\x3c\x00\x13\x00\x00\x00\x2b\x03\x00\x00\x00\x3e\x00\x00\x00\x3f\x00\x00\x00\x40\x00\x13\x00\x00\x00\x2c\x03\x00\x00\x00\x43\x00\x00\x00\x44\x00\x00\x00\x45\x00\x07\x00\x00\x00\x37\x00\x00\x13\x00\x00\x00\x38\x03\x00\x00\x00\x46\x00\x00\x00\x47\x00\x00\x00\x48\x00\x13\x00\x00\x00\x39\x03\x00\x00\x00\x49\x00\x00\x00\x4a\x00\x00\x00\x4b\x00\x13\x00\x00\x00\x3a\x03\x00\x00\x00\x4c\x00\x00\x00\x4d\x00\x00\x00\x4e\x00\x13\x00\x00\x00\x3b\x03\x00\x00\x00\x4f\x00\x00\x00\x50\x00\x00\x00\x51\x00\x07\x00\x00\x00\x46\x00\x00\x13\x00\x00\x00\x47\x03\x00\x00\x00\x5e\x00\x00\x00\x5f\x00\x00\x00\x60\x00\x13\x00\x00\x00\x48\x03\x00\x00\x00\x61\x00\x00\x00\x62\x00\x00\x00\x63\x00\x13\x00\x00\x00\x49\x03\x00\x00\x00\x64\x00\x00\x00\x65\x00\x00\x00\x66\x00\x13\x00\x00\x00\x4a\x03\x00\x00\x00\x67\x00\x00\x00\x68\x00\x00\x00\x69\x00\x13\x00\x00\x00\x55\x03\x00\x00\x00\x6a\x00\x00\x00\x6b\x00\x00\x00\x6c\x00\x07\x00\x00\x00\x56\x00\x00\x13\x00\x00\x00\x57\x03\x00\x00\x00\x6d\x00\x00\x00\x6e\x00\x00\x00\x6f\x00\x13\x00\x00\x00\x58\x03\x00\x00\x00\x70\x00\x00\x00\x71\x00\x00\x00\x72\x00\x13\x00\x00\x00\x59\x03\x00\x00\x00\x73\x00\x00\x00\x74\x00\x00\x00\x75\x00\x07\x00\x00\x00\x64\x00\x00\x13\x00\x00\x00\x65\x03\x00\x00\x00\x82\x00\x00\x00\x83\x00\x00\x00\x84\x00\x13\x00\x00\x00\x66\x03\x00\x00\x00\x85\x00\x00\x00\x86\x00\x00\x00\x87\x00\x07\x00\x00\x00\x67\x00\x00\x07\x00\x00\x00\x68\x00\x00\x0f\x00\x00\x00\x73\x02\x00\x00\x00\x88\x00\x00\x00\x91\x00\x0f\x00\x00\x00\x74\x02\x00\x00\x00\x92\x00\x00\x00\x93\x00\x07\x00\x00\x00\x75\x00\x00\x0f\x00\x00\x00\x76\x02\x00\x00\x00\x8f\x00\x00\x00\x90\x00\x07\x00\x00\x00\x77\x00\x09\x00\x00";
	SendToClient(Client, 24258, (BYTE*)buf, 6682, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
/*void ResponseTimerChallenge2ndMoreHallInfo(ClientNode* Client)
{
	BYTE buf[8192] = { 0 };
	BYTE* p = buf;
	size_t len = 0;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID

	Write16(p, 11); // ValueNum
	int Values[11] = {};
	for (size_t i = 0; i < 11; i++)
	{
		Write32(p, Values[i]); // Values[]
	}

	BYTE* pTimerChallenge2ndLevelAwardInfoNum = p;
	Write16(p, 0);
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\TimerChallenge2ndLevelAwardInfo-QQSpeed.yml");
		USHORT Num = (USHORT)Config.size();
		for (USHORT i = 0; i < Num; i++)
		{ // TimerChallenge2ndLevelAwardInfo
			BYTE* pTimerChallenge2ndLevelAwardInfo = p;
			Write16(pTimerChallenge2ndLevelAwardInfo, 0); // len

			YAML::Node TimerChallenge2ndLevelAwardInfo = Config[i];
			Write32(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["Level"].as<int>());
			Write32(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["JumpLevel"].as<int>());
			Write32(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["BoxFlag"].as<int>());

			YAML::Node Checkpoint = TimerChallenge2ndLevelAwardInfo["Checkpoint"];
			BYTE CheckpointNum = (BYTE)Checkpoint.size();
			Write8(pTimerChallenge2ndLevelAwardInfo, CheckpointNum); // CheckpointNum
			for (BYTE i = 0; i < CheckpointNum; i++)
			{
				Write32(pTimerChallenge2ndLevelAwardInfo, Checkpoint[i].as<int>()); // CheckpointIdx[]
			}

			Write8(pTimerChallenge2ndLevelAwardInfo, TimerChallenge2ndLevelAwardInfo["IsDragonGate"].as<int>());

			len = pTimerChallenge2ndLevelAwardInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Set16(pTimerChallenge2ndLevelAwardInfoNum, Num);
	}
	catch (const std::exception&)
	{
		return;
	}

	Write8(p, 1); // HasDoorCfg
	{			  // DoorCfg
		BYTE* pDoorCfg = p;
		Write16(pDoorCfg, 0); // len

		Write32(pDoorCfg, 0); // DoorPosNum
		/*
		for (size_t i = 0; i < n; i++)
		{ //DoorPosInfo
			BYTE* pDoorPosInfo = pDoorCfg;
			Write16(pDoorPosInfo, 0); //len

			Write32(pDoorPosInfo, 0); //Level
			Write8(pDoorPosInfo, 0); //DoorPosDetailNum
			for (size_t i = 0; i < n; i++)
			{ //DoorPosDetailInfo
				BYTE* pDoorPosDetailInfo = pDoorPosInfo;
				Write16(pDoorPosDetailInfo, 0); //len

				Write32(pDoorPosDetailInfo, 0); //Checkpoint
				Write32(pDoorPosDetailInfo, 0); //AxisX
				Write32(pDoorPosDetailInfo, 0); //AxisY
				Write32(pDoorPosDetailInfo, 0); //AxisZ
				Write32(pDoorPosDetailInfo, 0); //X
				Write32(pDoorPosDetailInfo, 0); //Y
				Write32(pDoorPosDetailInfo, 0); //Z

				len = pDoorPosDetailInfo - pDoorPosInfo;
				Set16(pDoorPosInfo, (WORD)len);
				pDoorPosInfo += len;
			}

			len = pDoorPosInfo - pDoorCfg;
			Set16(pDoorCfg, (WORD)len);
			pDoorCfg += len;
		}
		

		len = pDoorCfg - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write32(p, 0); // AllowJumpLevelPerPlayer

	Write8(p, 0); // ShadowNpcNum
	
	for (size_t i = 0; i < 0; i++)
	{ //TimerChallengeShadowNpcInfo
		BYTE* pTimerChallengeShadowNpcInfo = p;
		Write16(pTimerChallengeShadowNpcInfo, 0); //len

		Write8(pTimerChallengeShadowNpcInfo, 0); //FileMd5Len
		Write16(pTimerChallengeShadowNpcInfo, 0); //Len
		Write8(pTimerChallengeShadowNpcInfo, 0); //HasNPCItem
		for (size_t i = 0; i < n; i++)
		{ //NPCItemInfo
			BYTE* pNPCItemInfo = pTimerChallengeShadowNpcInfo;
			Write16(pNPCItemInfo, 0); //len

			Write32(pNPCItemInfo, 0); //RoleID
			Write32(pNPCItemInfo, 0); //HairID
			Write32(pNPCItemInfo, 0); //FaceID
			Write32(pNPCItemInfo, 0); //GlassID
			Write32(pNPCItemInfo, 0); //CoatID
			Write32(pNPCItemInfo, 0); //GloveID
			Write32(pNPCItemInfo, 0); //TrousersID
			Write32(pNPCItemInfo, 0); //PersonarID
			Write32(pNPCItemInfo, 0); //TattooID
			Write32(pNPCItemInfo, 0); //WingID
			Write32(pNPCItemInfo, 0); //DecoratorID
			Write32(pNPCItemInfo, 0); //KartID
			Write32(pNPCItemInfo, 0); //ColorID
			Write32(pNPCItemInfo, 0); //TrackID
			Write32(pNPCItemInfo, 0); //EffectID
			Write32(pNPCItemInfo, 0); //BrandID
			Write32(pNPCItemInfo, 0); //TailID

			len = pNPCItemInfo - pTimerChallengeShadowNpcInfo;
			Set16(pTimerChallengeShadowNpcInfo, (WORD)len);
			pTimerChallengeShadowNpcInfo += len;
		}
		Write16(pTimerChallengeShadowNpcInfo, 0); //BossNameLen
		Write16(pTimerChallengeShadowNpcInfo, 0); //TimeNum
		Write(pTimerChallengeShadowNpcInfo, 0); //LevTimeCfg[]

		len = pTimerChallengeShadowNpcInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	

	BYTE* pBuffNum = p;
	Write8(p, 0); // BuffNum;
	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\TimerChallengeOneBuffInfo-QQSpeed.yml");
		BYTE Num = (BYTE)Config.size();
		for (BYTE i = 0; i < Num; i++)
		{ // TimerChallengeOneBuffInfo
			BYTE* pTimerChallengeOneBuffInfo = p;
			Write16(pTimerChallengeOneBuffInfo, 0); // len

			YAML::Node TimerChallengeOneBuffInfo = Config[i];
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffId"].as<int>());
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["Prob"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffType"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffLev"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["ReduceDiscount"].as<int>());
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["KartId"].as<int>());
			Write32(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["DailyExpKartTimeLimit"].as<int>());

			std::string str = TimerChallengeOneBuffInfo["BuffName"].as<std::string>();
			len = str.length();
			Write16(pTimerChallengeOneBuffInfo, (WORD)len); // BuffNameLen
			memcpy(pTimerChallengeOneBuffInfo, str.c_str(), len);
			pTimerChallengeOneBuffInfo += len;

			str = TimerChallengeOneBuffInfo["BuffDes"].as<std::string>();
			len = str.length();
			Write16(pTimerChallengeOneBuffInfo, (WORD)len); // BuffDesLen
			memcpy(pTimerChallengeOneBuffInfo, str.c_str(), len);
			pTimerChallengeOneBuffInfo += len;

			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["Icon"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["BuffShape"].as<int>());
			Write8(pTimerChallengeOneBuffInfo, TimerChallengeOneBuffInfo["IsCanLevUp"].as<int>());

			len = pTimerChallengeOneBuffInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Set8(pBuffNum, Num);
	}
	catch (const std::exception&)
	{
		return;
	}

	Write32(p, 3); // CurrentActiveValue

	Write8(p, 1); // GotBuffNum

	{ // TimerChallenge3PlayerHasGotBuffInfo
		BYTE* pTimerChallenge3PlayerHasGotBuffInfo = p;
		Write16(pTimerChallenge3PlayerHasGotBuffInfo, 0); // len

		Write32(pTimerChallenge3PlayerHasGotBuffInfo, 116); // BuffId
		Write8(pTimerChallenge3PlayerHasGotBuffInfo, 1);	// BuffSatus
		Write8(pTimerChallenge3PlayerHasGotBuffInfo, 1);	// Position
		Write8(pTimerChallenge3PlayerHasGotBuffInfo, 100);	// HasExpTime

		len = pTimerChallenge3PlayerHasGotBuffInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}

	Write8(p, 80); // TriggerProb
	Write8(p, 15); // NoTriggerN
	Write8(p, 1);  // ASDSwitch
	{			   // RefitInfo
		BYTE* pRefitInfo = p;
		Write16(pRefitInfo, 0); // len

		Write8(pRefitInfo, 29); // SpeedRefitStar
		Write8(pRefitInfo, 29); // JetRefitStar
		Write8(pRefitInfo, 29); // SJetRefitStar
		Write8(pRefitInfo, 29); // AccuRefitStar

		len = pRefitInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	len = p - buf;
	SendToClient(Client, 24258, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}*/

void RequestPreGetCommonBoxAward(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);

	USHORT Type = Read16(Body);
	int AwardID = Read32(Body);
	UCHAR IsShowAllAward = Read8(Body);
	int EchoClient = Read32(Body);
	ResponsePreGetCommonBoxAward(Client, Type, AwardID, IsShowAllAward, EchoClient);
}

void ResponsePreGetCommonBoxAward(ClientNode *Client, USHORT Type, int AwardID, UCHAR IsShowAllAward, int EchoClient)
{
	BYTE buf[8192]; BYTE* p = buf; size_t len;

	Write32(p, Client->Uin); //Uin
	Write16(p, 0); //ResultID
	Write16(p, Type); //Type
	{ //AwardCfgArray
		BYTE* pAwardCfgArray = p;
		Write16(pAwardCfgArray, 0); //len

		Write8(pAwardCfgArray, 1); //ScoreAwardNum
		for (size_t i = 0; i < 1; i++)
		{ //ScoreAwardCfg
			BYTE* pScoreAwardCfg = pAwardCfgArray;
			Write16(pScoreAwardCfg, 0); //len

			Write8(pScoreAwardCfg, 1); //Type
			Write32(pScoreAwardCfg, 0); //StartScore
			{ //AwardInfo
				BYTE* pAwardInfo = pScoreAwardCfg;
				Write16(pAwardInfo, 0); //len

				Write32(pAwardInfo, 0); //Exp
				Write32(pAwardInfo, 0); //Money
				Write32(pAwardInfo, 0); //SuperMoney
				Write16(pAwardInfo, 0); //ItemNum
				for (size_t i = 0; i < 0; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pAwardInfo;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					Write32(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pAwardInfo;
					Set16(pAwardInfo, (WORD)len);
					pAwardInfo += len;
				}

				Write32(pAwardInfo, 0); //Coupons
				Write32(pAwardInfo, 0); //GuildPoint
				Write32(pAwardInfo, 0); //LuckMoney

				Write8(pAwardInfo, 0); //ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pAwardInfo;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pAwardInfo;
					Set16(pAwardInfo, (WORD)len);
					pAwardInfo += len;
				}

				Write32(pAwardInfo, 0); //SpeedCoin

				len = pAwardInfo - pScoreAwardCfg;
				Set16(pScoreAwardCfg, (WORD)len);
				pScoreAwardCfg += len;
			}
			Write8(pScoreAwardCfg, 0); //Status
			Write16(pScoreAwardCfg, 0); //ResultID
			Write16(pScoreAwardCfg, 0); //HasMoreInfo
			/*
			for (size_t i = 0; i < n; i++)
			{ //MoreInfo
				BYTE* pMoreInfo = pScoreAwardCfg;
				Write16(pMoreInfo, 0); //len

				Write16(pMoreInfo, 0); //AlreayGetNum
				Write16(pMoreInfo, 0); //MaxGetNum
				Write32(pMoreInfo, 0); //ItemID
				Write32(pMoreInfo, 0); //TimeStart
				Write32(pMoreInfo, 0); //TimeEnd
				Write32(pMoreInfo, 0); //NextAddNumLeftTime
				Write8(pMoreInfo, 0); //Num
				for (size_t i = 0; i < n; i++)
				{ //ComsumeItemArray
					BYTE* pComsumeItemArray = pMoreInfo;
					Write16(pComsumeItemArray, 0); //len

					Write32(pComsumeItemArray, 0); //ItemID
					Write32(pComsumeItemArray, 0); //StartScore
					Write32(pComsumeItemArray, 0); //DiscountStartScore

					len = pComsumeItemArray - pMoreInfo;
					Set16(pMoreInfo, (WORD)len);
					pMoreInfo += len;
				}
				Write8(pMoreInfo, 0); //ClientParaNum
				Write32(pMoreInfo, 0); //ClientParas[]
				Write32(pMoreInfo, 0); //DiscountTimeStart
				Write32(pMoreInfo, 0); //DiscountTimeEnd
				Write8(pMoreInfo, 0); //DiscountStatus
				Write32(pMoreInfo, 0); //LimitPara
				Write8(pMoreInfo, 0); //ItemExchangeType
				Write8(pMoreInfo, 0); //ItemLabelType
				Write16(pMoreInfo, 0); //ShowPriority
				Write8(pMoreInfo, 0); //Para0
				Write32(pMoreInfo, 0); //LastUpdateTime

				len = pMoreInfo - pScoreAwardCfg;
				Set16(pScoreAwardCfg, (WORD)len);
				pScoreAwardCfg += len;
			}
			*/

			Write32(pScoreAwardCfg, 0); //PosKey
			Write8(pScoreAwardCfg, 0); //AwardIdx
			Write8(pScoreAwardCfg, 0); //LabelStatus

			Write8(pScoreAwardCfg, 0); //ExtraAwardNum
			for (size_t i = 0; i < 0; i++)
			{ //ExtraAwardInfo
				BYTE* pExtraAwardInfo = pScoreAwardCfg;
				Write16(pExtraAwardInfo, 0); //len

				Write32(pExtraAwardInfo, 0); //Exp
				Write32(pExtraAwardInfo, 0); //Money
				Write32(pExtraAwardInfo, 0); //SuperMoney
				Write16(pExtraAwardInfo, 0); //ItemNum
				for (size_t i = 0; i < 0; i++)
				{ //ItemInfo
					BYTE* pItemInfo = pExtraAwardInfo;
					Write16(pItemInfo, 0); //len

					Write32(pItemInfo, 0); //ItemID
					Write32(pItemInfo, 0); //ItemNum
					Write32(pItemInfo, 0); //AvailPeriod
					Write8(pItemInfo, 0); //Status
					Write32(pItemInfo, 0); //ObtainTime
					Write32(pItemInfo, 0); //OtherAttribute
					Write16(pItemInfo, 0); //ItemType

					len = pItemInfo - pExtraAwardInfo;
					Set16(pExtraAwardInfo, (WORD)len);
					pExtraAwardInfo += len;
				}
				Write32(pExtraAwardInfo, 0); //Coupons
				Write32(pExtraAwardInfo, 0); //GuildPoint
				Write32(pExtraAwardInfo, 0); //LuckMoney

				Write8(pExtraAwardInfo, 0); //ExtendInfoNum
				for (size_t i = 0; i < 0; i++)
				{ //ExtendInfoAward
					BYTE* pExtendInfoAward = pExtraAwardInfo;
					Write16(pExtendInfoAward, 0); //len

					Write16(pExtendInfoAward, 0); //Key
					Write32(pExtendInfoAward, 0); //AddValue
					Write32(pExtendInfoAward, 0); //TotalValue
					Write8(pExtendInfoAward, 0); //ShowPriority
					Write16(pExtendInfoAward, 0); //BuyNeedScore
					Write8(pExtendInfoAward, 0); //OneMatchMaxNum

					len = pExtendInfoAward - pExtraAwardInfo;
					Set16(pExtraAwardInfo, (WORD)len);
					pExtraAwardInfo += len;
				}

				Write32(pExtraAwardInfo, 0); //SpeedCoin

				len = pExtraAwardInfo - pScoreAwardCfg;
				Set16(pScoreAwardCfg, (WORD)len);
				pScoreAwardCfg += len;
			}

			len = pScoreAwardCfg - pAwardCfgArray;
			Set16(pAwardCfgArray, (WORD)len);
			pAwardCfgArray += len;
		}
		len = pAwardCfgArray - p;
		Set16(p, (WORD)len);
		p += len;
	}
	Write32(p, 0); //AwardID
	Write32(p, 0); //NextClearLeftTime
	Write8(p, 0); //HasMoreInfo
	Write32(p, 0); //Score
	Write16(p, 0); //LeftGetAwardNum
	Write8(p, 0); //Status
	Write8(p, 0); //EnableMergeSameItem
	Write32(p, 0); //TimeStart
	Write32(p, 0); //TimeEnd
	Write32(p, 0); //SCPara
	Write8(p, 0); //IsShowAllAward
	Write16(p, 0); //ValueNum
	//Write32(p, 0); //Values[]
	Write32(p, 0); //EchoClient

	len = p - buf;
	SendToClient(Client, 24465, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);

}
void NotifyMuchPlayerGameBegin(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 1);			   // BaseGameMode
	Write16(p, 1);			   // SubGameMode
	Write32(p, Client->MapID); // MapID
	// Write8(p, 0); //MapHash[]
	memset(p, 0, 32);
	p += 32;
	WriteUn(p, Client->GameID); // GameID
	Write8(p, 1);				// SyncCarNum
	if (Client->SkinID > 0 && Client->KartID == Client->OrgKartId)
	{
		if (getGameConfig()["Global"]["SkillStoneUin"])
		{
			INT Uin = getGameConfig()["Global"]["SkillStoneUin"].as<INT>();
			if (Uin == -1)
			{
				NotifyUpdateKartSkillStoneInfo3(Client, Client->KartID, Client->SkinID);
			}
			else if (Client->Uin == Uin)
			{
				NotifyUpdateKartSkillStoneInfo3(Client, Client->KartID, Client->SkinID);
			}
			else
			{
				NotifyUpdateKartSkillStoneInfo2(Client, Client->KartID, Client->SkinID);
			}
		}
		else
		{
			NotifyUpdateKartSkillStoneInfo2(Client, Client->KartID, Client->SkinID);
		}
	}
	for (size_t i = 0; i < 1; i++)
	{ // SyncCar
		BYTE *pSyncCar = p;
		Write16(pSyncCar, 0); // len

		WriteUn(pSyncCar, Client->Uin);	   // PlayerUin
		Write16(pSyncCar, Client->KartID); // CurCarID
		Write8(pSyncCar, 0);			   // HasStoneInfo
		for (size_t i = 0; i < 1; i++)
		{ // STKartStoneGrooveInfo
			BYTE *pSTKartStoneGrooveInfo = pSyncCar;
			Write16(pSTKartStoneGrooveInfo, 0); // len

			Write32(pSTKartStoneGrooveInfo, 0); // KartID
			Write32(pSTKartStoneGrooveInfo, 0); // StoneGrooveNum
			for (size_t i = 0; i < 1; i++)
			{ // StoneGrooveInfo
				BYTE *pStoneGrooveInfo = pSTKartStoneGrooveInfo;
				Write16(pStoneGrooveInfo, 0); // len

				Write32(pStoneGrooveInfo, 0); // StoneUseOccaType
				Write32(pStoneGrooveInfo, 0); // SkillStoneID

				len = pStoneGrooveInfo - pSTKartStoneGrooveInfo;
				Set16(pSTKartStoneGrooveInfo, (WORD)len);
				pSTKartStoneGrooveInfo += len;
			}

			len = pSTKartStoneGrooveInfo - pSyncCar;
			Set16(pSyncCar, (WORD)len);
			pSyncCar += len;
		}

		len = pSyncCar - p;
		Set16(p, (WORD)len);
		p += len;
	}
	WriteUn(p, 8000); // NormalLoadTime
	Write16(p, 2);	  // TotalMapRound
	Write8(p, 1);	  // CurrentPlayers
	Write16(p, 0);	  // RaceTrackOrders[]
	Write16(p, 0);	  // NPCTrackOrders[]

	len = p - buf;
	SendToClient(Client, 1720, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestAddToMatchQueue(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	InitClientGameArrt(Client);
	BYTE *p = Body;
	

	UINT uin = Read32(p);
	UINT time = Read32(p);
	Client->MapID = 0;
	USHORT matchQueueType = Read16(p); // 1 普通匹配 12跳跳模式
	UINT para1 = Read32(p);
	UCHAR IsAllArea = Read8(p);
	UCHAR paraNum = Read8(p);
	
	//旧代码
	for (size_t i = 0; i < paraNum; i++)
	{
		UINT para = Read32(p);
	}
	int MapID = Read32(p);
	Client->MapID = MapID;
	if (matchQueueType == 0x8)
	{
		Client->matchQueueType = matchQueueType;
		ResponseStartTimerChallenge2(Client);
		return;
		//龙晶大闯关
	}
	if (matchQueueType == 0xa || matchQueueType == 0x0 || matchQueueType == 1)
	{
		Client->matchQueueType = matchQueueType;
		ResponseStartBorderGame(Client, matchQueueType);
		return;
		//排位 //边境 //匹配
	}
	ClientNode *Dst = GetClient(10002);
	Client->BaseGameMode = 1;
	Client->SubGameMode = 1;
	if (matchQueueType == 12)
	{
		Client->BaseGameMode = 17;
	}
	if (Client->BaseGameMode == 46)
	{
		Client->MapID = 431;
	}
	else
	{
		Client->MapID;
	}

	ResponseAddToMatchQueue(Client, matchQueueType);
	Client->matchQueueType = matchQueueType;
	delay(5);
	if (Dst)
	{ // 多人
		Dst->MapID = 0;
		ResponseAddToMatchQueue(Dst, matchQueueType);
		NotifyMatchResult(Dst, matchQueueType);
		NotifyMatchResult(Client, matchQueueType);
		RequestCreateRoomMatch(Client);
		Dst->RoomID = Client->RoomID;
		RequestEnterRoomMatch(Dst);
		RequestStartGameMatch(Client);
	}
	else
	{ // 单人
		NotifyMatchResult(Client, matchQueueType);
		delay(2);
		RequestCreateRoomMatch(Client);
		RequestStartGameMatch(Client);
	}
}

void ResponseAddToMatchQueue(ClientNode *Client, USHORT matchQueueType)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 0);				// resultId
	Write8(p, 0);				// reasonLen
	Write16(p, matchQueueType); // matchQueueType
	Write32(p, 0);				// Para0
	Write8(p, 1);				// IsAllArea
	Write32(p, Client->MapID);	// MapID

	len = p - buf;
	SendToClient(Client, 20079, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifyMatchResult(ClientNode* Client, USHORT matchQueueType)
{
	BYTE buf[8192] = { 0 };
	BYTE* p = buf;
	size_t len = 0;

	Write16(p, 0); // resultId
	Write8(p, 0);  // reasonLen

	Write16(p, matchQueueType); // matchQueueType
	Write8(p, 0);				// hasEnterInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //enterInfo
		BYTE* penterInfo = p;
		Write16(penterInfo, 0); //len

		{ //addrInfo
			BYTE* paddrInfo = penterInfo;
			Write16(paddrInfo, 0); //len

			WriteUn(paddrInfo, 0); //svrId
			WriteUn(paddrInfo, 0); //svrIP
			WriteUn(paddrInfo, 0); //tcpPortNum
			WriteUn(paddrInfo, 0); //tcpPort[]
			WriteUn(paddrInfo, 0); //udpPortNum
			WriteUn(paddrInfo, 0); //udpPort[]
			Write8(paddrInfo, 0); //HasMultiIPInfo
			for (size_t i = 0; i < n; i++)
			{ //MultiIPInfo
				BYTE* pMultiIPInfo = paddrInfo;
				Write16(pMultiIPInfo, 0); //len

				Write8(pMultiIPInfo, 0); //IPCount
				Write32(pMultiIPInfo, 0); //ServerIP[]
				Write8(pMultiIPInfo, 0); //DomainLen

				len = pMultiIPInfo - paddrInfo;
				Set16(paddrInfo, (WORD)len);
				paddrInfo += len;
			}

			len = paddrInfo - penterInfo;
			Set16(penterInfo, (WORD)len);
			penterInfo += len;
		}
		WriteUn(penterInfo, 0); //roomId
		Write16(penterInfo, 0); //SceneID

		len = penterInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/
	Write32(p, Client->MapID); // mapID
	Write16(p, 1);			   // MatchedRoomType
	Write16(p, 1);			   // TeamMatchType
	Write8(p, 0);			   // PVPType
	Write32(p, 0);			   // PVPKey
	Write32(p, 12345678);	   // GameSeqIDHigh
	Write32(p, 87654321);	   // GameSeqIDLow
	Write16(p, 0);			   // ForceTransCount
	// Write32(p, 0); //ForceTransMsgID[]
	Write16(p, 1); // PlayerMatchingType
	Write8(p, 0);  // paraNum
	// Write32(p, 0); //paraList[]

	len = p - buf;
	SendToClient(Client, 20081, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void RequestCancelMatchQueue(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	BYTE *p = Body;
	WORD len = {0};

	UINT uin = Read32(p);
	UINT time = Read32(p);
	UINT matchQueueType = Read32(p);
	UCHAR IsAllArea = Read8(p);
	Client->matchQueueType = 0;
	ResponseCancelMatchQueue(Client, matchQueueType, IsAllArea);
	ResponseStartGame(Client, "你取消了当前的匹配,请尝试重新匹配把");
}

void ResponseCancelMatchQueue(ClientNode *Client, UINT matchQueueType, UCHAR IsAllArea)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, Client->Uin);	// uin
	Write32(p, 0);				// time
	Write32(p, 0);				// resultId
	Write32(p, 0);				// reasonLen
	Write32(p, matchQueueType); // matchQueueType
	Write8(p, IsAllArea);		// IsAllArea

	len = p - buf;
	SendToClient(Client, 20080, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}
void RequestEnterMatchRoom(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT uin = Read32(Body);
	UINT time = Read32(Body);

	// Un roomId = ReadUn(Body);
	ResponseEnterMatchRoom(Client);
}

void ResponseEnterMatchRoom(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, 0); // resultId
	Write8(p, 0);  // reasonLen

	Write8(p, 0); // HasMemoryRaceInfo
	/*
	for (size_t i = 0; i < n; i++)
	{ //Info
		BYTE* pInfo = p;
		Write16(pInfo, 0); //len

		Write32(pInfo, 0); //Year
		Write32(pInfo, 0); //KartNum
		for (size_t i = 0; i < n; i++)
		{ //Kart
			BYTE* pKart = pInfo;
			Write16(pKart, 0); //len

			Write32(pKart, 0); //Uin
			Write32(pKart, 0); //KartId
			Write32(pKart, 0); //RefitCout
			Write16(pKart, 0); //MaxFlags
			Write16(pKart, 0); //WWeight
			Write16(pKart, 0); //SpeedWeight
			Write16(pKart, 0); //JetWeight
			Write16(pKart, 0); //SJetWeight
			Write16(pKart, 0); //AccuWeight
			Write32(pKart, 0); //ShapeRefitCount
			Write32(pKart, 0); //KartHeadRefitItemID
			Write32(pKart, 0); //KartTailRefitItemID
			Write32(pKart, 0); //KartFlankRefitItemID
			Write32(pKart, 0); //KartTireRefitItemID
			{ //KartRefitExInfo
				BYTE* pKartRefitExInfo = pKart;
				Write16(pKartRefitExInfo, 0); //len

				Write8(pKartRefitExInfo, 0); //SpeedRefitStar
				Write8(pKartRefitExInfo, 0); //JetRefitStar
				Write8(pKartRefitExInfo, 0); //SJetRefitStar
				Write8(pKartRefitExInfo, 0); //AccuRefitStar
				Write8(pKartRefitExInfo, 0); //SpeedAddRatio
				Write8(pKartRefitExInfo, 0); //JetAddRatio
				Write8(pKartRefitExInfo, 0); //SJetAddRatio
				Write8(pKartRefitExInfo, 0); //AccuAddRatio

				len = pKartRefitExInfo - pKart;
				Set16(pKart, (WORD)len);
				pKart += len;
			}
			Write32(pKart, 0); //SecondRefitCount
			Write16(pKart, 0); //Speed2Weight
			Write16(pKart, 0); //DriftVecWeight
			Write16(pKart, 0); //AdditionalZSpeedWeight
			Write16(pKart, 0); //AntiCollisionWeight
			Write16(pKart, 0); //LuckyValue
			Write16(pKart, 0); //RefitLuckyValueMaxWeight
			Write32(pKart, 0); //ShapeSuitID
			Write8(pKart, 0); //LegendSuitLevel
			Write32(pKart, 0); //LegendSuitLevelChoice
			Write32(pKart, 0); //ShapeLegendSuitID

			len = pKart - pInfo;
			Set16(pInfo, (WORD)len);
			pInfo += len;
		}
		Write32(pInfo, 0); //SuitNum
		for (size_t i = 0; i < n; i++)
		{ //Suit
			BYTE* pSuit = pInfo;
			Write16(pSuit, 0); //len

			Write16(pSuit, 0); //Idx
			Write16(pSuit, 0); //ItemNum
			Write32(pSuit, 0); //ItemID[]

			len = pSuit - pInfo;
			Set16(pInfo, (WORD)len);
			pInfo += len;
		}
		Write32(pInfo, 0); //RandSuitIdx

		len = pInfo - p;
		Set16(p, (WORD)len);
		p += len;
	}
	*/

	len = p - buf;
	SendToClient(Client, 20082, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void RequestLeaveGame(ClientNode *Client, BYTE *Body, size_t BodyLen)
{
	UINT uin = Read32(Body);
	UINT time = Read32(Body);

	UINT GameSeqIDHigh = Read32(Body);
	UINT GameSeqIDLow = Read32(Body);
	USHORT LeaveGameType = Read16(Body);
	int ParaList[20] = {};
	UCHAR ParaNum = Read8(Body);
	for (int i = 0; i < ParaNum; i++)
	{
		ParaList[i] = Read32(Body);
	}
	NotifyGameOver(Client, LeaveGameType, ParaNum, ParaList, 0);
	ResponseLeaveGame(Client);
}
void RequestLeaveGame2(ClientNode *Client)
{
	int ParaList[2] = {0};
	ParaList[0] = Client->MapID;
	ParaList[1] = Client->RoomID;
	NotifyGameOver(Client, 1, 2, ParaList, 0);
	ResponseLeaveGame(Client);
}

void ResponseLeaveGame(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, Client->Uin); // Uin
	Write16(p, 0);			 // ResultID

	len = p - buf;
	SendToClient(Client, 424, buf, len, Client->GameID, FE_GAMELOGIC, Client->ConnID, Response);
}

void NotifyRandRoomNameList(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	const char *RandRoomNameList[] = {"RandRoomName A", "RandRoomName B", "RandRoomName C"};
	BYTE Count = sizeof(RandRoomNameList) / sizeof(RandRoomNameList[0]);
	Write8(p, Count);
	for (BYTE i = 0; i < Count; i++)
	{ // RoomNames
		BYTE *pRoomNames = p;
		Write16(pRoomNames, 0); // len

		len = strlen(RandRoomNameList[i]);
		Write8(pRoomNames, (BYTE)len); // NameLen
		memcpy(pRoomNames, RandRoomNameList[i], len);
		pRoomNames += len;

		for (size_t i = 0; i < 20; i++)
		{
			Write8(pRoomNames, 0); // BaseModes[]
		}

		len = pRoomNames - p;
		Set16(p, (WORD)len);
		p += len;
	}

	len = p - buf;
	SendToClient(Client, 760, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}

void NotifyTopListDesc(ClientNode *Client)
{
	BYTE *buf = new BYTE[32768];
	BYTE *p = buf;
	size_t len = 0;

	try
	{
		YAML::Node Config = YAML::LoadFile(".\\Config\\TopListDesc-ZingSpeed.yml");
		USHORT Count = (USHORT)Config.size();

		Write16(p, Count);
		for (USHORT i = 0; i < Count; i++)
		{ // Descs
			BYTE *pDescs = p;
			Write16(pDescs, 0); // len

			YAML::Node Descs = Config[i];

			Write16(pDescs, Descs["ID"].as<int>());

			std::string Name = Descs["Name"].as<std::string>();
			std::string ValueDesc = Descs["ValueDesc"].as<std::string>();
			std::string TitleName = Descs["TitleName"].as<std::string>();

			memset(pDescs, 0, MaxNickName);
			strncpy_s((char *)pDescs, MaxNickName, Name.c_str(), Name.length());
			pDescs += MaxNickName;

			memset(pDescs, 0, MaxNickName);
			strncpy_s((char *)pDescs, MaxNickName, ValueDesc.c_str(), ValueDesc.length());
			pDescs += MaxNickName;

			memset(pDescs, 0, MaxNickName);
			strncpy_s((char *)pDescs, MaxNickName, TitleName.c_str(), TitleName.length());
			pDescs += MaxNickName;

			Write8(pDescs, 0); // HasOtherInfo
			/*
			{ //Other
				BYTE* pOther = pDescs;
				Write16(pOther, 0); //len

				//WriteString(pOther, 0); //IntParaDesc0[]
				//WriteString(pOther, 0); //IntParaDesc1[]
				//WriteString(pOther, 0); //StrParaDesc0[]
				//WriteString(pOther, 0); //StrParaDesc1[]

				len = pOther - pDescs;
				Set16(pDescs, (WORD)len);
				pDescs += len;
			}
			*/

			Write8(pDescs, Descs["IsEnableSelfRank"].as<bool>());

			len = pDescs - p;
			Set16(p, (WORD)len);
			p += len;
		}
	}
	catch (const std::exception &)
	{
		Z_ERROR("异常\n");
		delete[] buf;
		return;
	}

	len = p - buf;
	SendToClient(Client, 701, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	delete[] buf;
}

void RequestTopList(ClientNode *Client, BYTE *Body, size_t BodyLen) // Lupin
{
	BYTE *p = Body;

	UINT Uin = Read32(p);
	UINT Time = Read32(p);
	USHORT ID = Read16(p);
	UCHAR Type = Read8(p);

	ResponseTopList(Client, ID, Type);
}

void ResponseTopList(ClientNode *Client, USHORT ID, UCHAR Type) // Lupin
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	const char *sql = NULL;
	sqlite3_stmt *stmt = NULL;
	int result;

	Write32(p, Client->Uin); // Uin
	Write16(p, ID);			 // ID

	BYTE *pCount = p;
	Write16(p, 0); // Count
	if (ID==4) {//魅力
		sql = "SELECT Uin,NickName,CharmValueOfMonth FROM BaseInfo  ORDER BY CharmValueOfMonth DESC;";
	}
	else if(ID == 1) {//等级
		sql = "SELECT Uin,NickName,Experience FROM BaseInfo  ORDER BY Experience DESC;";
	}
	else {
		sql = "SELECT Uin,NickName,Uin FROM BaseInfo ORDER BY Uin;";
	}
	
	result = sqlite3_prepare_v2(PlayerDB, sql, -1, &stmt, NULL);
	if (result == SQLITE_OK)
	{
		int ItemCount = 0;
		while (sqlite3_step(stmt) == SQLITE_ROW)
		{
			if (ItemCount > 99)
			{
				break;
			}
			ItemCount++;

			UINT Uin = sqlite3_column_int(stmt, 0);
			std::string NickName = (char *)sqlite3_column_text(stmt, 1);
			UINT Experience = sqlite3_column_int(stmt, 2);
			{ // Items
				BYTE *pItems = p;
				Write16(pItems, 0); // len

				Write32(pItems, Uin); // Uin

				strcpy_s((char *)pItems, MaxNickName, NickName.c_str());
				pItems += MaxNickName;

				Write32(pItems, Experience); // Value

				Write8(pItems, 0); // HasOtherInfo
				/*
				for (size_t i = 0; i < n; i++)
				{ //Other
					BYTE* pOther = pItems;
					Write16(pOther, 0); //len

					Write32(pOther, 0); //IntPara0
					Write32(pOther, 0); //IntPara1
					WriteString(pOther, 0); //StrPara0[]
					WriteString(pOther, 0); //StrPara1[]
					Write32(pOther, 0); //IntPara2
					Write8(pOther, 0); //HasMapInfo
					for (size_t i = 0; i < n; i++)
					{ //MapInfo
						BYTE* pMapInfo = pOther;
						Write16(pMapInfo, 0); //len

						Write8(pMapInfo, 0); //MapItemNum
						for (size_t i = 0; i < n; i++)
						{ //MapItemList
							BYTE* pMapItemList = pMapInfo;
							Write16(pMapItemList, 0); //len

							Write32(pMapItemList, 0); //Uin
							Write8(pMapItemList, 0); //Area

							len = pMapItemList - pMapInfo;
							Set16(pMapInfo, (WORD)len);
							pMapInfo += len;
						}

						len = pMapInfo - pOther;
						Set16(pOther, (WORD)len);
						pOther += len;
					}

					len = pOther - pItems;
					Set16(pItems, (WORD)len);
					pItems += len;
				}
				*/

				Write8(pItems, 0);	// AreaId
				Write8(pItems, 0);	// ParaNum
				Write32(pItems, 0); // Para[]

				Write8(pItems, 0); // AnchorNum
				/*
				for (size_t i = 0; i < n; i++)
				{ //AnchorInfo
					BYTE* pAnchorInfo = pItems;
					Write16(pAnchorInfo, 0); //len

					Write32(pAnchorInfo, 0); //Uin
					Write32(pAnchorInfo, 0); //RoomID
					Write8(pAnchorInfo, 0); //StrNum

					len = pAnchorInfo - pItems;
					Set16(pItems, (WORD)len);
					pItems += len;
				}
				*/

				len = pItems - p;
				Set16(p, (WORD)len);
				p += len;
			}
		}
		Set16(pCount, (WORD)ItemCount);
	}
	sqlite3_finalize(stmt);
	stmt = NULL;

	Write32(p, 0); // SelfRank
	Write32(p, 0); // BeginRank
	Write32(p, 0); // UperPercentage

	len = p - buf;
	SendToClient(Client, 461, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Response);
}

void NotifySingleGameBegin(ClientNode *Client, std::string data, int lenng)
{
	Client->SetSkillID();
	if (data.length() > 0)
	{
		const unsigned char *pMsg530 = EHexToByte(data.data());
		SendToClient(Client, 530, pMsg530, lenng, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
	}
	else
	{
		std::vector<uint8_t> buf(81920 * 2); // 创建一个大小为81920*2字节的vector容器，并且清空所有元素
		uint8_t *p = buf.data();			 // 获取vector内部缓冲区的指针
		size_t len = 0;

		Write32(p, 6000);	 // NormalLoadTime
		Write32(p, 0);		 // TaskID
		Write32(p, 0);		 // MapID
		Write32(p, 0);		 // MapHash[]
		Write32(p, 0);		 // MapHash[]
		Write32(p, 0);		 // MapHash[]
		Write32(p, 0);		 // MapHash[]
		Write32(p, 0);		 // MapHash[]
		Write32(p, 0);		 // MapHash[]
		Write32(p, 0);		 // MapHash[]
		Write32(p, 0);		 // MapHash[]
		Write32(p, 254);	 // GameID
		Write32(p, 6818351); // Seed
		Write16(p, 0);		 // TotalMapRound
		Write32(p, 0);		 // TimeLimit
		Write32(p, 0);		 // MaxNPCDistance
		Write16(p, 0);		 // InitPropNum

		Write8(p, 1); // InitPropNum
		for (size_t i = 0; i < 1; i++)
		{ // PropInfo
			BYTE *pPropInfo = p;
			Write16(pPropInfo, 0); // len

			Write32(pPropInfo, 0); // PropIndex
			Write16(pPropInfo, 0); // PropID

			len = pPropInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}

		Write16(p, 1); // NpcNum
		for (size_t i = 0; i < 1; i++)
		{ // NpcInfo
			BYTE *pNpcInfo = p;
			Write16(pNpcInfo, 0); // len

			Write32(pNpcInfo, 0); // NpcID
			// WriteString(pNpcInfo, 0); //NPCName[]
			// WriteString(pNpcInfo, 0); //GuildName[]
			{ // NpcItemInfo
				BYTE *pNpcItemInfo = pNpcInfo;
				Write16(pNpcItemInfo, 0); // len

				Write32(pNpcItemInfo, 0); // RoleID
				Write32(pNpcItemInfo, 0); // HairID
				Write32(pNpcItemInfo, 0); // FaceID
				Write32(pNpcItemInfo, 0); // GlassID
				Write32(pNpcItemInfo, 0); // CoatID
				Write32(pNpcItemInfo, 0); // GloveID
				Write32(pNpcItemInfo, 0); // TrousersID
				Write32(pNpcItemInfo, 0); // PersonarID
				Write32(pNpcItemInfo, 0); // TattooID
				Write32(pNpcItemInfo, 0); // WingID
				Write32(pNpcItemInfo, 0); // DecoratorID
				Write32(pNpcItemInfo, 0); // KartID
				Write32(pNpcItemInfo, 0); // ColorID
				Write32(pNpcItemInfo, 0); // TrackID
				Write32(pNpcItemInfo, 0); // EffectID
				Write32(pNpcItemInfo, 0); // BrandID
				Write32(pNpcItemInfo, 0); // TailID

				len = pNpcItemInfo - pNpcInfo;
				Set16(pNpcInfo, (WORD)len);
				pNpcInfo += len;
			}
			Write32(pNpcInfo, 0); // GpSkillPeriod
			Write32(pNpcInfo, 0); // BaseValue
			{					  // DefaultSpeedPara
				BYTE *pDefaultSpeedPara = pNpcInfo;
				Write16(pDefaultSpeedPara, 0); // len

				Write32(pDefaultSpeedPara, 0); // MaxSpeed
				Write32(pDefaultSpeedPara, 0); // MinSpeed
				Write32(pDefaultSpeedPara, 0); // MaxPlusForce
				Write32(pDefaultSpeedPara, 0); // MinPlusForce
				Write32(pDefaultSpeedPara, 0); // MaxTurnSpeed
				Write32(pDefaultSpeedPara, 0); // MinTurnSpeed
				Write32(pDefaultSpeedPara, 0); // MaxDist
				Write32(pDefaultSpeedPara, 0); // MinDist

				len = pDefaultSpeedPara - pNpcInfo;
				Set16(pNpcInfo, (WORD)len);
				pNpcInfo += len;
			}

			Write8(pNpcInfo, 1); // NormalSpeedNum
			for (size_t i = 0; i < 1; i++)
			{ // NormalSpeedPara
				BYTE *pNormalSpeedPara = pNpcInfo;
				Write16(pNormalSpeedPara, 0); // len

				Write32(pNormalSpeedPara, 0); // MaxSpeed
				Write32(pNormalSpeedPara, 0); // MinSpeed
				Write32(pNormalSpeedPara, 0); // MaxPlusForce
				Write32(pNormalSpeedPara, 0); // MinPlusForce
				Write32(pNormalSpeedPara, 0); // MaxTurnSpeed
				Write32(pNormalSpeedPara, 0); // MinTurnSpeed
				Write32(pNormalSpeedPara, 0); // MaxDist
				Write32(pNormalSpeedPara, 0); // MinDist

				len = pNormalSpeedPara - pNpcInfo;
				Set16(pNpcInfo, (WORD)len);
				pNpcInfo += len;
			}
			Write8(pNpcInfo, 0); // GpNum
			for (size_t i = 0; i < 1; i++)
			{ // GpPara
				BYTE *pGpPara = pNpcInfo;
				Write16(pGpPara, 0); // len

				Write32(pGpPara, 0); // GpID
				Write32(pGpPara, 0); // Probability
				Write32(pGpPara, 0); // MaxDist
				Write32(pGpPara, 0); // MinDist

				len = pGpPara - pNpcInfo;
				Set16(pNpcInfo, (WORD)len);
				pNpcInfo += len;
			}
			Write16(pNpcInfo, 0); // NPCAILevel
			Write16(pNpcInfo, 0); // TeamID

			len = pNpcInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write32(p, 0); // PlayerItemNum
		for (size_t i = 0; i < 1; i++)
		{ // PlayerInfo
			BYTE *pPlayerInfo = p;
			Write16(pPlayerInfo, 0); // len

			Write32(pPlayerInfo, 0); // NpcID
			// WriteString(pPlayerInfo, 0); //NPCName[]
			// WriteString(pPlayerInfo, 0); //GuildName[]
			{ // NpcItemInfo
				BYTE *pNpcItemInfo = pPlayerInfo;
				Write16(pNpcItemInfo, 0); // len

				Write32(pNpcItemInfo, 0); // RoleID
				Write32(pNpcItemInfo, 0); // HairID
				Write32(pNpcItemInfo, 0); // FaceID
				Write32(pNpcItemInfo, 0); // GlassID
				Write32(pNpcItemInfo, 0); // CoatID
				Write32(pNpcItemInfo, 0); // GloveID
				Write32(pNpcItemInfo, 0); // TrousersID
				Write32(pNpcItemInfo, 0); // PersonarID
				Write32(pNpcItemInfo, 0); // TattooID
				Write32(pNpcItemInfo, 0); // WingID
				Write32(pNpcItemInfo, 0); // DecoratorID
				Write32(pNpcItemInfo, 0); // KartID
				Write32(pNpcItemInfo, 0); // ColorID
				Write32(pNpcItemInfo, 0); // TrackID
				Write32(pNpcItemInfo, 0); // EffectID
				Write32(pNpcItemInfo, 0); // BrandID
				Write32(pNpcItemInfo, 0); // TailID

				len = pNpcItemInfo - pPlayerInfo;
				Set16(pPlayerInfo, (WORD)len);
				pPlayerInfo += len;
			}
			Write32(pPlayerInfo, 0); // GpSkillPeriod
			Write32(pPlayerInfo, 0); // BaseValue
			{						 // DefaultSpeedPara
				BYTE *pDefaultSpeedPara = pPlayerInfo;
				Write16(pDefaultSpeedPara, 0); // len

				Write32(pDefaultSpeedPara, 0); // MaxSpeed
				Write32(pDefaultSpeedPara, 0); // MinSpeed
				Write32(pDefaultSpeedPara, 0); // MaxPlusForce
				Write32(pDefaultSpeedPara, 0); // MinPlusForce
				Write32(pDefaultSpeedPara, 0); // MaxTurnSpeed
				Write32(pDefaultSpeedPara, 0); // MinTurnSpeed
				Write32(pDefaultSpeedPara, 0); // MaxDist
				Write32(pDefaultSpeedPara, 0); // MinDist

				len = pDefaultSpeedPara - pPlayerInfo;
				Set16(pPlayerInfo, (WORD)len);
				pPlayerInfo += len;
			}
			Write8(pPlayerInfo, 0); // NormalSpeedNum
			for (size_t i = 0; i < 1; i++)
			{ // NormalSpeedPara
				BYTE *pNormalSpeedPara = pPlayerInfo;
				Write16(pNormalSpeedPara, 0); // len

				Write32(pNormalSpeedPara, 0); // MaxSpeed
				Write32(pNormalSpeedPara, 0); // MinSpeed
				Write32(pNormalSpeedPara, 0); // MaxPlusForce
				Write32(pNormalSpeedPara, 0); // MinPlusForce
				Write32(pNormalSpeedPara, 0); // MaxTurnSpeed
				Write32(pNormalSpeedPara, 0); // MinTurnSpeed
				Write32(pNormalSpeedPara, 0); // MaxDist
				Write32(pNormalSpeedPara, 0); // MinDist

				len = pNormalSpeedPara - pPlayerInfo;
				Set16(pPlayerInfo, (WORD)len);
				pPlayerInfo += len;
			}
			Write8(pPlayerInfo, 0); // GpNum
			for (size_t i = 0; i < 1; i++)
			{ // GpPara
				BYTE *pGpPara = pPlayerInfo;
				Write16(pGpPara, 0); // len

				Write32(pGpPara, 0); // GpID
				Write32(pGpPara, 0); // Probability
				Write32(pGpPara, 0); // MaxDist
				Write32(pGpPara, 0); // MinDist

				len = pGpPara - pPlayerInfo;
				Set16(pPlayerInfo, (WORD)len);
				pPlayerInfo += len;
			}
			Write16(pPlayerInfo, 0); // NPCAILevel
			Write16(pPlayerInfo, 0); // TeamID

			len = pPlayerInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 0); // SyncCarNum
		for (size_t i = 0; i < 1; i++)
		{ // SyncCar
			BYTE *pSyncCar = p;
			Write16(pSyncCar, 0); // len

			Write32(pSyncCar, 0); // PlayerUin
			Write16(pSyncCar, 0); // CurCarID
			Write8(pSyncCar, 0);  // HasStoneInfo
			for (size_t i = 0; i < 1; i++)
			{ // STKartStoneGrooveInfo
				BYTE *pSTKartStoneGrooveInfo = pSyncCar;
				Write16(pSTKartStoneGrooveInfo, 0); // len

				Write32(pSTKartStoneGrooveInfo, 0); // KartID
				Write32(pSTKartStoneGrooveInfo, 0); // StoneGrooveNum
				for (size_t i = 0; i < 1; i++)
				{ // StoneGrooveInfo
					BYTE *pStoneGrooveInfo = pSTKartStoneGrooveInfo;
					Write16(pStoneGrooveInfo, 0); // len

					Write32(pStoneGrooveInfo, 0); // StoneUseOccaType
					Write32(pStoneGrooveInfo, 0); // SkillStoneID

					len = pStoneGrooveInfo - pSTKartStoneGrooveInfo;
					Set16(pSTKartStoneGrooveInfo, (WORD)len);
					pSTKartStoneGrooveInfo += len;
				}

				len = pSTKartStoneGrooveInfo - pSyncCar;
				Set16(pSyncCar, (WORD)len);
				pSyncCar += len;
			}

			len = pSyncCar - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 1); // PlayerNum
		for (size_t i = 0; i < 1; i++)
		{ // AvatarChangeInfo
			BYTE *pAvatarChangeInfo = p;
			Write16(pAvatarChangeInfo, 0); // len

			Write32(pAvatarChangeInfo, 0); // PlayerUin
			Write32(pAvatarChangeInfo, 0); // HairID
			Write32(pAvatarChangeInfo, 0); // CoatID
			Write32(pAvatarChangeInfo, 0); // TrousersID
			Write32(pAvatarChangeInfo, 0); // SuitID
			Write32(pAvatarChangeInfo, 0); // KartID

			Write8(pAvatarChangeInfo, 0); // HasItemInfo
			for (size_t i = 0; i < 1; i++)
			{ // ItemInfo
				BYTE *pItemInfo = pAvatarChangeInfo;
				Write16(pItemInfo, 0); // len

				Write32(pItemInfo, 0); // RoleID
				Write32(pItemInfo, 0); // HairID
				Write32(pItemInfo, 0); // FaceID
				Write32(pItemInfo, 0); // GlassID
				Write32(pItemInfo, 0); // CoatID
				Write32(pItemInfo, 0); // GloveID
				Write32(pItemInfo, 0); // TrousersID
				Write32(pItemInfo, 0); // PersonarID
				Write32(pItemInfo, 0); // TattooID
				Write32(pItemInfo, 0); // WingID
				Write32(pItemInfo, 0); // DecoratorID
				Write32(pItemInfo, 0); // KartID
				Write32(pItemInfo, 0); // ColorID
				Write32(pItemInfo, 0); // TrackID
				Write32(pItemInfo, 0); // EffectID
				Write32(pItemInfo, 0); // BrandID
				Write32(pItemInfo, 0); // TailID

				len = pItemInfo - pAvatarChangeInfo;
				Set16(pAvatarChangeInfo, (WORD)len);
				pAvatarChangeInfo += len;
			}

			len = pAvatarChangeInfo - p;
			Set16(p, (WORD)len);
			p += len;
		}
		Write8(p, 0);  // FeedBackEnabled
		Write16(p, 0); // PropID
		Write32(p, 0); // PropIndex
		Write16(p, 0); // PropNum
		Write16(p, 0); // LogicType
		Write8(p, 0);  // MapCheckpointFileIndex
		Write32(p, 0); // Duration
		Write32(p, 0); // BeginCDTime
		Write32(p, 0); // PropInteval

		len = p - buf.data();
		SendToClient(Client, 530, buf.data(), len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
	}
}

void NotifySingleGameBeginTask1701(ClientNode *Client)
{
	Client->SetSkillID();
	std::vector<uint8_t> buf(8192); // 创建一个大小为81920字节的vector，并且清空所有元素
	uint8_t *p = buf.data();		// 获取vector内部缓冲区的指针
	size_t len = 0;

	Write32(p, 8000);		   // NormalLoadTime
	Write32(p, 1701);		   // TaskID
	Write32(p, Client->MapID); // MapID
	// WriteString(p, 0); //MapHash[]
	/*Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);*/
	memset(p, 0, 32);
	p += 32;
	Write32(p, Client->GameID);		// GameID
	Write32(p, 0);					// Seed
	Write16(p, Client->TotalRound); // TotalMapRound
	Write32(p, 0);					// TimeLimit
	Write32(p, 0);					// MaxNPCDistance
	Write16(p, 0);					// InitPropNum

	Write16(p, 0); // NpcNum

	Write32(p, 0); // PlayerItemNum

	Write8(p, 0); // SyncCarNum

	Write8(p, 0); // PlayerNum

	Write8(p, 0);  // FeedBackEnabled
	Write16(p, 0); // PropID
	Write32(p, 0); // PropIndex
	Write16(p, 0); // PropNum
	Write16(p, 3); // LogicType
	Write8(p, 0);  // MapCheckpointFileIndex
	Write32(p, 0); // Duration
	Write32(p, 0); // BeginCDTime
	Write32(p, 0); // PropInteval
	Write16(p, 0);

	len = p - buf.data();
	SendToClient(Client, 530, buf.data(), len, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifySingleGameBeginTask110(ClientNode *Client)
{
	Client->SetSkillID();
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, 8000); // NormalLoadTime
	Write32(p, 110);  // TaskID
	Write32(p, 199);  // MapID
	// WriteString(p, 0); //MapHash[]
	/*Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);
	Write32(p, 0);*/
	memset(p, 0, 32);
	p += 32;
	Write32(p, 709);	  // GameID
	Write32(p, 81304308); // Seed
	Write16(p, 2);		  // TotalMapRound
	Write32(p, 180000);	  // TimeLimit
	Write32(p, 0);		  // MaxNPCDistance
	Write16(p, 0);		  // InitPropNum

	Write16(p, 0); // NpcNum

	Write32(p, 0); // PlayerItemNum

	Write8(p, 0); // SyncCarNum

	Write8(p, 0); // PlayerNum

	Write8(p, 0);  // FeedBackEnabled
	Write16(p, 0); // PropID
	Write32(p, 0); // PropIndex
	Write16(p, 0); // PropNum
	Write16(p, 3); // LogicType
	Write8(p, 0);  // MapCheckpointFileIndex
	Write32(p, 0); // Duration
	Write32(p, 0); // BeginCDTime
	Write32(p, 0); // PropInteval
	Write16(p, 0);

	len = p - buf;
	// len = p - buf;
	SendToClient(Client, 530, buf, len, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
}
// const static unsigned char* pMsg1406 = EHexToByte("0000EA600000057E000000AB0000000000000000000000000000000000000000000000000000000000000000000001673EBA452E0002000000000000000000000001009E00000081BADAD2C2C8CB0000000000000000000000000000000000000000000000000000000000460000272600005A62000027770000000000005A6000005A9800005A61000000000000000000000000000000000000586E0000271E00002775000027730000000000000000000000000000271000220000000F0000000300000003000000010000000200000001000000000000000000000000000000000001009E000000BBD0A1E9D9D7D30000000000000000000000000000000000000000000000000000000000460000272200005AC300005AC60000000000005AC400005A9800005AC50000000000000000000000000000000000005E920000271E0000277500002773000000000000000000000000000027100022000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000");
// void NotifySingleGameBeginTask1406(ClientNode* Client)
//{
//	BYTE buf[8192] = {0}; BYTE* p = buf; size_t len = 0;
//
//	SendToClient(Client, 530, pMsg1406, 414, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
// }
// const static unsigned char* pMsg1407 = EHexToByte("0000EA600000057F000000B8000000000000000000000000000000000000000000000000000000000000000000000030404C83A20002000075300000003200000001009E0000008EC3A8C5AE000000000000000000000000000000000000000000000000000000000000004600002722000035A30000285400000000000035A1000035A4000035A20000000000000000000000000000000000005A160000271E0000277500002773000000000000000000000000000027100022000000100000000500000006000000020000000200000001000000000000000000000000000000000001009E000000BBD0A1E9D9D7D30000000000000000000000000000000000000000000000000000000000460000272200005AC300005AC60000000000005AC400005A9800005AC50000000000000000000000000000000000005E920000271E000027750000277300000000000000000000000000002710002200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000A000000000000000000000000003030");
// void NotifySingleGameBeginTask1407(ClientNode* Client)
//{
//	BYTE buf[8192] = {0}; BYTE* p = buf; size_t len = 0;
//
//	SendToClient(Client, 530, pMsg1407, 414, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
// }
// const static unsigned char* pMsg1408 = EHexToByte("0000EA6000000580000000A00000000000000000000000000000000000000000000000000000000000000000000000CB0A0334FA000200025D78000000000000000000000001009E000000BBD0A1E9D9D7D30000000000000000000000000000000000000000000000000000000000460000272200005AC300005AC60000000000005AC400005A9800005AC50000000000000000000000000000000000005E920000271E0000277500002773000000000000000000000000000027100022000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000000");
// void NotifySingleGameBeginTask1408(ClientNode* Client)
//{
//	BYTE buf[8192] = {0}; BYTE* p = buf; size_t len = 0;
//
//	SendToClient(Client, 530, pMsg1408, 256, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
// }
// const static unsigned char* pMsg1409 = EHexToByte("0000EA6000000581000000A600000000000000000000000000000000000000000000000000000000000000000000000E173359A600030001FBD0000000000000000000000001009E000000BBD0A1E9D9D7D30000000000000000000000000000000000000000000000000000000000460000272200005AC300005AC60000000000005AC400005A9800005AC50000000000000000000000000000000000005E920000271E0000277500002773000000000000000000000000000027100022000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000004000000000000000000000000000100");
// void NotifySingleGameBeginTask1409(ClientNode* Client)
//{
//	BYTE buf[8192] = {0}; BYTE* p = buf; size_t len = 0;
//
//	SendToClient(Client, 530, pMsg1409, 256, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
// }

const static unsigned char *pMsg531 = EHexToByte("00000FA0E768");
void NotifySingleRaceBegin(ClientNode *Client)
{
	if (Client->isRaceBegin == true)
	{
		return;
	}
	Client->isRaceBegin = true;
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	// Write32(p, 9000); //CountDownTime

	// len = p - buf;
	SendToClient(Client, 531, pMsg531, 6, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
}
void NotifySingleRaceBeginDiy(ClientNode *Client, UINT CountDownTime)
{
	if (Client->isRaceBegin == true)
	{
		return;
	}
	Client->isRaceBegin = true;
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, CountDownTime); // CountDownTime

	len = p - buf;
	SendToClient(Client, 531, buf, len, Client->ConnID, FE_GAMELOGIC, Client->ConnID, Notify);
}

void NotifyNPCDoAction(ClientNode *Client)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, 1); // NPCID
	Write16(p, 1); // ActionID
	Write8(p, 0);  // SpeedIdx

	len = p - buf;
	SendToClient(Client, 436, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}
void NotifyMatchSkillStoneInfo(ClientNode *Client, BYTE *Body, size_t BodyLen) // 宝石通知其他玩家
{
	UINT Uin = Read32(Body);
	UINT Time = Read32(Body);
	UINT ItemID = Read32(Body);

	if (ItemID < 21977 && ItemID > 22072)
		return;

	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write32(p, Uin);
	Write32(p, ItemID);

	len = p - buf;

	// printf("[GameServer][ConnID:%d][NotifyMatchSkillStoneInfo]Uin:%d, Time:%d, ItemID:%d\n", Client->ConnID, Uin, Time, ItemID);

	USHORT RoomID = Client->RoomID;
	RoomNode *Room = GetRoom(RoomID);

	if (!Room)
	{
		return;
	}
	// LeaveRoom(Client, 0);

	// 通知其它玩家
	for (char i = 0; i < 6; i++)
	{
		ClientNode *RoomClient = Room->Player[i];
		if (RoomClient && RoomClient != Client)
		{
			SendToClient(RoomClient, 910, buf, len, Client->GameID, FE_GAMELOGIC, RoomClient->ConnID, Response);
		}
	}
}
void NotifyChatGameOver(ClientNode *Client, UINT BaseMode, UINT SubMode)
{
	BYTE buf[8192] = {0};
	BYTE *p = buf;
	size_t len = 0;

	Write16(p, BaseMode); // BaseMode
	Write16(p, SubMode);  // SubMode
	Write8(p, 0);		  // IsTeamMatched
	Write16(p, 0);		  // GameOverReason

	len = p - buf;
	SendToClient(Client, 456, buf, len, Client->ConnID, FE_PLAYER, Client->ConnID, Notify);
}