#pragma once
#include <Windows.h>
#include "arcdps.h"
#include "imgui/imgui.h"
#include "SquadManager.h"
#include "Mumble.h"
#include "nlohmann/json.hpp"
#include <math.h>

using json = nlohmann::json;

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

HWND Game;

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

	Mumble::Create();

	return &ArcDPS::ArcPluginExports;
}
/* release mod -- return ignored */
uintptr_t Release()
{
	Mumble::Destroy();

	return 0;
}

uintptr_t ImGuiRender(uint32_t not_charsel_or_loading)
{
	ArcDPS::IsCharacterSelectOrLoading = !not_charsel_or_loading;
	ArcDPS::UpdateExports();

	bool movable = ArcDPS::IsWindowMovable();
	bool clickable = ArcDPS::IsWindowClickable();

	if (SquadManager::Visible) { SquadManager::DrawWindow(movable, clickable); }

	return 0;
}

struct KeyLParam
{
	uint32_t RepeatCount : 16;
	uint32_t ScanCode : 8;
	uint32_t ExtendedFlag : 1;
	uint32_t Reserved : 4;
	uint32_t ContextCode : 1;
	uint32_t PreviousKeyState : 1;
	uint32_t TransitionState : 1;

	KeyLParam(uint32_t key)
	{
		RepeatCount = 1;
		ScanCode = MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
		ExtendedFlag = 0;
		Reserved = 0;
		ContextCode = 0;
		PreviousKeyState = 0;
		TransitionState = 0;
	}

	static KeyLParam& Get(LPARAM& lp)
	{
		return *(KeyLParam*)&lp;
	}
};

float VectorDistance(float x1, float y1, float x2, float y2)
{
	return sqrt(powf(x2 - x1, 2) + powf(y2 - y1, 2));
}

void SetCursorCompass(float x, float y)
{
	ArcDPS::LogToArc((char*)"Distance:");
	ArcDPS::LogToArc((char*)std::to_string(VectorDistance(Mumble::Data->Context.MapCenter.X, Mumble::Data->Context.MapCenter.Y, x, y) * 24).c_str());
	ArcDPS::LogToArc((char*)"");

	int margin = 0; // margin in pixels 0 if top, 37 if bottom (normal UI)
	if (!Mumble::Data->Context.IsCompassTopRight) { margin = 37; }

	/* determine UI scaling */
	float scaling = 1.00f;
	json j = json::parse(Mumble::Data->Identity);
	switch (j["uisz"].get<unsigned>())
	{
	case 0: scaling = 0.90f; break;
	case 1: scaling = 1.00f; break;
	case 2: scaling = 1.11f; break;
	case 3: scaling = 1.22f; break;
	}

	// map center in screen pixels relative to bottom/top right
	// half of compass size + margin
	int mapCenterX = (Mumble::Data->Context.Compass.Width / 2) * scaling;
	int mapCenterY = ((Mumble::Data->Context.Compass.Height / 2) + margin) * scaling;

	/* replace desktop with gw2 window, there are people that don't play full screen, wtf */
	RECT bounds;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &bounds);

	int screenWidth = bounds.right;
	int screenHeight = bounds.bottom;

	// distance from map center in units
	float dX = (x - Mumble::Data->Context.MapCenter.X) * 24;
	float dY = (y - Mumble::Data->Context.MapCenter.Y) * 24;

	// 85px = 2000u
	float pixelsPerUnit = (85.0f / (2000.0f * Mumble::Data->Context.MapScale)) * scaling;

	// TODO: ADJUST FOR ROTATION
	/*float yaw = atan2f(Mumble->cam_front.x, Mumble->cam_front.z) * 180 / 3.14159265f; // gets the rotation in degrees; north = 0, clock-wise to 360
	if (yaw < 0) { yaw += 360; }*/

	SetCursorPos(screenWidth - mapCenterX + (dX * pixelsPerUnit), screenHeight - mapCenterY + (dY * pixelsPerUnit));
}

LPARAM GetLPARAM(uint32_t key, bool down, bool sys)
{
	uint64_t lp;
	lp = down ? 0 : 1; // transition state
	lp = lp << 1;
	lp += down ? 0 : 1; // previous key state
	lp = lp << 1;
	lp += 0; // context code
	lp = lp << 1;
	lp = lp << 4;
	lp = lp << 1;
	lp = lp << 8;
	lp += MapVirtualKeyA(key, MAPVK_VK_TO_VSC);
	lp = lp << 16;
	lp += 1;

	ArcDPS::LogToArc((char*)std::to_string(lp).c_str());

	return lp;
}

