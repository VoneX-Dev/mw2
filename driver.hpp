#pragma once
#include "includes.hpp"

namespace driver
{
	namespace detail
	{
		inline void* function;
		inline std::string process;
		inline DWORD pid;
		inline std::uint64_t base;
		inline DWORD size_of_image;
	}

	bool running();
	bool load();
	bool attach(const char* process_name);
	bool read(std::uint64_t dst, void* src, ULONGLONG size);
	bool write(std::uint64_t dst, void* src, ULONGLONG size);
	std::string read_str(std::uint64_t dst);

	template<typename T>
	inline T read(std::uint64_t dst)
	{
		if (!dst)
			return T();

		T buffer{};
		if (read(dst, &buffer, sizeof(buffer)))
			return buffer;
		else
			return T();
	}

	template<typename T>
	inline bool write(std::uint64_t dst, T buffer)
	{
		if (!dst)
			return false;

		write(dst, &buffer, sizeof(buffer));
		return TRUE;
	}

	std::uint64_t module(LPCWSTR module_name);
	bool valid(std::uint64_t dst);
	std::uint64_t get_process_exe_base();
	std::uint64_t get_peb();
}