#ifndef YUGIOH_GAME_GRAPHICS_FRAME_H
#define YUGIOH_GAME_GRAPHICS_FRAME_H

#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

/* The per-frame step multiplier. graphics_frame.c sets it to D_8009B0C1 + 1
 * once a frame and Main_Init (src/candidates/func_80012B50.c) seeds it at 1;
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
 * src/candidates_target/main_menu/func_80180390.S:217.
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
 * movie_playback_control.h describes what it means: func_8003594C sets bit
 * 0x80 when a movie starts, func_800359B0 sets 0x40 once the stream has been
 * asked to stop and clears 0x80 when it has, and func_80035A58 clears the
 * byte. graphics_frame.c reads bit 0x80 in the frame loop, which is why it is
 * declared here -- this is the header all three consumers already include,
 * and func_80043BCC.c does not include movie_playback_control.h at all.
 *
 * Two arms, for the same reason D_8009B0D8 below has them. Every consumer is
 * in the -G8 family, where a byte-sized global is reached %gp_rel by default.
 * func_80043BCC.c and graphics_frame.c both carry section(".data") to escape
 * that; movie_playback_control.c takes the plain spelling and wants the
 * gp-relative form. Each arm reproduces exactly what that file already
 * wrote. */
#ifdef D_8009B318_IN_DATA
extern u8 D_8009B318 __attribute__((section(".data")));
#else
extern u8 D_8009B318;
#endif

#ifdef D_8009B0D8_IN_DATA
extern s32 D_8009B0D8 __attribute__((section(".data")));
#elif defined(D_8009B0D8_IS_VOLATILE)
extern volatile s32 D_8009B0D8;
#else
extern s32 D_8009B0D8;
#endif

/* The frame-advance bound. Graphics_SyncFrame spins
 * `while (D_8009B0C8 < D_8009B0C0)`, so this byte is how many frames the
 * caller lets the sync run: Main_RunAnimatedBattle and
 * src/overlays/main_menu/trade_update.c sets it to 1,
 * src/candidates/func_800283F4.c:77 sets it to `flags - 2`, and Main_Init,
 * Main_ResetFrontendRuntime, Main_RunLibraryMenu and
 * src/candidates/func_800283F4.c:207 set it to 0. Every retail access is a
 * byte store or load.
 *
 * Two units reach it gp-relative (Main_Init stores, Graphics_SyncFrame
 * re-reads it each iteration); every other retail site is a bare store
 * through $at (`lui $at,%hi` / `sb ...,%lo(...)($at)`). The arms follow
 * the unit, and each is justified by a control build of that unit on the
 * plain arm (the PR that added this block records the five results):
 *
 *   _IS_VOLATILE -- graphics_frame.c and src/candidates/func_80012B50.c
 *   _IN_DATA     -- src/candidates/func_800283F4.c,
 *                   main_run_animated_battle.c and
 *                   main_run_duel_and_library.c and main_run_credits.c,
 *                   all at -G8: out of small
 *                   data at the compiler, with its true width
 *
 * main_reset_frontend_runtime.c (-G0) and the main_menu overlay take the
 * plain byte. */
#ifdef D_8009B0C0_IN_DATA
extern u8 D_8009B0C0 __attribute__((section(".data")));
#elif defined(D_8009B0C0_IS_VOLATILE)
extern volatile u8 D_8009B0C0;
#else
extern u8 D_8009B0C0;
#endif

/* The byte Graphics_SyncFrame publishes after its `while (D_8009B0C8 <
 * D_8009B0C0)` wait: it stores D_8009B0C8 into it, stores 1 if that byte is
 * nonzero, and stores the byte plus one into D_8009B0D8. Main_Init
 * zeroes it in its init block and func_80039794 compares a counter against
 * it (`cnt >= D_8009B0C1`). No other C unit touches it.
 *
 * graphics_frame.c DEFINES it (`u8 D_8009B0C1;`) rather than declaring it,
 * and that definition stays: its comment there says the assembler only
 * resolves a small global gp-relative when the unit defines it, which is
 * what supplies the load-delay nop before the store, and that the link
 * overrides the common symbol with the retail address (which splat's
 * generated undefined_syms_auto.txt supplies; c_symbols.ld does not list
 * it). This header's plain declaration sits in front of that definition
 * and does not change it.
 *
 * Graphics_SyncFrame and Main_Init reach it gp-relative (sb/lbu,
 * func_80012DB4.s:20/24/26 and func_80012B50.s:34), so they take the plain
 * byte; func_80039794 reaches it through %hi/%lo (func_80039794.s:50-51)
 * and defines the .data arm. main_init.c used to declare it volatile; the
 * plain form builds byte-identical (measured by the PR that added this
 * block). */
