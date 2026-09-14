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

/* Unpacked colour channels used by the tint pipeline. Most callers keep the
 * BGR555 0..31 range; the inverse transform may clamp a channel to 0xFF. */
typedef struct {
    u8 r;
    u8 g;
    u8 b;
} Color;

/* Fixed-point hue plus the pipeline's lightness/saturation pair. The retained
 * HsvT name is historical; the conversion branches use HSL lightness rules. */
typedef struct {
    s32 h;
    u16 s;
    u16 v;
} HsvT;

/* SXY2 as returned by the GTE. x remains unsigned to preserve lhu consumers;
 * y is signed because projection sites bias it through signed arithmetic. */
typedef struct {
    u16 x;
    s16 y;
} ProjectedPair;

typedef struct {
    s16 x;
    s16 y;
} ScreenPair;

/* Word/halfword views required by sprite builders that copy paired fields
 * with one load or store while other paths update their individual halves. */
typedef union {
    s32 word;
    struct {
        u16 x;
        u16 y;
    } h;
} SpritePos;

typedef union {
    u16 word;
    struct {
        u8 lo;
        u8 hi;
    } b;
} SpriteHalf;

typedef struct {
    u32 attribute;
    SpritePos xy;
    union {
        u32 word;
        struct {
            SpriteHalf w;
            u16 h;
        } wh;
    } extent;
    u16 tpage;
    SpriteHalf uv;
    union {
        u32 word;
        struct {
            u16 cx;
            u16 cy;
        } h;
    } cxcy;
    u32 rgb;
    SpritePos mxmy;
    u32 scale;
    s32 rotate;
} SpritePrim;

/* Scratchpad clip result at 0x1F800378. */
typedef struct {
    u32 unk0;
    u32 flag;
    u8 pad8[0x18];
    u8 out[4];
} ClipState;

typedef char Color_size_must_be_3[
    sizeof(Color) == 3 ? 1 : -1
];
typedef char HsvT_size_must_be_8[
    sizeof(HsvT) == 8 ? 1 : -1
];
typedef char ProjectedPair_size_must_be_4[
    sizeof(ProjectedPair) == 4 ? 1 : -1
];
typedef char ScreenPair_size_must_be_4[
    sizeof(ScreenPair) == 4 ? 1 : -1
];
typedef char SpritePrim_size_must_be_0x24[
    sizeof(SpritePrim) == 0x24 ? 1 : -1
];
typedef char SpritePrim_cy_must_be_at_0x12[
    (u32)&(((SpritePrim *)0)->cxcy.h.cy) == 0x12 ? 1 : -1
];
typedef char ClipState_size_must_be_0x24[
    sizeof(ClipState) == 0x24 ? 1 : -1
];

typedef u8 *(*ModelHandler)(u8 **);
typedef void (*ScriptCommandHandler)(void);

typedef struct {
    u8 b[11];
} SDInitBlk11;

typedef struct {
    u8 b[10];
} SDInitBlk10;

typedef struct {
    void *model;
    u8 pad_04[0x14];
    s8 field_18;
} DuelCardRenderHolder;

typedef struct {
    s16 x;
    s16 y;
    s16 z;
    u16 rotation_x;
    u16 rotation_y;
    u16 rotation_z;
    s16 scale_x;
    s16 scale_y;
    s16 scale_z;
} ModelAnimationSample;

typedef struct {
    s32 values[2];
} ModelSeparationPair;

typedef struct {
    u8 field_0;
    u8 field_1;
    u8 field_2;
    u8 field_3;
    u8 field_4;
    u8 field_5;
    u8 field_6;
    u8 field_7;
    u16 field_8;
    u8 field_A;
    u8 field_B;
} Func80028B08Ctx;

typedef struct {
    u32 field_0;
    u32 field_4;
} Func80028B08Extra;

typedef struct {
    u32 a;
    u32 b;
} SoundCommandPair;

