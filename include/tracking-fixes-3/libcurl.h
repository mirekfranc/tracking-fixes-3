// Copyright (C) 2024 SUSE LLC <mfranc@suse.cz>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef TF3_CURL_H
#define TF3_CURL_H

#include <ctime>
#include <sys/stat.h>
#include <curl/curl.h>

#include "helpers.h"

namespace
{
	struct LibCurl
	{
		LibCurl()
			{
				if (curl_global_init(CURL_GLOBAL_ALL))
					fail_with_message("Failed to initilize libcurl!");
			}
		~LibCurl() { curl_global_cleanup(); }

		LibCurl(const LibCurl&) = delete;
		LibCurl& operator=(const LibCurl&) = delete;
	};

	struct CurlHandle
	{
		CurlHandle()
			{
				if ((curl_handle = curl_easy_init()) == nullptr)
					fail_with_message("Failed to get curl_handle!");
				curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
				curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, ::fwrite);
				curl_easy_setopt(curl_handle, CURLOPT_FAILONERROR, 1L);
			}
		~CurlHandle() { curl_easy_cleanup(curl_handle); }

		CurlHandle(const CurlHandle&) = delete;
		CurlHandle& operator=(const CurlHandle&) = delete;

		CurlHandle(CurlHandle&& o) noexcept : curl_handle(o.curl_handle) {
			o.curl_handle = nullptr;
		}

		CurlHandle& operator=(CurlHandle&& o) noexcept {
			if (this != &o) {
				curl_easy_cleanup(curl_handle);
				curl_handle = o.curl_handle;
				o.curl_handle = nullptr;
			}
			return *this;
		}

		void set_url(const std::string &url) { curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str()); }
		CURL *curl_handle;
	};

	struct PageFile
	{
		PageFile(const std::string &path)
			{
				page_file = std::fopen(path.c_str(), "wb");
			}
		~PageFile() { std::fclose(page_file); }

		PageFile(const PageFile&) = delete;
		PageFile& operator=(const PageFile&) = delete;

		PageFile(PageFile&& o) noexcept : page_file(o.page_file) {
			o.page_file = nullptr;
		}

		PageFile& operator=(PageFile&& o) noexcept {
			if (this != &o) {
				std::fclose(page_file);
				page_file = o.page_file;
				o.page_file = nullptr;
			}
			return *this;
		}

		operator bool() const { return page_file != nullptr; }
		FILE* page_file;
	};

	std::string fetch_file_if_needed(std::string maintainers_path, const std::string &name, const std::string &url, bool trace, bool refresh)
	{
		if (!maintainers_path.empty())
			return maintainers_path;

		const char *xdg_cache_dir = std::getenv("XDG_CACHE_HOME");
		if(xdg_cache_dir)
			maintainers_path = xdg_cache_dir;
		else {
			const char *home_dir = std::getenv("HOME");
			if(home_dir)
				maintainers_path = home_dir;
			else
				fail_with_message("Unable to open HOME directory!");
			maintainers_path += "/.cache";
		}
		struct stat sb;
		if (stat(maintainers_path.c_str(), &sb) == -1)
			if(mkdir(maintainers_path.c_str(), 0755))
				fail_with_message("Unable to create .cache directory!");
		maintainers_path += "/tracking-fixes-3";
		if (stat(maintainers_path.c_str(), &sb) == -1)
			if (mkdir(maintainers_path.c_str(), 0755))
				fail_with_message("Unable to locate .cache/tracking-fixes-3 directory!");

		maintainers_path.push_back('/');
		maintainers_path += name;

		bool file_already_exists = false;
		if (stat(maintainers_path.c_str(), &sb) == 0) {
			file_already_exists = true;
			if (!refresh) {
				struct timespec current_time;
				timespec_get(&current_time, TIME_UTC);

				constexpr decltype(current_time.tv_sec) expires_after_seconds = 60 * 60 * 12;
				decltype(current_time.tv_sec) time_diff = current_time.tv_sec - sb.st_mtim.tv_sec;
				if (time_diff < expires_after_seconds)
					return maintainers_path;
			}
		}
		if (trace || refresh)
			emit_message("Downloading... ", maintainers_path, " from ", url);

		LibCurl libcurl;
		CurlHandle curl_handle;
		curl_handle.set_url(url);

		const std::string new_path = maintainers_path + ".NEW";
		PageFile page_file(new_path);
		if (page_file) {
			curl_easy_setopt(curl_handle.curl_handle, CURLOPT_WRITEDATA, page_file.page_file);
			if (curl_easy_perform(curl_handle.curl_handle)) {
				emit_message("Failed to fetch ", name, " from ", url, " to ", maintainers_path);
				if (file_already_exists)
					return maintainers_path;
				else
					throw 1;
			}
			long http_code = 0;
			curl_easy_getinfo(curl_handle.curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
			if (http_code >= 400) {
				emit_message("Failed to fetch ", name," (", http_code, ") from ", url, " to ", maintainers_path);
				if (file_already_exists)
					return maintainers_path;
				else
					throw 1;
			}
			rename(new_path.c_str(), maintainers_path.c_str());
		}
		return maintainers_path;
	}
}

#endif
