#ifndef MEMORIES_DECOMP_MOVIE_FRAME_PIPELINE_H
#define MEMORIES_DECOMP_MOVIE_FRAME_PIPELINE_H

#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

/* The movie frame pipeline's entry points.
 *
 * Movie_StopStream and Movie_WaitAndDecodeFrame both return a status their
 * callers are free to ignore, and both did before this shared header.
 *
 * Movie_OnDecodeOutput is the MDEC output-completion callback Movie_OpenStream
 * installs through DecDCToutCallback: each call uploads the decoded strip and
 * queues the next output or marks the frame complete. Its exact void(void)
 * shape is load bearing. The decode timeout is separate: Movie_WaitFrameDecoded
 * in mdec_sync.c counts down and resets the MDEC itself. */
s32 Movie_StopStream(s32 arg0);
s32 Movie_DecodeAndPresentFrame(void);
s32 Movie_WaitAndDecodeFrame(s32 resync);
void Movie_OnDecodeOutput(void);
void func_8005C374(s32 first, s32 second, s32 third);

/* The pipeline's state block, 0x8009B060 to 0x8009B073 in one run, plus the
 * two stream fields after it. Every symbol below was declared identically by
 * each of its users and none is named outside this family.
 *
 * The eight bytes are flags and small counters: gMovie_bRgb24 is set from
 * Movie_OpenStream's argument and gated on, gMovie_bFadingOut is cleared on start,
 * set by Movie_StartFadeOut and read by the fade path, and gMovie_bFrameDecoded is the
 * frame-decoded latch that the DecDCTout callback Movie_OnDecodeOutput raises and
 * mdec_sync.c's Movie_WaitFrameDecoded waits on and clears.
 * gMovie_dwStartFrame, gMovie_dwEndFrame and gMovie_dwEndSector are the three words that follow.
 *
 * gMovie_pWorkArea is the VLC buffer -- Movie_OpenStream points it at gRuntime_pSharedWorkBase and
 * DecDCTvlcBuild is handed it -- and gMovie_StreamLoc is the stream position, copied
 * from a CdlLOC and passed on by address. This header now includes
 * psyq/libcd.h for that type rather than relying on its consumers having done
 * so; mdec_sync.c had not.
 */
extern u8 gMovie_bRgb24;
extern u8 D_8009B061;
/* Per-frame pipeline state. gMovie_bFrameDecoded is the latch the DecDCTout
 * callback raises after the frame's last strip; gMovie_bStreamEnded and
 * gMovie_bFadingOut end the stream outright or after gMovie_bCdVolume, the CD
 * mix level, falls by 0x40 a frame from 0xFF; gMovie_bInputSlot (0/1) and
 * gMovie_bOutputSlot (0-3) pick the VLC input and decoded output buffers. */
extern u8 gMovie_bFrameDecoded;
extern u8 gMovie_bStreamEnded;
extern u8 gMovie_bFadingOut;
extern u8 gMovie_bCdVolume;
extern u8 gMovie_bInputSlot;
extern u8 gMovie_bOutputSlot;
extern u32 gMovie_dwStartFrame;
extern u32 gMovie_dwEndFrame;
extern u32 gMovie_dwEndSector;
/* The LIBGS active-buffer index and current graph/display dimensions,
 * at 0x800FE0CC-0x800FE0D7.
 *
 * The current LIBGS graph/display width and height each have a 16-bit RECT
 * view and a 32-bit arithmetic view. GraphicsDimension records that overlay
 * directly instead of making each consumer cast the global's address.
 * D_800FE0CC is the halfword GsGetActiveBuff returns and GsSwapDispBuff
 * updates. Movie open/stop set it to 1 before their existing graph/swap setup.
 *
 * All three keep the .data declaration forms both movie units already use.
 * These labels sit far from $gp and retail reaches them with lui/%lo.
 * The active index is SDK-owned state, not a movie-owned reset latch;
 * see notes/movie-handoff-controls.md. */
extern s16 D_800FE0CC __attribute__((section(".data")));
typedef union {
    u16 pixels;
    s32 word;
} GraphicsDimension;

typedef char GraphicsDimension_size_must_be_4[
    sizeof(GraphicsDimension) == 4 ? 1 : -1
];

extern GraphicsDimension gGraphics_CurrentWidth __attribute__((section(".data")));
extern GraphicsDimension gGraphics_CurrentHeight __attribute__((section(".data")));

/* The pending-interrupt word Movie_OnDecodeOutput tests, and clears after calling
 * StCdInterrupt, while the stream is running (gMovie_bRgb24). Four bytes, so at
 * -G8 a plain declaration would land in sdata and both accesses would come out
 * gp-relative; retail reaches it through lui %hi / %lo, which the .data
 * section attribute keeps. Movie_OnDecodeOutput is its only C consumer. */
extern s32 D_800F5D44 __attribute__((section(".data")));

/* The movie work area lives at gMovie_pWorkArea + 0x40000. Only its tail
 * is reached by name here. LoadImage is handed
 * gMovie_pWorkArea + 0x42400 + idx * 8 with idx kept in 0..3 by an
 * and-with-~3, which is what fixes slots at four entries; the two
 * rects that follow are the frame the display is centred on
 * ((screen - w) / 2 on both axes) and the strip the MDEC is filling
 * (w * h / 2 words to DecDCTout). Nothing before 0x2400 is reached
 * through this type, so the head stays an opaque block. The European
 * build keeps its tail 0xC00 further in; its wrappers define the head's
 * size. */
#ifndef MOVIE_WORK_AREA_HEAD_SIZE
#define MOVIE_WORK_AREA_HEAD_SIZE 0x2400
#endif
typedef struct {
    u8 head[MOVIE_WORK_AREA_HEAD_SIZE];
    RECT slots[4];
    RECT frame;
    RECT strip;
} MovieWorkArea;

extern u8 *G32 gMovie_pWorkArea;
extern CdlLOC gMovie_StreamLoc;

#endif