typedef struct {
    u16 field_0;
    u16 count;
    u8 pad_04[0xC];
    u16 indices[1];
} SoundIndexList;

typedef struct {
    u32 words[4];
} MainMenuTradeBlock16;

typedef struct {
    u32 words[256];
} MainMenuTradeBlock1024;

typedef union {
    u64 all;
    struct {
        s32 column;
        s32 stride;
    } parts;
} PasswordGlyphCoordinates;

typedef char DuelCardRenderHolder_size_must_be_0x1C[
    sizeof(DuelCardRenderHolder) == 0x1C ? 1 : -1
];
typedef char ModelAnimationSample_size_must_be_0x12[
    sizeof(ModelAnimationSample) == 0x12 ? 1 : -1
];
typedef char ModelSeparationPair_size_must_be_8[
    sizeof(ModelSeparationPair) == 8 ? 1 : -1
];
typedef char Func80028B08Ctx_size_must_be_0xC[
    sizeof(Func80028B08Ctx) == 0xC ? 1 : -1
];
typedef char Func80028B08Extra_size_must_be_8[
    sizeof(Func80028B08Extra) == 8 ? 1 : -1
];
typedef char SoundCommandPair_size_must_be_8[
    sizeof(SoundCommandPair) == 8 ? 1 : -1
];
typedef char SoundIndexList_indices_offset_must_be_0x10[
    (u32)&(((SoundIndexList *)0)->indices) == 0x10 ? 1 : -1
];
typedef char MainMenuTradeBlock16_size_must_be_0x10[
    sizeof(MainMenuTradeBlock16) == 0x10 ? 1 : -1
];
typedef char MainMenuTradeBlock1024_size_must_be_0x400[
    sizeof(MainMenuTradeBlock1024) == 0x400 ? 1 : -1
];
typedef char PasswordGlyphCoordinates_size_must_be_8[
    sizeof(PasswordGlyphCoordinates) == 8 ? 1 : -1
];

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

#define TEXT_STREAM_SLOT_COUNT 22

/* Narrow text-command view: twenty-two stream pointers place the signed
 * selector at the measured 0x58 offset. */
typedef struct {
    u8 *streams[TEXT_STREAM_SLOT_COUNT];
    s8 stream_index;
} TextStreamOwner;

/* Display-effect command view. Its depth selector follows twenty stream
 * pointers and the command state bytes at the same measured 0x58 offset. */
typedef struct {
    u8 *streams[20];
    u8 unk50;
    u8 state;
    u8 pad52[6];
    s8 depth;
} EffectObject;

/* One 0x14-byte scene-script slot at D_800EAE98. */
typedef struct {
    s32 unk00;
    s16 unk04;
    s16 unk06;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
} SceneScriptSlot;

/* Script image slot prefix: the owned display object and its image id. */
typedef struct {
    void *pointer;
    s16 value;
    u8 pad_06[10];
    /* Set to 1 by ScriptImage_CreateObject and ScriptImage_RebuildObjects
       whenever they build a display object into the slot; no matched code
       reads it yet. */
    u8 field_10;
    u8 pad_11[3];
} ScriptImageEntry;

struct DuelEffectChannel;
typedef void (*SceneScriptRecordCallback)(void *, s32);
typedef void (*TextBoxStateCallback)(struct DuelEffectChannel *);

typedef char TextStreamOwner_stream_index_offset_must_be_0x58[
    YGO_TYPE_OFFSET(TextStreamOwner, stream_index) == 0x58 ? 1 : -1
];
typedef char EffectObject_depth_offset_must_be_0x58[
    YGO_TYPE_OFFSET(EffectObject, depth) == 0x58 ? 1 : -1
];
typedef char SceneScriptSlot_size_must_be_0x14[
    sizeof(SceneScriptSlot) == 0x14 ? 1 : -1
];
typedef char ScriptImageEntry_size_must_be_0x14[
    sizeof(ScriptImageEntry) == 0x14 ? 1 : -1
];
typedef char ScriptImageEntry_field_10_offset_must_be_0x10[
    YGO_TYPE_OFFSET(ScriptImageEntry, field_10) == 0x10 ? 1 : -1
];

