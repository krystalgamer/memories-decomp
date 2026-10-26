#define G_SDVALUE_IN_DATA
#include "../types.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_output.h"

#define SD_VALUE_LINK_BLOCK_SIZE 0x800

void SD_QueueValueLinkTransfer(s32 link_index)
{
    SDCommand registration_command;
    SDValue *load_state;
    SDValue *registration_state;
    SDValue *cursor_state;
    s32 data_start_sector;
    s32 off;
    SDValueLink *entry;

    load_state = g_SDValue;
    data_start_sector = (s32)(load_state->value_link_count << 3);
    data_start_sector = (data_start_sector + SD_VALUE_LINK_BLOCK_SIZE - 1) /
           SD_VALUE_LINK_BLOCK_SIZE;
    data_start_sector = data_start_sector + 1;
    off = (link_index & SD_VALUE_LINK_INDEX_MASK) * SD_VALUE_LINK_RECORD_SIZE;
    entry = (SDValueLink *)(off + (s32)load_state->value_links);
    SD_EnqueueSpuRamTransfer(load_state->next_link_spu_address, 0x801E6800,
                  data_start_sector + entry->sector_offset, entry->spu_size,
                  SD_VALUE_LINK_BLOCK_SIZE, 0x10);
    registration_state = g_SDValue;
    registration_command.command = 0x51;
    registration_command.arg0 = registration_state->next_link_spu_address;
    registration_command.arg2 = 0x801E6800;
    SD_EnqueueCommand(&registration_command);
    cursor_state = g_SDValue;
    off += (s32)cursor_state->value_links;
    entry = (SDValueLink *)off;
    cursor_state->next_link_spu_address = cursor_state->next_link_spu_address + entry->spu_size;
}
