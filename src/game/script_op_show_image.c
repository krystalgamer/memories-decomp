#define D_8009B145_IN_DATA
#include "../types.h"
#include "display_object_config.h"
#include "scene_script.h"
#include "script_command_busy.h"
#include "fade.h"
#include "display_object.h"
#include "display_object_api.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "func_8002E128.h"
#include "script_op_show_image.h"

#define VRAM_COPY_WIDTH 0x140
#define VRAM_COPY_HEIGHT 0xA0

extern s32 gGraphics_bActiveBuffer __attribute__((section(".data")));
#define gGraphics_bActiveBuffer (*(u8 *)&gGraphics_bActiveBuffer)
extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data

void Script_OpShowImage(void) {
    DisplayObject *rec;
    u16 flags;
    s32 ret;
    s32 masked;
    u32 mask;

    ret = func_8002E3B4();
    mask = 0x2000000;
    if (ret == 0) {
        D_800E9D70[0].x = 0;
        D_800E9D70[0].y = 0;
        D_800E9D70[0].w = VRAM_COPY_WIDTH;
        D_800E9D70[0].h = VRAM_COPY_HEIGHT;
        if (gGraphics_bActiveBuffer == 0) {
            D_800E9D70[0].x = VRAM_COPY_WIDTH;
        }
        MoveImage(&D_800E9D70[0], 0x1C0, 0x100);
        return;
    }

    mask |= 0x30;
    masked = D_8009B0F4_abs & mask;
    if ((masked | D_8009B134_abs) != 0) {
        return;
    }
    if (gFade_State.flags & FADE_FLAG_ACTIVE) {
        return;
    }

    flags = D_8009B27C;
    if (!(flags & 0x4000)) {
        D_8009B27C = flags | 0x4000;
        rec = func_800400AC(func_8004006C(), 3);
        func_80040510((DisplayObjectConfigView *)rec, 0, 0, VRAM_COPY_WIDTH,
            VRAM_COPY_HEIGHT, 0, 0, 0x17, 0, 0xF4);
        D_8009B280 = rec;
        rec->attribute |= 0x2000000;
        func_8002E00C((ScriptImageEntry *)D_800EAE98);
        gGraphics_sViewportX = D_8009B2A8;
        gGraphics_sViewportY = D_8009B2AA;
        if (D_8009B145 == 0) {
            func_80015C84(D_8009B145);
        }
        func_8002DF2C((u8 *)D_800EAE98, D_8009B270 & 0xFFF);
        return;
    }
    if (!(flags & 0x2000)) {
        D_8009B27C = flags | 0x2000;
        func_8004036C(D_8009B280);
        func_8002E128((u8 *)D_800EAE98, -1);
        if (D_8009B145 == 0 && !(D_8009B270 & 0x4000)) {
            func_80015C0C();
        }
        return;
    }
    D_8009B27C = 0;
}
