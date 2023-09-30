#pragma once

#include "includes.hpp"

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx9.h"
#include <iostream>
#include <stdlib.h>


#define COLOR(R,G,B) D3DCOLOR_RGBA(R,G,B,255)

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND, UINT, WPARAM, LPARAM);

namespace overlay {

    inline bool g_open = false;

	namespace detail {
        inline HWND m_window = 0;
        inline HWND m_game_window = 0;
        inline RECT m_rect;

        inline float m_size_x = 1920;
        inline float m_size_y = 1080;
        inline float m_padding = 2;

        inline IDirect3DDevice9Ex* m_device = nullptr;
        inline IDirect3D9Ex* m_object = nullptr;
        inline D3DPRESENT_PARAMETERS m_params;
        inline ID3DXLine* m_line = nullptr;
        inline ID3DXFont* m_font = nullptr;
        inline ImFont* m_courbd = nullptr;

        inline DWORD get_process_id(LPCSTR string) {
            DWORD id = 0;
            HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (handle)
            {
                PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };

                if (Process32First(handle, &pe32)) {
                    while (Process32Next(handle, &pe32)) {
                        if (!strcmp(pe32.szExeFile, string)) {
                            id = (DWORD)(pe32.th32ProcessID);
                            break;
                        }
                    }
                }
            }

            CloseHandle(handle);
            return id;
        }

        inline LRESULT window_process(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
        {

            if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
                return true;

            switch (Message)
            {
            case WM_DESTROY:
                if (m_device != 0)
                {
                    m_device->EndScene();
                    m_device->Release();
                }

                if (m_object != 0)
                    m_object->Release();

                PostQuitMessage(0);
                exit(4);
                break;
            case WM_SIZE:
                if (m_device != NULL && wParam != SIZE_MINIMIZED)
                {
                    ImGui_ImplDX9_InvalidateDeviceObjects();
                    m_params.BackBufferWidth = LOWORD(lParam);
                    m_params.BackBufferHeight = HIWORD(lParam);
                    m_device->Reset(&m_params);
                    ImGui_ImplDX9_CreateDeviceObjects();
                }

                break;
            default:
                return DefWindowProcA(hWnd, Message, wParam, lParam);
                break;
            }
            return 0;
        }

        inline bool load_window() 
        {
            static std::string title = []() -> std::string 
            {
                static std::string chars = xorstr_("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz");
                std::string result = {};

                srand((unsigned)time(0));
                for (int i = 0; i < 16; i++)
                    result += chars[rand() % chars.length()];
                Sleep(500);
                return result;
            }();

            WNDCLASSEX window_class =
            {
                sizeof(WNDCLASSEX),
                0,
                window_process,
                0,
                0,
                0,
                LoadIconA(0, IDI_APPLICATION),
                LoadCursorA(0, IDC_ARROW),
                0,
                0,
                title.c_str(),
                LoadIconA(0, IDI_APPLICATION)
            };

            RegisterClassExA(&window_class);

            if (m_game_window)
            {
                POINT coords;
                GetClientRect(m_game_window, &m_rect);           
                ClientToScreen(m_game_window, &coords);
                m_rect.left = coords.x;
                m_rect.top = coords.y;

                m_size_x = m_rect.right;
                m_size_y = m_rect.bottom;

                if (m_size_x <= 0 || m_size_y <= 0)
                {
                    m_size_x = GetSystemMetrics(SM_CXSCREEN);
                    m_size_y = GetSystemMetrics(SM_CYSCREEN);
                }
                
                if (m_size_x <= 0)
                {
                    int size_x;

                    printf("Enter screen size X: ");
                    std::cin >> size_x;
                    m_size_x = size_x;
                }

                if (m_size_y <= 0)
                {
                    int size_y;

                    printf("Enter screen size Y: ");
                    std::cin >> size_y;
                    m_size_x = size_y;
                }
                printf("Menu Loaded Successfully . Open key is insert");
                printf("Your Screen Size = %f, %f\n", m_size_x, m_size_y);
            }
            else
            {
                MessageBoxA(0, xorstr_("Failed to find game window."), xorstr_("UserId issue"), 0);
                return false;
            }

            m_window = CreateWindowExA(0, window_class.lpszClassName, title.c_str(), WS_POPUP | WS_VISIBLE, m_rect.left - m_padding, m_rect.top - m_padding, m_size_x + m_padding, m_size_y + m_padding, 0, 0, 0, 0);
            if (!m_window) return false;

            MARGINS Margin = { m_rect.left - m_padding, m_rect.top - m_padding, m_size_x + m_padding, m_size_y + m_padding };
            DwmExtendFrameIntoClientArea(m_window, &Margin);

            SetWindowLongA(m_window, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
            ShowWindow(m_window, SW_SHOW);
            UpdateWindow(m_window);

            return true;
        }

        inline bool load_direct_x()
        {
            if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &m_object)))
                return false;

