///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  SquadMgr.cpp
/// Description  :  Squad manager logic.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "SquadMgr.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "nexus/Nexus.h"
#include "RTAPI/RTAPI.h"
#include "resource.h"
#include "Globals.h"
#include "Util/src/Strings.h"
#include "Util/src/Time.h"
#include "Util.h"

void PlayerLeftTooltip(const bool& aActive, const long long& aSeconds)
{
	if (!aActive) { return; }

	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::Text("Last seen %u %s ago.",
					aSeconds < 60 ? aSeconds : aSeconds / 60,
					aSeconds < 60 ? "seconds" : "minutes");
		ImGui::EndTooltip();
	};
}

void CSquadMgr::Render()
{
	if (!this->Visible)
	{
		return;
	}

	static ImGuiWindowFlags s_WndFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

	long long now = Time::GetTimestamp();

	if (ImGui::Begin("Squad Manager", &this->Visible, s_WndFlags))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		const std::lock_guard<std::mutex> lock(this->Mutex);

		if (this->Players.size() == 0)
		{
			ImGui::Text("Not in a squad or party.");
		}
		else
		{
			ImGui::Text("RealTime API not installed.");
			if (ImGui::BeginTable("##TableSquadMgr", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				float sz = ImGui::GetFontSize();

				/* Header Row*/
				{
					ImGui::TableHeadersRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Account");

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("Character");

					ImGui::TableSetColumnIndex(2);
					ImGui::Text("Damage");

					ImGui::TableSetColumnIndex(3);
					RenderIcon(sz, &G::Textures[ETextures::BoonAlacrity], "TEX_BOON_ALACRITY", "Alacrity", IDB_ALACRITY);

					ImGui::TableSetColumnIndex(4);
					RenderIcon(sz, &G::Textures[ETextures::BoonQuickness], "TEX_BOON_QUICKNESS", "Quickness", IDB_QUICKNESS);

					ImGui::TableSetColumnIndex(5);
					ImGui::Text("Notes");
				}

				for (size_t sub = 0; sub <= 15; sub++)
				{
					int           playerCount = 0;
					UtilityInfo_t summary = {};
					int           dmgType_pwr = 0;
					int           dmgType_cnd = 0;
					int           dmgType_heal = 0;

					std::string rem;

					for (auto& [accname, player] : this->Players)
					{
						if (player.Member.Subgroup != sub) { continue; }

						/* Push red font, if player left. */
						if (player.HasLeft && !player.Member.IsSelf)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(172, 89, 89, 255));
						}

						int secondsSinceLeft = now - player.HasLeft;

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						if (player.Member.IsCommander)
						{
							RenderIcon(sz, &G::Textures[ETextures::TagCommander], "TEX_TAG_COMMANDER", "Commander", IDB_ICON_COMMANDER);
							ImGui::SameLine();
						}
						else if (player.Member.IsLieutenant)
						{
							RenderIcon(sz, &G::Textures[ETextures::TagLieutenant], "TEX_TAG_LIEUTENANT", "Lieutenant", IDB_ICON_LIEUTENANT);
							ImGui::SameLine();
						}
						ImGui::Text(player.Member.AccountName);
						PlayerLeftTooltip(player.HasLeft, secondsSinceLeft);

						ImGui::TableNextColumn();
						ImGui::Text(player.Member.CharacterName);
						PlayerLeftTooltip(player.HasLeft, secondsSinceLeft);

						ImGui::TableNextColumn();
						std::string dmgTypePreview =
							player.UtilityInfo.DamageType == EDamageType::None
								? "None"
								: player.UtilityInfo.DamageType == EDamageType::Power
									? "PWR"
									: player.UtilityInfo.DamageType == EDamageType::Condition
										? "CND"
										: "HEAL";
						ImGui::SetNextItemWidth(sz * 5);
						if (ImGui::BeginCombo(("##DamageType_" + std::string(player.Member.AccountName)).c_str(), dmgTypePreview.c_str()))
						{
							if (ImGui::Selectable("-", player.UtilityInfo.DamageType == EDamageType::None))
							{
								player.UtilityInfo.DamageType = EDamageType::None;
							}

							if (ImGui::Selectable("PWR", player.UtilityInfo.DamageType == EDamageType::Power))
							{
								player.UtilityInfo.DamageType = EDamageType::Power;
							}

							if (ImGui::Selectable("CND", player.UtilityInfo.DamageType == EDamageType::Condition))
							{
								player.UtilityInfo.DamageType = EDamageType::Condition;
							}

							if (ImGui::Selectable("HEAL", player.UtilityInfo.DamageType == EDamageType::Heal))
							{
								player.UtilityInfo.DamageType = EDamageType::Heal;
							}

							ImGui::EndCombo();
						}

						ImGui::TableNextColumn();
						ImGui::Checkbox(("##Alacrity_" + std::string(player.Member.AccountName)).c_str(), &player.UtilityInfo.Alacrity);

						ImGui::TableNextColumn();
						ImGui::Checkbox(("##Quickness_" + std::string(player.Member.AccountName)).c_str(), &player.UtilityInfo.Quickness);

						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(sz * 10);
						ImGui::InputText(("##Note_" + std::string(player.Member.AccountName)).c_str(), &player.Note[0], sizeof(player.Note));
						PlayerLeftTooltip(player.HasLeft, secondsSinceLeft);

						if (player.HasLeft && !player.Member.IsSelf)
						{
							ImGui::SameLine();
							if (G::Textures[ETextures::BtnClose])
							{
								/* Push unique ID because image button uses texture as ID -> not unique. */
								ImGui::PushID(("X##Remove_" + std::string(player.Member.AccountName)).c_str());
								if (ImGui::ImageButton(
									G::Textures[ETextures::BtnClose]->Resource,
									ImVec2(sz, sz),
									ImVec2(0, 0),
									ImVec2(1, 1),
									-1,
									ImVec4(0, 0, 0, 0),
									ImColor(IM_COL32(172, 89, 89, 255))
								))
								{
									rem = accname;
								}
								ImGui::PopID();
							}
							else
							{
								G::Textures[ETextures::BtnClose] = G::APIDefs->Textures.GetOrCreateFromResource("ICON_CLOSE", IDB_CLOSE, G::Module);
								if (ImGui::SmallButton(("X##Remove_" + std::string(player.Member.AccountName)).c_str()))
								{
									rem = accname;
								}
							}
							PlayerLeftTooltip(player.HasLeft, secondsSinceLeft);

							/* Pop red font, if player left. */
							ImGui::PopStyleColor();

							/* Automatically remove, if left 5 minutes ago. */
							if (secondsSinceLeft >= 60 * 5)
							{
								rem = accname;
							}
						}

						switch (player.UtilityInfo.DamageType)
						{
							case EDamageType::Power:
								dmgType_pwr++;
								break;
							case EDamageType::Condition:
								dmgType_cnd++;
								break;
							case EDamageType::Heal:
								dmgType_heal++;
								break;
						}
						if (player.UtilityInfo.Alacrity) { summary.Alacrity = true; }
						if (player.UtilityInfo.Quickness) { summary.Quickness = true; }

						playerCount++;
					}

					if (!rem.empty())
					{
						this->Players.erase(rem);
					}

					if (playerCount == 0) { continue; }

					// subgroup totals
					bool fullCoverage = false;
					if (summary.Alacrity && summary.Quickness)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(89, 172, 98, 255)); // if all utilities are covered -> green text
						fullCoverage = true;
					}

					ImGui::TableHeadersRow();
					ImGui::TableSetColumnIndex(1);
					if (G::RTAPI && sub == 0)
					{
						if (G::RTAPI->GroupType == RTAPI::EGroupType::Party)
						{
							ImGui::Text("Party");
						}
						else
						{
							ImGui::Text("Pending");
						}
					}
					else
					{
						ImGui::Text("Subgroup %d", sub);
					}
					ImGui::TableSetColumnIndex(2);
					ImGui::Text(
						"%s%s%s",
						dmgType_pwr ? String::Format("PWR: %d\n", dmgType_pwr).c_str() : "",
						dmgType_cnd ? String::Format("CND: %d\n", dmgType_cnd).c_str() : "",
						dmgType_heal ? String::Format("HEAL: %d", dmgType_heal).c_str() : ""
					);

					ImU32 textCol = ImU32(ImColor(ImGui::GetStyle().Colors[ImGuiCol_Text]));
					ImU32 successCol = IM_COL32(89, 172, 98, 255);
					ImU32 warnCol = IM_COL32(255, 148, 79, 255);

					ImDrawList* dl = ImGui::GetWindowDrawList();
					if (summary.Alacrity) { ImGui::TableSetColumnIndex(3); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					if (summary.Quickness) { ImGui::TableSetColumnIndex(4); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }

					if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

					if (playerCount > 5) { ImGui::TableSetColumnIndex(5); ImGui::TextColored(ImColor(warnCol), "Warning: More than 5 players!"); }
				}

				ImGui::EndTable();
			}
		}

		ImGui::PopStyleVar();
	}
	ImGui::End();
}

