#pragma once
#include <Windows.h>
#include "imgui/imgui.h"
#include "SquadManager.h"
#include "mumble/Mumble.h"
#include "nlohmann/json.hpp"
#include <math.h>

#include "nexus/Nexus.h"
#include "arcdps.h"
#include "Shared.h"
#include "resource.h"
#include "Remote.h"
#include "Version.h"

using json = nlohmann::json;

/* entry */
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH: SelfModule = hModule; break;
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

AddonDefinition AddonDef{};

extern "C" __declspec(dllexport) AddonDefinition* GetAddonDef()
{
	AddonDef.Signature = -584326;
	AddonDef.APIVersion = NEXUS_API_VERSION;
	AddonDef.Name = "Commander's Toolkit";
	AddonDef.Version.Major = V_MAJOR;
	AddonDef.Version.Minor = V_MINOR;
	AddonDef.Version.Build = V_BUILD;
	AddonDef.Version.Revision = V_REVISION;
	AddonDef.Author = "Raidcore";
	AddonDef.Description = "Allows you to track boon & role coverage in a squad and other features.";
	AddonDef.Load = AddonLoad;
	AddonDef.Unload = AddonUnload;

	AddonDef.Provider = EUpdateProvider_GitHub;
	AddonDef.UpdateLink = REMOTE_URL;

	return &AddonDef;
}

void AddonLoad(AddonAPI* aApi)
{
	APIDefs = aApi;

	/* If you are using ImGui you will need to set these. */
	ImGui::SetCurrentContext((ImGuiContext*)APIDefs->ImguiContext);
	ImGui::SetAllocatorFunctions((void* (*)(size_t, void*))APIDefs->ImguiMalloc, (void(*)(void*, void*))APIDefs->ImguiFree);

	APIDefs->RegisterRender(ERenderType_Render, AddonRender);
	APIDefs->AddShortcut("QA_COMMANDERSTOOLKIT", "ICON_COMMANDERSTOOLKIT", "ICON_COMMANDERSTOOLKIT_HOVER", KB_CT, "Commander's Toolkit");
	//APIDefs->RegisterWndProc(AddonWndProc);
	APIDefs->RegisterKeybindWithString(KB_CT, ProcessKeybind, "CTRL+Q");
	APIDefs->SubscribeEvent("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", OnCombatEvent);
	APIDefs->SubscribeEvent("EV_ARCDPS_COMBATEVENT_SQUAD_RAW", OnCombatEvent);

	APIDefs->LoadTextureFromResource("ICON_COMMANDERSTOOLKIT", IDB_ICON, SelfModule, nullptr);
	APIDefs->LoadTextureFromResource("ICON_COMMANDERSTOOLKIT_HOVER", IDB_ICON_HOVER, SelfModule, nullptr);

	MumbleLink = (Mumble::Data*)APIDefs->GetResource("DL_MUMBLE_LINK");
}