void SetSquadMarker(int marker, float x, float y)
{
	if (VectorDistance(Mumble::Data->Context.MapCenter.X, Mumble::Data->Context.MapCenter.Y, x, y) * 24 > 4000) { return; } // out of placable range

	/* store cursor pos*/
	POINT point;
	GetCursorPos(&point);

	/* Set cursor to compass location */
	SetCursorCompass(x, y);

	int key = 0;

	switch (marker)
	{
	case 1: key = 0x31; break;
	case 2: key = 0x32; break;
	case 3: key = 0x33; break;
	case 4: key = 0x34; break;
	case 5: key = 0x35; break;
	case 6: key = 0x36; break;
	case 7: key = 0x37; break;
	case 8: key = 0x38; break;
	case 9: key = 0x39; break;
	}

	Sleep(15);

	PostMessage(Game, WM_SYSKEYDOWN, VK_MENU, GetLPARAM(VK_MENU, true, true));//0x20380001); // alt
	Sleep(5);
	PostMessage(Game, WM_KEYDOWN, key, GetLPARAM(key, true, false));//0x20001);
	Sleep(15);
	PostMessage(Game, WM_KEYUP, key, GetLPARAM(key, false, false));//0xC0020001);
	Sleep(5);
	PostMessage(Game, WM_SYSKEYUP, VK_MENU, GetLPARAM(VK_MENU, false, true));//0xC0380001); // alt
	
	Sleep(5);

	/* reset cursor pos */
	SetCursorPos(point.x, point.y);
}

uintptr_t PreZhaitan()
{
	std::thread([]()
		{
			SetSquadMarker(9, Mumble::Data->Context.MapCenter.X, Mumble::Data->Context.MapCenter.Y); Sleep(50);
			SetSquadMarker(1, 34208.9102, 104607.6641); Sleep(50);
			SetSquadMarker(2, 34141.4805, 104607.7109); Sleep(50);
			SetSquadMarker(3, 34175.2344, 104641.8672); Sleep(50);
			SetSquadMarker(5, 34175.1172, 104665.5625); Sleep(50);
			SetSquadMarker(7, 34146.2109, 104578.5000); Sleep(50);
			SetSquadMarker(8, 34204.2109, 104637.5000);
		}).detach();
}
uintptr_t Zhaitan()
{
	std::thread([]()
		{
			SetSquadMarker(9, Mumble::Data->Context.MapCenter.X, Mumble::Data->Context.MapCenter.Y); Sleep(50);
			SetSquadMarker(1, 34199.2031, 104583.8984); Sleep(50);
			SetSquadMarker(2, 34151.1211, 104583.8594); Sleep(50);
			SetSquadMarker(3, 34175.1875, 104641.7969); Sleep(50);
			SetSquadMarker(4, 34175.1719, 104573.9688); Sleep(50);
			SetSquadMarker(5, 34175.1172, 104665.5625);
		}).detach();
}
uintptr_t SooWon()
{
	std::thread([]()
		{
			SetSquadMarker(9, Mumble::Data->Context.MapCenter.X, Mumble::Data->Context.MapCenter.Y); Sleep(50);
			SetSquadMarker(1, 34201.5977, 104607.7422); Sleep(50);
			SetSquadMarker(2, 34198.4102, 104619.0781); Sleep(50);
			SetSquadMarker(3, 34181.0078, 104641.7969); Sleep(50);
			SetSquadMarker(5, 34175.1172, 104665.5625);
		}).detach();
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
	else
	{
		if (ImGui::Button("Pre-Zhaitan"))
		{
			PreZhaitan();
		}
		if (ImGui::Button("Zhaitan"))
		{
			Zhaitan();
		}
		if (ImGui::Button("Soo-Won"))
		{
			SooWon();
		}
	}

	return 0;
}

uintptr_t WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto const io = &ImGui::GetIO();

	if (!Game) { Game = hWnd; }

	/* unfiltered */
	if (io->KeysDown[VK_ESCAPE])
	{
		if (ArcDPS::ArcUISettings.IsClosingWithEscape)
		{
			if (SquadManager::Visible) { SquadManager::Visible = false; return 0; }
		}
	}
	if (uMsg == WM_KEYDOWN) { if (wParam == VK_F1) { PreZhaitan(); return 0; } }
	if (uMsg == WM_KEYDOWN) { if (wParam == VK_F2) { Zhaitan(); return 0; } }
	if (uMsg == WM_KEYDOWN) { if (wParam == VK_F3) { SooWon(); return 0; } }

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
							SquadManager::SquadMembers[i].IsSelf = dst->self;
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
						p.IsSelf = dst->self;
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