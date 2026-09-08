#include "../../types.h"
#include "../../game/display_object_api.h"
#include "name_entry_keyboard.h"
#include "../../game/save_data.h"
#include "name_entry_frame.h"
#include "../../game/gpu_packets.h"
#include "../../game/sound.h"
#include "../../game/display_object_helpers.h"
#include "../../game/text_box_runtime.h"

extern u8 D_8016D400;
extern u8 D_8016D401;
extern u8 D_8016D402;
extern u8 D_8016D403;
extern u8 D_8016D408;
extern u8 *D_8016D418;
extern u8 D_8016D426;
extern u8 D_8016D42C;
extern u8 *D_8016D404;
extern s16 D_8016D434;
extern s16 D_8016D436;
extern u8 *D_8016D43C;
extern u8 D_8016D4D0;
extern s16 D_8016D4D2;
extern u8 D_801B125A[];
extern u8 gSaveData_aPlayerNameSjis[];
extern void func_800403F0(void);
extern void func_8003BC40(u8 *, u8 *, s32);
extern void func_8003B6AC(s32, s32);
extern DuelEffectChannel *func_80035BE4(s32, s32, s32, s32, s32, s32);
extern void func_80040510(u8 *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80015A00(void);

void NameEntry_BuildKeyboardTextBox(s32 textOffset)
{
    DuelEffectChannel *object;

    func_8003B6AC(1, 1);
    object = func_80035BE4(1, textOffset + 0xF0, 0x16, 0x18, 0x140, 0xF0);
    object->field_5A = 0x14;
    object->field_5B = 0x12;
    func_80039A14((u8 *)object);
}

void NameEntry_DrawSelectionFrame(NameEntrySelectionFrameView *r, GsOT *ot)
{
    LINE_F3 *poly;
    LINE_G2 *line;
    s32 pri;
    s32 x;
    s32 y;
    u32 w;
    u32 h;
    s32 xm1;
    s32 xp3;
    s32 right;

    poly = (LINE_F3 *)0x1F800000;
    line = (LINE_G2 *)0x1F800040;
    /* Keep packed color writes; the constructors fill command bytes afterward. */
    *(u32 *)&poly->r0 = 0x0000FF00;
    *(u32 *)&line->r0 = 0x0000FF00;
    *(u32 *)&line->r1 = 0;
    /* Keep the signed load; narrow only at the packet-submit boundary. */
    pri = r->priority;
    x = r->x;
    y = r->y;
    w = r->width;
    h = r->height;
    setLineF3(poly);
    setLineG2(line);
    xm1 = x - 1;
    xp3 = x + 3;
    poly->x1 = xm1;
    poly->x0 = xm1;
    poly->x2 = xp3;
    poly->y2 = y - 1;
    poly->y1 = y - 1;
    poly->y0 = y + 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    right = x + w;
    poly->x1 = right + 1;
    poly->x0 = right + 1;
    poly->x2 = right - 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    poly->y2 = y + h + 1;
    poly->y1 = y + h + 1;
    poly->y0 = y + h - 3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    poly->x1 = xm1;
    poly->x0 = xm1;
    poly->x2 = xp3;
    func_8005B260((u32 *)poly, ot, (u16)pri, 1);
    line->x1 = x + (w >> 1);
    line->x0 = x + (w >> 1);
    line->y0 = y + 2;
    line->y1 = 0;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->y0 = y + h - 2;
    line->y1 = 192;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->y1 = y + (h >> 1);
    line->y0 = y + (h >> 1);
    line->x0 = x + 2;
    line->x1 = 0;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
    line->x0 = right - 2;
    line->x1 = 320;
    func_8005B260((u32 *)line, ot, (u16)pri, 1);
}

void NameEntry_Init(void)
{
    u8 *obj;
    DuelEffectChannel *boxes;
    DuelEffectChannel *sprite;

    SD_BGMPlay(28688);
    func_800403F0();
    D_8016D400 = 0;
    D_8016D403 = 0;
    D_8016D408 = 0;
    D_8016D418 = gSaveData_aPlayerNameSjis;
    func_8003BC40(D_801B125A, gSaveData_aPlayerNameSjis,
                  SAVE_DATA_PLAYER_NAME_CHAR_COUNT);
    func_8003B6AC(3, 1);
    func_80035BE4(3, 254, 112, 204, 96, 16);
    boxes = D_800EB0F8;
    boxes[3].field_5A = 16;
    boxes[3].field_5B = 16;
    func_80039A14((u8 *)&boxes[3]);
    func_8003B6AC(0, 1);
    sprite = func_80035BE4(0, 243, 262, 60, 100, 100);
    sprite->field_5A = 20;
    sprite->field_5B = 18;
    func_80039A14(sprite);
    D_8016D4D0 = 2;
    NameEntry_BuildKeyboardTextBox(2);
    D_8016D426 = 0;
    D_8016D402 = 0;
    D_8016D401 = 0;
    D_8016D42C = 0;
    obj = func_800400AC(func_8004002C(), 6);
    D_8016D434 = 22;
    *(s16 *)(obj + 0x30) = 22;
    D_8016D436 = 24;
    *(s16 *)(obj + 0x32) = 24;
    *(s16 *)(obj + 0x3C) = 16;
    *(s16 *)(obj + 0x3E) = 16;
    func_80042918(obj);
    func_800428EC(obj, 10);
    *(void **)(obj + 0x4C) = NameEntry_DrawSelectionFrame;
    D_8016D404 = obj;
    obj = func_800400AC(func_8004002C(), 1);
    func_80040510(obj, 107, 199, 32, 32, 144, 128, 23, 256, 240);
    *(s16 *)(obj + 0x4A) = 13;
    *(s16 *)(obj + 0x48) = 13;
    *(s32 *)(obj + 4) = *(s32 *)(obj + 4) | 0x40000000;
    func_80042918(obj);
    func_800428EC(obj, 10);
    D_8016D43C = obj;
    obj = func_800400AC(func_8004002C(), 3);
    func_80040510(obj, 0, 0, 320, 240, 0, 0, 20, 256, 243);
    *(s32 *)(obj + 0xC) = 0x404040;
    *(s32 *)(obj + 4) = *(s32 *)(obj + 4) | 0x1000000;
    func_8004293C(obj);
    D_8016D4D2 = 244;
    func_80015A00();
}
