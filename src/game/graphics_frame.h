#ifndef YUGIOH_GAME_GRAPHICS_FRAME_H
#define YUGIOH_GAME_GRAPHICS_FRAME_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

#define GRAPHICS_PACKET_BUFFER_SIZE 140000

/* The per-frame step multiplier. graphics_frame.c sets it to
 * gGraphics_bExtraFrameStep + 1
 * once a frame and Main_Init (src/game/main_init.c) seeds it at 1;
 * every other consumer scales a motion delta by it, which is why a dropped
 * frame moves things twice as far.
 *
 * It is one four-byte value, and the three widths seen in the listings come
 * from the use sites rather than the declaration. The resident listings carry
 * eighteen accesses across twelve functions, every one of them at offset
 * zero: thirteen word, three halfword, two byte, with func_80012DB4 in two of
 * those groups. That census is the resident one; the main_menu overlay adds a
 * nineteenth access, a halfword read at offset zero --
 * `lhu $a0, %lo(D_8009B0D8)($v0)` at
 * 0x80180680 in MainMenu_UpdateFrontendMenu.
 * gcc 2.8.1 picks the narrow load itself -- `(u8)` and `(u16)` on an
 * int global emit lbu and lhu against the same symbol -- so a narrow read is
 * written as a cast at the site, not as a declaration.
 *
 * The two arms left select addressing and ordering, which a cast cannot:
 *
 *   _IS_VOLATILE -- Main_Init seeds it and must not have the store folded;
 *                   display_object_fade_callbacks.c takes it for the whole
 *                   unit, and its three plain reads build the same under it
 *   _IN_DATA     -- out of small data at the compiler */

/* The movie playback state byte, shared by three files that disagree about
 * how to reach it.
 *
 * movie_playback_control.h describes what it means: Movie_Play sets bit
 * 0x80 when a movie starts, Movie_UpdatePlayback sets 0x40 once the stream has been
 * asked to stop and clears 0x80 when it has, and Movie_ResetPlaybackState
 * clears the byte. graphics_frame.c reads bit 0x80 in the frame loop, which is
 * why it is declared here -- this is the header all three consumers already
 * include, and main_run_frontend_loop.c does not include movie_playback_control.h at
 * all.
 *
 * Two arms, for the same reason D_8009B0D8 below has them. Every consumer is
 * in the -G8 family, where a byte-sized global is reached %gp_rel by default.
 * main_run_frontend_loop.c and graphics_frame.c both carry section(".data") to escape
 * that; movie_playback_control.c takes the plain spelling and wants the
 * gp-relative form. Each arm reproduces exactly what that file already
 * wrote. */
#ifdef D_8009B318_IN_DATA
extern u8 gMovie_bPlaybackState __attribute__((section(".data")));
#else
extern u8 gMovie_bPlaybackState;
#endif

#ifdef D_8009B0D8_IN_DATA
extern s32 D_8009B0D8 __attribute__((section(".data")));
#elif defined(D_8009B0D8_IS_VOLATILE)
extern volatile s32 D_8009B0D8;
#else
extern s32 D_8009B0D8;
#endif

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
 * through $at (`lui $at,%hi` / `sb ...,%lo(...)($at)`). The arms follow
 * the unit, and each is justified by a control build of that unit on the
 * plain arm (the PR that added this block records the five results):
 *
 *   _IS_VOLATILE -- graphics_frame.c and src/game/main_init.c
 *   _IN_DATA     -- main_run_animated_battle.c and
 *                   main_run_duel_and_library.c and main_mode_runners.c,
 *                   all at -G8: out of small
 *                   data at the compiler, with its true width
 *   _IN_DATA_VOLATILE -- src/game/duel_effect_states.c: the same .data view,
 *                   volatile. reorg treats a volatile reference as
 *                   conflicting with everything, so the delay-slot search
 *                   of the call after the store stops at the store; with
 *                   the plain byte that call takes its own `li a0,3` and
 *                   the unit differs from retail in two instructions.
 *                   src/game/library_runtime.c takes it for Library_UpdateCardView,
 *                   whose close path clears the byte right before
 *                   DisplayObject_SetResourceVariant: retail leaves that
 *                   call's delay slot empty, and the plain .data byte lets
 *                   the call take its own `li a1,2`
 *
 * main_reset_frontend_runtime.c (-G0) and the main_menu overlay take the
 * plain byte. */
