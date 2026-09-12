#ifndef MEMORIES_DECOMP_YGO_TYPES_H
#define MEMORIES_DECOMP_YGO_TYPES_H

#include "types.h"
#include "game/card_constants.h"

/* Only the first five words at 0x80010000: three reused MODEL payload
 * bases, followed by the two primary modules entered at +4. This is an
 * address-block prefix, not a homogeneous arena table or a payload layout.
 * The remaining module, data-argument and SU words keep separate labels. */
typedef struct {
    u8 *payload_bases[3];
    u8 *primary_modules[2];
} HighMemoryModelAddressPrefix;

typedef char HighMemoryModelAddressPrefix_size_must_be_0x14[
    sizeof(HighMemoryModelAddressPrefix) == 0x14 ? 1 : -1
];
typedef char HighMemoryModelAddressPrefix_modules_offset_must_be_0xC[
    (u32)&(((HighMemoryModelAddressPrefix *)0)->primary_modules) == 0xC ? 1 : -1
];

typedef struct {
    s16 id;
    u16 count;
} CardCountEntry;

typedef char CardCountEntry_size_must_be_4[
    sizeof(CardCountEntry) == 4 ? 1 : -1
];

typedef u8 *(*ModelHandler)(u8 **);
typedef void (*ScriptCommandHandler)(void);

typedef struct {
    u8 b[11];
} SDInitBlk11;

typedef struct {
    u8 b[10];
} SDInitBlk10;

/* Eight bytes copied as one unit. Three build-integrated candidates each
   defined this shape locally and used it only as the source and destination
   of a whole-struct assignment: func_80015EF4 copies four rotation corners,
   func_80029934 one parameter block out of D_80181000, and func_80030294 one
   mask block out of D_8009AF4C.

   The element type is what this type is for, and it is load-bearing for the
   reason model.h:113-124 gives about ModelSlotCF8BlockWords -- the element
   type sets the alignment and the alignment sets the move width. Here `u8`
   gives alignment 1, so the assignment lowers to the unaligned move pair
   rather than to word loads. The three targets say so directly -- an eight-byte
   alignment-1 copy is two lwl/lwr and two swl/swr, and the counts in
   src/candidates_target/ are exactly two pairs per source-level assignment:
   func_80015EF4.S has 8 of each for its four copies, func_80029934.S and
   func_80030294.S 2 of each for their one. A word-element spelling would not
   reproduce them.

   model.h's ModelBytes8 is the same shape and is deliberately left where it
   is; it is also the declared type of two defined objects, which is a claim
   about those objects rather than about a copy idiom. */
typedef struct {
    u8 b[8];
} Bytes8;

typedef char Bytes8_size_must_be_8[
    sizeof(Bytes8) == 8 ? 1 : -1
];

typedef struct {
    u8 bytes[20];
} TextDecimalDigitKeyBlock;

typedef char TextDecimalDigitKeyBlock_size_must_be_20[
    sizeof(TextDecimalDigitKeyBlock) == 20 ? 1 : -1
];

typedef struct {
    u16 positions[3];
} OptionsLayoutPositionBlock;

typedef char OptionsLayoutPositionBlock_size_must_be_6[
    sizeof(OptionsLayoutPositionBlock) == 6 ? 1 : -1
];

typedef struct {
    OptionsLayoutPositionBlock positions;
    u16 pad_06;
} OptionsLayoutPositionData;

typedef char OptionsLayoutPositionData_size_must_be_8[
    sizeof(OptionsLayoutPositionData) == 8 ? 1 : -1
];

typedef struct {
    s32 key;
    void **handler;
} ModelHandlerObject;

#define YGO_TYPE_OFFSET(type, member) ((u32)&(((type *)0)->member))

/* Shared text/effect node. The word members preserve the alignment required
   by the resident compactor's whole-entry lw/sw copies. */
typedef struct {
    u16 code_00;
    u16 pad_02;
    s32 field_04;
    s32 field_08;
    s16 x_0C;
    s16 y_0E;
    u8 field_10;
    u8 flags_11;
    u8 field_12;
    u8 field_13;
    u8 pad_14;
    u8 field_15;
    /* Written on distinct text-effect paths; their meaning is unresolved. */
    u8 field_16;
    u8 field_17;
    u8 field_18;
    u8 pad_19[3];
} DuelEffectEntry;

