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
 * func_8005C1F4 is installed as a Psy-Q decoder timeout callback through
 * DecDCToutCallback, so its exact void(void) shape is load bearing. */
s32 Movie_StopStream(s32 arg0);
s32 Movie_DecodeAndPresentFrame(void);
s32 Movie_WaitAndDecodeFrame(s32 resync);
void func_8005C1F4(void);
void func_8005C374(s32 first, s32 second, s32 third);

/* The pipeline's state block, 0x8009B060 to 0x8009B073 in one run, plus the
 * two stream fields after it. Every symbol below was declared identically by
 * each of its users and none is named outside this family.
 *
 * The eight bytes are flags and small counters: D_8009B060 is set from
 * func_8005B8A0's argument and gated on, D_8009B064 is cleared on start,
 * set by Movie_StartFadeOut and read by the fade path, and D_8009B062 is the
 * frame-decoded latch that the DecDCTout callback func_8005C1F4 raises and
 * mdec_sync.c's Movie_WaitFrameDecoded waits on and clears.
 * D_8009B068, D_8009B06C and D_8009B070 are the three words that follow.
 *
 * D_8009B498 is the VLC buffer -- func_8005B8A0 points it at D_80010000 and
 * DecDCTvlcBuild is handed it -- and D_8009B49C is the stream position, copied
 * from a CdlLOC and passed on by address. This header now includes
 * psyq/libcd.h for that type rather than relying on its consumers having done
 * so; mdec_sync.c had not.
 */
extern u8 D_8009B060;
extern u8 D_8009B061;
extern u8 D_8009B062;
extern u8 D_8009B063;
extern u8 D_8009B064;
extern u8 D_8009B065;
extern u8 D_8009B066;
extern u8 D_8009B067;
extern u32 D_8009B068;
extern u32 D_8009B06C;
extern u32 D_8009B070;
/* The decoded frame's rectangle and the resize latch, at 0x800FE0CC-0x800FE0D7.
 *
 * The current LIBGS graph/display width and height each have a 16-bit RECT
 * view and a 32-bit arithmetic view. GraphicsDimension records that overlay
 * directly instead of making each consumer cast the global's address.
 * D_800FE0CC is set to 1 on the paths that change the rectangle.
 *
 * All three keep the .data section attribute both sources already wrote, and
 * both write it: these sit at 0x800FE0xx, far from $gp, and the retail image
 * reaches them with lui/%lo. Unlike the guarded symbols elsewhere in this
 * tree there is no second group to serve -- no other source names them --
 * so one spelling carries. */
extern s16 D_800FE0CC __attribute__((section(".data")));
typedef union {
    u16 pixels;
    s32 word;
} GraphicsDimension;

typedef char GraphicsDimension_size_must_be_4[
    sizeof(GraphicsDimension) == 4 ? 1 : -1
];

extern GraphicsDimension gGraphics_CurrentWidth asm("D_800FE0D0")
    __attribute__((section(".data")));
extern GraphicsDimension gGraphics_CurrentHeight asm("D_800FE0D4")
    __attribute__((section(".data")));

/* The pending-interrupt word func_8005C1F4 tests, and clears after calling
 * StCdInterrupt, while the stream is running (D_8009B060). Four bytes, so at
 * -G8 a plain declaration would land in sdata and both accesses would come out
 * gp-relative; retail reaches it through lui %hi / %lo, which the .data
 * section attribute keeps. func_8005C1F4 is its only C consumer. */
extern s32 D_800F5D44 __attribute__((section(".data")));

/* The movie work area lives at D_8009B498 + 0x40000. Only its tail
 * is reached by name here. LoadImage is handed
 * D_8009B498 + 0x42400 + idx * 8 with idx kept in 0..3 by an
 * and-with-~3, which is what fixes slots at four entries; the two
 * rects that follow are the frame the display is centred on
 * ((screen - w) / 2 on both axes) and the strip the MDEC is filling
 * (w * h / 2 words to DecDCTout). Nothing before 0x2400 is reached
 * through this type, so the head stays an opaque block. */
typedef struct {
    u8 head[0x2400];
    RECT slots[4];
    RECT frame;
    RECT strip;
} MovieWorkArea;

extern u8 *D_8009B498;
extern CdlLOC D_8009B49C;

#endif
