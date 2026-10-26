#include "../types.h"
#include "model.h"
#include "model_is_camera_move_complete.h"
#include "model_camera_state.h"

int Model_IsCameraMoveComplete(int arg)
{
    int result = 0;
    if (gModel_CameraMove.flags == 0) {
        if (arg != 0 || gModel_CameraMove.mode == 0 || gModel_CameraMove.follow_countdown == 0)
            result = 1;
    }
    return result;
}
