// dllmain.cpp : Defines the entry point for the DLL application.
#pragma comment(lib, "winmm.lib")
#include <stdint.h>
#include <stdio.h>
#include <Windows.h>
#include <string>
#include "arcdps_structs.h"
#include "imgui\imgui.h"
#include "nlohmann/json.hpp"
#include "Player.h"
#include <d3d11.h>

using json = nlohmann::json;

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

/* proto */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext* imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion);
extern "C" __declspec(dllexport) void* get_release_addr();

arcdps_exports* mod_init();
uintptr_t mod_release();
uintptr_t mod_imgui(uint32_t not_charsel_or_loading);
uintptr_t mod_options_windows(const char* windowname); // Windows check boxes in main menu
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision);

void log(char* str);
void log_file(char* str);
void log_arc(char* str);

/* globals */
arcdps_exports arc_exports;
void* filelog;
void* arclog;

bool show_squadmanager = false;
std::vector<Player> SquadMembers;
bool working = false;

void log(char* str) /* log to arcdps.log and log window*/
{
	log_file(str);
	log_arc(str);
	return;
}
void log_file(char* str) /* log to arcdps.log, thread/async safe */
{
	size_t(*log)(char*) = (size_t(*)(char*))filelog;
	if (log) (*log)(str);
	return;
}
void log_arc(char* str) /* log to extensions tab in arcdps log window, thread/async safe */
{
	size_t(*log)(char*) = (size_t(*)(char*))arclog;
	if (log) (*log)(str);
	return;
}

/* export -- arcdps looks for this exported function and calls the address it returns on client load */
extern "C" __declspec(dllexport) void* get_init_addr(char* arcversion, ImGuiContext* imguictx, void* id3dptr, HANDLE arcdll, void* mallocfn, void* freefn, uint32_t d3dversion)
{
	ImGui::SetCurrentContext((ImGuiContext*)imguictx);
	ImGui::SetAllocatorFunctions((void *(*)(size_t, void*))mallocfn, (void(*)(void*, void*))freefn); // on imgui 1.80+

	filelog = (void*)GetProcAddress((HMODULE)arcdll, "e3");
	arclog = (void*)GetProcAddress((HMODULE)arcdll, "e8");

	return mod_init;
}
/* export -- arcdps looks for this exported function and calls the address it returns on client exit */
extern "C" __declspec(dllexport) void* get_release_addr()
{
	return mod_release;
}

/* initialize mod -- return table that arcdps will use for callbacks. exports struct and strings are copied to arcdps memory only once at init */
arcdps_exports* mod_init()
{
	/* for arcdps */
	memset(&arc_exports, 0, sizeof(arc_exports));
	arc_exports.sig = 0x4A584326;
	arc_exports.imguivers = IMGUI_VERSION_NUM;
	arc_exports.size = sizeof(arc_exports);
	arc_exports.out_name = "Commander's Toolkit";
	arc_exports.out_build = __DATE__ " " __TIME__;
	arc_exports.imgui = mod_imgui;
	arc_exports.options_windows = mod_options_windows;
	arc_exports.combat = mod_combat;

	return &arc_exports;
}
/* release mod -- return ignored */
uintptr_t mod_release()
{
	FreeConsole();
	return 0;
}

