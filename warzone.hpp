#pragma once

namespace wz
{
    struct ref_def_view
    {
        vec2 tan_half_fov;
        char pad[0xC];
        vec3 axis[3];
    };

    class refdef_t
    {
    public:
        int x;
        int y;
        int width;
        int height;
        ref_def_view view;
    };

    struct name_entry_t
    {
        uint32_t index;
        char name[36];
        uint8_t pad[92];
        int32_t health;
        uint8_t pad2[70];
    };

    namespace detail
    {
        inline HWND window = 0;
        inline std::uint64_t base = 0;
        inline std::uint64_t peb = 0;
        inline std::uint64_t client_info = 0;
        inline std::uint64_t client_info_base = 0;
        inline std::uint64_t refdef_ptr = 0;
        inline refdef_t refdef;

        BOOL CALLBACK enum_windows(HWND hwnd, LPARAM param)
        {
            DWORD process_id;
            GetWindowThreadProcessId(hwnd, &process_id);
            if (process_id == param)
            {
                window = hwnd;
                return false;
            }
            return true;
        }
    }

    void load_game_hwnd(DWORD process_id)
    {
        EnumWindows(detail::enum_windows, (LPARAM)process_id);
    }

    namespace offsets
    {
        auto ref_def_ptr = 0x12904080;
        auto name_array = 0x12921F48;
        auto name_array_pos = 0x5E70;
        auto loot_ptr = 0xFFFF8008A4702284;
        auto camera_base = 0x12FB5930;
        auto camera_pos = 0x1F8;
        auto local_index = 0x2CD98;
        auto local_index_pos = 0x2E0;
        auto game_mode = 0xFD2B260;
        auto weapon_definitions = 0x12881070;
        auto distribute = 0xBC85ED8;
        auto visible_offset = 0xA80;
        auto visible = 0x24B4CD0;
        namespace player {
            auto size = 0x64A8;
            auto valid = 0x350;
            auto pos = 0x340;
            auto team = 0x1100;
            auto weapon_index = 0x2B8;
            auto dead_1 = 0x236;
            auto dead_2 = 0x2A8;
        }
        namespace bone {
            auto bone_base = 0x170F08;
            auto size = 0x188;
            auto offset = 0xD8;
        }
    }

    namespace decryption
    {
    uint64_t get_client_info()
    {
        const uint64_t mb = detail::base;
        uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
        rbx = driver::read<uintptr_t>(detail::base + 0x12903688);
        if (!rbx)
            return rbx;
        rdx = detail::peb;              //mov rdx, gs:[rax]
        rcx = detail::base + 0x399D;          //lea rcx, [0xFFFFFFFFFDF5199F]
        rax = rdx;              //mov rax, rdx
        rax = ~rax;             //not rax
        rax ^= rcx;             //xor rax, rcx
        rbx += rax;             //add rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0xF;            //shr rax, 0x0F
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0x1E;           //shr rax, 0x1E
        rbx ^= rax;             //xor rbx, rax
        rax = rbx;              //mov rax, rbx
        rax >>= 0x3C;           //shr rax, 0x3C
        rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
        rax ^= rbx;             //xor rax, rbx
        rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
        rcx ^= driver::read<uintptr_t>(detail::base + 0xA5FB0F3);             //xor rcx, [0x00000000085490A8]
        rbx = detail::base + 0xF087;          //lea rbx, [0xFFFFFFFFFDF5D035]
        rbx = ~rbx;             //not rbx
        rcx = _byteswap_uint64(rcx);            //bswap rcx
        rax *= driver::read<uintptr_t>(rcx + 0xb);              //imul rax, [rcx+0x0B]
        rax += rdx;             //add rax, rdx
        rbx += rax;             //add rbx, rax
        rax = 0xC55A8F4B0168A119;               //mov rax, 0xC55A8F4B0168A119
        rbx *= rax;             //imul rbx, rax
        rax = 0x7D80121784370D3;                //mov rax, 0x7D80121784370D3
        rbx -= rax;             //sub rbx, rax
        return rbx;
        }

