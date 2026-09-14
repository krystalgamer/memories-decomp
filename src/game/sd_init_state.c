#define D_8009B0F0_IN_DATA
#define D_8009B120_IN_DATA
#include "../types.h"
#include "../ygo_types.h"

#include "file_transfer.h"
#include "sound.h"
#include "sound_buffer_init.h"
#include "sound_init.h"
#include "sound_output_transition.h"
#include "sound_transfer_lifecycle.h"
#include "sound_voice_data.h"
#include "sound_voice_selection.h"

extern u8 D_80010784[];
extern u8 D_80010790[];
extern u8 D_8001079C[];

/* Brings the sound subsystem's resident state up from cold. The state block
 * starts at the end of the loaded image and runs to 0x801EA7FF; it is zeroed a
 * word at a time, the two command callbacks are installed, three tables are
 * copied in from ROM, the mode byte is seeded (0xF0 extra bits when arg0 is
 * set), the mix defaults are seeded from the values just written at +0x1649
 * and +0x164A, and the subsystem initialisers run before the secondary state's
 * tick callback is armed.
 *
 * The image bounds are plain integer constants rather than `&loaded_image_end`
 * (which the linker also puts at 0x801E0000): the retail code derives
 * 0x801EA7FF from the same `lui` as the base with an `ori`, which only happens
 * when both are integers GCC can share a high half between. Through the symbol
 * they become separate relocations and the `ori` becomes a second `lui`. */
void SD_InitState(u8 arg0)
{
    u32 *p;
    SDValue *st;
    SDValue *st2;
    SDSecondaryState *sec;
    SDValue *q;
    s32 x;
    s32 y;

    g_SDValue = (SDValue *)0x801E0000;
    D_8009B460 = (FileRequestSlot *)0x801E1650;
    p = (u32 *)0x801E0000;
    do {
        *p = 0;
        p++;
    } while (p <= (u32 *)0x801EA7FF);
    D_8009B0F0 = func_8004666C;
    D_8009B120 = func_800466C8;
    *(SDInitBlk11 *)g_SDValue->field_1619 = *(SDInitBlk11 *)D_80010784;
    *(SDInitBlk10 *)g_SDValue->field_1629 = *(SDInitBlk10 *)D_80010790;
    *(SDInitBlk10 *)g_SDValue->field_1639 = *(SDInitBlk10 *)D_8001079C;
    g_SDValue->flags_004A = 3;
    if (arg0 != 0) {
        g_SDValue->flags_004A |= 0xF0;
    }
    g_SDValue->field_1649 = 0xFF;
    g_SDValue->field_164A = 0xD2;
    g_SDValue->field_164B = 0xFF;
    st = g_SDValue;
    x = st->field_1649;
    y = st->field_164A;
    st->field_0049 = 0xFF;
    q = g_SDValue;
    st->mix_scale = x;
    st->field_0044 = y;
    q->field_1584 = 0xFF;
    func_800494F4((s32 *)0x801E1670);
    func_8004671C();
    func_80044D48();
    func_80048F14();
    SD_SetOutputType(0);
    D_8009B458->field_0509 = 0;
    sec = D_8009B458;
    st2 = g_SDValue;
    sec->field_050C = func_800478EC;
    st2->flags_0040 = 0;
}
