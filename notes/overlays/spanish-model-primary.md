# Spanish MODEL primary handlers

The first registered MODEL batch covers seven models at both primary slots:
14 complete 4,096-byte images and 3,264 matching instruction bytes. Four source
units use the named `gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 pipeline. Slot-one
wrappers rename the entry and descriptor symbols before including the same C
body; there are no patched instructions or absolute descriptor aliases.

The particle batch adds models 8, 416 and 141 at both slots: six complete
images and 13,168 instruction bytes. Three additional C bodies and their
slot wrappers use the same profile. Total registered primary coverage is
20 nontrivial instances / 16,432 instruction bytes. The separately documented
[return-two family](spanish-model-return-two.md) adds two representative images
and 16 bytes; neither count is an exhaustive MODEL runtime-completion claim.

`MODEL.MRG` contains 621 compact records of 276 sectors. Transfer stages 11 and
12 read record sectors 220 and 222 respectively, two sectors each, to
`0x8013A000` and `0x8017A000`. The header is four bytes and the entry is base+4.
The record's final metadata sector is 275. Transfer phase 16 copies metadata
offset `0x100` to slot offset `0xCF8`; its command at metadata `0x118` becomes
slot `0xD10`. `model_load_step.c` phase 9 passes nonnegative `commands[2] % 1000`
to the primary entry. `model_control.c` subsequently passes `-1`.

| Models | Compact records | Header IDs, slots 0/1 | Entry bytes | Descriptor offset | Unclassified tail |
|---|---|---|---:|---:|---|
| 150, 167 | 150, 167 | 55 / 58 | 276 | `0x118` | `0x148..0x1000` |
| 116, 370, 394, 707, 715 | 116, 320, 344, 607, 615 | 56 / 59 | 216 | `0xDC` | `0x10C..0x1000` |
| 8 | 8 | 62 / 66 | 1572 | `0x750` | `0x760..0x1000`; also `0x654..0x750` |
| 416 | 366 | 63 / 67 | 2564 | `0xA18` | `0xA24..0x1000` |
| 141 | 141 | 64 / 68 | 2448 | `0xA30` | `0xA8A..0x1000` |

The copy handler selects a 16-byte descriptor, advances an unsigned tick,
computes the original delay/frame modulo, and copies the texture rectangle
through `MoveImage`. The slot contributes 256 to source/destination X and
the Y coordinates add 256. Model 150's command `999001` selects descriptor 1;
model 167's `999002` selects descriptor 2. The work structure is 16 bytes.

The effect handler selects a 16-byte descriptor using argument `% 100` and
animation `argument / 100 + 3`. It calls `func_8005D994` only when that animation
is active. Commands are `998001` (116), `998101` (370/715), `998102` (394),
and `998000` (707). They select indices 0..2 and animation 3 or 4. Its work
structure is eight bytes; the descriptor's last eight bytes are an `SVECTOR`.
The names of uncertain scalar effect parameters remain generic.

The 48-byte descriptor prefixes were reread from the legal Spanish disc and
are identical within each family. This establishes referenced storage for
indices 0..2, **not a terminal bound for every possible table**. The remaining
3,768 or 3,828 bytes per image stay unclassified generated storage, not recovered
C. Entire tails are retained independently because complete module hashes differ.

Production verification checks the real selected compiler object and linked
function's section, address and size, the generated descriptor object's 48-byte
storage and final section-defined symbol, and each complete module hash.
Target-compiled layout probes cover 27 sizes/offsets, including the canonical
metadata/slot command path. The linker binds `Model_GetActiveSlotIndex` at
`0x8005BED4`, `Model_GetSlotAnimationIndex` at `0x8005BF70`,
`func_8005D994` at `0x8004D9A4` and `MoveImage` at `0x8007FFD0`.
The MoveImage identity is supported by the identified North American SDK body
and its diagnostic string, with only regional address fields differing.

## Particle handlers

Family 62 initializes sixteen possible points and byte-sized texture frames,
then renders timed textured quads relative to a model part. The 16-byte
descriptor supplies RGB/part and signed size, radius, travel, count, uncertain
field `0xC`, and delay. Its 156-byte state contains a descriptor pointer,
sixteen `SVECTOR`s, sixteen byte frames, a **signed** packed texture word, and
delay. Model 8's command 0 selects descriptor 0. The known data owners are
the 16-byte scale at `0x628`, one 28-byte `GsIMAGE` at `0x638`, and the
16-byte descriptor at `0x750`. The intervening 252 bytes and trailing 2,208
bytes remain unclassified.

Family 63 initializes three groups of sixteen points and velocities, then
renders fading one-pixel particles with animation/frame visibility gates.
Its 12-byte descriptor contains RGB, three part indices, two radii and a
period. The 776-byte state places points at offset 4, velocities at 388 and
the frame at 772. Model 416's command 1000 selects descriptor 0: RGB
255/208/0, parts 16/17/18, radii 70/100 and period 16. Real storage owns
the 16-byte scale at `0xA08` and 12-byte descriptor at `0xA18`; the following
1,500 bytes are unclassified. Reusing `a`, `b` and `radius` between
initialization and update is necessary for the original register allocation.

Family 64 initializes twelve model-part positions and five **unsigned**
packed textures. Update preserves six signed random remainder operations per
point, negative-divisor motion, split quad transforms and averaged signed
depth. The 90-byte descriptor contains RGB/count, a divisor, twelve part
indices and twelve each of widths, heights and jitter magnitudes. Its state
is 124 bytes. Model 141's command 3000 selects descriptor 0 with count 2 and
divisor 6. The scale at `0x994` owns 16 bytes; the five `GsIMAGE`s at `0x9A4`
own 140 bytes. The descriptor at `0xA30` owns 90 bytes and the unknown suffix
at `0xA8A` owns 1,398 bytes. These last two symbols share one aligned generated
data object: splitting the input at `0xA8A` caused the word-oriented splitter
to discard two bytes from each slice, even with byte symbol annotations.
The shared object preserves both exact symbol extents; none of the suffix
is counted as C or as a known descriptor. Its symbol is marked `defined:true`
so the generated undefined-symbol script cannot replace its real section
owner with an absolute alias.

All three layouts use locally recovered declarations and canonical SDK/game
headers. `func_80059A50` is declared once in its owning texture-upload header
and binds to the real Spanish function at `0x8005CB58`. The uncertain
ordering-table getter remains `func_80058F10`, at `0x8005C018`; no SDK
identity is asserted for that address-based name.

Production checks compare all six complete images, selected compiler entry
objects, final function symbols, real non-executable input data owners and
their linked extents, 79 target-compiled layout values and each referenced
resident callee. Unknown ranges remain byte-for-byte storage, not C coverage.
`MulMatrix2` (`0x80087408`) and `RotTransPersN` (`0x80087C48`) have identical
268-/100-byte North American bodies; `GsSortBoxFill` (`0x800841C8`) differs
only in five regional address words across its 216-byte body.

### Matching experiments

All experiments used `gcc_2_8_1_g0_split`. Fingerprints identify the exact
source/header pair; intermediate candidates and instruction diffs are local
scratch evidence, not additional production implementations.

| Family | Fingerprint prefix | Compiled bytes | Differing words | Result |
|---|---|---:|---:|---|
| 62 | `581ad9b89c825bfb` | 1588 | 267 | Loop ordering added delays; immediate RGB stores changed scheduling. |
| 62 | `7eb8781f33fc3af1` | 1576 | 27 | Initialization/RGB corrected; render-loop index still advanced too early. |
| 62 | `0046ad960a45ee65` | 1572 | 0 | `point++, frame++, i++` matches both slots. |
| 63 | `2a9ec4fe5456dfc8` | 2552 | 251 | Separate update temporaries changed allocation and spills. |
| 63 | `fbc38d928add9606` | 2564 | 0 | Reused initialization temporaries; both slots exact. |
| 64 | `88739264c20e2019` | 2448 | 0 | First independently declared candidate exact at both slots. |

This is not exhaustive MODEL coverage. Return-two
primaries, all four variant phases, secondary loads, and every unclassified
tail remain separate recovery/registration work.

## Independent French registration

The first French batch reused all four accepted copy/effect source units unchanged,
with the same named compiler profile and independently checked French resident
bindings. The complete French MODEL archive was compared byte-for-byte against
the legal French disc's ISO extent, including its 621 compact records. Its
SHA-256 is `0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3`.
All fourteen selected 4,096-byte payloads independently reproduce the module
hashes above; they are separate images, not prefix-based duplicate declarations.

The French matched loader at `0x8005967C`, transfer phase at `0x80059EF4`,
and load-step routine at `0x800599A0` establish the same record/sector and command
path. The complete sixteen-phase sector accounting includes the one-sector
stage 5 read: stages 11/12 begin at 220/222, not 219/221. Retail pointer words
at `0x8001000C` and `0x80010010` independently establish the two load addresses.
French metadata commands select descriptor indices 1, 1, 2, 1, 2, 0 and 1 for
models 116, 150, 167, 370, 394, 707 and 715 respectively. Only `commands[2]`
belongs to this initial primary dispatch; the first two command words must not
be interpreted as additional indices into these same tables.

Each French image retains a real 48-byte generated descriptor-prefix object
and its own unclassified tail. The matching gate checks complete images,
selected compiler objects, actual executable entry symbols, descriptor input
and final owners, target layouts and resident imports. The fourteen entries
added 3,264 instruction bytes, bringing configured French coverage to 227 of 228
function instances and 141,568 instruction bytes across 22 images.
This is not exhaustive French MODEL or runtime coverage, and it does not
classify any preserved tail as non-code.

The subsequent particle batch reuses all six accepted family 62/63/64 source
units unchanged, adding 13,168 instruction bytes for models 8, 416 and 141
at both slots. French metadata commands 0, 1000 and 3000 respectively select
descriptor zero. Model 8's descriptor requests 16 particles; model 141's
count is 2 within its twelve-element work/config arrays. All six complete
French payloads match independently. Target compilation verifies 90 layout
values, and all 28 referenced callees have real French inventory owners and
complete bodies identical to the independently hash-verified Spanish resident.

The six images retain 18 real input/final data owners, including model 141's
90-byte descriptor and 1,398-byte unclassified suffix in one aligned input
object. The suffix remains section-defined, not an absolute alias; it is not
counted as C or as a descriptor extension. Model 8 was also recovered separately
before shared-source integration: its only instruction mismatch was corrected
by the signed packed-texture view required by the retail halfword load.
No independent C copy is retained in production.

Together with the separately verified final duel-bank helper, configured French
coverage is now 234 of 234 inventoried C instances / 155,572 instruction bytes
across 28 images. Return-two primaries, variant phases, secondary MODEL/SU loads,
boot/overworld ownership and all unclassified tails remain outside that claim.

## North American registration

All twenty Spanish images have North American counterparts at the same sector
offsets of the North American `MODEL.MRG` (SHA-256
`6a738960cfae167d6612f190c07cf0147f3bcc8c2dd97814e154fecc283e1d4b`, the hash
`config/slus_01411/files.sha256` already pins). Compared word by word, the
North American 4,096-byte payloads differ from the Spanish ones only in `jal`
targets into the resident executable and in the first header word, which is
data. Every Spanish binding maps to exactly one North American target across
all twenty images. Every target is the address the North American executable
already gives that name, for example `Model_GetActiveSlotIndex` at `0x80058DCC`,
`Model_GetSlotAnimationIndex` at `0x80058E68`, `func_8005D994` at `0x8005D994`
and `MoveImage` at `0x8007FA38`.

The North American modules are registered without a region prefix
(`model_primary_<model>_slot<slot>`), like the other North American overlays.
They reuse the Spanish source units, layouts, descriptor owners and symbol
files unchanged; only the archive, the image hashes and the resident bindings
differ. `make verify-overlays` rebuilds all twenty images byte-identically.
They add 20 functions and 16,432 instruction bytes, bringing configured North
American overlay coverage to 206 of 208 function instances across 27 images.
This is not exhaustive MODEL coverage: the variant modules, the secondary
loads and every unclassified tail remain open.
