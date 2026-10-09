#include "../../types.h"
#include "../../overlays/french_model_variant/variant450_coils.h"

typedef struct {
    SVECTOR a[17];
    PSXLONG sa[17];
    s32 angle[17];
    SVECTOR b[17];
    PSXLONG sb[17];
    s32 width[17];
    u8 unknown220[0x44];
    u8 color[17][4];
    s32 field_2A8;
    PSXLONG flag[17];
    s32 otz[17];
    s16 ox[17];
    s16 oy[17];
} Variant450NtscStreamer;

typedef char StreamerSize[(sizeof(Variant450NtscStreamer) == sizeof(Model450Coil)
                          && sizeof(Variant450NtscStreamer) == 0x378) ? 1 : -1];
#define CHECK_FIELD(a, b) typedef char StreamerOffset_##a[((u32)&((Variant450NtscStreamer *)0)->a == (u32)&((Model450Coil *)0)->b) ? 1 : -1]
CHECK_FIELD(a, points);
CHECK_FIELD(sa, screen);
CHECK_FIELD(angle, angle);
CHECK_FIELD(b, edges);
CHECK_FIELD(sb, edge_screen);
CHECK_FIELD(width, width);
CHECK_FIELD(color, color);
CHECK_FIELD(field_2A8, field_2A8);
CHECK_FIELD(flag, field_2AC);
CHECK_FIELD(otz, depth);
CHECK_FIELD(ox, x_offset);
CHECK_FIELD(oy, y_offset);
#undef CHECK_FIELD
