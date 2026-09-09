#include "../types.h"
#include "scene_script.h"
#include "file_transfer.h"
#include "graphics_frame.h"
#include "menu_record_reset.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

extern u16 D_8009B2A4;
extern void *D_8009B2A0;
extern s16 gGraphics_sViewportX_data asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern s16 gGraphics_sViewportY_data asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#define gGraphics_sViewportX gGraphics_sViewportX_data
#define gGraphics_sViewportY gGraphics_sViewportY_data
extern s32 D_80010000 __attribute__((section(".data")));

extern void func_8002FB78(void);
extern void *func_8002E3FC(void);

/* Loads the fixed scene presentation/event package. The argument initializes
 * D_8009B2A4 for the event driver; it does not select a different disc page.
 * The 0x1E57/0x31 request uses func_8002FB78 to route image, palette, event
 * script, and image/CLUT data. After resetting viewport and slot state, this
 * function polls IsIdleGPU(10) until it returns zero, then uploads 25 pairs
 * from the arena held in D_80010000: 24-word by 48-row image rectangles on a
 * five-wide grid from x=0x380, followed by 64x1 CLUT rectangles from y=0xF0.
 *
 * The clearing loop subscripts the record array rather than walking a
 * pointer. Both spellings give 112 instructions, but the walking pointer adds
 * a reference to `slot` and that changes which pseudo the allocator ranks
 * first: the %hi temporary for D_800EAE98 then takes $v0 and the address is
 * completed as `addiu $s0,$v0`, where retail coalesces the pair into
 * `lui $s0` / `addiu $s0,$s0` and emits it after the callback address rather
 * than before. Subscripting drops that reference and the prologue falls into
 * retail's order. */
void func_8002FD10(s16 arg0)
{
    SceneScriptSlot *slot;
    u32 *src;
    RECT *second;
    s32 i;

    slot = D_800EAE98;
    D_8009B2A4 = arg0;
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    D_8009B2A0 = 0;
    File_RequestAsyncTransfer(
        0, 0,
        FILE_WA_CAMPAIGN_SCENE_START_SECTOR,
        FILE_WA_CAMPAIGN_SCENE_SECTOR_COUNT,
        func_8002FB78, 0, 0
    );
    func_80039E9C();

    *(s16 *)&slot[3] = -1;
    for (i = 0; i < 3; i++) {
        slot[i].unk00 = 0;
        slot[i].unk04 = 0;
    }
    File_WaitForTransfers();

    while (IsIdleGPU(10)) {
        ;
    }

    src = (u32 *)D_80010000;
    D_800E9D70[0].x = 0x380;
    D_800E9D70[0].y = 0;
    D_800E9D70[0].w = 0x18;
    D_800E9D70[0].h = 0x30;
    second = &D_800E9D70[1];
    D_800E9D70[1].x = 0x380;
    second->y = 0xF0;
    second->w = 0x40;
    second->h = 1;

    i = 0;
    do {
        LoadImage2(&D_800E9D70[0], src);
        LoadImage2(&D_800E9D70[1], src + 0x240);
        i++;
        D_800E9D70[1].y = D_800E9D70[1].y + 1;
        D_800E9D70[0].x = (i % 5) * 24 + 0x380;
        D_800E9D70[0].y = (i / 5) * 48;
        if (D_800E9D70[1].y >= 0x100) {
            D_800E9D70[1].x = D_800E9D70[1].x + 0x40;
            D_800E9D70[1].y = 0xF0;
        }
        src += 0x260;
    } while (i < 25);

    D_8009B2A0 = func_8002E3FC();
}
