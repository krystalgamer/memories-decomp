## `func_8004A0FC` at 0x8004A0FC

`gcc_2_8_1_g0`, 96 of 96 instructions, opcode distance 0, 39 differing
positions, first difference at +0xA4.

Sound spatialisation for one secondary object, called from
`func_8004A2F8` as `func_8004A0FC(state + object_offset, state + value * 24)`,
so its two arguments are an `SDSecondaryObject` and an `SDSecondaryRecord`. It
forms a pan value, clamps it to 0..0x7F, derives a level from the transfer and
record scalars, splits the level into left and right channels about the 0x40
centre, and writes both back scaled by the two `field_07E4`/`field_07E6` trims
and the object's 7-bit `field_000E`.

All six canonical attempts were profile sweeps of one reference source that is
not in this tree; none was a reconstruction and the best differed at +0x0.
Everything below is new, and the first 41 instructions are byte-exact.

**Field names.** This entry needs eight members that `sound.h` had inside
`SDSecondaryObject`'s pads, so the pads are split in this branch: `field_0008`
and `field_0009` are the two level multipliers, `field_000A`/`field_000B` the
two pan contributions, `field_000C` the clamped pan, `field_000E` the 7-bit
output trim, and `field_0014`/`field_0016` the two `u16` outputs. The struct is
still 0x28 and `make match` is unchanged.

**What is settled.** Four shapes were each worth a measurable step and should
not be rewritten:

- The state pointer must **not** be a local. `SDSecondaryState *state =
  D_8009B458;` puts the pointer in `$a0`, which forces the parameter copy
  `addu $a3, $a0, $zero` to the top of the function; the target has that copy in
  the first branch's delay slot. Writing `D_8009B458->...` at each of the three
  places emits the same three loads and fixes the whole prologue. Worth 36 of
  the 91 positions the first reconstruction had.
- The pan test is `if (state->field_0815 != 0)`, and the three-way channel split
  has to be spelled `if (pan >= 0x40) { if (pan == 0x40) ... } else { ... }`.
  GCC emits `slti $v0, $a2, 0x40` either way, but only this nesting puts the
  low-pan arm out of line, which is where the target has it.
- Inside the `>= 0x40` arms, `right` is assigned **before** `left`, and the
  equal case is `left = right`, not `left = level`. That is what lets the
  `right = level` copy sink into the `bne` delay slot and be shared by both
  arms.
- The level chain is three statements: the four-factor product, then
  `((level >> 14) * field_0008 * field_0009) >> 14`. Splitting the shift into
  its own statement, or folding the whole chain into one expression, each costs
  two instructions.

**What is left** is one register-allocation family. At +0xA4 the target takes
the four-factor product into `$v1` and shifts it in place; this build takes it
into `$a0` and shifts into `$v0`. Every later difference follows from that: the
level chain, the `left`/`right` pair, and the two output chains are all the same
instructions with the same operands in permuted registers, which is why the
opcode distance is 0.

The tail has a second, smaller consequence. The target issues both
`left * field_07E4` and `right * field_07E6` before scaling either, so both
`lbu 0xE($a3)` reads happen before the first `sh`. Written as two independent
statements this build emits them sequentially; written as two temporaries stored
afterwards, GCC common-subexpressions the two `field_000E & 0x7F` reads into one
and the function comes out three instructions short. The target has both reads
and no CSE, so the sequential spelling is right and the interleave has to come
from the scheduler.

Crossed without moving it, about forty variants: five spellings of the state
pointer, eight arrangements of the three-way split, six of the level chain,
`register` pins on `level`/`left`/`right`/`pan` singly and in combination (every
pin removes a copy the target has and comes out short), two spellings of the
output tail, and all eighteen viable profiles -- every one of which measures the
same 45 or worse, with `gcc_2_8_1_g0` and its cohort tied at 39.

```c
#include "../types.h"
#include "sound.h"


void func_8004A0FC(SDSecondaryObject *object, SDSecondaryRecord *record)
{
    s32 pan;
    s32 level;
    s32 left;
    s32 right;

    if (D_8009B458->field_0815 != 0) {
        pan = 0x40;
    } else {
        pan = D_8009B458->transfer.field_001B + object->field_000A + object->field_000B
            + record->field_0001 - 0xC0;
    }
    if (pan < 0) {
        pan = 0;
    }
    if (pan >= 0x80) {
        pan = 0x7F;
    }
    object->field_000C = pan;

    level = D_8009B458->transfer.field_0018 * (u16)D_8009B458->field_0512
          * record->field_0005 * record->field_0003;
    level = ((level >> 14) * object->field_0008 * object->field_0009) >> 14;

    if (pan >= 0x40) {
        if (pan == 0x40) {
            right = level;
            left = right;
        } else {
            right = level;
            left = ((0x40 - (pan & 0x3F)) * (level * 2)) >> 7;
        }
    } else {
        left = level;
        right = (pan * (level * 2)) >> 7;
    }

    object->field_0014 = (((left * D_8009B458->field_07E4) >> 7) * (object->field_000E & 0x7F)) >> 7;
    object->field_0016 = (((right * D_8009B458->field_07E6) >> 7) * (object->field_000E & 0x7F)) >> 7;
}
```
