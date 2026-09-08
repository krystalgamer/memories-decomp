## `func_8004ADE8` at 0x8004ADE8

`gcc_2_8_1_cc_g8_as_g0`, **355 of 355 instructions, opcode multiset distance 4**
(two `nop` where retail has one `addu` and one `sll`), 160 differing words.
Written from scratch: no stored candidate, no rows in `external_attempts.csv`,
empty inventory note.

The secondary sound driver's note-on. It is a plain VAB lookup followed by an
SPU key-on, repeated once per tone in the program, and every structure it walks
is a documented Psy-Q one, which is what makes the body reconstructible:

- `D_8009B458->transfer.field_0004` (`+0x4A8`) is the **VAB header**. The
  program records are the 16-byte entries at `+0x20`, the tone records are the
  32-byte `VagAtr` at `+0x820`, and `hdr[0x12]` is `VabHdr.ps`, so
  `+0x820 + ps * 512` is the start of the VAG size table. The function sums the
  first `tone->vag` of those `u16` sizes and shifts left by 3 to get the sample
  offset, which it adds to `transfer.field_0014` (`+0x4B8`) to form the SPU
  address. This is the standard `SsUtGetVagAtr`-era address computation.
- The tone fields it reads are exactly `VagAtr`'s: `min`/`max` at `+6`/`+7`
  gate the note range, `vol`/`pan` at `+2`/`+3`, `center`/`shift` at
  `+4`/`+5` feed the pitch conversion, `pbmin`/`pbmax` at `+0xC`/`+0xD`,
  `adsr1`/`adsr2` at `+0x10`/`+0x12`, `vag` at `+0x16`, and `mode` at `+1`
  whose bit 2 selects reverb.
- `pad04C0[0x40]` in `SDSecondaryState` is a **`SpuVoiceAttr`**. Every offset
  checks out against the Psy-Q layout: `voice` `0x4C0`, `mask` `0x4C4`,
  `volume` `0x4C8`/`0x4CA`, `volmode` `0x4CC`/`0x4CE`, `pitch` `0x4D4`,
  `addr` `0x4DC`, `a_mode` `0x4E4`, `adsr1`/`adsr2` `0x4FA`/`0x4FC`. The mask
  it writes is `0x6019F` = `VOLL|VOLR|VOLMODEL|VOLMODER|PITCH|WDSA|ADSR_AR`
  plus `ADSR_ADSR1|ADSR_ADSR2`, which is precisely the set of fields the
  function fills before calling `SpuSetKeyOnWithAttr`.
- `field_0514`/`field_0516` are the per-side volume scalars: the two
  `SpuVoiceAttr` volumes are `object->field_0014 * state->field_0514 >> 7` and
  `object->field_0016 * state->field_0516 >> 7`.

Voice allocation goes through the two matched neighbours. When
`rec[6] & 0xF0` is set and the low nibble has reached that limit, it first
tries `func_8004A854` to steal a voice already owned by the channel, and only
falls back to `func_8004A940`. The `stolen` flag it sets there is what
suppresses the `rec[6]` low-nibble increment afterwards, so the count only
grows when a genuinely new voice was taken. `func_80020D4C`-style per-frame
work is then installed the same way the duel code does it.

The reverb tail is four copies of one loop: set `SpuSetReverbVoice` and spin on
`SpuGetReverbVoice` until the bit agrees. `rec[0x10] == 0` defers to the tone's
`mode & 4`; `rec[0x10] == 1` forces reverb off and anything else forces it on.

### Profile

`cc_g8_as_g0`: compile `-G8`, assemble `-G0`. `D_8009B458` is 0x550 past `gp`
and would be `%gp_rel` at `as -G8`, but retail loads it with a `lui`/`lw` pair
every time, seven times over. `-msplit-addresses` makes no difference to the
emitted bytes here -- every `%hi`/`%lo` pair in the target is adjacent and in
one register, i.e. the plain assembler macro -- so `cc_g8_as_g0` and
`cc_g8_as_g0_split` are byte-identical on this source. The matched neighbours
`func_8004AAFC` and `func_8004B374` are on the same profile.

### Levers that got it here, in the order they paid

