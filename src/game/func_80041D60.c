#define D_8009B0D8_IS_HALFWORD
#include "../types.h"
#include "func_80041D60.h"
#include "graphics_frame.h"

/* If arg0's 0x10 state bit isn't set yet: marks it set, resets f58/f5A,
   then walks a 3-level little-endian-u16 offset chain through arg0->f54's
   byte buffer (indexed at each level by f67, f68, f69) to resolve a final
   pointer. That pointer and a copy of f4 (masked to clear bit 0x00800000,
   restored if f8's 0x100 bit is set) become the args for func_80041C8C,
   which always fires on this path. Otherwise (0x10 already set): if
   neither of f8's low two bits are set and the f5A cooldown is nonzero,
   decrements it by D_8009B0D8 and fires func_80041C8C once it reaches
   zero or below.

   arg1 and arg2 are read, not just written. The first-time path assigns
   both before jumping to the call, but the cooldown path falls into the
   same call with neither assigned, so on that path they are whatever the
   caller left in $a1 and $a2. All three callers pass one argument, so on the
   cooldown path func_80041C8C receives two values the caller never
   supplied. That is what the retail image does. Declaring the real
   three-argument prototype at either call site makes the call a
   constraint violation, and passing arguments to satisfy it adds the
   register setup and breaks the match, so both callers keep their
   one-argument declarations on purpose.

   The three chain steps use separately-named pointer/offset locals
   (p1/off1, p2/off2, p3/off3) rather than one reused pair -- reusing a
   single pair compiles to the same logic but gcc allocates a spare
   register (a0) for the 2nd/3rd steps instead of matching the ROM's
   plain v0/v1 reuse. */
struct Obj {
    char pad0[0x4];
    s32 f4;
    u16 f8;
    char pad1[0x50 - 0xA];
    s32 f50;
    u8 *f54;
    u16 f58;
    s16 f5A;
    char pad3[0x67 - 0x5C];
    u8 f67;
    u8 f68;
    u8 f69;
};

extern void func_80041C8C(struct Obj *a0, s32 a1, s32 a2, struct Obj *a3);

void func_80041D60(struct Obj *arg0, s32 arg1, s32 arg2) {
    u8 *base;
    u8 *p;

    if (!(arg0->f8 & 0x10)) {
        u8 *p1, *p2, *p3;
        s32 off1, off2, off3;
        s32 mask;

        base = arg0->f54;
        arg0->f8 = arg0->f8 | 0x10;
        arg0->f58 = 0;
        arg0->f5A = 1;

        p1 = base + arg0->f67 * 2;
        off1 = p1[1] << 8;
        off1 |= p1[0];
        p2 = base + off1 + arg0->f68 * 2;
        off2 = p2[1] << 8;
        off2 |= p2[0];
        p3 = base + off2 + arg0->f69 * 2;
        off3 = p3[1] << 8;
        off3 |= p3[0];
        p = base + off3;

        mask = arg0->f4 & 0xFF7FFFFF;
        arg1 = (s32)p;
        arg2 = mask;
        arg0->f4 = mask;
        arg0->f50 = (s32)p;
        if (arg0->f8 & 0x100) {
            arg0->f4 = mask | 0x800000;
        }
        goto call_it;
    }

    if (!(arg0->f8 & 3) && arg0->f5A != 0) {
        s16 remain = (u16)arg0->f5A - D_8009B0D8;
        arg0->f5A = remain;
        if (remain <= 0) {
call_it:
            func_80041C8C(arg0, arg1, arg2, arg0);
        }
    }
}
