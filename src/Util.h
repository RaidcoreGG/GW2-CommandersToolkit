///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  Util.h
/// Description  :  Utility functions
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>

#include "imgui/imgui.h"
#include "nexus/Nexus.h"
#include "Globals.h"


inline void PlayerLeftTooltip(const bool& aActive, const long long& aSeconds)
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


inline void RenderIcon(float aSize, Texture** aTexture, std::string aIdentifer, std::string aAltText, uint32_t aResourceID)
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

inline bool RenderIconButton(float aSize, Texture** aTexture, std::string aIdentifer, std::string aAltText, uint32_t aResourceID, ImVec4 aTint = ImVec4(1, 1, 1, 1))
{
	bool pressed = false;
	if (*aTexture)
	{
		ImGui::PushID(aAltText.c_str());
		if (ImGui::ImageButton((*aTexture)->Resource, ImVec2(aSize, aSize), ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0, 0, 0, 0), aTint))
		{
			pressed = true;
		}
		ImGui::PopID();
	}
	else
	{
		*aTexture = G::APIDefs->Textures.GetOrCreateFromResource(aIdentifer.c_str(), aResourceID, G::Module);
		if (ImGui::Button(aAltText.c_str()))
		{
			pressed = true;
		}
	}

	return pressed;
}

inline void RenderSpecIcon(float aSize, int aProfession, int aSpec)
{
	struct IconEntry
	{
		ETextures textureEnum;
		const char* identifier;
		const char* altText;
		uint32_t resourceID;
	};

	IconEntry entry = { ETextures::SpecUnknown, "TEX_SPEC_UNKNOWN", "Unknown", IDB_SPEC_UNNKOWN };

	// Mapping helpers
	switch (aProfession)
	{
		case 1: // Guardian
			if (aSpec == -1) entry = { ETextures::SpecGrd, "TEX_SPEC_GRD", "Guardian", IDB_SPEC_GRD };
			else if (aSpec == 13) entry = { ETextures::SpecGrdDgh, "TEX_SPEC_GRD_DGH", "Dragonhunter", IDB_SPEC_GRD_DGH };
			else if (aSpec == 42) entry = { ETextures::SpecGrdWbd, "TEX_SPEC_GRD_WBD", "Willbender", IDB_SPEC_GRD_WBD };
			else if (aSpec == 62) entry = { ETextures::SpecGrdFbd, "TEX_SPEC_GRD_FBD", "Firebrand", IDB_SPEC_GRD_FBD };
			break;
		case 2: // Warrior
			if (aSpec == -1) entry = { ETextures::SpecWar, "TEX_SPEC_WAR", "Warrior", IDB_SPEC_WAR };
			else if (aSpec == 18) entry = { ETextures::SpecWarBrs, "TEX_SPEC_WAR_BRS", "Berserker", IDB_SPEC_WAR_BRS };
			else if (aSpec == 61) entry = { ETextures::SpecWarSpb, "TEX_SPEC_WAR_SPB", "Spellbreaker", IDB_SPEC_WAR_SPB };
			else if (aSpec == 68) entry = { ETextures::SpecWarBsw, "TEX_SPEC_WAR_BSW", "Bladesworn", IDB_SPEC_WAR_BSW };
			break;
		case 3: // Engineer
			if (aSpec == -1) entry = { ETextures::SpecEng, "TEX_SPEC_ENG", "Engineer", IDB_SPEC_ENG };
			else if (aSpec == 43) entry = { ETextures::SpecEngScr, "TEX_SPEC_ENG_SCR", "Scrapper", IDB_SPEC_ENG_SCR };
			else if (aSpec == 57) entry = { ETextures::SpecEngHls, "TEX_SPEC_ENG_HLS", "Holosmith", IDB_SPEC_ENG_HLS };
			else if (aSpec == 70) entry = { ETextures::SpecEngMch, "TEX_SPEC_ENG_MCH", "Mechanist", IDB_SPEC_ENG_MCH };
			break;
		case 4: // Ranger
			if (aSpec == -1) entry = { ETextures::SpecRgr, "TEX_SPEC_RGR", "Ranger", IDB_SPEC_RGR };
			else if (aSpec == 55) entry = { ETextures::SpecRgrSlb, "TEX_SPEC_RGR_SLB", "Soulbeast", IDB_SPEC_RGR_SLB };
			else if (aSpec == 72) entry = { ETextures::SpecRgrUtd, "TEX_SPEC_RGR_UTD", "Untamed", IDB_SPEC_RGR_UTD };
			else if (aSpec == 25) entry = { ETextures::SpecRgrDrd, "TEX_SPEC_RGR_DRD", "Druid", IDB_SPEC_RGR_DRD };
			break;
		case 5: // Thief
			if (aSpec == -1) entry = { ETextures::SpecThf, "TEX_SPEC_THF", "Thief", IDB_SPEC_THF };
			else if (aSpec == 35) entry = { ETextures::SpecThfDde, "TEX_SPEC_THF_DDE", "Daredevil", IDB_SPEC_THF_DDE };
			else if (aSpec == 58) entry = { ETextures::SpecThfDdl, "TEX_SPEC_THF_DDL", "Deadeye", IDB_SPEC_THF_DDL };
			else if (aSpec == 71) entry = { ETextures::SpecThfSpc, "TEX_SPEC_THF_SPC", "Specter", IDB_SPEC_THF_SPC };
			break;
		case 6: // Elementalist
			if (aSpec == -1) entry = { ETextures::SpecEle, "TEX_SPEC_ELE", "Elementalist", IDB_SPEC_ELE };
			else if (aSpec == 48) entry = { ETextures::SpecEleTmp, "TEX_SPEC_ELE_TMP", "Tempest", IDB_SPEC_ELE_TMP };
			else if (aSpec == 56) entry = { ETextures::SpecEleWvr, "TEX_SPEC_ELE_WVR", "Weaver", IDB_SPEC_ELE_WVR };
			else if (aSpec == 67) entry = { ETextures::SpecEleCat, "TEX_SPEC_ELE_CAT", "Catalyst", IDB_SPEC_ELE_CAT };
			break;
		case 7: // Mesmer
			if (aSpec == -1) entry = { ETextures::SpecMes, "TEX_SPEC_MES", "Mesmer", IDB_SPEC_MES };
			else if (aSpec == 40) entry = { ETextures::SpecMesChr, "TEX_SPEC_MES_CHR", "Chronomancer", IDB_SPEC_MES_CHR };
			else if (aSpec == 59) entry = { ETextures::SpecMesMir, "TEX_SPEC_MES_MIR", "Mirage", IDB_SPEC_MES_MIR };
			else if (aSpec == 66) entry = { ETextures::SpecMesVrt, "TEX_SPEC_MES_VRT", "Virtuoso", IDB_SPEC_MES_VRT };
			break;
		case 8: // Necromancer
			if (aSpec == -1) entry = { ETextures::SpecNec, "TEX_SPEC_NEC", "Necromancer", IDB_SPEC_NEC };
			else if (aSpec == 34) entry = { ETextures::SpecNecRpr, "TEX_SPEC_NEC_RPR", "Reaper", IDB_SPEC_NEC_RPR };
			else if (aSpec == 60) entry = { ETextures::SpecNecScg, "TEX_SPEC_NEC_SCG", "Scourge", IDB_SPEC_NEC_SCG };
			else if (aSpec == 64) entry = { ETextures::SpecNecHrb, "TEX_SPEC_NEC_HRB", "Harbinger", IDB_SPEC_NEC_HRB };
			break;
		case 9: // Revenant
			if (aSpec == -1) entry = { ETextures::SpecRev, "TEX_SPEC_REV", "Revenant", IDB_SPEC_REV };
			else if (aSpec == 52) entry = { ETextures::SpecRevHer, "TEX_SPEC_REV_HER", "Herald", IDB_SPEC_REV_HER };
			else if (aSpec == 63) entry = { ETextures::SpecRevRen, "TEX_SPEC_REV_REN", "Renegade", IDB_SPEC_REV_REN };
			else if (aSpec == 69) entry = { ETextures::SpecRevVin, "TEX_SPEC_REV_VIN", "Vindicator", IDB_SPEC_REV_VIN };
			break;
	}

	Texture** texPtr = &G::Textures[entry.textureEnum];
	if (*texPtr)
	{
		ImGui::Image((*texPtr)->Resource, ImVec2(aSize, aSize));
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::Text(entry.altText);
			ImGui::EndTooltip();
		}
	}
	else
	{
		*texPtr = G::APIDefs->Textures.GetOrCreateFromResource(entry.identifier, entry.resourceID, G::Module);
		ImGui::Text(entry.altText);
	}
}

#endif