struct DisplayObject;

/* The Library cursor's motion record at D_800EA1E8, 0x48 bytes. func_80029590,
   func_8002A3CC and func_8002A4A8 reach it through the typed declaration in
   game/func_8002A3CC.h, and func_8002BFCC through a cast of the byte view that
   game/library_runtime.h declares at the same address. */
typedef struct {
    u8 pad_00[8];
    s16 x;
    s16 y;
    u16 x_fraction;
    u16 y_fraction;
    u8 pad_10[2];
    u16 rest_x;
    u16 rest_y;
    u8 frames;
    u8 active;
    s32 velocity_x;
    s32 velocity_y;
    u8 pad_20[4];
    /* Eight display object pointers, 0x24 through 0x40, which func_80029590
       fills one per iteration. They were inside pad_20 until now; naming them
       moves nothing, and render still begins at 0x44 immediately after the
       last of them. */
    struct DisplayObject *slots[8];
    struct DisplayObject *render;
} LibraryMotionState;

typedef char LibraryMotionState_x_offset_must_be_0x8[
    YGO_TYPE_OFFSET(LibraryMotionState, x) == 0x8 ? 1 : -1
];
typedef char LibraryMotionState_frames_offset_must_be_0x16[
    YGO_TYPE_OFFSET(LibraryMotionState, frames) == 0x16 ? 1 : -1
];
typedef char LibraryMotionState_velocity_x_offset_must_be_0x18[
    YGO_TYPE_OFFSET(LibraryMotionState, velocity_x) == 0x18 ? 1 : -1
];
typedef char LibraryMotionState_render_offset_must_be_0x44[
    YGO_TYPE_OFFSET(LibraryMotionState, render) == 0x44 ? 1 : -1
];
typedef char LibraryMotionState_size_must_be_0x48[
    sizeof(LibraryMotionState) == 0x48 ? 1 : -1
];

/* One text-box record, 0x64 bytes, the element type of D_800EB0F8. 0x00 is the
   decoded string the record is playing back (TextBox_BuildStep stores it
   there), and 0x20/0x24 bracket the record's slice of D_800EB288:
   TextBox_BuildStep seeds both with &D_800EB288[range_start_5C],
   DuelEffect_ProcessEntries walks from 0x24 and moves 0x20 as it compacts. */