#ifdef D_8009B0C0_IN_DATA_VOLATILE
extern volatile u8 gGraphics_bSyncVBlankTarget __attribute__((section(".data")));
#elif defined(D_8009B0C0_IN_DATA)
extern u8 gGraphics_bSyncVBlankTarget __attribute__((section(".data")));
#elif defined(D_8009B0C0_IS_VOLATILE)
extern volatile u8 gGraphics_bSyncVBlankTarget;
#else
extern u8 gGraphics_bSyncVBlankTarget;
#endif

/* The normalized extra-step byte Graphics_SyncFrame publishes after its
 * `while (gGraphics_dwSyncVBlankCounter < gGraphics_bSyncVBlankTarget)` wait: it stores the counter into it,
 * stores 1 if that byte is nonzero, and stores the byte plus one into
 * D_8009B0D8. Main_Init
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
 * and does not change it.
 *
 * Graphics_SyncFrame and Main_Init reach it gp-relative (sb/lbu,
 * func_80012DB4.s:20/24/26 and func_80012B50.s:34), so they take the plain
 * byte; TextBox_UpdateChannels reaches it through %hi/%lo (func_80039794.s:50-51)
 * and defines the .data arm. main_init.c used to declare it volatile; the
 * plain form builds byte-identical (measured by the PR that added this
 * block). */
#ifdef D_8009B0C1_IN_DATA
extern u8 gGraphics_bExtraFrameStep __attribute__((section(".data")));
#else
extern u8 gGraphics_bExtraFrameStep;
#endif

/* A byte Main_VBlankCB stores 1 into after bumping gMain_dwFrameCounter and
 * gSaveData_dwVBlankCounter and before bumping gGraphics_dwSyncVBlankCounter,
 * and stores 0 into as the last
 * statement of its gMain_bSoundVSyncBusy == 0 block, after SD_VSync has run.
 * Main_Init zeroes it in its init block. No C unit reads it, and neither
 * of the two resident listings that name it loads it. Initial value not
 * read.
 *
 * Both units reach it gp-relative (sb, func_80012CD4.s:16/29 and
 * func_80012B50.s:33), so this is the plain byte for both. main_init.c used
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
 * Graphics_SyncFrame is `slt` (func_80012DB4.s:15), so it is signed.
 *
 * Three units reach it gp-relative and take the volatile form below;
 * Input_UpdatePads reads it through a %hi/%lo pair into the load's own
 * register (func_8003CCD8.s:50-51), the bare form, and takes _IN_DATA --
 * out of small data at the compiler with its true width. The unsized
 * `u32 []` it used to declare reached the same form ("Retail
 * rematerializes this address inside the repeat loop", its comment said),
 * and the .data scalar builds byte-identical there. Both arms are
 * justified by a control build recorded in the PR that added this
 * block. */
#ifdef D_8009B0C8_IN_DATA
extern s32 gGraphics_dwSyncVBlankCounter
#ifdef VERSION_JAPAN
    asm("gJapanese_GraphicsSyncVBlankCounter")
#endif
    __attribute__((section(".data")));
#else
extern volatile s32 gGraphics_dwSyncVBlankCounter
#ifdef VERSION_JAPAN
    asm("gJapanese_GraphicsSyncVBlankCounter")
#endif
    ;
#endif

/* The frame count: Graphics_SyncFrame increments it after VSync, Main_Init
 * zeroes it, and Text_WaitWithViewportShake and DuelResult_UpdateOrbitSprite test its bit
 * 0. The free_duel overlay's screen_runtime.c reads its low seven bits.
 * Retail reaches it gp-relative in Main_Init and graphics_frame.c and through
 * a lui/lw pair in the other two, which is the .data arm.
 * volatile is measured: without it Main_Init's zeroing store sinks below the
 * volatile gGraphics_dwSyncVBlankCounter store
 * beside it (mismatch at 0x80012BAC). Sign is not visible in any use
 * (& 1, & 0x7F, ++, = 0), so s32 follows gGraphics_dwSyncVBlankCounter above and is not
 * established. */
#ifdef D_8009B0CC_IN_DATA
extern s32 gGraphics_dwSyncedFrameCount __attribute__((section(".data")));
#else
extern volatile s32 gGraphics_dwSyncedFrameCount;
#endif

