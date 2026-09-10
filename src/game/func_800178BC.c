#include "../types.h"
#include "func_800178BC.h"
#include "view_state.h"
#include "screen_projection.h"
#include "duel_side_view_angles.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../unmatched.h"
#include "func_80017130.h"

extern s16 D_8009B200;
extern s16 D_8009B202;
void func_800178BC(void)
{
    ProjectedPair p;
    s32 y;

    func_80017130();
    SetGeomScreen(D_800F2848.projection);
    SetGeomOffset(0xA0, 0x6C);
    D_800F2848.field_00 = 0x14E;
    D_800F2848.field_04 = 0x3FE;
    D_800F2848.angle = D_8009AF20[0];
    func_8001352C();
    GsSetLsMatrix(&D_800FE148);
    __asm__ volatile(
        "lui $2, 0x1F80\n"
        "ori $2, $2, 0x03E0\n"
        "addiu $3, $0, 0x3E8\n"
        "sh $3, 0($2)\n"
        "sh $0, 2($2)\n"
        "sh $3, 4($2)\n"
        "lwc2 $0, 0($2)\n"
        "lwc2 $1, 4($2)\n"
        "nop\n"
        "nop\n"
        ".word 0x4A180001\n" /* rtps */
        "addiu $2, $sp, 0x10\n"
        "swc2 $14, 0($2)\n"
        : "=m"(p) : : "$2", "$3"
    );
    /* Taking y through an s32 keeps the sign extension in the load, which is
       what makes it lh rather than the lhu the x read gets. */
    y = p.y;
    D_8009B200 = p.x - 0xA0;
    D_8009B202 = y - 0x6C;
    SetGeomOffset(0, 0);
    func_80017130();
}
