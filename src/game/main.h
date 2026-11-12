#ifndef MEMORIES_DECOMP_MAIN_H
#define MEMORIES_DECOMP_MAIN_H
#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "graphics_base_frame_step.h"

extern RECT gGraphics_aImageTransferRects[2];

#define GRAPHICS_PACKET_BUFFER_SIZE 140000

/* The per-frame step multiplier. graphics_frame.c sets it to
 * gGraphics_bExtraFrameStep + 1
 * once a frame and Main_Init (src/game/main.c) seeds it at 1;
 * every other consumer scales a motion delta by it, which is why a dropped
 * frame moves things twice as far.
 *
 * It is one four-byte value, and the three widths seen in the listings come
 * from the use sites rather than the declaration. The resident listings carry
 * eighteen accesses across twelve functions, every one of them at offset
 * zero: thirteen word, three halfword, two byte, with Graphics_SyncFrame in two of
 * those groups. That census is the resident one; the main_menu overlay adds a
 * nineteenth access, a halfword read at offset zero --
 * `lhu $a0, %lo(gGraphics_nBaseFrameStep)($v0)` at
 * 0x80180680 in MainMenu_UpdateFrontendMenu.
 * gcc 2.8.1 picks the narrow load itself -- `(u8)` and `(u16)` on an
 * int global emit lbu and lhu against the same symbol -- so a narrow read is
 * written as a cast at the site, not as a declaration. Every consumer builds
 * byte-identically with the one plain declaration in
 * graphics_base_frame_step.h. */

/* The movie playback state byte, shared by three files.
 *
 * movie_playback_control.h describes what it means: Movie_Play sets bit
 * 0x80 when a movie starts, Movie_UpdatePlayback sets 0x40 once the stream has been
 * asked to stop and clears 0x80 when it has, and Movie_ResetPlaybackState
 * clears the byte. graphics_frame.c reads bit 0x80 in the frame loop, which is
 * why it is declared here -- this is the header all three consumers already
 * include, and main_run_frontend_loop.c does not include movie_playback_control.h at
 * all. */
extern u8 gMovie_bPlaybackState;

/* The frame-advance bound. Graphics_SyncFrame spins
 * `while (gGraphics_dwSyncVBlankCounter < gGraphics_bSyncVBlankTarget)`, so this byte is how many frames the
 * caller lets the sync run: Main_RunAnimatedBattle,
 * src/overlays/main_menu/trade_update.c and src/game/duel_effect_states.c
 * (opening the card viewer) set it to 1, and Main_Init,
 * Main_ResetFrontendRuntime, Main_RunLibraryMenu and
 * src/game/duel_effect_states.c (closing the viewer) set it to 0. Every retail
 * access is a byte store or load.
 *
 * Two units reach it gp-relative (Main_Init stores, Graphics_SyncFrame
 * re-reads it each iteration); every other retail site is a bare store
 * through $at (`lui $at,%hi` / `sb ...,%lo(...)($at)`).
 *
 * Every consumer builds byte-identically with the one volatile declaration.
 * It is load-bearing in src/game/duel_effect_states.c and
 * src/game/library_runtime.c: reorg treats a volatile reference as
 * conflicting with everything, so the delay-slot search of the call after
 * the store stops at the store, and retail leaves those calls' delay slots
 * empty (with a plain byte they take their own `li a0,3` / `li a1,2`). */
extern volatile u8 gGraphics_bSyncVBlankTarget;

/* The normalized extra-step byte Graphics_SyncFrame publishes after its
 * `while (gGraphics_dwSyncVBlankCounter < gGraphics_bSyncVBlankTarget)` wait: it stores the counter into it,
 * stores 1 if that byte is nonzero, and stores the byte plus one into
 * gGraphics_nBaseFrameStep. Main_Init
 * zeroes it in its init block and TextBox_UpdateChannels compares a counter against
 * it (`cnt >= gGraphics_bExtraFrameStep`) on its ordinary yield path.
 * Low-byte wrap and the separate store/read/normalization remain intentional.
 *
 * graphics_frame.c DEFINES it (`u8 gGraphics_bExtraFrameStep;`) rather than
 * declaring it,
 * and that definition stays: its comment there says the assembler only
 * resolves a small global gp-relative when the unit defines it, which is
 * what supplies the load-delay nop before the store, and that the link
 * overrides the common symbol with its retail address through the generated
 * symbol binding. This header's plain declaration sits in front of that
 * definition
 * and does not change it. */
