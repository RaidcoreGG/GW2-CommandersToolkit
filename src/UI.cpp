///----------------------------------------------------------------------------------------------------
/// Copyright (c) Raidcore.GG - All rights reserved.
/// 
/// Name         :  UI.cpp
/// Description  :  User interface implementation.
/// Authors      :  K. Bieniek
///----------------------------------------------------------------------------------------------------

#include "UI.h"

#include "imgui/imgui.h"

namespace UI
{
	static CSquadMgr  s_SquadMgr;
	static CPullTimer s_PullTimer;

	void Render()
	{
		s_SquadMgr.Render();
		s_PullTimer.Render();
	}

	void RenderShortcutContextMenu()
	{
		if (ImGui::Button(s_SquadMgr.IsVisible() ? "Hide Squad Manager" : "Show Squad Manager"))
		{
			s_SquadMgr.ToggleVisible();
		}

		if (ImGui::Button(s_PullTimer.IsActive() ? "Cancel Pull Countdown" : "Start Pull Countdown"))
		{
			s_PullTimer.Toggle();
			// start pull countdown
			// -> sent timestamp, commander (starter) name and IP address of instance
			// if commander not matching or instance not matching -> discard
		}
	}

	CSquadMgr* GetSquadMgr()
	{
		return &s_SquadMgr;
	}

	CPullTimer* GetPullTimer()
	{
		return &s_PullTimer;
	}
}
