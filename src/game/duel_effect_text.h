#ifndef MEMORIES_DECOMP_DUEL_EFFECT_TEXT_H
#define MEMORIES_DECOMP_DUEL_EFFECT_TEXT_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "duel_effect_states.h"
#include "display_object.h"
#include "../ygo_types.h"

/* Dialog_OpenChoice builds the choice/page cursor for a dialog record and hands
 * it back -- the caller stores it at the record's +0x30 and releases it through
 * DisplayObject_ReleaseIfPresent when the state ends. The frame pump handles
 * choice opening/closing; Text_WaitForPageAdvance owns the page-wait path.
 *
 * Four files declared Dialog_OpenChoice under three spellings between them,
 * `u8 *(u8 *)`, `u8 *(void *)` and `void *(void *)`. Every caller passes the
 * same record pointer and stores the result at the same channel slot, so the
 * differences were views of one signature.
 *
 * The parameter is now the record's own type. The five fields this function
 * reads -- rect_x, rect_width, rect_y, rect_height and depth_offset -- are all
 * declared in DuelEffectChannel at those offsets and at these widths, and
 * every caller, duel_effect_dialog_state.c included, holds a DuelEffectChannel *.
 *
 * The returned display object is stored in the channel's prompt_object slot and
 * released through the display-object lifecycle helpers. */
struct DisplayObject *Dialog_OpenChoice(DuelEffectChannel *record);

/* Choice-input polling and entry repainting. The constructor only builds the
 * cursor; the independent page-wait callback handles page input.
 * text_box_build_step.c installs this in two adjacent state slots. */
void Dialog_UpdateChoice(DuelEffectChannel *object);

struct DuelEffectChannel;

void Dialog_HighlightChoice(struct DuelEffectChannel *record);

struct DuelEffectChannel;

s32 Dialog_ReadChoiceInput(struct DuelEffectChannel *record);

#define DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE 0x40
#define DISPLAY_OBJECT_FADE_FLAG_INITIALIZED 0x80

s32 DisplayObjectFade_MarkInitialized(DuelEffectChannel *);

/* Releases the entry's occupancy claim and requests list compaction, not a
 * heap allocation. Regional occupancy clearing remains in the implementation. */
void DisplayObjectFade_ReleaseChannel(struct DuelEffectChannel *object);

/* The four duel effect entry handlers in gDuelEffect_EntryHandlers
 * (duel_effect_command_table.c), in table order: DuelEffect_UpdateGlyphConceal,
 * DuelEffect_UpdateGlyphReveal, DuelEffect_UpdateGlyphConceal again, DuelEffect_UpdateDelayedGlyphRotation.
 *
 * The paired shade/geometry transitions and delayed three-axis rotation use
 * DisplayObjectFade_MarkInitialized for their first-invocation setup and
 * DISPLAY_OBJECT_FADE_FLAG_SECOND_PHASE in byte 0x13 for the phase.
 *
 *   DuelEffect_UpdateGlyphReveal  Raises byte 4, then byte 5, by 16 per elapsed frame
 *                  (gGraphics_nBaseFrameStep), copying them into bytes 6 and 7. When byte 5
 *                  passes 0x7F it clears bytes 0x13 to 0x15 itself.
 *   DuelEffect_UpdateGlyphConceal  Starts the colour word at +4 at 0x80808080 and lowers
 *                  bytes 4/5, then 6/7, to zero at the same rate.
 *   DuelEffect_UpdateDelayedGlyphRotation counts the position-derived byte
 *   delay down, then raises rotation components +8..+A by four per invocation
 *   until0x40. The renderer multiplies them by16; these are not color bytes.
 *
 * The last two end by releasing the record's occupancy slot through
 * DisplayObjectFade_ReleaseChannel. */
void DuelEffect_UpdateGlyphReveal(DuelEffectChannel *record);
void DuelEffect_UpdateGlyphConceal(DuelEffectChannel *record);
void DuelEffect_UpdateDelayedGlyphRotation(DuelEffectChannel *record);