typedef char DuelEffectEntry_size_must_be_0x1C[
    sizeof(DuelEffectEntry) == 0x1C ? 1 : -1
];
typedef char DuelEffectEntry_must_be_four_byte_aligned[
    sizeof(struct { u8 lead; DuelEffectEntry entry; }) == 0x20 ? 1 : -1
];
typedef char DuelEffectEntry_code_00_offset_must_be_0[
    YGO_TYPE_OFFSET(DuelEffectEntry, code_00) == 0 ? 1 : -1
];
typedef char DuelEffectEntry_x_0C_offset_must_be_0x0C[
    YGO_TYPE_OFFSET(DuelEffectEntry, x_0C) == 0x0C ? 1 : -1
];
typedef char DuelEffectEntry_field_10_offset_must_be_0x10[
    YGO_TYPE_OFFSET(DuelEffectEntry, field_10) == 0x10 ? 1 : -1
];
typedef char DuelEffectEntry_flags_11_offset_must_be_0x11[
    YGO_TYPE_OFFSET(DuelEffectEntry, flags_11) == 0x11 ? 1 : -1
];
typedef char DuelEffectEntry_field_12_offset_must_be_0x12[
    YGO_TYPE_OFFSET(DuelEffectEntry, field_12) == 0x12 ? 1 : -1
];
typedef char DuelEffectEntry_field_13_offset_must_be_0x13[
    YGO_TYPE_OFFSET(DuelEffectEntry, field_13) == 0x13 ? 1 : -1
];
typedef char DuelEffectEntry_field_15_offset_must_be_0x15[
    YGO_TYPE_OFFSET(DuelEffectEntry, field_15) == 0x15 ? 1 : -1
];
typedef char DuelEffectEntry_field_18_offset_must_be_0x18[
    YGO_TYPE_OFFSET(DuelEffectEntry, field_18) == 0x18 ? 1 : -1
];

typedef void (*NameEntryGlyphUpdate)(u8 *sprite);

/* The installed callback selects scale versus destination XY at 0x44/0x46.
   This is a name-entry view, not a universal display-object interpretation. */
typedef struct {
    u8 pad_00[0x4];
    u32 flags;
    u8 pad_08[0x3C];
    s16 scale_x;
    s16 scale_y;
    u32 field_48;
    DuelEffectEntry *sourceGlyph;
    u8 pad_50[0xA];
    s16 savedSourceX;
    u8 pad_5C[0x4];
    s16 frame;
    u8 pad_62[0x5];
    u8 textBoxSlot;
    u8 pad_68[0x2];
    u8 sequence;
} GlyphSprite;

typedef char GlyphSprite_size_must_be_0x6C[
    sizeof(GlyphSprite) == 0x6C ? 1 : -1
];
typedef char GlyphSprite_scale_offset_must_be_0x44[
    YGO_TYPE_OFFSET(GlyphSprite, scale_x) == 0x44 ? 1 : -1
];
typedef char GlyphSprite_source_offset_must_be_0x4C[
    YGO_TYPE_OFFSET(GlyphSprite, sourceGlyph) == 0x4C ? 1 : -1
];
typedef char GlyphSprite_saved_x_offset_must_be_0x5A[
    YGO_TYPE_OFFSET(GlyphSprite, savedSourceX) == 0x5A ? 1 : -1
];
typedef char GlyphSprite_frame_offset_must_be_0x60[
    YGO_TYPE_OFFSET(GlyphSprite, frame) == 0x60 ? 1 : -1
];
typedef char GlyphSprite_sequence_offset_must_be_0x6A[
    YGO_TYPE_OFFSET(GlyphSprite, sequence) == 0x6A ? 1 : -1
];

/* Dialog panel reached through the text box, not the keyboard selection
   frame. slide is signed distance remaining on the panel's own transition. */
typedef struct {
    u8 pad_00[0x8];
    u16 flags;
    u8 pad_0A[0x26];
    s16 x;
    s16 y;
    u8 pad_34[0x2C];
    s16 slide;
    u8 pad_62[0xA];
    u8 status;
} DialogCaret;

typedef char DialogCaret_size_must_be_0x6E[
    sizeof(DialogCaret) == 0x6E ? 1 : -1
];
typedef char DialogCaret_position_offset_must_be_0x30[
    YGO_TYPE_OFFSET(DialogCaret, x) == 0x30 ? 1 : -1
];
typedef char DialogCaret_slide_offset_must_be_0x60[
    YGO_TYPE_OFFSET(DialogCaret, slide) == 0x60 ? 1 : -1
];
typedef char DialogCaret_status_offset_must_be_0x6C[
    YGO_TYPE_OFFSET(DialogCaret, status) == 0x6C ? 1 : -1
];

