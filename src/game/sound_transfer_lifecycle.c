#include "../types.h"
#include "func_8004A6D8.h"
#include "../psyq/libspu.h"
#include "sound.h"
#include "sound_event_runtime.h"
#include "sound_init.h"
#include "sound_transfer_lifecycle.h"

void func_80049640(void)
{
    s32 value;

    func_8004A6D8();
    value = D_8009B458->field_081C;
    if (value > 0) {
        if (value < 4)
            func_8004B910();
    }
    func_80049434();
}

void SD_Term(void)
{
    func_8004A6D8();
    func_80049434();
    SpuQuit();
}

/*
 * Opens the pBAV header at music_track + 0x50 for the secondary sound bank.
 * The header size is (ps << 9) + 0xA20; the remaining file bytes form the
 * body later submitted by func_8004975C or func_800497E0.
 */
s32 SD_VabOpenHead(u8 *vab, s16 vab_id, s32 spu_addr)
{
    /* The unused frame and zero/request ordering preserve retail codegen. */
    volatile s32 pad[2];
    s32 zero;
    s32 requested;
    SDSecondaryState *initial;
    SDSecondaryState *state;
    SDSecondaryTransfer *entry;

    do {
        zero = 0;
    } while (0);
    requested = vab_id;
    initial = D_8009B458;
    initial->bytes_consumed = 0;
    if (requested == SD_TRANSFER_STATE_INACTIVE &&
        initial->transfer.field_0000 != requested) {
        return SD_TRANSFER_ERROR;
    }
    state = D_8009B458;
    state->transfer.field_0000 = zero;
    entry = &state->transfer;
    entry->field_0004 = vab;
    entry->field_0008 = (*(u16 *)(vab + 0x12) << 9) + 0xA20;
    entry->field_0010 = *(s32 *)(vab + 0x0C) - entry->field_0008;
    entry->field_0018 = vab[0x18];
    entry->field_001B = vab[0x19];
    entry->field_0014 = (u8 *)spu_addr;
    return 0;
}

s32 func_8004975C(s32 value, s16 expected)
{
    register int saved;
    SDSecondaryState *state = D_8009B458;
    short current = state->transfer.field_0000;

    if (current != expected)
        return SD_TRANSFER_ERROR;
    saved = value;
    {
        SDSecondaryTransfer *entry = &state->transfer;
        SpuSetTransferStartAddr((u32)entry->field_0014);
        if (SpuWrite((u8 *)saved, (u32)entry->field_0010) !=
            entry->field_0010)
            return SD_TRANSFER_ERROR;
        entry->field_000C = saved;
    }
    return current;
}
