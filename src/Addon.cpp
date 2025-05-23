///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  Addon.cpp
/// Description  :  Primary addon functions.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "Addon.h"

#include <string>

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
	bool                 IsUEEnabled    = false;

	Texture*             Textures[ETextures::COUNT];
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

		G::APIDefs->Textures.LoadFromResource("ICON_COMMANDERSTOOLKIT",       IDB_QUICKACCESS,       G::Module, nullptr);
		G::APIDefs->Textures.LoadFromResource("ICON_COMMANDERSTOOLKIT_HOVER", IDB_QUICKACCESS_HOVER, G::Module, nullptr);

		G::APIDefs->Renderer.Register(ERenderType_Render, GUI::Render);
		G::APIDefs->QuickAccess.Add("QA_COMMANDERSTOOLKIT", "ICON_COMMANDERSTOOLKIT", "ICON_COMMANDERSTOOLKIT_HOVER", KB_COMMANDERSTOOLKIT, "Commander's Toolkit");
		G::APIDefs->QuickAccess.AddContextMenu("QACTX_COMMANDERSTOOLKIT", "QA_COMMANDERSTOOLKIT", GUI::RenderShortcutContextMenu);

		G::APIDefs->Events.Subscribe("EV_ADDON_LOADED",                   (EVENT_CONSUME)OnAddonLoaded);
		G::APIDefs->Events.Subscribe("EV_ADDON_UNLOADED",                 (EVENT_CONSUME)OnAddonUnloaded);

		G::APIDefs->Events.Subscribe(EV_RTAPI_GROUP_MEMBER_JOINED,        (EVENT_CONSUME)Addon::OnGroupMemberJoin);
		G::APIDefs->Events.Subscribe(EV_RTAPI_GROUP_MEMBER_LEFT,          (EVENT_CONSUME)Addon::OnGroupMemberLeave);
		G::APIDefs->Events.Subscribe(EV_RTAPI_GROUP_MEMBER_UPDATED,       (EVENT_CONSUME)Addon::OnGroupMemberUpdate);
		
		G::APIDefs->Events.Subscribe("EV_ARCDPS_SELF_JOIN",               (EVENT_CONSUME)Addon::OnAgentJoin);
		G::APIDefs->Events.Subscribe("EV_ARCDPS_SELF_LEAVE",              (EVENT_CONSUME)Addon::OnAgentLeave);
		G::APIDefs->Events.Subscribe("EV_ARCDPS_SQUAD_JOIN",              (EVENT_CONSUME)Addon::OnAgentJoin);
		G::APIDefs->Events.Subscribe("EV_ARCDPS_SQUAD_LEAVE",             (EVENT_CONSUME)Addon::OnAgentLeave);
		G::APIDefs->Events.Subscribe("EV_UNOFFICIAL_EXTRAS_SQUAD_UPDATE", (EVENT_CONSUME)Addon::OnSquadUpdate);

		G::APIDefs->InputBinds.RegisterWithString(KB_COMMANDERSTOOLKIT, Addon::OnInputBind, "CTRL+Q");

		G::MumbleLink = (Mumble::Data*)       G::APIDefs->DataLink.Get("DL_MUMBLE_LINK");
		G::RTAPI      = (RTAPI::RealTimeData*)G::APIDefs->DataLink.Get(DL_RTAPI);

		if (!G::RTAPI || (G::RTAPI && G::RTAPI->GameBuild == 0))
		{
			G::RTAPI = nullptr;
			G::APIDefs->Events.RaiseNotification("EV_REPLAY_ARCDPS_SELF_JOIN");
			G::APIDefs->Events.RaiseNotification("EV_REPLAY_ARCDPS_SQUAD_JOIN");
		}
	}

	void Unload()
	{
		G::APIDefs->Renderer.Deregister(GUI::Render);
		G::APIDefs->QuickAccess.Remove("QA_COMMANDERSTOOLKIT");
		G::APIDefs->QuickAccess.RemoveContextMenu("QACTX_COMMANDERSTOOLKIT");

		G::APIDefs->Events.Unsubscribe("EV_ADDON_LOADED",                   (EVENT_CONSUME)OnAddonLoaded);
		G::APIDefs->Events.Unsubscribe("EV_ADDON_UNLOADED",                 (EVENT_CONSUME)OnAddonUnloaded);

		G::APIDefs->Events.Unsubscribe(EV_RTAPI_GROUP_MEMBER_JOINED,        (EVENT_CONSUME)Addon::OnGroupMemberJoin);
		G::APIDefs->Events.Unsubscribe(EV_RTAPI_GROUP_MEMBER_LEFT,          (EVENT_CONSUME)Addon::OnGroupMemberLeave);
		G::APIDefs->Events.Unsubscribe(EV_RTAPI_GROUP_MEMBER_UPDATED,       (EVENT_CONSUME)Addon::OnGroupMemberUpdate);
		
		G::APIDefs->Events.Unsubscribe("EV_ARCDPS_SELF_JOIN",               (EVENT_CONSUME)Addon::OnAgentJoin);
		G::APIDefs->Events.Unsubscribe("EV_ARCDPS_SELF_LEAVE",              (EVENT_CONSUME)Addon::OnAgentLeave);
		G::APIDefs->Events.Unsubscribe("EV_ARCDPS_SQUAD_JOIN",              (EVENT_CONSUME)Addon::OnAgentJoin);
		G::APIDefs->Events.Unsubscribe("EV_ARCDPS_SQUAD_LEAVE",             (EVENT_CONSUME)Addon::OnAgentLeave);
		G::APIDefs->Events.Unsubscribe("EV_UNOFFICIAL_EXTRAS_SQUAD_UPDATE", (EVENT_CONSUME)Addon::OnSquadUpdate);

		G::APIDefs->InputBinds.Deregister(KB_COMMANDERSTOOLKIT);

		G::RTAPI = nullptr;
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
			GUI::GetSquadMgr()->ToggleVisible();
			return;
		}
	}

	void OnAddonLoaded(int* aSignature)
	{
		if (!aSignature) { return; }

		if (*aSignature == 620863532)
		{
			G::RTAPI = (RTAPI::RealTimeData*)G::APIDefs->DataLink.Get(DL_RTAPI);
		}
	}
	void OnAddonUnloaded(int* aSignature)
	{
		if (!aSignature) { return; }

		if (*aSignature == 620863532)
		{
			G::RTAPI = nullptr;
		}
	}

	void OnGroupMemberJoin(RTAPI::GroupMember* aGroupMember)
	{
		GUI::GetSquadMgr()->OnGroupMemberJoin(aGroupMember);
	}
	void OnGroupMemberLeave(RTAPI::GroupMember* aGroupMember)
	{
		GUI::GetSquadMgr()->OnGroupMemberLeave(aGroupMember);
	}
	void OnGroupMemberUpdate(RTAPI::GroupMember* aGroupMember)
	{
		GUI::GetSquadMgr()->OnGroupMemberUpdate(aGroupMember);
	}

	void OnAgentJoin(AgentUpdate* aAgentUpdate)
	{
		GUI::GetSquadMgr()->OnAgentJoin(aAgentUpdate);
	}
	void OnAgentLeave(AgentUpdate* aAgentUpdate)
	{
		GUI::GetSquadMgr()->OnAgentLeave(aAgentUpdate);
	}
	void OnSquadUpdate(SquadUpdate* aSquadUpdate)
	{
		GUI::GetSquadMgr()->OnSquadUpdate(aSquadUpdate);
	}
}
