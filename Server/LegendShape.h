#pragma once
// MaxConsumCfg
typedef struct KartShapeRefitLegendMaxConsumCfg
{
	DWORD ItemID;
	DWORD ItemNum;
	INT ItemPeriod;

} *LPKartShapeRefitLegendMaxConsumCfg;
// RefitEffectsCfg
typedef struct KartShapeRefitLegendRefitEffectsCfg
{
	WORD EffectID;
	BYTE Level;
	DWORD Value1;
	DWORD Value2;
	std::string Resource;
	std::string EffectRes;
	BYTE Grade;

} *LPKartShapeRefitLegendRefitEffectsCfg;
// LevelCfg
typedef struct KartShapeRefitLegendLevelCfg
{
	BYTE Level;
	// DWORD RefitItem[];
	std::vector<KartShapeRefitLegendRefitEffectsCfg> RefitEffectsInfo;
	WORD DressPoint;
	WORD CarLibraryCollectValue;
	// RefitModify
	// HasTurnModify
	DWORD ShapeSuitID;
	std::vector<KartShapeRefitLegendMaxConsumCfg> ConsumInfo;
	BYTE cTmp1 = 0;
	BYTE cTmp2 = 0;
} *LPKartShapeRefitLegendLevelCfg;
// KartShapeRefitLegendInfo
typedef struct KartShapeRefitLegendInfo
{
	DWORD KartID;
	std::string KarName;
	WORD Grade;
	std::string GradeResource;
	std::vector<KartShapeRefitLegendLevelCfg> LevelInfo;
	KartShapeRefitLegendMaxConsumCfg MaxConsumInfo;
	DWORD ShapeSuitID;
	BYTE LegendSkinType;
} *LPKartShapeRefitLegendInfo;
