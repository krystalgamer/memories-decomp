#include "../../types.h"

/* SLES-03947 build of src/game/movie_frame_pipeline.c: only the functions enabled below. */

#define VERSION_EUROPE
#define VERSION_EUROPE_FUNC_8005C1F4
#define VERSION_EUROPE_FUNC_8005C374

#define MOVIE_WORK_AREA_HEAD_SIZE 0x3000
#define MOVIE_DECODED_SLOT_SIZE 0x3000

/* European addresses of the US-named globals these functions reach. */
#define D_8009B060 gEuropean_D_8009B060
#define D_8009B067 gEuropean_D_8009B067
#define D_8009B498 gEuropean_D_8009B498
#define D_800F5D44 D_800F6CD4
#define StCdInterrupt func_80078A34
#define DecDCTout func_80090DB8
/* As in func_8005B8A0.c. */
#define func_8005C1F4 func_8005039C
#define D_8009B4A0 gEuropean_D_8009B4A0
#define D_8009B4A1 gEuropean_D_8009B4A1
#define D_8009B4A2 gEuropean_D_8009B4A2

#include "../movie_frame_pipeline.c"
