#define GINPUT_PAD1_PRESSED_IN_DATA_VOLATILE
#include "../types.h"
#include "func_8002C604.h"
#include "input.h"
#include "duel_card.h"
#include "display_object_api.h"
#include "file_transfer.h"

extern volatile u16 gInput_wPad1Held __attribute__((section(".data")));
extern volatile u16 gInput_wPad1Repeat __attribute__((section(".data")));

extern u16 D_8009B23A;
extern u8 D_8009B16C[4];
extern u8 D_8009AF2A;
extern u8 D_8009AF2C[2];
extern u8 D_8009AF2D;
extern u8 D_8009AF2E;
extern u8 *D_8009B180;
extern u8 *D_8009B184;
extern u16 D_800908A0[];
extern DuelCardRecord D_801A7B80[];

extern void func_800220B8(void);
extern void func_80029528(s32);
extern void func_80029164(s32, s32);
extern u8 *func_800291E0(s32, s32, s32);
extern void func_80012D84(s32);
extern void func_8001944C(u8 *);
extern u8 *func_80017F04(DuelCardRecord *, s32, s32);

/* Debug display controller: START hands the pad to func_800220B8; on the
   first call it initialises the D_8009B23A mode flags and the cursor state.
   With the rebuild flag set it releases the two spawned objects and
   recreates the display for the current page (a message box, one or two
   card objects from D_801A7B80). Otherwise SELECT cycles the page (0..3),
   left/right toggles which coordinate the up/down repeat adjusts, and CROSS
   places a new object through func_8002C604 at a page-dependent position. */
void func_800222F4(void) {
    u8 *obj;
    u8 *p;

    if (gInput_wPad1Held & PAD_BUTTON_START) {
        func_800220B8();
    }
    if ((D_8009B23A & 0x8000) == 0) {
        D_8009B23A |= 0xC000;
        D_8009B16C[2] = 0;
        D_8009AF2E = 0;
        D_8009AF2A = 0;
        D_8009AF2D = 0;
        D_8009B184 = 0;
        D_8009B180 = 0;
    }
    if (D_8009B23A & 0x4000) {
        D_8009B23A &= 0xBFFF;
        func_80029528(0);
        func_8004036C(D_8009B180);
        func_8004036C(D_8009B184);
        D_8009B184 = 0;
        D_8009B180 = 0;
        switch (D_8009AF2E) {
        case 0:
            break;
        case 1:
            func_80029164(0, 1);
            File_WaitForTransfers();
            obj = func_800291E0(0, -1, -1);
            *(s16 *)(obj + 0x30) = 0x5A;
            *(s16 *)(obj + 0x32) = 0x16;
            func_80012D84(4);
            func_8001944C(obj);
            break;
        case 2:
            D_8009B180 = func_80017F04(D_801A7B80, 0x86, 0x52);
            break;
        case 3:
            D_8009B180 = func_80017F04(D_801A7B80, 0x5C, 0x52);
            D_8009B184 = func_80017F04(&D_801A7B80[1], 0xB0, 0x52);
            break;
        }
    } else if (gInput_wPad1Pressed & PAD_BUTTON_SELECT) {
        D_8009B23A |= 0x4000;
        D_8009AF2E++;
        if (D_8009AF2E >= 4) {
            D_8009AF2E = 0;
        }
    } else if (gInput_wPad1Pressed & (PAD_DIRECTION_LEFT | PAD_DIRECTION_RIGHT)) {
        D_8009AF2A ^= 1;
    } else if (gInput_wPad1Repeat & (PAD_DIRECTION_UP | PAD_DIRECTION_DOWN)) {
        D_8009AF2C[D_8009AF2A]++;
        if (gInput_wPad1Repeat & PAD_DIRECTION_DOWN) {
            D_8009AF2C[D_8009AF2A] -= 2;
        }
    } else if (gInput_wPad1Pressed & PAD_BUTTON_CROSS) {
        p = func_8002C604(D_8009AF2C[0]);
        D_8009B16C[2] = (D_8009B16C[2] + 1) & 7;
        *(s16 *)(p + 0x1A) = D_8009AF2D;
        switch (D_8009AF2E) {
        case 0:
            *(s16 *)(p + 0) = D_800908A0[0xC];
            *(s16 *)(p + 2) = -0x18;
            *(s16 *)(p + 4) = D_800908A0[0xD];
            break;
        case 1:
            *(s16 *)(p + 0) = 0xA0;
            *(s16 *)(p + 2) = 0x78;
            break;
        case 2:
            *(s16 *)(p + 0) = 0xA0;
            *(s16 *)(p + 2) = 0x70;
            break;
        case 3:
            *(s16 *)(p + 0) = 0xA0;
            *(s16 *)(p + 2) = 0x70;
            break;
        }
    }
}
