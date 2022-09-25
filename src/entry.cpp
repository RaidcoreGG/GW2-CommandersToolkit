#pragma once
#include <Windows.h>
#include "arcdps.h"
#include "imgui/imgui.h"
#include "unofficial_extras/Definitions.h"
#include "SquadManager.h"

/* entry */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: break;
	case DLL_PROCESS_DETACH: break;
	case DLL_THREAD_ATTACH: break;
	case DLL_THREAD_DETACH: break;
	}
	return TRUE;
}
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext * imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion);
extern "C" __declspec(dllexport) void* get_release_addr();

/* proto */
ArcDPS::PluginExports* Initialize();
uintptr_t Release();
uintptr_t ImGuiRender(uint32_t not_charsel_or_loading);
uintptr_t Windows(const char* category); // Windows check boxes in main menu
uintptr_t Combat(ArcDPS::CombatEvent* ev, ArcDPS::Agent* src, ArcDPS::Agent* dst, char* skillname, uint64_t id, uint64_t revision);
uintptr_t WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
uintptr_t WndProcFiltered(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

/* export -- arcdps looks for this exported function and calls the address it returns on client load */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext * imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion)
{
	ImGui::SetCurrentContext((ImGuiContext*)imguictx);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))mallocfn, (void(*)(void*, void*))freefn); // on imgui 1.80+

	ArcDPS::LogFile = (void*)GetProcAddress((HMODULE)arcdll, "e3");
	ArcDPS::LogArc = (void*)GetProcAddress((HMODULE)arcdll, "e8");
	ArcDPS::GetUISettings = (ArcDPS::Export_GetU64)GetProcAddress((HMODULE)arcdll, "e6");
	ArcDPS::GetModifiers = (ArcDPS::Export_GetU64)GetProcAddress((HMODULE)arcdll, "e7");

	return Initialize;
}
/* export -- arcdps looks for this exported function and calls the address it returns on client exit */
extern "C" __declspec(dllexport) void* get_release_addr()
{
	return Release;
}

/* initialize mod -- return table that arcdps will use for callbacks. exports struct and strings are copied to arcdps memory only once at init */
ArcDPS::PluginExports* Initialize()
{
	/* for arcdps */
	memset(&ArcDPS::ArcPluginExports, 0, sizeof(ArcDPS::PluginExports));
	ArcDPS::ArcPluginExports.Signature = 0x4A584326;
	ArcDPS::ArcPluginExports.ImGuiVersion = IMGUI_VERSION_NUM;
	ArcDPS::ArcPluginExports.Size = sizeof(ArcDPS::PluginExports);
	ArcDPS::ArcPluginExports.Name = "Commander's Toolkit";
	ArcDPS::ArcPluginExports.Build = __DATE__ " " __TIME__;
	ArcDPS::ArcPluginExports.ImGuiRenderCallback = ImGuiRender;
	ArcDPS::ArcPluginExports.UIWindows = Windows;
	ArcDPS::ArcPluginExports.CombatCallback = Combat;
	ArcDPS::ArcPluginExports.WndProc = WndProc;
	ArcDPS::ArcPluginExports.WndProcFiltered = WndProcFiltered;

	return &ArcDPS::ArcPluginExports;
}
/* release mod -- return ignored */
uintptr_t Release()
{
	return 0;
}

uintptr_t ImGuiRender(uint32_t not_charsel_or_loading)
{
	ArcDPS::UpdateExports();

	bool movable = ArcDPS::IsWindowMovable();
	bool clickable = ArcDPS::IsWindowClickable();

	if (SquadManager::Visible) { SquadManager::DrawWindow(movable, clickable); }

	return 0;
}

uintptr_t Windows(const char* category)
{
	if (category)
	{
		if (strcmp(category, "squad") == 0)
		{
			ImGui::Checkbox("Squad Manager", &SquadManager::Visible);
		}
	}

	return 0;
}

uintptr_t WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto const io = &ImGui::GetIO();

	/* unfiltered */
	if (io->KeysDown[VK_ESCAPE])
	{
		if (ArcDPS::ArcUISettings.IsClosingWithEscape)
		{
			if (SquadManager::Visible) { SquadManager::Visible = false; return 0; }
		}
	}
	return uMsg;
}
uintptr_t WndProcFiltered(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto const io = &ImGui::GetIO();

	/* mod filtered*/
	if (io->KeysDown[ArcDPS::ArcModifiers.Mod1] && io->KeysDown[ArcDPS::ArcModifiers.Mod2])
	{
		if (io->KeysDown[0x51]) // Q
		{
			SquadManager::Visible = !SquadManager::Visible;
			return 0;
		}
	}

	return uMsg;
}

