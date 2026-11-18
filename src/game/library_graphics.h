#ifndef MEMORIES_DECOMP_LIBRARY_GRAPHICS_H
#define MEMORIES_DECOMP_LIBRARY_GRAPHICS_H
#include "../types.h"
#include "../ygo_types.h"
#include "card_constants.h"

#define LIBRARY_CARD_FLAG_KNOWN 0x80
#define LIBRARY_CARD_FLAG_UNOWNED 0x01

/* Card IDs index the table directly; entry zero is the empty selection. */
typedef struct {
    s16 clut_x;
    u8 flags;
    u8 pad_03;
} LibraryCardRecord;

typedef struct {
    LibraryMotionState motion;
    struct DisplayObject *selector_sprite;
    struct DisplayObject *card_view;
    struct DisplayObject *loading_icon;
    LibraryCardRecord cards[CARD_ID_END];
} LibraryState;

typedef char LibraryCardRecord_flags_offset_must_be_0x2[
    OFFSETOF(LibraryCardRecord, flags) == 0x2 ? 1 : -1
];
typedef char LibraryCardRecord_size_must_be_0x4[
    sizeof(LibraryCardRecord) == 0x4 ? 1 : -1
];
typedef char LibraryState_selector_sprite_offset_must_be_0x48[
    OFFSETOF(LibraryState, selector_sprite) == 0x48 ? 1 : -1
];
typedef char LibraryState_card_view_offset_must_be_0x4c[
    OFFSETOF(LibraryState, card_view) == 0x4C ? 1 : -1
];
typedef char LibraryState_loading_icon_offset_must_be_0x50[
    OFFSETOF(LibraryState, loading_icon) == 0x50 ? 1 : -1
];
typedef char LibraryState_cards_offset_must_be_0x54[
    OFFSETOF(LibraryState, cards) == 0x54 ? 1 : -1
];
typedef char LibraryState_size_must_be_0xba0[
    sizeof(LibraryState) == 0xBA0 ? 1 : -1
];

extern LibraryState gLibrary_State;

/* Creates the eight retained grid-section sprites. The distinct cursor/render
 * object is initialized separately by Library_InitializeScreen. */
void Library_CreateGridSectionSprites(void);

#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"
#include "../psyq/libgs.h"

/* One vertex of the globe's line packet. The GTE stores its screen position
 * as a single packed SXY word (RotAverage3's sxy outputs, gte_stsxy); the
 * GPU and Library_DrawWireframeGlobe read the halves as x and y. */
typedef union {
    long sxy;
    struct {
        s16 x;
        s16 y;
    } h;
} GlobeLineVertex;

/* One vertex colour of the globe's line packet. Library_DrawWireframeGlobe
 * sets a whole colour (with the byte after it) in one word store; the
 * fourth byte is the GPU code in rgb0 and padding in rgb1 and rgb2. */
typedef union {
    struct {
        u8 r, g, b, code;
    } c;
    u32 word;
} GlobeLineColor;

/* The LINE_G3 at scratchpad 0x1F800000 that Library_DrawWireframeGlobe
 * builds and the span helpers project into: LINE_G3's layout with each
 * vertex position typed as GlobeLineVertex and each colour as
 * GlobeLineColor. */
typedef struct {
    u32 tag;
    GlobeLineColor rgb0;
    GlobeLineVertex xy0;
    GlobeLineColor rgb1;
    GlobeLineVertex xy1;
    GlobeLineColor rgb2;
    GlobeLineVertex xy2;
    u32 pad;
} GlobeLinePacket;

/* Three-point coordinate spans; the other components come from the caller.
 * Negative projection flags cause one retry before unconditional submission. */
void Library_DrawGlobeXSpan(GlobeLinePacket *packet, GsOT *ot, SVECTOR *points,
                  long *control, s32 origin, s32 span);
void Library_DrawGlobeZSpan(GlobeLinePacket *packet, GsOT *ot, SVECTOR *points,
                  long *control, s32 origin, s32 span);

#endif
