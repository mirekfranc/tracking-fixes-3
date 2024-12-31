#ifndef TF3_CONFIG_H
#define TF3_CONFIG_H

#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>

#include "helpers.h"

namespace {
	struct Config
	{
		std::vector<std::string> ignored_branches;
		std::unordered_map<std::string, std::vector<std::string>> stable_branches;

		Config() = default;

		Config(const Config&) = delete;
		Config& operator=(const Config&) = delete;

		Config(Config&&) = default;
		Config& operator=(Config&&) = default;
	};

	void load_config(Config &config, const std::string &filename)
	{
		std::ifstream file{filename};

		if (!file.is_open())
			fail_with_message("Unable to open fixes3.ini file: ", filename);

		bool general = false, stable_branches = false;
		for (std::string line; getline(file, line);) {
			if (line.empty() || line[0] == ';' || line[0] == ' ') {
				general = stable_branches = false;
				continue;
			}
			if (line.starts_with("[tracking-fixes-3]")) {
				general = true;
				stable_branches = false;
				continue;
			}
			if (line.starts_with("[stable-branches]")) {
				stable_branches = true;
				general = false;
				continue;
			}

			const auto equal_sign_idx = line.find_first_of("=");
			if (equal_sign_idx == std::string::npos) {
				emit_message("tracking3.ini: ", line);
				continue;
			}
			const std::string key = strip(line.substr(0, equal_sign_idx));
			const std::string list = strip(line.substr(equal_sign_idx + 1));
			if (key.empty() || list.empty()) {
				emit_message("tracking3.ini: ", line);
				continue;
			}

			if (general) {
				if (key == "ignored-branches")
					split_string(config.ignored_branches, list, ',');
			} else if (stable_branches) {
				std::vector<std::string> branches;
				split_string(branches, list, ',');
				config.stable_branches.insert(std::make_pair(key, std::move(branches)));
				continue;
			}
		}
	}
}

#endif
