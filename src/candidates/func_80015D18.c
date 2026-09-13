/*
 * Reclassified from matching_c (#3859). Under gcc_2_8_1_g8_split this
 * source rebuilt the target byte for byte, but only by
 * 1 inline asm statement, so it is kept here as a candidate
 * rather than counted as a decompilation. It was src/game/display_projection.c.
 */
#include "../types.h"
#include "../game/duel_side_state.h"
#include "../game/display_object.h"
#include "../game/display_projection.h"
#include "../game/screen_projection.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../game/func_80016784.h"
#define ORDERING_TABLE_SLOT2_ARRAY
#include "../game/ordering_tables.h"

void func_80015D18(DisplayObject *object)
{
    SetGeomScreen(0x12C);
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix(&D_800FE148);
    __asm__ volatile(
        "lui $3, 0x1F80\n"
        "lhu $2, 40($16)\n"
        "ori $3, $3, 0x03E0\n"
        "sh $0, 2($3)\n"
        "sh $2, 0($3)\n"
        "lhu $2, 42($16)\n"
        "nop\n"
        "sh $2, 4($3)\n"
        "lwc2 $0, 0($3)\n"
        "lwc2 $1, 4($3)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n"
        "addiu $2, $16, 48\n"
        "swc2 $14, 0($2)\n"
        : : : "$2", "$3"
    );
    object->field_30.h.field_30 -= 0x20;
    object->field_30.h.field_32 -= 0x1E;
    SetGeomOffset(0, 0);
}

/* Immediately follows func_80015DB8 in the image and shares its projection
   setup: same 0x1F8003E0 scratchpad pair, same rtps, same write-back through
   swc2 $14. Here the source coordinates come from the object's own target
   record and the projected pair is biased into the object's screen fields. */

