#define D_8009B145_IN_DATA
#define GRAPHICS_ACTIVE_BUFFER_IN_DATA
#include "../types.h"
#include "display_object_config.h"
#include "scene_script.h"
#include "script_command_busy.h"
#include "fade.h"
#include "display_object.h"
#include "display_object_core.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "file_transfer.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "script_image_rebuild.h"
#include "script_image_commands.h"

#define VRAM_COPY_WIDTH 0x140
#define VRAM_COPY_HEIGHT 0xA0
#define SCRIPT_IMAGE_OBJECT_SET_VIEW(set) ((ScriptImageObjectSet *)(set))

void Script_OpStageImage(void){register unsigned char*p=D_8009B290;register unsigned char*p2=p+2;register unsigned char*p4;unsigned short value;D_8009B2AA=0;D_8009B2A8=0;D_8009B290=p2;value=p[0]|(p[1]<<8);D_8009B270=value;if(value&0x8000){p4=p+4;D_8009B290=p4;D_8009B2A8=p[2]|(p2[1]<<8);D_8009B290=p+6;D_8009B2AA=p[4]|(p4[1]<<8);}D_8009B27C=5;}

void Script_OpShowImage(void) {
    DisplayObject *rec;
    u16 flags;
    s32 ret;
    s32 masked;
    u32 mask;

    ret = func_8002E3B4();
    mask = DISPLAY_OBJECT_ATTRIBUTE_16BPP;
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
        rec = DisplayObject_AcquireSlot(DisplayObject_FindFreeSlot(), 3);
        DisplayObject_ConfigureScreenSprite(rec, 0, 0, VRAM_COPY_WIDTH,
            VRAM_COPY_HEIGHT, 0, 0, 0x17, 0, 0xF4);
        D_8009B280 = rec;
        rec->attribute |= DISPLAY_OBJECT_ATTRIBUTE_16BPP;
        ScriptImage_ReleaseObjects(SCRIPT_IMAGE_OBJECT_SET_VIEW(D_800EAE98));
        gGraphics_sViewportX = D_8009B2A8;
        gGraphics_sViewportY = D_8009B2AA;
        if (D_8009B145 == 0) {
            func_80015C84(D_8009B145);
        }
        ScriptImage_RequestTransfer(
            SCRIPT_IMAGE_OBJECT_SET_VIEW(D_800EAE98), D_8009B270 & 0xFFF);
        return;
    }
    if (!(flags & 0x2000)) {
        D_8009B27C = flags | 0x2000;
        DisplayObject_ReleaseIfPresent(D_8009B280);
        ScriptImage_RebuildObjects(
            SCRIPT_IMAGE_OBJECT_SET_VIEW(D_800EAE98), -1);
        if (D_8009B145 == 0 && !(D_8009B270 & 0x4000)) {
            func_80015C0C();
        }
        return;
    }
    D_8009B27C = 0;
}
