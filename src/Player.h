#include <string>

struct Utility
{
	bool Might = false;
	bool Alacrity = false;
	bool Quickness = false;
	bool Fury = false;
	bool Vulnerability = false;
	bool Heal = false;
};

class Player
{
	public:
		uintptr_t ID;
		char AccountName[33];
		char CharacterName[20];
		int Subgroup;
		Utility Utilities;
		char Notes[64] = "";
		bool IsTracked;
};