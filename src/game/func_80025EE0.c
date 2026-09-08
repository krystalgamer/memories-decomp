#include "../types.h"
#include "sound.h"

typedef struct {
    u16 x;
    u16 y;
    u8 pad_04[0x16];
    u16 field_1A;
} Object;

extern u16 D_8009B220;
extern int func_80024E24(void);
extern Object *func_8002C68C(int);
void func_80025EE0(void)
{
    Object *object;

    if (func_80024E24() == 0) {
        object = func_8002C68C(0x12);
        object->x = 0xA0;
        object->y = 0x78;
        object->field_1A = 1;
        SD_SEPlayFull(2);
    } else {
        D_8009B220 = 0;
    }
}
