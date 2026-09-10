#include "../types.h"
#include "display_object.h"
#include "display_object_brightness.h"

/* Element 0 is the object whose brightness these two set. The array
   shape is func_80031354.c's, which is the better-informed view; this
   file used to declare the same address as a bare u8 * and reach
   bytes 12 to 14 of whatever it pointed at. Those are the low three
   bytes of DisplayObject's field_0C, the colour word.

   The declaration is repeated rather than shared: the only header
   both files could take it from is display_object.h, which fifteen
   overlay files include, so putting it there is cross-module. Same
   reason D_800E9EF0 stayed put in #3531. */
extern DisplayObject *D_800EB184[4];

void func_80030090(void)
{
    ((u8 *)D_800EB184[0])[12] = ((u8 *)D_800EB184[0])[13] =
        ((u8 *)D_800EB184[0])[14] = 0x40;
}

void func_800300AC(void)
{
    ((u8 *)D_800EB184[0])[12] = ((u8 *)D_800EB184[0])[13] =
        ((u8 *)D_800EB184[0])[14] = 0x80;
}
