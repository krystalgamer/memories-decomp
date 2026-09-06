#include "../types.h"

extern u8 *D_8009B290;
extern u16 D_8009B27C;
extern u16 D_8009B29C;
extern u16 D_8009B2A8;
extern u16 D_8009B2AA;
extern u8 D_801A8000[];
extern void Library_UpdateCardUsedFlag(int);
extern int Campaign_TestStoryFlag(int);

void func_8002E918(void)
{
    register u8 *cursor __asm__("$5") = D_8009B290;
    register u8 *next __asm__("$6") = cursor + 2;
    register int high __asm__("$2");
    register int command __asm__("$3");
    int offset;

    D_8009B290 = next;
    high = cursor[1];
    command = cursor[0];
    command |= high << 8;
    if (command & 0x4000) {
        Library_UpdateCardUsedFlag(command & 0xBFFF);
    } else {
        int low;

        D_8009B290 = cursor + 4;
        high = next[1];
        low = cursor[2];
        offset = low | (high << 8);
        if (Campaign_TestStoryFlag(command)) {
            D_8009B290 = D_801A8000 + offset;
        }
    }
    D_8009B27C = 0;
}

void Script_OpViewportTween(void)
{
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B2A8 = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B2AA = cursor[0] | (cursor[1] << 8);
    }
    {
        u8 *cursor = D_8009B290;

        D_8009B290 = cursor + 2;
        D_8009B29C = cursor[0] | (cursor[1] << 8);
    }
    D_8009B27C = 7;
}