- *Unsigned compares.* `key` and `tidx` must be `u32`. The note-range tests
  against `tone[6]`/`tone[7]` and the loop test against `prog[0]` are `sltu` in
  the target; declaring them signed gives `slt` and costs three positions.
- *Do not cache `D_8009B458`.* Reading the global inline at each use, rather
  than through one local, is worth eight `lw` and twelve load-delay `nop`.
  Retail loads it seven times.
- *Rematerialise `&D_80011434[idx]` per reverb arm.* A function-scope pointer
  assigned in all four arms gets hoisted to one `lui` and loses four
  materialisations; the bare subscript `D_80011434[idx]` overshoots to nine.
  A **block-scoped** `s32 *m` inside each arm gives exactly retail's five.
  This alone took the build from 359 to 356.
- *Fold the constant into the index, not onto the base.* `vab + program * 0x10
  + 0x20` groups as `(vab + program * 0x10) + 0x20` and puts the `addiu` after
  the `addu`, which leaves a load-delay slot empty. `&vab[program * 0x10 +
  0x20]` emits `addiu` then `addu`, which is what retail has. The same fix
  applies to the tone address and to `obj`. Not to the VAG size table, where
  retail really does add the base first.
- *Hoist the size-table pointer out of its guard.* Retail computes it and `obj`
  before the `blez` on `tone->vag`, not inside the loop.
- *Two `func_8004A940` call sites, not one.* Retail cross-jumps them: the arm
  reached after a failed `func_8004A854` passes `ch` in `$s6`, and the arm
  reached when the steal was never attempted recomputes `channel & 0xFF`.
  Writing one shared call emits `andi $a0, $s7, 0xFF` twice.
- *One store for `a_mode`, not one per arm.* `if (...) amode = 1; else
  amode = 5;` then a single store; a store in each arm costs an extra `j` and
  `sw`.
- *Two inverted arms.* `if (idx == -1)` rather than `!= -1` around the steal,
  and `else if (rec[0x10] != 1)` rather than `== 1` in the reverb chain. Both
  are the "write the fallthrough arm second" shape; each was one
  `beqz`/`bnez` flip.
- *One forced reload of `D_8009B458`.* Retail re-reads the pointer for
  `obj[0x24] = attr.a_mode` instead of reusing the base it has just stored
  through. GCC will not reload it however the expression is written, so the
  entry declares a second name for the symbol,
  `extern u8 *D_8009B458_r asm("D_8009B458");`, the idiom already used for
  `D_8009B0F4_array` in `func_80049920.c`. That is the last three instructions
  and is what makes the count exact.

### What is left

Two things, both allocation rather than structure.

- **`channel` needs `$s7`, and only a pin puts it there.** Retail copies the
  incoming argument into a saved register (`addu $s7, $a0, $zero`) and
  recomputes `channel & 0xFF` from it four times; every pure-C form tried
  leaves the parameter in its incoming home slot at `88($sp)` and reloads it,
  which costs four instructions and pushes `stolen` and `level` into registers
  that retail keeps on the stack. The entry therefore carries one
  `register s32 channel asm("$23")`. It is verifiably safe -- `$s7` has exactly
  eight uses in the build, the same eight retail has, and nothing else is
  allocated to it -- but it is a pin, so this is a candidate and not a match.
  Copying the parameter to a plain local first does not work: GCC coalesces the
  copy. Pinning more registers is strictly worse: pinning `key`, `tidx`, `ch`
  and `prog` as well reaches 357, and pinning the four pointers too reaches
  359. **`asm("$fp")` must not be used** -- GCC honours it for the variable but
  then also allocates `stolen` to `$s8`, with overlapping live ranges, and
  silently miscompiles.
- **Four saved registers are permuted.** With the one pin the build allocates
  `tone`/`idx` to `$s1`/`$s2` where retail has `$s2`/`$s1`, and `key`/`tidx`/
  `ch` to `$s6`/`$s4`/`$s5` where retail has `$s4`/`$s5`/`$s6`. The definition
  order and the reference counts already agree, so this is the allocator's
  priority tie-break, not a source shape. That permutation is most of the 160
  differing words; the opcode stream itself aligns everywhere except the two
  `nop`.

