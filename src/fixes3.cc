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
	} gm;
}

int main(int argc, char **argv)
{
	TF3_BEGIN;

	parse_options(argc, argv);

	fail_with_message("unimplemented");

	TF3_END;
}

namespace {

	void usage(const char *prog, std::ostream &os)
	{
		os << prog << " For more information, read the man page.\n"
		<< "  --help, -h                    - Print this help message\n"
		<< "  --maintainers, -m <file>      - Custom path to the MAINTAINERS file instead of $HOME/.cache/tracking-fixes-3/MAINTAINERS\n"
		<< "  --kernel_tree, -k <dir>       - Clone of the mainline kernel repo ($LINUX_GIT)\n"
		<< "  --kernel_source, -s <dir>     - Clone of the SUSE kernel repo ($KSOURCE_GIT)\n"
		<< "  --context_map, -c <file>      - Custom path to the context-map\n"
		<< "  --origin, -o <remote>         - Use some other remote than origin (useful only for $LINUX_GIT)\n";
	}

	struct option opts[] = {
		{ "help", no_argument, nullptr, 'h' },
		{ "maintainers", required_argument, nullptr, 'm' },
		{ "kernel_tree", required_argument, nullptr, 'k' },
		{ "kernel_source", required_argument, nullptr, 's' },
		{ "context_map", required_argument, nullptr, 'c' },
		{ "branches_conf", required_argument, nullptr, 'b' },
		{ "origin", required_argument, nullptr, 'o' },
		{ nullptr, 0, nullptr, 0 },
	};

	void parse_options(int argc, char **argv)
	{
		int c;
		std::string option;

		for (;;) {
			int opt_idx;

			c = getopt_long(argc, argv, "hm:k:s:c:b:o:", opts, &opt_idx);
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
			default:
				usage(argv[0], std::cerr);
				throw 1;
			}
		}
	}
}
