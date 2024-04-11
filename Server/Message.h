#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <map>
#include "z_debug.h"
#ifdef _DEBUG
#define DEBUG
#endif
#include "yaml-cpp/yaml.h"
#include <time.h>
#include "HPSocket/HPSocket.h"
#include <iostream>
#include "Utils.h"
#include "GameServer.h"

#include <unordered_map>
constexpr int MaxNickName = 17;
constexpr int MaxSecPwd = 11;
constexpr int MaxBaseNameNew = 16;
constexpr int MaxQQNickName = 20;
constexpr int MaxSignature = 64;

constexpr int MAX_MUCH_SEATNUMINROOM = 20;
constexpr int MAXNPCNUMINROOM = 20;
struct EngageInfo
{
	UINT UIN = 0;
	char NickName[MaxNickName];
	char BabyName1[MaxNickName];
	char BabyName2[MaxNickName];
	UINT BabyOneID = 0;
	UINT BabyOneItemID = 0;
	UINT BabyOneEquipStat = 0;
	UINT BabyOneGender = 0;
	UINT BabyTwoID = 0;
	UINT BabyTwoItemID = 0;
	UINT BabyTwoEquipStat = 0;
	UINT BabyTwoGender = 0;
	UINT BackGroundID = 0;
	UINT EngageTime = 0;
	
};
struct PetInfo
{
	UINT ID;
	char Name[23];
	UINT Exp;
	UINT MaxExp;
	UINT Level;
	UINT Status;
	UINT PL;
};
struct Vector3
{
	UINT X;
	UINT Y;
	UINT Z;
};
struct ClientNode
{
	UINT NobleID = 0;
	UINT OBState = 0;
	UINT GameTime = 0;
	bool StartGameTime = false;
	UINT KartStatus = 0;
	USHORT BaseGameMode;
	USHORT SubGameMode;
	ITcpServer* Server;
	LONG_PTR BufferPtr;
	LONG_PTR BufferSize;
	LONG_PTR BufferOffset;
	int ConnID;
	BOOL IsLogin;
	BOOL IsFinish;
	BOOL IsExit;
	DWORD Uin;
	BYTE Key[16];
	USHORT ServerID;
	UINT KartID;
	UINT SkinID;
	UINT OrgKartId;
	UINT TeamID;
	UINT useDQ =0;
	UINT DriftCount=0;
	UINT HighestSpeed = 0;
	UINT CollisionCount = 0;
	UINT N2OGatherCount = 0;
	INT getPropNum;
	//INT PropList[10];
	INT SkillStoneList[5];
	UINT kartPow;
	UINT Fuel;
	UINT TeamFuel;
	UINT CollectedN2O;
	UINT AppellationType;
	UINT matchQueueType;
	bool isRaceBegin;
	UINT taskID;
	// Room
	USHORT RoomID;
	USHORT GameID;
	BOOL IsReady;

	//
	UINT MapID;
	int FinishTime;
	int TotalRound;
	int Round;
	int PassedCheckPoint;
	int EndCheckPoint;
	bool Loop;
	bool MapCompatibilityMode;

	// Dance
	int Scores;
	UINT TaskID;
	// Outdoor
	USHORT ChatRoomID;

	// Character Info
	char NickName[MaxNickName];
	char SecPwd[MaxSecPwd];
	UINT SecPwdStats=0;
	UINT Verify =0;
	UINT FrozenLeftSeconds =0;
	UINT FrozenSeconds =0;
	char Gender;
	char Country;
	UINT License;
	UINT Experience;
	UINT SuperMoney;
	UINT Money;
	UINT WinNum;
	UINT SecondNum;
	UINT ThirdNum;
	UINT TotalNum;
	UINT CurHonor;
	UINT TotalHonor;
	UINT TodayHonor;
	UINT RelaxTime;
	UINT MonthDurationBefor;
	UINT MonthDurationCur;
	UINT Charm;
	UINT DurationGame;
	UINT DanceExp;
	UINT Coupons;
	UINT Admiration;
	UINT LuckMoney;
	UINT TeamWorkExp;
	UINT AchievePoint;
	UINT RegTime;
	char Signature[MaxSignature];

	int NewModel; // Switch Character 3D

	PetInfo* Pet;
	EngageInfo* Engage;
	// MoveInfo* HeroMoveInfo;

	UINT NumPropsInHand;
	UINT KartSkillID;
	UINT PetSkillID;
	int UseCountUpperlimit = 0;
	int N2OCountRequiredForReborn;
	UINT BorderRoomID;
	void InitForRace(UINT GameMapID);
	void InitForDance();
	void SetSkillID();
	bool GetSkillID(UINT SkillID);

	UINT OptNum = 0;
	UINT OptIndex = 4;
	UINT JumpThread = 0;
	HANDLE Timer;
	WAITORTIMERCALLBACK Callback;
	UINT ClientVersion;
	bool StoneMode = false;
};
struct AppellationInfo
{
	UINT Type;
	UINT Level;
	UINT Difficulty;
	UINT MapId;
	UINT Value;
};
struct LimitMsgInfo
{
	UINT MSGID;
	UINT isInfo;
};
struct DoBagItem
{
	int IitemID;
	USHORT Status;
};
struct KART_STONE
{
	UINT ID = 0;
	UINT StoneGrooveID = 0;
	UINT StoneUseOccaType = 0;
	UINT SkillStoneID = 0;
};
struct KART_REFIT
{
	UINT Uin = 0;
	UINT KartID = 0;
	UINT RefitCout = 0;
	UINT MaxFlags = 0;
	UINT WWeight = 0;
	UINT SpeedWeight = 0;
	UINT JetWeight = 0;
	UINT SJetWeight = 0;
	UINT AccuWeight = 0;
	UINT ShapeRefitCount = 0;
	UINT KartHeadRefitItemID = 0;
	UINT KartTailRefitItemID = 0;
	UINT KartFlankRefitItemID = 0;
	UINT KartTireRefitItemID = 0;
	UINT SecondRefitCount = 0;
	UINT Speed2Weight = 0;
	UINT DriftVecWeight = 0;
	UINT AdditionalZSpeedWeight = 0;
	UINT AntiCollisionWeight = 0;
	UINT LuckyValue = 0;
	UINT RefitLuckyValueMaxWeight = 0;
	UINT ShapeSuitID = 0;
	UINT LegendSuitLevel = 0;
	UINT LegendSuitLevelChoice = 0;
	UINT ShapeLegendSuitID = 0;
};
struct PlayerDBBaseInfo
{
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
	UINT AchievePoint = 0;
	UINT RegTime = 0;
	UINT SpeedCoin = 0;
	std::string Signature = "";
};
struct FriendGroupInfo
{
	UINT ID;
	UINT Name;
};
struct ClientVipInfo {
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
};
struct ClientPlayerInfo {
	int VipFlag = 0;
	int IsInTopList = 0;
};
struct MapRecord{
	UINT MapID;
	DWORD Record;
};
struct ConditionInfo
{
	UINT BorderGameType;
	UINT NeededMoney;
	UINT NeededLevel;

	LONG OpenTime;
	LONG CloseTime;

	INT NeedHonor;
};
struct MoveInfo
{

