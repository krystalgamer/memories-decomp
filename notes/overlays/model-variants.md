# MODEL variant modules

`Model_LoadMonsterMerge` loads four more stages per model after the MODEL
primary. Each stage is 10 sectors at `record * 276 + 180`, `+ 190`, `+ 200` and
`+ 210`. Slot 0 loads at `0x8013B000` and slot 1 at `0x8017B000`. This note
calls these images the model variants. Sixty-two slot-0 images are registered;
see [Registered images](#registered-images).

## Compiler

The helpers registered here were not built with the gcc 2.8.1 that builds the
rest of the game. (The Spanish variant helpers in
`src/overlays/spanish_model_variant/` do match under `gcc_2_8_1_g0_split`, so
this is a statement about these functions, not about every variant.) They end in `addiu $sp, $sp, N; jr $ra; nop`. gcc 2.7.2 emits
that epilogue as reorder-mode text, and the assembler fills the delay slot
with a `nop`. gcc 2.8.1 emits the epilogue as RTL, and reorg moves the stack
adjustment into the slot. For the four header-397 helpers, gcc 2.8.1 output is
one word shorter than the retail function, and 21 to 24 words differ in all.

The `gcc_2_7_2_cdk_g0` profile uses the decompals/old-gcc 0.17
`gcc-2.7.2-cdk` release (`cygnus-2.7.2-970404`), installed by
`make compiler-272-prebuilt` from the pinned
`tools/bootstrap/old_gcc_272_prebuilt.json`. Apart from the compiler path it
is identical to `gcc_2_8_1_g0`: same flags, same maspsx arguments.

Four helper bodies were measured against their retail words with each 2.7.2
build before registration (quad and rings in their header-405 copies, spokes
and sheets under header 397):

| Helper | Instructions | `gcc-2.7.2-cdk` | `gcc-2.7.2-psx` |
|---|---:|---|---|
| quad | 218 | match | 4 words differ |
| rings | 224 | match | 24 words differ |
| spokes | 195 | match | 27 words differ |
| sheets | 314 | match | match |

PsyQ 4.1's `CC1PSX.EXE` (the same `cygnus-2.7.2-970404`) also matches all four,
run under wine.

Negative control: with `gcc_2_8_1_g0` selected, `make match-overlays` fails
for `model_variant_1_pos0_slot0` with "rebuilt module does not match its
input", and each of the four header-397 helpers compiles one word short.
`make overlays` and `make verify-overlays` pass under either profile, because
neither of them compiles C.

## Header families

The first word of a variant image is a header id. Every image with a given
header that has been checked has the same text: the same functions at the same
addresses, byte for byte. Only the data after the text differs. So the C files
are named by header, as the Spanish variant files are
(`src/overlays/spanish_model_variant/variant432_*.c`), and each registered image
of a header reuses them unchanged.

The same helper body also occurs under other headers at other addresses. Those
copies are not byte-identical: the work-area offsets differ. For example, rings
reads its ring array at `ctx + 0x15AC` under header 405 and at `ctx + 0x72C`
under header 397, and quad reads its divisor from `+ 0x20` of a pointed-to
record under header 405 and from `+ 0x24` under header 397. The header-397
rings and quad files are the header-405 sources with those constants replaced,
and the header-418, 428 and 404 files are the header-397 ones with theirs
replaced; nothing else changed. Under header 428 the two fields of the
pointed-to record that sheets and quad divide by move from `+ 0x20`/`+ 0x24`
to `+ 0x1C`/`+ 0x20`. Each new header therefore needs its own copy with its own
offsets.

| Header | Text end | C functions | Assembly functions | Images |
|---:|---|---|---:|---:|
| 405 | `0x3850` | bands `0x8013CD04`, sheets `0x8013D410`, webs `0x8013D8FC`, spokes* `0x8013DE54`, rings `0x8013E168`, quad `0x8013E4E8` | 3 | 2 |
| 397 | `0x2DE4` | bands `0x8013C22C`, sheets `0x8013C994`, webs `0x8013CE7C`, spokes `0x8013D3F0`, rings `0x8013D6FC`, quad `0x8013DA7C` | 1 | 12 |
| 418 | `0x396C` | spiral `0x8013C088`, ribbons `0x8013D238`, sheet `0x8013CAA4`, webs `0x8013CE50`, bands `0x8013D86C`, spokes `0x8013DF78`, rings `0x8013E284`, quad `0x8013E604` | 1 | 13 |
| 428 | `0x359C` | bands `0x8013C038`, webs `0x8013CC94`, sheets `0x8013C7AC`, spokes `0x8013D1FC`, rings `0x8013D508`, quad `0x8013D888` | 2 | 6 |
| 404 | `0x40FC` | bands `0x8013D4F8`, ribbons `0x8013C1D4`, sheets `0x8013DCA4`, webs `0x8013E18C`, spokes* `0x8013E700`, rings `0x8013EA14`, quad `0x8013ED94` | 2 | 6 |
| 416 | `0x30D0` | webs `0x8013CC68`, spokes* `0x8013D6D4`, rings `0x8013D9E8`, quad `0x8013DD68` | 4 | 2 |
| 425 | `0x43FC` | ribbons `0x8013DCC0`, webs `0x8013D8F8`, bands `0x8013E2F4`, spokes* `0x8013EA00`, rings `0x8013ED14`, quad `0x8013F094` | 4 | 2 |
| 448 | `0x43E4` | webs `0x8013DB58`, spokes `0x8013E0BC`, rings `0x8013E3C8`, quad `0x8013E748` | 5 | 2 |
| 423 | `0x2B38` | bands `0x8013BF18`, sheets `0x8013C6E4`, webs `0x8013CBDC`, spokes* `0x8013D13C`, rings `0x8013D450`, quad `0x8013D7D0` | 1 | 2 |
| 414 | `0x2F58` | spokes `0x8013D8CC`, rings `0x8013DBD8` | 5 | 1 |
| 401 | `0x3124` | spokes* `0x8013D728`, rings `0x8013DA3C`, quad `0x8013DDBC` | 5 | 1 |
| 443 | `0x2888` | sheets+ `0x8013C808`, strand `0x8013CD84` | 3 | 12 |

`sheet` (header 418) is a one-sheet form of sheets: a single `ModelVariantSheet`
at the variant origin whose size follows the two phases of the timing record at
`work + 0x1B74` rather than the sheets path. It was written from header 418's copy.

`webs` (header 418, 250 instructions) draws three 0x260-byte `ModelVariantWeb`
records as 6x6 `GsGLINE` segments from each near grid point to the far one,
fading the colour once the web's scale passes `0x1000`. Each scale grows by
`step * 0xC0` to `0x2000` and wraps until phase 4; when all three have stopped
in phase 4 the phase moves to 5. It opens with four `ratan2` calls whose
results are unused, and its loop counters are `s16`, which is what puts the
web counter and the done flag on the stack as the target has them.
Header 425 has a 242-instruction form at `0x28F8` (`variant425_webs`): 5x6
grids in 0x200-byte `ModelVariantWebSmall` records, projected with
`RotTransPers3`, faded past `0x800` and grown by `step * 128` to `0x1000`.
Header 398 has a 332-instruction form at `0x2514` (`variant398_webs`): the
header-397 phased webs over records at `work`. Its phase 0 places web `i` at
`(i << 12) / 3` less the progress `(now * 3 << 12) / end` of the timing record
at `work + 0x1A00`, plus `0x1000`, wrapped by `0x1000` (two statements, as in
the other staggered forms). In phase 4 each web stores the `done` flag, which
is set once and never cleared, into its own record at `+ 0x198` before the
phase moves to 5.
Header 405's form at `0x28FC` (`variant405_webs`, 342 instructions) is the
header-397 phased webs over records at `work + 0xDD4`. Its phase 0 places web
`i` at `(i << 12) / 3` less the progress `((now - start) * 3 << 12) / (end -
start)` of the timing record at `work + 0x1DC8`, plus `0x1000`, wrapped by
`0x1000`. That assignment matches only as two statements, the progress less
`0x1000` stored to the scale first. A line is sorted whenever its depth is
positive.
Header 423's form at `0x1BDC` (`variant423_webs`, 344 instructions) is the
header-397 phased webs over records at `work`, projected with `RotTransPers3`
and grown by `step * 0x180` from phase 2. Its phase 0 places web `i` at
`(i << 12) / 3` less the progress `((now - start) * 3 << 12) / (end - start)`
of the timing record at `work + 0xF00`, plus `0x1000`, wrapped by `0x1000`;
that assignment matches only as two statements, the progress less `0x1000`
stored to the scale first.
Model 712's header-459 image (and its header-609 slot-1 image) has a
243-instruction form at `0x2064` (`variant459_webs`): the header-418 grids
with header 425's fade and growth, sorted whenever `otz > 0`.
Header 397 has a 349-instruction form at `0x1E7C` (`variant397_webs`): 4x6
grids in 0x1A0-byte `ModelVariantWebNarrow` records, driven by the phase at
`work + 0xF78`. In phase 0 each scale shrinks by `step * 0xC0` from `0x1000`
and the colour fades in on the far end; in phase 1 web `i` waits at
`-(i << 13) / 3`; from phase 2 the webs sit on the `s16` position at
`work + 0xF0C`, grow by `step << 8` to `0x2000` with the colour on the near
end, and the phase moves to 5 as in the header-418 form. The timing record at
`work + 0x5FC` stops the phase-0 wrap once its word at `+ 0x88` passes `0x800`.
Header 404 carries the same body at `0x318C` (`variant404_webs`) with its own
work-area offsets.
Header 448's form at `0x2B58` (`variant448_webs`) keeps the records at
`work + 0x6F8`, shrinks by `step * 0xE0` in phase 0, and sorts a line whenever
its depth is positive.
Header 428's form at `0x1C94` (`variant428_webs`) keeps the records at
`work + 0x5D0`, projects with `RotTransPers3`, and shrinks by `step * 0xE0` in
phase 0.
Header 416's form at `0x1C68` (`variant416_webs`) keeps the records at
`work + 0xA80` and replaces the phase-0 shrink: each web with a positive scale
is set to `(i << 12) / 3` less the progress `(now - start) * 3 << 12 / (end -
start)` of the timing record at `work + 0x1A60`, plus `0x1000`, and wrapped by
`0x1000` when that is not positive. It matches only with the progress stored to
the scale first and subtracted from the third in a second statement.

`ribbons` (header 418, 397 instructions) builds eight 0x74-byte
`ModelVariantRibbon` records fanned `0x200` apart around the path, projects each
two-point spine and a copy of it moved 16 units sideways, and draws the first
point of each as a `POLY_G3` whose width is the projected offset. Three things
in its first loop nest decide the match, all through gcc 2.7.2's loop pass:

- the `16` is an `s16` local, which leaves `lui 0x10; sra 16` in the outer loop;
- the depth `(flags & 1) * 32 + 0xA0` is written inside the inner loop from a
  local copy of the word at `work + 0x1B58`. Its three instructions are hoisted
  out of both loops, and each move makes the pass less willing to hoist the
  next, which is what stops the `sra` above from following them. Computed
  before the loop, the constant folds to `li 16` and the function is five
  words short;
- the radius is an `s16`, which orders the two inner-loop registers.

Header 425 has the same body at `0x2CC0` (`variant425_ribbons`) over 0x6C-byte
`ModelVariantRibbonShort` records, drawn when the depth is positive rather than
non-negative, with the last frame read from `+ 0x18` of the timing record.
Header 404's form at `0x11D4` (`variant404_ribbons`, 420 instructions) is the
header-425 source with its own offsets, the far colour `0x40, 0x60, 0xFF`, and
the `RotTransPers4` flag of every point kept in a stack array `status[8][2]`;
a point is drawn only when both its depth and its flag are non-negative.

`spiral` (header 418, 647 instructions) is the ribbons helper over twelve
0x84-byte `Variant418SpiralArm` records at `work + 0x720`: each arm's outer
point sits on a spiral at `(i << 12) / 12` and `(i << 13) / 12` past the turn
at `work + 0x1B6C`, scaled by `(rsin << 9) * k`, and each arm is drawn as two
`POLY_GT4` halves on either side of the spine, sorted at depth 0. As in the
other ribbon and streamer helpers, the point stores need `setVector` on
`&arm->a[k]`, and the screen deltas need `dx`/`dy` locals. With the deltas
written inline in `ratan2`, the loads are scheduled in another order.

`bands` (451 instructions) draws one band of nine segments as pairs of
`POLY_GT4` quads along the variant path. Its screen coordinates are the `long`
values `RotTransPers3` writes, so `x0` is read with `lhu` and `y0` as `sxy >> 16`
with `lh`. Every work-area access goes through a `work` copy of `ctx`, including
the pointer initialisations; with the copy only on the field reads, every
callee-saved register is rotated. The header-418 copy's record is 0x24 bytes
longer, which shows only in the band stride.

Header 397's bands (`0x8013C22C`, 474 instructions) is the same helper over one
0x11C-byte `ModelVariantBandShort` of five points, with the `RotTransPers3` flag
kept per point in the record. Its radius is scaled by the word at `+ 0x44` of
the timing record at `work + 0xF54` (by 20/16 of it when the flag bit is set),
and every depth is clamped to zero before the sort, which also clears the
point's flag. The second quad of each pair reads its current column through a
separate pointer, `(s32 *)band + j`, that moves on to the next column after the
clamp and is still assigned after the loops. cse then keeps that pointer as a
copy (`move v1,s0`) up to the clamp and reloads the depth through the loop's
own address register at the join. With the fields indexed directly, the copy
folds away and 33 lines differ.

Header 428's bands (`0x8013C038`, 477 instructions) keeps nine points per band at
`work + 0xAB0` with a radius of `* 48 / 4096` or `* 56 / 4096`. Each depth is
clamped to zero before its sort, which also clears that point's
`RotTransPers3` flag in a stack array `flag[i][j]`. The second quad reads
column `j` through `(s32 *)band + j`, a pointer that moves on to the next column
after the clamp and is still assigned after the loops. cse then keeps it as the
target's `move v1,s0` copy up to the clamp.

Header 404's bands (`0x8013D4F8`, 491 instructions) keeps nine points per band at
`work + 0xE10`. Its radius is scaled by the word at `+ 0x38` of the timing
record at `work + 0x1938` (by 18/16 of it when the flag bit is set), and that
record's two phases sit at `+ 0x24` and `+ 0x2C`. Each depth is clamped to zero
before its sort, which also clears that point's `RotTransPers3` flag in a stack
array `flag[i][j]`. The second quad reads column `j` through
`(s32 *)band + j`, a pointer that moves on to the next column after the clamp
and is still assigned after the loops. cse then keeps it as the target's
`move v1,s0` copy up to the clamp.

Header 423's bands (`0x8013BF18`, 499 instructions) keeps five points per band
in 0x108-byte `Variant423Band` records at `work + 0x4E0`. Its radius is
`h * 48 / 4096` (or `h * 54 / 4096` with flag bit 0) plus
`rsin(angle) * (h / 256) >> 12`, where `h` is the `s16` at `work + 0xF18` and
the angle at `work + 0xF1E` then advances by `step * 256`. Written `<< 8`, the
step is narrowed to an `lhu` load. The path progress is the `s16` at
`work + 0xF1C`. As in the header-398 port, each depth is clamped to zero before
its sort and the second quad reads its column through `(s32 *)band + j`; the
sort key is the next point's depth, `otz[j + 1]`.

`strand` (header 443) fans six strands of thirteen points around the origin and
draws the visible span `[0x2EA6, 0x2EA8)` of each as `GsLINE` segments. It needs
the same `work` copy.

`sheets+` (header 443, 351 instructions) draws a variable number of 0x9C-byte
sheets. The count, and how many of them sit on the variant's own 0x20-byte slots
at `work + 0x2C50`, come from the timing record at `work + 0x2E7C`. The rest sit
on the `VECTOR` table at `work + 0x2D3C` and follow 0x2E8-byte objects that
start at `ctx`. The object cursor advances only for those.

`spokes*` is a second form of the spokes helper, 197 instructions instead of
195: it draws four rings like spokes but only sorts lines whose depth is below
`0x800`, as rings does. `variant405_spokes.c` was written from header 405's
copy and the other five were derived from it by the same offset substitution.

## Registered images

Model ids follow the compact-record ledger in
[`spanish-model-return-two-instances.csv`](spanish-model-return-two-instances.csv).
The modules are named `model_variant_<model>_<stage>_slot0`. The Spanish
notes tie record sectors 200..209 to loader stage 9, so the `+ 200` images are named `stage9`. The stage that
reads sectors 180..189 has not been confirmed from the loader, so those images
keep the neutral `pos0`. The images built from Spanish sources take the Spanish
module names instead: `stage7` for `+ 180`, `stage8` for `+ 190` and `stage10` for
`+ 210`, with the `+ 190` and `+ 210` images in slot 1 at `0x8017B000`.

| Header | Sector offset | Models |
|---:|---|---|
| 405 | `+ 180` | 1, 550 |
| 397 | `+ 180` | 2, 20, 87, 108, 138, 193, 573 |
| 397 | `+ 200` | 152, 168, 170, 388, 427 |
| 418 | `+ 180` | 34, 71, 124, 182, 279, 361, 491, 580, 640 |
| 418 | `+ 200` | 166, 275, 469, 590 |
| 428 | `+ 180` | 187, 596 |
| 428 | `+ 200` | 239, 361, 368, 478 |
| 404 | `+ 180` | 84, 162 |
| 404 | `+ 200` | 88, 114, 184, 369 |
| 416 | `+ 180` | 180, 440 |
| 425 | `+ 180` | 259, 630 |
| 448 | `+ 200` | 108, 573 |
| 423 | `+ 200` | 262, 631 |
| 414 | `+ 180` | 401 |
| 401 | `+ 200` | 410 |
| 443 | `+ 180` | 70, 460, 469, 704 |
| 443 | `+ 200` | 44, 98, 161, 370, 400, 458, 462, 558 |
| 443* | `+ 180` | 125 |
| 391 | `+ 200` | 54 |
| 541 | `+ 210` | 54 (slot 1) |
| 320 | `+ 180` | 410 |
| 320 | `+ 200` | 110, 159 |
| 470 | `+ 190` | 410 (slot 1) |
| 470 | `+ 210` | 110, 159 (slot 1) |
| 415 | `+ 200` | 401 |
| 565 | `+ 210` | 401 (slot 1) |

`391` and `541` are model 54's `+ 200` and `+ 210` images (stages 9 and 10;
the second loads at `0x8017B000`, see below). They are the North American build
of the Spanish header-408 variant: the files in `src/overlays/model_variant/`
named `variant391_*` include the Spanish sources and rename their addresses.
The same C matches the Spanish images under `gcc_2_8_1_g0_split` and the North
American ones under `gcc_2_7_2_cdk_g0`, so the European build recompiled these
helpers with the newer compiler. Two other things differ: the entry uploads its
palettes to CLUT column 512 instead of 640, and the data sits 0x18 bytes later
because the text is six words longer. The images also showed that the Spanish
linker symbols named `0x80082E48` `SetPolyF4`. Its body is `SetPolyG3` (length
6, code `0x30`), and the 28-byte primitive the entry sets up is a `POLY_G3`.

