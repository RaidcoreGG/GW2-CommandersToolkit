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
#include "RTAPI/RTAPI.h"

#define ADDON_NAME           "Commander's Toolkit"
#define KB_COMMANDERSTOOLKIT "KB_COMMANDERSTOOLKIT"

///----------------------------------------------------------------------------------------------------
/// ETextures Enumeration
///----------------------------------------------------------------------------------------------------
enum ETextures
{
	BoonAlacrity,
	BoonQuickness,
	TagCommander,
	TagLieutenant,
	COUNT
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

	extern Texture*             Textures[ETextures::COUNT];
}

#endif