#ifdef D_8009B0C1_IN_DATA
extern u8 D_8009B0C1 __attribute__((section(".data")));
#else
extern u8 D_8009B0C1;
#endif

/* A byte Main_VBlankCB stores 1 into after bumping D_8009B09C and
 * D_8009B0C4 and before bumping D_8009B0C8, and stores 0 into as the last
 * statement of its D_8009AF0C == 0 block, after func_80047050 has run.
 * Main_Init zeroes it in its init block. No C unit reads it, and neither
 * of the two resident listings that name it loads it. Initial value not
 * read.
 *
 * Both units reach it gp-relative (sb, func_80012CD4.s:16/29 and
 * func_80012B50.s:33), so this is the plain byte for both. main_init.c used
 * to declare it volatile with the rest of its init block; on this symbol
 * the plain form builds byte-identical (measured by the PR that added
 * this line). */
extern u8 D_8009B0C3;

/* The frame counter that comparison reads. Main_VBlankCB increments it
 * (main_frame.c), Graphics_SyncFrame publishes it into D_8009B0C1 and
 * then resets it to -1, Main_Init zeroes it, and Input_UpdatePads tests
 * it non-zero before folding the deferred pad bits in. The compare in
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
extern s32 D_8009B0C8 __attribute__((section(".data")));
#else
extern volatile s32 D_8009B0C8;
#endif

/* The frame count: Graphics_SyncFrame increments it after VSync, Main_Init
 * zeroes it, func_80037A58 and func_80020D4C test its bit 0 and the free_duel
 * overlay's screen_runtime.c reads its low seven bits. Retail reaches it
 * gp-relative in Main_Init and graphics_frame.c and through a lui/lw pair
 * in the other two, which is the .data arm. volatile is measured: without
 * it Main_Init's zeroing store sinks below the volatile D_8009B0C8 store
 * beside it (mismatch at 0x80012BAC). Sign is not visible in any use
 * (& 1, & 0x7F, ++, = 0), so s32 follows D_8009B0C8 above and is not
 * established. */
#ifdef D_8009B0CC_IN_DATA
extern s32 D_8009B0CC __attribute__((section(".data")));
#else
extern volatile s32 D_8009B0CC;
#endif

/* A frame counter ticked by Main_VBlankCB in the same block as D_8009B0C8.
 * Main_Init zeroes it and reads it into a local it stores to D_8009B0C4 at
 * the end of its block; func_80033BE8 and Widget_UpdatePulseColour fold its
 * low six and seven bits into a triangle wave for a pulsing colour; the
 * password overlay's NameEntry_Main shifts it left by eight and ors it
 * above a name checksum into the save block's stamped word; func_80029EC4
 * (still assembly) reads it too. Sign is not visible in any use (& 0x3F,
 * & 0x7F, << 8, ++, = 0), so s32 follows D_8009B0C8 and is not established.
 *
 * main_frame.c and src/candidates/func_80012B50.c reach it gp-relative and
 * take the volatile form below; every other retail site is a lui/lw pair.
 * volatile is measured (notes/research/matching-evidence.md:479-490):
 * Main_Init zeroes it and immediately re-reads it, and without volatile GCC
 * forwards the stored zero and the function is one instruction short.
 * func_800339D0.c (-G8) defines the .data arm;
 * widget_update_pulse_colour.c (-G0) and the password overlay's
 * name_entry_main.c take the plain form. */
#ifdef D_8009B09C_IN_DATA
extern s32 D_8009B09C __attribute__((section(".data")));
#else
extern volatile s32 D_8009B09C;
#endif