Headers 320/470 and 415/565 are ported the same way, from the Spanish
header-337 rings helper (`variant320_*`) and the header-432 draw, layers and
bands helpers (`variant415_*`). Every North American header is 17 below the
Spanish one. Their other functions stay in assembly. The header-415 layers and
bands helpers are 20 and 18 words longer than their Spanish builds, so the
compiler change is more than the epilogue `nop`.

`443*` is model 125's header-443 image. Its text runs to `0x4D0C`, with one more
function than the other twelve, but sheets+ and strand are byte-identical at the
same addresses, so it reuses both files. Model 168's longer header-443 image is
left out because it calls `0x80054A44`, inside `func_800540B4`.

Model 361 has two registered images: header 418 at `+ 180` and header 428 at
`+ 200`.

All images except model 54's stage-10 image load at `0x8013B000`. Their resident calls resolve to the North
American addresses of the same names and are listed once in
`model_variant_linker_symbols.txt`.

### Slot-1 images

The `+ 190` (stage 8) and `+ 210` (stage 10) images load at `0x8017B000`.
Each one's header is its slot-0 header plus 150, its text has the same function
boundaries, and every `lui` of a module address in it is `0x8018`. The
`*_slot1.c` files include the slot-0 C and rename the one function it defines to
`0x8017`. Sixty-two images are registered this way:

