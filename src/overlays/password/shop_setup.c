#include "../../types.h"
#include "../../psyq/libgte.h"
#include "../../psyq/libgpu.h"
#include "../../psyq/libgs.h"
#include "../../game/display_object_api.h"
#include "../../game/display_object_layout.h"
#include "shop.h"
#include "../../game/sound.h"
#include "../../game/display_object_helpers.h"

extern u8 gPassword_abDigits[];
extern u16 D_8016D424;
extern s32 gPassword_nDigitIndex;
extern u8 *D_8016D430;
extern u8 *gPassword_pDigitCursorWidget;
extern u8 *D_8016D440[];
extern u8 D_800EA0E8[];
extern void Password_RefreshDigitDisplay(void);
extern void Password_RefreshStarchipDisplay(void);
extern void func_80029528(s32);
extern void func_80029574(s32);
extern PasswordCardPreviewView *func_800291E0(s32, s32, s32);
extern void func_80040510(u8 *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void Password_SetDigitCursorTarget(u8 *);
extern void func_80040468(u8 *, s32, s32, s32, s32, s32);
extern void func_80015A00(void);

void Password_RecreateCardPreview(s32 ignored)
{
    PasswordCardPreviewView *obj;

    func_80029528(0);
    obj = func_800291E0(0, -1, -1);
    obj->y = 0x1E;
    obj->phase = 0x80;
    obj->flags |= DISPLAY_OBJECT_FLAG_CLIP_TEST;
    D_8016D4D8 = obj;
}

void Password_InitShopScreen(void)
{
    s32 i;
    u8 *o;
    u8 *cardCache;
    u8 **slot;
    PasswordCursorUpdate hook;
    u8 *p;

    i = 7;
    p = gPassword_abDigits + i;
    gPassword_nDigitIndex = 0;
    D_8016D424 = 0;
    do {
        *p = 0;
        i--;
        p--;
    } while (i >= 0);
    Password_RefreshDigitDisplay();
    Password_RefreshStarchipDisplay();
    Password_CreateMessageBox(226, 1);
    D_8016D430 = D_800EA0E8;
    func_80029574(0);
    cardCache = D_8016D430;
    *(s16 *)(cardCache + 40) = 320;
    *(s16 *)(cardCache + 42) = 256;
    *(s16 *)(cardCache + 44) = 512;
    *(s16 *)(cardCache + 46) = 240;
    o = func_800400AC(func_8004002C(), 2);
    func_800404CC(o, 152, 40, 0, 2, 3, 31, 257);
    func_800428EC(o, -8);
    *(u16 *)(o + 8) |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    Password_RecreateCardPreview(1);
    o = func_800400AC(func_8004002C(), 1);
    func_80040510(o, 256, 120, 32, 32, 16, 128, 30, 256, 240);
    *(u32 *)(o + 4) &= ~GsROTOFF;
    *(s16 *)(o + 72) = 13;
    *(s16 *)(o + 74) = 13;
    *(u32 *)(o + 4) |= (GsALON | GsAONE);
    func_80042918(o);
    func_800428EC(o, 10);
    *(PasswordCursorUpdate *)(o + 36) = Password_UpdateDigitCursor;
    Password_SetDigitCursorTarget(o);
    hook = Password_UpdateDigitCursorDecoration;
    slot = D_8016D440;
    *(u32 *)(o + 48) = *(u32 *)(o + 24);
    gPassword_pDigitCursorWidget = o;
    i = 0;
    do {
        o = func_800400AC(func_8004002C(), 2);
        func_80040468(o, 3, 1, i, 11, 524);
        *(u32 *)(o + 4) |= GsALON;
        func_80042918(o);
        func_800428EC(o, 8);
        *(PasswordCursorUpdate *)(o + 36) = hook;
        *(u16 *)(o + 8) |= 0x28;
        *slot = o;
        i++;
        slot++;
    } while (i < 4);
    SD_BGMPlay(29520);
    func_80015A00();
}
