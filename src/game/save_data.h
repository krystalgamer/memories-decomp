#ifndef MEMORIES_DECOMP_SAVE_DATA_H
#define MEMORIES_DECOMP_SAVE_DATA_H

#include "../types.h"

#define SAVE_DATA_HEADER_SIZE 0x200
#define SAVE_DATA_STATE_SIZE 0x680
#define SAVE_DATA_REPLICATED_STATE_SIZE (SAVE_DATA_STATE_SIZE * 2)
#define SAVE_DATA_DUPLICATE_STATE_OFFSET \
    (SAVE_DATA_HEADER_SIZE + SAVE_DATA_STATE_SIZE)
#define SAVE_DATA_DUELIST_CODE_OFFSET 0x334
#define SAVE_DATA_SEQUENCE_OFFSET 0x404
#define SAVE_DATA_VBLANK_COUNTER_OFFSET 0x408
#define SAVE_DATA_PLAYER_NAME_OFFSET 0x40C
#define SAVE_DATA_PLAYER_NAME_CHAR_COUNT 6
#define SAVE_DATA_PLAYER_NAME_SIZE \
    (SAVE_DATA_PLAYER_NAME_CHAR_COUNT * sizeof(u16))
#define TWO_PLAYER_SAVE_SLOT_STRIDE 0x1000
#define TWO_PLAYER_SAVE_TRANSFER_SIZE 0x400
#define SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET 0x5DC
#define SAVE_DATA_OUTPUT_TYPE_OFFSET 0x5DE

#define SAVE_DATA_CRC16_BITS 16
#define SAVE_DATA_CRC16_MASK ((1 << SAVE_DATA_CRC16_BITS) - 1)
#define SAVE_DATA_CRC16_HIGH_BIT (1 << (SAVE_DATA_CRC16_BITS - 1))
#define SAVE_DATA_CRC16_POLYNOMIAL 0x1021

#define SAVE_DATA_PRIMARY_LENGTH 0x340
#define SAVE_DATA_PRIMARY_MASK_LAST_OFFSET 0x378
#define SAVE_DATA_PRIMARY_MASK_WORD_COUNT 15
#define SAVE_DATA_PRIMARY_CHECKSUM_OFFSET \
    (SAVE_DATA_PRIMARY_MASK_LAST_OFFSET + sizeof(s32))

#define SAVE_DATA_SECONDARY_OFFSET 0x380
#define SAVE_DATA_SECONDARY_LENGTH 0x6C
#define SAVE_DATA_SECONDARY_MASK_LAST_OFFSET 0x3F8
#define SAVE_DATA_SECONDARY_MASK_WORD_COUNT 4
#define SAVE_DATA_SECONDARY_CHECKSUM_OFFSET \
    (SAVE_DATA_SECONDARY_MASK_LAST_OFFSET + sizeof(s32))

#define SAVE_DATA_TERTIARY_OFFSET 0x400
#define SAVE_DATA_TERTIARY_LENGTH 0x204
#define SAVE_DATA_TERTIARY_CHECKSUM_OFFSET \
    (SAVE_DATA_TERTIARY_OFFSET + SAVE_DATA_TERTIARY_LENGTH)
#define SAVE_DATA_TERTIARY_MASK_LAST_OFFSET 0x624
#define SAVE_DATA_TERTIARY_MASK_WORD_COUNT 8
#define SAVE_DATA_RESERVED_TAIL_OFFSET \
    (SAVE_DATA_TERTIARY_MASK_LAST_OFFSET + sizeof(s32))
#define SAVE_DATA_RESERVED_TAIL_SIZE \
    (SAVE_DATA_STATE_SIZE - SAVE_DATA_RESERVED_TAIL_OFFSET)
#define SAVE_DATA_RESERVED_TAIL_PAYLOAD_OFFSET \
    (SAVE_DATA_HEADER_SIZE + SAVE_DATA_RESERVED_TAIL_OFFSET)

/* Shared prefix of the persistent state block. Validation owns the duelist
 * code, while runtime restore owns the sequence, frame counter, player name,
 * campaign scene and sound-output fields. */
typedef struct {
    u8 pad_000[SAVE_DATA_DUELIST_CODE_OFFSET];
    s32 duelist_code;
    u8 pad_338[
        SAVE_DATA_SEQUENCE_OFFSET -
        (SAVE_DATA_DUELIST_CODE_OFFSET + sizeof(s32))
    ];
    u32 save_sequence;
    u32 vblank_counter;
    u8 player_name_sjis[SAVE_DATA_PLAYER_NAME_SIZE];
    u8 pad_418[
        SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET -
        (SAVE_DATA_PLAYER_NAME_OFFSET + SAVE_DATA_PLAYER_NAME_SIZE)
    ];
    u8 campaign_scene_index;
    u8 field_5DD;
    u8 output_type;
} SaveDataState;

