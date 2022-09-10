#pragma once
#include <Windows.h>
#include <vector>
#include <mutex>
#include "Player.h"

class SquadManager
{
public:
	/* UI */
	static bool Focused;
	static bool Visible;

	/* globals */
	static std::vector<Player> SquadMembers;
	static std::mutex SquadMembersMutex;

	/* funcs */
	static uintptr_t DrawWindow();

	~SquadManager() = default;
private:
	SquadManager();
};