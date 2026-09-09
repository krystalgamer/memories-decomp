#include "../types.h"
#include "model_image_copy_frames.h"

/* Initialized data at 0x80091508: the source-row sequence for the eight-frame
   ping-pong image copy in func_8004E9A0. */
u8 gModel_abImageCopyFrameRows[MODEL_IMAGE_COPY_FRAME_COUNT] = {
    0, 1, 2, 3, 4, 3, 2, 1,
};
