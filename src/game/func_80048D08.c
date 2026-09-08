#include "../types.h"
#include "sound.h"

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

extern void func_80048C70(u32 *dst, u32 *src);
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
