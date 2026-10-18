#include "../../types.h"

#define VERSION_EUROPE
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
#define D_800F5D44 D_800F6CD4
#define StCdInterrupt func_80078A34
/* As in european/func_8005B8A0.c. */
#define func_8005C1F4 func_8005039C
#define D_8009B4A0 gEuropean_D_8009B4A0
#define D_8009B4A1 gEuropean_D_8009B4A1
#define D_8009B4A2 gEuropean_D_8009B4A2
#define ClearImage func_8007FBE4
#define D_8009B061 gEuropean_D_8009B061
#define D_800FE0CC gEuropean_D_800FE0CC
#define DecDCToutCallback func_80090E80
#define GsDefDispBuff func_80085424
#define GsDISPENV gEuropean_GsDISPENV
#define GsInitGraph2 func_80084CE4
#define StUnSetRing func_80078664
#define D_8009B068 gEuropean_D_8009B068
#define D_8009B06C gEuropean_D_8009B06C
#define D_8009B070 gEuropean_D_8009B070
#define D_8009B49C gEuropean_D_8009B49C
#include "../movie_frame_pipeline.c"
