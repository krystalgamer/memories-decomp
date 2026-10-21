#include "../types.h"
#include "../psyq/libpress.h"
#include "mdec_sync.h"
#include "movie_frame_pipeline.h"

void Movie_StartFadeOut(void)
{
    gMovie_bFadingOut = 1;
}

s32 Movie_WaitFrameDecoded(void)
{
    s32 timeout = 0x800000;
    if (gMovie_bFrameDecoded == 0) {
        for (;;) {
            if (--timeout <= 0) {
                DecDCTReset(1);
                break;
            }
            if (gMovie_bFrameDecoded != 0) {
                break;
            }
        }
    }
    gMovie_bFrameDecoded = 0;
    return 0;
}
