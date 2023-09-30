#pragma once
#include "includes.hpp"

namespace memory
{
	enum : std::uint32_t
	{
		ret = 0x7740365a,
		nop = 0x9c7a80de
	};

	inline char to_lower(const char c)
	{
		return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
	}

	inline std::uint32_t hash_register(unsigned char byte)
	{
		uint32_t hash = 0;

		hash += to_lower(byte);
		hash += (hash << 10);
		hash ^= (hash >> 6);

		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);

		return hash;
	}

	void* __cdecl __memccpy(void* to, const void* from, int c, size_t count);
	void* __cdecl _memchr(const void* s, int c, size_t n);
	void* __cdecl _memcpy(void* dest, const void* src, size_t count);
	void* __cdecl _memmove(void* dest, const void* src, size_t count);
	void* __cdecl _memset(void* src, int val, size_t count);
	const unsigned short* __cdecl __pwctype_func(void);
	int __cdecl __isctype(int c, int ctypeFlags);
	wint_t __cdecl _towlower(wint_t c);
	int __cdecl _isspace(int c);
	int __cdecl _isdigit(int c);
	int __cdecl _isalpha(int c);
	int __cdecl _isupper(int c);
	unsigned long long __cdecl _strtoull(const char* nptr, char** endptr, int base);
	int __cdecl crc32_wcsicmp(const wchar_t* cs, const wchar_t* ct);
	unsigned int crc32_strlen(const char* String);
	int crc32_strcmp(const char* String1, const char* String2);
	int crc32_toupper(int c);
	unsigned long long crc32_strtoull(const char* nptr, char** endptr, int base);
	int crc32_memcmp(const void* s1, const void* s2, size_t n);
	const char* crc32_strstr(const char* String1, const char* String2);
	char* crc32_strncpy(char* DstString, const char* SrcString, unsigned int Count);
	std::uint64_t get_main_image_base();
	std::uint64_t get_module(std::string module_name);
	std::uint64_t get_export(LPCSTR module_name, LPCSTR exported_routine);
	void popup(const std::string message, std::uint32_t flags = (MB_OK | MB_ICONINFORMATION));
	DWORD get_pid(std::string process);
	bool create_thread(void* context);
	int random_num(int max);
	std::string random_str(int length = 15);
	void exit_handler(const char* message, ...);
	void obfuscate();
	bool initialize();

	template<typename T = void*, typename ... Args>
	inline T call(const char* szModule, const char* szExport, Args ... args)
	{
		auto fn = reinterpret_cast<T(*)(Args...)>(memory::get_export(szModule, szExport));
		if (!fn) return T();

		return fn(args ...);
	}
}

namespace memory
{
	inline BOOL(*fnQueryPerformanceFrequency)(LARGE_INTEGER*);
	inline BOOL(*fnQueryPerformanceCounter)(LARGE_INTEGER*);
	inline HCURSOR(*fnSetCursor)(HCURSOR);
	inline HCURSOR(*fnLoadCursorA)(HINSTANCE, LPCSTR);
	inline SHORT(*fnGetKeyState)(int);
	inline BOOL(*fnClientToScreen)(HWND, LPPOINT);
	inline BOOL(*fnSetCursorPos)(int, int);
	inline BOOL(*fnScreenToClient)(HWND, LPPOINT);
	inline BOOL(*fnGetClientRect)(HWND, LPRECT);
	inline BOOL(*fnTrackMouseEvent)(LPTRACKMOUSEEVENT);
	inline HWND(*fnSetCapture)(HWND);
	inline HWND(*fnGetCapture)();
	inline BOOL(*fnReleaseCapture)();
	inline HWND(*fnGetForegroundWindow)();
	inline BOOL(*fnGetCursorPos)(LPPOINT);
	inline LRESULT(*fnDefWindowProcA)(HWND, UINT, WPARAM, LPARAM);
	inline BOOL(*fnPeekMessageA)(LPMSG, HWND, UINT, UINT, UINT);
	inline BOOL(*fnTranslateMessage)(const MSG*);
	inline LRESULT(*fnDispatchMessageA)(const MSG*);
	inline BOOL(*NtUserGetAsyncKeyState)(INT);
	inline HWND(__stdcall* fnCreateWindowExA)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
	inline ULONGLONG(__stdcall* fnGetTickCount64)();
	inline LONG(*RtlCreateUserThread)(HANDLE, PSECURITY_DESCRIPTOR, BOOLEAN, ULONG, SIZE_T, SIZE_T, PTHREAD_START_ROUTINE, PVOID, PHANDLE, void*);
	inline void(*fnSleep)(DWORD);
}

