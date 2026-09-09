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
 * The main_menu overlay only ever uses slot 0, so it previously declared the
 * registry as a bare `void (*)(void)` -- and, where it only cleared the slot,
 * as an `s32`. Both spellings write the right address, because slot 0 sits at
 * the base, but they hide that three more slots follow and that something else
 * may own them. Reaching slot 0 by index says what the code is really doing.
 *
 * This is a registry of independent slots, NOT a chain: no slot is called with
 * arguments, none returns a value, and nothing enforces an order beyond the
 * index. Which subsystem owns slots 1 to 3 is still unknown -- every writer
 * found, in C and in the generated assembly for both the resident image and
 * the overlays, materializes %lo(D_800E9DB0) with no displacement, so slot 0
 * is the only one anything is known to write. */
extern void (*D_800E9DB0[4])(void);

/* The recovery point the registry's comment above already places at
 * 0x800E9DC0. main_init.c arms it with setjmp once the boot sequence is up,
 * and two sources jump back into it: main_run_frontend_menus.c passes 1 from
 * the game-over path and func_80030FD0.c passes 2, so the value distinguishes
 * which unwound. All three spell it jmp_buf and all three already include
 * psyq/setjmp.h, which this header now includes so the declaration stands on
 * its own. */
extern jmp_buf D_800E9DC0;

/* The single extra callback the pump runs after the four slots, and that
 * func_800134B4 clears alongside them. Only main_services.c refers to it. */
extern void (*D_8009B0B8)(void);

/* A one-byte state value main_services.c sets alongside D_8009B0A0 to
 * D_8009B0A2, and that the two menu runners set to 10 or 6 through index 0.
 *
 * Three arms, because all three consumers want a different addressing form
 * and each is load bearing. main_services.c wants the volatile scalar: it
 * writes the byte directly and -G8 reaches a scalar gp-relative.
 * main_run_duel_and_library.c wants a sized array and
 * main_run_selection_menus.c an unsized one, which are the two ways to leave
 * small data.
 *
 * The [9] is a lever, not a size. c_symbols.ld names D_8009B0A4 one byte
 * after this symbol, so nine bytes would run through that and on past
 * gGraphics_bActiveBuffer at 0x8009B0AC. The array length is chosen to make
 * the assembler build an absolute address, exactly as gDuel_bTerrain's [8]
 * and gSD_bOutputType's [16] are, and it asserts nothing about storage. */
#ifdef D_8009B0A3_IS_VOLATILE_SCALAR
extern volatile u8 D_8009B0A3;
#elif defined(D_8009B0A3_SIZED_ARRAY)
extern u8 D_8009B0A3[9];
#else
extern u8 D_8009B0A3[];
#endif

/* The pending frontend-menu request. Main_RunMenu hands the pair to the
   main_menu overlay as MainMenu_InitFrontendMenu(D_8009B268, D_8009B26D),
   whose parameters are (unused, menu): D_8009B26D is the menu id --
   func_8002D458 stores its mode argument, func_8002EE94 and func_8002FA28
   store 5, func_80030CB0 round-trips it through gDebug_nSceneOrSoundID,
   Main_RunGameOver stores 0 -- and D_8009B268 is stored 1 beside every
   request and 0 in three of func_8002D458's arms. Both are bytes, read lbu.
   func_8002D458.c and main_run_frontend_menus.c reach them through $gp;
   func_8002EE94.c, func_8002FA28.c and frontend_scene_states.c address them
   with %hi/%lo, outside small data, and define the .data arms. */
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
void func_800134B4(void);

#endif
