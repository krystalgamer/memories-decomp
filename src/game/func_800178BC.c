/* Scratchpad C stores and official Psy-Q GTE macros reproduce the projection
 * block through the RTPS-normalizing profile. */
#include "../types.h"
#include "func_800178BC.h"
#include "view_state.h"
#include "screen_projection.h"
#include "duel_side_view_angles.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "../unmatched.h"
#include "func_80017130.h"

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
    {
        u8 *scratch = (u8 *)0x1F8003E0;

        *(s16 *)scratch = 0x3E8;
        *(s16 *)(scratch + 2) = 0;
        *(s16 *)(scratch + 4) = 0x3E8;
        gte_ldv0(scratch);
        gte_rtps();
        gte_stsxy(&p);
    }
    /* Taking y through an s32 keeps the sign extension in the load, which is
       what makes it lh rather than the lhu the x read gets. */
    y = p.y;
    D_8009B200 = p.x - 0xA0;
    D_8009B202 = y - 0x6C;
    SetGeomOffset(0, 0);
    func_80017130();
}