/* A frame counter ticked by Main_VBlankCB in the same block as gGraphics_dwSyncVBlankCounter.
 * Main_Init zeroes it and reads it into a local it stores to
 * gSaveData_dwVBlankCounter at
 * the end of its block; BuildDeck_Update and Widget_UpdatePulseColour fold its
 * low six and seven bits into a triangle wave for a pulsing colour; the
 * password overlay's NameEntry_Main shifts it left by eight and ors it
 * above a name checksum into the save block's stamped word; Library_DrawCardGrid
 * masks its low seven bits for the Library cursor's colour ramp. Sign is not visible in any use (& 0x3F,
 * & 0x7F, << 8, ++, = 0), so s32 follows gGraphics_dwSyncVBlankCounter and is not established.
 *
 * main_frame.c and src/game/main_init.c reach it gp-relative and
 * take the volatile form below; every other retail site is a lui/lw pair.
 * volatile is measured (notes/research/matching-evidence.md:479-490):
 * Main_Init zeroes it and immediately re-reads it, and without volatile GCC
 * forwards the stored zero and the function is one instruction short.
 * build_deck_update.c and library_card_grid.c (-G8) define the .data arm;
 * widget_update_pulse_colour.c (-G0) and the password overlay's
 * name_entry_main.c take the plain form. */
#ifdef D_8009B09C_IN_DATA
extern s32 gMain_dwFrameCounter __attribute__((section(".data")));
#else
extern volatile s32 gMain_dwFrameCounter;
#endif

/* The active VBlank counter persisted in the save block. Main_VBlankCB ticks it beside
 * gMain_dwFrameCounter; Main_Init stores into it, at the end of its block, the value
 * it read from gMain_dwFrameCounter (notes/research/matching-evidence.md:488-490 is
 * why that copy goes through a local); SaveData_BuildPayload copies it into
 * the payload word at SAVE_DATA_VBLANK_COUNTER_OFFSET and
 * SaveData_ApplyRuntimeState restores it from state->vblank_counter, a u32
 * (save_data.h:65). Sign is not visible in any use (++, a store of that
 * local, a u32 field in and out), so s32 follows gMain_dwFrameCounter and gGraphics_dwSyncVBlankCounter
 * and is not established.
 *
 * main_frame.c and src/game/main_init.c reach it gp-relative and
 * take the volatile form below; SaveData_ApplyRuntimeState stores through
 * $at (lui/sw) and SaveData_BuildPayload loads through a lui/lw pair, each
 * while reaching another symbol through $gp, so save_data_payload.c, which
 * holds both, defines the .data arm. */
#ifdef D_8009B0C4_IN_DATA
extern s32 gSaveData_dwVBlankCounter
#ifdef VERSION_JAPAN
    asm("gJapanese_SaveDataVBlankCounter")
#endif
    __attribute__((section(".data")));
#else
extern volatile s32 gSaveData_dwVBlankCounter
#ifdef VERSION_JAPAN
    asm("gJapanese_SaveDataVBlankCounter")
#endif
    ;
#endif

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
 * graphics_frame.c defines it (gp-relative in the target);
 * src/game/main_init.c and src/game/main_services.c reach it
 * gp-relative and take the plain form.
 * main_init.c used to declare it volatile with the rest of its init block;
 * on this symbol the plain form builds byte-identical (measured by the PR
 * that added this block). Main_RunBootSequence stores through $at (lui/sh) in
 * a unit that reaches one other symbol through $gp, so
 * main_run_boot_sequence.c
 * defines the .data arm. */
#ifdef D_8009B098_IN_DATA
extern u16 gGraphics_wFlags __attribute__((section(".data")));
#else
extern u16 gGraphics_wFlags;
#endif

extern DISPENV gGraphics_DispEnv;

/* Graphics_BeginFrame publishes the active buffer index in this byte.
 * GPU readback and Script_OpShowImage reach it absolutely rather than
 * through the small-data base. */
#ifdef GRAPHICS_ACTIVE_BUFFER_IN_DATA
extern u8 gGraphics_bActiveBuffer __attribute__((section(".data")));
#else
extern u8 gGraphics_bActiveBuffer;
#endif

/* SDK environment addresses passed to PutDrawEnv / PutDispEnv. Keep the
 * draw environment unsized, and retain startup's volatile byte stores.
 * See notes/graphics-frame-environments.md for the address/field evidence. */