void AddonUnload()
{
	APIDefs->DeregisterRender(AddonRender);
	APIDefs->RemoveShortcut("QA_COMMANDERSTOOLKIT");
	APIDefs->DeregisterWndProc(AddonWndProc);
	APIDefs->DeregisterKeybind(KB_CT);
	APIDefs->UnsubscribeEvent("EV_ARCDPS_COMBATEVENT_LOCAL_RAW", OnCombatEvent);
	APIDefs->UnsubscribeEvent("EV_ARCDPS_COMBATEVENT_SQUAD_RAW", OnCombatEvent);
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

#include <cmath>

double calculate_angle(ImVec2 pos1, ImVec2 pos2)
{
	float delta_x = pos2.x - pos1.x;
	float delta_y = pos2.y - pos1.y;

	// Calculate the angle using arctangent (in radians)
	float angle_rad = atan2(delta_y, delta_x);

	// Convert radians to degrees
	float angle_deg = angle_rad * 180.0f / 3.14159f;

	// Adjust the angle to be between 0 and 360 degrees
	angle_deg += 90.0f;
	if (angle_deg < 0) {
		angle_deg += 360.0f;
	}

	return angle_deg;
}

bool isInitialCtrl = false;
bool wasCtrlHeld = false;
ImVec2 initialCursor;

bool h_arrow = false;
bool h_circle = false;
bool h_heart = false;
bool h_square = false;
bool h_star = false;
bool h_swirl = false;
bool h_triangle = false;
bool h_cross = false;

void AddonRender()
{
	// use wndproc instead of asynckeystate, else even while tabbed out holding ctrl will trigger it
	//meme FIX MEMEME
	/*bool ctrlHeld = GetAsyncKeyState(VK_CONTROL);
	if (ctrlHeld && !wasCtrlHeld)
	{
		isInitialCtrl = true;
		initialCursor = ImGui::GetMousePos();
	}
	else
	{
		isInitialCtrl = false;
	}
	wasCtrlHeld = ctrlHeld;

	if (ctrlHeld)
	{
		ImVec2 mPos = ImGui::GetMousePos();

		float angle = calculate_angle(initialCursor, mPos);

		if (angle >= 0 && angle < 45.0f)
		{
			h_arrow = true;
			h_circle = h_heart = h_square = h_star = h_swirl = h_triangle = h_cross = false;
		}
		else if (angle >= 45.0f && angle < 90.0f)
		{
			h_circle = true;
			h_arrow = h_heart = h_square = h_star = h_swirl = h_triangle = h_cross = false;
		}
		else if (angle >= 90.0f && angle < 135.0f)
		{
			h_heart = true;
			h_arrow = h_circle = h_square = h_star = h_swirl = h_triangle = h_cross = false;
		}
		else if (angle >= 135.0f && angle < 180.0f)
		{
			h_square = true;
			h_arrow = h_circle = h_heart = h_star = h_swirl = h_triangle = h_cross = false;
		}
		else if (angle >= 180.0f && angle < 225.0f)
		{
			h_star = true;
			h_arrow = h_circle = h_heart = h_square = h_swirl = h_triangle = h_cross = false;
		}
		else if (angle >= 225.0f && angle < 270.0f)
		{
			h_swirl = true;
			h_arrow = h_circle = h_heart = h_square = h_star = h_triangle = h_cross = false;
		}
		else if (angle >= 270.0f && angle < 315.0f)
		{
			h_triangle = true;
			h_arrow = h_circle = h_heart = h_square = h_star = h_swirl = h_cross = false;
		}
		else if (angle >= 315.0f && angle < 360.0f)
		{
			h_cross = true;
			h_arrow = h_circle = h_heart = h_square = h_star = h_swirl = h_triangle = false;
		}

		if (isInitialCtrl)
		{
			ImGui::SetNextWindowPos(ImVec2(mPos.x - 56, mPos.y - 56));
		}

		if (ImGui::Begin("##markers_wheel", (bool*)0, (ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar)))
		{
			ImVec2 intialPos = ImVec2(0, 0);
			if (MW_Grid && MW_Grid->Resource)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(MW_Grid->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Grid = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_GRID", IDB_MW_GRID, SelfModule);
			}

			if (MW_Arrow && MW_ArrowHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_arrow ? MW_Arrow->Resource : MW_ArrowHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Arrow = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_ARROW", IDB_MW_ARROW, SelfModule);
				MW_ArrowHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_ARROW_HOVER", IDB_MW_ARROW_HOVER, SelfModule);
			}

			if (MW_Circle && MW_CircleHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_circle ? MW_Circle->Resource : MW_CircleHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Circle = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_CIRCLE", IDB_MW_CIRCLE, SelfModule);
				MW_CircleHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_CIRCLE_HOVER", IDB_MW_CIRCLE_HOVER, SelfModule);
			}

			if (MW_Heart && MW_HeartHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_heart ? MW_Heart->Resource : MW_HeartHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Heart = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_HEART", IDB_MW_HEART, SelfModule);
				MW_HeartHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_HEART_HOVER", IDB_MW_HEART_HOVER, SelfModule);
			}

			if (MW_Square && MW_SquareHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_square ? MW_Square->Resource : MW_SquareHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Square = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_SQUARE", IDB_MW_SQUARE, SelfModule);
				MW_SquareHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_SQUARE_HOVER", IDB_MW_SQUARE_HOVER, SelfModule);
			}

			if (MW_Star && MW_StarHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_star ? MW_Star->Resource : MW_StarHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Star = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_STAR", IDB_MW_STAR, SelfModule);
				MW_StarHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_STAR_HOVER", IDB_MW_STAR_HOVER, SelfModule);
			}

			if (MW_Swirl && MW_SwirlHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_swirl ? MW_Swirl->Resource : MW_SwirlHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Swirl = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_SWIRL", IDB_MW_SWIRL, SelfModule);
				MW_SwirlHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_SWIRL_HOVER", IDB_MW_SWIRL_HOVER, SelfModule);
			}

			if (MW_Triangle && MW_TriangleHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_triangle ? MW_Triangle->Resource : MW_TriangleHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Triangle = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_TRIANGLE", IDB_MW_TRIANGLE, SelfModule);
				MW_TriangleHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_TRIANGLE_HOVER", IDB_MW_TRIANGLE_HOVER, SelfModule);
			}

			if (MW_Cross && MW_CrossHover)
			{
				ImGui::SetCursorPos(intialPos);
				ImGui::Image(!h_cross ? MW_Cross->Resource : MW_CrossHover->Resource, ImVec2(112, 112));
			}
			else
			{
				MW_Cross = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_CROSS", IDB_MW_CROSS, SelfModule);
				MW_CrossHover = APIDefs->GetTextureOrCreateFromResource("TEX_MARKERSWHEEL_CROSS_HOVER", IDB_MW_CROSS_HOVER, SelfModule);
			}
		}
		ImGui::End();
	}*/

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

	if (!GameHandle) { GameHandle = hWnd; }

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
	if (!MumbleLink || !MumbleLink->Identity)

	APIDefs->Log(ELogLevel_TRACE, "Commander's Toolkit", "Distance:");
	APIDefs->Log(ELogLevel_TRACE, "Commander's Toolkit", std::to_string(VectorDistance(MumbleLink->Context.Compass.Center.X, MumbleLink->Context.Compass.Center.Y, x, y) * 24).c_str());
	APIDefs->Log(ELogLevel_TRACE, "Commander's Toolkit", "");

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

	APIDefs->Log(ELogLevel_TRACE, "Commander's Toolkit", std::to_string(lp).c_str());

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

	PostMessage(GameHandle, WM_SYSKEYDOWN, VK_MENU, GetLPARAM(VK_MENU, true, true));//0x20380001); // alt
	Sleep(5);
	PostMessage(GameHandle, WM_KEYDOWN, key, GetLPARAM(key, true, false));//0x20001);
	Sleep(15);
	PostMessage(GameHandle, WM_KEYUP, key, GetLPARAM(key, false, false));//0xC0020001);
	Sleep(5);
	PostMessage(GameHandle, WM_SYSKEYUP, VK_MENU, GetLPARAM(VK_MENU, false, true));//0xC0380001); // alt
	
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