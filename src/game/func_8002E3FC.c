#include "../types.h"
#include "display_object_layout.h"

extern void *func_8004006C(void);
extern void *func_800400AC(void *,int);
extern void func_800404CC(void *,int,int,int,int,int,int,int);
extern void func_80042918(void *);
void *func_8002E3FC(void) {
    unsigned char *object = func_800400AC(func_8004006C(),2);
    func_800404CC(object,0x10,0xB0,0,0,0,0xD,0x100);
    *(unsigned short *)(object + 8) |= DISPLAY_OBJECT_FLAG_SCREEN_SPACE;
    func_80042918(object);
    return object;
}
