#ifndef MEMORIES_DECOMP_SAVE_DATA_H
#define MEMORIES_DECOMP_SAVE_DATA_H

#include "../types.h"
#include "card_constants.h"
#include "campaign_flags.h"

#define SAVE_DATA_HEADER_SIZE 0x200
#define SAVE_DATA_STATE_SIZE 0x680
#define SAVE_DATA_REPLICATED_STATE_SIZE (SAVE_DATA_STATE_SIZE * 2)
#define SAVE_DATA_DUPLICATE_STATE_OFFSET \
    (SAVE_DATA_HEADER_SIZE + SAVE_DATA_STATE_SIZE)
#define SAVE_DATA_CARD_QUANTITIES_OFFSET 0x50
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
#define SAVE_DATA_STARCHIPS_OFFSET 0x5E0
#define SAVE_DATA_STARCHIP_MAX 999999

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

/* The updater selects the outcome with a halfword cursor; the text producer
 * reads the same two counters with signed extension. */
typedef union {
    struct {
        u16 wins;
        u16 losses;
    } result;
    u16 counts[2];
} SaveDataDuelistRecord;

/* Observed prefix, not the full 0x680-byte persistent state allocation. */
typedef struct {
    u16 player_deck[DECK_SIZE];
    u8 card_quantities[CARD_COUNT];
    u8 pad_322[
        SAVE_DATA_DUELIST_CODE_OFFSET -
        (DECK_SIZE * sizeof(u16) + CARD_COUNT)
    ];
    s32 duelist_code;
    u8 pad_338[
        SAVE_DATA_SEQUENCE_OFFSET -
        (SAVE_DATA_DUELIST_CODE_OFFSET + sizeof(s32))
    ];
    u32 save_sequence;
    u32 vblank_counter;
    u8 player_name_sjis[SAVE_DATA_PLAYER_NAME_SIZE];
    u8 campaign_flags[
        (CAMPAIGN_FLAG_ID_MASK + 1) >> CAMPAIGN_FLAG_BYTE_SHIFT
    ];
    u8 pad_518[4];
    SaveDataDuelistRecord duelist_records[FREE_DUEL_GRID_ENTRY_COUNT];
    u8 pad_5BC[
        SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET -
        (0x51C + sizeof(SaveDataDuelistRecord) * FREE_DUEL_GRID_ENTRY_COUNT)
    ];
    u8 campaign_scene_index;
    u8 field_5DD;
    u8 output_type;
    u8 pad_5DF;
    u32 starchips;
} SaveDataState;

/* The live state starts at +0x200 in the reusable 0x801D0000 workspace.
 * This view describes only that prefix, not the cleared 0x3000-byte arena. */
typedef struct {
    u8 prefix[SAVE_DATA_HEADER_SIZE];
    SaveDataState state;
} SaveDataWorkspace;

typedef char SaveDataDuelistRecord_size_must_be_4[
    sizeof(SaveDataDuelistRecord) == FREE_DUEL_GRID_RECORD_SIZE ? 1 : -1
];
typedef char SaveDataDuelistRecord_losses_offset_must_be_2[
    (u32)&(((SaveDataDuelistRecord *)0)->result.losses) == sizeof(u16) ? 1 : -1
];
typedef char SaveDataState_card_quantities_offset_must_be_0x50[
    (u32)&(((SaveDataState *)0)->card_quantities) ==
        SAVE_DATA_CARD_QUANTITIES_OFFSET ? 1 : -1
];
typedef char SaveDataState_duelist_code_offset_must_be_0x334[
    (u32)&(((SaveDataState *)0)->duelist_code) ==
        SAVE_DATA_DUELIST_CODE_OFFSET ? 1 : -1
];
typedef char SaveDataState_campaign_flags_offset_must_be_0x418[
    (u32)&(((SaveDataState *)0)->campaign_flags) ==
        CAMPAIGN_FLAG_BANK_OFFSET - SAVE_DATA_HEADER_SIZE ? 1 : -1
];
typedef char SaveDataState_duelist_records_offset_must_be_0x51C[
    (u32)&(((SaveDataState *)0)->duelist_records) == 0x51C ? 1 : -1
];
typedef char SaveDataState_campaign_scene_index_offset_must_be_0x5DC[
    (u32)&(((SaveDataState *)0)->campaign_scene_index) ==
        SAVE_DATA_CAMPAIGN_SCENE_INDEX_OFFSET ? 1 : -1
];
typedef char SaveDataState_starchips_offset_must_be_0x5E0[
    (u32)&(((SaveDataState *)0)->starchips) ==
        SAVE_DATA_STARCHIPS_OFFSET ? 1 : -1
];
typedef char SaveDataState_size_must_be_0x5E4[
    sizeof(SaveDataState) == 0x5E4 ? 1 : -1
];
typedef char SaveDataWorkspace_state_offset_must_be_0x200[
    (u32)&(((SaveDataWorkspace *)0)->state) == SAVE_DATA_HEADER_SIZE ? 1 : -1
];

