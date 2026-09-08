#include "../../types.h"
#include "../../game/display_object_api.h"
#include "../../game/card_constants.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"

typedef struct {
    RECT img;
    RECT clut;
} RectPair;

extern RectPair D_800E9D70;

typedef struct {
    u8 pad0[4];
    u32 flags;
    u16 attr;
    u8 pad1[85];
    u8 mode;
} Obj;

extern u8 gFreeDuel_bReturnFlags;
extern s8 gFreeDuel_bCursorColumn;
extern s8 gFreeDuel_bCursorRow;
extern s8 gFreeDuel_bTargetColumn;
extern s8 gFreeDuel_bTargetRow;
extern u8 D_8009B362;
extern u8 gFreeDuel_aDuelistRecords[];
extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;
extern u8 gFreeDuel_bScreenFlags;
extern void *gFreeDuel_apSparklePool[];
extern u8 gFreeDuel_abGridAvailable[];
extern u8 *gFreeDuel_pCursorWidget;
extern u8 *gFreeDuel_pThumbWidget;
extern u8 D_801AF000[];

extern void func_80035C38(s32, s32, s32, s32, s32, s32, s32);
extern void func_80039794(void);
extern s32 Campaign_TestStoryFlag(s32);
extern void func_80040510(Obj *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_800428A8(Obj *, int, int, int, int, int, int, int, u8 *);
extern void func_800428EC(Obj *, int);
extern Obj *FreeDuel_SpawnSparkle(void);
extern void FreeDuel_PlaceCursor(u8 *, s32);
extern void FreeDuel_UpdateScrollbar(void);
extern void func_8003FF08(s32);

void FreeDuel_Init(u8 *src)
{
    s32 i;
    s32 one;
    s32 k;
    s32 row;
    s32 col;
    s32 count;
    u16 *rec;
    void **slot;
    u8 *cell;
    Obj *obj;
    RECT *clut;

    if (gFreeDuel_bReturnFlags & 0x80) {
        rec = (u16 *)(gFreeDuel_aDuelistRecords +
                      (gFreeDuel_bCursorRow * FREE_DUEL_GRID_COLUMN_COUNT +
                       gFreeDuel_bCursorColumn) * FREE_DUEL_GRID_RECORD_SIZE);
        if (D_8009B362 == 1) {
            rec++;
        }
        *rec = *rec + 1;
        if ((s16)*rec >= FREE_DUEL_RECORD_MAX + 1) {
            *rec = FREE_DUEL_RECORD_MAX;
        }
    }
    gGraphics_sViewportY = 0;
    gGraphics_sViewportX = 0;
    gFreeDuel_bScreenFlags = 0;
    if (gFreeDuel_bReturnFlags == 0) {
        gFreeDuel_bTargetRow = 0;
        gFreeDuel_bTargetColumn = 0;
        gFreeDuel_bCursorRow = 0;
        gFreeDuel_bCursorColumn = 0;
        func_80035C38(1, 13, 48, 108, 224, 16, 4136);
        func_80039794();
        gFreeDuel_bScreenFlags |= 0x20;
    }
    i = 15;
    slot = gFreeDuel_apSparklePool + i;
    do {
        *slot = 0;
        i--;
        slot--;
    } while (i >= 0);
    one = 1;
    i = FREE_DUEL_GRID_ENTRY_COUNT - 1;
    cell = gFreeDuel_abGridAvailable + i;
    do {
        *cell = one;
        i--;
        cell--;
    } while (i >= 0);
    for (i = FREE_DUEL_STORY_OPPONENT_FIRST_INDEX;
         i < FREE_DUEL_STORY_OPPONENT_INDEX_END; i++) {
        if (Campaign_TestStoryFlag(FREE_DUEL_UNLOCK_FLAG_BASE + i) == 0) {
            gFreeDuel_abGridAvailable[i] = 0;
        }
    }
    do {
    } while (IsIdleGPU(10) != 0);
    count = 0;
    clut = &D_800E9D70.clut;
    clut->x = 128;
    clut->y = 496;
    clut->w = 64;
    clut->h = 1;
    for (row = 0; row < 5; row++) {
        D_800E9D70.img.x = 128;
        D_800E9D70.img.y = row * 48 + 256;
        D_800E9D70.img.w = 24;
        D_800E9D70.img.h = 48;
        for (col = 0; col < FREE_DUEL_GRID_COLUMN_COUNT; col++) {
            LoadImage2(&D_800E9D70.img, (u32 *)src);
            LoadImage2(&D_800E9D70.clut,
                       (u32 *)(src + FREE_DUEL_PORTRAIT_IMAGE_SIZE));
            D_800E9D70.img.x += 24;
            D_800E9D70.clut.y++;
            if ((s16)D_800E9D70.clut.y >= 512) {
                D_800E9D70.clut.y = 496;
                D_800E9D70.clut.x += 64;
            }
            count++;
            src += FREE_DUEL_PORTRAIT_RECORD_SIZE;
        }
    }
    for (row = 0; row < 5; row++) {
        D_800E9D70.img.x = 256;
        D_800E9D70.img.y = row * 48 + 256;
        D_800E9D70.img.w = 24;
        D_800E9D70.img.h = 48;
        for (col = 0; col < FREE_DUEL_GRID_COLUMN_COUNT; col++) {
            LoadImage2(&D_800E9D70.img, (u32 *)src);
            LoadImage2(&D_800E9D70.clut,
                       (u32 *)(src + FREE_DUEL_PORTRAIT_IMAGE_SIZE));
            count++;
            if (count >= FREE_DUEL_GRID_ENTRY_COUNT) {
                goto done;
            }
            D_800E9D70.img.x += 24;
            D_800E9D70.clut.y++;
            src += FREE_DUEL_PORTRAIT_RECORD_SIZE;
            if ((s16)D_800E9D70.clut.y >= 512) {
                D_800E9D70.clut.y = 496;
                D_800E9D70.clut.x += 64;
            }
        }
    }
done:
    for (i = 0; i < 25; i++) {
        if (gFreeDuel_abGridAvailable[i] != 0) {
            obj = func_800400AC(func_8004002C(), 1);
            func_80040510(obj,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 56 + 20,
                          (i / FREE_DUEL_GRID_COLUMN_COUNT) * 52 + 40, 48, 48,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 48,
                          (i / FREE_DUEL_GRID_COLUMN_COUNT) * 48, 18,
                          (i / 16) * 64 + 128, (i & 15) + 496);
            obj->flags |= 0x1000000;
            obj->attr &= ~8;
        }
    }
    for (k = 25, i = 0; i < 15; i++, k++) {
        if (gFreeDuel_abGridAvailable[k] != 0) {
            obj = func_800400AC(func_8004002C(), 1);
            func_80040510(obj,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 56 + 20,
                          (k / FREE_DUEL_GRID_COLUMN_COUNT) * 52 + 40, 48, 48,
                          (i % FREE_DUEL_GRID_COLUMN_COUNT) * 48,
                          (i / FREE_DUEL_GRID_COLUMN_COUNT) * 48, 20,
                          (k / 16) * 64 + 128, (k & 15) + 496);
            obj->flags |= 0x1000000;
            obj->attr &= ~8;
        }
    }
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 0, 0, 0, 0, 0, 16, 0, D_801AF000);
    func_800428EC(obj, 10);
    obj->flags |= 0x1000000;
    obj->attr |= 8;
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 0, 0, 0, 0, 1, 16, 0, D_801AF000);
    func_800428EC(obj, -10);
    obj->flags |= 0x1000000;
    obj->attr |= 0x28;
    obj = func_800400AC(func_8004002C(), 2);
    func_800428A8(obj, 0, 0, 0, 0, 2, 17, 3, D_801AF000);
    obj->mode = 128;
    func_800428EC(obj, 15);
    obj->attr |= 0x28;
    gFreeDuel_pThumbWidget = (u8 *)obj;
    obj = FreeDuel_SpawnSparkle();
    gFreeDuel_pCursorWidget = (u8 *)obj;
    obj->flags &= ~0x8000000;
    if (gFreeDuel_bReturnFlags == 0) {
        obj->attr &= ~0x40;
        FreeDuel_PlaceCursor((u8 *)obj, 0);
    } else {
        FreeDuel_PlaceCursor((u8 *)obj, 1);
    }
    FreeDuel_UpdateScrollbar();
    func_8003FF08(29376);
}
