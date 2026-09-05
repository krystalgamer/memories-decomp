#include "../types.h"

typedef struct {
    u8 pad_000[0x500];
    u8 active;
    u8 pad_501[2];
    u8 guard;
    s32 event;
} SoundState;

extern SoundState *D_8009B458;

extern void func_800738B0(void);
extern void func_80073A54(s32);
extern s32 func_80073860(s32, s32, s32, void (*)(void));
extern void func_80073890(s32);
extern void func_80073950(s32, s32, s32);
extern void func_80073A24(s32);
extern void func_800738C0(void);
extern void func_8004B734(void);

void func_8004B854(void)
{
    s32 event;

    if (D_8009B458->guard)
        return;

    D_8009B458->guard = 1;
    func_800738B0();
    func_80073A54(0xF2000002);
    {
        register s32 descriptor asm("$4") = 0xF2000002;
        register s32 mode asm("$5") = 2;
        register s32 flags asm("$6") = 0x1000;
        register void (*callback)(void) asm("$7") = func_8004B734;

        event = func_80073860(descriptor, mode, flags, callback);
    }
    D_8009B458->event = event;
    func_80073890(event);
    func_80073950(0xF2000002, 0xE000, 0x1000);
    func_80073A24(0xF2000002);
    func_800738C0();
    D_8009B458->active = 0;
    D_8009B458->guard = 0;
}
