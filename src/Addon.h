///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  Addon.h
/// Description  :  Primary addon functions.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef ADDON_H
#define ADDON_H

#include <windows.h>

#include "nexus/Nexus.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved);

extern "C" __declspec(dllexport) AddonDefinition* GetAddonDef();

///----------------------------------------------------------------------------------------------------
/// Addon Namespace
///----------------------------------------------------------------------------------------------------
namespace Addon
{
	///----------------------------------------------------------------------------------------------------
	/// Load:
	/// 	Called when the addon gets loaded.
	///----------------------------------------------------------------------------------------------------
	void Load(AddonAPI* aApi);

	///----------------------------------------------------------------------------------------------------
	/// Unload:
	/// 	Called when the addon gets unloaded.
	///----------------------------------------------------------------------------------------------------
	void Unload();

	///----------------------------------------------------------------------------------------------------
	/// OnInputBind:
	/// 	Called when a keybind is pressed.
	///----------------------------------------------------------------------------------------------------
	void OnInputBind(const char* aIdentifier, bool aIsRelease);

	void OnGroupMemberJoin(void* aEventArgs);
	void OnGroupMemberLeave(void* aEventArgs);
	void OnGroupMemberUpdate(void* aEventArgs);
}

#endif
