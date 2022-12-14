#pragma once

struct Utility
{
	bool Might = false;
	bool Alacrity = false;
	bool Quickness = false;
	bool Fury = false;
	bool Vulnerability = false;
	bool Heal = false;
	bool Pow = false;
	bool Condi = false;

	Utility() {}
	Utility(bool m, bool a, bool q, bool f, bool v, bool h, bool p, bool c) { Might = m; Alacrity = a; Quickness = q; Fury = f; Vulnerability = v; Heal = h; Pow = p; Condi = c; }
};