| Header | Slot-0 header | Sector offset | Models |
|---:|---:|---|---|
| 547 | 397 | `+ 190` | 2, 20, 87, 108, 138, 193, 573 |
| 547 | 397 | `+ 210` | 152, 168, 170, 388, 427 |
| 551 | 401 | `+ 210` | 410 |
| 554 | 404 | `+ 190` | 84, 162 |
| 554 | 404 | `+ 210` | 88, 114, 184, 369 |
| 555 | 405 | `+ 190` | 1, 550 |
| 564 | 414 | `+ 190` | 401 |
| 566 | 416 | `+ 190` | 180, 440 |
| 568 | 418 | `+ 190` | 34, 71, 124, 182, 279, 361, 491, 580, 640 |
| 568 | 418 | `+ 210` | 166, 275, 469, 590 |
| 573 | 423 | `+ 210` | 262, 631 |
| 575 | 425 | `+ 190` | 259, 630 |
| 578 | 428 | `+ 190` | 187, 596 |
| 578 | 428 | `+ 210` | 239, 361, 368, 478 |
| 593 | 443 | `+ 190` | 70, 460, 469, 704 |
| 593* | 443 | `+ 190` | 125 |
| 593 | 443 | `+ 210` | 44, 98, 161, 370, 400, 458, 462, 558 |
| 598 | 448 | `+ 210` | 108, 573 |

