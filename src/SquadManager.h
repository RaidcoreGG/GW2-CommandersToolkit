#pragma once
#include <Windows.h>
#include <vector>
#include <mutex>
#include "Player.h"
#include "Subgroup.h"

class SquadManager
{
public:
	/* UI */
	static bool Visible;

	/* globals */
	static std::vector<Player> SquadMembers;
	static Subgroup Subgroups[15];
	static std::mutex SquadMembersMutex;

	/* funcs */
	static uintptr_t DrawWindow(bool movable, bool clickable);
	static uintptr_t PurgeSquadMembers();

	~SquadManager() = default;
private:
	SquadManager();
};