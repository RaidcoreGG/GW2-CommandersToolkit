///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  Addon.cpp
/// Description  :  Primary addon functions.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "Addon.h"

#include <string>

#include "Globals.h"
#include "UI.h"
#include "Version.h"
#include "Remote.h"
#include "resource.h"
#include "imgui/imgui.h"

#include "SquadMgr/SquadMgr.h"

namespace G
{
	HMODULE              Module         = nullptr;
	AddonAPI*            APIDefs        = nullptr;
	Mumble::Data*        MumbleLink     = nullptr;
	Mumble::Identity*    MumbleIdentity = nullptr;
	RTAPI::RealTimeData* RTAPI          = nullptr;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: G::Module = hModule; break;
		case DLL_PROCESS_DETACH: break;
		case DLL_THREAD_ATTACH: break;
		case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}

AddonDefinition* GetAddonDef()
{
	static AddonDefinition s_AddonDef{
		-584326,
		NEXUS_API_VERSION,

		ADDON_NAME,
		{ V_MAJOR, V_MINOR, V_BUILD, V_REVISION },
		"Raidcore",
		"Allows you to track boon & role coverage in a squad and other features.",

		Addon::Load,
		Addon::Unload,
		EAddonFlags_None,

		EUpdateProvider_GitHub,
		REMOTE_URL,
	};

	return &s_AddonDef;
}


namespace Addon
{
	void Load(AddonAPI* aApi)
	{
		G::APIDefs = aApi;

		ImGui::SetCurrentContext((ImGuiContext*)G::APIDefs->ImguiContext);
		ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))G::APIDefs->ImguiMalloc, (void(*)(void*, void*))G::APIDefs->ImguiFree);

		G::APIDefs->Textures.LoadFromResource("ICON_COMMANDERSTOOLKIT",       IDB_ICON,       G::Module, nullptr);
		G::APIDefs->Textures.LoadFromResource("ICON_COMMANDERSTOOLKIT_HOVER", IDB_ICON_HOVER, G::Module, nullptr);

		G::APIDefs->Renderer.Register(ERenderType_Render, SquadMgr::Render);
		G::APIDefs->QuickAccess.Add("QA_COMMANDERSTOOLKIT", "ICON_COMMANDERSTOOLKIT", "ICON_COMMANDERSTOOLKIT_HOVER", KB_COMMANDERSTOOLKIT, "Commander's Toolkit");
		G::APIDefs->QuickAccess.AddContextMenu("QACTX_COMMANDERSTOOLKIT", "QA_COMMANDERSTOOLKIT", UI::RenderShortcutContextMenu);

		G::APIDefs->Events.Subscribe(EV_RTAPI_GROUP_MEMBER_JOINED,  SquadMgr::OnGroupMemberJoin);
		G::APIDefs->Events.Subscribe(EV_RTAPI_GROUP_MEMBER_LEFT,    SquadMgr::OnGroupMemberLeave);
		G::APIDefs->Events.Subscribe(EV_RTAPI_GROUP_MEMBER_UPDATED, SquadMgr::OnGroupMemberUpdate);

		G::APIDefs->InputBinds.RegisterWithString(KB_COMMANDERSTOOLKIT, Addon::OnInputBind, "CTRL+Q");

		G::MumbleLink = (Mumble::Data*)G::APIDefs->DataLink.Get("DL_MUMBLE_LINK");
	}

	void Unload()
	{
		G::APIDefs->Renderer.Deregister(SquadMgr::Render);
		G::APIDefs->QuickAccess.Remove("QA_COMMANDERSTOOLKIT");
		G::APIDefs->QuickAccess.RemoveContextMenu("QACTX_COMMANDERSTOOLKIT");

		G::APIDefs->Events.Unsubscribe(EV_RTAPI_GROUP_MEMBER_JOINED,  SquadMgr::OnGroupMemberJoin);
		G::APIDefs->Events.Unsubscribe(EV_RTAPI_GROUP_MEMBER_LEFT,    SquadMgr::OnGroupMemberLeave);
		G::APIDefs->Events.Unsubscribe(EV_RTAPI_GROUP_MEMBER_UPDATED, SquadMgr::OnGroupMemberUpdate);

		G::APIDefs->InputBinds.Deregister(KB_COMMANDERSTOOLKIT);
	}

	void OnInputBind(const char* aIdentifier, bool aIsRelease)
	{
		if (aIsRelease)
		{
			return;
		}

		std::string bind = aIdentifier;

		if (bind == KB_COMMANDERSTOOLKIT)
		{
			SquadMgr::ToggleVisible();
			return;
		}
	}
}