`593*` is model 125's longer image. Like its slot-0 image, it has words past
`0x3004` that decode as `jal`s to addresses no resident function starts at; the
module matches with them kept as assembly. Model 168's longer image is left out,
as its slot-0 image is, because it calls `0x80054A44` inside `func_800540B4`.

### Header 407: petals

`variant407_petals.c` (`func_8013C2DC`, 379 instructions) draws 48 petals, one
`POLY_G4` each. Each petal grows from the variant centre along its own velocity
(`scale * velocity / 512`), swings around a circle whose radius shrinks with
`rcos(scale)`, fades past scale `0x200`, and cycles its scale through the timing
record's phases. It is in 15 header-407 images (models 68, 96, 186, 297, 376
and 595 at `+ 180`; 165, 242, 294, 352, 358, 399, 465, 520 and 621 at
`+ 200`) and their header-557 slot-1 images. What reproduces the target:

- two copies of the parameter, one for the work fields and one for the petal
  scales and colours;
- the counters, the phase, the sweep angle, the radius and the lift are `s16`;
- 16 unused stack bytes between `rot` and `scale`, and the stack locals
  declared in the target's slot order;
- the timing record's `u32` comparisons written with the record's field on the
  left, which fixes the load order;
- the velocity read as `base - -(i * 16) + K`, which keeps the base as the
  first `addu` operand;