/* The drawing callback and keyboard tween operate on the same allocation.
   Their old prefixes agreed at every common offset; this joins their fields. */
typedef struct {
    u8 pad_00[0x14];
    s16 priority;
    u8 pad_16[0x1A];
    s16 x;
    s16 y;
    u8 pad_34[0x2];
    s16 stepX;
    s16 stepY;
    u8 pad_3A[0x2];
    u16 width;
    u16 height;
    u8 pad_40[0x1E];
    u8 widthBonus;
    u8 pad_5F;
    s16 timer;
} SelectionFrame;

typedef SelectionFrame NameEntrySelectionFrameView;

typedef char SelectionFrame_size_must_be_0x62[
    sizeof(SelectionFrame) == 0x62 ? 1 : -1
];
typedef char SelectionFrame_priority_offset_must_be_0x14[
    YGO_TYPE_OFFSET(SelectionFrame, priority) == 0x14 ? 1 : -1
];
typedef char SelectionFrame_position_offset_must_be_0x30[
    YGO_TYPE_OFFSET(SelectionFrame, x) == 0x30 ? 1 : -1
];
typedef char SelectionFrame_velocity_offset_must_be_0x36[
    YGO_TYPE_OFFSET(SelectionFrame, stepX) == 0x36 ? 1 : -1
];
typedef char SelectionFrame_extent_offset_must_be_0x3C[
    YGO_TYPE_OFFSET(SelectionFrame, width) == 0x3C ? 1 : -1
];
typedef char SelectionFrame_bonus_offset_must_be_0x5E[
    YGO_TYPE_OFFSET(SelectionFrame, widthBonus) == 0x5E ? 1 : -1
];
typedef char SelectionFrame_timer_offset_must_be_0x60[
    YGO_TYPE_OFFSET(SelectionFrame, timer) == 0x60 ? 1 : -1
];

#define NAME_ENTRY_STARTER_DECK_POOL_PADDING_SIZE 18

typedef struct {
    u16 draw_count;
    u16 weights[CARD_COUNT];
    u8 padding[NAME_ENTRY_STARTER_DECK_POOL_PADDING_SIZE];
} NameEntryStarterDeckPool;

typedef char NameEntryStarterDeckPool_size_must_be_0x5B8[
    sizeof(NameEntryStarterDeckPool) == 0x5B8 ? 1 : -1
];
typedef char NameEntryStarterDeckPool_weights_offset_must_be_2[
    YGO_TYPE_OFFSET(NameEntryStarterDeckPool, weights) == 2 ? 1 : -1
];

/* Live overworld records: camera setup/tween, marker placement and exit
   selection all read the same 66-byte stride. The alternate map is separate. */
typedef struct {
    u16 story_flag;
    s16 x;
    s16 y;
    u16 input_mask;
    u8 field_08;
    u8 destination;
    u8 move_steps;
    u8 pad_0B;
} CampaignMapExit;

typedef struct {
    u16 confirm_gate;
    s16 camera_field_04;
    s16 camera_angle;
    s16 camera_field_00;
    s16 view_x;
    s16 view_z;
    s16 f12;
    s16 f14;
    u8 confirm_destination;
    u8 pad_11;
    CampaignMapExit exits[4];
} MapLocation;

typedef char CampaignMapExit_size_must_be_12[
    sizeof(CampaignMapExit) == 12 ? 1 : -1
];
typedef char CampaignMapExit_input_mask_offset_must_be_6[
    YGO_TYPE_OFFSET(CampaignMapExit, input_mask) == 6 ? 1 : -1
];
typedef char CampaignMapExit_destination_offset_must_be_9[
    YGO_TYPE_OFFSET(CampaignMapExit, destination) == 9 ? 1 : -1
];
typedef char MapLocation_size_must_be_66[
    sizeof(MapLocation) == 66 ? 1 : -1
];
typedef char MapLocation_marker_offset_must_be_12[
    YGO_TYPE_OFFSET(MapLocation, f12) == 12 ? 1 : -1
];
typedef char MapLocation_exits_offset_must_be_18[
    YGO_TYPE_OFFSET(MapLocation, exits) == 18 ? 1 : -1
];

typedef struct {
    u8 pad0[8];
    u16 f8;
    u8 pad10[38];
    s16 f48;
    s16 f50;
    u8 pad52[20];
    u16 f72;
    u16 f74;
    u8 pad76[20];
    s16 f96;
} MapObject;

