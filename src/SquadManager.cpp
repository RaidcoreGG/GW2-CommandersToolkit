#pragma once
#include "imgui/imgui.h"
#include "imgui_extensions.h"
#include <string>
#include "Templates.h"
#include "SquadManager.h"

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

	float notesWidth = ImGui::CalcTextSize(u8"警告:超過5名玩家!").x;

	ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 20), ImVec2(-1, 320));

	ImGui::Begin(u8"團隊管理", &Visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | (movable ? 0 : ImGuiWindowFlags_NoMove) | (clickable ? 0 : ImGuiWindowFlags_NoMouseInputs));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });

	if (ImGui::SmallButton(u8"移除所有無法追蹤的玩家"))
	{
		SquadMembers.erase(std::remove_if(SquadMembers.begin(), SquadMembers.end(),
			[](Player p) { return !p.IsTracked && !p.IsSelf; }), SquadMembers.end());
	}
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	ImGui::TooltipGeneric(u8"小隊編號在下列情況自動更新:\n- 進入戰鬥狀態時\n- 進入地圖時");

	if (ImGui::BeginTable("table_sqmgr", 10, ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_PadOuterX))
	{
		ImGui::TableSetupColumn(u8"玩家名");
		ImGui::TableSetupColumn(u8"小隊");
		ImGui::TableSetupColumn("Might");
		ImGui::TableSetupColumn("Alac");
		ImGui::TableSetupColumn("Quick");
		ImGui::TableSetupColumn("Fury");
		ImGui::TableSetupColumn("Vuln");
		ImGui::TableSetupColumn("Heal");
		ImGui::TableSetupColumn(u8"備註");
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
				ImGui::TableSetColumnIndex(8); ImGui::SetNextItemWidth(notesWidth); ImGui::InputText(("##Notes" + id).c_str(), SquadMembers[i].Notes, sizeof(SquadMembers[i].Notes));

				// player action field
				if (untracked) // show remove button if not tracked
				{
					ImGui::TableSetColumnIndex(9);
					if (ImGui::SmallButton((u8"移除##" + id).c_str())) { SquadMembers.erase(SquadMembers.begin() + i); }
					ImGui::PopStyleColor(); // reset red font
				}
			}

			if (current.AmountPlayers) // if has players render subgroup
			{
				bool fullCoverage = current.Covered();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text(u8"小隊"); if (ShortWarnings) { current.WarningShort(); }
				ImGui::TableSetColumnIndex(1); ImGui::Text("%u", sub);
				ImGui::TableSetColumnIndex(2); ImGui::TextCenteredColumn("%s", current.Utilities.Might ? "X" : "");
				ImGui::TableSetColumnIndex(3); ImGui::TextCenteredColumn("%s", current.Utilities.Alacrity ? "X" : "");
				ImGui::TableSetColumnIndex(4); ImGui::TextCenteredColumn("%s", current.Utilities.Quickness ? "X" : "");
				ImGui::TableSetColumnIndex(5); ImGui::TextCenteredColumn("%s", current.Utilities.Fury ? "X" : "");
				ImGui::TableSetColumnIndex(6); ImGui::TextCenteredColumn("%s", current.Utilities.Vulnerability ? "X" : "");
				ImGui::TableSetColumnIndex(7); ImGui::TextCenteredColumn("%s", current.Utilities.Heal ? "X" : "");
				ImGui::TableSetColumnIndex(8); ImGui::SetNextItemWidth(notesWidth); ImGui::InputText(("##Notes" + std::to_string(sub)).c_str(), current.Notes, sizeof(current.Notes));

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