	UINT Uin;
	char WalkStatue;
	int Posion[3];
	int Ori[9];
	UINT AttachUin;
	int WalkExtState;
	UCHAR Nums;
	UINT PermitSitHorsePetUins[5];
	int EquipedPetID;
	int EquipedPetStatus;
	int PetPKResultStatus;
	USHORT HeroMajorState;
	USHORT HorsePetStatus;
	int NextPoint;
	short PathLen;
	int Path[50];
	int CurCellID;
	USHORT Speed;
	int CellID;
	USHORT Flag;
};
struct NpcBaseInfo
{
	UINT NpcID = 0;
	UINT MinLevel = 0;
	UINT FinishTime = 0;
	UINT Level = 0;
	UINT MaxLevel = 0;
	std::string RoomName;
	UINT RoleID = 0;
	UINT HairID = 0;
	UINT FaceID = 0;
	UINT GlassID = 0;
	UINT CoatID = 0;
	UINT GloveID = 0;
	UINT TrousersID = 0;
	UINT PersonarID = 0;
	UINT TattooID = 0;
	UINT WingID = 0;
	UINT DecoratorID = 0;
	UINT KartID = 0;
	UINT ColorID = 0;
	UINT TrackID = 0;
	UINT EffectID = 0;
	UINT BrandID = 0;
	UINT TailID = 0;

};
NpcBaseInfo* RoomNcpInfo();
void LoadNpcListConfig();

struct ItemStatus
{
	UINT ItemID;
	BYTE NewStatus;
};
struct ListInfo
{
	UINT Rate;
	UINT No;
};
struct KeyInfo
{
	UINT Rate;
	UINT No;
};

struct ItemInfo
{
	UINT ItemID;
	int ItemNum;
	int AvailPeriod;
	bool Status;
	int ObtainTime;
	int OtherAttribute;
	int ItemType;
};

enum ServerType
{
	Game,
	Dance,
	Relax,
	Engage
};