typedef char MapObject_position_offset_must_be_48[
    YGO_TYPE_OFFSET(MapObject, f48) == 48 ? 1 : -1
];
typedef char MapObject_transition_offset_must_be_96[
    YGO_TYPE_OFFSET(MapObject, f96) == 96 ? 1 : -1
];

/* The two endpoint blocks in each model-effect request. `packed` preserves
   func_8005F91C's unaligned two-word copies from generic byte pointers. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 kind;
} __attribute__((packed)) ModelEffectEndpoint;

typedef char ModelEffectEndpoint_size_must_be_0x8[
    sizeof(ModelEffectEndpoint) == 0x8 ? 1 : -1
];
typedef char ModelEffectEndpoint_kind_offset_must_be_0x6[
    YGO_TYPE_OFFSET(ModelEffectEndpoint, kind) == 0x6 ? 1 : -1
];

/* Four halfwords produced by func_80059000. The fourth is the maximum of the
   first three after the slot-specific overrides are applied. */
typedef struct {
    s16 x;
    s16 y;
    s16 z;
    s16 max;
} ModelEffectAdjustment;

typedef char ModelEffectAdjustment_size_must_be_0x8[
    sizeof(ModelEffectAdjustment) == 0x8 ? 1 : -1
];
typedef char ModelEffectAdjustment_max_offset_must_be_0x6[
    YGO_TYPE_OFFSET(ModelEffectAdjustment, max) == 0x6 ? 1 : -1
];

typedef struct FileTransferDescriptor FileTransferDescriptor;
typedef void (*FileTransferCallback)();

struct FileTransferDescriptor {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
    u32 value_08;
    u32 value_0C;
    volatile s32 total_bytes;
    s32 file_bytes;
    u8 *loader_argument;
    u32 mode;
    FileTransferCallback phase_callback;
    s32 absolute_lba;
    s32 phase_remaining;
    u32 status_flags;
    /* One word at 0x30 that the loader's phase callbacks write both ways,
       so the record carries both readings rather than picking one.

       As two halfwords it is a pair: duel_card_data_transfer.c counts
       sectors in `counter` alone, the phase-2 paths write a VRAM position
       pair, and file_stream.c splits a byte count across the two halves.
       As one word it is a single packed constant, which is what the phase-3
       paths of func_8002BD0C and func_800577B0 store in one instruction.

       Neither view is a superset of the other and retail emits both, so
       collapsing them to either alone changes codegen: spelling the phase-3
       word as two halfword stores costs an instruction at each site and
       overflows .text. */
    union {
        struct {
            u16 counter;
            u16 field_32;
        } h;
        u32 word;
    } field_30;
    s32 direct_destination;
    void *callback_data;
    u32 position;
    u32 result;
    u16 buffer_index;
    u8 done;
    u8 substate;
};

typedef char FileTransferDescriptor_size_must_be_0x48[
    sizeof(FileTransferDescriptor) == 0x48 ? 1 : -1
];
typedef char FileTransferDescriptor_w_offset_must_be_0x04[
    YGO_TYPE_OFFSET(FileTransferDescriptor, w) == 0x04 ? 1 : -1
];
typedef char FileTransferDescriptor_h_offset_must_be_0x06[
    YGO_TYPE_OFFSET(FileTransferDescriptor, h) == 0x06 ? 1 : -1
];
typedef char FileTransferDescriptor_value_08_offset_must_be_0x08[
    YGO_TYPE_OFFSET(FileTransferDescriptor, value_08) == 0x08 ? 1 : -1
];
typedef char FileTransferDescriptor_total_bytes_offset_must_be_0x10[
    YGO_TYPE_OFFSET(FileTransferDescriptor, total_bytes) == 0x10 ? 1 : -1
];
typedef char FileTransferDescriptor_mode_offset_must_be_0x1C[
    YGO_TYPE_OFFSET(FileTransferDescriptor, mode) == 0x1C ? 1 : -1
];
typedef char FileTransferDescriptor_phase_callback_offset_must_be_0x20[
    YGO_TYPE_OFFSET(FileTransferDescriptor, phase_callback) == 0x20 ? 1 : -1
];
typedef char FileTransferDescriptor_absolute_lba_offset_must_be_0x24[
    YGO_TYPE_OFFSET(FileTransferDescriptor, absolute_lba) == 0x24 ? 1 : -1
];
typedef char FileTransferDescriptor_status_flags_offset_must_be_0x2C[
    YGO_TYPE_OFFSET(FileTransferDescriptor, status_flags) == 0x2C ? 1 : -1
];
typedef char FileTransferDescriptor_counter_offset_must_be_0x30[
    YGO_TYPE_OFFSET(FileTransferDescriptor, field_30.h.counter) == 0x30 ? 1 : -1
];
typedef char FileTransferDescriptor_field_32_offset_must_be_0x32[
    YGO_TYPE_OFFSET(FileTransferDescriptor, field_30.h.field_32) == 0x32 ? 1 : -1
];
typedef char FileTransferDescriptor_callback_data_offset_must_be_0x38[
    YGO_TYPE_OFFSET(FileTransferDescriptor, callback_data) == 0x38 ? 1 : -1
];
typedef char FileTransferDescriptor_done_offset_must_be_0x46[
    YGO_TYPE_OFFSET(FileTransferDescriptor, done) == 0x46 ? 1 : -1
];
typedef char FileTransferDescriptor_substate_offset_must_be_0x47[
    YGO_TYPE_OFFSET(FileTransferDescriptor, substate) == 0x47 ? 1 : -1
];

