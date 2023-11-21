#pragma once
#include <Windows.h>
#include "imgui/imgui.h"
#include "SquadManager.h"
#include "Mumble.h"
#include "nlohmann/json.hpp"
#include <math.h>

#include "Nexus.h"
#include "arcdps.h"

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

struct EvCombatData
{
	cbtevent* ev;
	ag* src;
	ag* dst;
	char* skillname;
	uint64_t id;
	uint64_t revision;
};

const char* KB_CT = "KB_COMMANDERSTOOLKIT";

void PreZhaitan();
void Zhaitan();
void SooWon();

/* proto */
void AddonLoad(AddonAPI* aApi);
void AddonUnload();
void ProcessKeybind(const char* aIdentifer);
void AddonRender();
void AddonShortcut();
UINT AddonWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void OnCombatEvent(void* aEventArgs);

HWND Game;
AddonDefinition* AddonDef;
AddonAPI* APIDefs;
Mumble::Data* MumbleLink;

extern "C" __declspec(dllexport) AddonDefinition* GetAddonDef()
{
	AddonDef = new AddonDefinition();
	AddonDef->Signature = -584326;
	AddonDef->APIVersion = NEXUS_API_VERSION;
	AddonDef->Name = "Commander's Toolkit";
	AddonDef->Version.Major = 1;
	AddonDef->Version.Minor = 0;
	AddonDef->Version.Build = 0;
	AddonDef->Version.Revision = 1;
	AddonDef->Author = "Raidcore";
	AddonDef->Description = "A GW2 addon that allows you to track boon & role coverage in a squad.";
	AddonDef->Load = AddonLoad;
	AddonDef->Unload = AddonUnload;

	return AddonDef;
}

void AddonLoad(AddonAPI* aApi)
{
	APIDefs = aApi;

	/* If you are using ImGui you will need to set these. */
	ImGui::SetCurrentContext(APIDefs->ImguiContext);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree);

	APIDefs->RegisterRender(ERenderType_Render, AddonRender);
	APIDefs->AddSimpleShortcut("QAS_COMMANDERSTOOLKIT", AddonShortcut);
	APIDefs->RegisterWndProc(AddonWndProc);
	APIDefs->RegisterKeybindWithString(KB_CT, ProcessKeybind, "CTRL+Q");
	APIDefs->SubscribeEvent("EV_ARC_COMBATEVENT_LOCAL", OnCombatEvent);
	APIDefs->SubscribeEvent("EV_ARC_COMBATEVENT_SQUAD", OnCombatEvent);

	MumbleLink = (Mumble::Data*)APIDefs->GetResource("DL_MUMBLE_LINK");
}

void AddonUnload()
{
	APIDefs->UnregisterRender(AddonRender);
	APIDefs->RemoveSimpleShortcut("QAS_COMMANDERSTOOLKIT");
	APIDefs->UnregisterWndProc(AddonWndProc);
	APIDefs->UnregisterKeybind(KB_CT);
	APIDefs->UnsubscribeEvent("EV_ARC_COMBATEVENT_LOCAL", OnCombatEvent);
	APIDefs->UnsubscribeEvent("EV_ARC_COMBATEVENT_SQUAD", OnCombatEvent);
	return;
}

void ProcessKeybind(const char* aIdentifier)
{
	/* if KB_COMPASS_TOGGLEVIS is passed, we toggle the compass visibility */
	if (strcmp(aIdentifier, KB_CT) == 0)
	{
		SquadManager::Visible = !SquadManager::Visible;
		return;
	}
}

void AddonRender()
{
	if (SquadManager::Visible)
	{
		SquadManager::DrawWindow(true, true);
	}
}

void AddonShortcut()
{
	ImGui::TextDisabled("Commander's Toolkit");

	ImGui::Checkbox("Squad Manager", &SquadManager::Visible);
	ImGui::Separator();
	/*if (ImGui::Button("Pre-Zhaitan"))
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
	}*/
}

UINT AddonWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto const io = &ImGui::GetIO();

	if (!Game) { Game = hWnd; }

	if (uMsg == WM_KEYDOWN) { if (wParam == VK_F1) { PreZhaitan(); return 0; } }
	if (uMsg == WM_KEYDOWN) { if (wParam == VK_F2) { Zhaitan(); return 0; } }
	if (uMsg == WM_KEYDOWN) { if (wParam == VK_F3) { SooWon(); return 0; } }

	return uMsg;
}

