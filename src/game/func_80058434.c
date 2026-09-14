#include "../types.h"
#include "model_update_view_metrics.h"
#include "camera_view.h"
#include "func_80058434.h"
#include "../psyq/libgte.h"
#include "model.h"
#include "../unmatched.h"

/* &D_800F56F0.vrx. Keep the interior symbol so the object retains the retail
 * relocation target while the canonical GsRVIEW2 owns the surrounding view. */

void func_80058434(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    long *b;
    long *t;
    s32 c;
    s32 sn;
    s32 r;
    s32 u;
    s32 w;
    s32 z;
    s32 v;

    /* b and t are the two long triples at the head of the reference view,
       vpx..vpz and vrx..vrz: t is moved around b. */
    if (arg0 > 0) {
        b = (long *)D_800F56FC;
        t = b - 3;
    } else {
        b = &D_800F56F0.vpx;
        t = b + 3;
    }

    if (*(s16 *)&D_8009B478 + arg3 >= 0xB) {
        *(u16 *)&D_8009B478 = *(u16 *)&D_8009B478 + arg3;
    }

    if ((arg1 & MODEL_ANGLE_MASK) != 0) {
        v = *(s16 *)&D_8009B47A + arg1 + MODEL_ANGLE_FULL_TURN;
        D_8009B47A = v - v / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if ((arg2 & MODEL_ANGLE_MASK) != 0) {
        v = *(s16 *)&D_8009B47C + arg2 + MODEL_ANGLE_FULL_TURN;
        D_8009B47C = v - v / MODEL_ANGLE_FULL_TURN *
            MODEL_ANGLE_FULL_TURN;
    }

    if (arg1 != 0 || arg2 != 0 || arg3 != 0) {
        c = rcos(*(s16 *)&D_8009B47C);
        sn = rsin(*(s16 *)&D_8009B47C);
        r = *(s16 *)&D_8009B478;
        u = r * c / MODEL_FIXED_ONE;
        w = r * sn / MODEL_FIXED_ONE;
        c = rcos(*(s16 *)&D_8009B47A);
        sn = rsin(*(s16 *)&D_8009B47A);
        z = u * sn / MODEL_FIXED_ONE;
        u = u * c / MODEL_FIXED_ONE;
        t[0] = b[0] + u * arg0;
        t[1] = b[1] + w * arg0;
        t[2] = b[2] + z * arg0;
    }
}
