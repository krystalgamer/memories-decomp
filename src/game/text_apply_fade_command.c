#define D_8009B140_IN_DATA
#define D_8009AF74_IN_DATA
#include "../types.h"
#include "fade.h"
#include "fade_overlay_controls.h"
#include "display_object_helpers.h"
#include "display_object_depth_bases.h"
#include "duel_effect_sound_commands.h"

void Text_ApplyFadeCommand(DuelEffectChannel *object)
{
    u8 *G32 *cursor = &((TextStreamOwner *)object)->streams[object->stream_index];
    u8 *stream = *cursor;
    s32 command = *stream;
    s32 opcode;

    *cursor = stream + 1;
    opcode = command;
    if (opcode & 0x40) {
        gFade_bKeepOverlayDepth = *(u8 *)&gDisplayObject_awOrderingTableDepthBases[1] + 9;
    }
    if (opcode & 0x20) {
        gFade_bKeepOverlayDepth = 4;
    }
    if (opcode & 0x10) {
        if (opcode & 1) {
            Fade_InitOutColor(0xFFFFFF);
        } else {
            Fade_InitInColor(0xFFFFFF);
        }
        gFade_State.step = 4;
    } else if (opcode & 1) {
        Fade_StartOutKeepOverlay();
    } else {
        Fade_StartInKeepOverlay();
    }
    if (opcode & 0x80) {
        Fade_Wait();
    }
}
