#ifndef MEMORIES_DECOMP_MAIN_SERVICES_H
#define MEMORIES_DECOMP_MAIN_SERVICES_H

#include "../types.h"
#include "../psyq/setjmp.h"

/* The per-frame callback registry the resident service pump walks.
 *
 * There are four slots, not one. func_8001306C calls every non-null slot in
 * order once per frame, and func_800134B4 clears all four. The size is not a
 * guess: c_symbols.ld places D_800E9DB0 at 0x800E9DB0 and the next symbol,
 * D_800E9DC0, sixteen bytes later, which is exactly four pointers.
 *
 * The main_menu overlay previously declared slot 0 as a bare
 * `void (*)(void)` -- and, where it only cleared the slot, as an `s32`. Both
 * spellings write the right address, because slot 0 sits at
 * the base, but they hide that three more slots follow and that something else
 * may own them. Reaching slot 0 by index says what the code is really doing.
 *
 * This is a registry of independent slots, NOT a chain: no slot is called with
 * arguments, none returns a value, and nothing enforces an order beyond the
 * index. Slot 3 is stored by func_800179F4 (src/candidates/func_800179F4.c:170,
 * `D_800E9DB0[3] = func_800164FC;`; func_800179F4.s:207-208), func_8002BFCC
 * (func_8002BD0C.c:189, `D_800E9DB0[3] = func_80029EC4;`; func_8002BFCC.s:52
 * and :56) and CampaignMap_SetLocation
 * (src/overlays/overworld/set_location.c:56,
 * `D_800E9DB0[3] = CampaignMap_UpdateView;`), and by func_8002ACA4.s, still
 * assembly: `sw $zero` at :407 and the address of func_80029EC4 at :854.
 * Slot 2 is stored only by func_8002ACA4.s (:509, :745), under splat's name
 * for that address, D_800E9DB8; no other listing and no C unit names it.
 * Slot 1 is stored by the main_menu overlay, by index:
 * MainMenu_InitTradeScreen (src/overlays/main_menu/trade_update.c,
 * `D_800E9DB0[1] = MainMenu_DrawTradeOffersAndHighlights;`) and
 * MainMenu_ReleaseTradeDisplayHandles
 * (src/overlays/main_menu/trade_offers.c:145, `D_800E9DB0[1] = 0;`). Before
 * that the overlay reached it as D_800E9DB4, with a private declaration in
 * each unit. The per-slot names D_800E9DB8 and D_800E9DBC (0x800E9DB0 + 8
 * and + 12) carry no displacement off D_800E9DB0, so their writers do not
 * appear as `%lo(D_800E9DB0)` references. */
extern void (*D_800E9DB0[4])(void);

/* The recovery point the registry's comment above already places at
 * 0x800E9DC0. Main_Init arms it with setjmp once the boot sequence is up,
 * and two functions jump back into it: Main_RunGameOver passes 1 from the
 * game-over path and func_80030FD0.c passes 2, so the value distinguishes
 * which unwound (the first two are now in src/candidates/). All three spell
 * it jmp_buf and include psyq/setjmp.h, which this header now includes so
 * the declaration stands on its own. */
extern jmp_buf D_800E9DC0;

/* The single extra callback the pump runs after the four slots, and that
 * func_800134B4.c clears alongside them; only it and main_services.c use it. */
extern void (*D_8009B0B8)(void);

/* The signed per-frame watchdog owned and initialized by main_services.c. */
extern s32 runtime_gp;

/* A one-byte state value func_80013154 sets alongside D_8009B0A0 to
 * D_8009B0A2, and that the two menu runners set to 10 or 6 through index 0.
 * c_symbols.ld names D_8009B0A4 one byte on, so no spelling here claims the
 * object is wider than the byte it is.
 *
 * Two arms, because the consumers split on addressing form. func_80013154
 * writes the byte from small data, which is what a scalar reaches at -G8:
 *     sb         $v1, %gp_rel(D_8009B0A3)($gp)
 * (src/candidates_target/func_80013154.S:65), and that arm is the volatile
 * one. Main_RunDuel and Main_RunCampaignMap write it through index 0 and
 * both want the absolute form, which an array of unknown size reaches at
 * the same -G8:
 *     lui        $at, %hi(D_8009B0A3)
 *     sb         $v0, %lo(D_8009B0A3)($at)
 * -- src/candidates_target/func_8002CEE8.S:35 and :120, and the same two
 * words 0A80013C / A3B022A0 at 0x8002D2F8 and 0x8002D350 inside
 * Main_RunCampaignMap.
 *
 * A third arm, `u8 D_8009B0A3[9]`, used to serve
 * src/candidates/func_8002CEE8.c. A bound the assembler can see is only
 * needed where its -G sits below the compiler's, which is what
 * duel_terrain_boost.h records for gDuel_bTerrain's [8] under
 * gcc_2_8_1_cc_g8_as_g4_split; both array consumers here are plain
 * gcc_2_8_1_g8, where the incomplete array is already outside small data.
 * Relaxing the [9] to the shared [] leaves the retail SHA-256 unchanged. */
#ifdef D_8009B0A3_IS_VOLATILE_SCALAR
extern volatile u8 D_8009B0A3;
#else
extern u8 D_8009B0A3[];
#endif

/* The pending frontend-menu request. Main_RunMenu hands the pair to the
   main_menu overlay as MainMenu_InitFrontendMenu(D_8009B268, D_8009B26D),
   whose parameters are (unused, menu): D_8009B26D is the menu id --
   Main_ApplyMenuSelection stores its selection argument, func_8002EE94 and func_8002FA28
   store 5, func_80030CB0 round-trips it through gDebug_nSceneOrSoundID,
   Main_RunGameOver stores 0 -- and D_8009B268 is stored 1 beside every
   request and 0 in three of Main_ApplyMenuSelection's arms. Both are bytes, read lbu.
   main_apply_menu_selection.c and main_run_frontend_menus.c reach them through $gp;
   src/candidates/func_8002EE94.c, func_8002FA28.c and frontend_scene_states.c
   address them with %hi/%lo, outside small data, and define the .data arms. */
#ifdef D_8009B268_IN_DATA
extern u8 D_8009B268 __attribute__((section(".data")));
#else
extern u8 D_8009B268;
#endif

#ifdef D_8009B26D_IN_DATA
extern u8 D_8009B26D __attribute__((section(".data")));
#else
extern u8 D_8009B26D;
#endif

/* Main_Init's no-argument spelling is load-bearing even though its work-area
 * pointer is already in $a0; making the source argument explicit changes the
 * caller before the call. The definition sees the canonical pointer arm. */
#ifdef FUNC_80013154_NO_ARGS
void func_80013154(void);
#else
void func_80013154(u8 *base);
#endif
void func_80013360(void);

/* The pump itself, and the call that empties the registry. func_8001306C
   walks the four slots once per frame; func_800134B4 clears all four. Both
   callers already spelled the pump this way. */
void func_8001306C(void);
void func_800134B4(void);

#endif