typedef struct DuelEffectChannel {
    u8 *text_00;
    /* The fade callbacks in D_80090EAC reach this block. They are reached as
       bytes at 0x04-0x0A, as halfwords at 0x0C and 0x0E, and as single bytes
       at 0x13-0x15; 0x04 is additionally written as one word (0 and
       0x80808080), which those sites spell as a width over field_04 rather
       than a separate member.

       0x13 is the byte DisplayObjectFade_MarkInitialized tests and sets, and
       the one carrying DISPLAY_OBJECT_FADE_FLAG_*. The rest keep field_NN:
       0x04-0x07 and 0x08-0x0A are each written as a run of equal bytes stepped
       together, which says component groups without saying which component
       is which. */
    u8 field_04;
    u8 field_05;
    u8 field_06;
    u8 field_07;
    u8 field_08;
    u8 field_09;
    u8 field_0A;
    u8 pad_0B;
    u16 field_0C;
    u16 field_0E;
    /* The object's slot in the D_800EAF08 occupancy table and the byte
       beside it; func_80039AD4 clears D_800EAF08[field_10] and zeroes
       field_11 when it releases the slot. */
    u8 field_10;
    u8 field_11;
    u8 pad_12;
    u8 field_13;
    u8 field_14;
    u8 field_15;
    u8 pad_16[10];
    DuelEffectEntry *entry_end_20;
    DuelEffectEntry *entry_head_24;
    /* Every consumer proves this is a DisplayObject pointer:
       func_800391E4 and the func_8002EE94 candidate cast it, card-list text
       reaches ->flags through it, and Dialog_UpdateChoice used to read it
       through a pointer cast. */
    struct DisplayObject *field_28;
    /* The second owned display object has the same evidence: producers store
       the object they just built and consumers release it through
       func_8004036C. Integer-looking field_2C writes elsewhere belong to the
       unrelated DuelEffectResourceRecord. */
    struct DisplayObject *field_2C;
    void *field_30;
    u16 flags_34;
    u16 field_36;
    u16 field_38;
    u16 field_3A;
    s16 field_3C;
    s16 field_3E;
    s16 field_40;
    s16 field_42;
    /* The glyph buffer func_80038148 formats a decimal number into; it then
       pushes the buffer as the channel's next stream. */
    u8 text_44[0x0D];
    /* TextBox_BuildStep dispatches on the low five bits through D_80090E64.
       0x80 is an initialization latch set by every callback on entry. */
    u8 state_51;
    /* Reloaded from field_53, decremented once per call, and used to return
       while the countdown remains nonzero. */
    u8 delay_52;
    u8 field_53;
    u8 field_54;
    u8 pad_55;
    u8 field_56;
    u8 index_57;
    /* Which leading pointer word is the live byte stream. Every reader scales
       it by four; it is signed because all three consumers read it as s8. */
    s8 stream_58;
    u8 field_59;
    u8 field_5A;
    u8 field_5B;
    u16 range_start_5C;
    u16 range_count_5E;
    u8 field_60;
    u8 field_61;
    /* Written with a type value by func_80037DA4, Text_CloseChoice, and
       duel_effect_entry_control.c, then read by func_80036C14. The byte's
       meaning remains unresolved. */
    u8 field_62;
    u8 pad_63;
} DuelEffectChannel;

