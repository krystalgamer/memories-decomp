#include "../../types.h"
#define D_8009B142_IN_DATA
#define HIGH_MEMORY_ADDRESSES_BASE_IN_DATA
#include "../movie_open_stream.h"
#define MOVIE_FRAME_PIPELINE_REGIONAL_SMALL_STORAGE
/* This build's own zero-initialized storage at 0x8009C818-0x8009C838 (#2602); its
 * layout differs from the North American block the shared source defines. */
u8 *gMovie_pWorkArea;
u32 D_8009C81C_sles_03948;
u8 D_8009C820_sles_03948;
u8 D_8009C821_sles_03948;
u16 D_8009C822_sles_03948;
u32 D_8009C824_sles_03948;
u32 D_8009C828_sles_03948;
u32 D_8009C82C_sles_03948;
u32 D_8009C830_sles_03948;
u32 D_8009C834_sles_03948;

/* SLES-03948 to SLES-03951 build of src/game/movie_frame_pipeline.c, which
 * gives these builds their own zero-initialized storage (#2602). */
#include "../european/movie_frame_pipeline.c"
