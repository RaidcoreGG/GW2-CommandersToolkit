#pragma once
#include "imgui/imgui.h"
#include <string>
#include "Templates.h"
#include "SquadManager.h"

/* UI */
bool SquadManager::Focused = false;
bool SquadManager::Visible = false;

/* globals */
std::vector<Player> SquadManager::SquadMembers;
std::mutex SquadManager::SquadMembersMutex;

/* funcs */
uintptr_t SquadManager::DrawWindow()
{
	std::lock_guard<std::mutex> lock(SquadMembersMutex);

	ImGui::Begin("Squad Manager", &Visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);

	Focused = ImGui::IsWindowFocused();

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
				if (!SquadMembers[i].IsTracked) { ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(172, 89, 89, 255)); }

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text(SquadMembers[i].CharacterName);
				// acc name tooltip on charname hover
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("%s", SquadMembers[i].AccountName);
					if (!SquadMembers[i].IsTracked)
					{
						int secondsSince = time(0) - SquadMembers[i].LastSeen;

						if (secondsSince < 60)
						{
							ImGui::Text("Last seen %u seconds ago", secondsSince);
						}
						else
						{
							ImGui::Text("Last seen %u minutes ago", secondsSince / 60);
						}
					}
					ImGui::EndTooltip();
				}
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
					ImGui::Separator();
					if (ImGui::MenuItem("Reset"))
					{
						SquadMembers[i].Utilities = Utility();
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
			if (subTotal.Might) { ImGui::TableSetColumnIndex(2); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Alacrity) { ImGui::TableSetColumnIndex(3); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Quickness) { ImGui::TableSetColumnIndex(4); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Fury) { ImGui::TableSetColumnIndex(5); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Vulnerability) { ImGui::TableSetColumnIndex(6); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }
			if (subTotal.Heal) { ImGui::TableSetColumnIndex(7); ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (ImGui::GetColumnWidth() - ImGui::CalcTextSize("X").x) / 2); ImGui::Text("X"); }

			if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

			if (subPlayerCount > 5) { ImGui::TableSetColumnIndex(8); ImGui::Text("Warning: more than 5 players!"); }
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