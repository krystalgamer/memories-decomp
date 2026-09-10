#include "../types.h"
#include "frontend_debug_tables.h"

/* Initialized data at 0x80090CB4 through 0x80090D7C, previously a generated
   blob (#2602).

   Six debug HUD format strings that func_80030250 prints, and the index
   table func_80030FA0 walks. They are one translation unit because they are
   one contiguous range, not because they are one idea.

   Each string carries its explicit size. In the image every one is padded to
   a four-byte boundary -- the first is thirty-seven characters and occupies
   forty bytes -- and a plain u8 name[] = "..." would emit strlen + 1 with no
   padding, because GCC aligns a char array to one byte. The six would then
   pack tightly and move everything after them.

   D_80090CDC and D_80090CF4 have no C declaration anywhere. func_80030998,
   which is generated assembly again, reaches both through R_MIPS_HI16 and
   R_MIPS_LO16 relocations, so their names must survive exactly even though
   nothing declares them. */
u8 D_80090CB4[0x28] = "          SE= %04X BGM=%03X XA= %03X\n";
u8 D_80090CDC[0x18] = "          MSG = %03X\n";
u8 D_80090CF4[0x18] = "          CAMP = %02X\n";
u8 D_80090D0C[0x1C] = "             MAP = %02X\n";
u8 D_80090D28[0x1C] = "             MENU = %02X\n";
u8 D_80090D44[0x24] = "                          = %02X\n";

/* Indexed by D_8009B2F1 in func_80030FA0. */
u8 D_80090D68[0x14] = {
    0x01, 0x02, 0x03, 0x04, 0x00, 0x00, 0x05, 0x00,
    0x06, 0x08, 0x09, 0x0A, 0x00, 0x00, 0x00, 0x00,
    0x0B, 0x0C, 0x0D, 0x00,
};
