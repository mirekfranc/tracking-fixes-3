// Copyright (C) 2024 SUSE LLC <mfranc@suse.cz>
// SPDX-License-Identifier: GPL-2.0-or-later

#include <getopt.h>

#include "tracking-fixes-3/libgit2.h"
#include "tracking-fixes-3/libsqlite3.h"
#include "tracking-fixes-3/libcurl.h"

namespace {
	void parse_options(int argc, char **argv);
	struct gm {
		std::string maintainers;
		std::string kernel_tree;
		std::string kernel_source;
		std::string context_map;
		std::string branches_conf;
		std::string origin = "origin";
		bool refresh = false;
		bool trace = false;
	} gm;
}

int main(int argc, char **argv)
{
	TF3_BEGIN;

	parse_options(argc, argv);

	constexpr const char maintainers_url[] = "https://kerncvs.suse.de/MAINTAINERS";
	gm.maintainers = fetch_file_if_needed(gm.maintainers, "MAINTAINERS", maintainers_url, gm.trace, gm.refresh);

	constexpr const char branches_conf_url[] = "https://kerncvs.suse.de/branches.conf";
	gm.branches_conf = fetch_file_if_needed(gm.branches_conf, "branches.conf", branches_conf_url, gm.trace, gm.refresh);

	try_to_fetch_env(gm.kernel_tree, "LINUX_GIT");
	try_to_fetch_env(gm.kernel_tree, "KSOURCE_GIT");

	fail_with_message("unimplemented");

	TF3_END;
}

namespace {

	void usage(const char *prog, std::ostream &os)
	{
		os << prog << " (version: " TF3_VERSION ") For more information, read the man page.\n"
		   << "  --help, -h                    - Print this help message\n"
		   << "  --maintainers, -m <file>      - Custom path to the MAINTAINERS file instead of $HOME/.cache/tracking-fixes-3/MAINTAINERS\n"
		   << "  --kernel_tree, -k <dir>       - Clone of the mainline kernel repo ($LINUX_GIT)\n"
		   << "  --kernel_source, -s <dir>     - Clone of the SUSE kernel repo ($KSOURCE_GIT)\n"
		   << "  --context_map, -c <file>      - Custom path to the context-map\n"
		   << "  --origin, -o <remote>         - Use some other remote than origin (useful only for $LINUX_GIT)\n"
		   << "  --refresh, -r                 - Refresh config files\n"
		   << "  --trace, -t                   - Be a bit more verbose about how we got there on STDERR\n"
		   << "  --version, -V                 - Print just the version number\n";
	}

	struct option opts[] = {
		{ "help", no_argument, nullptr, 'h' },
		{ "maintainers", required_argument, nullptr, 'm' },
		{ "kernel_tree", required_argument, nullptr, 'k' },
		{ "kernel_source", required_argument, nullptr, 's' },
		{ "context_map", required_argument, nullptr, 'c' },
		{ "branches_conf", required_argument, nullptr, 'b' },
		{ "origin", required_argument, nullptr, 'o' },
		{ "refresh", no_argument, nullptr, 'r' },
		{ "trace", no_argument, nullptr, 't' },
		{ "version", no_argument, nullptr, 'V' },
		{ nullptr, 0, nullptr, 0 },
	};

	void parse_options(int argc, char **argv)
	{
		int c;
		std::string option;

		for (;;) {
			int opt_idx;

			c = getopt_long(argc, argv, "hm:k:s:c:b:o:rtV", opts, &opt_idx);
			if (c == -1)
				break;

			switch (c) {
			case 'h':
				usage(argv[0], std::cout);
				throw 0;
			case 'm':
				gm.maintainers = optarg;
				break;
			case 'k':
				gm.kernel_tree = optarg;
				break;
			case 's':
				gm.kernel_source = optarg;
				break;
			case 'c':
				gm.context_map = optarg;
				break;
			case 'b':
				gm.branches_conf = optarg;
				break;
			case 'o':
				gm.origin = optarg;
				break;
			case 'r':
				gm.refresh = true;
				break;
			case 't':
				gm.trace = true;
				break;
			case 'V':
				std::cout << TF3_VERSION << '\n';
				throw 0;
			default:
				usage(argv[0], std::cerr);
				throw 1;
			}
		}
	}
}
