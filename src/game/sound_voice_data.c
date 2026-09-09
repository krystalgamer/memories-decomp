#include "../types.h"
#include "../psyq/libspu.h"
#include "sound.h"

extern void func_80044E90(s32);
extern void func_80047864(s32);

void func_80048A28(s32 arg0, s32 arg1, s32 arg2)
{
    register s32 i __asm__("$16");
    s32 pan;
    register s32 id __asm__("$18") = arg0;
    s32 a1v = arg1;
    s32 right;
    s32 left;
    s32 leftOk;
    register s32 a2v __asm__("$23") = arg2;
    s32 lo;
    s32 hi;
    register s32 ff __asm__("$4");

    if (arg0 & 0x8000) {
        g_SDValue->cd_volume = a1v & 0xFF;
        func_80044E90((s16)arg2);
        return;
    }
    if ((arg0 & SD_VOICE_LOOKUP_CODE_MASK) == SD_VOICE_LOOKUP_CODE_TAG) {
        SDValue *a = g_SDValue;
        u16 v;

        lo = (arg0 & SD_VOICE_LOOKUP_INDEX_MASK) << 1;
        hi = arg0 & SD_VOICE_LOOKUP_BANK_FLAG;
        hi = (hi != 0) << SD_VOICE_LOOKUP_BANK_BYTE_SHIFT;
        v = *(u16 *)((u8 *)a + (lo + hi) + SD_VOICE_LOOKUP_BYTE_OFFSET);
        ff = SD_PENDING_ENTRY_NONE;
        if (v == ff) {
            return;
        }
        id = a->field_043C[v];
        if (id == ff) {
            return;
        }
    }
    i = 0;
    leftOk = (u16)(arg2 - 1) < 0x80;
    pan = (s16)arg2;
    left = 0x80 - pan;
    right = pan + 0x80;
    do {
        s16 local;
        SDValue *b;
        u16 vid;

        SpuGetVoiceEnvelope(i + SD_VOICE_SLOT_FIRST_VOICE, &local);
        b = g_SDValue;
        vid = b->voice_ids[i];
        if (vid == (id & 0xFFFF) && local != 0) {
            b->voice_value[i] = a1v;
            if (pan != 0) {
                if (leftOk) {
                    SDValue *c = g_SDValue;
                    register s32 prod __asm__("$2");

                    prod = c->field_0444[vid].volume * left;
                    c->voice_volume_left[i] = prod;
                }
                if ((u16)(a2v + 0x80) < 0x80) {
                    SDValue *d = g_SDValue;
                    register s32 prod __asm__("$2");

                    prod = d->field_0444[vid].volume * right;
                    d->voice_volume_right[i] = prod;
                }
            }
            func_80047864(i);
        }
    } while (++i < SD_VOICE_SLOT_COUNT);
}

typedef char SDPendingInput_copy_groups_must_cover_block[
    SD_PENDING_INPUT_COPY_GROUP_COUNT * 8 * sizeof(u32) ==
        SD_PENDING_INPUT_BLOCK_SIZE ? 1 : -1
];

void func_80048C0C(u16 value, u8 enabled)
{
    s32 i;
    s32 mask;

    if (enabled == 0)
        return;
    mask = 1;
    for (i = 0; i < SD_VOICE_SLOT_COUNT; i++, mask <<= 1) {
        SDValue *state = g_SDValue;
        if (state->voice_ids[i] == value &&
            (state->voice_active_mask & mask)) {
            state->voice_step[i] = enabled;
        }
    }
}

void func_80048C70(u32 *dst, u32 *src)
{
    u32 i;

    for (i = 0; i < SD_PENDING_INPUT_COPY_GROUP_COUNT; i++) {
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
        *dst++ = *src++;
    }
}

typedef struct {
    s32 count;
    u8 pad0004[4];
    u16 keys[SD_PENDING_INPUT_ENTRY_CAPACITY];
    SDNote data[SD_PENDING_INPUT_ENTRY_CAPACITY];
} SDSeqBlock;

typedef char SDSeqBlock_size_must_match_input_block[
    sizeof(SDSeqBlock) == SD_PENDING_INPUT_BLOCK_SIZE ? 1 : -1
];
typedef char SDSeqBlock_id_region_must_have_whole_entries[
    (SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET - SD_PENDING_INPUT_IDS_BYTE_OFFSET) %
        SD_PENDING_INPUT_ID_ENTRY_SIZE == 0 ? 1 : -1
];
typedef char SDSeqBlock_keys_must_match_input_offset[
    (u32)&(((SDSeqBlock *)0)->keys) == SD_PENDING_INPUT_IDS_BYTE_OFFSET ? 1 : -1
];
typedef char SDSeqBlock_data_must_match_input_offset[
    (u32)&(((SDSeqBlock *)0)->data) ==
        SD_PENDING_INPUT_PAYLOAD_BYTE_OFFSET ? 1 : -1
];

