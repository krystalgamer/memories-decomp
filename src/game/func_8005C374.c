#include "../types.h"
#include "graphics_frame.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libpress.h"
#include "../psyq/libcd.h"
#include "file_cd_helpers.h"
#include "func_8005C62C.h"
#include "graphics_constants.h"
#include "mdec_sync.h"
#include "sound_mix.h"

/* The movie player's setter for the three bytes at D_8009B4A0, the last
   function of its gcc_2_8_1_g8 run. It follows func_8005C1F4, now a candidate
   in src/candidates/func_8005C1F4.c, and precedes movie_stream_requests.c. */

extern u8 D_8009B4A0;
extern u8 D_8009B4A1;
extern u8 D_8009B4A2;

void func_8005C374(s32 first, s32 second, s32 third)
{
    D_8009B4A0 = first;
    D_8009B4A1 = second;
    D_8009B4A2 = third;
}
