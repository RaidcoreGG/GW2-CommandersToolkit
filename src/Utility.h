#pragma once

struct Utility
{
	bool Might = false;
	bool Alacrity = false;
	bool Quickness = false;
	bool Fury = false;
	bool Vulnerability = false;
	bool Heal = false;

	Utility() {}
	Utility(bool m, bool a, bool q, bool f, bool v, bool h) { Might = m; Alacrity = a; Quickness = q; Fury = f; Vulnerability = v; Heal = h; }
};