/* Secondary command zero: reads a modifier byte and expands the selected
 * card's name/description, type or guardian field, or applies its staged
 * color. The card id is the existing gDuel_wSelectedCardID, not a new operand.
 *
 * The stream-slot offset first lives in `text`; a block-scoped stream pointer
 * advances from the object base and is then coalesced back into `text`. That
 * lifetime split preserves the retail allocation without hard-register
 * bindings. */
void Text_ExpandSelectedCardInfo(DuelEffectChannel *object);

/* Shared by the guardian and one-byte symbol wrappers, not a table entry:
 * raises the regional symbol flag (0x80, Europe 0x100), appends the glyph,
 * clears that flag and advances the pen by 0x10. */
void Text_EmitSymbolGlyph(DuelEffectChannel *object, s32 value);

/* Secondary command eight: emits the staged guardian glyph without consuming
 * a stream operand. */
void Text_EmitGuardianStarGlyph(DuelEffectChannel *object);

/* Secondary command eleven: consumes one symbol-code byte before emission. */
void Text_EmitNextSymbolGlyph(DuelEffectChannel *object);
/* Secondary command one: reset X, then add a signed operand byte to Y.
 * Both pen fields retain unsigned halfword storage and wrapping. */
void Text_ResetPenXAndAdvanceY(DuelEffectChannel *object);
/* Secondary command two: add an unsigned operand byte to X. */
void Text_AdvancePenX(DuelEffectChannel *object);

/* gText_SecondaryCommandHandlers entry: formats a decimal number into the object's text. The value
 * comes through TextStream_ReadU32LE as a pointer, the width from the low
 * nibble of the next stream byte, and Text_EncodeDecimalDigits does the
 * conversion into a local buffer before the glyphs are appended. */
void Text_PushFormattedDecimal(DuelEffectChannel *object);

/* Secondary command four: regional font mode, not one universal size enum.
 * NA handles modes 1/2, Japan stores operand * 8 into both bytes, and Europe
 * has four size/low-flag cases. Pre-switch flag clears and unknown-mode
 * partial behavior remain distinct. */
void Text_SetFontMode(DuelEffectChannel *object);

/* Secondary command five: separate byte reads/stores for width then height;
 * no font-flag update or metric bound is implied. */
void Text_SetGlyphCellSize(DuelEffectChannel *object);
/* Secondary command 6: read the channel-local glyph pen X from two stream
 * bytes. Wrapping remains a separate operation. */
void Text_SetPenX(DuelEffectChannel *object);
#ifdef VERSION_EUROPE
/* PAL-only two-byte pen-Y operand; no opcode index is assigned here. */
void Text_SetPenY(DuelEffectChannel *object);
#endif
/* Secondary command 7: reset progress and set a glyph (NA/Japan) or pixel
 * (Europe) completion limit. Zero disables this limit check. */
void Text_SetOutputLimit(DuelEffectChannel *object);

/* gText_SecondaryCommandHandlers entry: resolves the text bank pointer for the string id in
 * gText_StagedStringId. North America selects three banks; Japan selects gText_GlobalStringOffsets
 * for ids with bit 15 set and gText_PrimaryStringOffsets otherwise. It hands back the slot.
 *
 * Its parameter is the DuelEffectChannel whose stream_index byte selects one of
 * the pointer words at the front of the record. It returns the selected slot,
 * although the generic command table discards that result. The slot holds
 * a stored byte-stream pointer, not an integer payload word. */
u8 **Text_PushStagedString(DuelEffectChannel *channel);

/* gText_SecondaryCommandHandlers entry: sets the effective colour index at +0x54 from a one-byte
 * operand. With bit 7 set the low nibble indexes gText_abColorSlots instead of
 * being used directly, so a palette entry and a literal colour share one
 * opcode. */
void Text_SetColorIndex(DuelEffectChannel *object);

/* Script fast-print command: clears bit 0x1000 of the object's 0x34 flags and sets it
 * again when its one-byte operand is non-zero. The selected stream uses the
 * shared pointer-slot view; the original register-qualified locals remain. */
void Text_SetFastPrintMode(DuelEffectChannel *object);

/* Returns the selected opaque payload byte address, or zero at the key
 * terminator. Key and payload cursors retain their distinct 2/30-byte steps. */
u8 *Text_FindRecordById(s32 id);

