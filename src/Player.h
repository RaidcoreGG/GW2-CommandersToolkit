#include <string>

struct Utility
{
	Utility() {}
	Utility(bool m, bool a, bool q, bool f, bool v, bool h) { Might = m; Alacrity = a; Quickness = q; Fury = f; Vulnerability = v; Heal = h; }

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
		int Profession;
		int Subgroup;
		Utility Utilities;
		char Notes[64] = "";
		bool IsTracked;
};

struct Template
{
	Template(const char b[], Utility u) { strcpy_s(BuildName, b); Utilities = u; }

	char BuildName[65];
	Utility Utilities;
};