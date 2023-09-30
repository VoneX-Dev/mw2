#include "memory.hpp"

struct CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
};

struct TEB
{
	NT_TIB NtTib;
	PVOID EnvironmentPointer;
	CLIENT_ID ClientId;
	PVOID ActiveRpcHandle;
	PVOID ThreadLocalStoragePointer;
	struct PEB* ProcessEnvironmentBlock;
};

struct PEB_LDR_DATA
{
	ULONG Length;
	BOOLEAN Initialized;
	HANDLE SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
	PVOID EntryInProgress;
	BOOLEAN ShutdownInProgress;
	HANDLE ShutdownThreadId;
};

struct PEB
{
	BOOLEAN InheritedAddressSpace;
	BOOLEAN ReadImageFileExecOptions;
	BOOLEAN BeingDebugged;
	union
	{
		BOOLEAN BitField;
		struct
		{
			BOOLEAN ImageUsesLargePages : 1;
			BOOLEAN IsProtectedProcess : 1;
			BOOLEAN IsImageDynamicallyRelocated : 1;
			BOOLEAN SkipPatchingUser32Forwarders : 1;
			BOOLEAN IsPackagedProcess : 1;
			BOOLEAN IsAppContainer : 1;
			BOOLEAN IsProtectedProcessLight : 1;
			BOOLEAN SpareBits : 1;
		};
	};
	HANDLE Mutant;
	PVOID ImageBaseAddress;
	PEB_LDR_DATA* Ldr;
};

struct UNICODE_STRING
{
	USHORT Length;
	USHORT MaximumLength;
	PWCH Buffer;
};

struct LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	union
	{
		LIST_ENTRY InInitializationOrderLinks;
		LIST_ENTRY InProgressLinks;
	};
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
};

namespace memory
{
	void* __cdecl __memccpy(void* to, const void* from, int c, size_t count)
	{
		char t;
		size_t i;
		char* dst = (char*)to;
		const char* src = (const char*)from;

		for (i = 0; i < count; i++)
		{
			dst[i] = t = src[i];
			if (t == '\0')
				break;
			if (t == c)
				return &dst[i + 1];
		}
		return NULL; /* didn't copy c */
	}

	void* __cdecl _memchr(const void* s, int c, size_t n)
	{
		if (n)
		{
			const char* p = (const char*)s;
			do {
				if (*p++ == c)
					return (void*)(p - 1);
			} while (--n != 0);
		}
		return 0;
	}

