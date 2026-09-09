#include "../types.h"
#include "display_object_config.h"
#include "script_command_busy.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "display_object_api.h"
#include "display_object_layout.h"
#include "fade.h"
#include "display_object_helpers.h"
#include "file_transfer.h"
#include "script_state.h"
#include "script_image_objects.h"
#include "../unmatched.h"
#include "func_8002F630.h"

typedef struct {
    u8 pad00[4];
    u32 flags;
    u16 attr;
    u8 pad0A[0x1E];
    u32 field_28;
    u8 pad2C[4];
    u32 field_30;
    u8 pad34[4];
    u32 field_38;
    u32 field_3C;
    u32 field_40;
    u32 field_44;
} Object;

extern Object *D_8009B2A0;
extern Object *D_8009B280;
extern Object *D_800EAE98[];
extern void func_8002F4C0(u8 *, s32);
extern u32 func_8004703C(void);

/* Duel result screen setup. Reads the two-byte result code from the script
   stream, queues the result sector through File_RequestAsyncTransfer with
   func_8002F4C0 as the completion callback, and on the first pass (once the
   transfer and fade flags are clear) builds the screen: the banner object
   from D_801AF000, the two 0x140-wide panels, and the white-on-black fill
   object kept in D_8009B280, then starts the music. On later passes it
   waits for the fade, releases the fill object and the panel list, and
   clears the phase flag. */
void func_8002F630(void) {
    u8 *p;
    Object *o;
    Object *q;
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
            D_8009B2A0->attr &= ~DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        func_8002E00C((ScriptImageEntry *)D_800EAE98);
        four = 4;
        o = func_800400AC(func_8004002C(), 2);
        func_800428A8(o, 0, 0, 0, 0, 0, 0x17, four, D_801AF000);
        func_800428EC(o, 1);
        o->attr |= 0x28;
        o->flags |= 0x1000000;
        D_800EAE98[0] = o;
        o = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o, 0, 0, 0x140, 0xF0, 0, 0, 0x19, 0, 0xF5);
        func_800428EC(o, -1);
        D_800EAE98[5] = o;
        o = func_800400AC(func_8004002C(), 1);
        func_80040510((DisplayObjectConfigView *)o, 0x100, 0, 0x40, 0xF0, 0, 0, 0x19, 0, 0xF5);
        func_800428EC(o, -1);
        D_800EAE98[10] = o;
        q = func_800400AC(func_8004002C(), four);
        func_800427DC(q, 0);
        color = 0xF00140;
        q->field_30 = 0x140;
        q->field_38 = 0xF00000;
        q->field_3C = 0xFFFFFF;
        q->field_44 = 0xFFFFFF;
        q->field_28 = 0;
        q->field_40 = color;
        D_8009B280 = q;
        q->flags |= (GsALON | GsATWO);
        Fade_StartIn();
        gFade_State.step = four;
        Fade_Wait();
    } else {
        if (func_8004703C() & 0x80) {
            return;
        }
        Fade_WaitOut();
        if (D_8009B2A0 != 0) {
            D_8009B2A0->attr |= DISPLAY_OBJECT_FLAG_RENDERABLE;
        }
        func_8004036C(D_8009B280);
        func_8002E00C((ScriptImageEntry *)D_800EAE98);
        D_8009B27C = 0;
    }
}
