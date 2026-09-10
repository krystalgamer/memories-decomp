#include "../types.h"
#include "data_transfer_request.h"
#include "card_constants.h"
#include "func_8003FCD8.h"
#include "mem_card.h"
#include "save_data.h"
#include "text_sjis_to_glyph_codes.h"
#include "two_player_save_setup.h"

extern u8 D_801D1200[];
extern s32 D_801D5608[];
extern u8 D_801D160C[];
extern u8 D_801B122B[];
extern u8 D_801B1238[];
s32 func_8003FD14(void)
{
    s32 result;
    s32 i;
    u8 *left;
    u8 *right;
    s32 *state;

    if ((D_8009B3ED & 128) == 0) {
        D_8009B3ED |= 128;
        D_8009B3C0 = 40;
    }
    result = func_8003FCD8();
    if (result == 1) {
        left = D_801D1200;
        right = D_801D1200 + TWO_PLAYER_SAVE_SLOT_STRIDE;
        i = 0;
        state = D_801D5608;
        D_8009B3EA = 10;
        D_8009B3C0 = 36;
        for (; i < DECK_SIZE; i++, left += 2, right += 2) {
            if (*(u16 *)left == 0) {
                state[16] = 1;
                return 0;
            }
            if (*(u16 *)right == 0) {
                state[16] = 2;
                return 0;
            }
        }
        Text_SjisToGlyphCodes(
            D_801B122B,
            D_801D160C,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
        Text_SjisToGlyphCodes(
            D_801B1238,
            D_801D160C + TWO_PLAYER_SAVE_SLOT_STRIDE,
            SAVE_DATA_PLAYER_NAME_CHAR_COUNT
        );
    }
    return result;
}

extern u8 D_801D1880[];
extern void *D_8009B3E0;

void func_8003FE14(void)
{
    u8 *p = D_801D1880;
    u8 *q = p + TWO_PLAYER_SAVE_SLOT_STRIDE;

    SaveData_WritePrimarySecondaryIntegrity(p);
    SaveData_WritePrimarySecondaryIntegrity(q);
    D_8009B3E0 = q;
    func_8003F758(
        p,
        TWO_PLAYER_SAVE_TRANSFER_SIZE,
        gMemCard_szSaveFileName,
        4
    );
}
