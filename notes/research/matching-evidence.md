# Matching Type and Code-Generation Evidence

## Purpose

This note preserves recurring evidence recovered while matching larger game
functions. It records concrete widths, strides, relocation behavior, and
compiler idioms that should guide untouched functions. Address-based names
remain in use because the original semantic names are unknown.

Exact matching functions are the strongest anchors. Layouts inferred only from
nonmatching candidates are identified as provisional and must be corroborated
before they become shared C types.

## GCC 2.8.1 code-generation patterns

### Data placement and address formation

- `%gp_rel` byte and halfword globals require a `gcc_2_8_1_g8` profile.
- Large arrays and structures referenced with `%hi`/`%lo` pairs require G0
  declarations or declarations whose size exceeds the small-data limit.
- Use a split-address profile when the target retains a symbol's high half
  across intervening instructions before applying the low relocation.
- Mixed absolute and GP-relative references can still require a G8 profile;
  classify each symbol independently instead of assigning a profile from one
  global.
- `-msplit-addresses` is a per-translation-unit compiler flag, not a maspsx
  setting; the three G0 profiles pass identical maspsx flags and differ only in
  `-msplit-addresses` versus `-mno-split-addresses`. A function therefore cannot
  mix the two address forms, and a target that needs both is not reachable from
  the current profile set.

#### Targets that need both address forms

Some functions require macro form for a direct scalar load and split form for a
symbol-indexed load in the same body. Because the flag is per-translation-unit,
no profile satisfies both, and each choice leaves a different residual.

The two forms are easy to tell apart in the target:

- macro form reuses the destination register as its own address temporary, as in
  `lui $a0, %hi(sym)` followed by `lb $a0, %lo(sym)($a0)`. Because no spare
  register survives, a second read of the same global must re-materialise the
  address, which is itself a useful signal.
- split form keeps the symbol in a register of its own and adds an index to it,
  as in `lui $v0, %hi(sym)` / `addiu $v0, $v0, %lo(sym)` / `addu $v1, $v1, $v0`
  / `lb $v0, 0($v1)`, four instructions in total.

Under `-mno-split-addresses` GCC emits the indexed load as the macro
`lb $v1, sym($v1)`, which the assembler expands through `$at` in three
instructions rather than four.

The conflict shows up in two different ways, so both are worth recognising:

- `Duel_GetTerrainBoost` (`0x8002497C`) ends one instruction short, at 24 of 25.
  Its two `gDuel_bTerrain` reads are correct only under a non-split profile,
  which is also what forces the second read to re-materialise its address, but
  the indexed table load then collapses to the three-instruction `$at` form.
- `Ai_GetHandSize` (`0x80070710`) keeps the correct instruction count of 10
  under a split profile, and differs only in which register carries the high
  half: the target reuses the load's destination, as macro form does, while
  split form allocates a separate register for it.

A shortfall in the instruction count is therefore not the only symptom of this
conflict; a same-length body whose only fault is the address temporary is the
same problem seen from the other side.

Pinning a base pointer to a hard register does not help: GCC folds the pointer
back into a symbol-indexed load and the pin is optimised away.

#### How much the address temporary tells you about the profile

The two forms above are worth measuring rather than eyeballing, because the
signal is strong in one direction only. Scanning the matched corpus for every
`lui $X, %hi(sym)` immediately followed by a load whose base register is also
`$X`, and splitting those pairs by the profile each function is recorded
under, gives:

| Profile group | destination reused | separate register | pairs |
| --- | --- | --- | --- |
| non-split | 299 (99.3%) | 2 (0.7%) | 301 |
| split | 14 (60.9%) | 9 (39.1%) | 23 |

Read this asymmetrically:

- a **separate** register is roughly fifty times more likely under a split
  profile than a non-split one, so seeing one in the target is strong evidence
  for split before any C is written.
- **reuse** is only weak evidence for non-split. It is what non-split almost
  always does, but split still produces it in a clear majority of cases, so it
  cannot on its own rule split out.

The practical consequence is that reuse in the target does not justify
abandoning a split profile that is otherwise scoring well. `func_8004A27C`
(`0x8004A27C`) is the worked example: the target reuses the destination at its
`D_8009B458` load, yet the split profile reaches 12 differences at the correct
31 instructions with every register allocated as retail has it, while the
non-split profile that reproduces the reuse sits at 20 and misallocates the
`mflo` pair. Only the separate-register case should be treated as decisive.

