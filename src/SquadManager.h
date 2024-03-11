#pragma once
#include <Windows.h>
#include <vector>
#include <map>
#include <mutex>
#include "Player.h"
#include "Subgroup.h"

class SquadManager
{
public:
	/* UI */
	static bool Visible;

	/* globals */
	static std::map<std::string, Player> SquadMembers;
	static Subgroup Subgroups[15];
	static std::mutex SquadMembersMutex;
	// SelfAccountName populated and used by extras callbacks
	static std::string SelfAccountName;

	/* funcs */
	static uintptr_t DrawWindow(bool movable, bool clickable);
	static uintptr_t PurgeSquadMembers();
	static void MarkAllUntrackedButSelf();
	static Player* FindPlayerByID(uintptr_t id);
	static Player* FindPlayerByAccountName(const std::string &accountName);

	~SquadManager() = default;
private:
	SquadManager();
};