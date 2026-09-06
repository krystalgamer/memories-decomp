#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

extern s16 D_800F2856;
extern u8 D_800FE148[];
extern u16 D_800908A0[];

/* Projects the coordinate pair at D_800908A0[index * 2] through the GTE and
   returns the projected screen X. The result is read back as a signed halfword,
   which is what makes the return type s16 rather than s32. */
s16 func_8001B0CC(s32 index)
{
    /* The scaled index has to survive three calls, so it is pinned to the
       callee-saved register the GTE block below reads. */
    register s32 coord asm("$16") = index;
    s16 result;

    SetGeomScreen(D_800F2856);
    coord = coord * 4;
    SetGeomOffset(0xA0, 0x6C);
    GsSetLsMatrix((MATRIX *)D_800FE148);
    __asm__ volatile(
        "lui $3, 0x1F80\n"
        "ori $3, $3, 0x03E0\n"
        "lui $2, %%hi(D_800908A0)\n"
        "addiu $2, $2, %%lo(D_800908A0)\n"
        "addu $16, $16, $2\n"
        "lhu $4, 0($16)\n"
        "addiu $2, $0, -0x18\n"
        "sh $2, 2($3)\n"
        "sh $4, 0($3)\n"
        "lhu $2, 2($16)\n"
        "nop\n"
        "sh $2, 4($3)\n"
        "lwc2 $0, 0($3)\n"
        "lwc2 $1, 4($3)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n" /* rtps */
        "addiu $2, $sp, 0x10\n"
        "swc2 $14, 0($2)\n"
        : "+r"(coord), "=m"(result) : : "$2", "$3", "$4"
    );
    SetGeomOffset(0, 0);
    return result;
}
