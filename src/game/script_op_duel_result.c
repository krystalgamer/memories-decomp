#include "../types.h"
#include "display_object_config.h"
#include "script_command_busy.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "fade.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "sound_output.h"
#include "../unmatched.h"
#include "script_op_duel_result.h"
#include "func_8002F4C0.h"
#include "campaign_scene_package.h"

extern DisplayObject *D_800EAE98[];

/* Duel result screen setup. Reads the two-byte result code from the script
   stream, queues the result sector through File_RequestAsyncTransfer with
   func_8002F4C0 as the completion callback, and on the first pass (once the
   transfer and fade flags are clear) builds the screen: the banner object
   from D_801AF000, the two 0x140-wide panels, and the white-on-black fill
   object kept in D_8009B280, then starts the music. On later passes it
   waits for the fade, releases the fill object and the panel list, and
   clears the phase flag. */
void Script_OpDuelResult(void) {
    u8 *p;
    DisplayObject *o;
    DisplayObject *q;
    u16 flags;
    s32 four;
    s32 color;

    if (func_8002E3B4() == 0) {
        p = D_8009B290;
        D_8009B290 = p + 2;
        D_8009B29C = p[0] | (p[1] << 8);
        File_RequestAsyncTransfer(
            0, 0,
            FILE_WA_MENU_ASSETS_START_SECTOR,
            FILE_WA_MENU_ASSETS_SECTOR_COUNT,
            func_8002F4C0, 0, 0
        );
        File_WaitForTransfers();
    }
    flags = D_8009B27C;
    if ((flags & 0x4000) == 0) {
        if (((D_8009B0F4_abs & FILE_TRANSFER_REQUEST_BLOCKED_MASK) |
             D_8009B134_abs) != 0) {
            return;
        }
        D_8009B27C = flags | 0x4000;
        if (D_8009B2A0 != 0) {
            D_8009B2A0->flags &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        ScriptImage_ReleaseObjects((ScriptImageEntry *)D_800EAE98);
        four = 4;
        o = func_800400AC(func_8004002C(), 2);
        func_800428A8(o, 0, 0, 0, 0, 0, 0x17, four, D_801AF000);
        func_800428EC((u8 *)o, 1);
        o->flags |= 0x28;
        o->attribute |= DISPLAY_OBJECT_ATTRIBUTE_8BPP;
        D_800EAE98[0] = o;
        o = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o, 0, 0, 0x140, 0xF0, 0, 0, 0x19, 0, 0xF5);
        func_800428EC((u8 *)o, -1);
        D_800EAE98[5] = o;
        o = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o, 0x100, 0, 0x40, 0xF0, 0, 0, 0x19, 0, 0xF5);
        func_800428EC((u8 *)o, -1);
        D_800EAE98[10] = o;
        q = func_800400AC(func_8004002C(), four);
        func_800427DC(q, 0);
        color = 0xF00140;
        q->field_30.word = 0x140;
        q->field_38.word = 0xF00000;
        q->field_3C.word = 0xFFFFFF;
        q->field_44.word = 0xFFFFFF;
        q->position.word = 0;
        q->field_40.word = color;
        D_8009B280 = q;
        q->attribute |= (GsALON | GsATWO);
        Fade_StartIn();
        gFade_State.step = four;
        Fade_Wait();
    } else {
        if (func_8004703C() & 0x80) {
            return;
        }
        Fade_WaitOut();
        if (D_8009B2A0 != 0) {
            D_8009B2A0->flags |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        func_8004036C(D_8009B280);
        ScriptImage_ReleaseObjects((ScriptImageEntry *)D_800EAE98);
        D_8009B27C = 0;
    }
}
