///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  UtilityInfo.h
/// Description  :  UtilityInfo struct definition.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef UTILITYINFO_H
#define UTILITYINFO_H

#include <cstdint>

///----------------------------------------------------------------------------------------------------
/// EDamageType Enumeration
///----------------------------------------------------------------------------------------------------
enum class EDamageType : uint32_t
{
	None,
	Power,
	Condition,
	Heal
};

///----------------------------------------------------------------------------------------------------
/// UtilityInfo_t Struct
///----------------------------------------------------------------------------------------------------
struct UtilityInfo_t
{
	EDamageType DamageType;

	bool        Might;
	bool        Alacrity;
	bool        Quickness;
	bool        Fury;
	bool        Vulnerability;
};

#endif