- the angle written as `phase + (sweep - fade)` after `fade = 0x400`: with a
  literal, fold reassociates the constant out of the sum, while a named value
  is only propagated by cse, after fold.

### Sibling bodies

Some helpers exist in other images as a sibling body: the same instruction
sequence with other immediates. They are ported by mapping each differing
immediate against the reference image. The registered ports, and what each
changed beyond the map:

| Header | Offset | From | Edit |
|---:|---|---|---|
| 422 | `0x1E84` | header-397 sheets | the size step is `<< 7`, not `<< 6` |
| 422 | `0x236C` | the header-398 webs | a line is sorted whenever its depth is positive |
| 398 | `0x202C` | the header-422 port | none |
| 398 | `0x2514` | header-397 webs | records at `work`, a staggered phase 0, a per-web `done` field |
| 398 | `0x18AC` | header-418 bands | each depth is clamped to zero before its sort, which also clears that point's `RotTransPers3` flag in a stack array `flag[i][j]`; the second quad reads column `j` through `(s32 *)band + j`, a pointer that moves on after the clamp and is still assigned after the loops, so cse keeps it as the target's `move v1,s0` copy |
| 422 | `0x170C` | header-418 bands | the radius is `/ 256` or `* 24 / 4096`; each depth is clamped to zero before its sort, which also clears that point's `RotTransPers3` flag in a stack array `flag[i][j]`; the second quad reads column `j` through `(s32 *)band + j`, a pointer that moves on after the clamp and is still assigned after the loops, so cse keeps it as the target's `move v1,s0` copy |
| 423 | `0x16E4` | the header-398 sheets | sheets at `work + 0x5E8`; the bias is `size * 768 / 4096`; the path progress is the `s16` at `work + 0xF1C`; sorted at `otz - 8`; the shrink step is `<< 6` |
| 405 | `0x2410` | header-397 sheets | sheets at `work + 0x147C`; the corners are passed as `&sheet->vN[k]`, so all four offsets become loop inductions; a quad is sorted at its unscaled depth when that is positive, with no flag test; the timing record's grow phase is at `+ 0x1C` |
| 458 | `0x21D0` | header-443 strand | 0x84-byte strands (`ModelVariantStrandWide`), `otz > 0` |
| 458 | `0x2514` | header-443 streamers | 0x334-byte streamers (the gap before `otz` is 4 bytes), `otz > 0` |
| 321 | `0x1B64` | the header-458 port | the depth test is `otz >= 0 && flag >= 0` |
| 376 | `0x16D8` | Spanish header-337 rings | rewritten over `ModelVariant376State`: one ring or two, no rotation angle, other scale limits and phases |
| 321 | `0x16A8` | the header-376 rewrite | two rings always, over `ModelVariant321State`; the second ring grows to `0x3000` and fades out with the first |
| 324 | `0x29E0` | header-397 rings | `flag > 0` in place of `otz < 0x800` |
| 324 | `0x26C8` | header-405 spokes | `flag > 0` in place of `otz < 0x800` |
| 324 | `0x22F8` | Spanish header-432 draw | one ring over `ModelVariant324State`, placed at base + velocity * time / 1024, with its own phases |

