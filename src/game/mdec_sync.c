#include "../types.h"
#include "../psyq/libpress.h"
#include "mdec_sync.h"
#include "movie_frame_pipeline.h"

void Movie_StartFadeOut(void)
{
    D_8009B064 = 1;
}

s32 Movie_WaitFrameDecoded(void)
{
    s32 timeout = 0x800000;
    if (D_8009B062 == 0) {
        for (;;) {
            if (--timeout <= 0) {
                DecDCTReset(1);
                break;
            }
            if (D_8009B062 != 0) {
                break;
            }
        }
    }
    D_8009B062 = 0;
    return 0;
}
