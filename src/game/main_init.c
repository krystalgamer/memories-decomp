#include "../types.h"
#include "../psyq/libapi.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libetc.h"
#include "../psyq/setjmp.h"
#include "../psyq/rand.h"

extern volatile u32 D_8009B0CC;
extern volatile u32 D_8009B0C8;
extern volatile u8 D_8009B0C0;
extern volatile s32 D_8009B09C;
extern volatile u8 D_8009B0C3;
extern volatile u8 D_8009B0C1;
extern volatile s32 D_8009B0D8;
extern volatile s16 D_8009B098;
extern volatile u8 D_8009B0D1;
extern u8 D_8009B230 __attribute__((section(".data")));
extern void *volatile D_8009B0B4;
extern volatile s32 D_8009B0C4;
extern u8 D_8009B269 __attribute__((section(".data")));
extern u8 D_8009B4A8[];
extern u8 D_800E9EC0[];
extern jmp_buf D_800E9DC0;

extern void func_80012A78(void);
extern void func_800854C0(void);
extern void func_80015D0C(void);
extern void func_80013154(void);
extern void func_800403F0(void);
extern void func_800151B0(void);
extern void func_800134B4(void);
extern void func_80035A58(void);
extern void func_80035A64(void);
extern void func_8003B5C8(void);
extern void SD_InitState(s32);
extern void Main_VBlankCB(void);
extern void Sound_InitFrontend(void);
extern void Main_ResetFrontendRuntime(void);
extern void func_80043960(s32);
extern void func_8005B85C(void);
extern void func_800137E4(void);
extern s32 func_80043BCC(void);
extern void func_8002D458(s32);
extern void Main_Loop(void);

s32 Main_Init(void)
{
    s32 r;
    s32 t;
    register void *p __asm__("$4");

    func_80012A78();
    EnterCriticalSection();
    ResetCallback();
    func_800854C0();
    ExitCriticalSection();
    StopCallback();
    SetMem(2);
    SetDispMask(0);
    func_80015D0C();
    p = D_8009B4A8;
    D_8009B0CC = 0;
    D_8009B0C8 = 0;
    D_8009B0C0 = 0;
    D_8009B09C = 0;
    t = D_8009B09C;
    D_8009B0C3 = 0;
    D_8009B0C1 = 0;
    D_8009B0D8 = 1;
    D_8009B098 = 0x5000;
    D_8009B0D1 = 0;
    D_8009B230 = 1;
    D_8009B0B4 = p;
    D_8009B0C4 = t;
    func_80013154();
    func_800403F0();
    func_800151B0();
    func_800134B4();
    func_80035A58();
    func_80035A64();
    func_8003B5C8();
    SD_InitState(D_800E9EC0[0]);
    VSyncCallback(Main_VBlankCB);
    Sound_InitFrontend();
    srand(0x55555555);
    SetDispMask(1);
    Main_ResetFrontendRuntime();
    Main_ResetFrontendRuntime();
    func_80043960(0);
    r = setjmp(D_800E9DC0);
    Main_ResetFrontendRuntime();
    if (r != 0) {
        func_8005B85C();
        func_800137E4();
    }
    func_8002D458(func_80043BCC());
    D_8009B269 = 8;
    Main_Loop();
    return 0;
}