/* The head of the 0x680-byte persistent state block: SaveData_RequestWrite
 * copies SAVE_DATA_STATE_SIZE bytes starting here. Halfwords, as the name
 * says and as duel_deck_lookup.c, func_8002EE94.c, library_runtime.c
 * and free_duel/screen_runtime.c read it -- the files that walk it as bytes
 * are copying or scanning the block, not indexing the deck. */
extern u16 gDuel_awPlayerDeck[];
extern s32 gSaveData_dwDuelistCode;

extern u8 gSaveData_aTransferBuffer[];
extern u8 gSaveData_aHeaderTemplate[];

/* 0x801D1200. Five functions in four units take its address and nothing
 * else: func_800179F4 (func_800179F4.c:166, `pool = D_801D1200;`, then
 * `pool + 0x1000` at :168), func_8003F8D4 (func_8003F8D4.c:77 as the first
 * argument to MemCardDialog_Request, and :93-94 through SaveDataState casts,
 * the second at `D_801D1200 + 0x1000`; that unit also names +0x1000 as
 * D_801D2200, :13 and :102), func_8003FD14 (two_player_save_setup.c:28-29,
 * the second `+ TWO_PLAYER_SAVE_SLOT_STRIDE`), MainMenu_RefreshTradeInventory
 * (src/overlays/main_menu/trade_update.c:608, `+ slot * 0x1000`) and
 * MainMenu_UpdateTradeScreen (src/overlays/main_menu/trade_update.c:156 and
 * :197, through that unit's 16-byte Block16 view, cast at the use). The
 * resident listings form the address with lui/addiu (func_800179F4.s:226-227,
 * func_8003FD14.s:22-23, func_8003F8D4.s:115-116 and :136-137) and say
 * nothing about the object's width. c_symbols.ld names D_801D160C (+0x40C)
 * and D_801D1880 (+0x680) inside it, and the Block16 indexes above reach
 * +0x400, +0x680, +0x6D0, +0xA80, +0x1000, +0x1680 and +0x16D0; the interior
 * names keep their own declarations. Of the five units that used to declare
 * it privately, four said `u8 []` and trade_update.c `Block16 []`. */
extern u8 D_801D1200[];

/* 0x801B125A. SaveData_ApplyRuntimeState passes it as the destination of
 * Text_SjisToGlyphCodes, from state->player_name_sjis
 * (save_data_payload.c:191); NameEntry_Init does the same from
 * gSaveData_aPlayerNameSjis and NameEntry_UpdateDialog does the same from
 * D_8016D418. Both name-entry paths now live in
 * src/overlays/password/name_entry_runtime.c and call the converter by
 * address, as func_8003BC40. The one reader is NameEntry_UpdateDialog's byte
 * scan in that source, which runs to TEXT_STRING_TERMINATOR. Every access
 * takes the address (the resident listing func_8003D0F4.s:7-8 forms it with
 * lui/addiu), so the listings do not say how wide the object is. The resident
 * unit used to declare it `u8 [16]` and the overlay paths `u8 []`. */
extern u8 D_801B125A[];
extern s32 gSaveDataSequence;
#ifndef SAVE_DATA_DECLARE_MASK_STATE_LOCALLY
extern u32 gSaveData_dwMaskStateLow;
extern u32 gSaveData_dwMaskStateHigh;
#endif

u32 SaveData_NextMaskWord(void);
void SaveData_SetMaskSeed(u32);
u32 SaveData_CalcCrc16(u8 *, s32);
void SaveData_WritePrimarySecondaryIntegrity(u8 *);
void SaveData_WriteTertiaryIntegrity(u8 *);
void SaveData_BuildPayload(u8 *);
s32 SaveData_ValidateIntegrity(u8 *);
void SaveData_ApplyRuntimeState(SaveDataState *state);

/* Stages the runtime state into gSaveData_aTransferBuffer, builds the payload
 * ahead of it through SaveData_BuildPayload, and asks the memory card layer to
 * write it under gMemCard_szSaveFileName.
 *
 * Its one caller, func_8002EE94.c, reached it with no declaration in scope at
 * all before this, so neither the argument list nor the void return was being
 * checked against the definition. */
void SaveData_RequestWrite(void);

/* The load side of the same pair, both reached from the main-menu overlay's
 * boot path. SaveData_RequestLoad starts the read into
 * gSaveData_aTransferBuffer; SaveData_PollLoad reports how it finished, and
 * frontend.c branches on that result.
 *
 * Both were declared in that overlay and nowhere else. */
void SaveData_RequestLoad(void);
s32 SaveData_PollLoad(void);
s32 SaveData_HasSameDuelistCode(
    SaveDataState *left,
    SaveDataState *right
);
s32 SaveData_MatchesDuelistAndCurrentSequence(
    SaveDataState *left,
    SaveDataState *right
);

#endif