/* Conditionally appends an entry at the channel's 0x20 tail. Header stores
 * may already have happened on a rejected value; no append status is implied.
 *
 * NA/Japan use 0x1C/0x18-byte entries and return void. Europe uses compact
 * 0x16-byte entries and returns a signed spacing adjustment, not an append
 * Boolean or the complete glyph width. Keep each regional flag domain. */
#ifdef VERSION_EUROPE
s32 DuelEffect_AppendEntry(DuelEffectChannel *channel, s32 tagged_value);
#else
void DuelEffect_AppendEntry(DuelEffectChannel *channel, s32 tagged_value);
#endif

#ifdef VERSION_JAPAN
void DuelEffect_RasterizeGlyph(DuelEffectChannel *channel, s32 tagged_value, s32 slot, s16 *bitmap);
/* The sixteen 4bpp pixel patterns for one nibble of glyph bits, and the
 * colour masks a channel's color_index_54 selects among. */
extern u16 gText_awGlyphNibblePatterns[16];
extern u16 gText_awGlyphColorMasks[];
#endif

/* The glyph records Text_FindRecordById walks: a zero-terminated table of
 * big-endian halfword ids and, in lockstep, 0x1E-byte records. The Japanese
 * rasterizer reads a record as fifteen 16-pixel rows of glyph bits; Japan
 * defines its 27 records in japanese/text_glyph_records.c. The North American
 * image has no records: both labels sit just past the 93-entry glyph lookup
 * table, where the linker script binds them. */
extern u8 gText_aGlyphRecordBitmaps[];
extern u8 gText_aGlyphRecordKeys[];

typedef struct {
    u8 pad_00[0x5C];
    u8 entry_range_start;
    u8 entry_range_count;
} JapaneseDuelEffectEntryRange;

/* DuelEffect_HasActiveEntry checks the active prefix for a nonzero control
 * byte. The separate Text_CompletePageAdvance state requests clearing, then
 * tests the first configured entry's active flag on subsequent calls.
 *
 * DuelEffect_SetEntryControlBytes is the third function this unit defines. It was held back
 * until its narrowing parameters could be measured: it is defined
 * `(DuelEffectChannel *, u8, u8)` while both callers declared
 * `(DuelEffectChannel *, s32, s32)`, and a narrowing parameter can be load
 * bearing at the call site. Measured resident calls pass small literals
 * (3/0, 2/0 and 0/0), so the argument is materialised by the same `li`
 * either way and the executable is unchanged. The declaration below is the
 * definition's own. The shared Spanish intro also passes raw control bytes
 * 5, 6, 8 and 9 with render mode zero; those are not a page-only enum. */
s32 DuelEffect_HasActiveEntry(DuelEffectChannel *channel);
void Text_CompletePageAdvance(DuelEffectChannel *object);

/* Starting from the record's entry range, walks up to its regional count;
 * for each with entry_flags & 0x80 set, writes handler_state and render_mode
 * to the entry members of those names (+0x13 and +0x15), stopping at the
 * first entry with that flag clear. */
void DuelEffect_SetEntryControlBytes(DuelEffectChannel *channel, u8 handler_state,
                                     u8 render_mode);

#define JAPANESE_DUEL_EFFECT_ENTRY_COUNT 300

typedef struct {
    /* Word alignment preserves whole-record copies during compaction. */
    u32 pad_00[4];
    u8 pad_10;
    u8 entry_flags;
    u8 owner_channel_tag;
    u8 handler_state;
    u8 pad_14;
    u8 render_mode;
    u8 pad_16[2];
} JapaneseDuelEffectEntry;

typedef char JapaneseDuelEffectEntry_size_must_be_0x18[
    sizeof(JapaneseDuelEffectEntry) == 0x18 ? 1 : -1
];
typedef char JapaneseDuelEffectEntry_alignment_must_be_4[
    sizeof(struct { u8 lead; JapaneseDuelEffectEntry entry; }) == 0x1C ? 1 : -1
];

/* The European build's entries: 800 of them, 0x16 bytes each. The named
   fields are the ones matched code reads, under their US names: the
   marker flag and channel byte (DuelEffect_ClearMatchingMarker,
   DuelEffect_ResetEntryMarkers) and the fade fields of DuelEffect_UpdateGlyphReveal and
   DuelEffect_UpdateDelayedGlyphRotation. */
