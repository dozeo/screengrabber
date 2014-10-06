#include <cstdint>

#include <slog/slog.h>
#include "libcommon/string_helpers.h"

#ifdef _WIN32
#include <Windows.h>
#include <Shlobj.h>
#endif

namespace dz
{
	std::wstring to_utf16(const std::string& utf8string)
	{
		wchar_t buffer[200];
		uint32_t stringlength = utf8string.length() + 1; // this makes WideCharToMultiByte also null terminate the output string
		int result = MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), stringlength, &buffer[0], sizeof(buffer));
		if (result > 0)
			return std::wstring(buffer);

		int required = MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), stringlength, nullptr, 0);
		if (required <= 0)
			throw std::runtime_error(strobj() << "MultiByteToWideChar failed to give the required size to convert a utf8 string to utf16 (size of utf8: " << utf8string.length() << ")");

		std::wstring resultstr;
		resultstr.resize(required);
		required = MultiByteToWideChar(CP_UTF8, 0, utf8string.c_str(), stringlength, &resultstr[0], resultstr.length());
		if (required <= 0)
			throw std::runtime_error(strobj() << "WideCharToMultiByte failed to convert the utf8 string given to utf16 (size of utf8: " << utf8string.length() << ")");

		return std::move(resultstr);
	}

	std::string to_utf8(const std::wstring& widestring)
	{
		char buffer[200];
		uint32_t widestringlength = widestring.length() + 1; // this makes WideCharToMultiByte also null terminate the output string
		int result = WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), widestringlength, &buffer[0], sizeof(buffer), nullptr, nullptr);
		if (result > 0)
			return std::string(buffer);

		int required = WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), widestringlength, nullptr, 0, nullptr, nullptr);
		if (required <= 0)
			throw std::runtime_error(strobj() << "WideCharToMultiByte failed to give the required size to convert a utf16 string to utf8 (size of utf16: " << widestring.length() << ")");

		std::string resultstr;
		resultstr.resize(required);
		required = WideCharToMultiByte(CP_UTF8, 0, widestring.c_str(), widestringlength, &resultstr[0], resultstr.length(), nullptr, nullptr);
		if (required <= 0)
			throw std::runtime_error(strobj() << "WideCharToMultiByte failed to convert the utf16 string given to utf8 (size of utf16: " << widestring.length() << ")");

		return std::move(resultstr);
	}

	std::string get_tmp_dir()
	{
#ifdef _WIN32
		wchar_t buffer[1024];
		DWORD numchars = ExpandEnvironmentStringsW(L"%TMP%", buffer, sizeof(buffer));
		buffer[numchars] = 0;
		return to_utf8(buffer);
#else
		return "/tmp";
#endif
	}

	std::string get_user_profile_dir()
	{
#ifdef _WIN32
		wchar_t buffer[1024];
		DWORD numchars = ExpandEnvironmentStringsW(L"%appdata%", buffer, sizeof(buffer));
		buffer[numchars] = 0;
		return to_utf8(buffer);
#else
#endif
	}

	void ensure_directory_structure(const std::string& path)
	{
		auto result = SHCreateDirectoryExW(nullptr, to_utf16(path).c_str(), nullptr);
		if (result != ERROR_SUCCESS && result != ERROR_ALREADY_EXISTS && result != ERROR_FILE_EXISTS)
			throw std::runtime_error(strobj() << "SHCreateDirectoryExW failed to create directory structure '" << path << "'");
	}

	void alert(const std::string& text, std::string title)
	{
#ifdef _WIN32
		MessageBoxA(nullptr, text.c_str(), title.c_str(), MB_OK | MB_ICONERROR);
#endif
	}
}
