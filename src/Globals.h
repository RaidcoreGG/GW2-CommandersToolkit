///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  Globals.h
/// Description  :  Globally shared objects.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef GLOBALS_H
#define GLOBALS_H

#include "nexus/Nexus.h"
#include "mumble/Mumble.h"
#include "RTAPI/RTAPI.hpp"
#include "arcdps_ue/Definitions.h"

#define ADDON_NAME           "Commander's Toolkit"
#define KB_COMMANDERSTOOLKIT "KB_COMMANDERSTOOLKIT"

///----------------------------------------------------------------------------------------------------
/// ETextures Enumeration
///----------------------------------------------------------------------------------------------------
enum ETextures
{
	BtnClose,
	BtnProfile,

	BoonAlacrity,
	BoonQuickness,
	TagCommander,
	TagLieutenant,

	SpecEle,
	SpecEleCat,
	SpecEleTmp,
	SpecEleWvr,
	SpecEleEvk,

	SpecEng,
	SpecEngHls,
	SpecEngMch,
	SpecEngScr,
	SpecEngAma,

	SpecGrd,
	SpecGrdDgh,
	SpecGrdFbd,
	SpecGrdWbd,
	SpecGrdLum,

	SpecMes,
	SpecMesChr,
	SpecMesMir,
	SpecMesVrt,
	SpecMesTbd,

	SpecNec,
	SpecNecHrb,
	SpecNecRpr,
	SpecNecScg,
	SpecNecRit,

	SpecRev,
	SpecRevHer,
	SpecRevRen,
	SpecRevVin,
	SpecRevCnd,

	SpecRgr,
	SpecRgrDrd,
	SpecRgrSlb,
	SpecRgrUtd,
	SpecRgrGal,

	SpecThf,
	SpecThfDde,
	SpecThfDdl,
	SpecThfSpc,
	SpecThfAtq,

	SpecWar,
	SpecWarBrs,
	SpecWarBsw,
	SpecWarSpb,
	SpecWarPar,

	SpecUnknown,

	COUNT
};

///----------------------------------------------------------------------------------------------------
/// SquadUpdate Struct
///----------------------------------------------------------------------------------------------------
struct AgentUpdate
{
	char AccountName[64];
	char CharacterName[64];
	uintptr_t ID;
	uintptr_t InstanceID;
	uint32_t Added;
	uint32_t Target;
	uint32_t Self;
	uint32_t Prof;
	uint32_t Elite;
	uint16_t Team;
	uint16_t Subgroup;
};

///----------------------------------------------------------------------------------------------------
/// SquadUpdate Struct
///----------------------------------------------------------------------------------------------------
struct SquadUpdate
{
	UserInfo* UserInfo;
	uint64_t UsersCount;
};

///----------------------------------------------------------------------------------------------------
/// Globals Namespace
///----------------------------------------------------------------------------------------------------
namespace G
{
	extern HMODULE              Module;
	extern AddonAPI*            APIDefs;
	extern Mumble::Data*        MumbleLink;
	extern Mumble::Identity*    MumbleIdentity;
	extern RTAPI::RealTimeData* RTAPI;
	extern bool                 IsUEEnabled;

	extern Texture*             Textures[ETextures::COUNT];
}

#endif
