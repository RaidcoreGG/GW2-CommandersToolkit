#pragma once
#include "imgui/imgui.h"
#include "imgui_extensions.h"
#include <string>
#include "Templates.h"
#include "SquadManager.h"
#include "Shared.h"
#include "resource.h"

/* UI */
bool SquadManager::Visible = false;

/* globals */
std::vector<Player> SquadManager::SquadMembers;
std::mutex SquadManager::SquadMembersMutex;

/* funcs */
uintptr_t SquadManager::DrawWindow(bool movable = true, bool clickable = true)
{
	std::lock_guard<std::mutex> lock(SquadMembersMutex);

	ImGui::Begin("Squad Manager", &Visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | (movable ? 0 : ImGuiWindowFlags_NoMove) | (clickable ? 0 : ImGuiWindowFlags_NoMouseInputs));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });

	if (ImGui::SmallButton("Remove all untracked"))
	{
		SquadMembers.erase(std::remove_if(SquadMembers.begin(), SquadMembers.end(),
			[](Player p) { return !p.IsTracked && !p.IsSelf; }), SquadMembers.end());
	}
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	ImGui::TooltipGeneric("Subgroup numbers update automatically on:\n- Combat entry\n- Instance/Map join");

	if (ImGui::BeginTable("table_sqmgr", 10, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_PadOuterX))
	{
		float sz = ImGui::GetFontSize();
		ImGui::TableNextRow();
		ImGui::TableSetColumnIndex(0); ImGui::Text("Name");
		ImGui::TableSetColumnIndex(1); ImGui::Text("Sub");
		ImGui::TableSetColumnIndex(2);
		if (Might && Might->Resource)
		{
			ImGui::Image(Might->Resource, ImVec2(sz, sz));
			ImGui::TooltipGeneric("Might");
		}
		else
		{
			Might = APIDefs->GetTextureOrCreateFromResource("TEX_BOON_MIGHT", IDB_MIGHT, SelfModule);
			ImGui::Text("Might");
		}
		ImGui::TableSetColumnIndex(3);
		if (Alacrity && Alacrity->Resource)
		{
			ImGui::Image(Alacrity->Resource, ImVec2(sz, sz));
			ImGui::TooltipGeneric("Alacrity");
		}
		else
		{
			Alacrity = APIDefs->GetTextureOrCreateFromResource("TEX_BOON_ALACRITY", IDB_ALACRITY, SelfModule);
			ImGui::Text("Alac");
		}
		ImGui::TableSetColumnIndex(4);
		if (Quickness && Quickness->Resource)
		{
			ImGui::Image(Quickness->Resource, ImVec2(sz, sz));
			ImGui::TooltipGeneric("Quickness");
		}
		else
		{
			Quickness = APIDefs->GetTextureOrCreateFromResource("TEX_BOON_QUICKNESS", IDB_QUICKNESS, SelfModule);
			ImGui::Text("Quic");
		}
		ImGui::TableSetColumnIndex(5);
		if (Fury && Fury->Resource)
		{
			ImGui::Image(Fury->Resource, ImVec2(sz, sz));
			ImGui::TooltipGeneric("Fury");
		}
		else
		{
			Fury =  APIDefs->GetTextureOrCreateFromResource("TEX_BOON_FURY", IDB_FURY, SelfModule);
			ImGui::Text("Fury");
		}
		ImGui::TableSetColumnIndex(6);
		if (Vulnerability && Vulnerability->Resource)
		{
			ImGui::Image(Vulnerability->Resource, ImVec2(sz, sz));
			ImGui::TooltipGeneric("Vulnerability");
		}
		else
		{
			Vulnerability =APIDefs->GetTextureOrCreateFromResource("TEX_BOON_VULNERABILITY", IDB_VULNERABILITY, SelfModule);
			ImGui::Text("Vuln");
		}
		ImGui::TableSetColumnIndex(7);
		if (Heal && Heal->Resource)
		{
			ImGui::Image(Heal->Resource, ImVec2(sz, sz));
			ImGui::TooltipGeneric("Heal");
		}
		else
		{
			Heal = APIDefs->GetTextureOrCreateFromResource("TEX_BOON_HEAL", IDB_HEAL, SelfModule);
			ImGui::Text("Heal");
		}
		ImGui::TableSetColumnIndex(8); ImGui::Text("Notes");
		ImGui::TableNextRow();

		for (size_t sub = 1; sub <= 15; sub++)
		{
			int subPlayerCount = 0;
			bool subTotalUntracked;
			Utility subTotal;

			for (size_t i = 0; i < SquadManager::SquadMembers.size(); i++)
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
				if (!SquadMembers[i].IsTracked && !SquadMembers[i].IsSelf) { ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(172, 89, 89, 255)); }

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text(SquadMembers[i].CharacterName);
				
				SquadMembers[i].Tooltip();

				if (ImGui::BeginPopupContextItem(("##PlayerCtx" + std::to_string(SquadMembers[i].ID)).c_str()))
				{
					ImGui::Text("Apply from template:");
					ImGui::Separator();
					std::vector<Template> templates = GetTemplates(SquadMembers[i].Profession);
					for (size_t t = 0; t < templates.size(); t++)
					{
						ImGui::Bullet(); ImGui::SameLine();
						if (ImGui::MenuItem(templates[t].BuildName))
						{
							SquadMembers[i].Utilities = templates[t].Utilities;
						}
					}
					if (ImGui::BeginMenu("Other professions"))
					{
						for (int p = 1; p <= 9; p++)
						{
							if (SquadMembers[i].Profession == p) { continue; }
							std::string profession;
							switch (p)
							{
							case 1: profession = "Guardian"; break;
							case 2: profession = "Warrior"; break;
							case 3: profession = "Engineer"; break;
							case 4: profession = "Ranger"; break;
							case 5: profession = "Thief"; break;
							case 6: profession = "Elementalist"; break;
							case 7: profession = "Mesmer"; break;
							case 8: profession = "Necromancer"; break;
							case 9: profession = "Revenant"; break;
							}
							if (ImGui::BeginMenu(profession.c_str()))
							{
								std::vector<Template> otherTemplates = GetTemplates(p);
								for (size_t t = 0; t < otherTemplates.size(); t++)
								{
									if (ImGui::MenuItem(otherTemplates[t].BuildName))
									{
										SquadMembers[i].Utilities = otherTemplates[t].Utilities;
									}
								}
								ImGui::EndMenu();
							}
						}
						ImGui::EndMenu();
					}

					ImGui::Separator();

					if (ImGui::MenuItem("Reset")) { SquadMembers[i].Utilities = Utility(); }

					ImGui::EndPopup();
				}
				ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(64);
				ImGui::InputInt(("##Sub" + id).c_str(), &SquadMembers[i].Subgroup);
				if (SquadMembers[i].Subgroup < 1) { SquadMembers[i].Subgroup = 1; } // sub min is 1
				if (SquadMembers[i].Subgroup > 15) { SquadMembers[i].Subgroup = 15; } // sub max is 15
				ImGui::TableSetColumnIndex(2); ImGui::CheckboxCenteredColumn(("##Might" + id).c_str(), &SquadMembers[i].Utilities.Might);
				ImGui::TableSetColumnIndex(3); ImGui::CheckboxCenteredColumn(("##Alac" + id).c_str(), &SquadMembers[i].Utilities.Alacrity);
				ImGui::TableSetColumnIndex(4); ImGui::CheckboxCenteredColumn(("##Quic" + id).c_str(), &SquadMembers[i].Utilities.Quickness);
				ImGui::TableSetColumnIndex(5); ImGui::CheckboxCenteredColumn(("##Fury" + id).c_str(), &SquadMembers[i].Utilities.Fury);
				ImGui::TableSetColumnIndex(6); ImGui::CheckboxCenteredColumn(("##Vuln" + id).c_str(), &SquadMembers[i].Utilities.Vulnerability);
				ImGui::TableSetColumnIndex(7); ImGui::CheckboxCenteredColumn(("##Heal" + id).c_str(), &SquadMembers[i].Utilities.Heal);
				ImGui::TableSetColumnIndex(8); ImGui::SetNextItemWidth(128); ImGui::InputText(("##Notes" + id).c_str(), SquadMembers[i].Notes, sizeof(SquadMembers[i].Notes));

				if (!SquadMembers[i].IsTracked && !SquadMembers[i].IsSelf)
				{
					// remove button if not tracked
					ImGui::TableSetColumnIndex(9); if (ImGui::SmallButton(("Remove##" + id).c_str())) { SquadMembers.erase(SquadMembers.begin() + i); }
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
			if (subTotal.Might) { ImGui::TableSetColumnIndex(2); ImGui::TextCenteredColumn("X"); }
			if (subTotal.Alacrity) { ImGui::TableSetColumnIndex(3); ImGui::TextCenteredColumn("X"); }
			if (subTotal.Quickness) { ImGui::TableSetColumnIndex(4); ImGui::TextCenteredColumn("X"); }
			if (subTotal.Fury) { ImGui::TableSetColumnIndex(5); ImGui::TextCenteredColumn("X"); }
			if (subTotal.Vulnerability) { ImGui::TableSetColumnIndex(6); ImGui::TextCenteredColumn("X"); }
			if (subTotal.Heal) { ImGui::TableSetColumnIndex(7); ImGui::TextCenteredColumn("X"); }

			if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

			if (subPlayerCount > 5) { ImGui::TableSetColumnIndex(8); ImGui::TextColored(ImVec4(1.00f, 0.58f, 0.31f, 1.00f), "Warning: more than 5 players!"); }
		}

		ImGui::EndTable();
	}

	ImGui::PopStyleVar();

	ImGui::End();

	return 0;
}

uintptr_t SquadManager::PurgeSquadMembers()
{
	SquadMembers.erase(std::remove_if(SquadMembers.begin(), SquadMembers.end(),
		[](Player p) { return (time(0) - p.LastSeen) >= 60 * 30; }), SquadMembers.end());

	return 0;
}