#include "../types.h"

typedef struct { u8 pad00[0xF]; s8 x; s8 y; } Cursor;

extern u16 D_8009B162;
extern s8 D_8009B160;
extern s8 D_8009B1D6;
extern s8 D_8009B1D7;
extern s32 func_80024088(Cursor *cursor, s8 dir);

s32 func_8001D5B4(Cursor *cursor)
{
    if (D_8009B162 != 0) { D_8009B160 = -1; return 1; }
    if (func_80024088(cursor, D_8009B160) != 0) { return 1; }
    D_8009B160 = -1;
    if (D_8009B1D7 != cursor->y) {
        D_8009B160 = 1;
        if (D_8009B1D7 < cursor->y) { D_8009B160 = 3; }
        return 1;
    } else if (D_8009B1D6 != cursor->x) {
        D_8009B160 = 0;
        if (D_8009B1D6 < cursor->x) { D_8009B160 = 2; }
        return 1;
    } else {
        return 0;
    }
    return 1;
}
