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
 * SD_VabOpenHead: opens a VAB header for the secondary sound bank
 *
 * `vab` points at a "pBAV" header; both callers pass music_track + 0x50,
 * which is where every SD_BGM.DAT package keeps one. The header size is
 * (ps << 9) + 0xA20: the 0x20-byte VabHdr, 128 0x10-byte ProgAtr records,
 * ps * 16 0x20-byte VagAtr records and the 0x200-byte VAG offset table.
 * fsize (+0x0C) minus that is the body size, and mvol (+0x18) and pan
 * (+0x19) are kept beside them. `spu_addr` is where the body goes: the
 * packages' own +0x0C word, which with the body size at +8 ends every one at
 * SPU address 0x79020. SD_VabTransBody and func_800497E0 later SpuWrite the
 * body there. Asking for id -1 fails while the slot is still live.
 *
 * The 152-byte function at `0x800496C4` matches under the existing uniform
 * `gcc_2_8_1_g0` profile using `SDSecondaryState` and `SDSecondaryTransfer` from
 * `sound.h`. The shared `s32(u8 *, s16, s32)` contract is unchanged. No new
 * declaration view, compiler profile, inline assembly, or literal global address
 * is needed.
 *
 * The zero value is initialized in a single-iteration scope, then the existing
 * signed-halfword argument is captured into a word local before the first
 * `D_8009B458` load. This replaces both hard-register bindings. Flattening the
 * zero initialization loses one instruction; moving the argument capture before
 * that scope exchanges three prologue words; initializing the state pointer at
 * declaration moves its absolute load too early. The inherited eight-byte local
 * frame remains: retail adjusts `$sp` on entry and exit but never accesses it.
 *
 * The first state's consumed-byte count is cleared before the conditional
 * inactive-state rejection. The function then reloads the state pointer, clears
 * the transfer status, and populates the header pointer, header-derived byte
 * counts, two control bytes, and the final pointer-valued field in retail order.
 * The header remains a byte pointer because this match does not establish a new
 * shared VAB header type.
 *
 * All 38 target instructions agree with retail. The original canonical match
 * and six-entry inline-refinement series ending in deferral remain intact; a new
 * `post_terminal_resolution` row records the initialization-order discriminator.
 */
s32 SD_VabOpenHead(u8 *vab, s16 vab_id, s32 spu_addr)
{
    /* Retail reserves eight stack bytes without accessing them. */
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
    entry->field_0008 =
        (((SDVabHeader *)vab)->program_count << 9) + 0xA20;
    entry->field_0010 =
        ((SDVabHeader *)vab)->file_size - entry->field_0008;
    entry->field_0018 = ((SDVabHeader *)vab)->master_volume;
    entry->field_001B = ((SDVabHeader *)vab)->pan;
    entry->field_0014 = (u8 *)spu_addr;
    return 0;
}

s32 SD_VabTransBody(s32 value, s16 expected)
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