        inline uint64_t get_client_info_base(std::uint64_t client_info)
        {
            const uint64_t mb = detail::base;
            uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
            rdx = driver::read<uintptr_t>(client_info + 0x18ccb0);
            if (!rdx)
                return rdx;
            rbx = detail::peb;              //mov rbx, gs:[rax]
            rax = rbx;              //mov rax, rbx
            rax >>= 0xF;            //shr rax, 0x0F
            rax &= 0xF;
            switch (rax) {
            case 0:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4DF03]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB132);               //mov r9, [0x0000000008548FE6]
                rax = 0xFFFFFFFFFFFFAC30;               //mov rax, 0xFFFFFFFFFFFFAC30
                rax -= rbx;             //sub rax, rbx
                rax -= r11;             //sub rax, r11
                rdx += rax;             //add rdx, rax
                rdx += rbx;             //add rdx, rbx
                rax = rdx;              //mov rax, rdx
                rax >>= 0x20;           //shr rax, 0x20
                rdx ^= rax;             //xor rdx, rax
                rax = detail::base + 0x16339D83;              //lea rax, [0x00000000142878C0]
                rdx -= rbx;             //sub rdx, rbx
                rdx += rax;             //add rdx, rax
                rax = 0x34FE54DF24F95401;               //mov rax, 0x34FE54DF24F95401
                rdx *= rax;             //imul rdx, rax
                rax = 0xABF4FC571867234F;               //mov rax, 0xABF4FC571867234F
                rdx *= rax;             //imul rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = 0x3DC36396684685DE;               //mov rax, 0x3DC36396684685DE
                rdx += rax;             //add rdx, rax
                return rdx;
            }
            case 1:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x0000000008548B8D]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x14;           //shr rax, 0x14
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x28;           //shr rax, 0x28
                rdx ^= rax;             //xor rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x7;            //shr rax, 0x07
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xE;            //shr rax, 0x0E
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1C;           //shr rax, 0x1C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x38;           //shr rax, 0x38
                rdx ^= rax;             //xor rdx, rax
                rax = 0x799FC229D26FD843;               //mov rax, 0x799FC229D26FD843
                rdx ^= rax;             //xor rdx, rax
                rdx -= rbx;             //sub rdx, rbx
                rax = detail::base + 0x107067C4;              //lea rax, [0x000000000E653D5F]
                rax = ~rax;             //not rax
                rax *= rbx;             //imul rax, rbx
                rdx ^= rax;             //xor rdx, rax
                rax = 0x973818191B386276;               //mov rax, 0x973818191B386276
                rdx ^= rax;             //xor rdx, rax
                rax = 0xA3DF9FD351EBDDB;                //mov rax, 0xA3DF9FD351EBDDB
                rdx *= rax;             //imul rdx, rax
                return rdx;
            }
            case 2:
            {
                rcx = detail::base + 0x43F0209A;              //lea rcx, [0x0000000041E4F60E]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x0000000008548661]
                rax = 0xFE7178F22BAFE149;               //mov rax, 0xFE7178F22BAFE149
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xE;            //shr rax, 0x0E
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1C;           //shr rax, 0x1C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x38;           //shr rax, 0x38
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x24;           //shr rax, 0x24
                rax ^= rbx;             //xor rax, rbx
                rdx ^= rax;             //xor rdx, rax
                rax = detail::base + 0x4BE48BAB;              //lea rax, [0x0000000049D95F4A]
                rdx ^= rax;             //xor rdx, rax
                rax = 0x7BD7F9DD7E66362C;               //mov rax, 0x7BD7F9DD7E66362C
                rdx += rax;             //add rdx, rax
                rdx ^= rbx;             //xor rdx, rbx
                rdx ^= rcx;             //xor rdx, rcx
                rax = 0x398DA43CA65B466B;               //mov rax, 0x398DA43CA65B466B
                rdx -= rax;             //sub rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                return rdx;
            }
            case 3:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4D032]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB132);               //mov r9, [0x00000000085480D6]
                rdx += rbx;             //add rdx, rbx
                rdx += r11;             //add rdx, r11
                rax = rdx;              //mov rax, rdx
                rax >>= 0xE;            //shr rax, 0x0E
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1C;           //shr rax, 0x1C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x38;           //shr rax, 0x38
                rdx ^= rax;             //xor rdx, rax
                rax = 0x748F27428321F4CA;               //mov rax, 0x748F27428321F4CA
                rdx += rax;             //add rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rax = driver::read<uintptr_t>(rax + 0xf);               //mov rax, [rax+0x0F]
                uintptr_t RSP_0xFFFFFFFFFFFFFF90;
                RSP_0xFFFFFFFFFFFFFF90 = 0x3306C43739DCDB9F;            //mov rax, 0x3306C43739DCDB9F : RBP+0xFFFFFFFFFFFFFF90
                rax *= RSP_0xFFFFFFFFFFFFFF90;          //imul rax, [rbp-0x70]
                rdx *= rax;             //imul rdx, rax
                rdx ^= r11;             //xor rdx, r11
                rax = 0x2B0C59F0EF51A2F2;               //mov rax, 0x2B0C59F0EF51A2F2
                rdx -= rax;             //sub rdx, rax
                return rdx;
            }
            case 4:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4CAC6]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x0000000008547BA7]
                rax = 0x74064666C9A66EE;                //mov rax, 0x74064666C9A66EE
                rdx += rax;             //add rdx, rax
                rdx ^= rbx;             //xor rdx, rbx
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = r11 + 0x16a4bb06;                 //lea rax, [r11+0x16A4BB06]
                rax += rbx;             //add rax, rbx
                rdx += rax;             //add rdx, rax
                rcx = rbx + r11 * 1;            //lea rcx, [rbx+r11*1]
                rax = 0xAE16C0D2EE90771E;               //mov rax, 0xAE16C0D2EE90771E
                rdx += rax;             //add rdx, rax
                rdx += rcx;             //add rdx, rcx
                rax = rdx;              //mov rax, rdx
                rax >>= 0x27;           //shr rax, 0x27
                rdx ^= rax;             //xor rdx, rax
                rax = 0x18E6D244945B0967;               //mov rax, 0x18E6D244945B0967
                rdx *= rax;             //imul rdx, rax
                return rdx;
            }
            case 5:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4C66E]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB132);               //mov r9, [0x000000000854771B]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x14;           //shr rax, 0x14
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x28;           //shr rax, 0x28
                rdx ^= rax;             //xor rdx, rax
                rax = 0xF223F57AE36CD02;                //mov rax, 0xF223F57AE36CD02
                rdx ^= rax;             //xor rdx, rax
                rax = 0x4B70E78C32B841FD;               //mov rax, 0x4B70E78C32B841FD
                rdx *= rax;             //imul rdx, rax
                rdx += rbx;             //add rdx, rbx
                rax = 0xF0EF7B938A3E4356;               //mov rax, 0xF0EF7B938A3E4356
                rdx ^= rax;             //xor rdx, rax
                rdx ^= r11;             //xor rdx, r11
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = 0xFFFFFFFFFFFFA6E7;               //mov rax, 0xFFFFFFFFFFFFA6E7
                rax -= rbx;             //sub rax, rbx
                rax -= r11;             //sub rax, r11
                rdx += rax;             //add rdx, rax
                return rdx;
            }
            case 6:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x00000000085471F2]
                rax = rbx;              //mov rax, rbx
                uintptr_t RSP_0x68;
                RSP_0x68 = detail::base + 0xC221;             //lea rax, [0xFFFFFFFFFDF58368] : RSP+0x68
                rax ^= RSP_0x68;                //xor rax, [rsp+0x68]
                rdx += rax;             //add rdx, rax
                rdx += rbx;             //add rdx, rbx
                rax = rdx;              //mov rax, rdx
                rax >>= 0x17;           //shr rax, 0x17
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x2E;           //shr rax, 0x2E
                rdx ^= rax;             //xor rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x11;           //shr rax, 0x11
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x22;           //shr rax, 0x22
                rdx ^= rax;             //xor rdx, rax
                rax = 0xB3BA39DA276C253;                //mov rax, 0xB3BA39DA276C253
                rdx *= rax;             //imul rdx, rax
                rax = 0xF0AA23067E8E49D2;               //mov rax, 0xF0AA23067E8E49D2
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 7:
            {
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB132);               //mov r9, [0x0000000008546C8C]
                rax = detail::base + 0xCDA3;          //lea rax, [0xFFFFFFFFFDF584A0]
                rax = ~rax;             //not rax
                rax *= rbx;             //imul rax, rbx
                rdx += rax;             //add rdx, rax
                rax = 0x2F6EC3AC5CCD2FD9;               //mov rax, 0x2F6EC3AC5CCD2FD9
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x11;           //shr rax, 0x11
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x22;           //shr rax, 0x22
                rdx ^= rax;             //xor rdx, rax
                rdx -= rbx;             //sub rdx, rbx
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x4;            //shr rax, 0x04
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x8;            //shr rax, 0x08
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x10;           //shr rax, 0x10
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x20;           //shr rax, 0x20
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x2;            //shr rax, 0x02
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x4;            //shr rax, 0x04
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x8;            //shr rax, 0x08
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x10;           //shr rax, 0x10
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x20;           //shr rax, 0x20
                rdx ^= rax;             //xor rdx, rax
                rax = detail::base + 0x287B6DE3;              //lea rax, [0x0000000026702597]
                rdx += rbx;             //add rdx, rbx
                rdx += rax;             //add rdx, rax
                return rdx;
            }
            case 8:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x000000000854671E]
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4B5CD]
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = detail::base + 0x58A2633E;              //lea rax, [0x0000000056971443]
                rdx -= rbx;             //sub rdx, rbx
                rdx += rax;             //add rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x21;           //shr rax, 0x21
                rdx ^= rax;             //xor rdx, rax
                rcx = detail::base + 0xA8D6;          //lea rcx, [0xFFFFFFFFFDF558AD]
                rcx = ~rcx;             //not rcx
                rax = detail::base + 0x4AD18F47;              //lea rax, [0x0000000048C63EF5]
                rax = ~rax;             //not rax
                rcx += rax;             //add rcx, rax
                rcx *= rbx;             //imul rcx, rbx
                rdx += rcx;             //add rdx, rcx
                rax = r11 + 0x20bc;             //lea rax, [r11+0x20BC]
                rax += rbx;             //add rax, rbx
                rdx += rax;             //add rdx, rax
                rax = 0x64A324E596E5EDA7;               //mov rax, 0x64A324E596E5EDA7
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x6;            //shr rax, 0x06
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xC;            //shr rax, 0x0C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x18;           //shr rax, 0x18
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x30;           //shr rax, 0x30
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 9:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4AF84]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x0000000008546073]
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rax = driver::read<uintptr_t>(rax + 0xf);               //mov rax, [rax+0x0F]
                uintptr_t RSP_0x70;
                RSP_0x70 = 0x4017AE78A711DDE9;          //mov rax, 0x4017AE78A711DDE9 : RSP+0x70
                rax *= RSP_0x70;                //imul rax, [rsp+0x70]
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x12;           //shr rax, 0x12
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x24;           //shr rax, 0x24
                rdx ^= rax;             //xor rdx, rax
                rax = 0xB4D94C204C387845;               //mov rax, 0xB4D94C204C387845
                rdx *= rax;             //imul rdx, rax
                rdx ^= rbx;             //xor rdx, rbx
                rdx ^= r11;             //xor rdx, r11
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1;            //shr rax, 0x01
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x2;            //shr rax, 0x02
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x4;            //shr rax, 0x04
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x8;            //shr rax, 0x08
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x10;           //shr rax, 0x10
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x20;           //shr rax, 0x20
                rdx ^= rax;             //xor rdx, rax
                rax = 0x93FEE9AF2A75E7AC;               //mov rax, 0x93FEE9AF2A75E7AC
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 10:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x0000000008545C13]
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4AACE]
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rdx += r11;             //add rdx, r11
                rcx = detail::base + 0xB50F;          //lea rcx, [0xFFFFFFFFFDF55CA0]
                rcx = ~rcx;             //not rcx
                rax = 0x62005F47151C88B7;               //mov rax, 0x62005F47151C88B7
                rcx ^= rbx;             //xor rcx, rbx
                rdx += rax;             //add rdx, rax
                rdx += rcx;             //add rdx, rcx
                rax = 0xD2492AC1B22E0511;               //mov rax, 0xD2492AC1B22E0511
                rdx *= rax;             //imul rdx, rax
                rax = 0x4C4EEBF1D0EDEA52;               //mov rax, 0x4C4EEBF1D0EDEA52
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x12;           //shr rax, 0x12
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x24;           //shr rax, 0x24
                rdx ^= rax;             //xor rdx, rax
                rax = r11 + 0x60cacb04;                 //lea rax, [r11+0x60CACB04]
                rax += rbx;             //add rax, rbx
                rdx += rax;             //add rdx, rax
                return rdx;
            }
            case 11:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4A6A2]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x000000000854574C]
                rax = rdx;              //mov rax, rdx
                rax >>= 0xB;            //shr rax, 0x0B
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x16;           //shr rax, 0x16
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x2C;           //shr rax, 0x2C
                rdx ^= rax;             //xor rdx, rax
                rdx -= r11;             //sub rdx, r11
                rax = detail::base + 0x9DBF;          //lea rax, [0xFFFFFFFFFDF542BC]
                rax = ~rax;             //not rax
                rax += rbx;             //add rax, rbx
                rdx ^= rax;             //xor rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rdx ^= r11;             //xor rdx, r11
                rcx = rbx;              //mov rcx, rbx
                rcx = ~rcx;             //not rcx
                rax = detail::base + 0x1935;          //lea rax, [0xFFFFFFFFFDF4BBCB]
                rax = ~rax;             //not rax
                rcx += rax;             //add rcx, rax
                rdx ^= rcx;             //xor rdx, rcx
                rdx += rbx;             //add rdx, rbx
                rax = 0x3858C5C3A577CA13;               //mov rax, 0x3858C5C3A577CA13
                rdx *= rax;             //imul rdx, rax
                return rdx;
            }
            case 12:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4A1D6]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB132);               //mov r9, [0x00000000085452AA]
                rax = detail::base + 0x2D67DF8B;              //lea rax, [0x000000002B5C7CE8]
                rax = ~rax;             //not rax
                rax += rbx;             //add rax, rbx
                rax ^= rdx;             //xor rax, rdx
                rdx = 0xA4CFC8374A943461;               //mov rdx, 0xA4CFC8374A943461
                rax *= rdx;             //imul rax, rdx
                rdx = detail::base + 0x3325D2CA;              //lea rdx, [0x00000000311A7009]
                rax += rbx;             //add rax, rbx
                rdx = ~rdx;             //not rdx
                rdx += rax;             //add rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x5;            //shr rax, 0x05
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xA;            //shr rax, 0x0A
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x14;           //shr rax, 0x14
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x28;           //shr rax, 0x28
                rdx ^= rax;             //xor rdx, rax
                rax = rbx;              //mov rax, rbx
                uintptr_t RSP_0x48;
                RSP_0x48 = detail::base + 0x18DD1062;                 //lea rax, [0x0000000016D1B231] : RSP+0x48
                rax *= RSP_0x48;                //imul rax, [rsp+0x48]
                rdx -= rax;             //sub rdx, rax
                rax = 0xDBE43EC444821FF9;               //mov rax, 0xDBE43EC444821FF9
                rdx *= rax;             //imul rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rdx -= r11;             //sub rdx, r11
                return rdx;
            }
            case 13:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF49CEE]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x0000000008544DBA]
                rax = rbx;              //mov rax, rbx
                rax -= r11;             //sub rax, r11
                rax += 0xFFFFFFFFFFFFB4B9;              //add rax, 0xFFFFFFFFFFFFB4B9
                rdx += rax;             //add rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x27;           //shr rax, 0x27
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x14;           //shr rax, 0x14
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x28;           //shr rax, 0x28
                rdx ^= rax;             //xor rdx, rax
                rax = 0x2F4B8956C26757EB;               //mov rax, 0x2F4B8956C26757EB
                rdx *= rax;             //imul rdx, rax
                rax = 0x116A8BF62AC1A397;               //mov rax, 0x116A8BF62AC1A397
                rdx *= rax;             //imul rdx, rax
                uintptr_t RSP_0x70;
                RSP_0x70 = 0xFEFCFFD029CE53C3;          //mov rax, 0xFEFCFFD029CE53C3 : RSP+0x70
                rdx *= RSP_0x70;                //imul rdx, [rsp+0x70]
                rax = rbx;              //mov rax, rbx
                uintptr_t RSP_0xFFFFFFFFFFFFFFA8;
                RSP_0xFFFFFFFFFFFFFFA8 = detail::base + 0x58BE759B;           //lea rax, [0x0000000056B31282] : RBP+0xFFFFFFFFFFFFFFA8
                rax *= RSP_0xFFFFFFFFFFFFFFA8;          //imul rax, [rbp-0x58]
                rdx -= rax;             //sub rdx, rax
                return rdx;
            }
            case 14:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF4988C]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB132);               //mov r9, [0x0000000008544976]
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rdx -= r11;             //sub rdx, r11
                rax = 0x8F1682E42BDE8A9B;               //mov rax, 0x8F1682E42BDE8A9B
                rdx ^= rax;             //xor rdx, rax
                rax = 0x6E6AF5930C9F7FF3;               //mov rax, 0x6E6AF5930C9F7FF3
                rdx *= rax;             //imul rdx, rax
                rax = 0xFFFFFFFFFFFFE67F;               //mov rax, 0xFFFFFFFFFFFFE67F
                rax -= r11;             //sub rax, r11
                rdx += rax;             //add rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x19;           //shr rax, 0x19
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x32;           //shr rax, 0x32
                rdx ^= rax;             //xor rdx, rax
                rdx += r11;             //add rdx, r11
                return rdx;
            }
            case 15:
            {
                r11 = detail::base;           //lea r11, [0xFFFFFFFFFDF493DA]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB132);              //mov r10, [0x0000000008544466]
                rax = 0x6968CAC923C26FB3;               //mov rax, 0x6968CAC923C26FB3
                rdx *= rax;             //imul rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0xf);              //imul rdx, [rax+0x0F]
                rax = 0x36BB76C96CC68EB7;               //mov rax, 0x36BB76C96CC68EB7
                rdx -= rax;             //sub rdx, rax
                rdx ^= rbx;             //xor rdx, rbx
                rax = rdx;              //mov rax, rdx
                rax >>= 0x18;           //shr rax, 0x18
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x30;           //shr rax, 0x30
                rdx ^= rax;             //xor rdx, rax
                rax = 0xFFFFFFFF8F7E6544;               //mov rax, 0xFFFFFFFF8F7E6544
                rax -= rbx;             //sub rax, rbx
                rax -= r11;             //sub rax, r11
                rdx += rax;             //add rdx, rax
                rax = 0xC0A4F70E7D6CBF8B;               //mov rax, 0xC0A4F70E7D6CBF8B
                rdx *= rax;             //imul rdx, rax
                rax = rbx;              //mov rax, rbx
                uintptr_t RSP_0x68;
                RSP_0x68 = detail::base + 0x3781D984;                 //lea rax, [0x0000000035766D57] : RSP+0x68
                rax *= RSP_0x68;                //imul rax, [rsp+0x68]
                rdx += rax;             //add rdx, rax
                return rdx;
            }
            }
        }
        inline uintptr_t get_bone_base()
        {
            const uint64_t mb = detail::base;
            uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
            rdx = driver::read<uintptr_t>(detail::base + 0xDC3EA28);
            if (!rdx)
                return rdx;
            r11 = ~detail::peb;              //mov r11, gs:[rax]
            rax = r11;              //mov rax, r11
            rax <<= 0x29;           //shl rax, 0x29
            rax = _byteswap_uint64(rax);            //bswap rax
            rax &= 0xF;
            switch (rax) {
            case 0:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FD35A0]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x26;           //shr rax, 0x26
                rdx ^= rax;             //xor rdx, rax
                rax = 0x213C813B4AF4E6C7;               //mov rax, 0x213C813B4AF4E6C7
                rdx ^= rax;             //xor rdx, rax
                rdx += r11;             //add rdx, r11
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1F;           //shr rax, 0x1F
                rdx ^= rax;             //xor rdx, rax
                rcx = rdx;              //mov rcx, rdx
                rcx >>= 0x3E;           //shr rcx, 0x3E
                rcx ^= rdx;             //xor rcx, rdx
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rcx *= driver::read<uintptr_t>(rax + 0x9);              //imul rcx, [rax+0x09]
                rax = rcx;              //mov rax, rcx
                rax >>= 0x13;           //shr rax, 0x13
                rcx ^= rax;             //xor rcx, rax
                rax = 0x824CB16CB7D07CAD;               //mov rax, 0x824CB16CB7D07CAD
                rdx = rcx;              //mov rdx, rcx
                rdx >>= 0x26;           //shr rdx, 0x26
                rdx ^= rcx;             //xor rdx, rcx
                rdx *= rax;             //imul rdx, rax
                return rdx;
            }
            case 1:
            {
                r14 = detail::base + 0x5CE68DBC;              //lea r14, [0x000000005A840BE3]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);               //mov r9, [0x0000000007FD2FAC]
                rax = 0x1539524EB5DE62ED;               //mov rax, 0x1539524EB5DE62ED
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x24;           //shr rax, 0x24
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1A;           //shr rax, 0x1A
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x34;           //shr rax, 0x34
                rdx ^= rax;             //xor rdx, rax
                rax = r11 + 0x1;                //lea rax, [r11+0x01]
                rax *= r14;             //imul rax, r14
                rdx += rax;             //add rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = 0x1B272244514D3687;               //mov rax, 0x1B272244514D3687
                rdx *= rax;             //imul rdx, rax
                rax = 0x5F7C447C242CDF65;               //mov rax, 0x5F7C447C242CDF65
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x19;           //shr rax, 0x19
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x32;           //shr rax, 0x32
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 2:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FD29B0]
                rax = detail::base + 0x2C7E317D;              //lea rax, [0x000000002A1BA629]
                rax = ~rax;             //not rax
                rax *= r11;             //imul rax, r11
                rcx = 0;                //and rcx, 0xFFFFFFFFC0000000
                rdx += rax;             //add rdx, rax
                rcx = _rotl64(rcx, 0x10);               //rol rcx, 0x10
                rcx ^= r10;             //xor rcx, r10
                rcx = ~rcx;             //not rcx
                rdx *= driver::read<uintptr_t>(rcx + 0x9);              //imul rdx, [rcx+0x09]
                rax = rdx;              //mov rax, rdx
                rax >>= 0xB;            //shr rax, 0x0B
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x16;           //shr rax, 0x16
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x2C;           //shr rax, 0x2C
                rdx ^= rax;             //xor rdx, rax
                rax = 0xD7E63CEBC86D9517;               //mov rax, 0xD7E63CEBC86D9517
                rdx += r11;             //add rdx, r11
                rdx *= rax;             //imul rdx, rax
                rax = 0xD2FBFB14563595D2;               //mov rax, 0xD2FBFB14563595D2
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x24;           //shr rax, 0x24
                rdx ^= rax;             //xor rdx, rax
                rax = 0x1806267E8B1FD621;               //mov rax, 0x1806267E8B1FD621
                rdx += rax;             //add rdx, rax
                return rdx;
            }
            case 3:
            {
                rcx = detail::base + 0x12AB32C5;              //lea rcx, [0x000000001048A609]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FD24D7]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1D;           //shr rax, 0x1D
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x3A;           //shr rax, 0x3A
                rax ^= r11;             //xor rax, r11
                rdx ^= rax;             //xor rdx, rax
                rax = r11;              //mov rax, r11
                rax ^= rcx;             //xor rax, rcx
                rdx -= rax;             //sub rdx, rax
                r13 = 0x2DD51598DF53073;                //mov r13, 0x2DD51598DF53073
                rdx += r13;             //add rdx, r13
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = 0x49EA65F287C25B3C;               //mov rax, 0x49EA65F287C25B3C
                rdx ^= rax;             //xor rdx, rax
                rax = 0x3ACBFCABD49B0EAB;               //mov rax, 0x3ACBFCABD49B0EAB
                rdx *= rax;             //imul rdx, rax
                rdx += r11;             //add rdx, r11
                return rdx;
            }
            case 4:
            {
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);               //mov r9, [0x0000000007FD1EDE]
                rax = 0x401D386832CD0BA4;               //mov rax, 0x401D386832CD0BA4
                rdx -= rax;             //sub rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x3;            //shr rax, 0x03
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x6;            //shr rax, 0x06
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xC;            //shr rax, 0x0C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x18;           //shr rax, 0x18
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x30;           //shr rax, 0x30
                rdx ^= rax;             //xor rdx, rax
                rax = 0x331D38A1F2A6DE0E;               //mov rax, 0x331D38A1F2A6DE0E
                rdx -= rax;             //sub rdx, rax
                rax = 0xFFFFFFFFFFFFC839;               //mov rax, 0xFFFFFFFFFFFFC839
                rax -= r11;             //sub rax, r11
                rax -= detail::base;          //sub rax, [rsp+0xC0] -- didn't find trace -> use base
                rdx += rax;             //add rdx, rax
                rax = 0x422012D407F37B9F;               //mov rax, 0x422012D407F37B9F
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1C;           //shr rax, 0x1C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x38;           //shr rax, 0x38
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x14;           //shr rax, 0x14
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x28;           //shr rax, 0x28
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 5:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FD189D]
                rdx += r11;             //add rdx, r11
                rax = rdx;              //mov rax, rdx
                rcx = detail::base + 0x838C;          //lea rcx, [0xFFFFFFFFFD9DE802]
                rax >>= 0x18;           //shr rax, 0x18
                rcx = ~rcx;             //not rcx
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rcx *= r11;             //imul rcx, r11
                rax >>= 0x30;           //shr rax, 0x30
                rdx ^= rax;             //xor rdx, rax
                rax = 0x3BF4CA66FE52E0E3;               //mov rax, 0x3BF4CA66FE52E0E3
                rdx += rcx;             //add rdx, rcx
                rdx *= rax;             //imul rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = 0x34527C2DED54A1C7;               //mov rax, 0x34527C2DED54A1C7
                rdx *= rax;             //imul rdx, rax
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D6390]
                rdx -= rax;             //sub rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1B;           //shr rax, 0x1B
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x36;           //shr rax, 0x36
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 6:
            {
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);               //mov r9, [0x0000000007FD1482]
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D5FC4]
                rax += 0x1A8D;          //add rax, 0x1A8D
                rax += r11;             //add rax, r11
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xE;            //shr rax, 0x0E
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1C;           //shr rax, 0x1C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x38;           //shr rax, 0x38
                rdx ^= rax;             //xor rdx, rax
                rax = 0x7D97F85C019DD32D;               //mov rax, 0x7D97F85C019DD32D
                rdx *= rax;             //imul rdx, rax
                rdx -= r11;             //sub rdx, r11
                rax = rdx;              //mov rax, rdx
                rax >>= 0x3;            //shr rax, 0x03
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x6;            //shr rax, 0x06
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xC;            //shr rax, 0x0C
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x18;           //shr rax, 0x18
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x30;           //shr rax, 0x30
                rdx ^= rax;             //xor rdx, rax
                rax = 0x39DB0353EE0C4BAF;               //mov rax, 0x39DB0353EE0C4BAF
                rdx ^= rax;             //xor rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                return rdx;
            }
            case 7:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FD0F55]
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = r11;              //mov rax, r11
                rax -= detail::base;          //sub rax, [rsp+0xC0] -- didn't find trace -> use base
                rax += 0xFFFFFFFF8C27F22D;              //add rax, 0xFFFFFFFF8C27F22D
                rdx += rax;             //add rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0xD;            //shr rax, 0x0D
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1A;           //shr rax, 0x1A
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x34;           //shr rax, 0x34
                rdx ^= rax;             //xor rdx, rax
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D5B2C]
                rcx = r11;              //mov rcx, r11
                rax += rdx;             //add rax, rdx
                rcx = ~rcx;             //not rcx
                rdx = 0x1F0AD082FD6E065D;               //mov rdx, 0x1F0AD082FD6E065D
                rdx += rax;             //add rdx, rax
                rdx += rcx;             //add rdx, rcx
                rdx ^= r11;             //xor rdx, r11
                rax = 0x81D9BE99B154F229;               //mov rax, 0x81D9BE99B154F229
                rdx *= rax;             //imul rdx, rax
                rax = 0x9F692ED5E2905787;               //mov rax, 0x9F692ED5E2905787
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 8:
            {
                r13 = detail::base + 0xB224;          //lea r13, [0xFFFFFFFFFD9E0A19]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FD0983]
                rax = r11;              //mov rax, r11
                rax ^= r13;             //xor rax, r13
                rdx -= rax;             //sub rdx, rax
                rax = detail::base + 0xC24C;          //lea rax, [0xFFFFFFFFFD9E18A2]
                rax = ~rax;             //not rax
                rax -= r11;             //sub rax, r11
                rdx ^= rax;             //xor rdx, rax
                rax = 0x1B9DA4834EC9D3A0;               //mov rax, 0x1B9DA4834EC9D3A0
                rdx -= rax;             //sub rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rax = driver::read<uintptr_t>(rax + 0x9);               //mov rax, [rax+0x09]
                uintptr_t RSP_0x40;
                RSP_0x40 = 0xF4344B1AA1B53A55;          //mov rax, 0xF4344B1AA1B53A55 : RSP+0x40
                rax *= RSP_0x40;                //imul rax, [rsp+0x40]
                rdx *= rax;             //imul rdx, rax
                rdx ^= r11;             //xor rdx, r11
                rax = rdx;              //mov rax, rdx
                rax >>= 0x26;           //shr rax, 0x26
                rdx ^= rax;             //xor rdx, rax
                rax = 0xD50FA2F7D770347C;               //mov rax, 0xD50FA2F7D770347C
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 9:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FD047B]
                rcx = detail::base + 0x756D9950;              //lea rcx, [0x00000000730AEBE2]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x23;           //shr rax, 0x23
                rdx ^= rax;             //xor rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = r11;              //mov rax, r11
                uintptr_t RSP_0x90;
                RSP_0x90 = detail::base + 0x1694;             //lea rax, [0xFFFFFFFFFD9D697B] : RSP+0x90
                rax *= RSP_0x90;                //imul rax, [rsp+0x90]
                rdx += rax;             //add rdx, rax
                rax = r11;              //mov rax, r11
                rax *= rcx;             //imul rax, rcx
                rdx ^= rax;             //xor rdx, rax
                rax = r11;              //mov rax, r11
                rax -= detail::base;          //sub rax, [rsp+0xC0] -- didn't find trace -> use base
                rax -= 0x67F5;          //sub rax, 0x67F5
                rdx ^= rax;             //xor rdx, rax
                rax = 0x1E6D6047923A6C0F;               //mov rax, 0x1E6D6047923A6C0F
                rdx *= rax;             //imul rdx, rax
                rax = 0x6B5D0FE42FD5E758;               //mov rax, 0x6B5D0FE42FD5E758
                rdx ^= rax;             //xor rdx, rax
                rax = 0x4F227610FD1DC0F0;               //mov rax, 0x4F227610FD1DC0F0
                rdx -= rax;             //sub rdx, rax
                return rdx;
            }
            case 10:
            {
                r13 = detail::base + 0x4581FC6E;              //lea r13, [0x00000000431F4A9C]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);               //mov r9, [0x0000000007FCFF5D]
                rax = 0xCE08021436D25129;               //mov rax, 0xCE08021436D25129
                rdx *= rax;             //imul rdx, rax
                rdx -= r11;             //sub rdx, r11
                rax = rdx;              //mov rax, rdx
                rax >>= 0x14;           //shr rax, 0x14
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x28;           //shr rax, 0x28
                rax ^= r11;             //xor rax, r11
                rdx ^= rax;             //xor rdx, rax
                rax = 0xA4107B3D6AA1768F;               //mov rax, 0xA4107B3D6AA1768F
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x1;            //shr rax, 0x01
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x2;            //shr rax, 0x02
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x4;            //shr rax, 0x04
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x8;            //shr rax, 0x08
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x10;           //shr rax, 0x10
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x20;           //shr rax, 0x20
                rdx ^= rax;             //xor rdx, rax
                rax = r11;              //mov rax, r11
                rax = ~rax;             //not rax
                rdx ^= rax;             //xor rdx, rax
                rdx ^= r13;             //xor rdx, r13
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                return rdx;
            }
            case 11:
            {
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FCF9E8]
                rcx = rdx;              //mov rcx, rdx
                rcx >>= 0x22;           //shr rcx, 0x22
                rcx ^= r11;             //xor rcx, r11
                rcx ^= rdx;             //xor rcx, rdx
                rax = detail::base + 0xAF60;          //lea rax, [0xFFFFFFFFFD9DF4F8]
                rcx += r11;             //add rcx, r11
                rcx += rax;             //add rcx, rax
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D458B]
                rcx ^= rax;             //xor rcx, rax
                rax = 0x2ACCD6CE6F89DCF;                //mov rax, 0x2ACCD6CE6F89DCF
                rcx *= rax;             //imul rcx, rax
                rdx = rcx;              //mov rdx, rcx
                rdx >>= 0x21;           //shr rdx, 0x21
                rdx ^= rcx;             //xor rdx, rcx
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x8;            //shr rax, 0x08
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x10;           //shr rax, 0x10
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x20;           //shr rax, 0x20
                rdx ^= rax;             //xor rdx, rax
                return rdx;
            }
            case 12:
            {
                r13 = detail::base + 0xD327;          //lea r13, [0xFFFFFFFFFD9E1633]
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);               //mov r9, [0x0000000007FCF454]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x25;           //shr rax, 0x25
                rdx ^= rax;             //xor rdx, rax
                rdx ^= r11;             //xor rdx, r11
                rax = 0xF51FA8131ADA9CCF;               //mov rax, 0xF51FA8131ADA9CCF
                rdx *= rax;             //imul rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x4;            //shr rax, 0x04
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x8;            //shr rax, 0x08
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x10;           //shr rax, 0x10
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x20;           //shr rax, 0x20
                rdx ^= rax;             //xor rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = r11;              //mov rax, r11
                rax = ~rax;             //not rax
                rax *= r13;             //imul rax, r13
                rdx += rax;             //add rdx, rax
                rax = 0x232C8C64FAF1AA2D;               //mov rax, 0x232C8C64FAF1AA2D
                rdx *= rax;             //imul rdx, rax
                rax = 0x6EE7946622278109;               //mov rax, 0x6EE7946622278109
                rdx += rax;             //add rdx, rax
                return rdx;
            }
            case 13:
            {
                rcx = detail::base + 0xB28E;          //lea rcx, [0xFFFFFFFFFD9DF12D]
                r10 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);              //mov r10, [0x0000000007FCEFB7]
                rax = rcx;              //mov rax, rcx
                rax = ~rax;             //not rax
                rax *= r11;             //imul rax, r11
                rdx ^= rax;             //xor rdx, rax
                rax = 0x6962CD38A4A70339;               //mov rax, 0x6962CD38A4A70339
                rdx *= rax;             //imul rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r10;             //xor rax, r10
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D3BB2]
                rax += 0xF6F;           //add rax, 0xF6F
                rax += r11;             //add rax, r11
                rdx += rax;             //add rdx, rax
                rax = 0x4619612F3FCECDE3;               //mov rax, 0x4619612F3FCECDE3
                rdx -= rax;             //sub rdx, rax
                rax = 0x763CF75FCB4CF3F4;               //mov rax, 0x763CF75FCB4CF3F4
                rdx -= rax;             //sub rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x18;           //shr rax, 0x18
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x30;           //shr rax, 0x30
                rdx ^= rax;             //xor rdx, rax
                rdx += r11;             //add rdx, r11
                return rdx;
            }
            case 14:
            {
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);               //mov r9, [0x0000000007FCEA3C]
                rax = rdx;              //mov rax, rdx
                rax >>= 0xB;            //shr rax, 0x0B
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x16;           //shr rax, 0x16
                rdx ^= rax;             //xor rdx, rax
                rax = rdx;              //mov rax, rdx
                rax >>= 0x2C;           //shr rax, 0x2C
                rax ^= r11;             //xor rax, r11
                rdx ^= rax;             //xor rdx, rax
                rax = 0x25BD7952B01DE407;               //mov rax, 0x25BD7952B01DE407
                rdx *= rax;             //imul rdx, rax
                rax = r11;              //mov rax, r11
                rax -= detail::base;          //sub rax, [rsp+0xC0] -- didn't find trace -> use base
                rax -= 0xE208;          //sub rax, 0xE208
                rdx ^= rax;             //xor rdx, rax
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D3777]
                rax += 0x5BE18515;              //add rax, 0x5BE18515
                rax += r11;             //add rax, r11
                rdx ^= rax;             //xor rdx, rax
                rax = 0x563A2CD1CD6E0521;               //mov rax, 0x563A2CD1CD6E0521
                rdx -= rax;             //sub rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                return rdx;
            }
            case 15:
            {
                r9 = driver::read<uintptr_t>(detail::base + 0xA5FB1E2);               //mov r9, [0x0000000007FCE588]
                rax = rdx;              //mov rax, rdx
                rax >>= 0x26;           //shr rax, 0x26
                rdx ^= rax;             //xor rdx, rax
                rax = 0;                //and rax, 0xFFFFFFFFC0000000
                rax = _rotl64(rax, 0x10);               //rol rax, 0x10
                rax ^= r9;              //xor rax, r9
                rax = ~rax;             //not rax
                rdx *= driver::read<uintptr_t>(rax + 0x9);              //imul rdx, [rax+0x09]
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D32DC]
                rdx += rax;             //add rdx, rax
                rax = r11;              //mov rax, r11
                rax -= detail::base;          //sub rax, [rsp+0xC0] -- didn't find trace -> use base
                rax -= 0x269CDE20;              //sub rax, 0x269CDE20
                rdx ^= rax;             //xor rdx, rax
                rax = 0x2F775EA7BE786E47;               //mov rax, 0x2F775EA7BE786E47
                rdx *= rax;             //imul rdx, rax
                rax = 0xA7C4A230E69470D5;               //mov rax, 0xA7C4A230E69470D5
                rdx ^= rax;             //xor rdx, rax
                rax = detail::base;           //lea rax, [0xFFFFFFFFFD9D3261]
                rdx -= rax;             //sub rdx, rax
                rdx += 0xFFFFFFFFFFFF8701;              //add rdx, 0xFFFFFFFFFFFF8701
                rdx += r11;             //add rdx, r11
                rdx -= r11;             //sub rdx, r11
                return rdx;
            }
            }
        }
        inline uint16_t get_bone_index(uint32_t bone_index)
        {
            const uint64_t mb = detail::base;
            uint64_t rax = mb, rbx = mb, rcx = mb, rdx = mb, rdi = mb, rsi = mb, r8 = mb, r9 = mb, r10 = mb, r11 = mb, r12 = mb, r13 = mb, r14 = mb, r15 = mb;
            rdi = bone_index;
            rcx = rdi * 0x13C8;
            rax = 0x86C2D6724C0021B1;               //mov rax, 0x86C2D6724C0021B1
            rax = _umul128(rax, rcx, (uintptr_t*)&rdx);             //mul rcx
            r11 = detail::base;           //lea r11, [0xFFFFFFFFFD5FF468]
            r10 = 0x73C500DCBCF88311;               //mov r10, 0x73C500DCBCF88311
            rdx >>= 0xC;            //shr rdx, 0x0C
            rax = rdx * 0x1E65;             //imul rax, rdx, 0x1E65
            rcx -= rax;             //sub rcx, rax
            rax = 0x3EC3B9CDA5F55695;               //mov rax, 0x3EC3B9CDA5F55695
            r8 = rcx * 0x1E65;              //imul r8, rcx, 0x1E65
            rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
            rax = r8;               //mov rax, r8
            rax -= rdx;             //sub rax, rdx
            rax >>= 0x1;            //shr rax, 0x01
            rax += rdx;             //add rax, rdx
            rax >>= 0xD;            //shr rax, 0x0D
            rax = rax * 0x3366;             //imul rax, rax, 0x3366
            r8 -= rax;              //sub r8, rax
            rax = 0x3ACA34797BD66F09;               //mov rax, 0x3ACA34797BD66F09
            rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
            rax = 0xCCCCCCCCCCCCCCCD;               //mov rax, 0xCCCCCCCCCCCCCCCD
            rdx >>= 0xA;            //shr rdx, 0x0A
            rcx = rdx * 0x116B;             //imul rcx, rdx, 0x116B
            rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
            rdx >>= 0x2;            //shr rdx, 0x02
            rcx += rdx;             //add rcx, rdx
            rax = rcx + rcx * 4;            //lea rax, [rcx+rcx*4]
            rax += rax;             //add rax, rax
            rcx = r8 + r8 * 2;              //lea rcx, [r8+r8*2]
            rcx <<= 0x2;            //shl rcx, 0x02
            rcx -= rax;             //sub rcx, rax
            rax = driver::read<uint16_t>(rcx + r11 * 1 + 0xA6CB0C0);                //movzx eax, word ptr [rcx+r11*1+0xA6CB0C0]
            r8 = rax * 0x13C8;              //imul r8, rax, 0x13C8
            rax = r10;              //mov rax, r10
            rax = _umul128(rax, r8, (uintptr_t*)&rdx);              //mul r8
            rcx = r8;               //mov rcx, r8
            rax = r10;              //mov rax, r10
            rcx -= rdx;             //sub rcx, rdx
            rcx >>= 0x1;            //shr rcx, 0x01
            rcx += rdx;             //add rcx, rdx
            rcx >>= 0xC;            //shr rcx, 0x0C
            rcx = rcx * 0x1609;             //imul rcx, rcx, 0x1609
            r8 -= rcx;              //sub r8, rcx
            r9 = r8 * 0x1AFD;               //imul r9, r8, 0x1AFD
            rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
            rax = r9;               //mov rax, r9
            rax -= rdx;             //sub rax, rdx
            rax >>= 0x1;            //shr rax, 0x01
            rax += rdx;             //add rax, rdx
            rax >>= 0xC;            //shr rax, 0x0C
            rax = rax * 0x1609;             //imul rax, rax, 0x1609
            r9 -= rax;              //sub r9, rax
            rax = 0x73C9B97112FF186D;               //mov rax, 0x73C9B97112FF186D
            rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
            rax = 0x4EC4EC4EC4EC4EC5;               //mov rax, 0x4EC4EC4EC4EC4EC5
            rdx >>= 0x8;            //shr rdx, 0x08
            rcx = rdx * 0x236;              //imul rcx, rdx, 0x236
            rax = _umul128(rax, r9, (uintptr_t*)&rdx);              //mul r9
            rdx >>= 0x2;            //shr rdx, 0x02
            rcx += rdx;             //add rcx, rdx
            rax = rcx * 0x1A;               //imul rax, rcx, 0x1A
            rcx = r9 * 0x1C;                //imul rcx, r9, 0x1C
            rcx -= rax;             //sub rcx, rax
            r15 = driver::read<uint16_t>(rcx + r11 * 1 + 0xA6D5EF0);                //movsx r15d, word ptr [rcx+r11*1+0xA6D5EF0]
            return r15;
        }

        struct ref_def_key
        {
            int ref0;
            int ref1;
            int ref2;
        };

        inline std::uint64_t get_ref_def()
        {
            ref_def_key crypt = driver::read<ref_def_key>(detail::base + offsets::ref_def_ptr);

            DWORD lower = crypt.ref0 ^ (crypt.ref2 ^ (uint64_t)(detail::base + offsets::ref_def_ptr)) * ((crypt.ref2 ^ (uint64_t)(detail::base + offsets::ref_def_ptr)) + 2);
            DWORD upper = crypt.ref1 ^ (crypt.ref2 ^ (uint64_t)(detail::base + offsets::ref_def_ptr + 0x4)) * ((crypt.ref2 ^ (uint64_t)(detail::base + offsets::ref_def_ptr + 0x4)) + 2);

            return (uint64_t)upper << 32 | lower;
        }
    }

    inline int local_index()
    {
        auto local_index = driver::read<uintptr_t>(wz::detail::client_info + offsets::local_index);
        if (!local_index) return -1;

        return driver::read<int>(local_index + offsets::local_index_pos);
    }

    inline uint64_t get_name_list()
    {
        uint64_t ptr = driver::read<uint64_t>(detail::base + offsets::name_array);
        if (!ptr)
            return 0;

        return ptr + offsets::name_array_pos;
    }

    inline name_entry_t get_name_entry(uint32_t index)
    {
        uint64_t list = get_name_list();
        if (!list)
            return {};

        return driver::read<name_entry_t>(list + (index * 0xd0));
    }

    inline vec3 get_camera_location()
    {
        auto camera = driver::read<uint64_t>(detail::base + offsets::camera_base);
        if (!camera)
            return {};

        return driver::read<vec3>(camera + offsets::camera_pos);
    }

    inline bool world_to_screen(vec3 world_location, vec2* screen_position)
    {
        auto camera_position = get_camera_location();

        auto local = world_location - camera_position;
        auto trans = vec3{ local.dot(detail::refdef.view.axis[1]), local.dot(detail::refdef.view.axis[2]), local.dot(detail::refdef.view.axis[0]) };

        if (trans.z < 0.01f)
            return false;

        const float screen_width = detail::refdef.width;
        const float screen_height = detail::refdef.height;

        const float fov_x = detail::refdef.view.tan_half_fov.x;
        const float fov_y = detail::refdef.view.tan_half_fov.y;

        screen_position->x = ((float)screen_width / 2.0) * (1.0 - (trans.x / fov_x / trans.z));
        screen_position->y = ((float)screen_height / 2.0) * (1.0 - (trans.y / fov_y / trans.z));

        if (screen_position->x < 1 ||
            screen_position->y < 1 ||
            (screen_position->x > detail::refdef.width) ||
            (screen_position->y > detail::refdef.height)) {
            return false;
        }

        return true;
    }

    inline vec3 get_bone_position(const uintptr_t bone_ptr, const vec3& base_pos, const int bone)
    {
        vec3 pos = driver::read<vec3>(bone_ptr + ((uint64_t)bone * 0x20) + 0x10);
        pos.x += base_pos.x;
        pos.y += base_pos.y;
        pos.z += base_pos.z;
        return pos;
    }

    inline vec3 get_bone_base(uint64_t client_info)
    {
        return driver::read<vec3>(client_info + offsets::bone::bone_base);
    }

    class player_t
    {
    public:
        player_t(std::uint64_t address) {
            this->address = address;
        }

        std::uint64_t address{};

        bool is_valid()
        {
            return driver::read<bool>(address + offsets::player::valid);
        }

        bool dead()
        {
            auto dead1 = driver::read<bool>(address + offsets::player::dead_1);
            return dead1;
        }

        int team_id()
        {
            return driver::read<int>(address + offsets::player::team);
        }

        vec3 get_pos()
        {
            auto local_pos = driver::read<std::uint64_t>(address + offsets::player::pos);
            if (!local_pos) return {};

            return driver::read<vec3>(local_pos + 0x48);
        }

        uint64_t get_bone_ptr(uint64_t bone_base, uint64_t bone_index)
        {
            if (!bone_base)
                return 0;

            return driver::read<uint64_t>(bone_base + (bone_index * offsets::bone::size) + 0xD8);
        }
    };
}