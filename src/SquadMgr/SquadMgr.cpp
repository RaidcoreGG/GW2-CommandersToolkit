///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  SquadMgr.cpp
/// Description  :  Squad manager logic.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "SquadMgr.h"

#include <thread>
#pragma comment(lib, "wininet.lib")
#include <wininet.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "nlohmann/json.hpp"
using nlohmann::json;

#include "nexus/Nexus.h"
#include "RTAPI/RTAPI.hpp"
#include "resource.h"
#include "Globals.h"
#include "Util/src/Strings.h"
#include "Util/src/Time.h"
#include "Util.h"

void KPRequirementEditor(KPMEInfo_t& aKP)
{
	std::string popupName = "KPRequirementEditor";

	if (ImGui::SmallButton("KP Requirement"))
	{
		ImGui::OpenPopup(popupName.c_str());
	}

	float sz = ImGui::GetFontSize();

	if (ImGui::BeginPopupContextItem(popupName.c_str()))
	{
		ImGui::Text("Set the KP Requirements to check against within the group.");
		ImGui::Text("Raid coffers are normalized to 3 tokens.");
		if (ImGui::TreeNode("Raids"))
		{
			ImGui::SetNextItemWidth(sz * 10);
			ImGui::InputInt("Legendary Insights", &aKP.Raids.LI, 1, 50);

			if (ImGui::TreeNode("Wing 1 - Spirit Vale"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Vale Guardian", &aKP.Raids.W1_ValeGuardian, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Gorseval", &aKP.Raids.W1_Gorseval, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Sabetha", &aKP.Raids.W1_Sabetha, 1, 50);

				ImGui::Checkbox("The Eternal", &aKP.Raids.W1_TheEternal);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wing 2 - Salvation Pass"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Slothasor", &aKP.Raids.W2_Slothasor, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Matthias", &aKP.Raids.W2_Matthias, 1, 50);

				ImGui::Checkbox("Slippery Slubling", &aKP.Raids.W2_SlipperySlubling);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wing 3 - Stronghold of the Faithful"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Escort", &aKP.Raids.W3_Escort, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Keep Construct", &aKP.Raids.W3_KeepConstruct, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Xera", &aKP.Raids.W3_Xera, 1, 50);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wing 4 - Bastion of the Penitent"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Cairn", &aKP.Raids.W4_Cairn, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Mursaat Overseer", &aKP.Raids.W4_MursaatOverseer, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Samarog", &aKP.Raids.W4_Samarog, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Deimos", &aKP.Raids.W4_Deimos, 1, 50);

				ImGui::Checkbox("Comitted", &aKP.Raids.W4_Committed);
				ImGui::Checkbox("Silent Savior", &aKP.Raids.W4_SilentSavior);
				ImGui::Checkbox("Demons Demise", &aKP.Raids.W4_DemonsDemise);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wing 5 - Hall of Chains"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Soulless Horror", &aKP.Raids.W5_SoullessHorror, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Dhuum", &aKP.Raids.W5_Dhuum, 1, 50);

				ImGui::Checkbox("Voice in the Void", &aKP.Raids.W5_VoiceInTheVoid);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wing 6 - Mythwright Gambit"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Conjured Amalgamate", &aKP.Raids.W6_ConjuredAmalgamate, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Largos Twins", &aKP.Raids.W6_LargosTwins, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Qadim", &aKP.Raids.W6_Qadim, 1, 50);

				ImGui::Checkbox("Champion of Zomorros", &aKP.Raids.W6_ChampionOfZomorros);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wing 7 - The Key of Ahdashim"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Cardinal Adina", &aKP.Raids.W7_CardinalAdina, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Cardinal Sabir", &aKP.Raids.W7_CardinalSabir, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Qadim the Peerless", &aKP.Raids.W7_QadimThePeerless, 1, 50);

				ImGui::Checkbox("Everlasting Ally of Ahdashim", &aKP.Raids.W7_EverlastingAllyOfAhdashim);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Wing 8 - Mount Balrior"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Greer", &aKP.Raids.W8_Greer, 1, 50);
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Greer CM", &aKP.Raids.W8_GreerCM, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Decima", &aKP.Raids.W8_Decima, 1, 50);
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Decima CM", &aKP.Raids.W8_DecimaCM, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Ura", &aKP.Raids.W8_Ura, 1, 50);
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Ura CM", &aKP.Raids.W8_UraCM, 1, 50);

				ImGui::Checkbox("The Eternal", &aKP.Raids.W8_TheEternal);
				ImGui::Checkbox("Raid Expert", &aKP.Raids.W8_RaidExpert);
				ImGui::Checkbox("Godsbane", &aKP.Raids.W8_Godsbane);
				ImGui::Checkbox("Legendary Conqueror of Ura", &aKP.Raids.W8_LegendaryConquerorOfUra);

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Strikes"))
		{
			if (ImGui::TreeNode("Icebrood Saga"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Boneskinner Ritual Vials", &aKP.Strikes.IBS_BoneskinnerVial, 1, 50);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("End of Dragons"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Aetherblade Hideout", &aKP.Strikes.EOD_AH, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Aetherblade Hideout (CM)", &aKP.Strikes.EOD_AHCM, 1, 50);

				ImGui::Checkbox("Peerless Geometer", &aKP.Strikes.EOD_PeerlessGeometer);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Kaineng Overlook", &aKP.Strikes.EOD_KO, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Kaineng Overlook (CM)", &aKP.Strikes.EOD_KOCM, 1, 50);

				ImGui::Checkbox("The Great Equalizer", &aKP.Strikes.EOD_TheGreatEqualizer);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Ankka", &aKP.Strikes.EOD_Ankka, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Ankka (CM)", &aKP.Strikes.EOD_AnkkaCM, 1, 50);

				ImGui::Checkbox("Gazed into the Void", &aKP.Strikes.EOD_GazedIntoTheVoid);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Harvest Temple", &aKP.Strikes.EOD_HT, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Harvest Temple (CM)", &aKP.Strikes.EOD_HTCM, 1, 50);

				ImGui::Checkbox("The Voidwalker", &aKP.Strikes.EOD_TheVoidwalker);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("LWS1 - Scarlet's War"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Old Lion's Court", &aKP.Strikes.EOD_OLC, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Old Lion's Court (CM)", &aKP.Strikes.EOD_OLCCM, 1, 50);

				ImGui::Checkbox("Defender of Lion's Court", &aKP.Strikes.EOD_DefenderOfLionsCourt);

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Secrets of the Obscure"))
			{
				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Cosmic Observatory", &aKP.Strikes.EOD_Dagda, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Cosmic Observatory (CM)", &aKP.Strikes.EOD_DagdaCM, 1, 50);

				ImGui::Checkbox("Mind Flayer", &aKP.Strikes.EOD_MindFlayer);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Temple of Febe", &aKP.Strikes.EOD_Cerus, 1, 50);

				ImGui::SetNextItemWidth(sz * 10);
				ImGui::InputInt("Temple of Febe (CM)", &aKP.Strikes.EOD_CerusCM, 1, 50);

				ImGui::Checkbox("Embodiment of Sin", &aKP.Strikes.EOD_EmbodimentOfSin);
				ImGui::Checkbox("Legendary Conqueror of Cerus", &aKP.Strikes.EOD_LegendaryConquerorOfCerus);

				ImGui::TreePop();
			}
			
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Fractals"))
		{
			ImGui::SetNextItemWidth(sz * 10);
			ImGui::InputInt("Unstable Fractal Essence", &aKP.Fractals.UFE, 1, 50);

			ImGui::Checkbox("The Unclean - Nightmare Fractal", &aKP.Fractals.TheUnclean);
			ImGui::Checkbox("Leaves No Hero Behind - Shattered Observatory", &aKP.Fractals.LNHB);
			ImGui::Checkbox("Defier of Doubt - Sunqua Peak", &aKP.Fractals.DoD);
			ImGui::Checkbox("Dances with Demons - Sunqua Peak", &aKP.Fractals.DwD);
			ImGui::Checkbox("Nightmare Aspect - Silent Surf", &aKP.Fractals.NightmareAspect);
			ImGui::Checkbox("Kryptis Exorcist - Lonely Tower", &aKP.Fractals.KryptisExorcist);

			ImGui::Checkbox("Fractal Savant", &aKP.Fractals.FractalSavant);
			ImGui::Checkbox("Fractal Prodigy", &aKP.Fractals.FractalProdigy);
			ImGui::Checkbox("Fractal Champion", &aKP.Fractals.FractalChampion);
			ImGui::Checkbox("Fractal God", &aKP.Fractals.FractalGod);

			ImGui::TreePop();
		}

		ImGui::EndPopup();
	}
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
	static ImU32 errCol = IM_COL32(172, 89, 89, 255);

	if (ImGui::Begin("Squad Manager", &this->Visible, s_WndFlags))
	{
		const std::lock_guard<std::mutex> lock(this->Mutex);

		/* Cache these so that no race cond can happen. */
		bool hasRTAPI = G::RTAPI != nullptr;
		bool hasUE = G::IsUEEnabled;
		static const KPMEInfo_t nullcmp{};
		bool hasKPReq = memcmp(&this->KPRequirement, &nullcmp, sizeof(KPMEInfo_t)) != 0;

		float sz = ImGui::GetFontSize();

		if (this->Players.size() == 0)
		{
			ImGui::Text("Not in a squad or party.");
		}
		else
		{
			if (!(hasRTAPI || hasUE))
			{
				ImGui::TextColored(ImColor(warnCol), "RealTime API not installed.");
			}


			KPRequirementEditor(this->KPRequirement);
			if (hasKPReq)
			{
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::Text("Requirements:");
					ImGui::Text(CheckRequirements(KPMEInfo_t{}, this->KPRequirement, true).c_str());
					ImGui::EndTooltip();
				}

				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
				if (RenderIconButton(sz, &G::Textures[ETextures::BtnClose], "ICON_CLOSE", "Clear KP Requirements", IDB_CLOSE))
				{
					this->KPRequirement = {};
				}
				ImGui::PopStyleVar();
			}

			static bool s_AnyUntracked = false;
			bool anyUntracked = false;

			int subIdx = 2;
			int kpIdx = 2;
			int dmgIdx = 2;
			int alacIdx = 3;
			int quicIdx = 4;
			int noteIdx = 5;
			int trckIdx = 6;

			/* Advance columns. */
			if (!(hasRTAPI || hasUE))
			{
				kpIdx++;
				dmgIdx++;
				alacIdx++;
				quicIdx++;
				noteIdx++;
				trckIdx++;
			}

			/* Advance columns. */
			if (hasKPReq)
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
			if (hasKPReq)             { columns++; } // To render kp column.

			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

			if (ImGui::BeginTable("##TableSquadMgr", columns, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImDrawList* dl = ImGui::GetWindowDrawList();

				/* Header Row*/
				{
					ImGui::TableHeadersRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Account");

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("Character");

					if (!(hasRTAPI || hasUE))
					{
						ImGui::TableSetColumnIndex(subIdx);
						ImGui::Text("Subgroup");
					}

					if (hasKPReq)
					{
						ImGui::TableSetColumnIndex(kpIdx);
						ImGui::Text("KP");
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
						if (RenderIconButton(sz, &G::Textures[ETextures::BtnClose], "ICON_CLOSE", "Remove all untracked##RemoveAll", IDB_CLOSE))
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

						long long secondsSinceLeft = now - player.HasLeft;

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

						if (hasKPReq)
						{
							ImGui::TableNextColumn();
							if (player.KPMEInfo != nullptr)
							{
								std::string reqs = CheckRequirements(*player.KPMEInfo, this->KPRequirement).c_str();

								if (!reqs.empty())
								{
									ImGui::TextColored(ImColor(warnCol), reqs.c_str());
								}
								else
								{
									ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), successCol, sz);
									ImGui::Dummy(ImVec2(sz, sz));
								}
							}
							else
							{
								ImGui::Text("NO DATA");
							}
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

							if (RenderIconButton(sz, &G::Textures[ETextures::BtnClose], "ICON_CLOSE", ("X##Remove_" + std::string(player.Member.AccountName)).c_str(), IDB_CLOSE))
							{
								rem = accname;
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

					if (summary.Alacrity) { ImGui::TableSetColumnIndex(alacIdx); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }
					if (summary.Quickness) { ImGui::TableSetColumnIndex(quicIdx); ImGui::RenderCheckMark(dl, ImGui::GetCursorPos() + ImGui::GetWindowPos(), fullCoverage ? successCol : ImU32(ImColor(textCol)), sz); }

					if (fullCoverage) { ImGui::PopStyleColor(); } // reset green text

					if (playerCount > 5) { ImGui::TableSetColumnIndex(noteIdx); ImGui::TextColored(ImColor(warnCol), "Warning: More than 5 players!"); }
				}

				s_AnyUntracked = anyUntracked;

				ImGui::EndTable();
			}

			ImGui::PopStyleVar();
		}
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

void CSquadMgr::GetKPData(PlayerInfo_t& aPlayer)
{
	if (aPlayer.KPMEInfo == nullptr)
	{
		std::thread([this](std::string aAccountName) {
			KPMEInfo_t* kpmeInfo = nullptr;

			{
				const std::lock_guard<std::mutex> lock(this->KPMutex);
				auto it = this->KPData.find(aAccountName);

				if (it != this->KPData.end() && it->second != nullptr)
				{
					return;
				}
				else
				{
					kpmeInfo = new KPMEInfo_t();
					this->Players[aAccountName].KPMEInfo = kpmeInfo;
				}
			}

			HINTERNET hInternet = InternetOpen(
				"GW2/Raidcore/CommandersToolkit", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

			std::string result;

			if (hInternet)
			{
				HINTERNET hConnect = InternetOpenUrl(
					hInternet,
					("https://killproof.me/api/kp/" + aAccountName + "?lang=en").c_str(),
					NULL,
					0,
					INTERNET_FLAG_RELOAD,
					0
				);

				if (hConnect)
				{
					std::vector<char> buffer(4096);
					DWORD bytesRead;

					while (InternetReadFile(hConnect, buffer.data(), static_cast<DWORD>(buffer.size()), &bytesRead) && bytesRead > 0)
					{
						result.append(buffer.data(), bytesRead);
					}

					InternetCloseHandle(hConnect);
				}
				else
				{
					G::APIDefs->Log(ELogLevel_DEBUG, ADDON_NAME, "InternetOpenUrl failed.");
				}

				InternetCloseHandle(hInternet);
			}
			else
			{
				G::APIDefs->Log(ELogLevel_DEBUG, ADDON_NAME, "InternetOpen failed.");
			}

			try
			{
				json response = json::parse(result);

				if (!response.is_null() && !response["linked_totals"].is_null())
				{
					response = response["linked_totals"];
				}

				for (json title : response["titles"])
				{
					if (!title["id"].is_null())
					{
						switch (title["id"].get<int>())
						{
							case 230: kpmeInfo->Raids.W1_TheEternal = true; break;
							case 238: kpmeInfo->Raids.W2_SlipperySlubling = true; break;
							case 264: kpmeInfo->Raids.W4_Committed = true; break;
							case 262: kpmeInfo->Raids.W4_SilentSavior = true; break;
							case 269: kpmeInfo->Raids.W4_DemonsDemise = true; break;
							case 300: kpmeInfo->Raids.W5_VoiceInTheVoid = true; break;
							case 318: kpmeInfo->Raids.W6_ChampionOfZomorros = true; break;
							case 328: kpmeInfo->Raids.W7_EverlastingAllyOfAhdashim = true; break;
							case 549: kpmeInfo->Raids.W8_TheEternal = true; break;
							case 550: kpmeInfo->Raids.W8_RaidExpert = true; break;
							case 570: kpmeInfo->Raids.W8_Godsbane = true; break;
							case 574: kpmeInfo->Raids.W8_LegendaryConquerorOfUra = true; break;

							case 388: kpmeInfo->Strikes.EOD_PeerlessGeometer = true; break;
							case 389: kpmeInfo->Strikes.EOD_GazedIntoTheVoid = true; break;
							case 387: kpmeInfo->Strikes.EOD_TheGreatEqualizer = true; break;
							case 386: kpmeInfo->Strikes.EOD_TheVoidwalker = true; break;
							case 395: kpmeInfo->Strikes.EOD_DefenderOfLionsCourt = true; break;
							case 419: kpmeInfo->Strikes.EOD_MindFlayer = true; break;
							case 418: kpmeInfo->Strikes.EOD_EmbodimentOfSin = true; break;
							case 456: kpmeInfo->Strikes.EOD_LegendaryConquerorOfCerus = true; break;

							case 250: kpmeInfo->Fractals.TheUnclean = true; break;
							case 277: kpmeInfo->Fractals.LNHB = true; break;
							case 366: kpmeInfo->Fractals.DwD = true; break;
							case 365: kpmeInfo->Fractals.DoD = true; break;
							case 399: kpmeInfo->Fractals.NightmareAspect = true; break;
							case 526: kpmeInfo->Fractals.KryptisExorcist = true; break;
							case 299: kpmeInfo->Fractals.FractalSavant = true; break;
							case 297: kpmeInfo->Fractals.FractalProdigy = true; break;
							case 296: kpmeInfo->Fractals.FractalChampion = true; break;
							case 298: kpmeInfo->Fractals.FractalGod = true; break;
						}
					}
				}

				for (json kp : response["killproofs"])
				{
					if (!kp["id"].is_null())
					{
						switch (kp["id"].get<int>())
						{
							case 88485: kpmeInfo->Raids.LI += kp["amount"].get<int>(); break;
							case 77302: kpmeInfo->Raids.LI += kp["amount"].get<int>(); break;
							case 94020: kpmeInfo->Fractals.UFE = kp["amount"].get<int>(); break;
							case 81743: kpmeInfo->Fractals.UFE = kp["amount"].get<int>(); break;
							case 93781: kpmeInfo->Strikes.IBS_BoneskinnerVial = kp["amount"].get<int>(); break;
						}
					}
				}

				for (json token : response["tokens"])
				{
					if (!token["id"].is_null())
					{
						switch (token["id"].get<int>())
						{
							// Wing 1
							case 77705: kpmeInfo->Raids.W1_ValeGuardian += token["amount"].get<int>(); break;
							case 77751: kpmeInfo->Raids.W1_Gorseval += token["amount"].get<int>(); break;
							case 77728: kpmeInfo->Raids.W1_Sabetha += token["amount"].get<int>(); break;

							// Wing 2
							case 77706: kpmeInfo->Raids.W2_Slothasor += token["amount"].get<int>(); break;
							case 77679: kpmeInfo->Raids.W2_Matthias += token["amount"].get<int>(); break;

							// Wing 3
							case 78873: kpmeInfo->Raids.W3_Escort += token["amount"].get<int>(); break;
							case 78902: kpmeInfo->Raids.W3_KeepConstruct += token["amount"].get<int>(); break;
							case 78942: kpmeInfo->Raids.W3_Xera += token["amount"].get<int>(); break;

							// Wing 4
							case 80623: kpmeInfo->Raids.W4_Cairn += token["amount"].get<int>(); break;
							case 80269: kpmeInfo->Raids.W4_MursaatOverseer += token["amount"].get<int>(); break;
							case 80087: kpmeInfo->Raids.W4_Samarog += token["amount"].get<int>(); break;
							case 80542: kpmeInfo->Raids.W4_Deimos += token["amount"].get<int>(); break;

							// Wing 5
							case 85993: kpmeInfo->Raids.W5_SoullessHorror += token["amount"].get<int>(); break;
							case 85633: kpmeInfo->Raids.W5_Dhuum += token["amount"].get<int>(); break;

							// Wing 6
							case 88543: kpmeInfo->Raids.W6_ConjuredAmalgamate += token["amount"].get<int>(); break;
							case 88860: kpmeInfo->Raids.W6_LargosTwins += token["amount"].get<int>(); break;
							case 88645: kpmeInfo->Raids.W6_Qadim += token["amount"].get<int>(); break;

							// Wing 7
							case 91246: kpmeInfo->Raids.W7_CardinalAdina += token["amount"].get<int>(); break;
							case 91270: kpmeInfo->Raids.W7_CardinalSabir += token["amount"].get<int>(); break;
							case 91175: kpmeInfo->Raids.W7_QadimThePeerless += token["amount"].get<int>(); break;

							// Wing 8
							case 104047: kpmeInfo->Raids.W8_Greer += token["amount"].get<int>(); break;
							case 103754: kpmeInfo->Raids.W8_Decima += token["amount"].get<int>(); break;
							case 103996: kpmeInfo->Raids.W8_Ura += token["amount"].get<int>(); break;
						
							// AH
							case 95638: kpmeInfo->Strikes.EOD_AH += token["amount"].get<int>(); break;
							case 97269: kpmeInfo->Strikes.EOD_AHCM += token["amount"].get<int>(); break;

							// Ankka
							case 95982: kpmeInfo->Strikes.EOD_Ankka += token["amount"].get<int>(); break;
							case 96638: kpmeInfo->Strikes.EOD_AnkkaCM += token["amount"].get<int>(); break;

							// KO
							case 97451: kpmeInfo->Strikes.EOD_KO += token["amount"].get<int>(); break;
							case 96419: kpmeInfo->Strikes.EOD_KOCM += token["amount"].get<int>(); break;

							// HT
							case 97132: kpmeInfo->Strikes.EOD_HT += token["amount"].get<int>(); break;
							case 95986: kpmeInfo->Strikes.EOD_HTCM += token["amount"].get<int>(); break;

							// OLC
							case 99165: kpmeInfo->Strikes.EOD_OLC += token["amount"].get<int>(); break;
							case 99204: kpmeInfo->Strikes.EOD_OLCCM += token["amount"].get<int>(); break;
							
							// Dagda
							case 100068: kpmeInfo->Strikes.EOD_Dagda += token["amount"].get<int>(); break;
							case 101172: kpmeInfo->Strikes.EOD_DagdaCM += token["amount"].get<int>(); break;

							// Cerus
							case 100858: kpmeInfo->Strikes.EOD_Cerus += token["amount"].get<int>(); break;
							case 101542: kpmeInfo->Strikes.EOD_CerusCM += token["amount"].get<int>(); break;
						}
					}
				}

				for (json coffer : response["coffers"])
				{
					if (!coffer["id"].is_null())
					{
						switch (coffer["id"].get<int>())
						{
							// Wing 1
							case 91203: kpmeInfo->Raids.W1_ValeGuardian += coffer["amount"].get<int>() * 3; break;
							case 91215: kpmeInfo->Raids.W1_Gorseval += coffer["amount"].get<int>() * 3; break;
							case 91147: kpmeInfo->Raids.W1_Sabetha += coffer["amount"].get<int>() * 3; break;

							// Wing 2
							case 91160: kpmeInfo->Raids.W2_Slothasor += coffer["amount"].get<int>() * 3; break;
							case 91252: kpmeInfo->Raids.W2_Matthias += coffer["amount"].get<int>() * 3; break;

							// Wing 3
							case 91262: kpmeInfo->Raids.W3_Escort += coffer["amount"].get<int>() * 3; break;
							case 91187: kpmeInfo->Raids.W3_KeepConstruct += coffer["amount"].get<int>() * 3; break;
							case 91182: kpmeInfo->Raids.W3_Xera += coffer["amount"].get<int>() * 3; break;

							// Wing 4
							case 91186: kpmeInfo->Raids.W4_Cairn += coffer["amount"].get<int>() * 3; break;
							case 91191: kpmeInfo->Raids.W4_MursaatOverseer += coffer["amount"].get<int>() * 3; break;
							case 91267: kpmeInfo->Raids.W4_Samarog += coffer["amount"].get<int>() * 3; break;
							case 91233: kpmeInfo->Raids.W4_Deimos += coffer["amount"].get<int>() * 3; break;

							// Wing 5
							case 91211: kpmeInfo->Raids.W5_SoullessHorror += coffer["amount"].get<int>() * 3; break;
							case 91220: kpmeInfo->Raids.W5_Dhuum += coffer["amount"].get<int>() * 3; break;

							// Wing 6
							case 91157: kpmeInfo->Raids.W6_ConjuredAmalgamate += coffer["amount"].get<int>() * 3; break;
							case 91166: kpmeInfo->Raids.W6_LargosTwins += coffer["amount"].get<int>() * 3; break;
							case 91237: kpmeInfo->Raids.W6_Qadim += coffer["amount"].get<int>() * 3; break;

							// Wing 7
							case 91200: kpmeInfo->Raids.W7_CardinalAdina += coffer["amount"].get<int>() * 3; break;
							case 91241: kpmeInfo->Raids.W7_CardinalSabir += coffer["amount"].get<int>() * 3; break;
							case 91260: kpmeInfo->Raids.W7_QadimThePeerless += coffer["amount"].get<int>() * 3; break;

							// Wing 8
							case 103783: kpmeInfo->Raids.W8_Greer += coffer["amount"].get<int>(); break;
							case 104306: kpmeInfo->Raids.W8_Greer += coffer["amount"].get<int>(); break;
							case 104399: kpmeInfo->Raids.W8_GreerCM += coffer["amount"].get<int>(); break;
							case 103926: kpmeInfo->Raids.W8_Decima += coffer["amount"].get<int>(); break;
							case 104410: kpmeInfo->Raids.W8_Decima += coffer["amount"].get<int>(); break;
							case 104246: kpmeInfo->Raids.W8_DecimaCM += coffer["amount"].get<int>(); break;
							case 103946: kpmeInfo->Raids.W8_Ura += coffer["amount"].get<int>(); break;
							case 104439: kpmeInfo->Raids.W8_Ura += coffer["amount"].get<int>(); break;
							case 104355: kpmeInfo->Raids.W8_UraCM += coffer["amount"].get<int>(); break;
						}
					}
				}

				{
					const std::lock_guard<std::mutex> lock(this->KPMutex);
					this->KPData[aAccountName] = kpmeInfo;
				}
			}
			catch (json::parse_error& exc)
			{
				G::APIDefs->Log(ELogLevel_WARNING, ADDON_NAME, String::Format("KPME API: Json Parse error\n\t%s", exc.what()).c_str());
			}
		}, aPlayer.Member.AccountName).detach();
	}
}

void CSquadMgr::OnGroupMemberJoin(RTAPI::GroupMember* aGroupMember)
{
	const std::lock_guard<std::mutex> lock(this->Mutex);

	auto& player = this->Players[aGroupMember->AccountName];
	player.Member = *aGroupMember;
	player.HasLeft = 0; /* Reset in case the player rejoined without removal. */

	GetKPData(player);
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

	GetKPData(player);
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