            memset(&m_params, 0, sizeof(m_params));

            m_params.Windowed = TRUE;
            m_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
            m_params.hDeviceWindow = m_window;
            m_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
            m_params.BackBufferFormat = D3DFMT_A8R8G8B8;
            m_params.BackBufferWidth = m_size_x + m_padding;
            m_params.BackBufferHeight = m_size_y + m_padding;
            m_params.EnableAutoDepthStencil = TRUE;
            m_params.AutoDepthStencilFormat = D3DFMT_D16;
            m_params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
            m_params.MultiSampleQuality = DEFAULT_QUALITY;

            if (FAILED(m_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &m_params, 0, &m_device)))
            {
                m_object->Release();
                return false;
            }

            ImGui::CreateContext();

            m_courbd = ImGui::GetIO().Fonts->AddFontFromFileTTF(xorstr_("C:\\Windows\\Fonts\\courbd.ttf"), 14.5f);
            ImGui::GetIO().Fonts->AddFontDefault();

            ImGui_ImplWin32_Init(m_window);
            ImGui_ImplDX9_Init(m_device);

            m_object->Release();

            return true;
        }

        inline void unload_direct_x()
        {
            m_device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.f, 0);
            m_device->BeginScene();
            m_device->EndScene();
            m_device->PresentEx(0, 0, 0, 0, 0);

            m_object->Release();
            m_device->Release();
        }

        inline void draw_shadow_text(const char* String, int x, int y, int offset, D3DCOLOR color, D3DCOLOR shadowColor, ID3DXFont* font, DWORD format)
        {
            RECT FontPos;
            RECT shadowPos;
            FontPos.left = x;
            FontPos.top = y;
            shadowPos.left = x - offset;
            shadowPos.top = y + offset;
            font->DrawTextA(0, String, strlen(String), &shadowPos, format, shadowColor);
        }

        inline void draw_text(const char* format, int x, int y, D3DCOLOR color)
        {
            draw_shadow_text(format, x, y, 1, color, color, m_font, DT_NOCLIP);
        }
	}
 
    inline void draw_text(ImDrawList* draw, ImFont* font, int x, int y, ImColor color, const char* str)
    {
        if (font == nullptr)
        {

            draw->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(ImVec2(x, y), color, str);
        }
        else
        {
            draw->AddText(font, font->FontSize, ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(font, font->FontSize, ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(font, font->FontSize, ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(font, font->FontSize, ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), str);
            draw->AddText(font, font->FontSize, ImVec2(x, y), color, str);
        }
    }

    inline void draw_corner_box(ImDrawList* drawlist, ImVec2 pos, ImVec2 dim, ImColor col, int thick)
    {
        if (!drawlist) return;

        float cornersizex = dim.x / 3;
        float cornersizey = dim.y / 4;

        drawlist->AddLine(ImVec2(pos), ImVec2(pos.x - cornersizex, pos.y), col, thick);
        drawlist->AddLine(ImVec2(pos.x - dim.x, pos.y), ImVec2((pos.x - dim.x) + cornersizex, pos.y), col, thick);
        drawlist->AddLine(ImVec2(pos.x - dim.x, pos.y), ImVec2(pos.x - dim.x, pos.y - cornersizey), col, thick);
        drawlist->AddLine(ImVec2(pos.x - dim.x, (pos.y - dim.y)), ImVec2(pos.x - dim.x, (pos.y - dim.y) + cornersizey), col, thick);
        drawlist->AddLine(ImVec2(pos.x - dim.x, (pos.y - dim.y)), ImVec2((pos.x - dim.x) + cornersizex, pos.y - dim.y), col, thick);
        drawlist->AddLine(ImVec2(pos.x, (pos.y - dim.y)), ImVec2(pos.x - cornersizex, pos.y - dim.y), col, thick);
        drawlist->AddLine(ImVec2(pos.x, (pos.y - dim.y)), ImVec2(pos.x, (pos.y - dim.y) + cornersizey), col, thick);
        drawlist->AddLine(ImVec2(pos.x, pos.y), ImVec2(pos.x, pos.y - cornersizey), col, thick);
    }

    inline void draw_outlined_rect(ImDrawList* draw_list, const ImVec2& pos_start, const ImVec2& pos_end, const ImColor& foreground_color = ImColor(255, 255, 255, 255), const ImColor& background_color = ImColor(0, 0, 0, 255), const float& offset = 1.f, const float& thickness = 1.f, const float& rounding = 0.f, const ImDrawListFlags& flags = 0)
    {
        draw_list->AddRect(ImVec2(pos_start.x - offset * thickness, pos_start.y - offset * thickness), ImVec2(pos_end.x + offset * thickness, pos_end.y + offset * thickness), background_color, rounding, flags, thickness);
        draw_list->AddRect(ImVec2(pos_start.x + offset * thickness, pos_start.y + offset * thickness), ImVec2(pos_end.x - offset * thickness, pos_end.y - offset * thickness), background_color, rounding, flags, thickness);
        draw_list->AddRect(pos_start, pos_end, foreground_color, rounding, flags, thickness);
    }

    inline void present(std::function<void()> callback, std::function<void()> thread_callback)
    {
        if (!detail::load_window())
        {
            MessageBoxW(0, xorstr_(L"Error #1"), 0, 0);
            return;
        }

        if (!detail::load_direct_x())
        {
            MessageBoxW(0, xorstr_(L"Error #2"), 0, 0);
            return;
        }

        static RECT old_rc;
        static MSG current_message;

        memset(&current_message, 0, sizeof(MSG));
        while (current_message.message != WM_QUIT)
        {
            if (PeekMessageA(&current_message, detail::m_window, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&current_message);
                DispatchMessageA(&current_message);
            }

            HWND hwnd_active = GetForegroundWindow();

            if (GetAsyncKeyState(VK_END) & 1)
                exit(8);

            if (hwnd_active == detail::m_game_window)
            {
                HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
                SetWindowPos(detail::m_window, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }

            RECT rc;
            POINT xy;

            ZeroMemory(&rc, sizeof(RECT));
            ZeroMemory(&xy, sizeof(POINT));
            GetClientRect(detail::m_game_window, &rc);
            ClientToScreen(detail::m_game_window, &xy);
            rc.left = xy.x;
            rc.top = xy.y;

            ImGuiIO& io = ImGui::GetIO();
            io.ImeWindowHandle = detail::m_game_window;
            io.DeltaTime = 1.0f / 144.0f;

            POINT p;
            GetCursorPos(&p);
            io.MousePos.x = p.x - xy.x;
            io.MousePos.y = p.y - xy.y;

            if (GetAsyncKeyState(VK_LBUTTON)) {
                io.MouseDown[0] = true;
                io.MouseClicked[0] = true;
                io.MouseClickedPos[0].x = io.MousePos.x;
                io.MouseClickedPos[0].x = io.MousePos.y;
            }
            else
                io.MouseDown[0] = false;

            if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
            {
                old_rc = rc;
                Sleep(3);
                detail::m_size_x += rc.right;
                Sleep(1);
                detail::m_size_y = rc.bottom;

                detail::m_params.BackBufferWidth = detail::m_size_x + detail::m_padding;
                detail::m_params.BackBufferHeight = detail::m_size_y + detail::m_padding;
                SetWindowPos(detail::m_window, 0, xy.x, xy.y, detail::m_size_x + detail::m_padding, detail::m_size_y + detail::m_padding, SWP_NOREDRAW);
                detail::m_device->Reset(&detail::m_params);
            }


            ImGui_ImplDX9_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            HWND active_hwnd = GetForegroundWindow();
            if (active_hwnd == detail::m_window)
            {
                HWND window = GetWindow(active_hwnd, GW_HWNDPREV);
                SetWindowPos(detail::m_window, window, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
            }

            if (GetAsyncKeyState(VK_INSERT) & 1)
            {
                g_open ^= true;
                SetWindowLongA(detail::m_window, GWL_EXSTYLE, (g_open ? WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW : WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW));
            }

            ImGui::GetStyle().WindowRounding = 0;
            ImGui::GetStyle().FrameRounding = 0;

            if (g_open)
            {
                callback();
            }

            thread_callback();
        
            ImGui::EndFrame();

            detail::m_device->SetRenderState(D3DRS_ZENABLE, false);
            detail::m_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
            detail::m_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);

            detail::m_device->Clear(0, 0, D3DCLEAR_TARGET, 0, 1.0f, 0);
            if (detail::m_device->BeginScene() >= 0)
            {
                ImGui::Render();
                ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
                detail::m_device->EndScene();
            }

            if (detail::m_device->Present(0, 0, 0, 0) == D3DERR_DEVICELOST && detail::m_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            {
                ImGui_ImplDX9_InvalidateDeviceObjects();
                detail::m_device->Reset(&detail::m_params);
                ImGui_ImplDX9_CreateDeviceObjects();
            }
        }
  
        detail::unload_direct_x();
    }
}