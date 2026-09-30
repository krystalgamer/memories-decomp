# MODEL variant modules

`Model_LoadMonsterMerge` loads four more stages per model after the MODEL
primary. Each stage is 10 sectors at `record * 276 + 180`, `+ 190`, `+ 200` and
`+ 210`. Slot 0 loads at `0x8013B000` and slot 1 at `0x8017B000`. This note
calls these images the model variants. Sixty-one slot-0 images are registered;
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
| 405 | `0x3850` | bands `0x8013CD04`, spokes* `0x8013DE54`, rings `0x8013E168`, quad `0x8013E4E8` | 5 | 2 |
| 397 | `0x2DE4` | sheets `0x8013C994`, spokes `0x8013D3F0`, rings `0x8013D6FC`, quad `0x8013DA7C` | 3 | 12 |
| 418 | `0x396C` | sheet `0x8013CAA4`, bands `0x8013D86C`, spokes `0x8013DF78`, rings `0x8013E284`, quad `0x8013E604` | 4 | 13 |
| 428 | `0x359C` | sheets `0x8013C7AC`, spokes `0x8013D1FC`, rings `0x8013D508`, quad `0x8013D888` | 4 | 6 |
| 404 | `0x40FC` | sheets `0x8013DCA4`, spokes* `0x8013E700`, rings `0x8013EA14`, quad `0x8013ED94` | 5 | 6 |
| 416 | `0x30D0` | spokes* `0x8013D6D4`, rings `0x8013D9E8`, quad `0x8013DD68` | 5 | 2 |
| 425 | `0x43FC` | bands `0x8013E2F4`, spokes* `0x8013EA00`, rings `0x8013ED14`, quad `0x8013F094` | 6 | 2 |
| 448 | `0x43E4` | spokes `0x8013E0BC`, rings `0x8013E3C8`, quad `0x8013E748` | 6 | 2 |
| 423 | `0x2B38` | spokes* `0x8013D13C`, rings `0x8013D450`, quad `0x8013D7D0` | 4 | 2 |
| 414 | `0x2F58` | spokes `0x8013D8CC`, rings `0x8013DBD8` | 5 | 1 |
| 401 | `0x3124` | spokes* `0x8013D728`, rings `0x8013DA3C`, quad `0x8013DDBC` | 5 | 1 |
| 443 | `0x2888` | strand `0x8013CD84` | 4 | 12 |

`sheet` (header 418) is a one-sheet form of sheets: a single `ModelVariantSheet`
at the variant origin whose size follows the two phases of the timing record at
`work + 0x1B74` rather than the sheets path. It was written from header 418's copy.

`bands` (451 instructions) draws one band of nine segments as pairs of
`POLY_GT4` quads along the variant path. Its screen coordinates are the `long`
values `RotTransPers3` writes, so `x0` is read with `lhu` and `y0` as `sxy >> 16`
with `lh`. Every work-area access goes through a `work` copy of `ctx`, including
the pointer initialisations; with the copy only on the field reads, every
callee-saved register is rotated. The header-418 copy's record is 0x24 bytes
longer, which shows only in the band stride.

`strand` (header 443) fans six strands of thirteen points around the origin and
draws the visible span `[0x2EA6, 0x2EA8)` of each as `GsLINE` segments. It needs
the same `work` copy.

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
keep the neutral `pos0`.

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

Model 361 has two registered images: header 418 at `+ 180` and header 428 at
`+ 200`.

All images load at `0x8013B000`. Their resident calls resolve to the North
American addresses of the same names and are listed once in
`model_variant_linker_symbols.txt`.

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
