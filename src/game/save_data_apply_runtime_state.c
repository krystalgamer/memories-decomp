#include "../types.h"

typedef struct {
    u8 pad_000[0x404];
    u32 field_404;
    u32 field_408;
    u8 pad_40C[0x5DC - 0x40C];
    u8 field_5DC;
    u8 field_5DD;
    u8 field_5DE;
} SaveDataRuntimeState;

extern u8 D_801B125A[16];
extern void Text_SjisToGlyphCodes(void *, void *, s32);
extern u32 D_8009B3B8;
extern s8 gSD_bOutputType[16];
extern void SD_SetOutputType(s16);

#define D_8009B0C4 (*(u32 *)0x8009B0C4)
#define D_8009B27A (*(u8 *)0x8009B27A)
#define D_8009B408_write (*(u8 *)0x8009B408)

void SaveData_ApplyRuntimeState(SaveDataRuntimeState *state) {
    Text_SjisToGlyphCodes(D_801B125A, (u8 *)state + 0x40C, 6);

    /*
     * These discarded addresses use the retail assembler-temporary form.
     * Symbolic stores allocate ordinary registers and change exact codegen.
     */
    D_8009B0C4 = state->field_408;
    D_8009B3B8 = state->field_404;
    D_8009B27A = state->field_5DC;

    if (gSD_bOutputType[0] < 0) {
        u8 output_type = state->field_5DE;

        D_8009B408_write = output_type;
        SD_SetOutputType((s8)output_type);
    }
}
