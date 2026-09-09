#include "../types.h"
#include "main_frame.h"
#include "display_object_api.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libcd.h"
#include "fade.h"
#include "file_transfer.h"
#include "func_80043960.h"
#include "graphics_constants.h"
#include "display_object_helpers.h"
#include "main_reset_frontend_runtime.h"
#include "sound_voice_selection.h"
#include "../unmatched.h"

extern u8 D_8009B428;
extern short D_8009B098[];
extern void func_800434F4(void);
extern void func_80043328(void);
extern void func_8004365C(void *, void *);
extern void func_800438B8(int);
extern void func_801680F4(void);
extern int func_80168160(int);
extern void func_8007AFA4(void);
void func_80043960(s32 mode)
{
    register u8 *object;
    register u8 *first;
    D_8009B428 = 0;
    if (mode == 0) {
        File_RequestAsyncTransfer(0, 0, 0x1F85, 0x22, func_800434F4, 0, 0);
        File_WaitForTransfers();
    }
    File_RequestAsyncTransfer(0, 0, 0x1690, 0x36, func_80043328, 0, 0);
    if (mode != 0) {
        int display;
        File_WaitForTransfers();
        FntLoad(0x2C0, 0);
        display = FntOpen(
            0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
            0, 1000
        );
        SetDumpFnt(display);
        D_8009B098[0] = 0;
        func_80047AD0(2);
        func_80012D84(4);
        File_WaitForTransfers();
        return;
    }
    Fade_InitIn();
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 0, 0, 0, 0, 0, 0x10, 0x100,
                  (int)D_801AF000);
    *(u16 *)(object + 8) |= 0x28;
    func_8004365C(0, object);
    func_800438B8(4);
    FntLoad(0x2C0, 0);
    SetDumpFnt(FntOpen(
        0x10, 0x10, GRAPHICS_DEFAULT_WIDTH, GRAPHICS_DEFAULT_HEIGHT,
        0, 1000
    ));
    D_8009B098[0] = 0;
    CdFlush();
    first = object;
    func_801680F4();
    while (func_80168160(1) != 0) {
    }
    func_8007AFA4();
    object = func_800400AC(func_8004002C(), 2);
    func_800428A8(object, 0, 0, 0, 0, 1, 0x10, 0x100,
                  (int)D_801AF000);
    *(u16 *)(object + 8) |= 0x28;
    func_8004365C(first, object);
    func_80047AD0(2);
    func_80012D84(4);
    File_RequestMainMenuPackage();
    func_800438B8(0xB4);
    func_80015AD8();
    Main_ResetFrontendRuntime();
}
