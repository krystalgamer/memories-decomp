#include "../types.h"

extern u8 D_8009B34E;
extern u8 D_8009B355;
extern s32 D_8009B350;

extern s32 func_80036D3C(u8 *);
extern void Library_UpdateCardUsedFlag(s32);
extern void func_8004036C(void *);

void func_800389C4(u8 *value)
{
    *(u16 *)(value + 0x34) &= (u16)~8;
}

void func_800389D8(u8 *object)
{
    s32 offset = *(s8 *)(object + 0x58) * 4;
    s32 value;

    *(s32 *)(object + offset) += D_8009B34E * 2;
    value = func_80036D3C(object);
    offset = *(s8 *)(object + 0x58) * 4;
    *(s32 *)(object + offset) =
        (*(s32 *)(object + offset) & 0xFFFF0000) | (value & 0xFFFF);
}

void func_80038A44(u8 *object)
{
    s32 offset = *(s8 *)(object + 0x58) * 4;
    s32 value;

    *(s32 *)(object + offset) += D_8009B355 * 2;
    value = func_80036D3C(object);
    offset = *(s8 *)(object + 0x58) * 4;
    *(s32 *)(object + offset) =
        (*(s32 *)(object + offset) & 0xFFFF0000) | (value & 0xFFFF);
}

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
