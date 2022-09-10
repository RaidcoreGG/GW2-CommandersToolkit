#include "Settings.h"
#include <iostream>
#include <fstream>

namespace Settings
{
	void Load()
	{
		try {
			std::ifstream configFile(CONFIG_PATH);
			if (configFile.is_open())
			{
				json j = json::parse(configFile);

				Config = j.get<Configuration>();
				configFile.close();
			}
		}
		catch (...) {
			ArcDPS::Log((char*)"This is not good lol");
		}
	}

	void Save()
	{
		try {
			json j = Config;

			std::ofstream configFile(CONFIG_PATH);
			if (configFile.is_open())
			{
				configFile << j;
				configFile.close();
			}
		}
		catch (...) {
			ArcDPS::Log((char*)"This is not good lol");
		}
	}
}