extern u8 gGraphics_bExtraFrameStep;

/* A byte Main_VBlankCB stores 1 into after bumping gMain_dwFrameCounter and
 * gSaveData_dwVBlankCounter and before bumping gGraphics_dwSyncVBlankCounter,
 * and stores 0 into as the last
 * statement of its gMain_bSoundVSyncBusy == 0 block, after SD_VSync has run.
 * Main_Init zeroes it in its init block. No C unit reads it, and neither
 * of the two resident listings that name it loads it. Initial value not
 * read.
 *
 * Both units reach it gp-relative (sb, Main_VBlankCB.s:16/29 and
 * Main_Init.s:33), so this is the plain byte for both. main_init.c used
 * to declare it volatile with the rest of its init block; on this symbol
 * the plain form builds byte-identical (measured by the PR that added
 * this line). */
extern u8 gMain_bVBlankSoundPending;

/* The signed synchronization VBlank counter. Main_VBlankCB increments it
 * (main_frame.c), Graphics_SyncFrame publishes it into
 * gGraphics_bExtraFrameStep and
 * then resets it to -1, and Main_Init zeroes it. It is not a monotonic
 * elapsed-frame total. Input_UpdatePads accumulates deferred masks for a
 * nonzero value and merges/clears them for zero. The compare in
 * Graphics_SyncFrame is `slt` (Graphics_SyncFrame.s:15), so it is signed. */
extern volatile s32 gGraphics_dwSyncVBlankCounter;

/* The frame count: Graphics_SyncFrame increments it after VSync, Main_Init
 * zeroes it, and Text_WaitWithViewportShake and DuelResult_UpdateOrbitSprite test its bit
 * 0. The free_duel overlay's screen_runtime.c reads its low seven bits.
 * Retail reaches it gp-relative in Main_Init and graphics_frame.c and through
 * a lui/lw pair in the other two. volatile is measured: without it Main_Init's zeroing store sinks below the
 * volatile gGraphics_dwSyncVBlankCounter store
 * beside it (mismatch at 0x80012BAC). Sign is not visible in any use
 * (& 1, & 0x7F, ++, = 0), so s32 follows gGraphics_dwSyncVBlankCounter above and is not
 * established. */
extern volatile s32 gGraphics_dwSyncedFrameCount;

/* A frame counter ticked by Main_VBlankCB in the same block as gGraphics_dwSyncVBlankCounter.
 * Main_Init zeroes it and reads it into a local it stores to
 * gSaveData_dwVBlankCounter at
 * the end of its block; BuildDeck_Update and Widget_UpdatePulseColour fold its
 * low six and seven bits into a triangle wave for a pulsing colour; the
 * password overlay's NameEntry_Main shifts it left by eight and ors it
 * above a name checksum into the save block's stamped word; Library_DrawCardGrid
 * masks its low seven bits for the Library cursor's colour ramp. Sign is not visible in any use (& 0x3F,
 * & 0x7F, << 8, ++, = 0), so s32 follows gGraphics_dwSyncVBlankCounter and is not established. */
extern volatile s32 gMain_dwFrameCounter;

/* The active VBlank counter persisted in the save block. Main_VBlankCB ticks it beside
 * gMain_dwFrameCounter; Main_Init stores into it, at the end of its block, the value
 * it read from gMain_dwFrameCounter (notes/research/matching-evidence.md:488-490 is
 * why that copy goes through a local); SaveData_BuildPayload copies it into
 * the payload word at SAVE_DATA_VBLANK_COUNTER_OFFSET and
 * SaveData_ApplyRuntimeState restores it from state->vblank_counter, a u32
 * (save_data.h:65). Sign is not visible in any use (++, a store of that
 * local, a u32 field in and out), so s32 follows gMain_dwFrameCounter and gGraphics_dwSyncVBlankCounter
 * and is not established. */
extern volatile s32 gSaveData_dwVBlankCounter;

/* A flags halfword. Graphics_SyncFrame skips DrawSync(0) when bit 0x8000
 * is set; Graphics_BeginFrame calls PutDispEnv only when bit 0x2000 is set;
 * Graphics_AdjustScreenOffset ors 0x2000 in for its screen-offset adjustment loop and
 * clears it with & 0xDFFF when Start is pressed; Main_Init stores 0x5000;
 * Main_RunBootSequence zeroes it twice. Nothing in C reads 0x4000 or 0x1000.
 * Initial value not read. u16 follows the definition in graphics_frame.c
 * and every retail load, which is lhu; Main_Init and
 * main_run_boot_sequence.c
 * only store to it.
 *
 * src/game/main.c defines it and reaches it gp-relative (the plain form).
 * main_init.c used to declare it volatile with the rest of its init block; on
 * this symbol the plain form builds byte-identical (measured by the PR that
 * added this block). */
