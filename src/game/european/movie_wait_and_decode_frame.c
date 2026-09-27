#include "../../types.h"

/* SLES-03947 build of src/game/movie_frame_pipeline.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_MOVIE_WAIT_AND_DECODE_FRAME

/* The movie work area's head; 0x2400 in the US build. */
#define MOVIE_WORK_AREA_HEAD_SIZE 0x3000

/* European addresses of the US-named globals this function reaches. */
#define D_8009B060 gEuropean_D_8009B060
#define D_8009B066 gEuropean_D_8009B066
#define D_8009B068 gEuropean_D_8009B068
#define D_8009B06C gEuropean_D_8009B06C
#define D_8009B070 gEuropean_D_8009B070
#define D_8009B498 gEuropean_D_8009B498
#define D_8009B49C gEuropean_D_8009B49C

#include "../movie_frame_pipeline.c"