std::vector<Template> get_templates(int profession)
{
	/* ideally you don't want to hardcode builds in an ever changing game, but I cba right now */
	std::vector<Template> templates;

	//templates.push_back(Template("NAME", Utility(m, a, q, f, v, h)));

	switch (profession)
	{
		case 1: // Guardian
			templates.push_back(Template("Condition Quickness Firebrand", Utility(true, false, true, true, false, false)));
			templates.push_back(Template("Heal Quickness Firebrand", Utility(true, false, true, true, false, true)));
			break;

		case 2: // Warrior
			templates.push_back(Template("Condition Quickness Berserker", Utility(false, false, true, true, false, false)));
			templates.push_back(Template("Power Quickness Bladesworn", Utility(true, false, true, true, false, false)));
			break;

		case 3: // Engineer
			templates.push_back(Template("Power Scrapper", Utility(false, false, false, false, true, false)));
			templates.push_back(Template("Power Quickness Scrapper", Utility(false, false, true, false, true, false)));
			templates.push_back(Template("Heal Quickness Scrapper", Utility(true, false, true, true, false, true)));
			templates.push_back(Template("Power Holosmith", Utility(false, false, false, false, true, false)));
			templates.push_back(Template("Hand Kite Mechanist", Utility(true, true, false, false, false, false)));
			templates.push_back(Template("Power Alacrity Mechanist", Utility(true, true, false, true, false, false)));
			templates.push_back(Template("Heal Alacrity Mechanist", Utility(true, true, false, true, false, true)));
			break;

		case 4: // Ranger
			templates.push_back(Template("Heal Alacrity Druid", Utility(true, true, false, true, false, true)));
			templates.push_back(Template("Condition Alacrity Untamed", Utility(false, true, false, false, false, false)));
			break;

		case 5: // Thief
			templates.push_back(Template("Condition Boon Daredevil", Utility(true, false, true, true, false, false)));
			templates.push_back(Template("Condition Alacrity Specter", Utility(false, true, false, false, false, false)));
			break;

		case 6: // Elementalist
			templates.push_back(Template("Heal Alacrity Tempest", Utility(true, true, false, true, false, true)));
			templates.push_back(Template("Power Catalyst", Utility(true, false, false, false, false, false)));
			break;

		case 7: // Mesmer
			templates.push_back(Template("Power Quickness Chronomancer", Utility(false, false, true, false, false, false)));
			templates.push_back(Template("Condition Quickness Chronomancer", Utility(false, false, true, false, false, false)));
			templates.push_back(Template("Condition Alacrity Mirage", Utility(true, true, false, false, false, false)));
			templates.push_back(Template("Power Virtuoso", Utility(false, false, false, true, true, false)));
			break;

		case 8: // Necromancer
			templates.push_back(Template("Condition Harbinger", Utility(false, false, false, false, true, false)));
			templates.push_back(Template("Condition Quickness Harbinger", Utility(true, false, true, true, true, false)));
			templates.push_back(Template("Power Quickness Harbinger", Utility(false, false, true, true, false, false)));
			break;

		case 9: // Revenant
			templates.push_back(Template("Power Herald", Utility(true, false, false, true, false, false)));
			templates.push_back(Template("Power Quickness Herald", Utility(true, false, true, true, false, false)));
			templates.push_back(Template("Condition Renegade", Utility(false, true, false, false, true, false)));
			break;
	}

	return templates;
}

