#pragma once
#include "nlohmann/json.hpp"
#include "arcdps.h"
#include <fstream>

namespace Settings
{
#define CONFIG_PATH "addons\\commanders_toolkit\\config.json"

	struct Configuration
	{
		int HotKey;
		bool UseNumericValues;
	};
	void to_json(nlohmann::json& j, const Configuration& c)
	{
		j = nlohmann::json{ {"HotKey", c.HotKey}, {"UseNumericValues", c.UseNumericValues} };
	}
	void from_json(const nlohmann::json& j, Configuration& c)
	{
		j.at("HotKey").get_to(c.HotKey);
		j.at("UseNumericValues").get_to(c.UseNumericValues);
	}

	Configuration Config;

	void Load()
	{
		try {
			std::ifstream configFile(CONFIG_PATH);
			if (configFile.is_open())
			{
				nlohmann::json j = nlohmann::json::parse(configFile);

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
			auto j = nlohmann::json(Config);

			std::ofstream configFile(CONFIG_PATH);
			
			configFile << j;
			configFile.close();
		}
		catch (...) {
			ArcDPS::Log((char*)"This is not good lol");
		}
	}
}