int getMaxFlag(int SJetWeight, int AccuWeight, int JetWeight, int WWeight, int SpeedWeight, int AntiCollisionWeight, int Speed2Weight, int DriftVecWeight, int AdditionalZSpeedWeight);
std::string EncryptionIP(std::string IP, UINT Type);
std::string DecodeIP(std::string IP, UINT Type);
void initListConfig();
void RequestAddFriendGroup(ClientNode* Client);
ClientNode getBakClient(ClientNode* Client);
void LoadAppellationConfig();
YAML::Node getGameConfig();
void NotifyFeedBackData(ClientNode* Client);
void delay(int seconds);
void delay(double seconds);
void ResponseDanceGetRoomList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestUseItem(ClientNode *Client, BYTE *Body, size_t BodyLen); // 158
void ResponseUseItem(ClientNode* Client, int ItemID, UINT DstUin);// 158
void SendToClient(ClientNode *Client, short MsgID, const BYTE *Data, DWORD Length, short SrcID, char SrcFE, short DstID, short MsgType, char Encrypt = true);
void OnRecvFromClient(ClientNode *Client, USHORT MsgID, BYTE *Body, size_t BodyLen);
void OnFullReceive(ClientNode *Client, BYTE *Data, size_t Length);
void NotifyRankedMatchSeasonInfo(ClientNode *Client);
//void ResponseShapeLegendOperate(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestShapeLegendOperate(ClientNode* Client, BYTE* Body, size_t BodyLen);
void InitDirServer(const char *Name, UINT IP, USHORT TcpPort, USHORT UdpPort);
int GetServerType(int ServerID);
void ResponseGetExchangeShopInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseDoShopExchange(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetCfgFile(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetDiscountCardInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetPurchaseBroadcast(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseCarLottoryGetInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void WritePlayerDBBaseInfo(BYTE *&p, UINT Uin);
void WritePlayerGuildInfo(BYTE *&p, UINT Uin);
UINT WriteEquipedItem(BYTE *&p, UINT Uin);
UINT WriteEquipedItem2(BYTE *&p, UINT Uin, UINT *cPetId);
UINT WriteEquipedItem3(BYTE *&p, UINT Uin, UINT *cPetId);
void WriteVisbleInfo(BYTE *&p, UINT Uin, char SeatID, short PlayerID);

ClientNode *GetClient(UINT Uin);
// void EeloadRoomList();
void AddClient(ClientNode *Client, UINT Uin);
void DelClient(UINT Uin);

// 封包
void RequestGetWorldInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetWorldInfo(ClientNode *Client, UINT ClientVersion, UINT MasterVersion, UINT SlaveVersion);
void RequestGetChatRoomList(ClientNode *Client, BYTE *Body, size_t BodyLen);

void RequestRegister2(ClientNode *Client, BYTE *Body, size_t BodyLen);

void RequestLogin(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestLogin2(ClientNode *Client, BYTE *Body, size_t BodyLen);

void RequestLogout(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseLogout(ClientNode *Client);

#include "Room.h"

void RequestHello(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseHello(ClientNode *Client, UINT Uin, UINT Time);

void RequestPrepareReady(ClientNode *Client, BYTE *Body, size_t BodyLen);

void NotifyGameBegin(ClientNode *Client, ClientNode *Player[]);
void NotifyGameNPCInfo(ClientNode* Client);
void NotifyKartPhysParam(ClientNode *Client, ClientNode *Player[]);
void NotifyRaceBegin(ClientNode *Client);
void MyResponseQuickRPC(ClientNode* Client, BYTE* Msg, size_t MsgLen);
void RequestReportCurrentInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);

void NotifyCountDown(ClientNode *Client, UINT WinnerUin, bool WinnerNewRecord, UINT FinTime);
void NotifyRaceShow(ClientNode *Client);
void NotifyGameOver(ClientNode *Client, USHORT LeaveGameType, BYTE ParaNum, int *ParaList, USHORT ReturnHall);

void RequestChangeItemStatus(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseChangeItemStatus(ClientNode *Client, USHORT ItemNum, ItemStatus *ItemStatus);

void RequestGetLicenseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetLicenseInfo(ClientNode *Client);

void RequestGetSystemTaskList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetSystemTaskList(ClientNode *Client, UINT TaskVersion);

void RequestChangeGender(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestChangeVip(ClientNode *Client, BYTE *Body, size_t BodyLen, int itemId);
void RequestChange(ClientNode *Client, BYTE *Body, size_t BodyLen, int itemId);
void ResponseChangeGender(ClientNode *Client, BYTE NewGender, BYTE UseType);

void RequestSkipFreshTask(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseSkipFreshTask(ClientNode *Client);

void RequestStartShadowChallenge(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseStartShadowChallenge(ClientNode *Client);
void RequestEndShadowChallenge(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseEndShadowChallenge(ClientNode *Client, int ResultID);

void RequestFishLogin(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseFishLogin(ClientNode *Client);

void RequestGetPlayerEventCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetPlayerEventCfg(ClientNode *Client);
void RequestGetPlayerEvent(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetPlayerEvent(ClientNode *Client, BYTE TargetType, BYTE TimeType);

void NotifyMsgBox(ClientNode *Client);

void RequestTransferByTCP(ClientNode *Client, BYTE *Body, size_t BodyLen);
void NotifyTranferByTCP(ClientNode *Client, UINT SrcUin, USHORT SrcPlayerID, int Seq, BYTE *Buff, int Bufflen, char Ver);

void RequestGetTaskList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetTaskList(ClientNode *Client);

void RequestUseProp2(ClientNode *Client, BYTE *Body, size_t BodyLen);

void RequestChangeModel(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseChangeModel(ClientNode *Client, BYTE NewModel);
void NotifyChangeModel(ClientNode *Client, UINT Uin, BYTE NewModel);

void NotifySvrConfig(ClientNode *Client);
void NotifySvrConfig2(ClientNode *Client);
void NotifySvrConfig3(ClientNode *Client);

void NotifySkillStoneKartInfo(ClientNode *Client);
void NotifyOtherKartStoneInfo(ClientNode *Client);

void RequestChangeUdpState(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestTransferUDPOK(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseTransferUDPOK(ClientNode *Client);

void RequestGetProp(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetProp(ClientNode *Client);

void NotifySpeed2Cfg(ClientNode *Client);

void RequestKartRefitInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseKartRefitInfo(ClientNode *Client);

void RequestNewGetFriendList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseNewGetFriendList(ClientNode *Client);
void ResponseNewGetFriendList2(ClientNode *Client);

void RequestFizzInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseFizzInfo(ClientNode *Client);

void RequestGetShoppingCarItemList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetShoppingCarItemList(ClientNode *Client);

void RequestReplayOperate(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseReplayOperate(ClientNode *Client, UCHAR Cmd);

void RequestGetKeyTranslateInfoNew(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetKeyTranslateInfoNew(ClientNode *Client);

void RequestC2SGetKartAltasConfig(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseC2SGetKartAltasConfig(ClientNode *Client, UINT CfgVersion);

void RequestGetMaster(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetMaster(ClientNode *Client);

void RequestSSCOpenStatus(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseSSCOpenStatus(ClientNode *Client);

void RequestSwitchInterface(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseSwitchInterface(ClientNode *Client);

void RequestSecondGetKartRefitLimitCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseSecondGetKartRefitLimitCfg(ClientNode *Client);

void RequestGetPrivilegeIntroInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetPrivilegeIntroInfo(ClientNode *Client);

void RequestGetActivityCenterInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetActivityCenterInfo(ClientNode *Client);

void NotifyTopUIItemInfo(ClientNode *Client);

void NotifyRedPointInfo(ClientNode *Client);

void RequestGetRandomTask(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetRandomTask(ClientNode *Client);

void RequestGetItemTimeLimtCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestGetShapeRefitCfgNew(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponsetGetItemTimeLimtCfg(ClientNode *Client, UINT UpdateTime);

void RequestGetShapeRefitCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetShapeRefitCfg(ClientNode *Client);
void ResponseGetShapeRefitCfgTest(ClientNode *Client);

void RequestWeRelayCommonCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseWeRelayCommonCfg(ClientNode *Client);

void RequestC2GSign3Operate(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseC2GSign3Operate(ClientNode *Client, UCHAR OperateType);

void NotifyClientAddItem(ClientNode *Client, UINT ItemNum, ItemInfo *Items);
void ResponseRegister(ClientNode *Client, const char *Nickname, BYTE Gender, BYTE Country, UINT ItemNum, ItemInfo *Items);
void NotifyUpdateKartSkillStoneInfo(ClientNode *Client, int KartID);
void NotifyUpdateKartSkillStoneInfo2(ClientNode *Client, int KartID, int SkinID);
void NotifyUpdateKartSkillStoneInfo3(ClientNode *Client, int KartID, int SkinID);
void NotifyUpdateKartSkillStoneInfo4(ClientNode* Client, int KartID, int SkinID,int buffID);
void ResponseLogin(ClientNode *Client, UINT ClientVersion, UINT LoginType, UINT ItemNum, ItemInfo *Items, bool HasMoreInfo);
void NotifyLoginMoreInfo(ClientNode *Client, UINT ItemNum, ItemInfo *Items, bool HasNext);

void RequestDeleteOneItem(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseDeleteOneItem(ClientNode *Client, int ItemID, int ResultID, const char *Reason);

void RequestRefitKart(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseRefitKart(ClientNode *Client, int KartItemId, const char *Reason, short inc_SJetWeight, short inc_AccuWeight, short inc_JetWeight, short inc_WWeight, short inc_SpeedWeight, short inc_DriftVecWeight, short RefitTimes, int Idx);

void RequestHyperSpaceJumboGetCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseHyperSpaceJumboGetCfg(ClientNode *Client, UCHAR CfgType);

void RequestTimerChallenge2ndMoreHallInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseTimerChallenge2ndMoreHallInfo(ClientNode *Client);

void RequestPreGetCommonBoxAward(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponsePreGetCommonBoxAward(ClientNode *Client, USHORT Type, int AwardID, UCHAR IsShowAllAward, int EchoClient);
void RequestAddToMatchQueue(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseAddToMatchQueue(ClientNode *Client, USHORT matchQueueType);

void NotifyMatchResult(ClientNode *Client, USHORT matchQueueType);

void RequestEnterMatchRoom(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseEnterMatchRoom(ClientNode *Client);

void RequestLeaveGame(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestLeaveGame2(ClientNode *Client);
void ResponseLeaveGame(ClientNode *Client);

void RequestModifyPlayerSignature(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseModifyPlayerSignature(ClientNode *Client, char *Signature);

void RequestFindPlayerByQQ(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseFindPlayerByQQ(ClientNode *Client, UINT DstUin, UCHAR AreaIdDst, UCHAR SimpleInfo);
void ResponseFindPlayerByQQ2(ClientNode *Client, UINT DstUin);

void RequestGetAchieveList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetAchieveList(ClientNode *Client, UINT DstUin);

void NotifyRandRoomNameList(ClientNode *Client);

void NotifyTopListDesc(ClientNode *Client);
void RequestTopList(ClientNode *Client, BYTE *Body, size_t BodyLen); // 461
void ResponseTopList(ClientNode *Client, USHORT ID, UCHAR Type);	 // 461

void NotifyUseItemResult(ClientNode *Client, int ItemID, int UseItemNum, int CurrentItemNum);

#include "Room.h"
#include "Dance.h"
#include "Outdoor.h"

void RequestBuyCommodity(ClientNode *Client, BYTE *Body, size_t BodyLen);

void RequestSaveShapeRefit(ClientNode *Client, BYTE *Body, size_t BodyLen); // 316
void ResponseSaveShapeRefit(ClientNode *Client, int KartID, int RefitItemNum, int RefitItemID[], int ShapeSuitID);
void ResponseSaveShapeRefit2(ClientNode *Client, int KartID, int ShapeSuitID);
void ResponseSaveShapeRefit4(ClientNode *Client, int KartID);

void RequestDoBagItemShowOperate(ClientNode *Client, BYTE *Body, size_t BodyLen); // 28412

void RequestCheckName(ClientNode *Client, BYTE *Body, DWORD BodyLen);
void ResponseCheckName(ClientNode *Client, const char *NewNickName, const char *Reason);

void RequestModifyNickname(ClientNode *Client, BYTE *Body, DWORD BodyLen);							// 154
void ResponseModifyNickname(ClientNode *Client, const char *NewModifyNickName, const char *Reason); // 154

void NotifyClientUpdateMoney(ClientNode *Client, int SuperMoney, int Coupons, int LuckMoney, UINT SpeedCoin);

void RequestReportCommonEvent(ClientNode *Client, BYTE *Body, size_t BodyLen); // 24028
void NotifyClientUpdateMoney2(ClientNode *Client);							   // 528

void RequestStartSingleTask(ClientNode *Client, BYTE *Body, size_t BodyLen);									// 138
void ResponseStartSingleTask(ClientNode *Client, int a, int b);													// 138
void RequestChangeTeam(ClientNode *Client, BYTE *Body, size_t BodyLen);											// 128
void ResponseChangeTeam(ClientNode *Client);																	// 128
void RequestOpenBox(ClientNode *Client, BYTE *Body, size_t BodyLen);											// 168
void ResponseOpenBox(ClientNode *Client, int TreasureBoxID, ItemInfo *aItemStatus, int itemNum, int ItemIDNum); // 168
void ResponseOpenSignBox(ClientNode *Client);																	// 128
void ResponseTowerOpenBox(ClientNode *Client);																	// 128
void NotifyChangeItemStatus(ClientNode *Client, UINT Uin, USHORT ItemNum, ItemStatus *ItemStatus);
void NotifyChangeItemStatus2(ClientNode *Client, UINT Uin, USHORT ItemNum, ItemStatus *ItemStatus);
void RequestUseItem(ClientNode *Client, BYTE *Body, size_t BodyLen);				   // 158
void ResponseUseItem(ClientNode *Client);											   // 158
void RequestChangeIdentity(ClientNode *Client, BYTE *Body, size_t BodyLen);			   // 131
void ResponseChangeIdentity(ClientNode* Client, char SetOrUnset, char IsExRightFlag, UINT IdentityFlag);									   // 131
void NotifyAntiBot(ClientNode *Client);												   // 538
void AckAntiBot(ClientNode *Client, BYTE *Body, size_t BodyLen);					   // 538
void RequestWorldOnlineInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);		   // 1400
void ResponseWorldOnlineInfo(ClientNode *Client);									   // 1400
void RequestEnterPersonalGarden(ClientNode *Client, BYTE *Body, size_t BodyLen);	   // 20137
void ResponseEnterPersonalGarden(ClientNode *Client);								   // 20137
void RequestQuickRPC(ClientNode *Client, BYTE *Body, size_t BodyLen);				   // 24251
void ResponseQuickRPC(ClientNode *Client, USHORT MsgLen);							   // 24251
void RequestGetKeyOfBox(ClientNode *Client, BYTE *Body, size_t BodyLen);			   // 189
void ResponseGetKeyOfBox(ClientNode *Client, UINT Type, UINT ID);					   // 189
void RequestGetBoxItems(ClientNode *Client, BYTE *Body, size_t BodyLen);			   // 190
void ResponseGetBoxItems(ClientNode *Client);										   // 190
void RequestGetNewHandCritBoxAwardCfg(ClientNode *Client, BYTE *Body, size_t BodyLen); // 313
void ResponseGetNewHandCritBoxAwardCfg(ClientNode *Client, int itemID);				   // 313
void RequestGetCommodityExchangeItems(ClientNode *Client, BYTE *Body, size_t BodyLen); // 171
void ResponseGetCommodityExchangeItems(ClientNode *Client);							   // 171
void RequestExchange(ClientNode *Client, BYTE *Body, size_t BodyLen);				   // 160
void ResponseExchange(ClientNode *Client, UINT Time, int ExchangeID);				   // 160
void RequestGetExchangeDesc(ClientNode *Client, BYTE *Body, size_t BodyLen);		   // 936
void ResponseGetExchangeDesc(ClientNode *Client);									   // 936
void RequestGetLuckAwardList(ClientNode *Client, BYTE *Body, size_t BodyLen);		   // 172
void ResponseGetLuckAwardList(ClientNode *Client, UINT Time, int ExchangeID);		   // 172
void RequestInvitePlayer(ClientNode *Client, BYTE *Body, size_t BodyLen);			   // 159
void ResponseInvitePlayer(ClientNode *Client, int dstuin, bool type);				   // 159
void RequestC2GGetPlayerStatus(ClientNode *Client, BYTE *Body, size_t BodyLen);		   // 24053
void ResponseC2GGetPlayerStatus(ClientNode *Client, USHORT Num, ItemStatus* aItemStatus);								   // 24053
void ResponseGetPropS(ClientNode *Client, int index);
void NotifyGetProp(ClientNode *Client);													 // 516
void RequestReportFeedBack(ClientNode *Client, BYTE *Body, size_t BodyLen);				 // 934
void RequestGetKartTransformersInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);	 //?
void ResponseGetKartTransformersInfo(ClientNode *Client, UINT KartTransID);				 // 28331
void RequestReportAntiCollisionDataBuff(ClientNode *Client, BYTE *Body, size_t BodyLen); // 26114
void RequestCompositeSkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen);		 // 905
void ResponseCompositeSkillStone(ClientNode *Client, BYTE *Body, size_t BodyLen);		 // 905
void RequestReportDrift(ClientNode *Client, BYTE *Body, size_t BodyLen);				 // 119
void ResponseReportDrift(ClientNode *Client, int PropID, int SuperJetKartFuelInc);		 // 119
void RequestReportCommonAction(ClientNode *Client, BYTE *Body, size_t BodyLen);			 // 20097
void RequestGetMagicHouseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);			 //?
void ResponseGetMagicHouseInfo(ClientNode *Client, USHORT MagicHouseID);				 // 24470
void RequestAddFriend(ClientNode *Client, BYTE *Body, size_t BodyLen);					 // 114
void ResponseAddFriend(ClientNode *Client, UINT DstUin, UCHAR Type, USHORT Src);		 // 114
void RequestGetRankedMatchShowInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);		 //?
void ResponseGetRankedMatchShowInfo(ClientNode *Client);								 // 25000
void RequestChangePetEquipStatus(ClientNode *Client, BYTE *Body, size_t BodyLen);		 //?
void ResponseChangePetEquipStatus(ClientNode *Client, int NewStatus);									 // 15398
void NotifyAddPet(ClientNode *Client);
void NotifyUseItemMagic(ClientNode* Client, int ItemID, UINT SrcUin, UINT DstUin);
void NotifyUseItemPK(ClientNode *Client, int ItemID, UINT DstUin, bool DstType, UINT Uin);
bool NotifyUseItem(ClientNode *Client, UINT CUin, UINT ItemID, int UseItemNum);
void ResponseC2SGetTimesExperienceInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseC2SReceiveTimesExperience(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseC2SFrostThawTimesExperience(ClientNode *Client, BYTE *Body, size_t BodyLen);

void ResponseGetTeamList(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseC2GGetSignInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseC2GSandglassInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseSetShortCutSet(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetShortCutSet(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetMagicLampCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseMagicLampLightOn(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetMagicCupCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetTempBoxItemInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseMagicCupGetAward(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseMagicCupExchangePiece(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseMagicHouseCallOn(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetDailyExtractShowInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseChooseAwardForDailyExtract(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseDailyExtractInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetMagicCubesInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseGetAward(ClientNode *Client, BYTE *Body, size_t BodyLen);
void RequestChangeHammerStatus(ClientNode *Client, BYTE *Body, size_t BodyLen); // 359
void ResponseChangeHammerStatus(ClientNode *Client, int EquipedHammerID);		// 359
#include "Pet.h"
void RequestQuickEnterRoom(ClientNode *Client, BYTE *Body, size_t BodyLen);				 // 106
void ResponseQuickEnterRoom(ClientNode *Client);										 // 106
void RequestCancelMatchQueue(ClientNode *Client, BYTE *Body, size_t BodyLen);			 // 20080
void ResponseCancelMatchQueue(ClientNode *Client, UINT matchQueueType, UCHAR IsAllArea); // 20080
void getKartSkin(ClientNode *Client);
void getGameMode(ClientNode *Client);
void ChangKartMode(ClientNode *Client, UINT kartID);
void ResponseShapeLegendOperate2(ClientNode *Client, int ShapeID, UCHAR ShapeLevel, int NewShowChoice);
void ResponseShapeLegendOperateNew(ClientNode *Client, int ShapeID, UCHAR ShapeLevel, int NewShowChoice);

void LoadKartConfig();
void LoadTaskConfig();
void ResponseGetBattleModeCarSkillInfoList(ClientNode *Client);								// 10929
void RequestGetBattleModeRoleSkillInfoList(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void NotifyKickPlayer(ClientNode *Client, const char *Reason, UINT KickType);				// 460
void ResponseGetFavoriteTask(ClientNode *Client, BYTE *Body, size_t BodyLen);

void RequestG2DMagicCardGetCardAward(ClientNode *Client, BYTE *Body, size_t BodyLen); // 12007
void ResponseG2DMagicCardGetCardAward(ClientNode *Client, int cardID);				  // 12007

void RequestC2GMagicCardGetNews(ClientNode *Client, BYTE *Body, size_t BodyLen);		 //?
void ResponseC2GMagicCardGetNews(ClientNode *Client);									 // 12000
void NotifyC2GMagicCardNotifyCardsEverHad(ClientNode *Client);							 // 12001
void RequestC2GMagicCardGetExchangeInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseC2GMagicCardGetExchangeInfo(ClientNode *Client);							 // 12002
void RequestC2GMagicCardCompound(ClientNode *Client, BYTE *Body, size_t BodyLen);		 //?
void ResponseC2GMagicCardCompound(ClientNode *Client);									 // 12003
void RequestC2GMagicCardExchange(ClientNode *Client, BYTE *Body, size_t BodyLen);		 //?
void ResponseC2GMagicCardExchange(ClientNode *Client);									 // 12004
void RequestC2GMagicCardTransform(ClientNode *Client, BYTE *Body, size_t BodyLen);		 //?
void ResponseC2GMagicCardTransform(ClientNode *Client);									 // 12005
void NotifyC2GMagicCardNotifyGenCardAward(ClientNode *Client);							 // 12006
void RequestSecondRefitKart(ClientNode *Client, BYTE *Body, size_t BodyLen);			 // 21110
void Response_28346(ClientNode *Client, BYTE *Body, size_t BodyLen);
void NotifySingleGameBegin(ClientNode *Client, std::string data, int len);
void NotifyNPCDoAction(ClientNode *Client);
void ResponseSecondRefitKart(ClientNode *Client, int KartItemId, int RefitItemID, const char *Reason, short inc_Speed2Weight, short inc_DriftVecWeight, short inc_AdditionalZSpeedWeight, short RefitTimes, int Idx); // 21110
void NotifyG2CUpdateSkateTaskLevel(ClientNode *Client);																																								  // 778
void RequestSkateSkillUpgrade(ClientNode *Client, BYTE *Body, size_t BodyLen);																																		  // 10927
void ResponseSkateSkillUpgrade(ClientNode *Client, UINT SkillId);																																					  // 10927
void NotifyG2CSkateCfg(ClientNode *Client);																																											  // 10928
void NotifySkateLoveAdd(ClientNode *Client);																																										  // 20117
void RequestSkatePropRoomZan(ClientNode *Client, BYTE *Body, size_t BodyLen);																																		  // 24412
void ResponseSkatePropRoomZan(ClientNode *Client);																																									  // 24412
void NotifySkatePropRoomZanInfo(ClientNode *Client);																																								  // 24413
void NotifySkatePropRoomZanStatus(ClientNode *Client);																																								  // 24414
void NotifySkatePropCoinChangeInfo(ClientNode *Client);																																								  // 24415
void NotifySingleRaceBegin(ClientNode *Client);
void NotifySingleRaceBeginDiy(ClientNode *Client, UINT CountDownTime);
void NotifySingleRaceFinish(ClientNode *Client);								// 532
void NotifySingleRaceOver(ClientNode *Client);									// 533
void NotifySingleGameOver(ClientNode *Client);									// 534
void NotifyKickFromGame(ClientNode *Client, const char *Reason, UINT KickType); // 460
void RequestReportSkillStoneTakeEffect(ClientNode *Client, BYTE *Body, size_t BodyLen);
void NotifyOtherSkillStoneTakeEffect(ClientNode *Client, int StoneID);
void NotifyMatchSkillStoneInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); // 宝石通知其他玩家
void RequestPVPLogin(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponsePVPLogin(ClientNode *Client);
void NotifyUseProp(ClientNode* Client, UINT Uin, UINT DstUin, UINT ProdID, UINT ProdIndex, int CurrentPosition1, int CurrentPosition2, int CurrentPosition3, short NewID, BYTE DstType, int ItemID, int StoneSkillType, UINT LapTime, std::vector<UINT> DstUins, BYTE PropUseMode, std::vector<UINT> QueryUins, UCHAR Position, int Status, UINT PropPosIdx, UINT PropSecType, UINT SubPropID);
void NotifySingleGameBeginTask110(ClientNode *Client);
// void NotifySingleGameBeginTask1406(ClientNode* Client);
// void NotifySingleGameBeginTask1407(ClientNode* Client);
// void NotifySingleGameBeginTask1408(ClientNode* Client);
// void NotifySingleGameBeginTask1409(ClientNode* Client);
void NotifySingleGameBeginTask1701(ClientNode *Client);
void RequestReportCurrentOpt(ClientNode *Client, BYTE *Body, size_t BodyLen);				   // 139
void ResponseReportCurrentOpt(ClientNode *Client, short OptType, int OptID, int SpecialValue); // 139

void NotifyClientVipFlag(ClientNode *Client);
void NotifyClientUpdateNobleInfo(ClientNode *Client);							 // 961
void NotifyClientUpdateEmperorInfo(ClientNode *Client);							 // 978
void NotifyClientUpdateLoverVipInfo(ClientNode *Client);						 // 13800
void NotifyClientUpdatePersonalGardenBaseInfo(ClientNode *Client);				 // 20301
void NotifyClientUpdateConsumeVipRealteInfo(ClientNode *Client);				 // 16062
void NotifyClientUpdateGuildVipInfo(ClientNode *Client);						 // 12031
void NotifyPetEvolution(ClientNode *Client);									 // 584
void NotifyCustomMapList(ClientNode *Client);									 // 631
void RequestUpdateCustomMapList(ClientNode *Client, BYTE *Body, size_t BodyLen); // 1651
void ResponseUpdateCustomMapList(ClientNode *Client);							 // 1651
void RequestGetAllWakeKartInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //16068
void ResponseGetAllWakeKartInfo(ClientNode* Client); //16068
void NotifyPlayerLimitedWakeKartInfo(ClientNode *Client);							// 16072
void NotifyChatGameOver(ClientNode *Client, UINT BaseMode, UINT SubMode);			// 456
void NotifyBorderGameCondition(ClientNode *Client);									// 544
void RequestGetThemeHouseShowInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);	//?
void ResponseGetThemeHouseShowInfo(ClientNode *Client);								// 24501
void NotifyClientMoreThemeHouseShowInfo(ClientNode *Client);						// 24503
void RequestReceThemeItemInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);		//?
void ResponseReceThemeItemInfo(ClientNode *Client);									// 24500
void NotifyClientCollectDressUpValue(ClientNode *Client);							// 24502
void RequestGetConsumeScoreConfUrl(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseGetConsumeScoreConfUrl(ClientNode *Client, UCHAR Md5StrLen);			// 11471
void NotifyAddFriendResult(ClientNode *Client, UINT SrcUin, UINT DstUin, UCHAR Type, USHORT Src);
void NotifyBeAddedAsFriend(ClientNode *Client, ClientNode *DstUin, UCHAR Type, USHORT Src); // 507
void AckBeAddedAsFriend(ClientNode *Client, BYTE *Body, size_t BodyLen);
void NotifyGenGameEndBoxAward(ClientNode *Client);
void RequestGetDropInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); // 24043
void ResponseGetDropInfo(ClientNode *Client);							 // 24043
void NotifyBugletTalk(ClientNode *SrcClient, ClientNode *Client, USHORT ContentLength, char *Content, BYTE BugletType);
void NotifyBroadcast(ClientNode *Client);
void NotifyBugletTalkALL(ClientNode *SrcClient, USHORT ContentLength, char *Content, BYTE BugletType);

// 情侣
void RequestEngage(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseEngage(ClientNode *Client, UINT DstUin, int EngageType);
void NotifyBeEngaged(ClientNode *Client);														// 572
void AckBeEngaged(ClientNode *Client, BYTE *Body, size_t BodyLen);								// 572
void NotifyEngageResult(ClientNode *Client, UINT ResultID, ClientNode *DstUin, int EngageType); // 573
void NotifyEngageResult2(ClientNode *Client, UINT ResultID, int EngageType);					// 573
void NotifyEngageResult3(ClientNode *Client);					// 573
void NotifyEngageResult4(ClientNode* Client, BYTE*& p);
void NotifyEngageResult5(UINT Uin, BYTE*& p);
void RequestCancelEngage(ClientNode *Client, BYTE *Body, size_t BodyLen);						// 167
void ResponseCancelEngage(ClientNode *Client, UINT DstUin);													// 167
void NotifyCancelEngage(ClientNode* Client, UINT SrcUin, std::string NickName, UINT DstUin, int EngageType);													// 574
void NotifyBeEngaged(ClientNode *SrcClient, ClientNode *Client, int RingID, int EngageType);
void RequestDeleteFriendGroup(ClientNode* Client, BYTE* Body, size_t BodyLen); //117
void ResponseDeleteFriendGroup(ClientNode* Client); //117
// 邀请好友
void NotifyInviteResult(ClientNode *Client);					   // 568
void NotifyBeInvited(ClientNode *Client, ClientNode *Dest);		   // 569
void AckBeInvited(ClientNode *Client, BYTE *Body, size_t BodyLen); // 569

// 情侣宝宝
void RequestRaiseLoveBabyInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);											// 24350
void ResponseRaiseLoverBabyInfo(ClientNode *Client, UCHAR BabySeq, short LoveBabyGender, UINT InviteUin, UINT ViceUin); // 24350
void NotifyClientInviteRaiseLoveBabyFailedInfo(ClientNode *Client);														// 24361
void NotifyClientReceiveRaiseLoveBabyInvition(ClientNode *Client);														// 24361
void RequestRaiseLoverBabyDirectlyInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);									// 24360
void RequestGetLoveBabyInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);											// 24352
void ResponseGetLoveBabyInfo(ClientNode *Client);																		// 24352
void NotifyClientLoveBabyInfoWhenLogIn(ClientNode* Client); //24370
void ResponseGetLoveCrystalInfo(ClientNode* Client); //24784
void RequestGetLoveCrystalInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void RequestGetBabyDeleteAwardInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseGetBabyDeleteAwardInfo(ClientNode* Client); //24793
void RequestChangeLoverBabyEquipStatus(ClientNode* Client, BYTE* Body, size_t BodyLen); //24386
void ResponseChangeLoverBabyEquipStatus(ClientNode* Client, short EquipStatus); //24386
void RequestInteractWithLoveBaby(ClientNode* Client, BYTE* Body, size_t BodyLen); //24355
void ResponseInteractWithLoveBaby(ClientNode* Client); //24355
// 精灵
void RequestGetWizardHomeBaseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseGetWizardHomeBaseInfo(ClientNode *Client);							   // 11474
void NotifyMuchPlayerInfo(ClientNode *Client);									   // 1718
void NotifyMuchPlayerMoreInfo(ClientNode *Client);								   // 1719
void NotifyMuchPlayerGameBegin(ClientNode *Client);								   //?
void NotifyMuchPlayerKartPhysParamInfo(ClientNode *Client);						   //?
void NotifyMuchPlayerKartPhysParamMoreInfo(ClientNode *Client);					   //?
void NotifyMuchPlayerRaceOver(ClientNode *Client);								   // 1721
void NotifyChangeHammerStatus(ClientNode *Client, int EquipedHammerID, UINT SrcUin);
void RequestC2GGetWizardBaseInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseC2GGetWizardBaseInfo(ClientNode *Client);							  // 11526
void RequestChangeCarryWizard(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseChangeCarryWizard(ClientNode* Client, USHORT WizardID, bool Carry); //11506

void RequestRankedMatchPersonalPanelInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseRankedMatchPersonalPanelInfo(ClientNode *Client);							  // 25012
void RequestRankedMatchPannelOperate(ClientNode *Client, BYTE *Body, size_t BodyLen);	  //?
void ResponseRankedMatchPannelOperate(ClientNode *Client, UINT OperateType);			  // 25013
void NotifyAppellationList(ClientNode *Client);
void RequestWearAppellation(ClientNode *Client, BYTE *Body, size_t BodyLen);	// 1618
void ResponseWearAppellation(ClientNode *Client, UCHAR Type);					// 1618
void RequestUnwearAppellation(ClientNode *Client, BYTE *Body, size_t BodyLen);	// 1619
void ResponseUnwearAppellation(ClientNode *Client, UCHAR Type);					// 1619
void RequestGetShopNonSaleInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); // 20126
void ResponseGetShopNonSaleInfo(ClientNode *Client);							// 20126

void RequestGetSpecialBroadcasts(ClientNode *Client, BYTE *Body, size_t BodyLen); // 362
void ResponseGetSpecialBroadcasts(ClientNode *Client, bool Type);							  // 362

void RequestC2GGetSpecialTaskData(ClientNode *Client, BYTE *Body, size_t BodyLen); // 231
void ResponseC2GGetSpecialTaskData(ClientNode *Client);							   // 231

void RequestGetMapMedalCfg(ClientNode *Client, BYTE *Body, size_t BodyLen); // 20086
void ResponseGetMapMedalCfg(ClientNode *Client);							// 20086
void NotifyPlayerDoAction(ClientNode *Client, short ActionID);
void RequestGetPetSkillList(ClientNode *Client, BYTE *Body, size_t BodyLen); // 440
void ResponseGetPetSkillList(ClientNode *Client);							 // 440
UINT GetAppellationLevel(UINT ID);
void RequestQsLabCommittalTaskInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseQsLabCommittalTaskInfo(ClientNode *Client);							// 28419

void RequestGetPwdRedEnvelopeContent(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseGetPwdRedEnvelopeContent(ClientNode *Client);							  // 24724
void RequestSendPwdRedEnvelope(ClientNode *Client, BYTE *Body, size_t BodyLen);		  //?
void ResponseSendPwdRedEnvelope(ClientNode *Client);								  // 24723
void RequestGetPlayerNonSaleItemInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); // 20125
void ResponseGetPlayerNonSaleItemInfo(ClientNode *Client);							  // 20125

// 车队
void RequestCheckGuildName(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15017
void ResponseCheckGuildName(ClientNode *Client);									  // 15017
void RequestCreateGuild(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15000
void ResponseCreateGuild(ClientNode *Client, char *GuildName, UINT Guid);			  // 15000
void RequestGuildDetail(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15003
void ResponseGuildDetail(ClientNode *Client);										  // 15003
void RequestC2GGetGuildChallengeInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); // 11211
void ResponseC2GGetGuildChallengeInfo(ClientNode *Client);							  // 11211
void NotifyBriefGuildInfo(ClientNode *Client);										  // 15205
void RequestAddMember(ClientNode *Client, BYTE *Body, size_t BodyLen);				  // 15001
void ResponseAddMember(ClientNode *Client);											  // 15001
void NotifyBeAddedToGuild(ClientNode *Client);										  // 15206
void RequestReplyBeAddedToGuild(ClientNode *Client, BYTE *Body, size_t BodyLen);	  // 15014
void NotifyAddToGuildResult(ClientNode *Client);									  // 15210
void RequestApplyJoinGuild(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15010
void ResponseApplyJoinGuild(ClientNode *Client);									  // 15010
void NotifyApplyJoinGuild(ClientNode *Client);										  // 15208
void RequestReplyJoinGuild(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15013
void ResponseReplyJoinGuild(ClientNode *Client);									  // 15013
void RequestRemoveMember(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15002
void ResponseRemoveMember(ClientNode *Client);										  // 15002
void NotifyRemoveMember(ClientNode *Client);										  // 15202
void RequestGetGuildList(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15004
void ResponseGetGuildList(ClientNode *Client);										  // 15004
void RequestFindGuild(ClientNode *Client, BYTE *Body, size_t BodyLen);				  // 15009
void ResponseFindGuild(ClientNode *Client);											  // 15009
void RequestGuildNotes(ClientNode *Client, BYTE *Body, size_t BodyLen);				  //?
void ResponseGuildNotes(ClientNode *Client);										  // 24309
void RequestChangeMemberDuty(ClientNode *Client, BYTE *Body, size_t BodyLen);		  // 15006
void ResponseChangeMemberDuty(ClientNode *Client);									  // 15006
void NotifyChangeMemberDutyResult(ClientNode *Client);								  // 15204
void RequestModifyDutyRight(ClientNode *Client, BYTE *Body, size_t BodyLen);		  // 15007
void ResponseModifyDutyRight(ClientNode *Client);									  // 15007
void RequestModifyGuildAttriute(ClientNode *Client, BYTE *Body, size_t BodyLen);	  // 15008
void ResponseModifyGuildAttriute(ClientNode *Client);								  // 15008
void RequestModifyGuildNotes(ClientNode *Client, BYTE *Body, size_t BodyLen);		  // 15011
void ResponseModifyGuildNotes(ClientNode *Client);									  // 15011
void RequestModifyGuildDesc(ClientNode *Client, BYTE *Body, size_t BodyLen);		  // 15012
void ResponseModifyGuildDesc(ClientNode *Client);									  // 15012
void RequestPlayerGuildInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);		  // 15015
void ResponsePlayerGuildInfo(ClientNode *Client);									  // 15015
void RequestGuildLogInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15016
void ResponseGuildLogInfo(ClientNode *Client);										  // 15016
void RequestCheckGuildName(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 15017
void ResponseCheckGuildName(ClientNode *Client);									  // 15017
void NotifyDismissGuildResult(ClientNode *Client);									  // 15217
void RequestSavingQQshow(ClientNode *Client, BYTE *Body, size_t BodyLen);			  // 356
void ResponseSavingQQshow(ClientNode *Client);										  // 356

// 城邦
void RequestC2GCheckWonderLandName(ClientNode *Client, BYTE *Body, size_t BodyLen); // 895
void ResponseC2GCheckWonderLandName(ClientNode *Client);							// 895
void RequestC2GCreateWonderLand(ClientNode *Client, BYTE *Body, size_t BodyLen);	// 881
void ResponseC2GCreateWonderLand(ClientNode *Client, char *WonderLandName);			// 881
void RequestPlayerAction(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponsePlayerAction(ClientNode *Client, short ActionType);
void RequestInternationalSeriesGetInfo(ClientNode *Client, BYTE *Body, size_t BodyLen); //?
void ResponseInternationalSeriesGetInfo(ClientNode *Client);							// 25110
void NotifyMatchRaceOver(ClientNode *Client);											// 564
void InitClientGameArrt(ClientNode *Client);
void NoitfyDobagStatus(ClientNode *Client);
void RequestDressInfoOperate(ClientNode *Client, BYTE *Body, size_t BodyLen);
void ResponseDressInfoOperate(ClientNode *Client, USHORT OptType, short ItemNum, int ItemID[100], USHORT SuitIdx, UCHAR FileMd5Len);
void NotifyTeamN2OProgressInc(ClientNode *Client, int PropID);
void NotifyReportDrift(ClientNode *Client, UINT PropID, UINT DriftPlayerUin, UINT TeamAccelerateFule);
void RequestGetWareHouseStatus(ClientNode *Client, BYTE *Body, size_t BodyLen);									  // 420
void ResponseGetWareHouseStatus(ClientNode *Client);															  // 420
void RequestGetWareHouseItemList(ClientNode *Client, BYTE *Body, size_t BodyLen);								  // 421
void ResponseGetWareHouseItemList(ClientNode *Client);															  // 421
void ResponseGetWareHouseItemListMoreInfo(ClientNode *Client);													  // 429
void RequestDoWareHouseOperation(ClientNode *Client, BYTE *Body, size_t BodyLen);								  // 422
void ResponseWareHouseOperation(ClientNode *Client, int ItemID, bool OpType);									  // 422
void NotifyWareHouseDelItem(ClientNode *Client);																  // 423
void RequestDoWareHouseOperationBatch(ClientNode *Client, BYTE *Body, size_t BodyLen);							  //?
void ResponseWareHouseOperationBatch(ClientNode *Client, int ItemNun, ItemStatus *Items, bool OpType);			  // 21036
void RequestGetSecondPassInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);									  //?
void NotifySecondPassInfo(ClientNode* Client);
void NotifySecondPassReClose(ClientNode* Client); //21016
void RequestOpSecondPass(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseOpSecondPass(ClientNode* Client, char Op, UINT Status); //21017
void NotifySecondPassOpNotAllowed(ClientNode* Client); //21018														  // 21018
void RequestKartRefitAttrTransfer(ClientNode *Client, BYTE *Body, size_t BodyLen);								  //?
void ResponseRefitKartAttrTransfer(ClientNode *Client, int SrcKartID, int DstKartID, UCHAR ItemType, UCHAR Type); // 1208
void RequestGetKartRefitAttrTransferInfo(ClientNode *Client, BYTE *Body, size_t BodyLen);						  //?
void ResponseGetRefitKartAttrTransferInfo(ClientNode *Client);													  // 1214
void RequestWeRelayCommonCfg(ClientNode *Client, BYTE *Body, size_t BodyLen);									  //?
void ResponseWeRelayCommonCfg(ClientNode *Client);																  // 24391
void RequestGetGameEndBoxAward(ClientNode *Client, BYTE *Body, size_t BodyLen);									  // 24756
void ResponseGetGameEndBoxAward(ClientNode *Client);															  // 24756
void NotifyTaskAward(ClientNode *Client, UINT TaskID, UINT TaskWindowType);
void NotifyUpdateAchieveProgress(ClientNode *Client);			 // 759
void NotifyUpdateKartPhysparam(ClientNode *Client, UINT KartID); //?
void ResponsePointChallengeOpenMainBoard(ClientNode* Client);
void ResponseStartTimerChallenge(ClientNode* Client);
void ResponseStartTimerChallenge2(ClientNode* Client);
void RequestReportTimerChallenge2ndEvent(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseStartBorderGame(ClientNode* Client, USHORT matchQueueType);
void ResponseRankedMatchDoBanMap(ClientNode* Client, UCHAR Type);
void NotifyRankedMatchBanMapResult(ClientNode* Client);
void RequestRankedMatchDoBanMap(ClientNode* Client, BYTE* Body, size_t BodyLen);
void RequestChangeRecommandStatus(ClientNode* Client, BYTE* Body, size_t BodyLen); //275
void ResponseChangeRecommandStatus(ClientNode* Client); //275
void NotifyRecommandResult(ClientNode* Client); //276
void RequestEnter3DRoom(ClientNode* Client);
void RemoveAllClothes(ClientNode* Client);
void RequestC2GLoadFile(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseC2GLoadFile(ClientNode* Client, UCHAR Cmd, UCHAR UuidLen);
void RequestGetQuickRaceTimeRandMap(ClientNode* Client, BYTE* Body, size_t BodyLen);
void ResponseGetQuickRaceTimeRandMap(ClientNode* Client);
void RequestRandomOneNobleID(ClientNode* Client, BYTE* Body, size_t BodyLen); //1652
void ResponseRandomOneNobleID(ClientNode* Client,UCHAR RandType); //1652
void RequestDropRandomNobleID(ClientNode* Client, BYTE* Body, size_t BodyLen); //1653
void RequestReplaceNobleID(ClientNode* Client, BYTE* Body, size_t BodyLen); //1654
void ResponseReplaceNobleID(ClientNode* Client, UINT NobleID); //1654
void RequestGetNonSaleItemConsumeItemList(ClientNode* Client, BYTE* Body, size_t BodyLen); //20128
void ResponseGetNonSaleItemConsumeItemList(ClientNode* Client); //20128
void RequestC2SSearchKartAltas(ClientNode* Client, BYTE* Body, size_t BodyLen); //13705
void ResponseC2SearchKartAltas(ClientNode* Client, UINT DstUin); //13705
void ResponseC2SearchKartAltasMoreInfo(ClientNode* Client); //13703
void NotifyS2CMyKartAltas(ClientNode* Client); //13706
void NotifyS2CMyKartAltasMoreInfo(ClientNode* Client, UINT Index, std::vector<UINT> HasKartAltasList); //13704
void RequestGameLogicCommonReport(ClientNode* Client, BYTE* Body, size_t BodyLen); //636
void ResponseGameLogicCommonReport(ClientNode* Client, USHORT ReportType, UCHAR ParaNum, UINT ParaList[], UCHAR OtherPropPosNum);
void NotifyReplaceProp(ClientNode* Client);
void ResponseGetSuperN2O2(ClientNode* Client);
void RequestGetSpecialActivityInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseGetSpecialActivityInfo(ClientNode* Client); //28374
void RequestSpecialActivityDoLottery(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseSpecialActivityDoLottery(ClientNode* Client); //28375
void NotifySpecialActivityInfo(ClientNode* Client); //24767
void RequestGetForbidRefitKartList(ClientNode* Client, BYTE* Body, size_t BodyLen); //175
void ResponseGetForbidRefitKartList(ClientNode* Client); //175
void NotifyKartActiveCfgInfo(ClientNode* Client);
void RequestGetActivedKartInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //24311
void ResponseGetActivedKartInfo(ClientNode* Client, UINT KartID); //24311
void NotifyPlayerAllActiveKartInfo(ClientNode* Client); //24338
void RequestDoActiveKartInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //24310
void ResponseDoActiveKartInfo(ClientNode* Client, UINT KartID, UINT ActiveSuccNums, UINT ActiveFailedNums, UINT CurrentActiveLevel); //24310
void RequestPointCfg(ClientNode* Client, BYTE* Body, size_t BodyLen); //13711
void ResponsePointCfg(ClientNode* Client); //13711
void RequestUseProp(ClientNode* Client, BYTE* Body, size_t BodyLen); //125
void ResponseUseProp(ClientNode* Client); //125
void RequestKartPetDoAction(ClientNode* Client, BYTE* Body, size_t BodyLen); //348
void NotifyKartPetDoAction(ClientNode* Client, short ActionID); //349
void NotifyPetStrengCfgInfo(ClientNode* Client); //20100
void NotifyPetStrengCfgMoreInfo(ClientNode* Client); //28228
void NotifyPetHuanHuaCfgInfo(ClientNode* Client); //24073

void RequestPetStrengthen(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponsePetStrengthen(ClientNode* Client); //20101
void RequestGetPKPetInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseGetPKPetInfo(ClientNode* Client, UINT DstUin, char Flag, int PetId); //21133
void RequestPetTrialInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponsePetTrialInfo(ClientNode* Client,USHORT TeamID); //21150
void ResponseGetPKPetCfg(ClientNode* Client, BYTE* Body, size_t BodyLen);
void RequestGetPetSkillList(ClientNode* Client, BYTE* Body, size_t BodyLen); //440
void ResponseGetPetSkillList(ClientNode* Client); //440
void RequestChangeOBState(ClientNode* Client, BYTE* Body, size_t BodyLen); //134
void ResponseChangeOBState(ClientNode* Client, UINT OBState); //134
void NotifyChangeOBState(ClientNode* Client, UINT OBState, UINT Uin); //609
void RequestPropEffectResult(ClientNode* Client, BYTE* Body, size_t BodyLen); //126
void NotifyPropEffectResult(ClientNode* Client, UINT Uin, std::vector<UCHAR> RemoveOwnProp, UINT SrcUin, UINT PropID, UINT PropIndex, UINT StoneSkillType, UINT LapTime);
void NotifyPropPoint(ClientNode* Client, UINT Uin, UINT PropID);
void RequestGetBattleModeCarSkillInfoList(ClientNode* Client, BYTE* Body, size_t BodyLen); //?
void ResponseGetBattleModeCarSkillInfoList(ClientNode* Client); //10929
void RequestChangeAimState(ClientNode* Client, BYTE* Body, size_t BodyLen); //127
void NotifyChangeAimState(ClientNode* Client, UINT Uin, UINT DstUin, UINT AimState, UINT DstType);
void RequestGetGroupFriendBaseInfo(ClientNode* Client, BYTE* Body, size_t BodyLen); //148
void ResponseGetGroupFriendBaseInfo(ClientNode* Client, UINT Uin); //148
void NotifyClientXCarEvent(ClientNode* Client); //5100
