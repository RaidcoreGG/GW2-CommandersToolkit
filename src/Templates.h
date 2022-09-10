#pragma once
#include <vector>
#include "Utility.h"

struct Template
{
	Template(const char b[], Utility u) { strcpy_s(BuildName, b); Utilities = u; }

	char BuildName[65];
	Utility Utilities;
};

std::vector<Template> GetTemplates(int profession);