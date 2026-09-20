#include "../types.h"
#include "scene_script.h"
#include "script_command_busy.h"
#include "fade.h"
#include "file_transfer.h"
#define GRAPHICS_VIEWPORT_IN_DATA
#include "graphics_frame.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "script_image_rebuild.h"
#include "script_op_load_image_scene.h"

#define SCRIPT_IMAGE_OBJECT_SET_VIEW(set) ((ScriptImageObjectSet *)(set))

void Script_OpLoadImageScene(void)
{
    u8 *script;
    u8 *next;
    u8 *next2;
    s32 value;
    u16 flags;

    if (ScriptCommand_MarkStarted() == 0) {
        gGraphics_sViewportY = 0;
        gGraphics_sViewportX = 0;
        ScriptImage_ReleaseObjects(SCRIPT_IMAGE_OBJECT_SET_VIEW(D_800EAE98));
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
        ScriptImage_RequestTransfer(
            SCRIPT_IMAGE_OBJECT_SET_VIEW(D_800EAE98), D_8009B270);
    }
    flags = D_8009B27C;
    if ((flags & 0x800) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) == 0) {
            D_8009B27C = flags | 0x800;
            ScriptImage_RebuildObjects(
                SCRIPT_IMAGE_OBJECT_SET_VIEW(D_800EAE98), -1);
            Fade_StartIn();
        }
    } else {
        if ((gFade_State.flags & FADE_FLAG_ACTIVE) == 0) {
            D_8009B27C = 0;
        }
    }
}