/* combat callback -- may be called asynchronously, use id param to keep track of order, first event id will be 2. return ignored */
/* at least one participant will be party/squad or minion of, or a buff applied by squad in the case of buff remove. not all statechanges present, see evtc statechange enum */
uintptr_t Combat(ArcDPS::CombatEvent* ev, ArcDPS::Agent* src, ArcDPS::Agent* dst, char* skillname, uint64_t id, uint64_t revision)
{
	/* ev is null. dst will only be valid on tracking add. skillname will also be null */
	if (!ev)
	{
		/* notify tracking change */
		if (!src->elite)
		{
			// only run when names are set and not null
			if (src->name != nullptr && src->name[0] != '\0' && dst->name != nullptr && dst->name[0] != '\0')
			{
				std::string accountName(dst->name);

				// remove ':' at the beginning of the name.
				if (accountName.at(0) == ':') {
					accountName.erase(0, 1);
				}

				/* add */
				if (src->prof)
				{
					std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);

					auto squadMember = SquadManager::FindPlayerByAccountName(accountName);
					if (squadMember != nullptr)
					{
						squadMember->CharacterName = src->name;
						squadMember->Profession = dst->prof;
						squadMember->Subgroup = dst->team;
						squadMember->IsSelf = dst->self;
						squadMember->IsTrackedByArcdps = true; 
						squadMember->LastSeen = time(0);
						SquadManager::PurgeSquadMembers();
					}
					else
					{
						Player p;
						p.ID = src->id;
						p.AccountName = accountName;
						p.CharacterName = src->name;
						p.Profession = dst->prof;
						p.Subgroup = dst->team;
						p.IsSelf = dst->self;
						p.IsTrackedByArcdps = true;
						p.LastSeen = time(0);
						p.AddedBy = AddedBy::Arcdps;

						SquadManager::SquadMembers[accountName] = p;
						SquadManager::PurgeSquadMembers();
					}
				}
				else /* remove */
				{
					std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
					auto squadMember = SquadManager::FindPlayerByAccountName(accountName);
					if (squadMember != nullptr)
					{
						squadMember->IsTrackedByArcdps = false;
						squadMember->LastSeen = time(0);
					}

					SquadManager::PurgeSquadMembers();
				}
			}
		}
	}
	else // combat enter
	{
		if (ev->is_statechange == ArcDPS::CBTS_ENTERCOMBAT)
		{
			std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
			auto squadMember = SquadManager::FindPlayerByID(src->id);
			if (squadMember != nullptr)
			{
				squadMember->Subgroup = ev->dst_agent;
			}
		}
	}

	return 0;
}

void SquadUpdate(const UserInfo* pUpdatedUsers, uint64_t pUpdatedUsersCount)
{
	std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
	for (size_t i = 0; i < pUpdatedUsersCount; i++)
	{
		const auto user = pUpdatedUsers[i];
		auto accountName = std::string(user.AccountName);
		// remove ':' at the beginning of the name.
		if (accountName.at(0) == ':') {
			accountName.erase(0, 1);
		}

		if (user.Role != UserRole::None)
		{
			// User was added/updated in some way
			auto squadMember = SquadManager::FindPlayerByAccountName(accountName);
			if (squadMember != nullptr)
			{
				squadMember->IsTrackedByExtras = true;
				// Extras always takes priority
				squadMember->AddedBy = AddedBy::Extras;
				squadMember->LastSeen = time(0);
				squadMember->Subgroup = user.Subgroup + 1;
			}
			else
			{
				Player p;
				p.AccountName = accountName;
				p.IsSelf = accountName == SquadManager::SelfAccountName;
				p.IsTrackedByExtras = true;
				p.LastSeen = time(0);
				p.Subgroup = user.Subgroup + 1;
				p.AddedBy = AddedBy::Extras;

				SquadManager::SquadMembers[accountName] = p;
			}
		}
		else
		{
			// User was removed from squad
			auto squadMember = SquadManager::FindPlayerByAccountName(accountName);
			if (squadMember != nullptr)
			{
				squadMember->IsTrackedByExtras = false;
				squadMember->LastSeen = time(0);
			}
			// Self left the squad, everyone should be marked untracked
			if (accountName == SquadManager::SelfAccountName)
			{
				SquadManager::MarkAllUntrackedButSelf();
			}
		}
	}
}

void ChatMessage(const ChatMessageInfo* pChatMessage)
{
	// Empty function for future use
}

extern "C" __declspec(dllexport) void arcdps_unofficial_extras_subscriber_init(const ExtrasAddonInfo* pExtrasInfo, void* pSubscriberInfo) {
	// MaxInfoVersion has to be higher to have enough space to hold this object
	if (pExtrasInfo->ApiVersion == 2 && pExtrasInfo->MaxInfoVersion >= 2) {
		auto selfAccountName = std::string(pExtrasInfo->SelfAccountName);
		if (selfAccountName.at(0) == ':')
		{
			selfAccountName.erase(0, 1);
		}
		SquadManager::SelfAccountName = selfAccountName;

		const auto subscriber_info = static_cast<ExtrasSubscriberInfoV2*>(pSubscriberInfo);
		subscriber_info->InfoVersion = 2;
		subscriber_info->SubscriberName = "Commander's Toolkit";
		subscriber_info->SquadUpdateCallback = SquadUpdate;
		subscriber_info->ChatMessageCallback = ChatMessage;
  }
}
