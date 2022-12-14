#pragma once
#include "Templates.h"

std::vector<Template> GetTemplates(int profession)
{
	/* ideally you don't want to hardcode builds in an ever changing game, but I cba right now */
	std::vector<Template> templates;

	//templates.push_back(Template("NAME", Utility(m, a, q, f, v, h, p, c)));

	switch (profession)
	{
	case 1: // Guardian
		templates.push_back(Template("Condition Quickness Firebrand", Utility(true, false, true, true, false, false, false, true)));
		templates.push_back(Template("Heal Quickness Firebrand", Utility(true, false, true, true, false, true, false, false)));
		break;

	case 2: // Warrior
		templates.push_back(Template("Condition Quickness Berserker", Utility(false, false, true, true, false, false, false, true)));
		templates.push_back(Template("Power Quickness Bladesworn", Utility(true, false, true, true, false, false, true, false)));
		break;

	case 3: // Engineer
		templates.push_back(Template("Power Scrapper", Utility(false, false, false, false, true, false, true, false)));
		templates.push_back(Template("Power Quickness Scrapper", Utility(false, false, true, false, true, false, true, false)));
		templates.push_back(Template("Heal Quickness Scrapper", Utility(true, false, true, true, false, true, false, false)));
		templates.push_back(Template("Power Holosmith", Utility(false, false, false, false, true, false, true, false)));
		templates.push_back(Template("Hand Kite Mechanist", Utility(true, true, false, false, false, false, true, false)));
		templates.push_back(Template("Power Alacrity Mechanist", Utility(true, true, false, true, false, false, true, false)));
		templates.push_back(Template("Heal Alacrity Mechanist", Utility(true, true, false, true, false, true, false, false)));
		break;

	case 4: // Ranger
		templates.push_back(Template("Heal Alacrity Druid", Utility(true, true, false, true, false, true, false, false)));
		templates.push_back(Template("Condition Alacrity Untamed", Utility(false, true, false, false, false, false, false, true)));
		break;

	case 5: // Thief
		templates.push_back(Template("Condition Boon Daredevil", Utility(true, false, true, true, false, false, false, true)));
		templates.push_back(Template("Condition Alacrity Specter", Utility(false, true, false, false, false, false, false, true)));
		break;

	case 6: // Elementalist
		templates.push_back(Template("Heal Alacrity Tempest", Utility(true, true, false, true, false, true, false, false)));
		templates.push_back(Template("Power Catalyst", Utility(true, false, false, false, false, false, true, false)));
		break;

	case 7: // Mesmer
		templates.push_back(Template("Power Quickness Chronomancer", Utility(false, false, true, false, false, false, true, false)));
		templates.push_back(Template("Condition Quickness Chronomancer", Utility(false, false, true, false, false, false, false, true)));
		templates.push_back(Template("Condition Alacrity Mirage", Utility(true, true, false, false, false, false, false, true)));
		templates.push_back(Template("Power Virtuoso", Utility(false, false, false, true, true, false, true, false)));
		break;

	case 8: // Necromancer
		templates.push_back(Template("Condition Harbinger", Utility(false, false, false, false, true, false, false, true)));
		templates.push_back(Template("Condition Quickness Harbinger", Utility(true, false, true, true, true, false, false, true)));
		templates.push_back(Template("Power Quickness Harbinger", Utility(false, false, true, true, false, false, true, false)));
		break;

	case 9: // Revenant
		templates.push_back(Template("Power Herald", Utility(true, false, false, true, false, false, true, false)));
		templates.push_back(Template("Power Quickness Herald", Utility(true, false, true, true, false, false, true, false)));
		templates.push_back(Template("Condition Renegade", Utility(false, true, false, false, true, false, false, true)));
		break;
	}

	return templates;
}