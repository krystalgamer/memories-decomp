#include "../types.h"
#include "duel_effect_request.h"
#include "duel_effect_object_pool.h"
#include "save_data.h"

int func_8002C570(int offset)
{
    u8 *object;

    object = (u8 *)((u32)D_801D0000 + offset);
    if (object[SAVE_DATA_HEADER_SIZE + SAVE_DATA_CARD_QUANTITIES_OFFSET -
               CARD_ID_FIRST]) {
        return 1;
    }
    return -1;
}

void func_8002C598(void)
{
    DuelEffectRequest *entry;
    int count;
    int fill;

    D_8009B260 = 0;
    count = DUEL_EFFECT_REQUEST_COUNT;
    fill = -1;
    entry = D_800EAD88;
    do {
        entry->flags = 0;
        entry->id = fill;
        entry++;
    } while (--count != 0);
}

DuelEffectRequest *func_8002C5CC(void)
{
    DuelEffectRequest *entry = D_800EAD88;
    int count = DUEL_EFFECT_REQUEST_COUNT;

    for (;;) {
        if (!(entry->flags & 0x80)) {
            return entry;
        }
        if (--count == 0) {
            return 0;
        }
        entry++;
    }
}
