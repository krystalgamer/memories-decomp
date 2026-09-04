#include "../types.h"

extern s32 D_8009B350;

extern void Library_UpdateCardUsedFlag(s32);
extern void func_8004036C(void *);

void func_80038AB0(u8 *object)
{
    register u32 value asm("$3");
    register s32 card_id asm("$16");
    u8 **stream;
    u8 *cursor;

    stream = &((u8 **)object)[*(s8 *)(object + 0x58)];
    cursor = *stream;
    value = *cursor++;
    card_id = value;
    *stream = cursor;
    if (card_id > 0) {
        Library_UpdateCardUsedFlag(card_id + 0x1F);
        Library_UpdateCardUsedFlag(card_id + 0x6E0);
    }
}

void func_80038B08(u8 *object)
{
    func_8004036C(*(void **)(object + 0x30));
    *(void **)(object + 0x30) = 0;
    object[0x51] = 2;
    object[0x62] = 0;
    D_8009B350 = 1;
}