#ifdef GRAPHICS_DRAW_ENV_IS_VOLATILE
extern volatile DRAWENV D_800FE048[];
#else
extern DRAWENV D_800FE048[];
#endif
/* Preserve both array/qualifier arms without conflicting with GsDRAWENV. */
#define gLibGs_DrawEnvView D_800FE048

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
 * bytes between. Four units used to declare this `u8 [100]` -- func_8001944C.c
 * said why: "Spelt as an oversized array so -G8 keeps the eight-byte
 * rectangle out of small data; the retail code addresses it with a %hi/%lo
 * pair". Sixteen bytes clear -G8 on their own, so the real shape does the
 * same job. Retail reaches it with a %hi/%lo pair everywhere, never
 * gp-relative. */
extern RECT D_800E9D70[2];

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
 * which is the only claim made here about the two.
 *
 *   _IN_DATA_VOLATILE -- startup's ordered, absolute-address byte stores
 *   _IN_DATA          -- out of small data at the compiler
 *   _IS_AGGREGATE     -- unsized array, read as [0]
 */
#ifdef D_8009B142_IN_DATA_VOLATILE
extern volatile u8 gGraphics_bClearBlue __attribute__((section(".data")));
extern volatile u8 gGraphics_bClearGreen __attribute__((section(".data")));
extern volatile u8 gGraphics_bClearRed __attribute__((section(".data")));
#elif defined(D_8009B142_IN_DATA)
extern u8 gGraphics_bClearBlue __attribute__((section(".data")));
extern u8 gGraphics_bClearGreen __attribute__((section(".data")));
extern u8 gGraphics_bClearRed __attribute__((section(".data")));
#elif defined(D_8009B142_IS_AGGREGATE)
extern u8 gGraphics_bClearBlue[];
extern u8 gGraphics_bClearGreen[];
extern u8 gGraphics_bClearRed[];
#else
extern u8 gGraphics_bClearBlue;
extern u8 gGraphics_bClearGreen;
extern u8 gGraphics_bClearRed;
#endif

/* Viewport scroll offset in pixels, signed: Library_UpdateGridScroll subtracts it from a
   sprite position and DisplayObject_RenderSpriteList subtracts it from a primitive's, and both
   results have to be able to go negative.

   Files that reach these through `__attribute__((section(".data")))`, or as an
   unsized or [4] array, select the guarded view below because those spellings
   change how the address is materialised, not just how the value reads.
   The unsigned [4] view has its own C names, so a unit may select it
   together with the signed .data view (script_image_commands.c does). */
#ifdef GGRAPHICS_VIEWPORT_SIZED_UNSIGNED_IN_DATA
extern u16 gGraphics_uViewportX[4] asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern u16 gGraphics_uViewportY[4] asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#endif
#if defined(GRAPHICS_VIEWPORT_IN_DATA)
extern s16 gGraphics_sViewportX __attribute__((section(".data")));
extern s16 gGraphics_sViewportY __attribute__((section(".data")));
#elif !defined(GGRAPHICS_VIEWPORT_SIZED_UNSIGNED_IN_DATA)
extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;
#endif

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

#ifdef GRAPHICS_ACTIVE_FRAME_BUFFER_IS_VOLATILE
extern GraphicsFrameBuffer *G32 volatile gGraphics_pActiveFrameBuffer;
#else
extern GraphicsFrameBuffer *G32 gGraphics_pActiveFrameBuffer;
#endif

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

/* Three flags Main_Init sets in its init block: gGraphics_bDither and gGraphics_bClearBackground to
 * 1, D_8009B0A8 to 0. graphics_frame.c DEFINES all three plain, for the same
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
 * Their neighbour gGraphics_abOrderingTableLengths begins a four-byte state block. The defining
 * frame unit addresses the block as an array, while Main_InitHardware writes the
 * first three bytes as ordered volatile scalars. The selector keeps both
 * measured views at the owner instead of leaving private declarations in the
 * startup source. */
extern u8 gGraphics_bDither;
extern u8 gGraphics_bClearBackground
#ifdef VERSION_JAPAN
    asm("gJapanese_D_8009B0D0")
#endif
    ;
extern u8 D_8009B0A8;

#ifdef GRAPHICS_INIT_STATE_IS_VOLATILE_SCALAR
extern volatile u8 gGraphics_abOrderingTableLengths;
extern volatile u8 gGraphics_bOrderingTable1Length;
extern volatile u8 gGraphics_bOrderingTable2Length;
#else
extern u8 gGraphics_abOrderingTableLengths[4];
#endif

void Graphics_SyncFrame(void);
void Graphics_BeginFrame(void);

#endif
