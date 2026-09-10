#include "../types.h"
#include "movie_frame_pipeline.h"

/* The movie frame pipeline's .sdata state block, 0x8009B060 to 0x8009B073.
 *
 * movie_frame_pipeline.h already describes what these are and settled that
 * they form one family: every symbol was declared identically by each of its
 * users and none is named outside the pipeline. This file only gives that
 * block a definition, so the header stays the place the meaning is recorded.
 *
 * The block sits inside the 40-byte range at 0x8009B058 that the notes call
 * the scattered tail. Scattered is right about ownership and wrong about
 * structure: the range holds three separate owners, and this is the middle
 * one. main_menu_load_package_stage.c owns the eight bytes before it and
 * model_transfer_flags.h the twelve after, so both are left extracted and
 * this carve takes only the run that one family names.
 *
 * Every object is zero, which makes the section attributes load bearing
 * rather than decorative: a zero-initialised object is placed in .sbss, and
 * without the attribute all twenty bytes would leave .sdata at once and the
 * window would collapse. model_primitive_templates.c relies on the same
 * thing for its three trailing zero words.
 *
 * The eight bytes are written as eight separate objects rather than an array
 * because that is how the header declares them and how every consumer reads
 * them -- none indexes across the run. Two of them, D_8009B062 and the
 * unassigned neighbour above this block, were never given linker aliases,
 * which is the same conclusion reached from the other direction. */

u8 D_8009B060 __attribute__((section(".sdata"))) = 0;
u8 D_8009B061 __attribute__((section(".sdata"))) = 0;
u8 D_8009B062 __attribute__((section(".sdata"))) = 0;
u8 D_8009B063 __attribute__((section(".sdata"))) = 0;
u8 D_8009B064 __attribute__((section(".sdata"))) = 0;
u8 D_8009B065 __attribute__((section(".sdata"))) = 0;
u8 D_8009B066 __attribute__((section(".sdata"))) = 0;
u8 D_8009B067 __attribute__((section(".sdata"))) = 0;
u32 D_8009B068 __attribute__((section(".sdata"))) = 0;
u32 D_8009B06C __attribute__((section(".sdata"))) = 0;
u32 D_8009B070 __attribute__((section(".sdata"))) = 0;
