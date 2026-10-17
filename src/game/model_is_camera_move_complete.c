#include "../types.h"
#include "model.h"
#include "model_is_camera_move_complete.h"

int Model_IsCameraMoveComplete(int arg)
{
    int result = 0;
    if (D_800F2B20.flags == 0) {
        if (arg != 0 || D_800F2B20.mode == 0 || D_800F2B20.field_02 == 0)
            result = 1;
    }
    return result;
}