#define EUROPEAN_DUEL_EFFECT_ENTRY_COUNT 800

typedef struct {
    u16 glyph_code;
    u16 local_x;   /* 0x02 */
    u16 local_y;   /* 0x04 */
    u8 field_04;    /* 0x06 */
    u8 field_05;
    u8 field_06;
    u8 field_07;
    u8 field_08;    /* 0x0A */
    u8 field_09;
    u8 field_0A;
    u8 pad_0D[2];
    u8 entry_flags;    /* 0x0F */
    u8 owner_channel_tag;    /* 0x10 */
    u8 handler_state;    /* 0x11 */
    u8 field_14;
    u8 render_mode;    /* 0x13 */
    u8 pad_14[2];
} EuropeanDuelEffectEntry;

typedef char EuropeanDuelEffectEntry_size_must_be_0x16[
    sizeof(EuropeanDuelEffectEntry) == 0x16 ? 1 : -1
];

void DuelEffect_ClearOccupancyValue(s32 value);
void DuelEffect_ResetOccupancy(void);
/* NA/Japan start coordinates, not cursors updated by the query. Reset clears
 * both; the query walks locals and does not reserve the returned slot. */
extern u8 gDuelEffect_OccupancyStartColumn;
extern u8 gDuelEffect_OccupancyStartRow;
s32 DuelEffect_FindFreeOccupancySlot(void);
void DuelEffect_ClearMatchingMarker(s32 value);
void DuelEffect_ResetEntryMarkers(void);

/* The record DuelEffect_FadeOutBgmCommand walks. It covers only the three
 * fields the handler itself reaches: the command state byte at 0x51 and the
 * signed cursor slot at 0x58 that selects which of the byte streams at the
 * front of the record the opcode is read from. */
typedef struct {
    u8 pad_00[0x51];
    u8 command_state;
    u8 pad_52[6];
    s8 cursor_slot;
} DuelEffectCommand;

/* Takes the record above, not the u8 * the command table's element type
 * spells. The table casts it there. */
void DuelEffect_FadeOutBgmCommand(DuelEffectCommand *command);

/* Initialises channel `index` for string `value`, ORs 0x8000 into its flags,
 * and takes its range from consecutive gDuelEffect_awEntryRangeBoundaries
 * values. Japanese channels have a 0x60-byte stride and byte-sized range
 * fields; the returned pointer still names the selected channel. */
DuelEffectChannel *DuelEffect_InitEntry(s32 index, s32 value, s32 flags);

/* Secondary commands 0x17/0x18: index a u16 target table with independent
 * shared selectors, then patch the same post-read stream pointer's low half.
 * Producer-specific meanings and the regional high mask remain separate. */
void Text_JumpBySelectorA(DuelEffectChannel *object);
void Text_JumpBySelectorB(DuelEffectChannel *object);
/* Secondary text command 0x19: consumes a duelist id and marks both its
 * campaign defeat and Free Duel unlock flags. Zero is a no-op. */
void Text_UnlockDuelist(DuelEffectChannel *object);
/* Secondary text command 0x1A: releases the active choice object and returns
 * the channel to state 2, yielding the current text-building pass. */
void Text_CloseChoice(DuelEffectChannel *object);

#ifdef VERSION_EUROPE
/* European only: pushes the player name's glyph codes as the next text
 * stream (secondary text command 27). */
void Text_PushPlayerNameStream(DuelEffectChannel *object);
#endif

/* The three gText_SecondaryCommandHandlers sound commands. All read their operand through
 * TextStream_ReadU16LE, which is the "next word of this object's current script
 * stream" accessor, so none of them touches the cursor arithmetic directly
 * except DuelEffect_ProcessBgmCommand, which reads a raw opcode byte first.
 *
 * DuelEffect_PlaySoundCommand plays a sound effect or, with bit 15 set, routes
 * the id through SD_CdSoundPlay and latches a second word into
 * gText_CdSoundPollBudget before selecting state 0x11. That word limits
 * qualifying polls; it is not a count of reissued playback requests.
 * DuelEffect_PlayBgmCommand is the plain BGM play.
 * DuelEffect_ProcessBgmCommand reads the BGM control byte: a non-zero low six
 * bits play the next stream word, otherwise bits 0, 1 and 2 replay, latch or
 * copy the current command -- and bit 7 puts the object into wait state 0xC
 * and raises gText_CommandDisposition. */
