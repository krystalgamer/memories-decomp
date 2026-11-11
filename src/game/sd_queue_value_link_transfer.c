#include "../types.h"
#include "sd_queue_value_link_transfer.h"
#include "sound.h"
#include "sound_driver.h"

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
    data_start_sector = (s32)(load_state->s_stGlobalHdrSE.s_wSD_NUM_SEGROUP << 3);
    data_start_sector = (data_start_sector + SD_VALUE_LINK_BLOCK_SIZE - 1) /
           SD_VALUE_LINK_BLOCK_SIZE;
    data_start_sector = data_start_sector + 1;
    off = (link_index & SD_VALUE_LINK_INDEX_MASK) * SD_VALUE_LINK_RECORD_SIZE;
    entry = (SDValueLink *)(off + (s32)load_state->s_pstSD_SEGROUP_TABLE);
    SD_EnqueueSpuRamTransfer(load_state->s_dwSD_SEGROUP_SPU_ADDR, 0x801E6800,
                  data_start_sector + entry->sector_offset, entry->spu_size,
                  SD_VALUE_LINK_BLOCK_SIZE, 0x10);
    registration_state = g_SDValue;
    registration_command.command = 0x51;
    registration_command.args.seq_block.spu_address = registration_state->s_dwSD_SEGROUP_SPU_ADDR;
    registration_command.args.seq_block.block_buffer = 0x801E6800;
    SD_EnqueueCommand(&registration_command);
    cursor_state = g_SDValue;
    off += (s32)cursor_state->s_pstSD_SEGROUP_TABLE;
    entry = (SDValueLink *)off;
    cursor_state->s_dwSD_SEGROUP_SPU_ADDR = cursor_state->s_dwSD_SEGROUP_SPU_ADDR + entry->spu_size;
}
