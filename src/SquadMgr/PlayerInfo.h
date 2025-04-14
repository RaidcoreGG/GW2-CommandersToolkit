///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  PlayerInfo.h
/// Description  :  PlayerInfo struct definition.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <string>
#include <cstdint>

#include "RTAPI/RTAPI.hpp"

#include "UtilityInfo.h"
#include "KillproofInfo.h"

///----------------------------------------------------------------------------------------------------
/// EProfession Enumeration
///----------------------------------------------------------------------------------------------------
enum class EProfession : int32_t
{
	Unknown = -1,
	None,
	Guardian,
	Warrior,
	Engineer,
	Ranger,
	Thief,
	Elementalist,
	Mesmer,
	Necromancer,
	Revenant
};

///----------------------------------------------------------------------------------------------------
/// PlayerInfo_t Struct
///----------------------------------------------------------------------------------------------------
struct PlayerInfo_t
{
	RTAPI::GroupMember Member;
	KPMEInfo_t*        KPMEInfo;
	UtilityInfo_t      UtilityInfo;
	char               Note[500];
	long long          HasLeft; /* Timestamp (seconds) */
};

#endif
