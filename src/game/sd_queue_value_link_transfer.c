#define G_SDVALUE_IN_DATA
#include "../types.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output.h"

#define SD_VALUE_LINK_BLOCK_SIZE 0x800

void SD_QueueValueLinkTransfer(s32 index)
{
    SDCommand req;
    SDValue *a;
    SDValue *b;
    SDValue *c;
    s32 step;
    s32 off;
    SDValueLink *entry;

    a = g_SDValue;
    step = (s32)(a->value_link_count << 3);
    step = (step + SD_VALUE_LINK_BLOCK_SIZE - 1) /
           SD_VALUE_LINK_BLOCK_SIZE;
    step = step + 1;
    off = (index & SD_VALUE_LINK_INDEX_MASK) * SD_VALUE_LINK_RECORD_SIZE;
    entry = (SDValueLink *)(off + (s32)a->value_links);
    SD_EnqueueSpuRamTransfer(a->next_link_spu_address, 0x801E6800,
                  step + entry->sector_offset, entry->spu_size,
                  SD_VALUE_LINK_BLOCK_SIZE, 0x10);
    b = g_SDValue;
    req.command = 0x51;
    req.field_0004 = b->next_link_spu_address;
    req.field_000C = 0x801E6800;
    SD_EnqueueCommand(&req);
    c = g_SDValue;
    off += (s32)c->value_links;
    entry = (SDValueLink *)off;
    c->next_link_spu_address = c->next_link_spu_address + entry->spu_size;
}
