#pragma once

#include <string>

namespace dz
{
	std::string to_utf8(const std::wstring& widestring);
	std::string get_tmp_dir();
	std::string get_user_profile_dir();
	void ensure_directory_structure(const std::string& path);

	void alert(const std::string& text, std::string title = "Error");
}