The header-422 images are models 185, 391, 436, 504 and 594 at `+ 180` and 367
and 395 at `+ 200`; header 398 is models 102, 282, 288, 642 and 645 at `+ 200`;
header 458 is models 116 and 576 at `+ 180`; header 321 is models 164, 165,
210, 424 and 609 at `+ 180` and 34, 443 and 459 at `+ 200`; header 376 is models
427, 458 and 459 at `+ 180` and 190, 217, 221, 296, 457, 598, 612 and 647 at
`+ 200`; header 324 is models 7 and 552 at `+ 180`; header 459 is model 712 at
`+ 200`. Their slot-1 images (headers 572, 548, 608, 471, 526, 474 and 609) are
registered with `_slot1` wrappers, as above. The other
functions of these images stay in assembly.

Header 422 also has its own `curtains` helper at `0x2888` (264 instructions,
`variant422_curtains`): three 0x118-byte `ModelVariantCurtain` records, each two
rows of seventeen points drawn as sixteen `POLY_GT4` strips, turned by two
`ratan2` angles and faded from `0x800`. Each scale grows by `step << 7` to
`0x1000` and wraps with a count until the fourth phase. It needs the `work`
copy of `ctx`, and the packet pointer is set before the second angle.
Header 398 has the same helper at `0x2A44` (359 instructions,
`variant398_curtains`), which rebuilds the two rows of each shown curtain every
frame around the angle at `work + 0x1A2C`. The inner row is `rcos >> 4`, read as
`(u32)` so that the shift is `srl`; the outer row is at a radius of `lift +
0x180`, lifted by `lift + 0x80`, where `lift` is `((flags & 1) << 5)` times the
size of the first sheet at `work + 0x6A8`, over 4096. The strip colours are
stored inside the strip loop. The retail register and stack-slot layout needs
the sheet pointer set before the packet pointer and both before the second
angle, the declaration order `curtain, ot, i, lift, yaw, pitch, radius`, and
`i = 0` set between `lift` and `radius`.