Reproduce the table by pairing `config/slus_01411/matching_c.json` against the
built executable; file offset is VRAM minus `0x8000F800`.

#### Comparing profiles: count parity beats diff size

When choosing between profiles, rank candidates by whether the instruction
count equals the target's, not by how small a normalised diff is. The two
disagree, and the diff is the misleading one.

A profile that renders a `%gp_rel` load as `lui` plus `lw` adds an instruction
the target does not have. Every later instruction then shifts by one, but the
extra instruction also pads the body, and a diff that compares position by
position can report fewer differences for the profile that is further from a
match.

`func_800472A8` (`0x800472A8`) is the worked example: it needs `gcc_2_8_1_g8`
because `g_SDValue` is `%gp_rel`, yet a G0 profile scored better on a naive
diff while being strictly wrong. Once the correct profile was used the count
matched at 27 and every remaining difference was cosmetic.

Prefer instruction-count parity first, then compare the bodies. "The diff got
smaller" is the natural way to pick a profile and it will rank the wrong one
first whenever address form changes how many instructions a load costs.

#### Separate register-permutation residuals from schedule-permutation ones

A body whose instruction multiset, registers and relocations are all exact but
whose order is wrong is a different blocker from one whose order is exact and
whose registers are wrong, and the two want opposite fixes. Record which of
them a terminal residual is, because the pin advice under "Register pins" only
applies to the second.

`func_8004A764` (`0x8004A764`) is the schedule case. Its six terminal attempts
were all recorded under `gcc_2_8_1_g0`, but the matched neighbour
`func_8004A7C0` uses `gcc_2_8_1_g0_split`, and the target's
`lui`/`addiu %hi`/`%lo(D_80011434)` pair is the split form. Under the cohort
profile, and with the `Packet` type and the `func_80077450(Packet *)` prototype
already recovered in `src/game/sound_secondary_playback.c`, the body comes out
at 23 of 23 instructions, 0x5C of 0x5C bytes, with identical registers and
identical relocations. Everything from `sw $v0, 0x4C0($v1)` to the epilogue,
including the `sw $v0, 0x4E4($v1)` branch-delay store, is byte-exact. Only the
11-instruction prologue is ordered differently: retail finishes the
`D_80011434` address arithmetic before materialising the `D_8009B458` high
half, and sinks `sw $ra` below `ori $a1`.

Six source shapes were probed against it. Stores taken through the packet
pointer collapse to small offsets off `$a0` and are the wrong shape outright.
Stores taken through the state pointer, with the cast applied only at the call,
are the right shape; a typed overlay struct with a named `packet` member
compiles to the same thing and reads better. The two shapes that fix the
prologue order both break the allocation instead: hoisting the loaded value
into a local moves it to `$v1` and the state to `$a0`, and hoisting the element
address moves `addiu $a0, $a0, 0x4C0` into the delay slot. `-fno-schedule-insns`
and `-fno-schedule-insns2` each produce a third order that is no closer.

So every shape that fixes the schedule breaks the allocation, and every shape
that keeps the allocation keeps the schedule. Pins do not help here: the
allocation is already correct. A residual of this kind is compiler-side, and
the next attempt on it needs a `cc1` whose list scheduler tie-breaks
differently, not a seventh C variant.

`func_8004A6F8` (`0x8004A6F8`) is the same function with three `u16` fields read
from a second parameter instead of constants, and it lands in the same place
from the other direction. Reading the table entry into a local before the state
pointer gives its 27 instructions and 0x6C bytes; pinning that local to `$2` and
the state pointer to `$3` then makes every register correct. 23 of the 27 are in
place, and the four that are not are one permutation: the target issues the
state load immediately after its `lui` and the candidate fills that slot with
`sw $ra`. So pins can finish the allocation half of this residual even when the
schedule half stays out of reach — worth doing, because it narrows what the next
attempt has to explain.

#### Terminal histories recorded before the profile system are not terminal

`config/slus_01411/attempts.csv` has two eras. Later rows name a profile from
`compiler_profiles.json` in both the `compiler` and `flags` columns; earlier
rows name a toolchain and a free-form phrase, such as
`gcc-2.8.1-psx / -O2 -G8 early-return`. 268 addresses carry at least one row of
the older kind, and 161 of those are still `unmatched_asm`. Those histories
never tried their cohort's profile, so their six attempts do not mean what a
profile-era six means.