/* Keep the base and interior labels distinct: they are different relocation
 * targets. The save-prompt halfword arm retains its base register and writes
 * both campaign_scene_index and field_5DD, unlike the byte-sized restore. */
#ifdef SAVE_DATA_WORKSPACE_AS_HALFWORDS
extern s16 D_801D0000[];
#else
extern u8 D_801D0000[];
#endif
extern u8 gLibrary_abCardChest[];
extern u8 gSaveData_aPlayerNameSjis[];
extern SaveDataDuelistRecord gFreeDuel_aDuelistRecords[];

/* The head of the 0x680-byte persistent state block: SaveData_RequestWrite
 * copies SAVE_DATA_STATE_SIZE bytes starting here. Halfwords, as the name
 * says and as duel_deck_lookup.c, func_8002EE5C.c, func_8002BD0C.c
 * and free_duel/screen_runtime.c read it -- the files that walk it as bytes
 * are copying or scanning the block, not indexing the deck. */
extern u16 gDuel_awPlayerDeck[];
extern u32 gLibrary_dwStarchips;

extern u8 gSaveData_aTransferBuffer[];
extern u8 gSaveData_aHeaderTemplate[];

/* 0x801D1200. Five functions in four units take its address and nothing
 * else: func_800179F4 (src/candidates/func_800179F4.c, `pool =
 * D_801D1200;`, then `pool + 0x1000`), SaveData_UpdateLoadPair
 * (save_data_transfer_runtime.c as the first argument to MemCardDialog_Request
 * and through SaveDataState casts, the second at
 * `D_801D1200 + 0x1000`; that unit also names +0x1000 as D_801D2200),
 * SaveData_UpdateDuelLoad (save_data_transfer_runtime.c, the
 * second `+ TWO_PLAYER_SAVE_SLOT_STRIDE`), MainMenu_RefreshTradeInventory
 * (src/overlays/main_menu/trade_inventory.c:41, `+ slot * 0x1000`) and
 * MainMenu_UpdateTradeScreen (now a build-integrated candidate,
 * src/candidates/main_menu/func_801821DC.c, through its 16-byte Block16
 * view, cast at the use). The
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
 * D_8016D418. The two name-entry paths live in
 * src/overlays/password/name_entry_runtime.c and name_entry_dialog.c and call
 * the converter by address, as func_8003BC40. The one reader is
 * NameEntry_UpdateDialog's byte scan in name_entry_dialog.c, which runs to
 * TEXT_STRING_TERMINATOR. Every access
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
 * Its earlier C caller, func_8002EE94 (now src/candidates/func_8002EE94.c),
 * reached it with no declaration in scope at
 * all before this, so neither the argument list nor the void return was being
 * checked against the definition. Main_RunCredits uses this same declaration
 * when requesting the completion save. */
void SaveData_RequestWrite(void);

/* The load side of the same pair, both reached from the main-menu overlay's
 * boot path. SaveData_RequestLoad starts the read into
 * gSaveData_aTransferBuffer; SaveData_PollLoad reports how it finished, and
 * MainMenu_UpdateFrontendMenu (now a build-integrated candidate,
 * src/candidates/main_menu/func_80180390.c) branches on that result.
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