inline HANDLE _CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	static auto function = (decltype(&_CreateFileA))(0);

	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("kernel32.dll")), xorstr_("CreateFileA")));

	return function(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

inline HANDLE _CreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile)
{
	static auto function = (decltype(&_CreateFileW))(0);

	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("kernel32.dll")), xorstr_("CreateFileW")));

	return function(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}

inline BOOL _DeviceIoControl(HANDLE hDevice, DWORD dwIoControlCode, PVOID lpInBuffer, DWORD nInBufferSize, PVOID lpOutBuffer, DWORD nOutBufferSize, DWORD* lpBytesReturned, LPOVERLAPPED lpOverlapped)
{
	static auto function = (BOOL(__stdcall*)(HANDLE, DWORD, PVOID, DWORD, PVOID, DWORD, DWORD*, LPOVERLAPPED))(0);

	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("kernel32.dll")), xorstr_("DeviceIoControl")));

	return function(hDevice, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, lpBytesReturned, lpOverlapped);
}

inline BOOL _CloseHandle(HANDLE handle)
{
	static auto function = reinterpret_cast<BOOL(__stdcall*)(HANDLE)>(0);

	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("kernel32.dll")), xorstr_("CloseHandle")));

	return function(handle);
}

inline LONG _NtWriteVirtualMemory(HANDLE handle, PVOID base, PVOID buffer, ULONG bytes, PULONG written)
{
	static auto function = reinterpret_cast<LONG(__stdcall*)(HANDLE, PVOID, PVOID, ULONG, PULONG)>(0);

	if (!function)
		*(std::uint64_t*)&function = (memory::get_export(xorstr_("ntdll.dll"), xorstr_("NtWriteVirtualMemory")));

	return function(handle, base, buffer, bytes, written);
}

inline LONG _NtReadVirtualMemory(HANDLE handle, PVOID base, PVOID buffer, ULONG read, PULONG readed)
{
	static auto function = reinterpret_cast<LONG(__stdcall*)(HANDLE, PVOID, PVOID, ULONG, PULONG)>(0);

	if (!function)
		*(std::uint64_t*)&function = (memory::get_export(xorstr_("ntdll.dll"), xorstr_("NtReadVirtualMemory")));

	return function(handle, base, buffer, read, readed);
}

inline LONG _NtClose(HANDLE hObject)
{
	return memory::call<LONG>(xorstr_("ntdll.dll"), xorstr_("NtClose"), hObject);
}

inline BOOL _VirtualFreeEx(HANDLE hProcess, PVOID lpAddress, ULONG dwSize, DWORD dwFreeType)
{
	return memory::call<BOOL>(xorstr_("ntdll.dll"), xorstr_("NtFreeVirtualMemory"), hProcess, &lpAddress, &dwSize, dwFreeType);
}

inline LPVOID __stdcall _VirtualAllocEx(HANDLE hProcess, LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect)
{
	memory::call<LONG>(xorstr_("ntdll.dll"), xorstr_("NtAllocateVirtualMemory"), hProcess, &lpAddress, 0, &dwSize, flAllocationType, flProtect);
	return lpAddress;
}

inline DWORD __stdcall _GetModuleFileNameA(HMODULE a1, LPSTR a2, DWORD a3)
{
	return memory::call<DWORD>(xorstr_("kernel32.dll"), xorstr_("GetModuleFileNameA"), a1, a2, a3);
}

inline BOOL __stdcall _OpenProcessToken(HANDLE a1, DWORD a2, PHANDLE a3)
{
	return memory::call<BOOL>(xorstr_("advapi32.dll"), xorstr_("OpenProcessToken"), a1, a2, a3);
}

