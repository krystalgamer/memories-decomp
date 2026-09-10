#include "../types.h"
#include "mem_card_directory.h"

/* The .sdata window at 0x8009AF6C, carved as one unit because its objects
 * tile it exactly with no gaps: 4 + 4 + 8 + 4 + 8 = 28 bytes, which is
 * precisely the distance to model_graphics_state at 0x8009AF88.
 *
 * The objects here have nothing to do with each other. Four subsystems
 * happened to land small objects next to one another, so this file is named
 * for the address rather than for a subject, the way data_80091510.c is.
 * Splitting it by owner is not possible: the units are smaller than the
 * granularity the split file works in.
 *
 * Splat names seven labels in this range, not five. D_8009AF74 below is one
 * eight-byte object, but code addresses its middle elements directly, so the
 * disassembler also named 0x8009AF76 and 0x8009AF7A. Those two labels are
 * positions inside an array, not separate objects, and defining them
 * separately here would be wrong even though it would produce identical
 * bytes. display_object_helpers.h reached that conclusion first and records
 * the reasoning; this file follows it.
 *
 * Every object carries an explicit .sdata attribute, as the neighbouring
 * carves do. Nothing here is entirely zero, so none of it is at risk of
 * migrating to .sbss, but the attribute is kept for consistency with
 * model_primitive_templates.c and save_data_mask_state.c and so that the
 * placement is stated rather than inferred from the -G8 threshold.
 *
 * Array sizes are written out rather than left to the initializer. Each one
 * is what the retail layout requires, and for the three strings that is
 * wider than the text: the trailing bytes are real padding in the image and
 * an unsized array would come out short. */

/* Used only by the still-unmatched function at 0x8002E41C, which passes it
 * to a print routine. */
char D_8009AF6C[4] __attribute__((section(".sdata"))) = "%s\n";

/* Read by MemCardDialog_UpdateSave (src/candidates/func_8003E854.c). */
u8 D_8009AF70[4] __attribute__((section(".sdata"))) = "*";

/* Indexed by a display object's ot_index. Declared in
 * display_object_helpers.h, which explains the volatile qualifier and why
 * elements [1] and [3] used to carry private names of their own. The
 * qualifier is load-bearing in display_object_helpers.c, so it belongs on
 * the definition too. */
volatile u16 D_8009AF74[4] __attribute__((section(".sdata"))) = {
    0x0000,
    0x0020,
    0x0800,
    0x0020,
};

/* Read by MemCard_DoLoadDirectory as the pattern that lists every file. Same
 * single character as D_8009AF70, but a separate object in the image, so it
 * stays separate here. */
u8 D_8009AF7C[4] __attribute__((section(".sdata"))) = "*";

/* The MIDI track chunk tag. sound_sequence_marker_scan.c compares against it
 * while walking a sequence, which is what the tag means in a standard MIDI
 * file: the four bytes that introduce each track. */
u8 D_8009AF80[8] __attribute__((section(".sdata"))) = "MTrk";