`func_80013B04` (`0x80013B04`) shows the difference. Its six rows are all
free-form, and every one of them blames the branch orientation: "inverted the
busy branch", "moved the null return to the shared epilogue". Both claims are
wrong. Under `gcc_2_8_1_g8_split` — the profile its matched neighbour
`func_80014A5C` uses, and the one the target's `%hi`/`%lo(gFile_anLba)` pair
requires — writing the guard positively puts the null return inline exactly
where the target has it:

```c
if (((D_8009B0F4 & 0x2000030) | D_8009B134) == 0) {
    transfer = &D_800E9E60;
    transfer->state = 0;
    D_8009B0F4 = 0x100010;
    transfer->field_24 = gFile_anLba[file_index] + sector_offset;
    return transfer;
}
return 0;
```

That is 25 of 25 instructions, 0x64 of 0x64 bytes, the target's registers, the
target's relocations, `%gp_rel` on both `D_8009B*` reads, and the target's block
layout. What is left is a `sched2` permutation: the target hoists
`addu $a3, $a1, $zero` to instruction 1 and fills the delay slot with
`lui $a1, 0x10`, and it interleaves the two address pairs in the body rather
than completing each one.

So the recorded blocker was an artefact of the wrong profile, and the real one
is the same schedule residual as above. When a pre-profile history is the only
history an address has, the cheapest new evidence is its cohort's profile, and
`matching_c.json` gives that for free from any matched neighbour.

### Disassembly artifacts

Splat names any address-shaped literal as though it were a symbol. A `%hi`/`%lo`
pair against a symbol with no definition anywhere in the image is therefore a
constant that splat has guessed at, and its two instructions should be read
independently rather than as one materialisation.

`func_8005C5D4` (`0x8005C5D4`) was deferred after six attempts for this reason.
Its `lui $v1, 0x80` and `addiu $v1, $v1, -1` were read as a single `0x7FFFFF`
named `D_7FFFFF`, and asking GCC for `0x7FFFFF` yields `lui 0x7F` + `ori 0xFFFF`,
which no profile bends into the retail pair. The value is really `0x800000` with
the loop's first decrement peeled out ahead of the loop label, corroborated by
the compensating `addiu $v1, $v1, 1` on the early exit.

### Signed values and arithmetic

- Signed bytes are commonly loaded with `lbu` followed by `sll 24` and
  `sra 24`. A plain `signed char` expression often reproduces this shape.
- Signed division by ten uses the `0x66666667` multiply-high sequence.
- Division of a signed halfword by two sign-extends the halfword, adds the sign
  correction, and shifts right to preserve C truncation toward zero.
- Fixed-point power-of-two division rounds negative products by adding
  `divisor - 1` before the arithmetic shift.
- Values cast back to signed bytes may require an explicit C cast to recover
  the target's final `sll`/`sra` pair.

### Structures and control flow

- Packed eight-byte structure assignments can generate
  `lwl`/`lwr`/`swl`/`swr`; replacing them with two aligned words changes the
  instruction stream.
- Local absolute jumps carry `R_MIPS_26 .text`, so branch order, fallthrough,
  and duplicated epilogues are part of the match.
- Input values retained across calls naturally occupy `$s0` and then
  `$s1`-`$s3` in declaration/use order.
- Three-way state initializers are especially sensitive to assignment order
  and whether the original source was a switch or nested conditionals.

### Compiler-generated jump tables cannot currently be integrated

A `switch` that GCC compiles into a jump table cannot be accepted by the build
as it stands, however exact the C is. Check for this before starting a function,
because the C can be finished and still be unusable.

The symptom is a link failure rather than a mismatch:

```
ld: section .initialized_data LMA [00080ee0,0008b88f]
    overlaps section .rodata LMA [00080ed8,00080ef3]
ld: section .rodata VMA [800906d8,800906f3]
    overlaps section .text_padding VMA [800906d4,800906df]
```

Two facts combine to cause it:

- `linker/slus_01411.ld` defines no `.rodata` output section, so a compiler
  table becomes an orphan and is placed immediately after `.text`, on top of
  `.text_padding` and `.initialized_data`.
- The retail table is already in the image as data. For `Ai_GetWinningCardRange`
  it lives at `0x8001194C`, which falls inside `.initial_data`
  (`0x80010000` for `0x29D8`), a single blob emitted from `initial_data.o`.