void DuelEffect_PlaySoundCommand(DuelEffectChannel *object);
void DuelEffect_PlayBgmCommand(DuelEffectChannel *object);
void DuelEffect_ProcessBgmCommand(DuelEffectChannel *object);

/* Clears flags_34 and fields 0x28-0x30 on all four gText_aChannels effect
   channels, then the occupancy table (not in the European build) and the
   entry markers. */
void DuelEffect_ResetState(void);

/* Four gText_SecondaryCommandHandlers entries. Text_OpenCampaignMap reads one byte from the object's
 * current stream into gCampaignMap_bEntryLocation and puts the scene mode at 5.
 *
 * Text_NoOpSecondary12 and Text_NoOpSecondary13 take NO argument at all -- they are defined
 * `void (void)` and their bodies are empty -- while the table's entry type is
 * `void (*)(DuelEffectChannel *)`. The table casts them, which is the same
 * shape #3073 settled on for Text_EmitGuardianStarGlyph: the real prototype lives here and
 * the cast is at the one place that needs it. They are also two distinct
 * addresses rather than one entry repeated. */
void Text_NoOpSecondary12(void);
void Text_NoOpSecondary13(void);
void Text_OpenCampaignMap(DuelEffectChannel *object);
void Text_ApplyFadeCommand(DuelEffectChannel *object);

/* Fifteen entries of the gTextBox_apfnStateCallbacks text-box state table, plus the choice
 * object constructor used by the first interaction state. Each callback takes
 * the state record, tests or sets its 0x51 byte, and advances from there.
 *
 * They are reached only through the table, which indexes on the low five bits
 * of that same 0x51 byte -- so the byte is both the dispatch key and the state
 * each of these updates. */
void Text_BeginPageClear(DuelEffectChannel *object);
void Text_WaitForPageAdvance(DuelEffectChannel *object);
/* Shared signed-byte Square-hold countdown; not per-channel storage. */
extern s8 gText_PageAdvanceHoldCountdown;
/* State 5 stages command 5; optional text operands are Y8 then X16. */
void Text_StageScriptImage(DuelEffectChannel *object);
/* State 11 stages command 7, with the original signed Y-threshold wait. */
void Text_StartViewportTween(DuelEffectChannel *object);
void TextBox_WaitForScriptCompletion(DuelEffectChannel *object);
/* State 6: wait for setup, then finish or run the ordinary continuation once. */
void Text_ContinueDisplayEffect(DuelEffectChannel *object);
/* State 7 releases on step zero; state 8 only observes completion. */
void Text_WaitForDisplayEffectAndRelease(DuelEffectChannel *object);
void Text_WaitForDisplayEffect(DuelEffectChannel *object);
/* States 14/9 gate on the existing low activity bits, then hand off to 8. */
void Text_RequestDisplayEffectFadeOut(DuelEffectChannel *object);
void Text_RequestDisplayEffectCrossfade(DuelEffectChannel *object);
void DuelEffect_WaitForFileTransfer(DuelEffectChannel *object);
void Text_WaitForBgmFade(DuelEffectChannel *object);
/* State 15: initialization consumes a word and decrements in the same call. */
void Text_WaitForDelay(DuelEffectChannel *object);
/* State 16: shares that counter; optional jitter precedes completion restore. */
void Text_WaitWithViewportShake(DuelEffectChannel *object);
/* Shared delay counter and shake origin, not per-channel state. */
extern s16 gText_WaitCounter;
extern u16 gText_ShakeViewportOrigin[2];
void Text_WaitForCdSoundTransfer(DuelEffectChannel *object);

/* Shared CD-sound wait phase, not the independent script-dispatch gate. */
extern u8 gText_CdSoundWaitPhase;
/* Shared gate stores 5/7, but dispatch indexes the current command word,
 * not this byte. Redispatch clears it only for whole-word completion;
 * new-box initialization also clears it. */