extern u16 gGraphics_wFlags;

/* Graphics_BeginFrame publishes the active buffer index in this byte.
 * GPU readback and Script_OpShowImage reach it absolutely rather than
 * through the small-data base. */
extern u8 gGraphics_bActiveBuffer;

/* libgs's environments passed to PutDrawEnv / PutDispEnv. Startup writes the
 * draw environment through a volatile view at the use. See
 * notes/graphics-frame-environments.md for the address/field evidence. */
extern DRAWENV GsDRAWENV;

extern DISPENV GsDISPENV;

typedef char GraphicsDrawEnvSize[sizeof(DRAWENV) == 0x5C ? 1 : -1];
typedef char GraphicsDrawEnvDitherOffset[
    (u32)&((DRAWENV *)0)->dtd == 0x16 ? 1 : -1];
typedef char GraphicsDrawEnvClearOffset[
    (u32)&((DRAWENV *)0)->isbg == 0x18 ? 1 : -1];
typedef char GraphicsDrawEnvRedOffset[
    (u32)&((DRAWENV *)0)->r0 == 0x19 ? 1 : -1];
typedef char GraphicsDrawEnvGreenOffset[
    (u32)&((DRAWENV *)0)->g0 == 0x1A ? 1 : -1];
typedef char GraphicsDrawEnvBlueOffset[
    (u32)&((DRAWENV *)0)->b0 == 0x1B ? 1 : -1];
typedef char GraphicsDispEnvSize[sizeof(DISPENV) == 0x14 ? 1 : -1];

/* Two scratch rectangles for the VRAM transfers. Every user fills x, y, w, h
 * and hands the address to LoadImage2, StoreImage2 or MoveImage in the same
 * block, so there is no producer to own them; Campaign_LoadScenePackage and
 * FreeDuel_Init use the second one, at +8, for the CLUT / second strip.
 *
 * [2] is the extent that is WRITTEN, not the object's size: the next symbol,
 * gGraphics_apOrderingTables, sits at +0x20, and nothing in either tree touches the sixteen
 * bytes between. Four units used to declare this `u8 [100]` -- DuelCard_CaptureRoundedTexture.c
 * said why: "Spelt as an oversized array so -G8 keeps the eight-byte
 * rectangle out of small data; the retail code addresses it with a %hi/%lo
 * pair". Sixteen bytes clear -G8 on their own, so the real shape does the
 * same job. Retail reaches it with a %hi/%lo pair everywhere, never
 * gp-relative. */
/* Declared once, as gGraphics_aImageTransferRects, in main.h. */

/* The tint colour, three consecutive bytes with the components in address
 * order blue, green, red. Movie_OpenStream and Movie_StopStream pass them straight
 * to ClearImage(RECT *, u8 r, u8 g, u8 b) as r = gGraphics_bClearRed, g = gGraphics_bClearGreen,
 * b = gGraphics_bClearBlue, which is what fixes the roles; graphics_frame.c copies the
 * same three into DRAWENV.r0/g0/b0 at 0x19/0x1A/0x1B.
 *
 * Fade_Update defines the three rather than declaring them, and its
 * functions.csv row says why: as definitions the assembler resolves them
 * gp-relative and supplies the three load-delay nops in the tint copy.  That
 * function's translation unit includes this header, so it builds
 * byte-identical with the declaration below visible ahead of its definition,
 * which is the only claim made here about the two. */
extern u8 gGraphics_bClearBlue;
extern u8 gGraphics_bClearGreen;
extern u8 gGraphics_bClearRed;

/* Viewport scroll offset in pixels, signed: Library_UpdateGridScroll subtracts it from a
   sprite position and DisplayObject_RenderSpriteList subtracts it from a primitive's, and both
   results have to be able to go negative. */
extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;

/* One half of the double-buffered graphics work area. The first 0x5110 bytes
 * hold the ordering-table tags; four SDK descriptors occupy the final 0x50
 * bytes. Main_InitHardware initializes their tag bases to offsets 0, 0x10, 0x110
 * and 0x4110, and Graphics_BeginFrame publishes the descriptor addresses.
 * The complete layout lives in graphics_frame_buffer.h; this header keeps a
 * forward declaration so unrelated frame-state consumers do not inherit the
 * full LIBGS interface.
 */
