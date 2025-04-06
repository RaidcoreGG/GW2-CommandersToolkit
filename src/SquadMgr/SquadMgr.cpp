///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  SquadMgr.cpp
/// Description  :  Squad manager logic.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "SquadMgr.h"

#include <mutex>
#include <unordered_map>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "nexus/Nexus.h"
#include "RTAPI/RTAPI.h"
#include "PlayerInfo.h"
#include "resource.h"
#include "Globals.h"
#include "Util/src/Strings.h"

namespace SquadMgr
{
	static bool                                          s_Visible          = false;

	static std::mutex                                    s_Mutex;
	static std::unordered_map<std::string, PlayerInfo_t> s_Players;

	static Texture*                                      s_TexMight         = nullptr;
	static Texture*                                      s_TexAlacrity      = nullptr;
	static Texture*                                      s_TexQuickness     = nullptr;
	static Texture*                                      s_TexFury          = nullptr;
	static Texture*                                      s_TexVulnerability = nullptr;
	static Texture*                                      s_TexHeal          = nullptr;
	static Texture*                                      s_TexTagCommander  = nullptr;
	static Texture*                                      s_TexTagLieutenant = nullptr;

	void RenderIcon(float& aSize, Texture** aTexture, std::string aIdentifer, std::string aAltText, uint32_t aResourceID)
	{
		if (*aTexture)
		{
			ImGui::Image((*aTexture)->Resource, ImVec2(aSize, aSize));
			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::Text(aAltText.c_str());
				ImGui::EndTooltip();
			}
		}
		else
		{
			*aTexture = G::APIDefs->Textures.GetOrCreateFromResource(aIdentifer.c_str(), aResourceID, G::Module);
			ImGui::Text(aAltText.c_str());
		}
	}

	void Render()
	{
		if (!s_Visible)
		{
			return;
		}

		static ImGuiWindowFlags s_WndFlags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse;

		if (ImGui::Begin("Squad Manager", &s_Visible, s_WndFlags))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

			if (ImGui::BeginTable("##TableSquadMgr", 10, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
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
					ImGui::Text("Sub");

					ImGui::TableSetColumnIndex(3);
					ImGui::Text("Damage");

					ImGui::TableSetColumnIndex(4);
					RenderIcon(sz, &s_TexMight, "TEX_BOON_MIGHT", "Might", IDB_MIGHT);

					ImGui::TableSetColumnIndex(5);
					RenderIcon(sz, &s_TexAlacrity, "TEX_BOON_ALACRITY", "Alacrity", IDB_ALACRITY);

					ImGui::TableSetColumnIndex(6);
					RenderIcon(sz, &s_TexQuickness, "TEX_BOON_QUICKNESS", "Quickness", IDB_QUICKNESS);

					ImGui::TableSetColumnIndex(7);
					RenderIcon(sz, &s_TexFury, "TEX_BOON_FURY", "Fury", IDB_FURY);

					ImGui::TableSetColumnIndex(8);
					RenderIcon(sz, &s_TexVulnerability, "TEX_BOON_VULNERABILITY", "Vulnerability", IDB_VULNERABILITY);

					ImGui::TableSetColumnIndex(9);
					ImGui::Text("Notes");
				}

				const std::lock_guard<std::mutex> lock(s_Mutex);

				for (size_t sub = 0; sub <= 15; sub++)
				{
					int           playerCount  = 0;
					UtilityInfo_t summary      = {};
					int           dmgType_pwr  = 0;
					int           dmgType_cnd  = 0;
					int           dmgType_heal = 0;
					
					for (auto& [accname, player] : s_Players)
					{
						if (player.Member.Subgroup != sub) { continue; }

						ImGui::TableNextRow();
						ImGui::TableNextColumn();
						if (player.Member.IsCommander)
						{
							RenderIcon(sz, &s_TexTagCommander, "TEX_TAG_COMMANDER", "Commander", IDB_ICON_COMMANDER);
							ImGui::SameLine();
						}
						else if (player.Member.IsLieutenant)
						{
							RenderIcon(sz, &s_TexTagLieutenant, "TEX_TAG_LIEUTENANT", "Lieutenant", IDB_ICON_LIEUTENANT);
							ImGui::SameLine();
						}
						ImGui::Text(player.Member.AccountName);

						ImGui::TableNextColumn();
						ImGui::Text(player.Member.CharacterName);

						ImGui::TableNextColumn();
						ImGui::Text("%d", player.Member.Subgroup);

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
						ImGui::Checkbox(("##Might_" + std::string(player.Member.AccountName)).c_str(), &player.UtilityInfo.Might);

						ImGui::TableNextColumn();
						ImGui::Checkbox(("##Alacrity_" + std::string(player.Member.AccountName)).c_str(), &player.UtilityInfo.Alacrity);

						ImGui::TableNextColumn();
						ImGui::Checkbox(("##Quickness_" + std::string(player.Member.AccountName)).c_str(), &player.UtilityInfo.Quickness);

						ImGui::TableNextColumn();
						ImGui::Checkbox(("##Fury_" + std::string(player.Member.AccountName)).c_str(), &player.UtilityInfo.Fury);

						ImGui::TableNextColumn();
						ImGui::Checkbox(("##Vulnerability_" + std::string(player.Member.AccountName)).c_str(), &player.UtilityInfo.Vulnerability);

						ImGui::TableNextColumn();
						ImGui::SetNextItemWidth(sz * 10);
						ImGui::InputText(("##Note_" + std::string(player.Member.AccountName)).c_str(), &player.Note[0], sizeof(player.Note));

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
						if (player.UtilityInfo.Might)         { summary.Might = true; }
						if (player.UtilityInfo.Alacrity)      { summary.Alacrity = true; }
						if (player.UtilityInfo.Quickness)     { summary.Quickness = true; }
						if (player.UtilityInfo.Fury)          { summary.Fury = true; }
						if (player.UtilityInfo.Vulnerability) { summary.Vulnerability = true; }

						playerCount++;
					}

					if (playerCount == 0) { continue; }

					// subgroup totals
					bool fullCoverage = false;
					if (summary.Might && summary.Alacrity && summary.Quickness && summary.Fury && summary.Vulnerability)
					{
						ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(89, 172, 98, 255)); // if all utilities are covered -> green text
						fullCoverage = true;
					}

					ImGui::TableHeadersRow();
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("Subgroup");
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%u", sub);
					ImGui::TableSetColumnIndex(3);
					ImGui::Text(
						"%s%s%s",
						dmgType_pwr  ? String::Format("PWR: %d\n",  dmgType_pwr).c_str()  : "",
						dmgType_cnd  ? String::Format("CND: %d\n",  dmgType_cnd).c_str()  : "",
						dmgType_heal ? String::Format("HEAL: %d",   dmgType_heal).c_str() : ""
					);

					ImU32 textCol = ImU32(ImColor(ImGui::GetStyle().Colors[ImGuiCol_Text]));
					ImU32 successCol = IM_COL32(89, 172, 98, 255);
					ImU32 warnCol = IM_COL32(255, 148, 79, 255);

					ImDrawList* dl = ImGui::GetWindowDrawList();
					if (summary.Might)         { ImGui::TableSetColumnIndex(4); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					if (summary.Alacrity)      { ImGui::TableSetColumnIndex(5); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					if (summary.Quickness)     { ImGui::TableSetColumnIndex(6); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					if (summary.Fury)          { ImGui::TableSetColumnIndex(7); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					if (summary.Vulnerability) { ImGui::TableSetColumnIndex(8); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					
					if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

					if (playerCount > 5) { ImGui::TableSetColumnIndex(9); ImGui::TextColored(ImColor(warnCol), "Warning: More than 5 players!"); }
				}

				ImGui::EndTable();

				ImGui::PopStyleVar();
			}
		}
		ImGui::End();
	}

	bool IsVisible()
	{
		return s_Visible;
	}

	void ToggleVisible()
	{
		s_Visible = !s_Visible;
	}

	void OnGroupMemberJoin(void* aEventArgs)
	{
		RTAPI::GroupMember* member = (RTAPI::GroupMember*)aEventArgs;

		const std::lock_guard<std::mutex> lock(s_Mutex);

		s_Players[member->AccountName] = PlayerInfo_t{
			*member
		};
	}

	void OnGroupMemberLeave(void* aEventArgs)
	{
		RTAPI::GroupMember* member = (RTAPI::GroupMember*)aEventArgs;

		const std::lock_guard<std::mutex> lock(s_Mutex);

		if (member->IsSelf)
		{
			s_Players.clear();
		}
		else
		{
			s_Players.erase(member->AccountName);
		}
	}

	void OnGroupMemberUpdate(void* aEventArgs)
	{
		RTAPI::GroupMember* member = (RTAPI::GroupMember*)aEventArgs;

		const std::lock_guard<std::mutex> lock(s_Mutex);

		s_Players[member->AccountName].Member = *member;
	}
}
