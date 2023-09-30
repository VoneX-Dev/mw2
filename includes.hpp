#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <thread>
#include <vector>
#include <cstdio>
#include <cstdint>
#include <functional>
#include <string>
#include <d3d9.h>
#include <random>
#include <xorstr.hpp>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <sddl.h>

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <dwmapi.h>
#include <time.h>
#pragma comment(lib, "dwmapi")

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

#include <d3dx9core.h>
#pragma comment(lib, "d3dx9.lib")

#include "callstack.hpp"
#include "vectors.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);