The two remaining `nop` are in the `&objects[idx]` computation that retail
interleaves into the object field-copy block to fill load-delay slots. Applying
the `&arr[...]` grouping to the `func_8004A0FC` arguments moves them but costs
an instruction elsewhere (354 of 355, and 271 differing), so it is not the fix.

### Layout notes for whoever integrates this

`SDSecondaryObject` in `src/game/sound.h` needs layout-preserving names for the
bytes this function writes: `+0x00`, `+0x02`, `+0x04`, `+0x05`, `+0x06`,
`+0x0D` (currently `pad000D`), `+0x10` through `+0x13` (currently `pad0010[4]`),
`+0x1A` and `+0x1C` (inside `pad0018[6]`), and `+0x20`, `+0x22`, `+0x24`
(inside `pad0020[8]`). `SDSecondaryState`'s `pad04C0[0x40]` should become a
`SpuVoiceAttr`. Every existing size and offset assertion still holds; nothing
in the header changes size.

### Source

```c
#include "../types.h"
#include "../psyq/libspu.h"
#include "sound_sequence_constants.h"

extern u8 *D_8009B458;
extern s32 D_80011434[];
/* Second name for the same symbol: retail re-reads the pointer here rather
   than reusing the base it just stored through. */
extern u8 *D_8009B458_r asm("D_8009B458");

extern s32 func_8004A854(s32);
extern s32 func_8004A940(s32, s32);
extern void func_8004A0FC(u8 *, u8 *);
extern s32 func_8004A3BC(u8 *, s32);
extern s32 func_80049FB4(s32, s32, s32, s32);

void func_8004ADE8(s32 arg0, s32 note, u8 velocity)
{
    register s32 channel asm("$23");
    u8 *prog;
    s32 ch;
    u32 tidx;
    u32 key;
    u8 *rec;
    u8 *vab;
    u8 *hdr;
    u8 *tone;
    u8 *obj;
    s32 idx;
    s32 used;
    s32 i;
    s32 sum;
    s32 vag;
    u16 *sizes;
    u8 program;
    u8 stolen;
    s32 level;
    s32 pitch;
    s32 amode;
    s32 *voice;
    u16 adsr1;
    u16 adsr2;

    channel = arg0;
    rec = D_8009B458 + (channel & 0xFF) * SD_SEQUENCE_CHANNEL_RECORD_SIZE;
    hdr = *(u8 **)(D_8009B458 + 0x4A8);
    program = rec[0];
    vab = hdr;
    used = 0;
    i = 0;
    if (program != 0) {
        do {
            if (vab[i * 0x10 + 0x20] != 0) {
                used++;
            }
            i++;
        } while (i < program);
    }
    prog = &vab[program * 0x10 + 0x20];
    if (prog[0] == 0) {
        return;
    }
    tidx = 0;
    key = note & 0xFF;
    ch = channel & 0xFF;
    level = note & 0x7F;
    do {
        tone = &vab[(used * 16 + (tidx & 0xFFFF)) * 32 + 0x820];
        if (key < tone[6]) {
            goto next;
        }
        if (tone[7] < key) {
            goto next;
        }
        {
            u32 limit = rec[6] & 0xF0;

            stolen = 0;
            if (limit != 0) {
                if ((rec[6] & 0xF) >= (limit >> 4)) {
                    idx = func_8004A854(ch);
                    if (idx == -1) {
                        idx = func_8004A940(ch, key);
                        goto have;
                    }
                    stolen = 1;
                    goto have;
                }
            }
        }
        idx = func_8004A940(channel & 0xFF, key);
    have:
        if (idx == -1) {
            goto next;
        }
        i = 0;
        sum = 0;
        vag = *(s16 *)(tone + 0x16);
        sizes = (u16 *)(*(u8 **)(D_8009B458 + 0x4A8)
                        + (*(u16 *)(hdr + 0x12)) * 512 + 0x820);
        obj = &D_8009B458[idx * SD_SECONDARY_OBJECT_SIZE + 0x180];
        if (vag > 0) {
            do {
                sum += sizes[i];
                i++;
            } while (i < vag);
        }
        sum <<= 3;
        voice = &D_80011434[idx];
        *(s32 *)(D_8009B458 + 0x4C4) = 0x6019F;
        *(s16 *)(D_8009B458 + 0x4CC) = 0;
        *(s16 *)(D_8009B458 + 0x4CE) = 0;
        *(s32 *)(D_8009B458 + 0x4C0) = *voice;
        *(s32 *)(D_8009B458 + 0x4DC) = *(s32 *)(D_8009B458 + 0x4B8) + sum;
        adsr1 = *(u16 *)(tone + 0x10);
        *(s16 *)(D_8009B458 + 0x4FA) = adsr1;
        *(u16 *)(obj + 0x20) = adsr1;
        adsr2 = *(u16 *)(tone + 0x12);
        *(s16 *)(D_8009B458 + 0x4FC) = adsr2;
        *(u16 *)(obj + 0x22) = adsr2;
        if ((*(u16 *)(tone + 0x10) & 0x80) == 0) {
            amode = 1;
        } else {
            amode = 5;
        }
        *(s32 *)(D_8009B458 + 0x4E4) = amode;
        *(u16 *)(obj + 0x24) = *(u16 *)(D_8009B458_r + 0x4E4);
        rec[4] = velocity;
        obj[0x12] = tone[4];
        obj[0x13] = tone[5];
        obj[0x11] = tone[0xC];
        obj[0] = idx;
        obj[4] = tidx;
        obj[2] = program;
        obj[5] = level;
        obj[3] = channel;
        obj[0xD] = 1;
        obj[0x10] = tone[0xD];
        obj[6] = level;
        obj[8] = prog[1];
        obj[0xA] = prog[4];
        obj[9] = tone[2];
        obj[0xE] = velocity;
        *(u16 *)(obj + 0x1E) = 0xFFFF;
        obj[0xB] = tone[3];
        func_8004A0FC(D_8009B458 + idx * SD_SECONDARY_OBJECT_SIZE + 0x180,
                      D_8009B458 + ch * SD_SEQUENCE_CHANNEL_RECORD_SIZE);
        *(s16 *)(D_8009B458 + 0x4C8) =
            (*(u16 *)(obj + 0x14) * *(u16 *)(D_8009B458 + 0x514)) >> 7;
        *(s16 *)(D_8009B458 + 0x4CA) =
            (*(u16 *)(obj + 0x16) * *(u16 *)(D_8009B458 + 0x516)) >> 7;
        obj[5] = level;
        *(s16 *)(obj + 0x1A) = -1;
        *(s16 *)(obj + 0x1C) = rec[7];
        pitch = func_8004A3BC(obj, rec[7]) + obj[6] * 128;
        *(s16 *)(D_8009B458 + 0x4D4) =
            func_80049FB4((s16)pitch >> 7, pitch & 0x7F, tone[4], tone[5]);
        SpuSetKeyOnWithAttr((SpuVoiceAttr *)(D_8009B458 + 0x4C0));
        if (stolen == 0) {
            if ((rec[6] & 0xF) < 0xF) {
                rec[6] = rec[6] + 1;
            }
        }
        obj[0xF] = 1;
        if (rec[0x10] == 0) {
            if ((tone[1] & 4) != 0) {
                s32 *m = &D_80011434[idx];
                do {
                    SpuSetReverbVoice(SPU_ON, *m);
                } while ((SpuGetReverbVoice() & *m) == 0);
            } else {
                s32 *m = &D_80011434[idx];
                do {
                    SpuSetReverbVoice(SPU_OFF, *m);
                } while ((SpuGetReverbVoice() & *m) != 0);
            }
        } else if (rec[0x10] != 1) {
            s32 *m = &D_80011434[idx];
            do {
                SpuSetReverbVoice(SPU_ON, *m);
            } while ((SpuGetReverbVoice() & *m) == 0);
        } else {
            s32 *m = &D_80011434[idx];
            do {
                SpuSetReverbVoice(SPU_OFF, *m);
            } while ((SpuGetReverbVoice() & *m) != 0);
        }
    next:
        tidx++;
    } while ((tidx & 0xFFFF) < prog[0]);
}
```