Other images with a portable sibling are left out because they call into the
middle of a resident function, as model 168 does. Headers 372 (models 184 and
269), 435 (models 1, 360 and 550 at `+ 200`) and 172 (models 188 and 597) call
`0x800534BC`, `0x8002A5BC`, `0x80051288` and `0x8002B35C`. The spokes port
(header-405 spokes, reading three words at `0x1FF0` where the reference reads
three halfwords at `0x1D80`) and a sheets port with `otz > 0 && flag > 0`
matched those images' functions, but the images cannot be registered yet. The
header-172 slot-1 image has header 302, not 322, so it does not follow the
plus-150 rule either. Models 149 (header 388) and 264 (header 766) carry the header-324
rings and spokes with `otz < 0x800` and `otz >> 2` as the sort depth, and both
match, but model 149 calls `0x8004D5E8` and model 264's text has words that
link as `jal`s outside RAM, so neither image is registered.

## Images left unregistered

Eight more images contain these helpers, but their text calls addresses that
are not function starts in the North American executable. Examples:

- `0x80060A00` inside `func_800608B8`;
- `0x8005F6D0` inside `func_8005F5C8`;
- a `jal` to `0x8053E210` in the `+ 200` image of model 1.

They are headers 435 (models 1, 360, 550 at `+ 200`), 172 (188, 597 and 149 at
`+ 200`) and 372 (184 and 269 at `+ 180`). They are left as they are until it is
known how those images are loaded. No symbols are declared inside resident
functions to make them link.
