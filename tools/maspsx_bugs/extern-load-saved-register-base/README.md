# Saved-register destination used as an extern-load base

Status: awaiting verification against ASPSX 2.81.

`func_8004CB0C` is a `gcc_2_8_1_g0` secondary-sound channel initializer.
The attached C is the full 394-instruction function, not a reduction. It
matches the target instruction count, opcode multiset, text relocations, and
387 of 394 instruction words.

Four of the remaining words are two loads of adjacent external pointer
objects. GCC 2.8.1 emits these pseudo-instructions:

```asm
lw $fp,D_800E9D98
lw $fp,D_800E9D9C
```

MASPSX leaves both pseudo-loads unchanged. GNU as then uses the destination
register as the address base:

```asm
lui $fp,%hi(D_800E9D98)
lw  $fp,%lo(D_800E9D98)($fp)
lui $fp,%hi(D_800E9D9C)
lw  $fp,%lo(D_800E9D9C)($fp)
```

Retail instead uses `$v0` as a separate address base:

```asm
lui $v0,%hi(D_800E9D98)
lw  $fp,%lo(D_800E9D98)($v0)
lui $v0,%hi(D_800E9D9C)
lw  $fp,%lo(D_800E9D9C)($v0)
```

The retail words and little-endian bytes are:

| Address | File offset | Word | Bytes |
| --- | --- | --- | --- |
| `0x8004CB74` | `0x3D374` | `3C02800F` | `0F 80 02 3C` |
| `0x8004CB78` | `0x3D378` | `8C5E9D98` | `98 9D 5E 8C` |
| `0x8004CB84` | `0x3D384` | `3C02800F` | `0F 80 02 3C` |
| `0x8004CB88` | `0x3D388` | `8C5E9D9C` | `9C 9D 5E 8C` |

The current pipeline produces `3C1E800F` / `8FDE9D98` and
`3C1E800F` / `8FDE9D9C`.

## What was ruled out

The function first had two missing instructions from unrelated loop-invariant
motion. Source reconstruction recovered both:

- computing the event tag before its zero-word guard lets the scratch-pad
  constant fill the preceding event-loop break delay slot;
- separating the retry scan limit, sentinel, and post-call result, then using
  the loaded count as the guard before copying it into the limit, restores the
  target `lbu` / branch / `li -1` / `move` sequence.

After those changes the complete function has the target size and opcode
multiset. The two external loads above stayed unchanged through:

- scalar pointer, pointer-array, raw-byte-array, integer, and one-field struct
  declarations;
- direct loads, explicit address locals, ternaries, assign-then-override, and
  inline reader helpers;
- `.data` attributes and the existing no-CSE-follow-jumps profile;
- 32-bit unions, unpinned and pinned address locals, and separate hard-register
  views;
- volatile pointer and volatile word loads, which instead produce
  `lui` / `addiu` in each arm followed by one shared `lw` at the join.

RTL reaches register allocation as one `MEM(symbol)` assignment to the table
pseudo. The compiler assembly still contains the bare pseudo-load shown above,
and MASPSX does not rewrite it. The question for ASPSX is therefore narrow:
does ASPSX 2.81 expand `lw $fp,external_symbol` through `$v0`, or through
`$fp` as GNU as does?

Three other differing words at the function end are an unrelated GCC reload
ordering (`arg1`/`arg2` in `$t0`/`$t1`) and are not part of this report.

## Reproduction

From the repository root:

```sh
tools/maspsx_bugs/extern-load-saved-register-base/build.sh
```

All generated intermediates are written under
`tmp/maspsx_bugs/extern-load-saved-register-base/`.
