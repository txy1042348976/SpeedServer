#pragma once

struct KartSkill
{
	WORD PID;
	bool IsOpen;
	DWORD Progress;
};
struct KartSkatePara
{
	INT TrackSpeed;
	INT SkateSpeedCoef_AirBigJet;
	INT SkateSpeedCoef_Jump;
	INT SkateSpeedCoef_AirJetPoint;
	INT SkateSpeedCoef_MaxForce;
	INT OntrackAccRef;
	INT TrackAccRef;
	INT DashSkill;
};
struct KartGrooveInfo
{
	BYTE ID;
	BYTE Type;
};
struct KartPhysInfo
{
	DWORD PID;
	DWORD AffectID;
	DWORD Param;
	DWORD LifeTime;
};
struct KartCompensateParam
{
	DWORD Percent;
	DWORD Value;
};
struct KartCollisionBoxParam
{
	int BoxLength;
	int BoxWidth;
	int BoxHeight;
	int AdjustOffset;
	int AdjustPickGap;
	int AdjustRemoteOffset;
	int AdjustBevelOffset;
	int ControllerOffset;
};
struct KartCarToCarCollisionParam
{
	int CollisionRadius;
	int CollisionLength;
	int CollisionWidth;
	int CollisionHeight;
	DWORD CollisionLostCtrlTime;
};
struct KartForwardAccel
{
	int Key;
	int Value;
};
struct KartForwardDecel
{
	int Key;
	int Value;
};
struct KartBackwardAccel
{
	int Key;
	int Value;
};
struct KartBackwardDecel
{
	int Key;
	int Value;
};
struct TaskInfo
{
	UINT TaskID;
	UINT MapID;
	bool DBType;
	UINT Status;
	UINT PrevStatus;
	std::string EHexToByte;
	int EHexToByteLen;
};
struct KartItem
{
	UINT KartID;
	KartSkill Skill;
	BYTE PhysInfoCount;
	KartPhysInfo PhysInfo[120];
	BYTE GrooveInfoCount=0;
	KartGrooveInfo GrooveInfo[5];
	BYTE CompensateParamCount;
	KartCompensateParam CompensateParam[120];
	KartCollisionBoxParam CollisionBoxParam;
	KartCarToCarCollisionParam CarToCarCollisionParam;
	KartSkatePara SkatePara;
	UINT BaseTurnRate;
	UINT VolatileTurnRate;
	UINT MaxTurnSpeed;
	UINT MinTurnSpeed;
	UINT MaxAccuTime;
	UINT BaseAccuRate;
	UINT MaxAffectSpeed;
	int Gravity;
	int AdditionalLocalZSpeed;
	UINT StartVec;
	UINT EndVecFist;
	UINT EndVecSecon;
	UINT DirKeyForce;
	UINT DirKeyTwist;
	UINT BannerTwist;
	UINT BannerKeyTwist;
	UINT BannerVecForce;
	UINT BannerHeadForce;
	UINT SlidFricForce;
	UINT RollFricForce;
	UINT StartWec;
	UINT MaxWec;
	UINT SuaiJianTwist;
	UINT DirUpKeyForce;
	BYTE AccelStatusCount;
	UINT AccelStatus[120];
	BYTE ForwardAccelNum;
	KartForwardAccel ForwardAccel[120];
	BYTE ForwardDecelNum;
	KartForwardDecel ForwardDecel[120];
	BYTE BackwardAccelNum;
	KartBackwardAccel BackwardAccel[120];
	BYTE BackwardDecelNum;
	KartBackwardDecel BackwardDecel[120];
};

KartItem *GetKartItem(UINT KartID);
TaskInfo *GetTaskInfo(UINT TaskID);
void WriteKartPhysParamOld(BYTE *&p, UINT Uin, UINT KartID, UINT PetID, UINT PetLevel, UINT SkinID);
void WriteKartPhysParam(BYTE *&p, UINT Uin, UINT KartID, UINT PetID, UINT PetLevel, UINT SkinID);
void loadKartBoatInfo(BYTE*& p);
