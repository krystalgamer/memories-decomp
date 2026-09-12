#include "../types.h"
#include "../game/func_800178BC.h"
#include "../game/view_state.h"
#include "../game/screen_projection.h"
#include "../game/duel_side_view_angles.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"
#include "../psyq/inline_c.h"
#include "../unmatched.h"
#include "../game/func_80017130.h"

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
    {
        volatile s16 *scratch = (volatile s16 *)0x1F8003E0;
        s16 value = 0x3E8;

        scratch[0] = value;
        scratch[1] = 0;
        scratch[2] = value;
        gte_ldv0((s16 *)scratch);
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
