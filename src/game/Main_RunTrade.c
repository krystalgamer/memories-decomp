#include "../types.h"
#include "display_object_api.h"
#include "../overlays/main_menu/entrypoints.h"

typedef struct {
    u8 pad00[0x59];
    u8 field59;
} Box;

typedef struct {
    u8 pad00[4];
    u32 field04;
    u16 field08;
    u8 pad0A[0x30 - 0xA];
    s16 field30;
    s16 field32;
    u8 pad34[0x60 - 0x34];
    u16 field60;
} Obj;

/* Defined rather than declared: the assembler only resolves a small global
   gp-relative when the translation unit defines it, which is the form the
   target uses for all three. c_symbols.ld overrides these common symbols, so
   no storage is allocated here. */
u8 D_8009B269;
u8 D_8009B26C;
u8 D_8009B26E;

extern u8 D_800EB0F8[];
/* Spelt as an incomplete array so -G8 keeps the four-byte slot out of small
   data; the target addresses it with a %hi/%lo pair and keeps the high half
   in $s2 across the tail. The section(".data") spelling is not equivalent
   here - it leaves GCC emitting the assembler macro form instead. */
extern u8 D_800E9EF0[];
#define gTradeObj (*(Obj **)D_800E9EF0)
extern volatile u16 gInput_wPad1Pressed __attribute__((section(".data")));
extern volatile u16 gInput_wPad2Pressed __attribute__((section(".data")));

extern void func_80032328(void);
extern void SD_BGMPlay(u32);
extern Box *TextBox_CreateFlagged(s32, s32, s32, s32, s32, s32, s32);
extern void func_80039A14(void *);
extern void func_80042918(Obj *);
extern void func_800428EC(Obj *, s32);
extern void func_80015A00(void);
extern void SD_SEPlayFull(s32);
extern void func_80043178(Obj *);
extern void func_80043230(Obj *, s32, s32, s32);
extern void TextBox_SetPos(void *, s32, s32);
extern void TextBox_Destroy(void *);
extern void func_8004036C(Obj *);
extern s32 DuelEffect_UpdateState(void);
extern void SD_BGMFadeOut(void);
extern void Fade_WaitOut(void);
extern void func_80183FE4(void);

void Main_RunTrade(void)
{
    Box *box;
    Obj *obj;
    s32 y;

    if ((D_8009B26C & 0x40) == 0) {
        D_8009B26C = D_8009B26C | 0x40;
        func_80032328();
        SD_BGMPlay(0x72D0);
        MainMenu_InitTradeScreen();
        D_8009B26E = 1;
        box = TextBox_CreateFlagged(0, 0xB, 0x18, 0x20, 0x110, 0xA0, 0x20);
        box->field59 = 0x10;
        func_80039A14(box);
        obj = func_800400AC(func_8004002C(), 2);
        func_800404CC(obj, 0, 0, 0, 4, 0xB, 0xC, 0x208);
        obj->field08 = obj->field08 | 0x20;
        obj->field04 = obj->field04 | 0x40000000;
        func_80042918(obj);
        func_800428EC(obj, 0xF);
        gTradeObj = obj;
        func_80015A00();
    }

    box = (Box *)D_800EB0F8;
    obj = gTradeObj;
    switch (D_8009B26E) {
    case 1:
        if (((gInput_wPad1Pressed | gInput_wPad2Pressed) & 0xE0) != 0) {
            SD_SEPlayFull(0x1E);
            func_80043178(obj);
            obj->field60 = 0x400;
            D_8009B26E = 2;
        }
        break;
    case 2:
        y = obj->field60 - 0x20;
        obj->field60 = y;
        func_80043230(obj, 0, 0xF0, (s16)y);
        TextBox_SetPos(box, obj->field30 + 0x18, obj->field32 + 0x20);
        if ((s16)obj->field60 <= 0) {
            D_8009B26E = 0;
            TextBox_Destroy(box);
            func_8004036C(gTradeObj);
            gTradeObj = 0;
        }
        break;
    default:
        if (DuelEffect_UpdateState() == 0) {
            if (MainMenu_UpdateTradeScreen() != 0) {
                SD_BGMFadeOut();
                Fade_WaitOut();
                func_80183FE4();
                D_8009B26C = D_8009B269;
            }
        }
        break;
    }
}
