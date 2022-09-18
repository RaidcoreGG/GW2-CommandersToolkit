#pragma once
#include <ctime>
#include "Player.h"
#include "imgui_extensions.h"

void Player::Tooltip()
{
	ImGui::Tooltip([this]
	{
		ImGui::Text("%s", Player::AccountName);
		if (!Player::IsTracked)
		{
			int secondsSince = time(0) - Player::LastSeen;

			ImGui::Text("Last seen %u %s ago",
				secondsSince < 60 ? secondsSince : secondsSince / 60,
				secondsSince < 60 ? "seconds" : "minutes");
		}
	});
}