#include "../../types.h"

#define VERSION_EUROPE
#define VERSION_EUROPE_MOVIE_DECODE_AND_PRESENT_FRAME

#define MOVIE_WORK_AREA_HEAD_SIZE 0x3000
#define MOVIE_FRAME_HEIGHT 0x100
#define MOVIE_DECODED_SLOT_SIZE 0x3000

#define D_8009B060 gEuropean_D_8009B060
#define D_8009B065 gEuropean_D_8009B065
#define D_8009B066 gEuropean_D_8009B066
#define D_8009B067 gEuropean_D_8009B067
#define D_8009B498 gEuropean_D_8009B498

#define GetDrawEnv func_8008025C
#define DecDCTin func_80090D3C
#define DecDCTout func_80090DB8

#include "../movie_frame_pipeline.c"
