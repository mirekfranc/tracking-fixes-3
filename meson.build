# Copyright (C) 2024 SUSE LLC <mfranc@suse.cz>
# SPDX-License-Identifier: GPL-2.0-or-later

project('tracking-fixes-3', 'cpp',
  version: '1.0.0',
  default_options: ['cpp_std=c++20']
)

sqlite3_dep = dependency('sqlite3', required: true)
libgit2_dep = dependency('libgit2', required: true)
libcurl_dep = dependency('libcurl', required: true)

inc_dir = include_directories('include')

src_files = files(
  'src/fixes3.cc'
)

executable('fixes3',
  src_files,
  include_directories: inc_dir,
  dependencies: [sqlite3_dep, libgit2_dep, libcurl_dep]
)
