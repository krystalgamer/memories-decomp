# Spanish MODEL441 retained Gouraud lines

`func_8013C1BC` / `func_8017C1BC`, offsets `0x11BC..0x173C`, is independently
recovered game-owned C: 1,408 instruction bytes in each of ten physical loads.
The fresh pre-integration screen covered 5,983 configured regional C entries;
none of the four same-size bodies had its normalized instruction shape.
No accepted regional C body was ported. GCC 2.8.1 / MASPSX 2.81 under the named
`gcc_2_8_1_g0_split` profile matches both slots without forced registers, dummy
locals, padding or inline assembly.

## Retained code, not a recovered runtime dispatch

All nine function intervals are closed, contiguous CFGs with one return and no
indirect calls. The nominal `+4` entry calls only `+173C`, `+2A48`, `+3060` and
`+414C`. The four helpers at `+11BC`, `+1DDC`, `+23FC` and `+3630` have no path
from that entry call graph. In particular, no in-image direct jump/call or
aligned raw address pointer to the selected line helper was found.

The selected helper is retained game code, not an SDK routine: it operates on
the model-specific groups and state also initialized by the entry, and calls
the resident geometry/Gouraud-line APIs. **No runtime caller has been recovered.**
This does not prove universal unreachability through outside code. No dispatch
is added, and no original-context call edge is invented. The helper's own `a0`
context capture and packet lifetime are proved independently; entry initialization
and resident loader/context ownership corroborate layout, not execution.

## Physical inventory

Each image is 20 KiB, with header 441 at `0x8013B000` or 591 at `0x8017B000`.
The instance CSV records all physical hashes and sectors. Models and archive
record numbers are deliberately distinct.

| Model | Record | Stages | Sectors | Command | Start | End |
|---|---|---|---|---|---|---|
| 166 | 166 | 7 / 8 | 45996 / 46006 | 607000 | 46 | 76 |
| 360 | 310 | 7 / 8 | 85740 / 85750 | 607002 | 60 | 168 |
| 487 | 437 | 7 / 8 | 120792 / 120802 | 607003 | 50 | 128 |
| 590 | 540 | 7 / 8 | 149220 / 149230 | 607000 | 46 | 76 |
| 709 | 609 | 9 / 10 | 168284 / 168294 | 607004 | 0 | 84 |

Boundaries are `4, 11BC, 173C, 1DDC, 23FC, 2A48, 3060, 3630, 414C, 4754`
(hexadecimal). At the line-only checkpoint, each image retained eight ASM functions, its four-byte header,
and the untouched `4754..5000` raw tail. This adds ten matching C instances,
14,080 instruction bytes, and an inventory of ninety physical functions;
eighty remain ASM. No shared helper is counted as ten unique routines.

The subsequent [tubular-mesh recovery](spanish-model-variant441-tube.md) preserves
this line code and its caller limitation. With that separate helper integrated,
each image has two C functions and seven ASM functions.
The later [framebuffer-ring recovery](spanish-model-variant441-framebuffer-rings.md)
adds a third C helper per image, leaving six ASM functions without changing
this retained helper's caller limitation.
The subsequent [retained-ribbon recovery](spanish-model-variant441-ribbon.md)
adds a fourth C helper per image, leaving five ASM functions. Neither retained
helper gains a recovered runtime caller.

The descriptor address is `base+4850+(command%1000)*48`. Signed start/end
fields at `+1C/+20` have positive differences in all actual descriptors.
Those are descriptor facts, not evidence that the retained helper is called.

## Independently recovered behavior and layout

The private view ends at `2720`; it is not a declaration of the entire context.
Three groups start at `AF8`, stride `1A0`, and end at `FD8`. Each contains two
banks of `[4][6]` `SVECTOR` points, bank one at `C0`, RGB at `180`, and signed
size at `194`. Unknown bytes remain opaque; no completion field is invented.

The `GsGLINE` occupies `2668..267C`. Origin is `2690`, target `269C`,
direction pair `26A8`, projected pair `26B4`, three-word direction `26B8`,
signed time `26D4`, step `26DC`, descriptor pointer `26E4`, and phase `271C`.
Thirty target-compiler size/offset constants are checked. The helper captures
its context in `s3` at `+11C4`; its packet pointer in `s1` at `+120C` remains
stable through the body. Repeated output pointers at `266C/2670` address only
the packet coordinates; RGB writes stay within the same twenty-byte packet.

Four `ratan2` calls retain their unused results. For each group, zero rotation
and uniform scale are applied around origin before phase two and target
afterward. Four rows of six endpoint pairs are projected with repeated
`RotTransPers4` inputs/outputs. Attribute is `50000000`; RGB endpoint ownership
switches at phase two. Submission requires both nonnegative depth and
nonnegative GTE flag, then narrows priority to `u16`.

Phase zero uses size below 4096 (otherwise zero scale), with fade above 2048.
Phase one uses scale 4096 and black RGB. Later phases clamp nonpositive scale
to zero and fade after 6144. For a positive group size in phase zero:

```c
progress = (time - start) * 3 * 4096 / (end - start) - 4096;
size = i * 4096 / 3 - progress;
if (size <= 0)
    size += 4096;
```

This is signed relative timing, not an unsigned quotient or a duration-only
field. The exact binary preserves the compiler's signed divide-by-zero and
overflow trap paths. Phase one sets `-(i*8192/3)`; later phases add `step*256`
while below 8192. Crossing 8192 clamps when phase is at least five, otherwise
wraps and clears a local eligibility flag. Phase five advances to six only at
the last group, on that crossing, while eligibility remains one. The flag is
not an aggregate of stored group-completion fields.

## Evidence and preservation

The attempt ledger records the first independent exact body, its independent
slot-one wrapper, and all ten complete-image matches. Dependency fingerprints
cover body C, private header, and shared local SDK declarations in that order.
There were no failed source experiments for this target.

Focused regressions check exhaustive archive discovery, actual descriptors,
all ninety CFGs, the missing entry call path, packet/register lifetimes, thirty
layouts, repeated header inclusion, all function/data input and final owners,
every selected relocation, eleven resident calls to eight addresses, and ten
local jumps. Resident checks cover all 37 bound addresses, accepted loader and
context owners, and non-overlap of the private view with live banks.

Complete-image identity is necessary but not the sole ownership proof: the
selected compiler object must own the exact `STT_FUNC` interval, while each
remaining function and raw region keeps its own linker-selected owner.
Sources, configs, ledgers and research are tracked; retail bytes, generated
assembly, tools and scratch evidence are not.
