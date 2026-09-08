#include "../types.h"
#include "camera_view.h"
#include "../psyq/libgte.h"
#include "model.h"
#include "model_update_view_metrics.h"

extern u8 D_8009B478;
extern u16 D_8009B47A;
extern u16 D_8009B47C;

void Model_UpdateViewMetrics(u8 *m) {
    u8 *b;

    b = (u8 *)&D_800F56F0;
    if (m == 0) {
        m = b;
    } else if (b != 0 && m != b) {
        D_800F56F0 = *(GsRVIEW2 *)m;
    }

    *(s16 *)&D_8009B478 = SquareRoot0((*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) * (*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) + (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)) * (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)));

    D_8009B47A = ratan2(*(s32 *)(m + 8) - *(s32 *)(m + 0x14),
                        *(s32 *)(m + 0) - *(s32 *)(m + 0xC));

    D_8009B47C = ratan2(*(s32 *)(m + 4) - *(s32 *)(m + 0x10),
                        *(s16 *)&D_8009B478);

    *(s16 *)&D_8009B478 = SquareRoot0((*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) * (*(s32 *)(m + 0) - *(s32 *)(m + 0xC)) + (*(s32 *)(m + 4) - *(s32 *)(m + 0x10)) * (*(s32 *)(m + 4) - *(s32 *)(m + 0x10)) + (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)) * (*(s32 *)(m + 8) - *(s32 *)(m + 0x14)));

    D_8009B47A = (*(s16 *)&D_8009B47A + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
    D_8009B47C = (*(s16 *)&D_8009B47C + MODEL_ANGLE_FULL_TURN) %
        MODEL_ANGLE_FULL_TURN;
}
