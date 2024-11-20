// Copyright (C) 2024 SUSE LLC <mfranc@suse.cz>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TF3_HELPERS_H
#define TF3_HELPERS_H

#include <cstdlib>
#include <string>
#include <string_view>
#include <ranges>
#include <vector>
#include <iostream>

#define TF3_BEGIN try {
#define TF3_END } catch (int ret) { return ret; } catch (...) { return 42; }

namespace {
	template<typename... Args> void fail_with_message(Args&&... args)
	{
		(std::cerr << ... << args) << std::endl;
		throw 1;
	}

	template<typename... Args> void emit_message(Args&&... args)
	{
		(std::cerr << ... << args) << std::endl;
	}

	void try_to_fetch_env(std::string &var, const std::string &name)
	{
		if (var.empty()) {
			const char *ptr = std::getenv(name.c_str());
			if (ptr)
				var = ptr;
		}
	}

	std::string strip(std::string s)
	{
		std::erase_if(s, isspace);
		return s;
	}

	void split_string(std::vector<std::string> &list, const std::string &s, char delimiter)
	{
		for (auto w: std::views::split(s, delimiter))
			list.push_back(strip(std::string(w.begin(), w.end())));
	}
}

#endif
