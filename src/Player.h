#pragma once
#include <Windows.h>
#include "Utility.h"

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
		bool IsTracked;
		time_t LastSeen;

		void Tooltip();
};