So accepting one would require splitting that blob around the table's address
and placing the object's `.rodata` into the resulting hole, which changes the
data emission and the size assertions. That is a build-configuration decision,
not something to solve by trying more source shapes.

`Ai_GetWinningCardRange` (`0x80070738`) is the worked example. Its C reaches all
35 instructions exactly, and the emitted `.rodata` is `0x1C` bytes - exactly the
seven words of the retail table - so it is the right table at the wrong address.

Confirming this is a new case rather than an oversight: `symbols.txt` contains no
`jtbl_` symbols, no source under `src/game` references one, and every matching
source that contains a `switch` compiles to a comparison chain instead of a
table. Small or sparse switches are therefore fine; only a dense one that GCC
turns into a table hits this.

When reading a target, the tell is a `lw` from a `%hi`/`%lo` symbol pair
followed by `jr` on the loaded register.

### Register pins

Issue #5 accepts `register` variables pinned to a hard register for functions
that are otherwise unmatchable. Two things are worth knowing before reaching for
one.

#### Fixing allocation is a smaller claim than coercing a sequence

Distinguish the two cases when recording a pinned match, because they are not
equally strong:

- the instruction sequence is already exact and only the register assignment
  differs, so pins correct **allocation**;
- the sequence itself is wrong and pins are used to force it, which is a much
  larger intervention.

`func_80026C0C` (`0x80026C0C`) is the first kind: all 24 instructions were in
the right order before any pin, and only the base index and record pointer were
swapped between `v1` and `a0`. State which case applies in the ledger row, since
a reader deciding whether to revisit a function cannot tell them apart from the
pin count alone.

#### Pins cannot place a split-address high temporary

A pin also does not reach the `%hi` temporary that `-msplit-addresses` creates
for a scalar load. That temporary is a compiler-internal pseudo, not a C
variable, so no `register` declaration names it.

`Ai_GetHandSize` (`0x80070710`) is the worked example. Pinning only the array
base to `$2` corrects both wrong roles of its 10-instruction body at once:

```c
register s8 *data asm("$2");

data = gDuel_aOpponentData;
return data[gDuel_bOpponentID * 9];
```

The base moves to `v0`, the index accumulator to `v1`, and the opponent-ID
value is already `a0`. Nine of the ten instructions are then byte-exact. The
one that is left is the address temporary of the ID load:

```
target      lui $a0, %hi(gDuel_bOpponentID) ; lb $a0, %lo(...)($a0)
candidate   lui $v1, %hi(gDuel_bOpponentID) ; lb $a0, %lo(...)($v1)
```

Retail coalesces the high temporary with the load destination; GCC 2.8.1 hands
it the next free register instead. Three ways of reaching for it fail:

- pinning the loaded value to `$4` as well changes nothing, because the
  temporary is a different pseudo from the pinned variable;
- pinning `&gDuel_bOpponentID` to `$4` and loading through it changes nothing,
  because GCC folds the pointer back into the symbol reference;
- pinning the accumulator to `$3` alone, or all three roles at once, is
  strictly worse: the first loses the split address form and the second emits
  `addu $v0, $v1, $v0`.

The practical rule matches the one below: if the register you want to control
holds a `%hi` address temporary rather than a value the source names, a pin is
the wrong tool.

#### Pins cannot place a call's return value

A pin works for a value the function **computes**. It does not work for a value
the function **receives from a call**: the return register is fixed at `v0`, and
GCC 2.8.1 will not make a pinned variable the direct destination of the call
result. It copies through a scratch instead, emitting

```
move a2,v0
move a0,a2
```

where the unpinned form emits a single `move`. The pin therefore *adds* an
instruction rather than relabelling one. Splitting the declaration from the
assignment does not avoid it:

```c
register PoolEntry *entry asm("a0");
entry = func_8002C5CC();          /* still copies through a scratch */
```

`func_8002C604` (`0x8002C604`) is the worked example. Its remaining difference
is that retail keeps the returned pointer in `a0` while GCC uses `a1`; pinning
raises the instruction count from 34 to 35 instead of fixing it, so that
difference is not reachable by pinning at all.

The practical rule: if the register you want to control holds a call result, a
pin is the wrong tool and will cost an instruction.

### Fixed-size builtin copies