bool CSquadMgr::IsVisible()
{
	return this->Visible;
}

void CSquadMgr::ToggleVisible()
{
	this->Visible = !this->Visible;
}

void CSquadMgr::OnGroupMemberJoin(RTAPI::GroupMember* aGroupMember)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto& player = this->Players[aGroupMember->AccountName];
	player.Member = *aGroupMember;
	player.HasLeft = 0; /* Reset in case the player rejoined without removal. */
}
void CSquadMgr::OnGroupMemberLeave(RTAPI::GroupMember* aGroupMember)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	if (aGroupMember->IsSelf)
	{
		if (G::RTAPI || G::IsUEEnabled)
		{
			this->Players.clear();
		}
	}
	else
	{
		this->Players[aGroupMember->AccountName].HasLeft = Time::GetTimestamp();
	}
}
void CSquadMgr::OnGroupMemberUpdate(RTAPI::GroupMember* aGroupMember)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto& player = this->Players[aGroupMember->AccountName];
	player.Member = *aGroupMember;
	player.HasLeft = 0; /* Reset in case the player rejoined without removal. */
}

void CSquadMgr::OnAgentJoin(AgentUpdate* aAgentUpdate)
{
	if (!aAgentUpdate) { return; }

	const std::lock_guard<std::mutex> lock(this->CacheMutex);

	this->CachedAgentsArc[&aAgentUpdate->AccountName[1]] = *aAgentUpdate;

	/* Is in party according to arc, else it'd give the display sub. */
	if (aAgentUpdate->Subgroup == 0)
	{
		this->CachedAgentsArc[&aAgentUpdate->AccountName[1]].Subgroup++;
	}

	RTAPI::GroupMember member{};
	strcpy_s(&member.AccountName[0], sizeof(member.AccountName), &aAgentUpdate->AccountName[1]);
	strcpy_s(&member.CharacterName[0], sizeof(member.CharacterName), &aAgentUpdate->CharacterName[0]);
	member.Profession = aAgentUpdate->Prof;
	member.EliteSpecialization = aAgentUpdate->Elite;
	member.IsSelf = aAgentUpdate->Self;
	member.IsInInstance = true; // arc only reports in instance players

	auto ue = this->CachedAgentsUE.find(member.AccountName);

	if (ue != this->CachedAgentsUE.end())
	{
		member.Subgroup = ue->second.Subgroup;
		member.IsCommander = ue->second.Role == UserRole::SquadLeader;
		member.IsLieutenant = ue->second.Role == UserRole::Lieutenant;
	}
	else
	{
		member.Subgroup = aAgentUpdate->Subgroup;
	}

	if (G::RTAPI) { return; }
	this->OnGroupMemberUpdate(&member);
}
void CSquadMgr::OnAgentLeave(AgentUpdate* aAgentUpdate)
{
	if (!aAgentUpdate) { return; }
	if (G::IsUEEnabled) { return; } // We only want leave events from UE.

	RTAPI::GroupMember member{};

	/* Minimal data needed here. */
	strcpy_s(&member.AccountName[0], sizeof(member.AccountName), &aAgentUpdate->AccountName[1]);
	member.IsSelf = aAgentUpdate->Self;

	if (G::RTAPI) { return; }
	this->OnGroupMemberLeave(&member);
}
void CSquadMgr::OnSquadUpdate(SquadUpdate* aSquadUpdate)
{
	G::IsUEEnabled = true;
	
	if (G::RTAPI) { return; } /* Prefer RTAPI updates over UE. */

	for (size_t i = 0; i < aSquadUpdate->UsersCount; i++)
	{
		const std::lock_guard<std::mutex> lock(this->CacheMutex);

		UserInfo& user = aSquadUpdate->UserInfo[i];
		RTAPI::GroupMember member{};
		AgentUpdate& agent = this->CachedAgentsArc[&user.AccountName[1]];

		this->CachedAgentsUE[&user.AccountName[1]] = user;
		this->CachedAgentsUE[&user.AccountName[1]].Subgroup++;

		strcpy_s(&member.AccountName[0], sizeof(member.AccountName), &user.AccountName[1]);
		strcpy_s(&member.CharacterName[0], sizeof(member.CharacterName), &agent.CharacterName[0]);
		member.Subgroup = user.Subgroup;
		/* UE reports 0 based subgroup instead of display value. */
		if (user.Role != UserRole::Invited && user.Role != UserRole::Applied)
		{
			member.Subgroup++;
		}
		member.Profession = agent.Prof;
		member.EliteSpecialization = agent.Elite;
		member.IsInInstance = agent.ID > 0;
		member.IsSelf = agent.Self;
		member.IsCommander  = user.Role == UserRole::SquadLeader;
		member.IsLieutenant = user.Role == UserRole::Lieutenant;

		if (user.Role == UserRole::None)
		{
			this->CachedAgentsUE.erase(&user.AccountName[1]);
			if (!this->CachedAgentsArc[&user.AccountName[1]].Self)
			{
				this->CachedAgentsArc.erase(&user.AccountName[1]);
			}

			if (G::RTAPI) { return; }
			this->OnGroupMemberLeave(&member);
		}
		else
		{
			if (G::RTAPI) { return; }
			this->OnGroupMemberUpdate(&member);
		}
	}
}