/* The VBlank counter the save block carries. Main_VBlankCB ticks it beside
 * D_8009B09C; Main_Init stores into it, at the end of its block, the value
 * it read from D_8009B09C (notes/research/matching-evidence.md:488-490 is
 * why that copy goes through a local); SaveData_BuildPayload copies it into
 * the payload word at SAVE_DATA_VBLANK_COUNTER_OFFSET and
 * SaveData_ApplyRuntimeState restores it from state->vblank_counter, a u32
 * (save_data.h:65). Sign is not visible in any use (++, a store of that
 * local, a u32 field in and out), so s32 follows D_8009B09C and D_8009B0C8
 * and is not established.
 *
 * main_frame.c and src/candidates/func_80012B50.c reach it gp-relative and
 * take the volatile form below; SaveData_ApplyRuntimeState stores through
 * $at (lui/sw) and SaveData_BuildPayload loads through a lui/lw pair, each
 * while reaching another symbol through $gp, so save_data_payload.c, which
 * holds both, defines the .data arm. */
#ifdef D_8009B0C4_IN_DATA
extern s32 D_8009B0C4 __attribute__((section(".data")));
#else
extern volatile s32 D_8009B0C4;
#endif

/* A flags halfword. Graphics_SyncFrame skips DrawSync(0) when bit 0x8000
 * is set; Graphics_BeginFrame calls PutDispEnv only when bit 0x2000 is set;
 * func_80013360 ors 0x2000 in for its screen-offset adjustment loop and
 * clears it with & 0xDFFF when Start is pressed; Main_Init stores 0x5000;
 * Main_RunBootSequence zeroes it twice. Nothing in C reads 0x4000 or 0x1000.
 * Initial value not read. u16 follows the definition in graphics_frame.c
 * and every retail load, which is lhu; Main_Init and
 * main_run_boot_sequence.c
 * only store to it.
 *
 * graphics_frame.c defines it (gp-relative in the target);
 * src/candidates/func_80012B50.c and src/candidates/func_80013360.c reach it
 * gp-relative and take the plain form.
 * main_init.c used to declare it volatile with the rest of its init block;
 * on this symbol the plain form builds byte-identical (measured by the PR
 * that added this block). Main_RunBootSequence stores through $at (lui/sh) in
 * a unit that reaches one other symbol through $gp, so
 * main_run_boot_sequence.c
 * defines the .data arm. */
#ifdef D_8009B098_IN_DATA
extern u16 D_8009B098 __attribute__((section(".data")));
#else
extern u16 D_8009B098;
#endif

extern DISPENV gGraphics_DispEnv;

/* SDK environment addresses passed to PutDrawEnv / PutDispEnv. Keep the
 * draw environment unsized, and retain startup's volatile byte stores.
 * See notes/graphics-frame-environments.md for the address/field evidence. */
#ifdef GRAPHICS_DRAW_ENV_IS_VOLATILE
extern volatile DRAWENV D_800FE048[];
#else
extern DRAWENV D_800FE048[];
#endif
extern DISPENV D_800FE0A8;

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
 * D_800E9D90, sits at +0x20, and nothing in either tree touches the sixteen
 * bytes between. Four units used to declare this `u8 [100]` -- func_8001944C.c
 * said why: "Spelt as an oversized array so -G8 keeps the eight-byte
 * rectangle out of small data; the retail code addresses it with a %hi/%lo
 * pair". Sixteen bytes clear -G8 on their own, so the real shape does the
 * same job. Retail reaches it with a %hi/%lo pair everywhere, never
 * gp-relative. */
extern RECT D_800E9D70[2];

