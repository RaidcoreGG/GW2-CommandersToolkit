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

#endif