extern u8 gText_ScriptDispatchGate;

/* Repositions the six corner pairs of the channel's attached object so the
 * highlight sits on the currently selected choice. The entry's layout_selector
 * taken modulo ten selects one of three layouts: 1 is the 0x10-pitch list,
 * 2 and 0 are 0xC-pitch lists offset by two pixels either way. */
void DuelEffect_UpdateObjectLayout(DuelEffectChannel *p);

#ifdef VERSION_EUROPE

void DuelEffect_UpdateGlyphSpiralFade(DuelEffectChannel *record, void *context);
void DuelEffect_StartGlyphSpiralReveal(DuelEffectChannel *record, void *context);
void DuelEffect_StartGlyphSpiralConceal(DuelEffectChannel *record, void *context);
void DuelEffect_UpdateGlyphLinearFade(DuelEffectChannel *record);
void DuelEffect_UpdateGlyphFadeIn(DuelEffectChannel *record);
void DuelEffect_UpdateGlyphFadeOut(DuelEffectChannel *record);
#endif

/* Marks every gDisplayEffect_aMenuRecords menu record inactive (effect_id = -1), clears its
   variant_update_flags, requested_variant, variant_update_counter and child grid, and frees every display-effect
   VRAM slot.
   The returned pointer is a loop leftover that no caller reads. */
s8 *DisplayEffect_ResetMenuRecords(void);

struct DuelEffectChannel;

void Text_NoOpSecondary12(void);
void Text_NoOpSecondary13(void);
void Text_OpenCampaignMap(struct DuelEffectChannel *object);

/* One byte at 0x8009B363, used by four files that share nothing else:
 * three writers and one reader.
 *
 * debug_menu.c clears it, and duel_effect_sound_commands.c and
 * script_scene.c (Script_OpOpenCampaignMap) store the unsigned script operand into it.
 * Main_RunCampaignMap in main_mode_runners.c only reads it and passes
 * it to CampaignMap_SetLocation.
 *
 * It is a single byte: c_symbols.ld has gDuel_bTerrain at 0x8009B364,
 * immediately after it. Declared as the byte it is, it is small data at -G8,
 * which is what gives -msplit-addresses users (script_scene.c,
 * debug_menu.c) retail's one-instruction macro store; the earlier
 * incomplete-array spelling only escaped %gp_rel while the assembler ran at
 * -G8, and forced those two units off the split profile. */
extern u8 gCampaignMap_bEntryLocation;

/* Keep the legacy runtime header and its private regional views unchanged. */
struct DuelEffectChannel;
void TextBox_BuildBlocking(struct DuelEffectChannel *object);
void TextBox_BuildBlockingWithGlyphScratch(struct DuelEffectChannel *object);

/* Unsized original halfword view: RECT prefix and regional glyph scratch. */
extern s16 gText_GlyphUploadScratch[];

/* Draws a duel-effect channel's text glyph, icon or rotating quad from
 * scratchpad packets. Nothing calls it directly: TextBox_RefreshDisplayObjects stores its
 * address in a display object's field_4C, and the object's draw pass
 * supplies both arguments. */
void TextBox_DrawEntries(DisplayObject *obj, GsOT *ot);

struct DuelEffectChannel;

void TextBox_Destroy(struct DuelEffectChannel *record);
void TextBox_DestroyIndex(s32 index);
void *TextBox_Create(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height
);
void *TextBox_CreateFlagged(
    s32 index, s32 string_id, s32 x, s32 y, s32 width, s32 height, s32 flags
);

/* Declared through the tag rather than the typedef, so this header does
   not need duel_effect.h. */
struct DuelEffectChannel;
void TextBox_BuildStep(struct DuelEffectChannel *object);

/* Shared glyph-index publication, not a choice index or per-channel array.
 * BuildStep publishes the script word after control/wrap handling. The frame
 * pump stores zero on active-channel entry and -1 before incomplete-channel
 * pumping and batching resets; only nonnegative values index the glyph table.
 * Keep signed halfword storage and all existing publication/reset order. */
extern s16 gText_PumpedGlyphIndex;
/* Repositions an existing type-four outline around an existing primary object.
 * TextBox_SetPos takes a separate path for other second-object types. */
