#pragma once
#include "imgui/imgui.h"
#include "imgui_extensions.h"
#include <string>
#include "Templates.h"
#include "SquadManager.h"
#include <algorithm>

/* UI */
bool SquadManager::Visible = false;
bool ShortWarnings = false;

/* globals */
std::vector<Player> SquadManager::SquadMembers;
Subgroup SquadManager::Subgroups[15];
std::mutex SquadManager::SquadMembersMutex;



/* funcs */
uintptr_t SquadManager::DrawWindow(bool movable = true, bool clickable = true)
{
	std::lock_guard<std::mutex> lock(SquadMembersMutex);

	float notesWidth = ImGui::CalcTextSize("Warning: more than 5 players!").x;

	ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 20), ImVec2(-1, 340));

	ImGui::Begin("Squad Manager", &Visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | (movable ? 0 : ImGuiWindowFlags_NoMove) | (clickable ? 0 : ImGuiWindowFlags_NoMouseInputs));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });

	if (ImGui::SmallButton("Remove all untracked"))
	{
		SquadMembers.erase(std::remove_if(SquadMembers.begin(), SquadMembers.end(),
			[](Player p) { return !p.IsTracked && !p.IsSelf; }), SquadMembers.end());
	}
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())ImGui::SetTooltip("Subgroup numbers update automatically on:\n- Combat entry\n- Instance/Map join");

	if (ImGui::BeginTable("table_sqmgr", 14, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_PadOuterX))
	{
		ImGui::TableSetupColumn("Name");
		ImGui::TableSetupColumn("Sub");
		ImGui::TableSetupColumn("Might");
		ImGui::TableSetupColumn("Alac");
		ImGui::TableSetupColumn("Quic");
		ImGui::TableSetupColumn("Fury");
		ImGui::TableSetupColumn("Vuln");
		ImGui::TableSetupColumn("Heal");
		ImGui::TableSetupColumn("Pow");
		ImGui::TableSetupColumn("Condi");
		ImGui::TableSetupColumn("Notes");
		ImGui::TableHeadersRow();

		for (size_t sub = 1; sub <= 15; sub++)
		{
			Subgroup current = Subgroups[sub - 1];
			current.AmountPlayers = 0;
			current.Utilities = Utility();

			for (size_t i = 0; i < SquadManager::SquadMembers.size(); i++)
			{
				if (SquadMembers[i].Subgroup != sub) { continue; }
				current.AmountPlayers++;

				// set subgroup coverage
				if (SquadMembers[i].Utilities.Might)			{ current.Utilities.Might = true; } 
				if (SquadMembers[i].Utilities.Alacrity)			{ current.Utilities.Alacrity = true; }
				if (SquadMembers[i].Utilities.Quickness)		{ current.Utilities.Quickness = true; }
				if (SquadMembers[i].Utilities.Fury)				{ current.Utilities.Fury = true; }
				if (SquadMembers[i].Utilities.Vulnerability)	{ current.Utilities.Vulnerability = true; }
				if (SquadMembers[i].Utilities.Heal)				{ current.Utilities.Heal = true; }
				if (SquadMembers[i].Utilities.Pow)				{ current.Utilities.Pow = true; }
				if (SquadMembers[i].Utilities.Condi)			{ current.Utilities.Condi = true; }

				std::string id = std::to_string(SquadMembers[i].ID); // helper for unique chkbxIds

				bool untracked = SquadMembers[i].Untracked(); // red font if not tracked

				// player name field
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(SquadMembers[i].CharacterName);
				SquadMembers[i].Tooltip();
				SquadMembers[i].ContextMenu();
				
				// player sub field
				ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(64);
				ImGui::InputInt(("##Sub" + id).c_str(), &SquadMembers[i].Subgroup);
				if (SquadMembers[i].Subgroup < 1) { SquadMembers[i].Subgroup = 1; } // sub min is 1
				if (SquadMembers[i].Subgroup > 15) { SquadMembers[i].Subgroup = 15; } // sub max is 15

				// player utility fields
				ImGui::TableSetColumnIndex(2); ImGui::CheckboxCenteredColumn(("##Might" + id).c_str(), &SquadMembers[i].Utilities.Might);
				ImGui::TableSetColumnIndex(3); ImGui::CheckboxCenteredColumn(("##Alac" + id).c_str(), &SquadMembers[i].Utilities.Alacrity);
				ImGui::TableSetColumnIndex(4); ImGui::CheckboxCenteredColumn(("##Quic" + id).c_str(), &SquadMembers[i].Utilities.Quickness);
				ImGui::TableSetColumnIndex(5); ImGui::CheckboxCenteredColumn(("##Fury" + id).c_str(), &SquadMembers[i].Utilities.Fury);
				ImGui::TableSetColumnIndex(6); ImGui::CheckboxCenteredColumn(("##Vuln" + id).c_str(), &SquadMembers[i].Utilities.Vulnerability);
				ImGui::TableSetColumnIndex(7); ImGui::CheckboxCenteredColumn(("##Heal" + id).c_str(), &SquadMembers[i].Utilities.Heal);
				ImGui::TableSetColumnIndex(8); ImGui::CheckboxCenteredColumn(("##Pow" + id).c_str(), &SquadMembers[i].Utilities.Pow);
				ImGui::TableSetColumnIndex(9); ImGui::CheckboxCenteredColumn(("##Condi" + id).c_str(), &SquadMembers[i].Utilities.Condi);
				ImGui::TableSetColumnIndex(10); ImGui::SetNextItemWidth(notesWidth); ImGui::InputText(("##Notes" + id).c_str(), SquadMembers[i].Notes, sizeof(SquadMembers[i].Notes));

				// player action field
				if (untracked) // show remove button if not tracked
				{
					ImGui::TableSetColumnIndex(9);
					if (ImGui::SmallButton(("Remove##" + id).c_str())) { SquadMembers.erase(SquadMembers.begin() + i); }
					ImGui::PopStyleColor(); // reset red font
				}
			}

			if (current.AmountPlayers) // if has players render subgroup
			{
				bool fullCoverage = current.Covered();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Subgroup"); if (ShortWarnings) { current.WarningShort(); }
				ImGui::TableSetColumnIndex(1); ImGui::Text("%u", sub);
				ImGui::TableSetColumnIndex(2); ImGui::TextCenteredColumn("%s", current.Utilities.Might ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Might");
				ImGui::TableSetColumnIndex(3); ImGui::TextCenteredColumn("%s", current.Utilities.Alacrity ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Alacrity");
				ImGui::TableSetColumnIndex(4); ImGui::TextCenteredColumn("%s", current.Utilities.Quickness ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Quickness");
				ImGui::TableSetColumnIndex(5); ImGui::TextCenteredColumn("%s", current.Utilities.Fury ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Fury");
				ImGui::TableSetColumnIndex(6); ImGui::TextCenteredColumn("%s", current.Utilities.Vulnerability ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Vulnerability");
				ImGui::TableSetColumnIndex(7); ImGui::TextCenteredColumn("%s", current.Utilities.Heal ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Heal");
				ImGui::TableSetColumnIndex(8); ImGui::TextCenteredColumn("%s", current.Utilities.Pow ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Power Damage");
				ImGui::TableSetColumnIndex(9); ImGui::TextCenteredColumn("%s", current.Utilities.Condi ? "X" : ""); if (ImGui::IsItemHovered())ImGui::SetTooltip("Condition Damage");
				ImGui::TableSetColumnIndex(10); ImGui::SetNextItemWidth(notesWidth); ImGui::InputText(("##Notes" + std::to_string(sub)).c_str(), current.Notes, sizeof(current.Notes));

				if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

				if (!ShortWarnings) { current.Warning(); }
			}
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