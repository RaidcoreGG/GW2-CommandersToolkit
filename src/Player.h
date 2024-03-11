#pragma once
#include <Windows.h>
#include "Utility.h"
#include <ctime>
#include "imgui_extensions.h"
#include <string>
#include "Templates.h"

enum class AddedBy
{
	Arcdps,
	Extras,
};

/*
 * Possible state transitions:
 * - Agent added by arc, (agent updated by arc), agent removed by arc
 *   - arcdps only state transition
 * - Agent added by extras, agent added by arc, (agent updated by arc/extras), (agent removed by arc), (agent added by arc) agent removed by extras, agent removed by arc
 *   - normal arcdps+extras transition
 * - Agent added by arc, (agent updated by arc/extras), agent removed by extras, agent removed by arc
 *   - arcdps+extras transition for players that were already in the squad when game started up - limitations of extras hooking at character select and missing the initial squad update
 *
 * Arcdps updates should always come after extras updates due to live API delays
 */
class Player
{
	public:
		uintptr_t ID{};
		std::string AccountName;
		std::string CharacterName;
		int Profession{};
		int Subgroup{};
		Utility Utilities;
		char Notes[64] = "";
		bool IsSelf{};
		bool IsTrackedByArcdps{};
		bool IsTrackedByExtras{};
		time_t LastSeen{};
		AddedBy AddedBy;

		void Tooltip()
		{
			ImGui::Tooltip([this]
				{
					ImGui::Text("%s", AccountName.c_str());
					if (!IsTrackedByArcdps)
					{
						int secondsSince = time(0) - LastSeen;

						ImGui::Text("Last seen %u %s ago",
							secondsSince < 60 ? secondsSince : secondsSince / 60,
							secondsSince < 60 ? "seconds" : "minutes");
					}
#ifdef _DEBUG
					ImGui::Text("prof: %d, sub: %d, self: %d, arc: %d, extras: %d, added: %d", Profession, Subgroup, IsSelf, IsTrackedByArcdps, IsTrackedByExtras, AddedBy);
#endif
				});
		}

		bool Untracked()
		{
			if (!IsTrackedByArcdps && !IsTrackedByExtras && !IsSelf)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(172, 89, 89, 255));
				return true;
			}

			return false;
		}

		void ContextMenu()
		{
			if (ImGui::BeginPopupContextItem(("##PlayerCtx" + std::to_string(ID)).c_str()))
			{
				ImGui::Text("Apply from template:");
				ImGui::Separator();
				std::vector<Template> templates = GetTemplates(Profession);
				for (size_t t = 0; t < templates.size(); t++)
				{
					ImGui::Bullet(); ImGui::SameLine();
					if (ImGui::MenuItem(templates[t].BuildName))
					{
						Utilities = templates[t].Utilities;
					}
				}
				if (ImGui::BeginMenu("Other professions"))
				{
					for (int p = 1; p <= 9; p++)
					{
						if (Profession == p) { continue; }
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
									Utilities = otherTemplates[t].Utilities;
								}
							}
							ImGui::EndMenu();
						}
					}
					ImGui::EndMenu();
				}

				ImGui::Separator();

				if (ImGui::MenuItem("Reset")) { Utilities = Utility(); }

				ImGui::EndPopup();
			}
		}
};