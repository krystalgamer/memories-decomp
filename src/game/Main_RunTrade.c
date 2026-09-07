#include "../types.h"

extern u8 D_8009B269;
extern u8 D_8009B26C;
extern u8 D_8009B26E;
extern u16 D_8009B398 __attribute__((section(".data")));
extern u16 D_8009B39A __attribute__((section(".data")));
extern u8 *D_800E9EF0[];
extern u8 D_800EB0F8[];

extern void func_80032328(void);
extern void SD_BGMPlay(s32);
extern void func_80181F68(void);
extern u8 *TextBox_CreateFlagged(s32, s32, s32, s32, s32, s32, s32);
extern void func_80039A14(u8 *);
extern void *func_8004002C(void);
extern u8 *func_800400AC(void *, s32);
extern void func_800404CC(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_80042918(u8 *);
extern void func_800428EC(u8 *, s32);
extern void func_80015A00(void);
extern void SD_SEPlayFull(s32);
extern void func_80043178(u8 *);
extern void func_80043230(u8 *, s32, s32, s32);
extern void TextBox_SetPos(u8 *, s32, s32);
extern void TextBox_Destroy(u8 *);
extern void func_8004036C(u8 *);
extern s32 DuelEffect_UpdateState(void);
extern s32 func_801821DC(void);
extern void SD_BGMFadeOut(void);
extern void Fade_WaitOut(void);
extern void func_80183FE4(void);

void Main_RunTrade(void)
{
    u8 flags = D_8009B26C;
    u8 *box;
    u8 *obj;
    u8 state;
    s32 height;
    u8 value;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_80032328();
        SD_BGMPlay(0x72D0);
        func_80181F68();
        D_8009B26E = 1;
        box = TextBox_CreateFlagged(0, 11, 24, 32, 272, 160, 32);
        box[89] = 16;
        func_80039A14(box);
        obj = func_800400AC(func_8004002C(), 2);
        func_800404CC(obj, 0, 0, 0, 4, 11, 12, 520);
        *(u16 *)(obj + 8) |= 0x20;
        *(s32 *)(obj + 4) |= 0x40000000;
        func_80042918(obj);
        func_800428EC(obj, 15);
        D_800E9EF0[0] = obj;
        func_80015A00();
    }

    box = D_800EB0F8;
    state = D_8009B26E;
    obj = D_800E9EF0[0];
    switch (state) {
    case 1:
        if (((D_8009B398 | D_8009B39A) & 0xE0) != 0) {
            SD_SEPlayFull(30);
            func_80043178(obj);
            *(u16 *)(obj + 96) = 1024;
            D_8009B26E = 2;
        }
        break;
    case 2:
        height = *(u16 *)(obj + 96) - 32;
        *(u16 *)(obj + 96) = height;
        func_80043230(obj, 0, 240, (s16)height);
        TextBox_SetPos(box, *(s16 *)(obj + 48) + 24, *(s16 *)(obj + 50) + 32);
        if (*(s16 *)(obj + 96) <= 0) {
            D_8009B26E = 0;
            TextBox_Destroy(box);
            func_8004036C(D_800E9EF0[0]);
            D_800E9EF0[0] = 0;
        }
        break;
    default:
        if (DuelEffect_UpdateState() == 0) {
            if (func_801821DC() != 0) {
                SD_BGMFadeOut();
                Fade_WaitOut();
                func_80183FE4();
                value = D_8009B269;
                __asm__ volatile("nop");
                D_8009B26C = value;
            }
        }
        break;
    }
}