/* Sound's staged command and the loader's two request slots are the same
   record: func_80045514 passes it to func_80014C40 for a 0x20-byte copy. */
typedef struct {
    s32 field_00;
    s32 field_04;
    u8 pad_08[4];
    s32 field_0C;
    s32 field_10;
    s32 field_14;
    s32 field_18;
    s16 field_1C;
    u8 field_1E;
    u8 field_1F;
} FileRequestSlot;

typedef char FileRequestSlot_size_must_be_0x20[
    sizeof(FileRequestSlot) == 0x20 ? 1 : -1
];
typedef char FileRequestSlot_field_00_offset_must_be_0x00[
    YGO_TYPE_OFFSET(FileRequestSlot, field_00) == 0x00 ? 1 : -1
];
typedef char FileRequestSlot_field_04_offset_must_be_0x04[
    YGO_TYPE_OFFSET(FileRequestSlot, field_04) == 0x04 ? 1 : -1
];
typedef char FileRequestSlot_field_0C_offset_must_be_0x0C[
    YGO_TYPE_OFFSET(FileRequestSlot, field_0C) == 0x0C ? 1 : -1
];
typedef char FileRequestSlot_field_10_offset_must_be_0x10[
    YGO_TYPE_OFFSET(FileRequestSlot, field_10) == 0x10 ? 1 : -1
];
typedef char FileRequestSlot_field_14_offset_must_be_0x14[
    YGO_TYPE_OFFSET(FileRequestSlot, field_14) == 0x14 ? 1 : -1
];
typedef char FileRequestSlot_field_18_offset_must_be_0x18[
    YGO_TYPE_OFFSET(FileRequestSlot, field_18) == 0x18 ? 1 : -1
];
typedef char FileRequestSlot_field_1C_offset_must_be_0x1C[
    YGO_TYPE_OFFSET(FileRequestSlot, field_1C) == 0x1C ? 1 : -1
];
typedef char FileRequestSlot_field_1E_offset_must_be_0x1E[
    YGO_TYPE_OFFSET(FileRequestSlot, field_1E) == 0x1E ? 1 : -1
];
typedef char FileRequestSlot_field_1F_offset_must_be_0x1F[
    YGO_TYPE_OFFSET(FileRequestSlot, field_1F) == 0x1F ? 1 : -1
];
#undef YGO_TYPE_OFFSET

#define FADE_BAND_COUNT 30
#define FADE_TRANSITION_STATE_SIZE 0x28

typedef struct {
    u8 tint_r;
    u8 tint_g;
    u8 tint_b;
    u8 pad_03;
    u8 level;
    u8 target_level;
    u8 flags;
    u8 step;
    u16 field_08;
    u8 band_levels[FADE_BAND_COUNT];
} FadeTransitionState;

typedef char FadeTransitionState_size_must_be_0x28[
    sizeof(FadeTransitionState) == FADE_TRANSITION_STATE_SIZE ? 1 : -1
];
typedef char FadeTransitionState_level_offset_must_be_0x04[
    (u32)&((FadeTransitionState *)0)->level == 0x04 ? 1 : -1
];
typedef char FadeTransitionState_target_level_offset_must_be_0x05[
    (u32)&((FadeTransitionState *)0)->target_level == 0x05 ? 1 : -1
];
typedef char FadeTransitionState_flags_offset_must_be_0x06[
    (u32)&((FadeTransitionState *)0)->flags == 0x06 ? 1 : -1
];
typedef char FadeTransitionState_step_offset_must_be_0x07[
    (u32)&((FadeTransitionState *)0)->step == 0x07 ? 1 : -1
];
typedef char FadeTransitionState_field_08_offset_must_be_0x08[
    (u32)&((FadeTransitionState *)0)->field_08 == 0x08 ? 1 : -1
];
typedef char FadeTransitionState_band_levels_offset_must_be_0x0A[
    (u32)&((FadeTransitionState *)0)->band_levels == 0x0A ? 1 : -1
];

