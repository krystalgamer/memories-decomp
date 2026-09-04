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
- `0x64`-byte records rooted at `D_800EB0F8`.
- Signed 16-bit motion fields.

### Transfer and animation anchors

Exact `func_80013998` confirms the `0x48`-byte transfer descriptor constructor
used by `func_80013940`. Public arguments five through eight arrive on the
stack, and the vertical/configuration argument selects direct, negative, or
fixed-point initialization behavior.

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
| `func_80013998` | Transfer-descriptor constructor and stack argument order |
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
