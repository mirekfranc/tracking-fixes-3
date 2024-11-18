# Copyright (C) 2024 SUSE LLC <mfranc@suse.cz>
# SPDX-License-Identifier: GPL-2.0-or-later

project('tracking-fixes-3', 'cpp',
  version: '1.0.0',
  default_options: ['buildtype=release', 'cpp_std=c++20', 'cpp_args=-Wall -Wextra -fPIE -g']
)

add_global_arguments('-DTF3_VERSION="' + meson.project_version() + '"', language: 'cpp')

sqlite3_dep = dependency('sqlite3', method : 'pkg-config', required: true)
libgit2_dep = dependency('libgit2', method : 'pkg-config', required: true)
libcurl_dep = dependency('libcurl', method : 'pkg-config', required: true)

inc_dir = include_directories('include')

src_files = files(
  'src/fixes3.cc'
)

executable('fixes3',
  src_files,
  include_directories: inc_dir,
  dependencies: [sqlite3_dep, libgit2_dep, libcurl_dep]
)