typedef struct GraphicsFrameBuffer GraphicsFrameBuffer;

/* Graphics_BeginFrame picks the half for the frame it is starting and
 * publishes it:
 *
 *     gGraphics_pActiveFrameBuffer =
 *         &gGraphics_aFrameBuffers[gGraphics_bActiveBuffer];
 *
 * The asserted 0x5160 size is the 20832-byte stride selected by the same index
 * that chooses gGraphics_abPacketBuffers's half a few lines above it. Main_Init takes the
 * array base while it brings the loader block up. The number of buffers stays
 * unsized because only the two-buffer startup loop establishes it.
 */
extern GraphicsFrameBuffer gGraphics_aFrameBuffers[];

extern GraphicsFrameBuffer *gGraphics_pActiveFrameBuffer;

/* The other half of that pair, the one the comment above refers to:
 *
 *     arg = &gGraphics_abPacketBuffers[
 *         gGraphics_bActiveBuffer * GRAPHICS_PACKET_BUFFER_SIZE
 *     ];
 *
 * Model_HasInsufficientBufferSpace says what the region is. It takes the
 * allocation pointer gGraphics_pPacketWorkCursor, subtracts this base to get the bytes used,
 * subtracts the current half
 * (`gGraphics_bPacketBufferIndex * GRAPHICS_PACKET_BUFFER_SIZE`, gGraphics_bPacketBufferIndex being the copy of
 * the buffer index Graphics_BeginFrame stores next to it), and measures the
 * remainder against the same capacity.
 *
 * The stride and capacity are both `GRAPHICS_PACKET_BUFFER_SIZE`, so this is
 * one 140000-byte buffer per half and the model data is bump-allocated inside
 * the active one.
 *
 * Unsized for the same reason as its neighbour: the stride is measured, the
 * number of halves is not. */
extern u8 gGraphics_abPacketBuffers[];

extern DISPENV gGraphics_DispEnv;

/* Three flags Main_Init sets in its init block: gGraphics_bDither and gGraphics_bClearBackground to
 * 1, gGraphics_bSkipDisplaySwap to 0. graphics_frame.c DEFINES all three plain, for the same
 * reason it defines gGraphics_bExtraFrameStep: the assembler resolves a small
 * global
 * gp-relative only when the unit defines it.
 *
 * main_services.c reached them through its own `extern volatile u8`
 * declarations, under a comment saying the init block is volatile so the
 * emitted order is the source order. That holds for the rest of that run but
 * not for these three: dropping the volatile and taking these declarations
 * instead builds byte-identical, measured by the PR adding this block. The
 * same was already true of gGraphics_bExtraFrameStep above, whose volatile view in
 * main_init.c was dropped the same way.
 *
 * Their neighbour gGraphics_abOrderingTableLengths holds the four ordering-table
 * lengths. Main_InitHardware writes all four through small data in program
 * order (volatile); the two menu runners write element 3 through the absolute
 * %hi/%lo form. Every consumer builds byte-identically with the sized
 * declaration. */
extern u8 gGraphics_bDither;
extern u8 gGraphics_bClearBackground;
extern u8 gGraphics_bSkipDisplaySwap;

extern u8 gGraphics_abOrderingTableLengths[4];

void Graphics_SyncFrame(void);
void Graphics_BeginFrame(void);

extern s32 D_8009B0A4;
extern s32 D_8009B0B0;
extern s32 D_8009B0BC;
extern s32 D_8009B0D4;

/* The VSync callback installed by Main_Init: ticks the three frame counters,
   reads the raw pads, and runs the sound driver's per-frame work under a
   reentrancy flag. */
void Main_VBlankCB(void);

/* Advance one frame: run the pending frame work, wait for the previous frame
   to finish drawing, start the next one, and fold the raw pads into the
   held/pressed/repeat state. It is the only frame-advance primitive, and
   every one of its call sites is a loop body -- the busy-waits spin on their
   own condition, Main_Loop is the frontend loop itself, and the counted ones
   count passes through this. */
void Main_AdvanceFrame(void);

/* Advance `count` frames through Main_AdvanceFrame. The count is not
   checked, so zero means 2^32. */
void Main_AdvanceFrames(s32 count);

extern u8 gMain_bSoundVSyncBusy;
#endif
