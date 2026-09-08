#include "../types.h"
#include "sound.h"

/* Builds a 0x30-byte request struct on the stack and forwards it to
   SD_EnqueueCommand: command=41, field_0008=sign-extended low 16 bits of a0,
   field_0002=a1's low byte, field_0001=a2's low byte. */
s32 func_80044FFC(s32 a0, s32 a1, s32 a2) {
    SDCommand s;
    s.command = 41;
    s.field_0008 = (s16) a0;
    s.field_0002 = (s16) (u8) a1;
    s.field_0001 = (u8) a2;
    return SD_EnqueueCommand(&s);
}
