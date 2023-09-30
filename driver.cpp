#include "driver.hpp"
#include "memory.hpp"


#define KE_MAGIC 0x4040

namespace driver
{
	enum : UINT32
	{
		KERNEL_ID_WRITE,
		KERNEL_ID_READ,
		KERNEL_ID_MODULE,
		KERNEL_ID_GET_PROCESS_EXE_BASE,
		KERNEL_ID_RUNNING,
		KERNEL_GET_PROCESS_PEB
	};

	typedef struct _KERNEL_REQUEST
	{
		UINT32 magic; // 0x4040
		UINT32 id;
		PVOID pid;
		PVOID dst;
		PVOID out;
		ULONGLONG size;
		BOOLEAN physhical;
		LPCWSTR name;
	} KERNEL_REQUEST, * PKERNEL_REQUEST;

	namespace detail
	{
		__int64(*usermode_caller)(__int64, __int64, __int64, unsigned int, __int64, __int64, char, void*, void*) = 0;

		template<typename T>
		bool send(T& args)
		{
			if (!function) return false;

			int(*usermode_caller)(__int64 a1, __int64 a2, __int64 a3, __int64 a4, int a5, __int64 a6, __int64 a7, __int64 a8, void* a9, void* a10) = 0;
			usermode_caller = (decltype(usermode_caller))function;

			NTSTATUS status;
			SPOOF_CALL(usermode_caller)(0x69, 0x69, 0x69, 0x69, 0x69, 0x69, 0x69, 0x69, &args, &status);
			return (status == 0ull);
		}
	}

	bool running()
	{
		_KERNEL_REQUEST message;
		message.magic = KE_MAGIC;
		message.id = KERNEL_ID_RUNNING;

		detail::send(message);
		return (message.out == (PVOID)0x2000);
	}

	bool load()
	{
		*(std::uint64_t*)&detail::function = memory::get_export(xorstr_("win32u.dll"), xorstr_("NtGdiGetUFIPathname"));
		if (!detail::function) return false;

		detail::usermode_caller = reinterpret_cast<decltype(detail::usermode_caller)>(detail::function);
		if (!detail::usermode_caller) return false;

		return running();
	}

	bool attach(const char* process_name)
	{
		detail::pid = memory::get_pid(process_name);
		return (detail::pid != 0);
	}

	bool read(std::uint64_t dst, void* src, ULONGLONG size)
	{
		if (!dst || !src || size <= 0)
			return false;

		_KERNEL_REQUEST message;
		message.magic = KE_MAGIC;
		message.id = KERNEL_ID_READ;
		message.pid = (PVOID)detail::pid;
		message.dst = (PVOID)dst;
		message.out = src;
		message.size = size;
		message.physhical = FALSE;

		return detail::send(message);
	}

	bool write(std::uint64_t dst, void* src, ULONGLONG size)
	{
		if (!dst || !src || size <= 0)
			return false;

		_KERNEL_REQUEST message;
		message.magic = KE_MAGIC;
		message.id = KERNEL_ID_WRITE;
		message.pid = (PVOID)detail::pid;
		message.dst = (PVOID)dst;
		message.out = src;
		message.size = size;
		message.physhical = FALSE;

		return detail::send(message);
	}

	std::string read_str(std::uint64_t dst)
	{
		if (!dst)
			return xorstr_("**invalid**");

		char buf[256];
		return (read(dst, &buf, sizeof(buf)) ? std::string(buf) : xorstr_("**invalid**"));
	}

	std::uint64_t module(LPCWSTR module_name)
	{
		_KERNEL_REQUEST message;
		message.magic = KE_MAGIC;
		message.id = KERNEL_ID_MODULE;
		message.pid = (PVOID)detail::pid;
		message.name = module_name;

		return (detail::send(message) ? (std::uint64_t)message.out : 0);
	}

	bool valid(std::uint64_t dst)
	{
		static const std::uint64_t max_range = 0x7fffffffffffffff; // (detail::base + detail::size_of_image);
		return (dst > 0x1000 && dst < max_range);
	}

	std::uint64_t get_process_exe_base()
	{
		_KERNEL_REQUEST message;
		message.magic = KE_MAGIC;
		message.id = KERNEL_ID_GET_PROCESS_EXE_BASE;
		message.pid = (PVOID)detail::pid;

		return (detail::send(message) ? (std::uint64_t)message.out : 0);
	}

	std::uint64_t get_peb()
	{
		_KERNEL_REQUEST message;
		message.magic = KE_MAGIC;
		message.id = KERNEL_GET_PROCESS_PEB;
		message.pid = (PVOID)detail::pid;

		return (detail::send(message) ? (std::uint64_t)message.out : 0);
	}
}