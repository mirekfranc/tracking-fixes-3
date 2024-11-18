// Copyright (C) 2024 SUSE LLC <mfranc@suse.cz>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TF3_GIT2_H
#define TF3_GIT2_H

#include <git2.h>
#include "helpers.h"

namespace {
	struct LibGit2
	{
		LibGit2() { git_libgit2_init(); }
		~LibGit2() { git_libgit2_shutdown(); }

		LibGit2(const LibGit2&) = delete;
		LibGit2& operator=(const LibGit2&) = delete;
	};

	struct Repo
	{
		Repo() : repo(nullptr) {}
		~Repo() { git_repository_free(repo); }

		Repo(const Repo&) = delete;
		Repo& operator=(const Repo&) = delete;

		Repo(Repo&& o) noexcept : repo(o.repo) {
			o.repo = nullptr;
		}

		Repo& operator=(Repo&& o) noexcept {
			if (this != &o) {
				git_repository_free(repo);
				repo = o.repo;
				o.repo = nullptr;
			}
			return *this;
		}

		int from_path(const std::string &repo_path) { return git_repository_open(&repo, repo_path.c_str()); }
		git_repository *repo;
	};

	struct Object
	{
		Object() : object(nullptr) {}
		~Object() { git_object_free(object); }

		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;

		Object(Object&& o) noexcept : object(o.object) {
			o.object = nullptr;
		}

		Object& operator=(Object&& o) noexcept {
			if (this != &o) {
				git_object_free(object);
				object = o.object;
				o.object = nullptr;
			}
			return *this;
		}

		int from_rev(Repo &r, const std::string &rev) { return git_revparse_single(&object, r.repo, rev.c_str()); }
		git_object *object;
	};

	struct Commit
	{
		Commit() : commit(nullptr) {}
		~Commit() { git_commit_free(commit); }

		Commit(const Commit&) = delete;
		Commit& operator=(const Commit&) = delete;

		Commit(Commit&& o) noexcept : commit(o.commit) {
			o.commit = nullptr;
		}

		Commit& operator=(Commit&& o) noexcept {
			if (this != &o) {
				git_commit_free(commit);
				commit = o.commit;
				o.commit = nullptr;
			}
			return *this;
		}

		int from_oid(Repo &r, const git_oid *oid) { return git_commit_lookup(&commit, r.repo, oid); }
		int from_parent(const Commit &c, unsigned int parent_number = 0) { return git_commit_parent(&commit, c.commit, parent_number); }
		unsigned int parent_count() const { return git_commit_parentcount(commit); }
		git_commit *commit;
	};

	struct Tree
	{
		Tree() : tree(nullptr) {}
		~Tree() { git_tree_free(tree); }

		Tree(const Tree&) = delete;
		Tree& operator=(const Tree&) = delete;

		Tree(Tree&& o) noexcept : tree(o.tree) {
			o.tree = nullptr;
		}

		Tree& operator=(Tree&& o) noexcept {
			if (this != &o) {
				git_tree_free(tree);
				tree = o.tree;
				o.tree = nullptr;
			}
			return *this;
		}

		int from_commit(const Commit &c) { return git_commit_tree(&tree, c.commit); }
		int from_tree_entry(const Repo &r, const git_tree_entry *e) {return git_tree_lookup(&tree, r.repo, git_tree_entry_id(e)); }
		std::size_t entry_count() const { return git_tree_entrycount(tree); }
		git_tree *tree;
	};
}

#endif
