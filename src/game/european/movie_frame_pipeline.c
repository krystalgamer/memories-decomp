#include "../../types.h"

#define VERSION_EUROPE

#define MOVIE_WORK_AREA_HEAD_SIZE 0x3000
#define MOVIE_FRAME_HEIGHT 0x100
#define MOVIE_DECODED_SLOT_SIZE 0x3000
#include "../movie_frame_pipeline.c"