extern void func_8004763C(void);

void func_80048D08(s32 side, u32 *src)
{
    SDSeqBlock *blk;
    SDSeqBlock *other;
    SDSeqBlock *cur;
    SDNote *e;
    u32 addr_side;
    u32 addr_other;
    s32 rev;
    s32 i;
    s32 j;

    if ((u32)side >= 2) {
        return;
    }

    rev = 1 - side;
    addr_side = 0xD810 + (side % 2) * 0x19000;
    addr_other = 0xD810 + (rev % 2) * 0x19000;
    blk = (SDSeqBlock *)(0x801E7800 + (side << SD_PENDING_INPUT_BLOCK_SHIFT));
    other = (SDSeqBlock *)(0x801E7800 + (rev << SD_PENDING_INPUT_BLOCK_SHIFT));
    func_80048C70((u32 *)blk, src);
    func_8004763C();

    for (i = 0; i < 2; i++) {
        cur = other;
        if (i != 0) {
            cur = blk;
        }
        for (j = 0; j < cur->count; j++) {
            u16 key = cur->keys[j];
            if (key != SD_PENDING_ENTRY_NONE) {
                u16 n = g_SDValue->field_0440;
                u16 v;

                g_SDValue->field_043C[key] = n;
                g_SDValue->field_0444[n] = cur->data[j];
                e = (SDNote *)(n * 8 + (u32)g_SDValue->field_0444);
                v = *(u16 *)((u8 *)e + 6);
                *(u16 *)((u8 *)e + 6) =
                    (i != 0) ? (u16)(v + (addr_side >> 4))
                             : (u16)(v + (addr_other >> 4));
                g_SDValue->field_0440 = g_SDValue->field_0440 + 1;
            }
            if (i != 0) {
                g_SDValue->field_044C[side][j] = key;
            }
        }
    }
}

#include "sound_voice_data.h"

extern void func_80049594(s32);
extern void func_80049600(s32);
extern void func_80049544(void);

void func_80048F14(void)
{
    SpuReverbAttr packet;
    /* Pinned: the first g_SDValue load's delay slot is what decides this
       function. Unpinned, GCC fills it with the 0xFF constant, which extends
       that value's live range across the three stores and pushes this pointer
       to $a2, shifting $v0/$a2/$a1 throughout. Pinning the pointer restores
       retail's fill, `lui $a0, 0x801E`. */
    register SDValue *a __asm__("$2");
    SDValue *c;
    u16 *base;
    u16 *r1;
    u16 *r2;
    u16 *r3;
    u16 *r4;
    u16 *r5;

    SpuReserveReverbWorkArea(SPU_ON);
    SpuSetReverb(SPU_ON);
    packet.mask = SPU_REV_MODE | SPU_REV_DEPTHL | SPU_REV_DEPTHR;
    packet.mode = SPU_REV_MODE_STUDIO_A;
    packet.depth.left = 0x7FFF;
    packet.depth.right = 0x7FFF;
    SpuSetReverbModeParam(&packet);
    a = g_SDValue;
    a->field_1586 = 0;
    a->field_1588 = 0;
    ((u8 *)a)[0x158A] = 0;
    a = g_SDValue;
    *(s16 *)((u8 *)a + 0x1580) = 0xFF;
    a->field_1584 = 0xFF;
    c = g_SDValue;
    a->field_1582 = 0;
    base = (u16 *)0x801EA800;
    c->music_track = base;
    c->field_1560 = (u8 *)0x801E2000;
    c->field_1578 = -1;
    c->field_157A = -1;
    *(s16 *)((u8 *)c + 0x157C) = -1;
    c->field_157E = -1;
    r1 = c->music_track;
    *r1 = 0xFFFF;
    r2 = c->music_track;
    *(s16 *)((u8 *)r2 + 2) = 0;
    r3 = c->music_track;
    *(s32 *)((u8 *)r3 + 4) = 0;
    r4 = c->music_track;
    *(s32 *)((u8 *)r4 + 8) = 0;
    r5 = c->music_track;
    *(s32 *)((u8 *)r5 + 0xC) = 0x40000;
    func_80049594(2);
    func_80049600(0x14);
    func_80049544();
}
