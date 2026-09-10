#include "../../types.h"
#include "shop.h"
#include "../../game/duel_effect.h"
#include "../../game/func_8003B6AC.h"
#include "../../game/text_constants.h"
#include "../../game/text_box_runtime.h"

extern u8 D_801B1245[];
extern s32 D_801D5608;
extern void *func_80035BE4(s32, s32, s32, s32, s32, s32);

void Password_RefreshDigitDisplay(void)
{
    DuelEffectChannel *boxes;
    u8 *out;
    s32 i;
    s32 glyph;

    out = D_801B1245;
    for (i = 0; i < 8; i++) {
        glyph = D_800EAFF8[gPassword_abDigits[i]];
        if (glyph >= TEXT_SINGLE_BYTE_GLYPH_LIMIT) {
            /* The bare -0x10 is deliberate. It applies the same top-nibble
               marker that TEXT_SINGLE_BYTE_GLYPH_LIMIT tests for, but writing
               | TEXT_SINGLE_BYTE_GLYPH_LIMIT here is not equivalent: the two
               agree only after truncation to u8 and differ as int
               expressions, which GCC 2.8.1 can turn into different
               instructions. Do not "finish" this substitution. */
            out[0] = (glyph >> 8) | -0x10;
            out[1] = glyph;
            out += 2;
        } else {
            out[0] = glyph;
            out += 1;
        }
    }
    *out = TEXT_STRING_TERMINATOR;
    func_8003B6AC(2, 1);
    func_80035BE4(2, 0xFD, 0xA8, 0x68, 0xA0, 0x10);
    boxes = D_800EB0F8;
    boxes[2].field_5A = 0x10;
    boxes[2].field_5B = 0x10;
    func_80039A14((u8 *)&boxes[2]);
}

void Password_RefreshStarchipDisplay(void)
{
    DuelEffectChannel *boxes;

    D_801D5608 = gLibrary_dwStarchips;
    func_8003B6AC(3, 1);
    func_80035BE4(3, 0xE1, 0x98, 0x28, 0xA0, 0x20);
    boxes = D_800EB0F8;
    boxes[3].field_5A = 0x10;
    boxes[3].field_5B = 0x10;
    func_80039A14((u8 *)&boxes[3]);
}
