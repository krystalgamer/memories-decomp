#include "../types.h"
#include "scene_script.h"
#include "script_command_busy.h"
#include "fade.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "script_state.h"

extern u8 *D_8009B290;
extern u16 D_8009B270;
extern u8 D_800E9ECE[];
extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data
extern void func_8002E00C(void *);
extern void func_8002DF2C(void *, s32);
extern void func_8002E128(void *, s32);

void func_8002E470(void)
{
    u8 *script;
    u8 *next;
    u8 *next2;
    s32 value;
    u16 flags;

    if (func_8002E3B4() == 0) {
        gGraphics_sViewportY = 0;
        gGraphics_sViewportX = 0;
        func_8002E00C((u8 *)D_800EAE98);
        script = D_8009B290;
        next = script + 2;
        D_8009B290 = next;
        value = script[0] | (script[1] << 8);
        D_8009B270 = value;
        if ((value & 0x8000) != 0) {
            D_8009B270 = value & 0xFFF;
            next2 = script + 4;
            D_8009B290 = next2;
            gGraphics_sViewportX = script[2] | (next[1] << 8);
            D_8009B290 = script + 6;
            gGraphics_sViewportY = script[4] | (next2[1] << 8);
        }
        func_8002DF2C((u8 *)D_800EAE98, D_8009B270);
    }
    flags = D_8009B27C;
    if ((flags & 0x800) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) == 0) {
            D_8009B27C = flags | 0x800;
            func_8002E128((u8 *)D_800EAE98, -1);
            Fade_StartIn();
        }
    } else {
        if ((D_800E9ECE[0] & 0x80) == 0) {
            D_8009B27C = 0;
        }
    }
}
