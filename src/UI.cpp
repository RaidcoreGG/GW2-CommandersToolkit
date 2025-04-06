///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  UI.cpp
/// Description  :  User interface implementation.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "UI.h"

#include "imgui/imgui.h"
#include "SquadMgr/SquadMgr.h"

namespace UI
{
	void RenderShortcutContextMenu()
	{
		if (ImGui::Button(SquadMgr::IsVisible() ? "Hide Squad Manager" : "Show Squad Manager"))
		{
			SquadMgr::ToggleVisible();
		}
	}
}
