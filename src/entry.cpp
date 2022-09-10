#pragma once
#include <Windows.h>
#include "arcdps.h"
#include "imgui/imgui.h"
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

	// initialize arc settings at startup
	ArcDPS::ArcUISettings = ArcDPS::UISettings(ArcDPS::GetUISettings());
	ArcDPS::ArcModifiers = ArcDPS::Modifiers(ArcDPS::GetModifiers());

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
	auto io = &ImGui::GetIO();
	bool moveLocked = false;
	bool clickLocked = false;
	bool mods = false;
	if (io->KeysDown[ArcDPS::ArcModifiers.Mod1] && io->KeysDown[ArcDPS::ArcModifiers.Mod2]) // modifiers pressed
	{
		mods = true;
	}

	if(ArcDPS::ArcUISettings.IsModMoveLocked) { moveLocked = !mods; }
	else { moveLocked = false; }

	if (ArcDPS::ArcUISettings.IsModClickLocked) { clickLocked = !mods; }
	else { clickLocked = false; }

	if (SquadManager::SquadManager::Visible) { SquadManager::DrawWindow(moveLocked, clickLocked); }

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
			/* add */
			if (src->prof)
			{
				//p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				//p += _snprintf_s(p, 400, _TRUNCATE, "agent added: %s:%s (%0llx), instid: %u, prof: %u, elite: %u, self: %u, team: %u, subgroup: %u\n", src->name, dst->name, src->id, dst->id, dst->prof, dst->elite, dst->self, src->team, dst->team);

				if (src->name != nullptr && src->name[0] != '\0' && dst->name != nullptr && dst->name[0] != '\0')
				{
					std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
					bool agUpdate = false;

					for (size_t i = 0; i < SquadManager::SquadMembers.size(); i++)
					{
						if (SquadManager::SquadMembers[i].ID != src->id) { continue; }
						else
						{
							agUpdate = true;
							strcpy_s(SquadManager::SquadMembers[i].CharacterName, src->name);
							SquadManager::SquadMembers[i].Profession = dst->prof;
							SquadManager::SquadMembers[i].Subgroup = dst->team;
							SquadManager::SquadMembers[i].IsTracked = true;
							SquadManager::SquadMembers[i].LastSeen = time(0);
							SquadManager::PurgeSquadMembers();
							break;
						}
					}

					if (!agUpdate) // if not agUpdate -> add new ag
					{
						Player p;
						p.ID = src->id;
						strcpy_s(p.AccountName, dst->name);
						strcpy_s(p.CharacterName, src->name);
						p.Profession = dst->prof;
						p.Subgroup = dst->team;
						p.IsTracked = true;
						p.LastSeen = time(0);

						SquadManager::SquadMembers.push_back(p);
						SquadManager::PurgeSquadMembers();
					}
				}
			}
			else /* remove */
			{
				//p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				//p += _snprintf_s(p, 400, _TRUNCATE, "agent removed: %s (%0llx)\n", src->name, src->id);

				std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
				for (size_t i = 0; i < SquadManager::SquadMembers.size(); i++)
				{
					if (SquadManager::SquadMembers[i].ID != src->id) { continue; }
					else
					{
						SquadManager::SquadMembers[i].IsTracked = false;
						SquadManager::SquadMembers[i].LastSeen = time(0);
						break;
					}
				}
				SquadManager::PurgeSquadMembers();
			}
		}
	}
	else // combat enter
	{
		if (ev->is_statechange == ArcDPS::CBTS_ENTERCOMBAT)
		{
			std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
			for (size_t i = 0; i < SquadManager::SquadMembers.size(); i++)
			{
				if (SquadManager::SquadMembers[i].ID != src->id) { continue; }
				else
				{
					SquadManager::SquadMembers[i].Subgroup = ev->dst_agent;
					break;
				}
			}
		}
	}

	return 0;
}