Exact `func_800476B4` confirms that GCC 2.8.1 expands an 8-byte
`__builtin_memcpy` between alignment-one pointers into the retail
`lwl`/`lwr` and `swl`/`swr` pairs. This is a useful source-level lever when a
manually written pair of 32-bit copies produces the right data movement but
the wrong unaligned load/store schedule.

### GTE instructions

Projection helpers use scratchpad address `0x1F8003E0`, load GTE data
registers 0 and 1, execute RTPS, and read data register 14. GNU `as` does not
accept the historical `rtps` spelling in this pipeline, so matching C uses the
validated inline word:

```c
__asm__ volatile(".word 0x4A180001");
```

`func_80015D18` is the current matching template for this pattern.

## Confirmed and strongly supported layouts

### Transform and card data

| Base | Evidence-backed layout |
|---|---|
| `D_800F2848` | Signed 16-bit transform angles/parameters at `+0`, `+2`, and `+4`; object is larger than eight bytes |
| `D_801D4244` | 32-bit card/property table indexed by signed 16-bit ID minus one |
| `D_800908A0` | Array of signed 16-bit coordinate pairs |
| `D_801A7AD8` | `0x1C`-byte entries: pointer/value at `+0`, signed ID at `+0xC`, unsigned flags at `+0x16` |

Observed `D_801D4244` property fields include:

```text
value >> 18 & 0x0F
value >> 22 & 0x0F
value >> 26 & 0x1F
```

### Object and event state rooted through `D_8009B458`

`D_8009B458` and `D_8009B45C` are global pointers using absolute
`%hi`/`%lo` relocation pairs.

Strongly repeated layouts include:

- Event queue records are `0x30` bytes. Their type byte is at offset zero.
- A resource/object array at root `+0x180` uses `0x28`-byte entries.
  Repeated fields occur at `+0x183`, `+0x18D`, and `+0x19E` relative to the
  root.
- A transfer state at root `+0x4A4` has a signed 16-bit ID at `+0`, source
  pointer at `+4`, lengths at `+0x10/+0x14`, byte fields at `+0x18/+0x1B`,
  and accumulated transfer count at root `+0x818`.
- State cleanup around `func_80049010` clears sequence flags and counters near
  root `+0x1578` through `+0x1588`.

These offsets are supported by multiple callers and matching neighbors, but a
single final aggregate type has not yet been committed.

### Large `0xE20` records

Functions around `0x80058xxx-0x80059xxx` repeatedly use records with stride
`0xE20`.

Observed fields and subregions include:

- An alignment-one eight-byte block at `+0xDC8`.
- Per-slot transform records of `0x50` bytes beginning at `+0xD14`.
- Signed clamp/state fields at `+0xDA0`, `+0xDA4`, `+0xDA8`, and `+0xE11`.
- A relocation/pointer area near `+0x1E0`.
- A count byte at `+0xE1B`.

`func_80059000` and `func_8005C6A0` are matching anchors for portions of this
layout. The full record remains only partially typed.

### `0x70`-byte allocator objects

Analysis around `func_800400AC` supports:

- Object stride `0x70`.
- Linked-list heads at `D_800EFE38` and `D_800F2878`.
- Used flag `0x80`.
- Default type value 2.
- A type lookup through `D_8009AF74`.
- A pointer into the tail-data region.

The allocator itself is still deferred, so these fields are provisional.
Exact `func_80038530` independently confirms that nearby stream handlers use
absolute G0 references for `D_8009B360` through `D_8009B374`.

### Two-slot object controller

Exact `func_8003D614` confirms a controller with:

- Object pointers at `+0` and `+4`.
- Type/index field at `+0x1A`.
- `0x64`-byte records rooted at `D_800EB0F8`; exact `func_8003F2B0`
  independently confirms the stride through a raw `u8 *` view and explicit
  multiplication by 100 rather than through the typed struct.
- Signed 16-bit motion fields.

### Transfer and animation anchors

Exact `File_InitTransferDescriptor` confirms the `0x48`-byte transfer
descriptor constructor used by `func_80013940`. Public arguments five through
eight arrive on the stack, and the vertical/configuration argument selects
direct, negative, or fixed-point initialization behavior.

Exact `func_80019BD0` confirms animation-object fields including:

- Flags at `+8`.
- Current and mode bytes at `+0x21/+0x22`.
- Callback pointer at `+0x24`.
- Threshold halfword at `+0x2E`.
- Object index at `+0x6A`.
- State field at `+0x6C`.