	void* __cdecl _memcpy(void* dest, const void* src, size_t count)
	{
		char* char_dest = (char*)dest;
		char* char_src = (char*)src;

		if ((char_dest <= char_src) || (char_dest >= (char_src + count)))
		{
			/*  non-overlapping buffers */
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest++;
				char_src++;
				count--;
			}
		}
		else
		{
			/* overlaping buffers */
			char_dest = (char*)dest + count - 1;
			char_src = (char*)src + count - 1;

			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest--;
				char_src--;
				count--;
			}
		}

		return dest;
	}

	void* __cdecl _memmove(void* dest, const void* src, size_t count)
	{
		char* char_dest = (char*)dest;
		char* char_src = (char*)src;

		if ((char_dest <= char_src) || (char_dest >= (char_src + count)))
		{
			/*  non-overlapping buffers */
			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest++;
				char_src++;
				count--;
			}
		}
		else
		{
			/* overlaping buffers */
			char_dest = (char*)dest + count - 1;
			char_src = (char*)src + count - 1;

			while (count > 0)
			{
				*char_dest = *char_src;
				char_dest--;
				char_src--;
				count--;
			}
		}

		return dest;
	}

	void* __cdecl _memset(void* src, int val, size_t count)
	{
		char* char_src = (char*)src;

		while (count > 0) {
			*char_src = val;
			char_src++;
			count--;
		}
		return src;
	}

	const unsigned short* __cdecl __pwctype_func(void)
	{
		return __pwctype_func();
	}

	int __cdecl __isctype(int c, int ctypeFlags)
	{
		return (__pctype_func()[(unsigned char)(c & 0xFF)] & ctypeFlags);
	}

	wint_t __cdecl _towlower(wint_t c)
	{
		if (__isctype(c, _UPPER))
			return (c - ('A' - 'a'));
		return(c);
	}

	int __cdecl _isspace(int c)
	{
		return (__isctype(c, _SPACE));
	}

	int __cdecl _isdigit(int c)
	{
		return(__isctype(c, _DIGIT));
	}

	int __cdecl _isalpha(int c)
	{
		return(__isctype(c, _ALPHA));
	}

	int __cdecl _isupper(int c)
	{
		return (__isctype(c, _UPPER));
	}

	unsigned long long __cdecl _strtoull(const char* nptr, char** endptr, int base)
	{
		const char* s = nptr;
		unsigned long long acc;
		int c;
		unsigned long long cutoff;
		int neg = 0, any, cutlim;

		/*
		 * See strtol for comments as to the logic used.
		 */
		do {
			c = *s++;
		} while (_isspace(c));
		if (c == '-')
		{
			neg = 1;
			c = *s++;
		}
		else if (c == '+')
			c = *s++;
		if ((base == 0 || base == 16) &&
			c == '0' && (*s == 'x' || *s == 'X'))
		{
			c = s[1];
			s += 2;
			base = 16;
		}
		if (base == 0)
			base = c == '0' ? 8 : 10;
		cutoff = (unsigned long long)ULLONG_MAX / (unsigned long long)base;
		cutlim = (unsigned long long)ULLONG_MAX % (unsigned long long)base;
		for (acc = 0, any = 0;; c = *s++)
		{
			if (_isdigit(c))
				c -= '0';
			else if (_isalpha(c))
				c -= _isupper(c) ? 'A' - 10 : 'a' - 10;
			else
				break;
			if (c >= base)
				break;
			if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
				any = -1;
			else {
				any = 1;
				acc *= base;
				acc += c;
			}
		}
		if (neg)
			acc = 0 - acc;
		if (endptr != 0)
			*endptr = any ? (char*)((size_t)(s - 1)) : (char*)((size_t)nptr);
		return acc;
	}

	int __cdecl crc32_wcsicmp(const wchar_t* cs, const wchar_t* ct)
	{
		while (_towlower(*cs) == _towlower(*ct))
		{
			if (*cs == 0)
				return 0;
			cs++;
			ct++;
		}
		return _towlower(*cs) - _towlower(*ct);
	}

	unsigned int crc32_strlen(const char* String)
	{
		UINT32 Length = 0;
		while (*String)
		{
			Length++;
			String++;
		}

		return (Length);
	}

	int crc32_strcmp(const char* String1, const char* String2)
	{
		for (; (*String1 == *String2); String2++)
		{
			if (!*String1++)
			{
				return (0);
			}
		}

		return ((unsigned char)*String1 - (unsigned char)*String2);
	}

	int crc32_toupper(int c)
	{
		return (islower(c) ? ((c)-0x20) : (c));
	}

	unsigned long long crc32_strtoull(const char* nptr, char** endptr, int base)
	{
		const char* s = nptr;
		unsigned long long acc;
		int c;
		unsigned long long cutoff;
		int neg = 0, any, cutlim;

		do {
			c = *s++;
		} while (_isspace(c));
		if (c == '-')
		{
			neg = 1;
			c = *s++;
		}
		else if (c == '+')
			c = *s++;
		if ((base == 0 || base == 16) &&
			c == '0' && (*s == 'x' || *s == 'X'))
		{
			c = s[1];
			s += 2;
			base = 16;
		}
		if (base == 0)
			base = c == '0' ? 8 : 10;
		cutoff = (unsigned long long)ULLONG_MAX / (unsigned long long)base;
		cutlim = (unsigned long long)ULLONG_MAX % (unsigned long long)base;
		for (acc = 0, any = 0;; c = *s++)
		{
			if (_isdigit(c))
				c -= '0';
			else if (_isalpha(c))
				c -= _isupper(c) ? 'A' - 10 : 'a' - 10;
			else
				break;
			if (c >= base)
				break;
			if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
				any = -1;
			else {
				any = 1;
				acc *= base;
				acc += c;
			}
		}
		if (any < 0)
		{
			acc = ULLONG_MAX;
		}
		else if (neg)
			acc = 0 - acc;
		if (endptr != 0)
			*endptr = any ? (char*)((size_t)(s - 1)) : (char*)((size_t)nptr);
		return acc;
	}

	int crc32_memcmp(const void* s1, const void* s2, size_t n)
	{
		if (n != 0) {
			const unsigned char* p1 = (const unsigned char*)s1, * p2 = (const unsigned char*)s2;
			do {
				if (*p1++ != *p2++)
					return (*--p1 - *--p2);
			} while (--n != 0);
		}
		return 0;
	}

	const char* crc32_strstr(const char* String1, const char* String2)
	{
		unsigned int Length;

		Length = crc32_strlen(String2);
		if (!Length)
		{
			return (String1);
		}

		while (crc32_strlen(String1) >= Length)
		{
			if (crc32_memcmp(String1, String2, Length) == 0)
			{
				return (String1);
			}
			String1++;
		}

		return (NULL);
	}

	char* crc32_strncpy(char* DstString, const char* SrcString, unsigned int Count)
	{
		char* String = DstString;
		for (String = DstString;
			Count && (Count--, (*String++ = *SrcString++)); )
		{
			;
		}

		while (Count--)
		{
			*String = 0;
			String++;
		}

		return (DstString);
	}

	_TEB* _NtCurrentTeb()
	{
		return (struct _TEB*)__readgsqword(((LONG)__builtin_offsetof(NT_TIB, Self)));
	}

	std::uint64_t get_main_image_base()
	{
		PEB* ProcessEnvironmentBlock = ((PEB*)((TEB*)((TEB*)_NtCurrentTeb())->ProcessEnvironmentBlock));
		return (std::uint64_t)ProcessEnvironmentBlock->ImageBaseAddress;
	}

	std::uint64_t get_module(std::string module_name)
	{
		auto GetFileNameFromPath = [](wchar_t* Path) -> wchar_t*
		{
			wchar_t* LastSlash = NULL;
			for (DWORD i = 0; Path[i] != 0; i++)
			{
				if (Path[i] == '\\')
					LastSlash = &Path[i + 1];
			}
			return LastSlash;
		};

		auto RemoveFileExtension = [](wchar_t* FullFileName, wchar_t* OutputBuffer, DWORD OutputBufferSize)	-> wchar_t*
		{
			wchar_t* LastDot = NULL;
			for (DWORD i = 0; FullFileName[i] != NULL; i++)
				if (FullFileName[i] == '.')
					LastDot = &FullFileName[i];

			for (DWORD j = 0; j < OutputBufferSize; j++)
			{
				OutputBuffer[j] = FullFileName[j];
				if (&FullFileName[j] == LastDot)
				{
					OutputBuffer[j] = NULL;
					break;
				}
			}
			OutputBuffer[OutputBufferSize - 1] = NULL;
			return OutputBuffer;
		};

		PEB* ProcessEnvironmentBlock = ((PEB*)((TEB*)((TEB*)_NtCurrentTeb())->ProcessEnvironmentBlock));
		PEB_LDR_DATA* Ldr = ProcessEnvironmentBlock->Ldr;

		std::wstring Temp(module_name.begin(), module_name.end());
		const wchar_t* lpModuleName = Temp.c_str();

		LIST_ENTRY* ModuleLists[3] = { 0,0,0 };
		ModuleLists[0] = &Ldr->InLoadOrderModuleList;
		ModuleLists[1] = &Ldr->InMemoryOrderModuleList;
		ModuleLists[2] = &Ldr->InInitializationOrderModuleList;

		for (int j = 0; j < 3; j++)
		{
			for (LIST_ENTRY* pListEntry = ModuleLists[j]->Flink; pListEntry != ModuleLists[j]; pListEntry = pListEntry->Flink)
			{
				LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)((BYTE*)pListEntry - sizeof(LIST_ENTRY) * j);

				if (crc32_wcsicmp(pEntry->BaseDllName.Buffer, lpModuleName) == 0)
				{
					return (uintptr_t)pEntry->DllBase;
				}

				wchar_t* FileName = GetFileNameFromPath(pEntry->FullDllName.Buffer);
				if (!FileName)
					continue;

				if (crc32_wcsicmp(FileName, lpModuleName) == 0)
				{
					return (uintptr_t)pEntry->DllBase;
				}

				wchar_t FileNameWithoutExtension[256];
				RemoveFileExtension(FileName, FileNameWithoutExtension, 256);

				if (crc32_wcsicmp(FileNameWithoutExtension, lpModuleName) == 0)
				{
					return (uintptr_t)pEntry->DllBase;
				}
			}
		}

		return 0;
	}

	std::uint64_t get_export(LPCSTR module_name, LPCSTR exported_routine)
	{
		PIMAGE_DOS_HEADER pIDH;
		PIMAGE_NT_HEADERS pINH;
		PIMAGE_EXPORT_DIRECTORY pIED;

		HMODULE hModule;
		PDWORD Address, Name;
		PWORD Ordinal;

		DWORD i;

		hModule = (HMODULE)get_module(module_name);
		if (!hModule) return 0;

		pIDH = (PIMAGE_DOS_HEADER)hModule;

		if (pIDH->e_magic != IMAGE_DOS_SIGNATURE) return 0;
		pINH = (PIMAGE_NT_HEADERS)((LPBYTE)hModule + pIDH->e_lfanew);

		if (pINH->Signature != IMAGE_NT_SIGNATURE) return 0;
		if (pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0) return 0;

		pIED = (PIMAGE_EXPORT_DIRECTORY)((LPBYTE)hModule + pINH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
		Address = (PDWORD)((LPBYTE)hModule + pIED->AddressOfFunctions);
		Name = (PDWORD)((LPBYTE)hModule + pIED->AddressOfNames);
		Ordinal = (PWORD)((LPBYTE)hModule + pIED->AddressOfNameOrdinals);

		for (i = 0; i < pIED->AddressOfFunctions; i++)
		{
			if (!strcmp(exported_routine, (char*)hModule + Name[i]))
			{
				return (std::uint64_t)((LPBYTE)hModule + Address[Ordinal[i]]);
			}
		}

		return 0;
	}

	void popup(const std::string message, std::uint32_t flags)
	{
		MSGBOXPARAMSA msgbox;

		msgbox.cbSize = sizeof(MSGBOXPARAMSA);
		msgbox.hwndOwner = 0;
		msgbox.hInstance = 0;
		msgbox.lpszText = message.c_str();
		msgbox.lpszCaption = 0;
		msgbox.dwStyle = flags;
		msgbox.lpszIcon = 0;
		msgbox.dwContextHelpId = 0;
		msgbox.lpfnMsgBoxCallback = 0;
		msgbox.dwLanguageId = LANG_NEUTRAL;

		auto function = reinterpret_cast<int(__stdcall*)(const tagMSGBOXPARAMSA*)>(get_export(xorstr_("user32.dll"), xorstr_("MessageBoxIndirectA")));
		function(&msgbox);
	}

	DWORD get_pid(std::string process)
	{
		DWORD pid = 0;

		HANDLE snapshot = memory::call<HANDLE>(xorstr_("kernel32.dll"), xorstr_("CreateToolhelp32Snapshot"), TH32CS_SNAPPROCESS, 0);
		if (snapshot == INVALID_HANDLE_VALUE) return false;

		PROCESSENTRY32 pe32{ sizeof(PROCESSENTRY32) };
		if (!memory::call<bool>(xorstr_("kernel32.dll"), xorstr_("Process32First"), snapshot, &pe32)) return false;

		while (memory::call<bool>(xorstr_("kernel32.dll"), xorstr_("Process32Next"), snapshot, &pe32))
		{
			if (crc32_strcmp(pe32.szExeFile, process.c_str()) == 0)
			{
				pid = pe32.th32ProcessID;
				break;
			}
		}

		memory::call<bool>(xorstr_("kernel32.dll"), xorstr_("CloseHandle"), snapshot);
		return pid;
	}

	bool create_thread(void* context)
	{
		HANDLE thread_handle = 0;
		RtlCreateUserThread((HANDLE)-1, 0, 0, 0, 0, 0, (PTHREAD_START_ROUTINE)context, 0, &thread_handle, 0);

		return true;
	}

	int random_num(int max)
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> distribution(0, max);

		return distribution(rng);
	}

	std::string random_str(int length)
	{
		static std::string characters = xorstr_("ABCDEGFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890/+");
		std::string result = {};

		srand((unsigned)time(0) * memory::call<DWORD>(xorstr_("kernel32.dll"), xorstr_("GetCurrentThreadId")));
		for (int i = 0; i < 16; i++)
			result += characters[rand() % characters.length()];

		return result;
	}

	void exit_handler(const char* message, ...)
	{
		if (crc32_strlen(message) > 1)
		{
			char buf[1024];
			va_list args;
			va_start(args, message);
			vsnprintf_s(buf, sizeof(buf), message, args);
			va_end(args);
			printf(buf);
		}

		fnSleep(10000);

		int* array = new int[1];
		for (int x = 0; x < 0xff; x++)
			array[x] = (x >> (0 | ~(x & 255) ^ 0xc3));

		*(int**)(array)[16] = *(int**)&message;
		array[25] = 52;
		return;
	}

	std::vector<std::pair<std::string, uintptr_t>> get_all_modules()
	{
		std::vector<std::pair<std::string, uintptr_t>> results{};

		PEB* ProcessEnvironmentBlock = ((PEB*)((TEB*)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock));
		PEB_LDR_DATA* Ldr = ProcessEnvironmentBlock->Ldr;

		LIST_ENTRY* ModuleLists[3] = { 0,0,0 };
		ModuleLists[0] = &Ldr->InLoadOrderModuleList;
		ModuleLists[1] = &Ldr->InMemoryOrderModuleList;
		ModuleLists[2] = &Ldr->InInitializationOrderModuleList;

		for (int j = 0; j < 3; j++)
		{
			for (LIST_ENTRY* pListEntry = ModuleLists[j]->Flink; pListEntry != ModuleLists[j]; pListEntry = pListEntry->Flink)
			{
				LDR_DATA_TABLE_ENTRY* pEntry = (LDR_DATA_TABLE_ENTRY*)((BYTE*)pListEntry - sizeof(LIST_ENTRY) * j);

				std::wstring wide(pEntry->BaseDllName.Buffer);
				results.push_back({ std::string(wide.begin(), wide.end()), (uintptr_t)pEntry->DllBase });
			}
		}

		return results;
	}

	void obfuscate()
	{
		char buf[MAX_PATH];
		memory::call<DWORD, HMODULE, LPSTR, DWORD>(xorstr_("kernel32.dll"), xorstr_("GetModuleFileNameA"), 0, buf, sizeof(buf));

		//int status = std::rename(buf, std::string(memory::random_str(memory::random_num(11))).append(xorstr_(".exe")).c_str());
		memory::call<void>(xorstr_("kernel32.dll"), xorstr_("SetConsoleTitleA"), memory::random_str(memory::random_num(24)).c_str());

		PEB* peb = ((PEB*)((TEB*)((TEB*)NtCurrentTeb())->ProcessEnvironmentBlock));
		if (!peb) return;

		for (auto& dll : get_all_modules())
		{
			auto hModule = dll.second;
			if (!hModule) continue;

			auto dll_dos = (PIMAGE_DOS_HEADER)hModule;
			if (!dll_dos) continue;

			auto dll_nt = (PIMAGE_NT_HEADERS)(hModule + dll_dos->e_lfanew);
			if (!dll_nt) continue;

			const auto obfuscate_section = [](PIMAGE_SECTION_HEADER& section)
			{
				DWORD old;
				memory::call<int, void*, SIZE_T, DWORD, PDWORD>(xorstr_("kernel32.dll"), xorstr_("VirtualProtect"), section, sizeof(section), PAGE_EXECUTE_READWRITE, &old);

				section->Name[0] = ' ';
				section->Name[1] = '\0';

				section->Characteristics = IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_LOCKED;
				section->Misc.PhysicalAddress = 0;
				section->Misc.VirtualSize = 0;

				section->NumberOfLinenumbers = 0;
				section->NumberOfRelocations = 0;
				section->PointerToRawData = 0;
				section->PointerToRelocations = 0;
				section->SizeOfRawData = 0;
				section->VirtualAddress = 0;

				memory::call<int, void*, SIZE_T, DWORD, PDWORD>(xorstr_("kernel32.dll"), xorstr_("VirtualProtect"), section, sizeof(section), old, &old);
			};

			// DESTROY SECTIONS
			PIMAGE_SECTION_HEADER section = IMAGE_FIRST_SECTION(dll_nt);

			WORD sec_cnt = 0;
			while (sec_cnt < dll_nt->FileHeader.NumberOfSections)
			{
				obfuscate_section(section);
				sec_cnt++;
				section++;
			}

			// ----------------------------------

			DWORD old;
			memory::call<int, void*, SIZE_T, DWORD, PDWORD>(xorstr_("kernel32.dll"), xorstr_("VirtualProtect"), dll_nt, sizeof(dll_nt), PAGE_EXECUTE_READWRITE, &old);

			char buffer[4096 * sizeof(dll_nt) * sizeof(dll_nt->OptionalHeader.SizeOfImage)];
			memory::_memset(&buffer, 0, sizeof(buffer));

			dll_nt->FileHeader.NumberOfSections = 0;
			dll_nt->FileHeader.NumberOfSymbols = 0;

			dll_nt->OptionalHeader.AddressOfEntryPoint = 0;
			dll_nt->OptionalHeader.ImageBase = 0;

			dll_nt->OptionalHeader.SizeOfCode = 0;
			dll_nt->OptionalHeader.SizeOfHeaders = 0;

			dll_nt->OptionalHeader.SectionAlignment = 0;

			memory::_memcpy(&buffer, &dll_nt, sizeof(buffer));

			memory::call<int, void*, SIZE_T, DWORD, PDWORD>(xorstr_("kernel32.dll"), xorstr_("VirtualProtect"), dll_nt, sizeof(dll_nt), old, &old);
		}

		BYTE key[256];

		srand((unsigned int)time(0));
		for (uint32_t i = 0; i < 256; i++)
			key[i] = ((rand() % 255) & 0xFF);

		static BYTE last_decryption_key = 0x00;
		auto PEB_ENCRYPTION = [&](std::function<void(PEB*)> callback)->void
		{
			BYTE value = (key[rand() % 256] ^ (uintptr_t)&callback);
			last_decryption_key = value;

			// encrypt PEB
			*(uint64_t*)&peb <<= 64;
			*(uint64_t*)(&peb) += value & 0xFF;

			// decrypt  PEB
			BYTE i;
			for (i = 0; i != value; i++);

			*(uint64_t*)(&peb) -= 0xFF & i;
			*(uint64_t*)&peb >>= 64;

			callback(peb);
		};

		PEB_ENCRYPTION([](PEB* _peb) { _peb->BeingDebugged = FALSE; });
		PEB_ENCRYPTION([](PEB* _peb) { _peb->BitField = FALSE; });
		PEB_ENCRYPTION([](PEB* _peb) { _peb->ImageUsesLargePages = TRUE; });
		PEB_ENCRYPTION([](PEB* _peb) { _peb->InheritedAddressSpace = FALSE; });
		PEB_ENCRYPTION([](PEB* _peb) { _peb->IsAppContainer = FALSE; });
		PEB_ENCRYPTION([](PEB* _peb) { _peb->IsImageDynamicallyRelocated = TRUE; });
		PEB_ENCRYPTION([](PEB* _peb) { _peb->IsProtectedProcessLight = TRUE; });
	}

	bool initialize()
	{
		HMODULE(__stdcall * crc32LoadLibraryA)(LPCSTR) = 0;
		if (!crc32LoadLibraryA)
			*(std::uint64_t*)&crc32LoadLibraryA = memory::get_export(xorstr_("kernel32.dll"), xorstr_("LoadLibraryA"));

		crc32LoadLibraryA(xorstr_("kernel32.dll"));
		crc32LoadLibraryA(xorstr_("ntdll.dll"));
		crc32LoadLibraryA(xorstr_("win32u.dll"));
		crc32LoadLibraryA(xorstr_("user32.dll"));
		crc32LoadLibraryA(xorstr_("dbghelp.dll"));
		crc32LoadLibraryA(xorstr_("rpcrt4.dll"));
		crc32LoadLibraryA(xorstr_("d3d9.dll"));
		crc32LoadLibraryA(xorstr_("advapi32.dll"));

		const auto GetProc = [](std::string dll, std::string str) -> std::uint64_t
		{
			return reinterpret_cast<std::uint64_t>(memory::call<FARPROC, HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetProcAddress"),
				memory::call<HMODULE, LPCSTR>(xorstr_("kernel32.dll"), xorstr_("GetModuleHandleA"), dll.c_str()),
				str.c_str()
				));
		};

		*(std::uint64_t*)&fnQueryPerformanceFrequency = get_export(xorstr_("kernel32.dll"), xorstr_("QueryPerformanceFrequency"));
		if (!fnQueryPerformanceFrequency)
			*(std::uint64_t*)&fnQueryPerformanceFrequency = GetProc(xorstr_("kernel32.dll"), xorstr_("QueryPerformanceFrequency"));

		*(std::uint64_t*)&fnQueryPerformanceCounter = get_export(xorstr_("kernel32.dll"), xorstr_("QueryPerformanceCounter"));
		if (!fnQueryPerformanceCounter)
			*(std::uint64_t*)&fnQueryPerformanceCounter = GetProc(xorstr_("kernel32.dll"), xorstr_("QueryPerformanceCounter"));

		*(std::uint64_t*)&fnSetCursor = get_export(xorstr_("user32.dll"), xorstr_("SetCursor"));
		if (!fnSetCursor)
			*(std::uint64_t*)&fnSetCursor = GetProc(xorstr_("user32.dll"), xorstr_("SetCursor"));

		*(std::uint64_t*)&fnLoadCursorA = get_export(xorstr_("user32.dll"), xorstr_("LoadCursorA"));
		if (!fnLoadCursorA)
			*(std::uint64_t*)&fnLoadCursorA = GetProc(xorstr_("user32.dll"), xorstr_("LoadCursorA"));

		*(std::uint64_t*)&fnGetKeyState = get_export(xorstr_("user32.dll"), xorstr_("GetKeyState"));
		if (!fnGetKeyState)
			*(std::uint64_t*)&fnGetKeyState = GetProc(xorstr_("user32.dll"), xorstr_("GetKeyState"));

		*(std::uint64_t*)&fnClientToScreen = get_export(xorstr_("user32.dll"), xorstr_("ClientToScreen"));
		if (!fnClientToScreen)
			*(std::uint64_t*)&fnClientToScreen = GetProc(xorstr_("user32.dll"), xorstr_("ClientToScreen"));

		*(std::uint64_t*)&fnSetCursorPos = get_export(xorstr_("user32.dll"), xorstr_("SetCursorPos"));
		if (!fnSetCursorPos)
			*(std::uint64_t*)&fnSetCursorPos = GetProc(xorstr_("user32.dll"), xorstr_("SetCursorPos"));

		*(std::uint64_t*)&fnScreenToClient = get_export(xorstr_("user32.dll"), xorstr_("ScreenToClient"));
		*(std::uint64_t*)&fnGetClientRect = get_export(xorstr_("user32.dll"), xorstr_("GetClientRect"));
		*(std::uint64_t*)&fnTrackMouseEvent = get_export(xorstr_("user32.dll"), xorstr_("TrackMouseEvent"));
		*(std::uint64_t*)&fnSetCapture = get_export(xorstr_("user32.dll"), xorstr_("SetCapture"));
		*(std::uint64_t*)&fnGetCapture = get_export(xorstr_("user32.dll"), xorstr_("GetCapture"));
		*(std::uint64_t*)&fnReleaseCapture = get_export(xorstr_("user32.dll"), xorstr_("ReleaseCapture"));
		*(std::uint64_t*)&fnGetForegroundWindow = get_export(xorstr_("user32.dll"), xorstr_("GetForegroundWindow"));
		*(std::uint64_t*)&fnGetCursorPos = get_export(xorstr_("user32.dll"), xorstr_("GetCursorPos"));

		*(std::uint64_t*)&fnDefWindowProcA = get_export(xorstr_("user32.dll"), xorstr_("DefWindowProcA"));
		*(std::uint64_t*)&fnPeekMessageA = get_export(xorstr_("user32.dll"), xorstr_("PeekMessageA"));
		*(std::uint64_t*)&fnTranslateMessage = get_export(xorstr_("user32.dll"), xorstr_("TranslateMessage"));
		*(std::uint64_t*)&fnDispatchMessageA = get_export(xorstr_("user32.dll"), xorstr_("DispatchMessageA"));
		*(std::uint64_t*)&NtUserGetAsyncKeyState = get_export(xorstr_("win32u.dll"), xorstr_("NtUserGetAsyncKeyState"));
		*(std::uint64_t*)&fnCreateWindowExA = memory::get_export(xorstr_("user32.dll"), xorstr_("CreateWindowExA"));
		*(std::uint64_t*)&fnGetTickCount64 = memory::get_export(xorstr_("kernel32.dll"), xorstr_("GetTickCount64"));
		*(std::uint64_t*)&RtlCreateUserThread = memory::get_export(xorstr_("ntdll.dll"), xorstr_("RtlCreateUserThread"));
		*(std::uint64_t*)&fnSleep = memory::get_export(xorstr_("kernel32.dll"), xorstr_("Sleep"));

		return true;
	}
}