typedef char DuelEffectChannel_size_must_be_0x64[
    sizeof(DuelEffectChannel) == 0x64 ? 1 : -1
];
typedef char DuelEffectChannel_entry_end_20_offset_must_be_0x20[
    YGO_TYPE_OFFSET(DuelEffectChannel, entry_end_20) == 0x20 ? 1 : -1
];
typedef char DuelEffectChannel_entry_head_24_offset_must_be_0x24[
    YGO_TYPE_OFFSET(DuelEffectChannel, entry_head_24) == 0x24 ? 1 : -1
];
typedef char DuelEffectChannel_field_28_offset_must_be_0x28[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_28) == 0x28 ? 1 : -1
];
typedef char DuelEffectChannel_field_2C_offset_must_be_0x2C[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_2C) == 0x2C ? 1 : -1
];
typedef char DuelEffectChannel_field_30_offset_must_be_0x30[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_30) == 0x30 ? 1 : -1
];
typedef char DuelEffectChannel_flags_34_offset_must_be_0x34[
    YGO_TYPE_OFFSET(DuelEffectChannel, flags_34) == 0x34 ? 1 : -1
];
typedef char DuelEffectChannel_field_3C_offset_must_be_0x3C[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_3C) == 0x3C ? 1 : -1
];
typedef char DuelEffectChannel_state_51_offset_must_be_0x51[
    YGO_TYPE_OFFSET(DuelEffectChannel, state_51) == 0x51 ? 1 : -1
];
typedef char DuelEffectChannel_field_53_offset_must_be_0x53[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_53) == 0x53 ? 1 : -1
];
typedef char DuelEffectChannel_field_56_offset_must_be_0x56[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_56) == 0x56 ? 1 : -1
];
typedef char DuelEffectChannel_index_57_offset_must_be_0x57[
    YGO_TYPE_OFFSET(DuelEffectChannel, index_57) == 0x57 ? 1 : -1
];
typedef char DuelEffectChannel_stream_58_offset_must_be_0x58[
    YGO_TYPE_OFFSET(DuelEffectChannel, stream_58) == 0x58 ? 1 : -1
];
typedef char DuelEffectChannel_field_5A_offset_must_be_0x5A[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_5A) == 0x5A ? 1 : -1
];
typedef char DuelEffectChannel_range_start_5C_offset_must_be_0x5C[
    YGO_TYPE_OFFSET(DuelEffectChannel, range_start_5C) == 0x5C ? 1 : -1
];
typedef char DuelEffectChannel_field_61_offset_must_be_0x61[
    YGO_TYPE_OFFSET(DuelEffectChannel, field_61) == 0x61 ? 1 : -1
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

/* Known prefixes of the password shop's preview and digit-cursor objects. */
typedef struct {
    u8 pad0[0x8];
    u16 flags;
    u8 pad0A[0x17];
    u8 phase;
    u8 pad22[0x10];
    u16 y;
} PasswordCardPreviewView;

typedef void (*PasswordCursorUpdate)(u8 *object);

typedef struct {
    u8 pad00[0x8];
    u16 flags;
    u8 pad0A[0x0E];
    s16 target_x;
    s16 target_y;
    u8 pad1C[0x14];
    s16 x;
    s16 y;
    u8 pad34[0x2C];
    s16 timer;
    u8 pad62[0x7];
    u8 kind;
    u8 pad6A[0x2];
    u8 updateFlags;
} PasswordCursorView;

typedef struct {
    u8 flags;
    s8 keyboardColumn;
    u8 keyboardRow;
    u8 resetState;
    SelectionFrame *selectionFrame;
    u8 glyphSequence;
    u8 unknown409[7];
    u8 digits[8];
    u8 *nameBuffer;
    u8 dialogState;
    u8 unknown41D[3];
    PasswordCursorView *digitCursor;
    u16 displayedStarchips;
    u8 savedKeyboardRow;
    u8 unknown427;
    s32 digitIndex;
    s8 caretIndex;
    u8 unknown42D[3];
    u8 *cardCache;
    s16 cursorTargetX;
    s16 cursorTargetY;
    u32 cardPrice;
    u8 *lengthObject;
    u8 *digitDecorations[4];
    u8 unknown450[0x80];
    u8 resetMode;
    u8 unknown4D1;
    u16 dialogId;
    u16 keyboardFlags;
    u16 unknown4D6;
    PasswordCardPreviewView *cardPreview;
    u16 cardId;
    u8 unknown4DE[0xB2];
} PasswordModuleState;

typedef char PasswordModuleState_size_must_be_0x190[
    sizeof(PasswordModuleState) == 0x190 ? 1 : -1
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
    /* Not volatile: func_80013C28 stores it in a branch delay slot, and
       a volatile member keeps that store out of the slot (+0x1C bytes). */
    s32 total_bytes;
    s32 file_bytes;
    u8 *loader_argument;
    /* Total byte count for the current callback-programmed transfer phase.
       func_8001513C copies it back into phase_remaining after each callback. */
    u32 phase_size;
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
typedef char FileTransferDescriptor_phase_size_offset_must_be_0x1C[
    YGO_TYPE_OFFSET(FileTransferDescriptor, phase_size) == 0x1C ? 1 : -1
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

#define FILE_TRANSFER_DESCRIPTOR_WORD_COUNT 18

/* File_ActivateTransfer copies a whole descriptor through aligned words.
   This is the block-move view of FileTransferDescriptor, not a second record
   description; the element type is what selects the copy width. */
typedef struct {
    s32 value[FILE_TRANSFER_DESCRIPTOR_WORD_COUNT];
} FileTransferDescriptorWords;

typedef char FileTransferDescriptorWords_size_must_match_descriptor[
    sizeof(FileTransferDescriptorWords) == sizeof(FileTransferDescriptor) ? 1 : -1
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

/* One row of the eight-byte transfer request table func_8005FB30 walks: the
   model id it is asked to stage and the state it reports back. */
typedef struct {
    s16 id;
    u8 pad_02[4];
    s16 state;
} ModelTransferItem;

typedef char ModelTransferItem_size_must_be_8[
    sizeof(ModelTransferItem) == 8 ? 1 : -1
];
typedef char ModelTransferItem_id_offset_must_be_0[
    YGO_TYPE_OFFSET(ModelTransferItem, id) == 0 ? 1 : -1
];
typedef char ModelTransferItem_state_offset_must_be_6[
    YGO_TYPE_OFFSET(ModelTransferItem, state) == 6 ? 1 : -1
];

/* One movie-stream entry. sector_count advances to the next stream and a
   nonzero end_frame overrides the caller's frame limit. */
typedef struct {
    u16 sector_count;
    u16 end_frame;
} MovieStreamRange;

typedef char MovieStreamRange_size_must_be_4[
    sizeof(MovieStreamRange) == 4 ? 1 : -1
];
typedef char MovieStreamRange_end_frame_offset_must_be_2[
    YGO_TYPE_OFFSET(MovieStreamRange, end_frame) == 2 ? 1 : -1
];

/* The updater selects the duel outcome with a halfword cursor; the text
   producer reads the same two counters with signed extension. */
typedef union {
    struct {
        u16 wins;
        u16 losses;
    } result;
    u16 counts[2];
} SaveDataDuelistRecord;

typedef char SaveDataDuelistRecord_size_must_be_4[
    sizeof(SaveDataDuelistRecord) == FREE_DUEL_GRID_RECORD_SIZE ? 1 : -1
];
typedef char SaveDataDuelistRecord_losses_offset_must_be_2[
    YGO_TYPE_OFFSET(SaveDataDuelistRecord, result.losses) == sizeof(u16) ? 1 : -1
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

/* Display-object script state shared by the seven D_80090FEC handlers,
   func_8004141C through func_80041534.
   The two pointers delimit the script buffer and the trailing halfwords are
   handler status fields. */
typedef struct {
    u8 pad_00[4];
    u32 flags;
    u8 pad_08[0x1A];
    /* func_80041534 (opcode 0xF9) loads its first operand byte here. */
    u8 field_22;
    u8 pad_23[0x25];
    /* func_80041534 stores its little-endian halfword operand in field_48
       and its sign-extended second operand byte in field_4A. */
    s16 field_48;
    s16 field_4A;
    /* func_80041C8C points this at the current opcode's operand target:
       base plus the little-endian halfword that follows the opcode. */
    u8 *field_4C;
    u8 *current;
    u8 *base;
    s16 field_58;
    s16 field_5A;
} DisplayObjectStreamState;

typedef char DisplayObjectStreamState_size_must_be_0x5C[
    sizeof(DisplayObjectStreamState) == 0x5C ? 1 : -1
];
typedef char DisplayObjectStreamState_field_22_offset_must_be_0x22[
    (u32)&((DisplayObjectStreamState *)0)->field_22 == 0x22 ? 1 : -1
];
typedef char DisplayObjectStreamState_field_48_offset_must_be_0x48[
    (u32)&((DisplayObjectStreamState *)0)->field_48 == 0x48 ? 1 : -1
];
typedef char DisplayObjectStreamState_field_4A_offset_must_be_0x4A[
    (u32)&((DisplayObjectStreamState *)0)->field_4A == 0x4A ? 1 : -1
];
typedef char DisplayObjectStreamState_field_4C_offset_must_be_0x4C[
    (u32)&((DisplayObjectStreamState *)0)->field_4C == 0x4C ? 1 : -1
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

typedef union {
    u64 d;
    Pair w;
} Pair64;

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
