#include "../types.h"
#include "card_constants.h"

extern u8 D_8009B2F8;
/* Retail addresses these three with %hi/%lo under -G8, so they live outside
   small data. */
extern u8 D_8009AF76 __attribute__((section(".data")));
extern u8 D_8009B140 __attribute__((section(".data")));
extern s8 gDialog_bChoice __attribute__((section(".data")));
extern u8 D_800EB0F8[];

extern s32 func_80032B38(u8 *);
extern void SD_SEPlayFull(s32);
extern s32 func_80033998(void);
extern u8 *TextBox_CreateFlagged(s32, s32, s32, s32, s32, s32, s32);
extern void func_80039794(void);
extern void func_80015BD8(s32, s32);
extern void TextBox_Destroy(void *);
extern void func_80032370(void);

/* Handles leaving the deck editor. When the editor's own check passes, the
 * confirm sound plays and, if the deck is complete (func_80033998), bit 14 of
 * the state word at +0x633E is set, a confirmation box is created (the wide
 * one in the 640-wide mode selected by bit 7 of D_8009B2F8, the narrow one
 * otherwise, which is then waited on until its +0x30 pointer is filled),
 * func_80015BD8(0xA0, 2) runs and D_8009B140 is set from D_8009AF76 - 8.
 * With bit 14 set the effect channel at D_800EB0F8 is polled: once its flags
 * read 0x2000 under the 0x2008 mask the box is destroyed and either bit 14
 * is cleared (narrow mode with a choice made) or the state word is reloaded
 * from +0x6340 and func_80015BD8(0xFF, 2) runs. Without bit 14 the CARD_COUNT
 * trunk bytes at +0x5D98 are copied after the DECK_SIZE halfwords of the
 * object at +0. Those halfwords are rebuilt from the 0x10-byte entries at
 * +0x2D54 whose byte 9 is set, func_80032370 runs and the state word is
 * cleared. */
void func_800339D0(u8 *state)
{
    u8 *box;
    u8 *src;
    u8 *dst;
    u16 *slot;
    u8 *entry;
    s32 mode;
    s32 i;

    if (func_80032B38(state) == 0) {
        SD_SEPlayFull(8);
        if (func_80033998() != 0) {
            /* The mode byte is read before the flag store, as retail
               schedules it. */
            mode = D_8009B2F8 & 0x80;
            *(u16 *)(state + 0x633E) |= 0x4000;
            if (mode) {
                TextBox_CreateFlagged(0, 8, 0x28, 0x78, 0xF0, 0x10, 0x1028)[0x59] = 0xA;
            } else {
                box = TextBox_CreateFlagged(0, 9, 0x30, 0x60, 0xE0, 0x30, 0x20);
                box[0x59] = 0xA;
                do {
                    func_80039794();
                } while (*(s32 *)(box + 0x30) == 0);
            }
            func_80015BD8(0xA0, 2);
            D_8009B140 = D_8009AF76 - 8;
        }
    }
    /* Computed before the branch: it is only used on the copy path, so it
       crosses no call, and reorg lifts it into the branch delay slot. */
    src = state + 0x5D98;
    if (*(u16 *)(state + 0x633E) & 0x4000) {
        func_80039794();
        /* The same variable as the confirmation box, which keeps the
           channel in $s0 across the destroy call. */
        box = D_800EB0F8;
        if ((*(u32 *)(box + 0x34) & 0x2008) == 0x2000) {
            TextBox_Destroy(box);
            if (!(D_8009B2F8 & 0x80) && gDialog_bChoice != 0) {
                *(u16 *)(state + 0x633E) &= 0xBFFF;
            } else {
                *(u16 *)(state + 0x633E) = *(u16 *)(state + 0x6340);
                func_80015BD8(0xFF, 2);
            }
        }
    } else {
        /* One counter for both loops, initialised before the pointers each
           time; that is what ranks the counter and the pointers into
           retail's argument registers. */
        i = 0;
        dst = *(u8 **)state + DECK_SIZE * sizeof(u16);
        for (; i < CARD_COUNT; i++) {
            *dst++ = *src++;
        }
        slot = *(u16 **)state;
        i = 0;
        entry = state + 0x2D54;
        for (; i < DECK_SIZE; i++) {
            *slot = 0;
            if (entry[9] != 0) {
                *slot = *(u16 *)entry;
            }
            slot++;
            entry += 0x10;
        }
        func_80032370();
        *(u16 *)(state + 0x633E) = 0;
    }
}