void OnCombatEvent(void* aEventArgs)
{
	EvCombatData* cbtEv = (EvCombatData*)aEventArgs;

	/* ev is null. dst will only be valid on tracking add. skillname will also be null */
	if (!cbtEv->ev)
	{
		/* notify tracking change */
		if (!cbtEv->src->elite)
		{
			/* add */
			if (cbtEv->src->prof)
			{
				//p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				//p += _snprintf_s(p, 400, _TRUNCATE, "agent added: %s:%s (%0llx), instid: %u, prof: %u, elite: %u, self: %u, team: %u, subgroup: %u\n", src->name, dst->name, src->id, dst->id, dst->prof, dst->elite, dst->self, src->team, dst->team);

				if (cbtEv->src->name != nullptr && cbtEv->src->name[0] != '\0' && cbtEv->dst->name != nullptr && cbtEv->dst->name[0] != '\0')
				{
					std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
					bool agUpdate = false;

					for (size_t i = 0; i < SquadManager::SquadMembers.size(); i++)
					{
						if (SquadManager::SquadMembers[i].ID != cbtEv->src->id) { continue; }
						else
						{
							agUpdate = true;
							strcpy_s(SquadManager::SquadMembers[i].CharacterName, cbtEv->src->name);
							SquadManager::SquadMembers[i].Profession = cbtEv->dst->prof;
							SquadManager::SquadMembers[i].Subgroup = cbtEv->dst->team;
							SquadManager::SquadMembers[i].IsSelf = cbtEv->dst->self;
							SquadManager::SquadMembers[i].IsTracked = true;
							SquadManager::SquadMembers[i].LastSeen = time(0);
							SquadManager::PurgeSquadMembers();
							break;
						}
					}

					if (!agUpdate) // if not agUpdate -> add new ag
					{
						Player p;
						p.ID = cbtEv->src->id;
						strcpy_s(p.AccountName, cbtEv->dst->name);
						strcpy_s(p.CharacterName, cbtEv->src->name);
						p.Profession = cbtEv->dst->prof;
						p.Subgroup = cbtEv->dst->team;
						p.IsSelf = cbtEv->dst->self;
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
					if (SquadManager::SquadMembers[i].ID != cbtEv->src->id) { continue; }
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
		if (cbtEv->ev && cbtEv->ev->is_statechange == CBTS_ENTERCOMBAT)
		{
			std::lock_guard<std::mutex> lock(SquadManager::SquadMembersMutex);
			for (size_t i = 0; i < SquadManager::SquadMembers.size(); i++)
			{
				if (SquadManager::SquadMembers[i].ID != cbtEv->src->id) { continue; }
				else
				{
					SquadManager::SquadMembers[i].Subgroup = cbtEv->ev->dst_agent;
					break;
				}
			}
		}
	}
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
	APIDefs->Log(ELogLevel_TRACE, "Distance:");
	APIDefs->Log(ELogLevel_TRACE, std::to_string(VectorDistance(MumbleLink->Context.Compass.Center.X, MumbleLink->Context.Compass.Center.Y, x, y) * 24).c_str());
	APIDefs->Log(ELogLevel_TRACE, "");

	int margin = 0; // margin in pixels 0 if top, 37 if bottom (normal UI)
	if (!MumbleLink->Context.IsCompassTopRight) { margin = 37; }

	/* determine UI scaling */
	float scaling = 1.00f;
	json j = json::parse(MumbleLink->Identity);
	switch (j["uisz"].get<unsigned>())
	{
	case 0: scaling = 0.90f; break;
	case 1: scaling = 1.00f; break;
	case 2: scaling = 1.11f; break;
	case 3: scaling = 1.22f; break;
	}

	// map center in screen pixels relative to bottom/top right
	// half of compass size + margin
	int mapCenterX = (MumbleLink->Context.Compass.Width / 2) * scaling;
	int mapCenterY = ((MumbleLink->Context.Compass.Height / 2) + margin) * scaling;

	/* replace desktop with gw2 window, there are people that don't play full screen, wtf */
	RECT bounds;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &bounds);

	int screenWidth = bounds.right;
	int screenHeight = bounds.bottom;

	// distance from map center in units
	float dX = (x - MumbleLink->Context.Compass.Center.X) * 24;
	float dY = (y - MumbleLink->Context.Compass.Center.Y) * 24;

	// 85px = 2000u
	float pixelsPerUnit = (85.0f / (2000.0f * MumbleLink->Context.Compass.Scale)) * scaling;

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

	APIDefs->Log(ELogLevel_TRACE, std::to_string(lp).c_str());

	return lp;
}
void SetSquadMarker(int marker, float x, float y)
{
	if (VectorDistance(MumbleLink->Context.Compass.Center.X, MumbleLink->Context.Compass.Center.Y, x, y) * 24 > 4000) { return; } // out of placable range

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
void PreZhaitan()
{
	std::thread([]()
		{
			SetSquadMarker(9, MumbleLink->Context.Compass.Center.X, MumbleLink->Context.Compass.Center.Y); Sleep(50);
			SetSquadMarker(1, 34208.9102, 104607.6641); Sleep(50);
			SetSquadMarker(2, 34141.4805, 104607.7109); Sleep(50);
			SetSquadMarker(3, 34175.2344, 104641.8672); Sleep(50);
			SetSquadMarker(5, 34175.1172, 104665.5625); Sleep(50);
			SetSquadMarker(7, 34146.2109, 104578.5000); Sleep(50);
			SetSquadMarker(8, 34204.2109, 104637.5000);
		}).detach();
}
void Zhaitan()
{
	std::thread([]()
		{
			SetSquadMarker(9, MumbleLink->Context.Compass.Center.X, MumbleLink->Context.Compass.Center.Y); Sleep(50);
			SetSquadMarker(1, 34199.2031, 104583.8984); Sleep(50);
			SetSquadMarker(2, 34151.1211, 104583.8594); Sleep(50);
			SetSquadMarker(3, 34175.1875, 104641.7969); Sleep(50);
			SetSquadMarker(4, 34175.1719, 104573.9688); Sleep(50);
			SetSquadMarker(5, 34175.1172, 104665.5625);
		}).detach();
}
void SooWon()
{
	std::thread([]()
		{
			SetSquadMarker(9, MumbleLink->Context.Compass.Center.X, MumbleLink->Context.Compass.Center.Y); Sleep(50);
			SetSquadMarker(1, 34201.5977, 104607.7422); Sleep(50);
			SetSquadMarker(2, 34198.4102, 104619.0781); Sleep(50);
			SetSquadMarker(3, 34181.0078, 104641.7969); Sleep(50);
			SetSquadMarker(5, 34175.1172, 104665.5625);
		}).detach();
}