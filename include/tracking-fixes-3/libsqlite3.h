// Copyright (C) 2024 SUSE LLC <mfranc@suse.cz>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TF3_SQLITE3_H
#define TF3_SQLITE3_H

#include <sqlite3.h>
#include "helpers.h"

namespace {
	struct Database
	{
		Database() : database(nullptr) {}
		~Database() { sqlite3_close(database); }

		Database(const Database&) = delete;
		Database& operator=(const Database&) = delete;

		Database(Database&& o) noexcept : database(o.database) {
			o.database = nullptr;
		}

		Database& operator=(Database&& o) noexcept {
			if (this != &o) {
				sqlite3_close(database);
				database = o.database;
				o.database = nullptr;
			}
			return *this;
		}

		int from_path(const std::string &db_path) { return sqlite3_open(db_path.c_str(), &database); }
		int exec(const std::string &sql) {
			char *err = nullptr;
			int ret = sqlite3_exec(database, sql.c_str(), nullptr, nullptr, &err);
			emit_message(err);
			sqlite3_free(err);
			return ret;
		}
		sqlite3 *database;
	};
}
#endif
