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
std::map<std::string, Player> SquadManager::SquadMembers;
Subgroup SquadManager::Subgroups[15];
std::mutex SquadManager::SquadMembersMutex;
std::string SquadManager::SelfAccountName;

/* funcs */
uintptr_t SquadManager::DrawWindow(bool movable = true, bool clickable = true)
{
	std::lock_guard<std::mutex> lock(SquadMembersMutex);

	float notesWidth = ImGui::CalcTextSize("Warning: more than 5 players!").x;

	ImGui::SetNextWindowSizeConstraints(ImVec2(-1, 20), ImVec2(-1, 320));

	ImGui::Begin("Squad Manager", &Visible, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | (movable ? 0 : ImGuiWindowFlags_NoMove) | (clickable ? 0 : ImGuiWindowFlags_NoMouseInputs));

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });

	if (ImGui::SmallButton("Remove all untracked"))
	{
		for (auto it = SquadMembers.begin(); it != SquadMembers.end(); ++it)
		{
			if (!it->second.IsTrackedByArcdps && !it->second.IsTrackedByExtras && !it->second.IsSelf)
			{
				SquadMembers.erase(it);
			}
		}
	}
	ImGui::SameLine(); ImGui::TextDisabled("(?)");
	ImGui::TooltipGeneric("Subgroup numbers update automatically on:\n- Combat entry\n- Instance/Map join");

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
			Subgroup current = Subgroups[sub - 1];
			current.AmountPlayers = 0;
			current.Utilities = Utility();

			for (auto it = SquadMembers.begin(); it != SquadMembers.end();)
			{
				auto& squadMember = it->second;
				if (squadMember.Subgroup != sub)
				{
					++it;
					continue;
				}
				current.AmountPlayers++;

				// set subgroup coverage
				if (squadMember.Utilities.Might)			{ current.Utilities.Might = true; }
				if (squadMember.Utilities.Alacrity)			{ current.Utilities.Alacrity = true; }
				if (squadMember.Utilities.Quickness)		{ current.Utilities.Quickness = true; }
				if (squadMember.Utilities.Fury)				{ current.Utilities.Fury = true; }
				if (squadMember.Utilities.Vulnerability)	{ current.Utilities.Vulnerability = true; }
				if (squadMember.Utilities.Heal)				{ current.Utilities.Heal = true; }

				std::string id = squadMember.AccountName; // helper for unique chkbxIds

				bool untracked = squadMember.Untracked(); // red font if not tracked

				// player name field
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text(squadMember.CharacterName.empty() ? squadMember.AccountName.c_str() : squadMember.CharacterName.c_str());
				squadMember.Tooltip();
				squadMember.ContextMenu();

				// player sub field
				ImGui::TableSetColumnIndex(1); ImGui::SetNextItemWidth(64);
				ImGui::InputInt(("##Sub" + id).c_str(), &squadMember.Subgroup);
				if (squadMember.Subgroup < 1) { squadMember.Subgroup = 1; } // sub min is 1
				if (squadMember.Subgroup > 15) { squadMember.Subgroup = 15; } // sub max is 15

				// player utility fields
				ImGui::TableSetColumnIndex(2); ImGui::CheckboxCenteredColumn(("##Might" + id).c_str(), &squadMember.Utilities.Might);
				ImGui::TableSetColumnIndex(3); ImGui::CheckboxCenteredColumn(("##Alac" + id).c_str(), &squadMember.Utilities.Alacrity);
				ImGui::TableSetColumnIndex(4); ImGui::CheckboxCenteredColumn(("##Quic" + id).c_str(), &squadMember.Utilities.Quickness);
				ImGui::TableSetColumnIndex(5); ImGui::CheckboxCenteredColumn(("##Fury" + id).c_str(), &squadMember.Utilities.Fury);
				ImGui::TableSetColumnIndex(6); ImGui::CheckboxCenteredColumn(("##Vuln" + id).c_str(), &squadMember.Utilities.Vulnerability);
				ImGui::TableSetColumnIndex(7); ImGui::CheckboxCenteredColumn(("##Heal" + id).c_str(), &squadMember.Utilities.Heal);
				ImGui::TableSetColumnIndex(8); ImGui::SetNextItemWidth(notesWidth); ImGui::InputText(("##Notes" + id).c_str(), squadMember.Notes, sizeof(squadMember.Notes));

				bool remove = false;
				// player action field
				if (untracked) // show remove button if not tracked
				{
					ImGui::TableSetColumnIndex(9);
					if (ImGui::SmallButton(("Remove##" + id).c_str())) { remove = true; }
					ImGui::PopStyleColor(); // reset red font
				}

				if (remove)
				{
					it = SquadMembers.erase(it);
				}
				else
				{
					++it;
				}
			}

			if (current.AmountPlayers) // if has players render subgroup
			{
				bool fullCoverage = current.Covered();

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0); ImGui::Text("Subgroup"); if (ShortWarnings) { current.WarningShort(); }
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
	for (auto it = SquadMembers.begin(); it != SquadMembers.end();)
	{
		if (time(0) - it->second.LastSeen >= 60 * 30)
		{
			it = SquadMembers.erase(it);
		}
		else
		{
			++it;
		}
	}

	return 0;
}

void SquadManager::MarkAllUntrackedButSelf()
{
	for (auto it = SquadMembers.begin(); it != SquadMembers.end(); ++it)
	{
		if (it->first != SelfAccountName)
		{
			it->second.IsTrackedByArcdps = false;
			it->second.IsTrackedByExtras = false;
		}
	}
}

Player* SquadManager::FindPlayerByID(uintptr_t id)
{
	for (auto it = SquadMembers.begin(); it != SquadMembers.end(); ++it)
	{
		if (it->second.ID == id)
		{
			return &it->second;
		}
	}
	return nullptr;
}

Player* SquadManager::FindPlayerByAccountName(const std::string &accountName)
{
	auto it = SquadMembers.find(accountName);
	if (it != SquadMembers.end())
	{
		return &it->second;
	}
	return nullptr;
}
