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

#include "RTAPI/RTAPI.h"

#include "UtilityInfo.h"

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
	UtilityInfo_t      UtilityInfo;
	char               Note[250];
	long long          HasLeft;     /* Timestamp (seconds) */
};

#endif
