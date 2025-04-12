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

	static ImU32 successCol = IM_COL32(89, 172, 98, 255);
	static ImU32 warnCol = IM_COL32(255, 148, 79, 255);

	if (ImGui::Begin("Squad Manager", &this->Visible, s_WndFlags))
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

		const std::lock_guard<std::mutex> lock(this->Mutex);

		/* Cache these so that no race cond can happen. */
		bool hasRTAPI = G::RTAPI != nullptr;
		bool hasUE = G::IsUEEnabled;

		if (this->Players.size() == 0)
		{
			ImGui::Text("Not in a squad or party.");
		}
		else
		{
			if (!hasRTAPI)
			{
				ImGui::TextColored(ImColor(warnCol), "RealTime API not installed.");
			}

			static bool s_AnyUntracked = false;
			bool anyUntracked = false;

			int dmgIdx = 2;
			int alacIdx = 3;
			int quicIdx = 4;
			int noteIdx = 5;
			int trckIdx = 6;

			if (!(hasRTAPI || hasUE))
			{
				dmgIdx++;
				alacIdx++;
				quicIdx++;
				noteIdx++;
				trckIdx++;
			}

			int columns = 6;
			if (!(hasRTAPI || hasUE)) { columns++; } // To render subgroup column.
			if (s_AnyUntracked)       { columns++; } // To render untracked column.

			if (ImGui::BeginTable("##TableSquadMgr", columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				float sz = ImGui::GetFontSize();

				/* Header Row*/
				{
					ImGui::TableHeadersRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Account");

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("Character");

					if (!(hasRTAPI || hasUE))
					{
						ImGui::TableSetColumnIndex(2);
						ImGui::Text("Subgroup");
					}

					ImGui::TableSetColumnIndex(dmgIdx);
					ImGui::Text("Damage");

					ImGui::TableSetColumnIndex(alacIdx);
					RenderIcon(sz, &G::Textures[ETextures::BoonAlacrity], "TEX_BOON_ALACRITY", "Alacrity", IDB_ALACRITY);

					ImGui::TableSetColumnIndex(quicIdx);
					RenderIcon(sz, &G::Textures[ETextures::BoonQuickness], "TEX_BOON_QUICKNESS", "Quickness", IDB_QUICKNESS);

					ImGui::TableSetColumnIndex(noteIdx);
					ImGui::Text("Notes");

					if (s_AnyUntracked)
					{
						ImGui::TableSetColumnIndex(trckIdx);
						if (G::Textures[ETextures::BtnClose])
						{
							/* Push unique ID because image button uses texture as ID -> not unique. */
							ImGui::PushID("X##RemoveAll");
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
								for (auto it = this->Players.begin(); it != this->Players.end(); )
								{
									if (it->second.HasLeft > 0)
									{
										it = this->Players.erase(it);
									}
									else
									{
										++it;
									}
								}
							}
							ImGui::PopID();
						}
						else
						{
							G::Textures[ETextures::BtnClose] = G::APIDefs->Textures.GetOrCreateFromResource("ICON_CLOSE", IDB_CLOSE, G::Module);
							if (ImGui::SmallButton("X##RemoveAll"))
							{
								for (auto it = this->Players.begin(); it != this->Players.end(); )
								{
									if (it->second.HasLeft > 0)
									{
										it = this->Players.erase(it);
									}
									else
									{
										++it;
									}
								}
							}
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							ImGui::Text("Remove all untracked players.");
							ImGui::EndTooltip();
						};
					}
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
							RenderIcon(sz, &G::Textures[ETextures::TagCommander], "TEX_TAG_COMMANDER", "Commander", IDB_TAG_COMMANDER);
							ImGui::SameLine();
						}
						else if (player.Member.IsLieutenant)
						{
							RenderIcon(sz, &G::Textures[ETextures::TagLieutenant], "TEX_TAG_LIEUTENANT", "Lieutenant", IDB_TAG_LIEUTENANT);
							ImGui::SameLine();
						}
						ImGui::Text(player.Member.AccountName);
						PlayerLeftTooltip(player.HasLeft, secondsSinceLeft);

						ImGui::TableNextColumn();
						RenderSpecIcon(sz, player.Member.Profession, player.Member.EliteSpecialization);
						ImGui::SameLine();
						ImGui::Text(player.Member.CharacterName);
						PlayerLeftTooltip(player.HasLeft, secondsSinceLeft);

						/* Render subgroup input if no RTAPI. */
						if (!(hasRTAPI || hasUE))
						{
							ImGui::TableNextColumn();
							ImGui::SetNextItemWidth(sz * 5);
							ImGui::InputInt(("##Subgroup_" + std::string(player.Member.AccountName)).c_str(), &player.Member.Subgroup);
							player.Member.Subgroup = min(max(player.Member.Subgroup, 1), 15);
						}

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
							if (ImGui::Selectable("None", player.UtilityInfo.DamageType == EDamageType::None))
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
							anyUntracked = true;

							ImGui::TableNextColumn();

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
					if (hasRTAPI && sub == 0)
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
					ImGui::TableSetColumnIndex(dmgIdx);
					ImGui::Text(
						"%s%s%s",
						dmgType_pwr ? String::Format("PWR: %d\n", dmgType_pwr).c_str() : "",
						dmgType_cnd ? String::Format("CND: %d\n", dmgType_cnd).c_str() : "",
						dmgType_heal ? String::Format("HEAL: %d", dmgType_heal).c_str() : ""
					);

					ImU32 textCol = ImU32(ImColor(ImGui::GetStyle().Colors[ImGuiCol_Text]));

					ImDrawList* dl = ImGui::GetWindowDrawList();
					if (summary.Alacrity) { ImGui::TableSetColumnIndex(alacIdx); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					if (summary.Quickness) { ImGui::TableSetColumnIndex(quicIdx); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }

					if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

					if (playerCount > 5) { ImGui::TableSetColumnIndex(noteIdx); ImGui::TextColored(ImColor(warnCol), "Warning: More than 5 players!"); }
				}

				s_AnyUntracked = anyUntracked;

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
		/* Only clear for RT/UE, because arc also reports changes on load screen. */
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