#ifndef MEMORIES_DECOMP_SOUND_EFFECT_H
#define MEMORIES_DECOMP_SOUND_EFFECT_H

#include "../types.h"
#include "sound.h"

typedef struct {
    s32 count;
    u8 pad0004[4];
    u16 keys[SD_PENDING_INPUT_ENTRY_CAPACITY];
    SDNote data[SD_PENDING_INPUT_ENTRY_CAPACITY];
} SDSeqBlock;

typedef char SDPendingInput_id_size_must_be_halfword[
    SD_PENDING_INPUT_ID_ENTRY_SIZE == sizeof(u16) ? 1 : -1
];
typedef char SDPendingInput_copy_groups_must_cover_block[
    SD_PENDING_INPUT_COPY_GROUP_COUNT * 8 * sizeof(u32) ==
        SD_PENDING_INPUT_BLOCK_SIZE ? 1 : -1
];
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

/* Local input snapshots, not a sound-driver memory record. */
typedef union {
    u64 all;
    struct { s32 original; s32 copy; } words;
} SDSEPlayIdPair;

void SD_ResetPendingEntries(void);
void SD_RegisterSeqBlockNotes(SDSeqBlock *input, u32 spu_addr);
void SD_QueueValueLinkTransfer(s32 link_index);

void SD_ApplyVoiceSlotVolume(s32 index);
void SD_UpdateVoiceSlots(void);
void SD_KeyOffDynamicVoices(void);
s32 SD_ReplaceValueLink(s32 value);
s32 SD_RequestValueLink(u16 value);
void SD_QueueValueLinkList(u16 *items, s32 count);
s32 SD_FilterCdFlaggedId(s32 value);
void SD_KeyOffVoice(s32 voice);
void SD_KeyOffVoiceById(s32 value);
void SD_SEStop(s32 id);
s32 SD_KeyOffVoiceGroup(u8 group);
s16 SD_CountActiveSoundEffectVoices(s32 id);
void SD_StartSoundEffectVoice(u16 id, u8 voice, s32 pitch_add, u8 volume,
                   s16 pan, u8 flags, u8 value);

/* Allocates one of the four sound-effect voice slots, whose SPU voice numbers
 * begin at SD_VOICE_SLOT_FIRST_VOICE. */
void SD_AllocateSoundEffectVoice(s32 id, s16 pitch, u8 volume, s16 pan, u32 mode, u8 value);

/* Updates active effect voices for a direct or bank-mapped sound ID.
 * Bit 0x8000 routes to the output-pan path instead. Pan arithmetic uses the
 * signed low halfword; the public arguments retain their full word ABI. */
void SD_SESetPan(s32 sound_id, s32 pan);

/* SD_SESetVolumePan takes three plain words at the call site. Its definition
 * is old style with u16/u8/u16 parameters, which default promotion keeps
 * compatible with this prototype: the narrowing happens in the callee, where
 * GCC keeps each incoming register and its narrowed copy apart exactly as
 * retail does. */
void SD_SESetVolumePan(s32 id, s32 volume, s32 pan);
void SD_FadeOutSoundEffectVoices(u16 id, u8 step);

/* SD_LoadSequenceBankPair's second argument is a word pointer, not a word. */
void SD_CopySeqBlock(u32 *dst, u32 *src);
void SD_LoadSequenceBankPair(s32 side, u32 *src);

#endif
