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

/* Immediately follows func_80015DB8 in the image and shares its projection
   setup: same 0x1F8003E0 scratchpad pair, same rtps, same write-back through
   swc2 $14. Here the source coordinates come from the object's own target
   record and the projected pair is biased into the object's screen fields. */

void func_80015DFC(DisplayProjectionTrackedObject *object)
{
    ProjectedPair p;
    s32 y;

    GsSetLsMatrix(&D_800FE148);
    __asm__ volatile(
        "lw $2, 0($16)\n"
        "lui $3, 0x1F80\n"
        "lhu $2, 0x30($2)\n"
        "ori $3, $3, 0x03E0\n"
        "sh $0, 2($3)\n"
        "sh $2, 0($3)\n"
        "lw $2, 0($16)\n"
        "nop\n"
        "lhu $2, 0x34($2)\n"
        "nop\n"
        "sh $2, 4($3)\n"
        "lwc2 $0, 0($3)\n"
        "lwc2 $1, 4($3)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n" /* rtps */
        "addiu $2, $sp, 0x10\n"
        "swc2 $14, 0($2)\n"
        : "=m"(p) : : "$2", "$3"
    );
    object->screen_x = p.x - 0x1A;
    y = p.y;
    object->screen_y = y - 0x1E;
    if (D_8009B1D5 != 0) {
        if (object->field_18 < 0xF) {
            object->screen_y = y - 0x1D;
        }
    } else {
        if (object->field_18 >= 0xF) {
            object->screen_y = y - 0x1D;
        }
    }
    func_80016784(object->record, (s32)D_800E9D98[0], object->screen_x,
                  object->screen_y);
    SetGeomOffset(0xA0, 0x6C);
}