Exact `func_8001B7AC` confirms that `D_800EA030` uses `0x0C`-byte entries
selected by a signed byte.

### Coordinate and constructor anchors

Exact `func_8005A1F4` and `func_8005A2E0` confirm two unsigned-halfword
coordinate triplets in `D_800F56F0`, at offsets `0/4/8` and `0xC/0x10/0x14`.
Both functions copy an alignment-one eight-byte vector packet, accumulate
three signed square terms, and call `func_80086E50`.

Exact `func_800610E0` and `func_800611D0` reinforce the `0x70`-byte allocator
object model through two related parent/child constructors.

## Matching anchors from the deep wave

| Function | Reusable evidence |
|---:|---|
| `File_InitTransferDescriptor` | Transfer-descriptor constructor and stack argument order |
| `func_8001306C` | Nullable callback array, GP-relative callback, pacing counters, and 60-tick countdown |
| `func_8001EE44` | Signed card ID indexing and conditional 4-bit property extraction |
| `func_80021480` | Ten-child object iteration and bit `0x40` state updates |
| `func_80019BD0` | Animation object field widths and callback layout |
| `func_8001B7AC` | `0x0C`-byte global entry selection and child linkage |
| `func_80028310` | G8 state transition with child creation and cleanup |
| `func_8002ABB4` | `0x70`-byte object clone/initialization wrapper |
| `func_8002DF2C` | Three archive layouts selected by high byte; packed decimal index calculation |
| `func_8002E060` | Object creation wrapper with signed mode byte |
| `func_8002EB78` | G8 stream state with split absolute `0x4C`-byte table entries |
| `func_80030D5C` | G8 state machine mixing GP-relative state and absolute flag word |
| `func_800375A4` | Signed countdown state and object cleanup |
| `func_80037A58` | Signed duration, randomized coordinate snapshot, and restoration |
| `func_80038530` | Four direct byte-stream reads with absolute G0 globals |
| `func_8003D614` | Two-slot controller and `0x64`-byte object records |
| `func_80043230` | G0 pointer-rooted queue/object state |
| `func_80044DC0` | Signed 16-bit argument, four-byte stack packet, and byte-order selection |
| `func_80049010` | Shared sequence-state cleanup |
| `func_800497E0` | Transfer ID validation, clamped read length, and accumulated byte count |
| `func_80049CF8` | `0x28`-byte object loop and split table-base relocation |
| `func_8004C77C` | `0x2C`-byte record initialization and variable-length decoding |
| `func_80058A7C` | Mixed signed comparisons and unsigned halfword bit extraction |
| `func_80059000` | `0xE20` record stride and unaligned eight-byte copy |
| `func_80059CE4` | G8-gated teardown while preserving an absolute state pointer |
| `func_8005A1F4` / `func_8005A2E0` | Coordinate triplets, packed vector copy, and signed square accumulation |
| `func_8005C6A0` | Relocation enumeration and translated handler count |
| `func_8005F828` | Nullable eight-byte record iteration and repeated throttle-byte loads |
| `func_800610E0` / `func_800611D0` | Related constructors using `0x70`-byte allocator objects |

## Deferred-function guidance

The following evidence narrows future analysis but does not authorize a seventh
variant. Deferred functions remain terminal under the six-attempt policy.

- `func_8003D334` reached exact size and relocations; its remaining mismatch is
  scratchpad-packet initialization and register scheduling.
- `func_8003A990` is four bytes short and differs mainly in allocation plus one
  reload around signed division by `0x400`.
- `func_8003AAE4` matches its initialization/call prefix; the remaining
  mismatch is phase-register and color-replication ordering.
- Future untouched handlers in the module surrounding exact
  `func_80038530` should start from G0 direct byte-stream reads rather than
  wrapper helpers.
- Future untouched allocator/list users in the module surrounding deferred
  `func_800400AC` should begin with the provisional `0x70`-byte layout and a
  G8 split-address profile.
- Four-state callbacks in the `0x8003Bxxx` module use nested branch trees,
  while five- and six-state callbacks use explicit jump tables. Absolute
  destination tables in those callbacks require G0; G8 produces truncated
  `R_MIPS_GPREL16` relocations.

If a new exact neighbor, original type declaration, or compiler artifact later
changes one of these conclusions, record that evidence before revisiting any
terminal function.