void TextBox_UpdateOutline(struct DuelEffectChannel *record);
void TextBox_SetPos(struct DuelEffectChannel *record, s32 x, s32 y);

/* Refreshes the display objects a record draws through, and is reached only
 * from TextBox_BuildStep above. It creates the object at +0x28 on first use, then
 * on every call copies the record's four geometry halfwords at 0x3C, 0x40,
 * 0x3E and 0x42 into it and derives two half-extents from the latter pair.
 *
 * When the regional outline bit (NA/Japan 0x20, Europe 0x40) is set it also
 * builds the second object at +0x2C, releasing any previous one, and calls
 * TextBox_UpdateOutline on the same record -- which is why it is declared beside it
 * rather than with the display-object headers whose fields it fills. The
 * Primary allocation installs TextBox_DrawEntries; reusing it does not
 * reinstall the callback. Both allocation paths retain their unchecked
 * success assumption. The record is what this operates on. */
void TextBox_RefreshDisplayObjects(struct DuelEffectChannel *record);

/* Requests a build and pumps the text box until it reports done. The two
   entry points differ only in the flag word they set first. */
void TextBox_BuildBlocking(struct DuelEffectChannel *object);

void TextBox_SetRect(s32, s32, s32, s32, s32);

/* Kind four reuses this byte; other indexed kinds replace it. Shared across
 * channels/frames, with no additional reset or initialization. */
extern u8 gText_PreviousGlyphKind;

/* Pumps all four active channels, coordinates their choice objects and
 * forwards the emitted glyph kind to menu records before entry processing. */
void TextBox_UpdateChannels(void);

/* Advances an object's scroll position by one step and reports whether it has
 * reached the end.
 *
 * When the counter pen_x reaches its limit rect_width the counter resets and
 * the position pen_y moves on by the step held in line_advance_y. If that would
 * carry the position past the bound rect_height the step is taken back off and 1
 * is returned, leaving the position at the last value that still fits.
 *
 * pen_x and pen_y are u16 in the record but this function compares them
 * signed, so those two reads keep an explicit `*(s16 *)&` at the use.
 *
 * The return is a flag, not a count. */
s32 TextBox_WrapLineIfNeeded(DuelEffectChannel *object);

void Text_DisableCompletionWait(DuelEffectChannel *value);

/* gText_SecondaryCommandHandlers entry, and the dispatcher for the whole table: it reads one byte
 * from the object's current stream, advances the cursor past it, and calls
 * gText_SecondaryCommandHandlers[byte] with the same object -- so an entry can chain into another,
 * and the table's bound is a property of the script data rather than of any
 * call site. */
void Text_DispatchSecondaryCommand(DuelEffectChannel *object);

#define TEXT_CHOICE_COMMAND_COUNT_MASK 7
#define TEXT_CHOICE_COMMAND_HAS_OPTIONS 0x08
#define TEXT_CHOICE_COMMAND_BRANCH 0x80
#define TEXT_CHOICE_OPTIONS_ENABLED_MASK 0x0F
#define TEXT_CHOICE_OPTIONS_INPUT_FLAG 0x80

/* Primary FB handler in the NA table. Consumes the optional options byte
 * before either setup or selected-branch handling. Setup without options uses
 * enabled mask 0xF and input state zero. Setup publishes the unchecked
 * three-bit count, high control nibble and low enabled nibble; options bit 7
 * sets input state to 1. Branching skips choice * 2 bytes and calls
 * Text_SetCursorOffset. Both forms raise gText_CommandDisposition. */
void Text_HandleChoiceCommand(DuelEffectChannel *object);

/* Two gText_SecondaryCommandHandlers entries. Text_StartPageWait puts the object into wait state
 * 4 and raises gText_CommandDisposition, which is how a command hands the frame back.
 *
 * Text_HandleCampaignFlagCommand is the campaign-flag command, and like its
 * scene-script
 * counterpart it is both a write and a branch: with
 * CAMPAIGN_FLAG_COMMAND_WRITE it records a card-used flag and returns,
 * otherwise it reads a second word as the target and, when
 * Campaign_TestStoryFlag agrees, rewrites the low half of the object's current
 * stream cursor -- a jump within the same 64K rather than a full pointer
 * store. */
