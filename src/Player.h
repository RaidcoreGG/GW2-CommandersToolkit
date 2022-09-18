#pragma once
#include <Windows.h>
#include "Utility.h"
#include <ctime>
#include "imgui_extensions.h"
#include <string>
#include "Templates.h"

class Player
{
	public:
		uintptr_t ID;
		char AccountName[64];
		char CharacterName[64];
		int Profession;
		int Subgroup;
		Utility Utilities;
		char Notes[64] = "";
		bool IsSelf;
		bool IsTracked;
		time_t LastSeen;

		void Tooltip()
		{
			ImGui::Tooltip([this]
				{
					ImGui::Text("%s", AccountName);
					if (!IsTracked)
					{
						int secondsSince = time(0) - LastSeen;

						ImGui::Text("Last seen %u %s ago",
							secondsSince < 60 ? secondsSince : secondsSince / 60,
							secondsSince < 60 ? "seconds" : "minutes");
					}
				});
		}

		bool Untracked()
		{
			if (!IsTracked && !IsSelf)
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