/* Display-object script state shared by the five handlers at 0x8004141C.
   The two pointers delimit the script buffer and the trailing halfwords are
   handler status fields. */
typedef struct {
    u8 pad_00[4];
    u32 flags;
    u8 pad_08[0x48];
    u8 *current;
    u8 *base;
    s16 field_58;
    s16 field_5A;
} DisplayObjectStreamState;

typedef char DisplayObjectStreamState_size_must_be_0x5C[
    sizeof(DisplayObjectStreamState) == 0x5C ? 1 : -1
];
typedef char DisplayObjectStreamState_current_offset_must_be_0x50[
    (u32)&((DisplayObjectStreamState *)0)->current == 0x50 ? 1 : -1
];
typedef char DisplayObjectStreamState_base_offset_must_be_0x54[
    (u32)&((DisplayObjectStreamState *)0)->base == 0x54 ? 1 : -1
];
typedef char DisplayObjectStreamState_field_58_offset_must_be_0x58[
    (u32)&((DisplayObjectStreamState *)0)->field_58 == 0x58 ? 1 : -1
];
typedef char DisplayObjectStreamState_field_5A_offset_must_be_0x5A[
    (u32)&((DisplayObjectStreamState *)0)->field_5A == 0x5A ? 1 : -1
];

typedef struct {
    u32 field_00;
    u16 field_04;
    u16 field_06;
    u32 field_08;
    u16 field_0C;
    u16 field_0E;
    u32 field_10;
    u32 field_14;
} DuelStatusDigitPacket;

typedef char DuelStatusDigitPacket_size_must_be_0x18[
    sizeof(DuelStatusDigitPacket) == 0x18 ? 1 : -1
];
typedef char DuelStatusDigitPacket_field_14_offset_must_be_0x14[
    (u32)&((DuelStatusDigitPacket *)0)->field_14 == 0x14 ? 1 : -1
];

/* Two words written together as a pair. free_duel/screen_runtime.c and
 * password/shop.c formerly defined this identically to view D_801D5608, and
 * each writes both members at once -- the low word from a table entry and the
 * high word from the index beside it. The password writer,
 * Password_UpdateShopScreen, is now a build-integrated candidate
 * (src/candidates/password/func_8016A37C.c).
 *
 * game/text_staging.h owns the declaration of D_801D5608. Pair is reached
 * through that header's union member `pair`, not through a guarded extern
 * view of its own; only the separate starchip alias is still guarded. Pair
 * remains the overlays' view, not a claim that the staging area always holds
 * this shape. Main_RunCredits uses it for the two four-digit secret-number
 * components while preserving the same absolute-address staging accesses. */
typedef struct {
    u32 lo;
    u32 hi;
} Pair;

typedef char Pair_size_must_be_8[
    sizeof(Pair) == 8 ? 1 : -1
];

/* One colour triple with a carried fourth byte. game/triangle_subdivision.c
   subdivides a triangle and writes its corners through this type; the four
   functions in that unit read and write three bytes each and none of them
   reads a fourth byte or a stride, so the fourth is carried by the stride of
   4 rather than used. This is the file's only array typedef, so it takes no
   size assertion -- the extent is in the declaration. */
typedef u8 Triplet[4];

/* One of the two values the value-setup screen edits.
   MainMenu_StartValueSetup seeds value and shown from *out, the update steps
   shown toward value, the drawing code reads shown, and
   MainMenu_FinishValueSetup writes value back through out. Two of them fill
   the 0x18 bytes from D_801845C0 up to D_801845D8, and the merged rows for
   that screen in config/slus_01411/overlays/main_menu_functions.csv read the
   two displayed halfwords at +2 and +0xE, which is the same 0xC stride the
   assertion below states. */
typedef struct {
    u16 value;
    u16 shown;
    u16 *out;
    u8 pad_08[4];
} ValueSetupEntry;

typedef char ValueSetupEntry_size_must_be_0xC[
    sizeof(ValueSetupEntry) == 0xC ? 1 : -1
];

#endif