uintptr_t UISquadManager()
{
	ImGui::Begin("Squad Manager", &show_squadmanager, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });

	if (ImGui::SmallButton("Remove all untracked"))
	{
		SquadMembers.erase(std::remove_if(SquadMembers.begin(), SquadMembers.end(),
			[](Player p) { return !p.IsTracked; }), SquadMembers.end());
	}
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Subgroup numbers update automatically on:\n- Combat entry\n- Instance/Map join");
		ImGui::EndTooltip();
	}

	if (ImGui::BeginTable("table_sqmgr", 10, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_PadOuterX))
	{
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Sub");
		ImGui::TableSetupColumn("Might");
		ImGui::TableSetupColumn("Alac");
		ImGui::TableSetupColumn("Quic");
		ImGui::TableSetupColumn("Fury");
		ImGui::TableSetupColumn("Vuln");
		ImGui::TableSetupColumn("Heal");
		ImGui::TableSetupColumn("Notes");
		ImGui::TableHeadersRow();

		for (size_t sub = 1; sub <= 15; sub++)
		{
			int subPlayerCount = 0;
			Utility subTotal;

			for (size_t i = 0; i < SquadMembers.size(); i++)
			{
				if (SquadMembers[i].Subgroup != sub) { continue; }
				subPlayerCount++;

				// set subgroup coverage
				if (SquadMembers[i].Utilities.Might) { subTotal.Might = true; }
				if (SquadMembers[i].Utilities.Alacrity) { subTotal.Alacrity = true; }
				if (SquadMembers[i].Utilities.Quickness) { subTotal.Quickness = true; }
				if (SquadMembers[i].Utilities.Fury) { subTotal.Fury = true; }
				if (SquadMembers[i].Utilities.Vulnerability) { subTotal.Vulnerability = true; }
				if (SquadMembers[i].Utilities.Heal) { subTotal.Heal = true; }

				std::string id = std::to_string(SquadMembers[i].ID); // helper for unique chkbxIds

				// red font if not tracked
				if (!SquadMembers[i].IsTracked) { ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(172, 89, 89, 255)); }

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("%s", SquadMembers[i].IsTracked ? SquadMembers[i].CharacterName : SquadMembers[i].AccountName);
				// acc name tooltip on charname hover
				if (ImGui::IsItemHovered() && SquadMembers[i].IsTracked)
				{
					ImGui::BeginTooltip();
					ImGui::Text("%s", SquadMembers[i].AccountName);
					ImGui::EndTooltip();
				}
				if (ImGui::BeginPopupContextItem(("##PlayerCtx" + std::to_string(SquadMembers[i].ID)).c_str()))
				{
					if (ImGui::BeginMenu("Apply from template"))
					{
						//ImGui::Text(std::to_string(SquadMembers[i].Profession).c_str());

						std::vector<Template> templates = get_templates(SquadMembers[i].Profession);
						for (size_t t = 0; t < templates.size(); t++)
						{
							if (ImGui::MenuItem(templates[t].BuildName))
							{
								SquadMembers[i].Utilities = templates[t].Utilities;
							}
						}
						ImGui::Separator();
						if (ImGui::MenuItem("Reset"))
						{
							SquadMembers[i].Utilities = Utility();
						}

						ImGui::EndMenu();
					}
					ImGui::EndPopup();
				}
				ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(64);
				ImGui::InputInt(("##Sub" + id).c_str(), &SquadMembers[i].Subgroup);
				if (SquadMembers[i].Subgroup < 1) { SquadMembers[i].Subgroup = 1; } // sub min is 1
				if (SquadMembers[i].Subgroup > 15) { SquadMembers[i].Subgroup = 15; } // sub max is 15
				ImGui::TableSetColumnIndex(2); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFontSize()) / 2);
				ImGui::Checkbox(("##Might" + id).c_str(), &SquadMembers[i].Utilities.Might);
				ImGui::TableSetColumnIndex(3); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFontSize()) / 2);
				ImGui::Checkbox(("##Alac" + id).c_str(), &SquadMembers[i].Utilities.Alacrity);
				ImGui::TableSetColumnIndex(4); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFontSize()) / 2);
				ImGui::Checkbox(("##Quic" + id).c_str(), &SquadMembers[i].Utilities.Quickness);
				ImGui::TableSetColumnIndex(5); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFontSize()) / 2);
				ImGui::Checkbox(("##Fury" + id).c_str(), &SquadMembers[i].Utilities.Fury);
				ImGui::TableSetColumnIndex(6); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFontSize()) / 2);
				ImGui::Checkbox(("##Vuln" + id).c_str(), &SquadMembers[i].Utilities.Vulnerability);
				ImGui::TableSetColumnIndex(7); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::GetFontSize()) / 2);
				ImGui::Checkbox(("##Heal" + id).c_str(), &SquadMembers[i].Utilities.Heal);
				ImGui::TableSetColumnIndex(8); ImGui::SetNextItemWidth(128);
				ImGui::InputText(("##Notes" + id).c_str(), SquadMembers[i].Notes, sizeof(SquadMembers[i].Notes));
				
				if (!SquadMembers[i].IsTracked)
				{
					// remove button if not tracked
					ImGui::TableSetColumnIndex(9); if (ImGui::SmallButton("Remove")) { SquadMembers.erase(SquadMembers.begin() + i); }
					ImGui::PopStyleColor(); // reset red font
				}
			}
			
			if (!subPlayerCount) { continue; }

			// subgroup totals
			bool fullCoverage = false;
			if (subTotal.Might && subTotal.Alacrity && subTotal.Quickness && subTotal.Fury && subTotal.Vulnerability && subTotal.Heal)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(89, 172, 98, 255)); // if all utilities are covered -> green text
				fullCoverage = true;
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0); ImGui::Text("Subgroup");
			ImGui::TableSetColumnIndex(1); ImGui::Text("%u", sub);
			if (subTotal.Might)			{ ImGui::TableSetColumnIndex(2); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Alacrity)		{ ImGui::TableSetColumnIndex(3); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Quickness)		{ ImGui::TableSetColumnIndex(4); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Fury)			{ ImGui::TableSetColumnIndex(5); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Vulnerability) { ImGui::TableSetColumnIndex(6); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Heal)			{ ImGui::TableSetColumnIndex(7); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			
			if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

			if (subPlayerCount > 5) { ImGui::TableSetColumnIndex(8); ImGui::Text("Warning: more than 5 players!"); }
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar();

	ImGui::End();

	return 0;
}

