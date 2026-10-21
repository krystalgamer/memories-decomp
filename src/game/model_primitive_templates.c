#include "../types.h"
#include "model_primitive_templates.h"

/* The model renderer's prototype GPU primitive words. See the header for what
 * the opcode and colour fields are, and for what is deliberately not claimed
 * about which handler uses which word.
 *
 * Each object carries an explicit .sdata section attribute. The last three
 * words are zero, and a zero-initialised object would otherwise be placed in
 * .sbss; the attribute is what keeps them in .sdata where retail has them.
 * save_data_mask_state.c spells its two words the same way. */
u32 gModel_dwPolyFt3Template __attribute__((section(".sdata"))) = 0x24808080;
u32 gModel_dwPolyFt4Template __attribute__((section(".sdata"))) = 0x2C808080;
u32 gModel_dwPolyGt3Template __attribute__((section(".sdata"))) = 0x34808080;
u32 gModel_dwPolyGt4Template __attribute__((section(".sdata"))) = 0x3C808080;
u32 gModel_dwPolyFt3SemiTemplate __attribute__((section(".sdata"))) = 0x24404040;
u32 gModel_dwPolyFt4SemiTemplate __attribute__((section(".sdata"))) = 0x2C404040;
u32 gModel_dwPolyGt3SemiTemplate __attribute__((section(".sdata"))) = 0x34404040;
u32 gModel_dwPolyGt4SemiTemplate __attribute__((section(".sdata"))) = 0x3C404040;
u32 gModel_dwLineF3Template __attribute__((section(".sdata"))) = 0x4A808080;
u32 gModel_dwLineF4Template __attribute__((section(".sdata"))) = 0x4E808080;
u32 D_8009AFD4 __attribute__((section(".sdata"))) = 0xE1000220;
u32 D_8009AFD8 __attribute__((section(".sdata"))) = 0x00000000;
u32 D_8009AFDC __attribute__((section(".sdata"))) = 0x00000000;
u32 D_8009AFE0 __attribute__((section(".sdata"))) = 0x00000000;
