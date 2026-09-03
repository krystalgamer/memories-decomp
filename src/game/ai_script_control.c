#include "../types.h"

extern void func_8008E870();
extern int AiScript_ReadShort(void);
extern s32 AiScript_ReadByte(void);
extern s32 rand(void);

struct Big {
    s32 f0;
    s32 f4;
    s32 f8;
    u8 pad2[0x14 - 0xC];
    u8 count;
    u8 pad3[3];
    s32 items[1];
};

extern struct Big gAiScript_State;
extern char D_800118E4[];
extern char D_8009B084[];
extern char D_800118CC[];
extern s32 gAiScript_aMemory[];

/* Pops the top of gAiScript_State's item stack into f8; if the stack is already
   empty, prints an error and hangs (original panic-on-underflow handler). */
void AiScript_Return(void) {
    u8 count = gAiScript_State.count;
    if (count != 0) {
        count = count - 1;
        gAiScript_State.count = count;
        gAiScript_State.f8 = gAiScript_State.items[count];
        return;
    }
    func_8008E870(D_800118E4);
    func_8008E870(D_8009B084, D_800118CC, 0x193);
    for (;;)
        ;
}

/* Reads a [lo, hi] bound pair from the byte stream, picks a table slot, then
   stores a value from rand folded into [lo, hi] via modulo. */
void AiScript_SetRandom(void) {
    s32 lo = AiScript_ReadShort();
    s32 hi = AiScript_ReadShort();
    s32 idx = AiScript_ReadByte();

    gAiScript_aMemory[idx] = rand() % (hi - lo + 1) + lo;
}