inline BOOL _GetTokenInformation(HANDLE a1, TOKEN_INFORMATION_CLASS a2, LPVOID a3, DWORD a4, PDWORD a5)
{
	return memory::call<BOOL>(xorstr_("advapi32.dll"), xorstr_("GetTokenInformation"), a1, a2, a3, a4, a5);
}

inline BOOL _ConvertSidToStringSidA(PSID Sid, LPSTR* StringSid)
{
	return memory::call<BOOL>(xorstr_("advapi32.dll"), xorstr_("ConvertSidToStringSidA"), Sid, StringSid);
}


inline LSTATUS _RegDeleteKeyW(HKEY hKey, LPCWSTR lpSubKey)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCWSTR)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegDeleteKeyW")));

	return function(hKey, lpSubKey);
}

inline LSTATUS _RegCloseKey(HKEY hKey)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegCloseKey")));

	return function(hKey);
}

inline LSTATUS _RegOpenKeyW(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCWSTR, PHKEY)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegOpenKeyW")));

	return function(hKey, lpSubKey, phkResult);
}

inline LSTATUS _RegSetKeyValueW(HKEY hKey, LPCWSTR lpSubKey, LPCWSTR lpValueName, DWORD dwType, LPCVOID lpData, DWORD cbData)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCWSTR, LPCWSTR, DWORD, LPCVOID, DWORD)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegSetKeyValueW")));

	return function(hKey, lpSubKey, lpValueName, dwType, lpData, cbData);
}

inline LSTATUS _RegCreateKeyW(HKEY hKey, LPCWSTR lpSubKey, PHKEY phkResult)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCWSTR, PHKEY)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegCreateKeyW")));

	return function(hKey, lpSubKey, phkResult);
}

inline LSTATUS _RegCreateKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCSTR, PHKEY)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegCreateKeyA")));

	return function(hKey, lpSubKey, phkResult);
}

inline LSTATUS _RegSetValueExA(HKEY hKey, LPCSTR lpValueName, DWORD Reserved, DWORD dwType, CONST BYTE* lpData, DWORD cbData)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCSTR, DWORD, DWORD, CONST BYTE*, DWORD)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegSetValueExA")));

	return function(hKey, lpValueName, Reserved, dwType, lpData, cbData);
}

inline LSTATUS _RegOpenKeyA(HKEY hKey, LPCSTR lpSubKey, PHKEY phkResult)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCSTR, PHKEY)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegOpenKeyA")));

	return function(hKey, lpSubKey, phkResult);
}

inline LSTATUS _RegDeleteKeyA(HKEY hKey, LPCSTR lpSubKey)
{
	auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCSTR)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegDeleteKeyA")));

	return function(hKey, lpSubKey);
}

inline LSTATUS _RegGetValueA(HKEY hkey, LPCSTR lpSubKey, LPCSTR lpValue, DWORD dwFlags, LPDWORD pdwType, PVOID pvData, LPDWORD pcbData)
{
	auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCSTR, LPCSTR, DWORD, LPDWORD, PVOID, LPDWORD)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegGetValueA")));

	return function(hkey, lpSubKey, lpValue, dwFlags, pdwType, pvData, pcbData);
}

inline LSTATUS _RegQueryValueExA(HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData)
{
	auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD)>(0);
	if (!function)
		*(FARPROC*)&function = (memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegQueryValueExA")));

	return function(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData);
}

inline LSTATUS _RegOpenKeyExA(HKEY hKey, LPCSTR lpSubKey, DWORD ulOptions, REGSAM samDesired, PHKEY phkResult)
{
	static auto function = reinterpret_cast<LSTATUS(__stdcall*)(HKEY, LPCSTR, DWORD, REGSAM, PHKEY)>(0);
	if (!function)
		*(FARPROC*)&function = memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"), (HMODULE)memory::get_module(xorstr_("advapi32.dll")), xorstr_("RegOpenKeyExA"));

	return function(hKey, lpSubKey, ulOptions, samDesired, phkResult);
}


#define ENCRYPTED_CALL(prototype, address, ...) reinterpret_cast<prototype>(&address)(__VA_ARGS__)
