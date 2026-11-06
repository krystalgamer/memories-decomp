#include "../../types.h"
#include "../movie_open_stream.h"
#define MOVIE_FRAME_PIPELINE_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C818-0x8009C838 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 *gMovie_pWorkArea;
CdlLOC gMovie_StreamLoc;
u8 D_8009B4A0;
u8 D_8009B4A1;
u8 D_8009B4A2;
static PADDING_VARIABLE(1);
static PADDING_VARIABLE(4);
u32 D_8009C828;
static PADDING_VARIABLE(4);
static PADDING_VARIABLE(4);
static PADDING_VARIABLE(4);

/* SLES-03948 to SLES-03951 build of src/game/movie_pipeline.c, which
 * gives these builds their own zero-initialized storage (#2602). */
#include "../european/movie_pipeline.c"