void Text_StartPageWait(DuelEffectChannel *object);
void Text_HandleCampaignFlagCommand(DuelEffectChannel *object);

/* gText_SecondaryCommandHandlers entry: pushes a stream cursor. It takes a word through
 * TextStream_ReadU16LE, writes it as the low half of the NEXT cursor slot -- keeping
 * that slot's high half -- and then increments the object's +0x58 cursor index,
 * so the following commands read from the new stream. */
void Text_PushStreamOffset(DuelEffectChannel *object);

void Text_NewLine(DuelEffectChannel *object);
void Text_EndStream(DuelEffectChannel *object);

/* gText_SecondaryCommandHandlers entry: the effect-script command. It reads a command id and a flag
 * byte from the object's current stream, finds the display effect record for
 * the id in gDisplayEffect_aMenuRecords -- ids below 0x41 live in the first two records, others
 * in the third -- and depending on the flags stops it (bit 7), adjusts a
 * running one (bits 5 and 6), or starts it in the slot bit 0 names, putting the
 * object into the matching wait state.
 *
 * The parameter uses the narrow shared EffectObject view in ygo_types.h. The
 * table uses the wider DuelEffectChannel view, so this entry retains an
 * explicit cast. */

void Text_HandleDisplayEffectCommand(EffectObject *object);

/* gText_SecondaryCommandHandlers entry: arms a campaign duel from the object's script stream. It
 * takes four bytes in order -- opponent id, gDuel_awCampaignSceneByOutcome[0], gDuel_awCampaignSceneByOutcome[1], terrain --
 * then a word through TextStream_ReadU16LE for gDuel_wBgmId, picks the duel's music
 * bank from the opponent id (0x7280 for ids 9 through 16, 0x7290 for 0x11,
 * 0x72B0 for 0x26, 0x72A0 otherwise), and hands the scene to mode 3. A
 * controlled Forest Mage transition observed opponent 23, terrain 1 and
 * gDuel_wBgmId 0x7210 unchanged through the first sixty duel frames.
 *
 * It also resets gDuel_bSide0ControllerSelector[0] to -1 before any of that, which is what makes the
 * duel's own state read as unset. Each byte read uses independent local
 * slot/cursor/value lifetimes; no register bindings are needed. */
void Text_StartCampaignDuel(DuelEffectChannel *object);

/* Reads the next little-endian halfword from one of the byte streams held at
 * the front of an object and advances that stream past it.
 *
 * The stream is chosen by the signed byte at 0x58, which indexes the array of
 * pointers the object begins with, so a caller switches streams by writing
 * that byte rather than by passing a different object.
 *
 * The parameter is the object that owns the streams, not a stream itself. */
int TextStream_ReadU16LE(DuelEffectChannel *object);

/* Reads one little-endian 32-bit word out of a script stream and advances it.
 *
 * The TextStreamOwner carries 22 stream pointers followed by the signed
 * selector at +0x58. The selected pointer is stepped four bytes on, and the
 * word is assembled from those bytes low-first, so the stream is
 * little-endian here, unlike the sequence readers in the sound path.
 *
 * DuelEffectChannel has the same stream-pointer prefix and calls its selector
 * stream_index; the sole caller casts that wider record to this narrow view.
 *
 * Text_PushFormattedDecimal is the only consumer. */
u32 TextStream_ReadU32LE(TextStreamOwner *object);

struct DisplayObject;

/* Drives a widget's pulsing highlight from the frame counter. It folds
 * gMain_dwFrameCounter into a triangle wave, gates each colour channel on the enable
 * bytes at +0x0C, +0x0D and +0x0E, and writes the result into the six colour
 * words at 0x2C, 0x34, 0x3C, 0x44, 0x4C and 0x54 -- the stride-8 run
 * display_object.h documents -- with the four dimmed to a quarter.
 *
 * Dialog_UpdateChoice is the only consumer: it installs this as the
 * object's DisplayObjectCallback, casting it to that u8 * hook type. */
void Widget_UpdatePulseColour(struct DisplayObject *object);

#endif