uintptr_t mod_imgui(uint32_t not_charsel_or_loading)
{
	if (show_squadmanager) { UISquadManager(); }

	return 0;
}

uintptr_t mod_options_windows(const char* windowname)
{
	if (windowname)
	{
		if (strcmp(windowname, "squad") == 0)
		{
			ImGui::Checkbox("Squad Manager", &show_squadmanager);
		}
	}

	return 0;
}

/* combat callback -- may be called asynchronously, use id param to keep track of order, first event id will be 2. return ignored */
/* at least one participant will be party/squad or minion of, or a buff applied by squad in the case of buff remove. not all statechanges present, see evtc statechange enum */
uintptr_t mod_combat(cbtevent* ev, ag* src, ag* dst, char* skillname, uint64_t id, uint64_t revision)
{
	/* ev is null. dst will only be valid on tracking add. skillname will also be null */
	if (!ev)
	{
		/* notify tracking change */
		if (!src->elite)
		{
			while (working) {
				Sleep(5);
			}
			working = true;

			/* add */
			if (src->prof)
			{
				//p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				//p += _snprintf_s(p, 400, _TRUNCATE, "agent added: %s:%s (%0llx), instid: %u, prof: %u, elite: %u, self: %u, team: %u, subgroup: %u\n", src->name, dst->name, src->id, dst->id, dst->prof, dst->elite, dst->self, src->team, dst->team);

				// some if not null cancer (useless?)
				if (src->name != nullptr && src->name[0] != '\0' && dst->name != nullptr && dst->name[0] != '\0')
				{
					bool agUpdate = false;

					for (size_t i = 0; i < SquadMembers.size(); i++)
					{
						if (SquadMembers[i].ID != src->id) { continue; }
						else
						{
							agUpdate = true;
							strcpy_s(SquadMembers[i].CharacterName, src->name);
							SquadMembers[i].Profession = dst->prof;
							SquadMembers[i].Subgroup = dst->team;
							SquadMembers[i].IsTracked = true;
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

						SquadMembers.push_back(p);
					}
				}
			}
			else /* remove */
			{
				//p += _snprintf_s(p, 400, _TRUNCATE, "==== cbtnotify ====\n");
				//p += _snprintf_s(p, 400, _TRUNCATE, "agent removed: %s (%0llx)\n", src->name, src->id);

				for (size_t i = 0; i < SquadMembers.size(); i++)
				{
					if (SquadMembers[i].ID != src->id) { continue; }
					else
					{
						SquadMembers[i].IsTracked = false;
						break;
					}
				}
			}

			working = false;
		}
	}
	else // combat enter
	{
		if (ev->is_statechange == CBTS_ENTERCOMBAT)
		{
			for (size_t i = 0; i < SquadMembers.size(); i++)
			{
				if (SquadMembers[i].ID != src->id) { continue; }
				else
				{
					SquadMembers[i].Subgroup = ev->dst_agent;
					break;
				}
			}
		}
	}

	// add cbtstart subgroup update

	return 0;
}