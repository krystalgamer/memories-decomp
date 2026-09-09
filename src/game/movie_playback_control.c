#define GINPUT_PAD1_PRESSED_IS_AGGREGATE
#include "../types.h"
#define D_8009B142_IS_AGGREGATE
#include "graphics_frame.h"
#include "file_transfer.h"
#include "input.h"
#include "mdec_sync.h"
#include "movie_playback_control.h"
#include "sound_mix.h"

extern void func_8005C388(s32, s32, s32, s32, s32);
extern s32 func_8005C530(void);

void func_8003594C(s32 arg0)
{
    File_WaitForTransfers();
    func_80044F58(0xFF);
    D_8009B318 = 0x80;
    D_8009B0F4_abs |= 0x2000000;
    func_8005C388(arg0, 1, -1, 1, 0);
}

void func_800359B0(void)
{
    if (func_8005C530() == 0) {
        u8 value = D_8009B318;

        if ((value & 0x40) ||
            (gInput_wPad1Pressed[0] & PAD_BUTTON_START)) {
            D_8009B318 = value | 0x40;
            D_8009B144[0] = 1;
            D_8009B143[0] = 1;
            D_8009B142[0] = 1;
            func_8005C5C4();
        }
    } else {
        D_8009B0F4_abs = D_8009B0F4_abs & 0xFDFFFFFF;
        D_8009B318 &= 0x7F;
    }
}

void func_80035A58(void)
{
    D_8009B318 = 0;
}
