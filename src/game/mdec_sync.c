#include "../types.h"
#include "../psyq/libpress.h"
#include "mdec_sync.h"
#include "movie_frame_pipeline.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"
#include "movie_seek_and_start_read.h"
#include "movie_on_stream_end.h"

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

#define CD_LOCATION_BYTES(location) ((u8 *)(location))
#define DS_LOCATION_VIEW(location) ((DslLOC *)(location))

void Movie_SeekAndStartRead(CdlLOC *loc){while(!CdControlB(CdlSetloc,CD_LOCATION_BYTES(loc),0)){}while(!CdControlB(CdlSeekP,CD_LOCATION_BYTES(loc),0)){}while(!DsRead2(DS_LOCATION_VIEW(loc),0x1E0)){} }

void Movie_OnStreamEnd(void)
{
    gMovie_bStreamEnded = 1;
}
