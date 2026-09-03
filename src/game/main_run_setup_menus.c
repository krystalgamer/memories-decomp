#include "../types.h"

extern unsigned char D_8009B26C;
extern unsigned char D_8009B269;
extern void func_8003BBF8(void);
extern void func_801683EC(void);
extern int func_80169C08(void);
extern void func_8003BEB8(void);
extern void func_8016A080(void);
extern void func_8016A37C(void);
extern void func_8003C2B4(void);
extern void func_8003C628(void);
extern void func_80015A00(void);
extern void func_8003FF34(void);
extern int func_8003C8CC(void);

void Main_RunNameEntry(void)
{
    unsigned char flags = D_8009B26C;
    unsigned char value;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003BBF8();
        func_801683EC();
    }
    if (func_80169C08()) {
        value = D_8009B269;
        __asm__ volatile("nop");
        D_8009B26C = value;
    }
}

void Main_RunPasswordMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003BEB8();
        func_8016A080();
    }
    func_8016A37C();
}

void Main_RunOptionsMenu(void)
{
    unsigned char flags = D_8009B26C;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        func_8003C2B4();
        func_8003C628();
        func_80015A00();
    }
    if (func_8003C8CC() == 0) {
        unsigned char value;

        func_8003FF34();
        value = D_8009B269;
        __asm__ volatile("nop");
        D_8009B26C = value;
    }
}
