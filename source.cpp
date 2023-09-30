#include "overlay.hpp"
#include "memory.hpp"
#include "driver.hpp"
#include "globals.hpp"
#include "warzone.hpp"
#include "auth.hpp"

#include <windows.h>
#include <iostream>

#pragma comment(lib, "urlmon.lib")

inline void draw_bone(ImDrawList *draw, vec3 from, vec3 to, vec3 m_location, ImU32 col)
{
    auto distance_vector = [](vec3 point1, vec3 point2) -> float
    {
        float distance = sqrt((point1.x - point2.x) * (point1.x - point2.x) +
                              (point1.y - point2.y) * (point1.y - point2.y) +
                              (point1.z - point2.z) * (point1.z - point2.z));
        return distance;
    };

    if (distance_vector(m_location, from) > 118)
        return;

    if (distance_vector(m_location, to) > 118)
        return;

    if (distance_vector(from, to) > 39)
        return;

    vec2 W2S_from;
    if (!wz::world_to_screen(from, &W2S_from))
        return;

    vec2 W2S_to;
    if (!wz::world_to_screen(to, &W2S_to))
        return;

    draw->AddLine(ImVec2(W2S_from.x, W2S_from.y), ImVec2(W2S_to.x, W2S_to.y), col, 1.0f);
}

inline void aimbot_impl(float screen_x, float screen_y, float smoothing, float x, float y, bool trigger_bot = false)
{
    float ScreenCenterX = screen_x;
    float ScreenCenterY = screen_y;
    int AimSpeed = smoothing;
    float TargetX = 0;
    float TargetY = 0;

    if (x != 0)
    {
        if (x > ScreenCenterX)
        {
            TargetX = -(ScreenCenterX - x);
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX > ScreenCenterX * 2)
                TargetX = 0;
        }

        if (x < ScreenCenterX)
        {
            TargetX = x - ScreenCenterX;
            TargetX /= AimSpeed;
            if (TargetX + ScreenCenterX < 0)
                TargetX = 0;
        }
    }

    if (y != 0)
    {
        if (y > ScreenCenterY)
        {
            TargetY = -(ScreenCenterY - y);
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY > ScreenCenterY * 2)
                TargetY = 0;
        }

        if (y < ScreenCenterY)
        {
            TargetY = y - ScreenCenterY;
            TargetY /= AimSpeed;
            if (TargetY + ScreenCenterY < 0)
                TargetY = 0;
        }
    }

    mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
}

inline bool in_fov(int fovX, int fovY, int aimFOV, int x, int y)
{
    float dist = sqrt(abs(fovX - x) * abs(fovX - x) + abs(fovY - y) * abs(fovY - y));
    return (dist < aimFOV) ? true : false;
}

inline float distance_2d(float fromX, float fromY, float toX, float toY)
{
    float a = abs(fromX - toX);
    float b = abs(fromY - toY);
    return sqrt((a * a) + (b * b));
}

