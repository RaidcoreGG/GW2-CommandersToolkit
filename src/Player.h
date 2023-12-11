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

						ImGui::Text(u8"�W���J���O�b %u %s ���e",
							secondsSince < 60 ? secondsSince : secondsSince / 60,
							secondsSince < 60 ? u8"��" : u8"����");
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
				ImGui::Text(u8"�q�ҪO�����:");
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
				if (ImGui::BeginMenu(u8"��L¾�~"))
				{
					for (int p = 1; p <= 9; p++)
					{
						if (Profession == p) { continue; }
						std::string profession;
						switch (p)
						{
						case 1: profession = u8"�u�@��"; break;
						case 2: profession = u8"�Ԥh"; break;
						case 3: profession = u8"�u�{�v"; break;
						case 4: profession = u8"�C�L"; break;
						case 5: profession = u8"����"; break;
						case 6: profession = u8"������"; break;
						case 7: profession = u8"�۳N�v"; break;
						case 8: profession = u8"���F�v"; break;
						case 9: profession = u8"��Z��"; break;
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

				if (ImGui::MenuItem(u8"���]")) { Utilities = Utility(); }

				ImGui::EndPopup();
			}
		}
};