/* The tint colour, three consecutive bytes with the components in address
 * order blue, green, red. func_8005B8A0 and Movie_StopStream pass them straight
 * to ClearImage(RECT *, u8 r, u8 g, u8 b) as r = D_8009B144, g = D_8009B143,
 * b = D_8009B142, which is what fixes the roles; graphics_frame.c copies the
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
extern volatile u8 D_8009B142 __attribute__((section(".data")));
extern volatile u8 D_8009B143 __attribute__((section(".data")));
extern volatile u8 D_8009B144 __attribute__((section(".data")));
#elif defined(D_8009B142_IN_DATA)
extern u8 D_8009B142 __attribute__((section(".data")));
extern u8 D_8009B143 __attribute__((section(".data")));
extern u8 D_8009B144 __attribute__((section(".data")));
#elif defined(D_8009B142_IS_AGGREGATE)
extern u8 D_8009B142[];
extern u8 D_8009B143[];
extern u8 D_8009B144[];
#else
extern u8 D_8009B142;
extern u8 D_8009B143;
extern u8 D_8009B144;
#endif

/* Viewport scroll offset in pixels, signed: func_8002A660 subtracts it from a
   sprite position and func_80040588 subtracts it from a primitive's, and both
   results have to be able to go negative.

   Files that reach these through `__attribute__((section(".data")))`, or as an
   unsized or [4] array, select the guarded view below because those spellings
   change how the address is materialised, not just how the value reads. */
#ifdef GGRAPHICS_VIEWPORT_SIZED_UNSIGNED_IN_DATA
extern u16 gGraphics_uViewportX[4] asm("gGraphics_sViewportX")
    __attribute__((section(".data")));
extern u16 gGraphics_uViewportY[4] asm("gGraphics_sViewportY")
    __attribute__((section(".data")));
#else
extern s16 gGraphics_sViewportX;
extern s16 gGraphics_sViewportY;
#endif

/* The double-buffered graphics work area. Graphics_BeginFrame picks the half
 * for the frame it is starting and publishes it:
 *
 *     D_8009B0B4 = &D_8009B4A8[gGraphics_bActiveBuffer * 20832];
 *
 * so the buffer is 20832 bytes per half, selected by the same index that
 * chooses D_800A5768's half a few lines above it. Main_Init takes the base
 * while it brings the loader block up.
 *
 * Left unsized, which is what both declarers already said; the stride is the
 * measurement here, not the total. */
extern u8 D_8009B4A8[];

/* The other half of that pair, the one the comment above refers to:
 *
 *     arg = &D_800A5768[gGraphics_bActiveBuffer * 140000];
 *
 * Model_HasInsufficientBufferSpace says what the region is. It takes the
 * allocation pointer D_800FE240, subtracts this base to get the bytes used,
 * subtracts the current half (`D_8009AFA2 * 140000`, D_8009AFA2 being the copy
 * of the buffer index Graphics_BeginFrame stores next to it), and measures the
 * remainder against 0x222E0.
 *
 * 0x222E0 is 140000. The stride and the capacity are the same number, written
 * decimal where the half is selected and hex where the free space is checked,
 * so this is one 140000-byte buffer per half and the model data is
 * bump-allocated inside the active one.
 *
 * Unsized for the same reason as its neighbour: the stride is measured, the
 * number of halves is not. */
extern u8 D_800A5768[];

/* Three flags Main_Init sets in its init block: D_8009B0AD and D_8009B0D0 to
 * 1, D_8009B0A8 to 0. graphics_frame.c DEFINES all three plain, for the same
 * reason it defines D_8009B0C1: the assembler resolves a small global
 * gp-relative only when the unit defines it.
 *
 * main_services.c reached them through its own `extern volatile u8`
 * declarations, under a comment saying the init block is volatile so the
 * emitted order is the source order. That holds for the rest of that run but
 * not for these three: dropping the volatile and taking these declarations
 * instead builds byte-identical, measured by the PR adding this block. The
 * same was already true of D_8009B0C1 above, whose volatile view in
 * main_init.c was dropped the same way.
 *
 * Their neighbour D_8009B0A0 is deliberately NOT here: graphics_frame.c
 * defines it `u8 D_8009B0A0[4]` while src/candidates/func_80013154.c both
 * declares it a scalar and assigns `D_8009B0A0 = 2`. Array and scalar are
 * two faithful views of one address, so neither spelling can absorb the
 * other and that declaration stays local. */
extern u8 D_8009B0AD;
extern u8 D_8009B0D0;
extern u8 D_8009B0A8;

void Graphics_SyncFrame(void);
void Graphics_BeginFrame(void);

#endif
