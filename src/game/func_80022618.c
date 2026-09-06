#include "../types.h"
#include "../psyq/libgte.h"
#include "../psyq/libgpu.h"

/* "~c0702D EFFECT = %2d %2d\n" */
extern u8 D_80010074[];
/* "               **\n~c777\0" */
extern u8 D_80010090[];
/* "            **\n~c777\0" */
extern u8 D_800100A8[];
extern u8 D_8009AF2A;
extern u8 D_8009AF2C;
extern u8 D_8009AF2D;
extern void func_800222F4(void);

/* Prints the "EFFECT = %2d %2d" debug line, then one of two divider strings
   depending on D_8009AF2A. */
void func_80022618(void) {
    u8 v0;

    func_800222F4();
    FntPrint(D_80010074, D_8009AF2C, D_8009AF2D);
    v0 = D_8009AF2A;
    if (v0 != 0) {
        FntPrint(D_80010090);
    } else {
        FntPrint(D_800100A8);
    }
}