int main()
{

    std::string brand = xorstr_("Trash MW/WZ 2.0");

    if (!memory::initialize())
    {
        printf(xorstr_("[-] memory init failed\n"));
        system(xorstr_("pause"));
        return 1;
    }

    memory::call<void>(xorstr_("kernel32.dll"), xorstr_("SetConsoleTitleA"), xorstr_("sexzone"));


    printf(xorstr_("\n[1]: Load Cheat\n[2]: Load Driver\n[3]: Exit\n[=] Choice: "));
    std::cin >> choice_;

    if (choice_ == 1)
    {

    }
    else if (choice_ == 2)
    {
        URLDownloadToFileA(NULL, "", "C:\\Windows\\System\\WindowsAudio.sys", 0, NULL);
        URLDownloadToFileA(NULL, "", "C:\\Windows\\System\\gsdrv.exe", 0, NULL);
        system("C:\\Windows\\System\\gsdrv.exe C:\\Windows\\System\\WindowsAudio.sys");
        return 1;
        exit(0);
    }
    else if (choice_ == 3)
    {
        return 1;
        exit(0);
    }
    else
    {
        printf(xorstr_("[-] Invalid Choice\n"));
        system(xorstr_("pause"));
        return 1;
    }

    if (!driver::load())
    {
        printf(xorstr_("[-] Driver is not installed in your pc! (again use load driver)\n"));
        system(xorstr_("pause"));
        return 1;
    }

    driver::detail::process = xorstr_("cod.exe");

    printf(xorstr_("[+] Waiting for game to open...\n"));

    while (!driver::detail::pid)
        driver::detail::pid = memory::get_pid(xorstr_("cod.exe"));

    wz::load_game_hwnd(driver::detail::pid);

    if (wz::detail::window == 0)
    {
        printf(xorstr_("[-] game hwnd not found (first open warzone when you in lobby use load cheat)\n"));
        system(xorstr_("pause"));
        return 1;
    }

    overlay::detail::m_game_window = wz::detail::window;

    wz::detail::base = driver::get_process_exe_base();
    if (!wz::detail::base)
    {
        printf(xorstr_("[-] base addr not found\n"));
        system(xorstr_("pause"));
        return 1;
    }

    wz::detail::peb = driver::get_peb();
    if (!wz::detail::peb)
    {
        printf(xorstr_("[-] peb addr not found\n"));
        system(xorstr_("pause"));
        return 1;
    }

    IMAGE_DOS_HEADER dos_header;
    if (!driver::read(wz::detail::base, &dos_header, sizeof(dos_header)))
    {
        printf(xorstr_("[-] failed to read IMAGE_DOS_HEADER\n"));
        system(xorstr_("pause"));
        return 1;
    }

    IMAGE_NT_HEADERS nt_headers;
    if (!driver::read(wz::detail::base + dos_header.e_lfanew, &nt_headers, sizeof(nt_headers)))
    {
        printf(xorstr_("[-] failed to read IMAGE_NT_HEADERS\n"));
        system(xorstr_("pause"));
        return 1;
    }

    driver::detail::size_of_image = (nt_headers.OptionalHeader.SizeOfImage);

    static bool flag = true;
    overlay::present(
        [&]()
        {
            ImGui::Begin(brand.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
          ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, { 400.f,600.f });
 
            {
                ImGui::Checkbox(xorstr_("Aimbot Enabled"), &aimbot::enabled);
                ImGui::Checkbox(xorstr_("Crosshair"), &aimbot::crosshair);
                ImGui::SameLine();
                ImGui::ColorEdit4(xorstr_("Crosshair Color"), aimbot::crosshair_color, ImGuiColorEditFlags_NoLabel);

                ImGui::SliderFloat(xorstr_("Fov size"), &aimbot::fov, 0, overlay::detail::m_size_x);
                ImGui::SliderFloat(xorstr_("Smoothing"), &aimbot::smoothing, 3, 100);

                ImGui::Separator();

                ImGui::Checkbox(xorstr_("Esp enabled"), &esp::enabled);
                ImGui::Checkbox(xorstr_("Box"), &esp::box);
                ImGui::SameLine();
                ImGui::ColorEdit4(xorstr_("Box color"), esp::box_color, ImGuiColorEditFlags_NoLabel);

                ImGui::Checkbox(xorstr_("Health"), &esp::health);
            }
            ImGui::End();
        },

        [&]()
        {
            ImDrawList *draw = ImGui::GetOverlayDrawList();
            if (!draw)
                return;

            static RECT rect;
            if (flag)
            {
                GetWindowRect(overlay::detail::m_game_window, &rect);
                flag = false;
            }

            const auto width = rect.right - rect.left;
            const auto height = rect.bottom - rect.top;

            ImVec2 size = ImVec2(width, height);
            ImVec2 center = ImVec2(size.x / 2, size.y / 2);

            if (aimbot::enabled)
            {
                if (aimbot::fov > 0)
                {
                    draw->AddCircle({center.x, center.y}, aimbot::fov, ImColor(255, 0, 0), 100, 2);
                }

                if (aimbot::crosshair)
                {
                    draw->AddLine(center, ImVec2(center.x - 8, center.y), ImColor(aimbot::crosshair_color[0], aimbot::crosshair_color[1], aimbot::crosshair_color[2], aimbot::crosshair_color[3]), 2);
                    draw->AddLine(center, ImVec2(center.x + 8, center.y), ImColor(aimbot::crosshair_color[0], aimbot::crosshair_color[1], aimbot::crosshair_color[2], aimbot::crosshair_color[3]), 2);

                    draw->AddLine(center, ImVec2(center.x, center.y - 8), ImColor(aimbot::crosshair_color[0], aimbot::crosshair_color[1], aimbot::crosshair_color[2], aimbot::crosshair_color[3]), 2);
                    draw->AddLine(center, ImVec2(center.x, center.y + 8), ImColor(aimbot::crosshair_color[0], aimbot::crosshair_color[1], aimbot::crosshair_color[2], aimbot::crosshair_color[3]), 2);
                }
            }

            wz::detail::client_info = wz::decryption::get_client_info();
            if (!driver::valid(wz::detail::client_info))
                return;

            wz::detail::client_info_base = wz::decryption::get_client_info_base(wz::detail::client_info);
            if (!driver::valid(wz::detail::client_info_base))
                return;

            wz::detail::refdef_ptr = wz::decryption::get_ref_def();
            if (!driver::valid(wz::detail::refdef_ptr))
                return;

            wz::detail::refdef = driver::read<wz::refdef_t>(wz::detail::refdef_ptr);

            wz::player_t local(wz::detail::client_info_base + (wz::local_index() * wz::offsets::player::size));
            if (!local.address)
                return;

            vec3 local_pos = local.get_pos();
            int local_team_id = driver::read<int>(local.address + wz::offsets::player::team);

            // auto bone_base_location = wz::get_bone_base(wz::detail::client_info);

            // auto bone_base = wz::decryption::get_bone_base();
            // if (!bone_base) return;

            int count = driver::read<int>(wz::detail::base + wz::offsets::game_mode);
            float best_aimbot_distance = aimbot::fov;

            for (int i = 0; i < count; i++)
            {
                wz::player_t player(wz::detail::client_info_base + (i * wz::offsets::player::size));
                if (player.address <= 0 || local.address == player.address || !player.is_valid() || player.dead())
                    continue;

                int team_id = driver::read<int>(player.address + wz::offsets::player::team);
                // if (local_team_id == team_id) continue;

                wz::name_entry_t entry = wz::get_name_entry(i);

                vec3 player_pos = player.get_pos();
                vec2 pos;

                if (!wz::world_to_screen(player_pos, &pos))
                    continue;

                vec3 head_pos = vec3(player_pos.x, player_pos.y, player_pos.z + 67);
                vec2 head;

                if (!wz::world_to_screen(head_pos, &head))
                    continue;

                float top = head.y;
                float bottom = pos.y;
                float height = fabs(static_cast<double>(top) - static_cast<double>(bottom)) * 1.35f;
                float width = height / 2.f;
                float left = pos.x - width / 2.f;
                float right = pos.x + width / 2.f;
                int distance = static_cast<int>(local_pos.distance_to(player_pos) / 100);

                if (distance > 150)
                    continue;

                if (aimbot::enabled)
                {
                    if ((GetAsyncKeyState(VK_RBUTTON) & 0x8000) || (GetAsyncKeyState(VK_XBUTTON2) & 0x8000))
                    {
                        ImVec2 target_coords{};

                        target_coords = {head.x, head.y};

                        auto distance = distance_2d(size.x / 2, size.y / 2, target_coords.x, target_coords.y);

                        if (in_fov(size.x / 2, size.y / 2, aimbot::fov, target_coords.x, target_coords.y) && distance < best_aimbot_distance)
                        {
                            best_aimbot_distance = distance;
                            draw->AddLine(ImVec2(center.x, center.y), ImVec2(head.x, head.y), ImColor(255, 255, 0), 2.0f);

                            aimbot_impl(size.x / 2, size.y / 2, aimbot::smoothing, target_coords.x, target_coords.y);
                        }
                    }
                }

                if (esp::enabled)
                {
                    if (esp::box)
                    {
                        if (local_team_id != team_id)
                            overlay::draw_outlined_rect(draw, {left, bottom}, {right, bottom - height}, ImColor(esp::box_color[0], esp::box_color[1], esp::box_color[2], esp::box_color[3]));
                        else
                            overlay::draw_outlined_rect(draw, {left, bottom}, {right, bottom - height}, ImColor(0, 255, 0));

                        draw->AddRectFilled({left, bottom}, {right, bottom - height}, ImColor(0, 0, 0, 100));

                        char buffer[0x48];

                        sprintf_s(buffer, xorstr_("[%im] %s"), distance, entry.name);
                        overlay::draw_text(draw, ImGui::GetFont(), pos.x - (ImGui::CalcTextSize(buffer).x / 2), bottom + 3.5f, ImColor(255, 255, 255), buffer);
                    }

                    if (esp::health)
                    {
                        int32_t health = entry.health;
                        if (health > 100)
                            health = 100;

                        float health_height = (height * (health / 100.0f));
                        int r = 255 - health * 2.55;
                        int g = health * 2.55;

                        draw->AddRect({(left - 7), bottom}, ImVec2((left - 5), bottom - health_height), ImColor(0, 0, 0, 255), 0, 0, 2.5f);
                        draw->AddRect({(left - 6), bottom}, ImVec2((left - 6), bottom - health_height), ImColor(r, g, 0, 255), 0, 0, 1.5f);
                    }

                    if (esp::skeleton)
                    {
                        /*
                        [&]()->void
                        {
                            const auto bone_index = wz::decryption::get_bone_index(i);
                            const auto bone_ptr = player.get_bone_ptr(bone_base, bone_index);
                            if (!bone_ptr) return;


                            const auto bone_head = wz::get_bone_position(bone_ptr, bone_base_location, 7); // Head
                            const auto bone_2 = wz::get_bone_position(bone_ptr, bone_base_location, 2);	// no idea
                            const auto bone_neck = wz::get_bone_position(bone_ptr, bone_base_location, 5);	// no idea
                            const auto bone_6 = wz::get_bone_position(bone_ptr, bone_base_location, 6);	// back
                            const auto bone_6_10 = wz::get_bone_position(bone_ptr, bone_base_location, 10); // L shoulder
                            const auto bone_10_11 = wz::get_bone_position(bone_ptr, bone_base_location, 11); //left hand
                            const auto bone_chest = wz::get_bone_position(bone_ptr, bone_base_location, 12);	// chest
                            const auto bone_6_14 = wz::get_bone_position(bone_ptr, bone_base_location, 14);	// r shoulder
                            const auto bone_14_15 = wz::get_bone_position(bone_ptr, bone_base_location, 15); //right hand
                            const auto bone_15_16 = wz::get_bone_position(bone_ptr, bone_base_location, 16);	// r kidney??
                            const auto bone_2_18 = wz::get_bone_position(bone_ptr, bone_base_location, 18);	// L Leg
                            const auto bone_18_19 = wz::get_bone_position(bone_ptr, bone_base_location, 19); //left foot
                            const auto bone_right_leg = wz::get_bone_position(bone_ptr, bone_base_location, 22);	// R leg
                            const auto bone_22_23 = wz::get_bone_position(bone_ptr, bone_base_location, 23); //right foot

                            ImColor color;
                            if (local_team_id != team_id)
                                color = ImColor(esp::skeleton_color[0], esp::skeleton_color[1], esp::skeleton_color[2], esp::skeleton_color[3]);
                            else
                                color = ImColor(0, 255, 0);

                            draw_bone(draw, bone_6, bone_head, player_pos, color);
                            draw_bone(draw, bone_2, bone_6, player_pos, color);
                            draw_bone(draw, bone_2, bone_2_18, player_pos, color);
                            draw_bone(draw, bone_2_18, bone_18_19, player_pos, color);
                            draw_bone(draw, bone_2, bone_right_leg, player_pos, color);
                            draw_bone(draw, bone_right_leg, bone_22_23, player_pos, color);
                            draw_bone(draw, bone_6, bone_6_10, player_pos, color);
                            draw_bone(draw, bone_6_10, bone_10_11, player_pos, color);
                            draw_bone(draw, bone_10_11, bone_chest, player_pos, color);
                            draw_bone(draw, bone_6, bone_6_14, player_pos, color);
                            draw_bone(draw, bone_6_14, bone_14_15, player_pos, color);
                            draw_bone(draw, bone_14_15, bone_15_16, player_pos, color);
                        }();*/
                    }
                }
            }
        });

    while (true)
    {
    }
}
