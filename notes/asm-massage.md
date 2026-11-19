# Assembly massage for hand-written renderers

Status: integrated for the twelve wireframe drivers of
`src/game/wireframe_renderers.c`, the sixteen shared Gt3/Gt4 drivers of
`src/game/shared_polygon_renderers.c` and the thirty-two direct polygon
drivers of `src/game/polygon_renderers.c`, which build through the
`gcc_2_8_1_o2_massaged` profile in every resident executable (see
[Build integration](#build-integration)). Only the shared cache-preparation
stage (`src/game/shared_primitive_cache_prepare.c`) still uses the
register-variable C of the `gcc_2_8_1_g0_handwritten_asm` profile; see
[Next steps](#next-steps) for why.

The tool is `tools/project/asm_massage.py`, with its rule catalogue in
`tools/project/asm_massage_rules.py`, the dependence checker in
`tools/project/asm_massage_deps.py` and unit tests in
`tools/project/tests/test_asm_massage.py`. Edit files live beside their
source, in `src/game/wireframe_renderers.massage/`,
`src/game/shared_polygon_renderers.massage/` and
`src/game/polygon_renderers.massage/`.

## Motivation

The model renderers (`src/game/wireframe_renderers.c`,
`src/game/polygon_renderers.c`, `src/game/shared_polygon_renderers.c` and
`src/game/shared_primitive_cache_prepare.c`, about 60 functions) were written
in assembly. Without the massage they match through register variables,
`handwritten_asm.h` and `gte_native.h`: C that spells out the original's
registers and instructions and would not run anywhere else.

The massage keeps the C plain and moves the hand optimisation into an
explicit, reviewable post-pass:

- the C is portable C over the stock Psy-Q `inline_c.h` GTE macros and the
  `libgpu.h` primitive macros, with no `register`/`asm`, no
  `handwritten_asm.h`/`gte_native.h`, no type punning and no endianness
  tricks;
- it is compiled with exactly the fixed flags (`-O2 -G0 -mel -mips1
  -mcpu=R3000 -msoft-float -fno-builtin -Dpsx -D__psx__ -D__psx`, one level
  for the whole family);
- shared, named rules rewrite GCC's code into the original's idioms, a
  per-function register map assigns the original's registers, and a small
  per-function residual diff supplies the rest (mostly scheduling);
- the result must assemble to the retail bytes with the same relocations.

## Pipeline position

```text
C --gcc -O2 -S--> .s --asm_massage (unit edit files)--> .s --GNU as -EL -mips1 -G0--> .o
```

In the build this is `compile_massaged_c` in
`tools/project/build_baseline.py`, used for every unit whose profile sets
`"asm_massage": true`, in place of the MASPSX path.

The massage consumes GCC's raw `-S` output and writes a fully explicit
`.set noreorder` / `.set nomacro` / `.set noat` file in which every line is
one machine instruction (explicit `%hi`/`%lo`, explicit delay slots), so GNU
as encodes it one-to-one.

MASPSX is not in this pipeline. Its job is to make GNU as behave like ASPSX:
expand assembler macros the way ASPSX did and insert the load-delay and
delay-slot nops ASPSX inserted. The massage does both itself, and must:

- its pre-pass expands the few pseudo-instructions GCC emits (`li`, `la`,
  `move`, `negu`, `not`, symbol loads/stores, `addu`/`subu` with an
  immediate including the `$sp` frame adjust, immediate logic/compare forms,
  `beqz`/`bnez`/`b`, `j $r`, shifts by a register) and rejects the rest
  (`div`/`mul`/`rem` macros); each branch in a `.set reorder` region gets
  the delay-slot nop ASPSX would have added;
- load-delay nops depend on the final order, which only exists after the
  rules and the residual, so they are always derived last
  (`insert_hazard_nops`); MASPSX's nops would only be stripped again.

Running MASPSX first changes nothing in the results (identical register maps
and residuals for Ft3, Gt3 and Ft4), but would add a stage whose output the
massage throws away. The existing per-profile `assembly_filter` hook (between
GCC and MASPSX) cannot host the massage either, because MASPSX would then
re-process the hand-ordered stream; hence a profile whose C path replaces
MASPSX with the massage (see [Build integration](#build-integration)).

## Processing steps

1. Pre-pass: parse the function from GCC's output, expand pseudo-instructions
   to machine form (`lw r,sym` as `lui r,%hi; lw r,%lo(r)`, `sw r,sym`
   through `$at`, `j $31` as `jr $ra`, ...; see `parse_instruction`) and give
   reorder-mode branches an explicit nop delay slot.
2. Split registers into def-use webs: `$v0.3` is the third live range GCC
   put in `$v0`, `$a0.0` is a live-in. Returns implicitly read `$v0` and the
   callee-saved registers.
3. Run the `web` rules (catalogue below).
4. Rename webs to physical registers: the edit file's `[registers]` map,
   otherwise GCC's own register. A web whose only reader defines the next web
   in the same GCC register (`lui`/`lw` pairs, `srl`/`sll`/`addu` index
   chains) continues that chain, so one entry names the whole chain.
5. Run the `physical` rules.
6. Number labels `.L1`, `.L2`... in order and apply the residual diff.
7. Re-insert load-delay nops where the final order needs them.

## Rule catalogue

Every massaged function (the twelve wireframe drivers, the sixteen shared
polygon drivers and the thirty-two direct polygon drivers) runs the same
list, in this order:

```text
strip-nops aligned-word-copy dmpsx-native gte-direct-read gte-direct-write
unspill invariant-last fold-address undo-strength-reduction word-compose
halfword-words tag-build loop-load-forwarding at-constants
at-branch-constants sign-extend-load zero-extend-shifts reuse-load
zero-as-addiu increment-sink loop-constant store-delay-slot gte-load-early
dce                                                       (web stage)
drop-self-moves frameless guard-chain preheader-order fill-load-delay
fill-branch-slot fill-jump-slot                           (physical stage)
```

| Rule | Stage | Rewrite | Why it is safe / why the original looks like this |
| --- | --- | --- | --- |
| `strip-nops` | web | drop nops outside delay slots (the `nop` inside `gte_stflg`), except the GTE waits in front of a command (`gte_nclip()`) | step 7 re-derives load-delay nops from the final order; GTE waits are not load delays |
| `aligned-word-copy` | web | adjacent `lwl`/`lwr` and `swl`/`swr` pairs (GCC's copy of a 2-byte-aligned `DVECTOR`) -> `lw`/`sw` | GsWORKUNIT cache entries and packets are word aligned (A3) |
| `invariant-last` | web | inside a loop, a commutative op with one loop-invariant operand writes it last (`addu t5,index,vertices`); runs after `unspill`, so a reloaded invariant counts | operand order is free; this is the original's spelling everywhere |
| `dmpsx-native` | web | `inline_c.h` command markers -> native COP2 words (RTPS, RTPT, NCDS, NCCS, NCCT, NCLIP, AVSZ3, AVSZ4) | exactly what DMPSX did; an unknown marker is an error |
| `gte-direct-read` | web | `cfc2 $12,$31; sw` / `swc2 $7`/`$24` to a stack variable and the C's later `lw` -> `cfc2`/`mfc2` straight into the reader's register | straight-line code, one read per store (a reused variable such as two `gte_stflg(&flag)` pairs each store with its own read), no GTE command in between |
| `gte-direct-write` | web | a C variable stored to a stack slot only so `gte_ldrgb(&rgb)` can `lwc2` it -> `mtc2` from the variable's register | straight-line, the slot has no other access and the register is unchanged in between; the direct drivers' `mtc2 template,$6` |
| `unspill` | web | GCC's stack spill of a value that does not change after the store: reloads read the value's register (a byte or halfword reload too, when every definition is a `srl` that fits it, as `code = poly >> 24`); a reload feeding a merge (`colour = poly` or the semi template) becomes a register copy; a never-reloaded spill goes. A slot stored more than once (a loop-carried value, the spilled argument pointer) becomes a register: stores and loads turn into copies the register map coalesces | exact: the slot holds that value at every reload |
| `fold-address` | web | `addiu t,b,K` + accesses at `0(t)` -> `K(b)` | `gte_stsxy0(&packet[3])` passes an address; `t` must be single-use and `b` unchanged |
| `undo-strength-reduction` | web | GCC's biased (`record+18`) or duplicated (`packet+3`) loop pointers -> offsets from the real pointer | constant pointer difference proven by dataflow at every use |
| `word-compose` | web | a word composed from adjacent zero-extended fields -> one `lw`: the three draw-mode globals, a GsWORKUNIT's `otz`/`p` pair or its `vx`/`vy` SXY word | global adjacency and alignment checked against `config/slus_01411/symbols.txt`; structure fields need a word-aligned offset (A3) |
| `halfword-words` | web | `lhu` record halfwords of one loop -> `lw` of the containing words at the first load (which must dominate the others) + `srl 16` / `sll 16; srl 16-k` (fusing the `<<3` scale); a record that would need more than four word registers packs two lone high halves with `lw x,k; lwr x,k-2` (the GT4 polygon `norm0`/`vert0`). A load past a branch whose word holds no other index, is not packed and is read whole elsewhere stays an `lhu` (the FT4 `norm0`, beside `u3`/`v3`) | little-endian target; the words are only hoisted past stores through other pointers (A2) |
| `tag-build` | web | `setlen` + `setaddr`/`addPrim` byte/mask sequence -> `lw; sll 8; ori len; srl $at,8; sll 24; or; sw tag; sll 8; srl 8; sw ot` (the OT store may sit in a delay slot) | the OT entry is read where the C reads it (A1) |
| `loop-load-forwarding` | web | a loop that reloads a word it stored on every back edge (the second pass re-reading the OT entry it just linked) keeps the stored value: the load moves in front of the loop | every back edge's last store to the word is the register's value |
| `at-constants` | web | a large constant used only as an ALU operand is built in `$at` before each use (`lui $at,0xFF9F; ori; and t1,t1,$at`), not hoisted into a register; a use GCC put in a delay slot leaves the slot (a nop stays) with its constant, and a register copy keeps its `lui` | what the assembler's immediate macros produced; `$at` must be free there, and the slot use's result unread on the taken path |
| `at-branch-constants` | web | a small constant compared by `beq`/`bne` against a loop-invariant value is built in `$at` before each branch (`addiu $at,$zero,1; beq mode,$at`) | the assembler's `beq reg,imm` macro; a constant the loop compares fresh loads with (the shared drivers' `-1`) keeps its register |
| `sign-extend-load` | web | `lbu`/`lhu` whose only use is `sll 24`/`16; sra` -> `lb`/`lh` into the result | the `(s8)gModel_bCurrentDrawMode` casts; the result web has no other definition |
| `zero-extend-shifts` | web | `andi d,s,0xFFFF` -> `sll 16; srl 16`; in a delay slot the `sll` stays in the slot | the hand-written code never uses `andi` for this; `d` must be dead at the branch target |
| `reuse-load` | web | a second `lw` of the same word through an unchanged pointer reuses the first | GCC reloads because a packet store might alias the cache (A2) |
| `zero-as-addiu` | web | `move d,$zero` -> `addiu d,$zero,0` (`li d,0`) | identical value |
| `increment-sink` | web | a pointer increment moves to the end of its block, past the accesses through it (their offsets grow), but never past another increment | pure re-addressing |
| `loop-constant` | web | a small `addiu r,$zero,imm` that only one loop reads is set at the top of the loop body, not hoisted by GCC's loop-invariant motion | re-materialising a constant |
| `store-delay-slot` | web | a store right before a branch with an empty slot moves into the slot | it ran on both paths before and still does |
| `gte-load-early` | web | an `mtc2` moves up to just after the test of its own value, into that test's empty delay slot unless a store waits there; a value no earlier test reads does not cross a test (`gte_ldsz3`/`gte_ldsz4` writes, one per vertex) | only the skip path sees the extra SZ writes, and it reloads them before any AVSZ (A4) |
| `dce` | web | delete pure instructions whose results are unread | |
| `drop-self-moves` | physical | `addu r,r,$zero` after renaming | GCC's `move $11,$4` once both are `$a0` |
| `frameless` | physical | delete the `$sp` frame; park the callee-saved registers the body writes in the argument block (`frameless = $a0 0x20`; shared drivers `0x28`) | saves follow the argument unpacking, restores open the exit block; fails if anything else uses `$sp` |
| `guard-chain` | physical | adjacent early-exit tests to one target with only the second test's global load and constant between: the load moves before the first test, the first test's slot work into the second test's slot | the moved values are dead at the exit and untouched in between |
| `preheader-order` | physical | before a loop: template loads, then `lui`/`ori` constants (the terminator), then the argument fields | pure constant moves, checked against every intervening use |
| `fill-load-delay` | physical | a load whose result the next instruction reads takes the nearest movable ALU instruction above it into its delay slot | operands untouched in between; otherwise a nop is derived |
| `fill-branch-slot` | physical | an empty conditional-branch slot takes the nearest movable ALU instruction above it, unless that would leave a load-use hazard | it ran on both paths before and still does |
| `fill-jump-slot` | physical | an empty `j` slot takes the nearest small constant load above it in its block (the second pass's `semi = 0`) | the jump always runs its slot; operands untouched in between |

Twelve rules came from the wireframe drivers. The sixteen polygon drivers
added `unspill`, `loop-load-forwarding`, `at-constants`,
`zero-extend-shifts`, `zero-as-addiu`, `increment-sink`, `loop-constant`
and `fill-branch-slot`, and widened `strip-nops` (GTE waits),
`halfword-words` (dominance across blocks, per loop, `lwr` packing),
`word-compose` (SXY words), `tag-build` (OT store in a delay slot),
`gte-load-early` (delay slot), `guard-chain` (adjacent tests only) and
`preheader-order` (its loop-constant part became `loop-constant`). Each was
added only for a hunk that repeated across functions; the twelve wireframe
edit files changed only in their `rules =` line and still match.

The thirty-two direct polygon drivers added `gte-direct-write`,
`at-branch-constants`, `sign-extend-load` and `fill-jump-slot`, widened
`unspill` (narrow reloads, entry values, promoted slots), `halfword-words`
(FT4 `norm0`) and `at-constants` (delay-slot uses, copies), stopped
`increment-sink` from swapping two increments, and moved `invariant-last`
after `unspill`. Fixing `at-constants` also changed what it did with a
constant GCC had put in a delay slot (the next instruction slid into the
slot): the shared drivers' edit files were re-derived with the one list
(their residuals moved by -1 to +6 lines), and the wireframes stayed at
their single line.

### Semantic assumptions

The C states portable semantics; these rules reproduce the original under
five documented assumptions about the game's data:

- A1 (OT length byte): retail stores `p & 0xFFFFFF` into the OT entry,
  clearing its length byte, where `addPrim` keeps it. Ordering-table
  entries are created with a zero length byte (`ClearOTag`/`GsClearOt` write
  24-bit links) and every `addPrim` keeps it zero.
- A2 (no aliasing): primitive records, the GsWORKUNIT cache, the lit
  colour cache (`nortop2`, or `gModel_dwLitColorCacheBase` in the direct
  drivers), the packet buffer and the OT never overlap, so loads may move
  across packet stores.
- A3 (alignment): GsWORKUNIT cache entries and packet buffers are word
  aligned, so a `DVECTOR` copy or the `otz`/`p` pair is one word access.
- A4 (GTE state on skipped primitives): the shared drivers' early SZ writes
  are only visible on paths that drop the primitive, and SZ0-SZ3 are
  reloaded before the next AVSZ. The C passes each cache entry's whole
  depth word (`otz | p << 16`) to `gte_ldsz3`/`gte_ldsz4`; the 16-bit SZ
  registers keep the low half, `otz`.
- A5 (GPU and HMD words): the polygon C reads records through the
  `words[]` view of the `ModelDraw*Record` unions and builds packets as GPU
  command words. Changing a CLUT (`(uv0 & 0xFFFF) | clut`) or the blend
  bits (`uv1 & 0xFF9FFFFF | 0x00400000`) uses the GPU's documented word
  layout, as libgpu's `setClut`/`getTPage` do, not host byte order. SXY
  words are composed as `x | y << 16`, the GTE register format.

The draw-mode guard: the original loads the word at `0x8009AFE4`
(`gModel_bCurrentDrawMode`, `gModel_bBlendRateActive`,
`gModel_wDrawTintRedGreen`) and draws unless it equals 1. The C composes
that value explicitly from the three globals and `word-compose` turns it back
into the single `lw`. `Model_DrawSlot` returns early for draw mode 1
(`src/game/model.c`, before its part loop), so the guard is effectively
always true.

## Edit-file format

```text
# comment
function = Model_DrawFt3Wireframes
source = src/game/wireframe_renderers.c
address = 0x80069E44
size = 0x150
rules = strip-nops aligned-word-copy invariant-last dmpsx-native ...
frameless = $a0 0x20

[registers]
$v0.1 = $a3       # lw $v0.1, 0x0($t3.1)
%word1 = $t0      # lw %word1, 0xC($a2.1)

[residual]
--- rules
+++ retail
@@ -23,2 +23,3 @@
 lw $a1, 0x18($a0)
+lw $a2, 0x1C($a0)
 lw $t5, 0x4($a0)
```

- `address` and `size` are the North American values; only `check` uses
  them (the build massages the same GCC output for every region).
- `[registers]` keys are web names (step 2) or registers created by rules
  (`%word1`, `%tag1`); the comment is the web's first definition.
- `[residual]` is a unified diff over the canonical text after the rules
  and renaming, applied strictly: every context and removed line must match
  at its recorded position, so a C or compiler change fails loudly instead of
  patching the wrong place. Added lines are tagged `residual` in the metrics.
- `asm_massage.py derive EDIT` regenerates both sections from the retail
  bytes (authoring aid: majority vote of aligned registers, then a diff).
  `apply` and `check` never write the edit file, and `apply` never reads the
  retail executable.

## Metrics

All 60 functions match exactly with the one rule list. "By
rules" counts final instructions produced or rewritten by a rule, including
derived nops; "kept" are GCC's instructions with only their registers
renamed; "residual" instructions are the ones the residual diff placed.

### Wireframe drivers

The twelve drivers of `src/game/wireframe_renderers.c`:

| Function | Address | Retail | Kept | By rules | Residual instrs | Residual diff lines | Map entries |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `Model_DrawFt3Wireframes` | `0x80069E44` | 84 | 37 | 46 | 1 | 1 | 23 |
| `Model_DrawGt3Wireframes` | `0x80069F94` | 85 | 38 | 46 | 1 | 1 | 24 |
| `Model_DrawFt4Wireframes` | `0x8006A0E8` | 96 | 40 | 55 | 1 | 1 | 25 |
| `Model_DrawGt4Wireframes` | `0x8006A268` | 98 | 42 | 55 | 1 | 1 | 27 |
| `Model_DrawFt3WindowRecordWireframes` | `0x8006A3F0` | 84 | 37 | 46 | 1 | 1 | 23 |
| `Model_DrawGt3WindowRecordWireframes` | `0x8006A540` | 85 | 38 | 46 | 1 | 1 | 24 |
| `Model_DrawFt4WindowRecordWireframes` | `0x8006A694` | 96 | 40 | 55 | 1 | 1 | 25 |
| `Model_DrawGt4WindowRecordWireframes` | `0x8006A814` | 98 | 42 | 55 | 1 | 1 | 27 |
| `Model_DrawSharedGt3Wireframes` | `0x8006A99C` | 88 | 34 | 53 | 1 | 1 | 31 |
| `Model_DrawSharedGt4Wireframes` | `0x8006AAFC` | 99 | 36 | 62 | 1 | 1 | 37 |
| `Model_DrawSharedGt3WindowRecordWireframes` | `0x8006AC88` | 88 | 34 | 53 | 1 | 1 | 31 |
| `Model_DrawSharedGt4WindowRecordWireframes` | `0x8006ADE8` | 99 | 36 | 62 | 1 | 1 | 37 |

Every residual is the same single line: the original loads the unused
`arg->nortop` (`nortop2` in the shared drivers) into `$a2`, which C cannot
express. The register maps are the bulk of each edit file (23-37 entries,
generated by `derive`).

### History

| Step | Ft3 residual lines (instructions) |
| --- | --- |
| starting C, guard composed, first rule set | 32 (17) |
| `frameless` places saves after argument unpacking and restores at the exit block | 24 (13) |
| C reads the OT entry with `getaddr` before the `gte_stsxy` stores and links with `setlen`/`setaddr` | 16 (9) |
| C sets up line, draw mode, terminator, vertices, OT, shift in that order | 10 (6) |
| MASPSX dropped (pre-pass instead) | 10 (6), map and residual unchanged |
| `guard-chain`, `preheader-order`, `invariant-last`, `fill-load-delay` | 1 (1) |

Computing the guard word before the count test or the record pointer inside
the `if` made it worse (17-21 lines). The quads compute the fourth vertex's
address before `gte_rtpt_b()`; the shared drivers store `packet->xy[3]`
before looking up the second vertex. Both are plain statement orders.

### Shared polygon drivers

The sixteen drivers of `src/game/shared_polygon_renderers.c`. Their C was
written from one template per shape (GT3/GT4, window record, two-sided,
semi), so the drivers differ only where the shapes do; the four packet
layouts are shared in `src/game/shared_primitive_renderers.h`:

| Function | Address | Retail | Kept | By rules | Residual instrs | Residual diff lines | Map entries |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `Model_DrawSharedGt3Primitives` | `0x80067354` | 146 | 47 | 79 | 20 | 41 | 52 |
| `Model_DrawSharedGt4Primitives` | `0x8006759C` | 175 | 53 | 99 | 23 | 46 | 60 |
| `Model_DrawSharedGt3WindowRecordPrimitives` | `0x80067858` | 153 | 46 | 80 | 27 | 56 | 55 |
| `Model_DrawSharedGt4WindowRecordPrimitives` | `0x80067ABC` | 182 | 54 | 103 | 25 | 52 | 61 |
| `Model_DrawSharedTwoSidedGt3Primitives` | `0x80067D94` | 143 | 47 | 76 | 20 | 40 | 47 |
| `Model_DrawSharedTwoSidedGt4Primitives` | `0x80067FD0` | 163 | 49 | 88 | 26 | 50 | 56 |
| `Model_DrawSharedTwoSidedGt3WindowRecordPrimitives` | `0x8006825C` | 150 | 46 | 77 | 27 | 55 | 49 |
| `Model_DrawSharedTwoSidedGt4WindowRecordPrimitives` | `0x800684B4` | 170 | 49 | 89 | 32 | 63 | 58 |
| `Model_DrawSharedSemiGt3Primitives` | `0x8006875C` | 169 | 52 | 89 | 28 | 57 | 57 |
| `Model_DrawSharedSemiGt4Primitives` | `0x80068A00` | 198 | 56 | 106 | 36 | 75 | 64 |
| `Model_DrawSharedSemiGt3WindowRecordPrimitives` | `0x80068D18` | 176 | 52 | 82 | 42 | 87 | 62 |
| `Model_DrawSharedSemiGt4WindowRecordPrimitives` | `0x80068FD8` | 205 | 61 | 111 | 33 | 67 | 68 |
| `Model_DrawSharedTwoSidedSemiGt3Primitives` | `0x8006930C` | 166 | 52 | 86 | 28 | 56 | 52 |
| `Model_DrawSharedTwoSidedSemiGt4Primitives` | `0x800695A4` | 186 | 51 | 96 | 39 | 79 | 61 |
| `Model_DrawSharedTwoSidedSemiGt3WindowRecordPrimitives` | `0x8006988C` | 173 | 54 | 79 | 40 | 79 | 57 |
| `Model_DrawSharedTwoSidedSemiGt4WindowRecordPrimitives` | `0x80069B40` | 193 | 56 | 96 | 41 | 81 | 63 |

The polygon residuals are larger: 20-42 instructions (13-24% of each
function, 40-87 diff lines). Classified by content across the sixteen (as
first measured; the re-derivation with the direct drivers' rule list moved
each function by -1 to +6 lines):

| Hunks | Diff lines | Functions | Content |
| --- | --- | --- | --- |
| 35 | 307 | 16 | second pass: retail does the CLUT/blend updates, then load/colour/store per vertex, and puts `li $v1,0` in the `j` slot; GCC interleaves them |
| 20 | 172 | 16 | normal-colour addresses around NCLIP: retail does all shifts, then the adds in the MAC0 load delay and the `blez` slot |
| 22 | 162 | 8 | semi setup block (`lb` of the blend flag, a dead `colour = template` copy, branch layout) |
| 15 | 76 | 15 | prologue: dead `addu $t4,$s0,$zero` colour copy, draw-mode `lb` before the code-byte shifts |
| 9 | 30 | 8 | window words: the `0xE2000000` constant set once in the guard slot, store order |
| 23 | 25 | 14 | the unused `nortop2` load (and where GCC loaded `nortop2`) |
| 6 | 18 | 6 | saved-register map follow-ons |
| 71 | 160 | 16 | other scheduling (single moved or swapped instructions) |

These are GCC's own instruction scheduling (`-O2` runs both scheduler
passes) and a few redundant instructions the original programmer wrote.
They repeat across functions, but none is a rewrite with a dataflow
justification: matching them would mean a list scheduler tuned to this
code, so they stay in the residual. A C ordering experiment for each
(statement order of the colour loads, the copies, the guard) changed GCC's
schedule only for the colour loads, which the template now uses.

### Direct polygon drivers

The thirty-two drivers of `src/game/polygon_renderers.c`, from one C
template per shape (FT3/GT3/FT4/GT4, window record, two-sided, semi):

| Function | Address | Retail | Kept | By rules | Residual instrs | Residual diff lines | Map entries |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `Model_DrawFt3Primitives` | `0x800612C0` | 151 | 52 | 89 | 10 | 20 | 40 |
| `Model_DrawGt3Primitives` | `0x8006151C` | 177 | 65 | 90 | 22 | 46 | 51 |
| `Model_DrawFt4Primitives` | `0x800617E0` | 169 | 59 | 94 | 16 | 33 | 44 |
| `Model_DrawGt4Primitives` | `0x80061A84` | 214 | 68 | 110 | 36 | 76 | 63 |
| `Model_DrawFt3WindowRecordPrimitives` | `0x80061DDC` | 159 | 54 | 94 | 11 | 23 | 43 |
| `Model_DrawGt3WindowRecordPrimitives` | `0x80062058` | 185 | 65 | 95 | 25 | 54 | 54 |
| `Model_DrawFt4WindowRecordPrimitives` | `0x8006233C` | 177 | 60 | 98 | 19 | 41 | 46 |
| `Model_DrawGt4WindowRecordPrimitives` | `0x80062600` | 222 | 72 | 115 | 35 | 71 | 63 |
| `Model_DrawTwoSidedFt3Primitives` | `0x80062978` | 146 | 52 | 79 | 15 | 31 | 39 |
| `Model_DrawTwoSidedGt3Primitives` | `0x80062BC0` | 172 | 64 | 86 | 22 | 46 | 51 |
| `Model_DrawTwoSidedFt4Primitives` | `0x80062E70` | 164 | 58 | 90 | 16 | 33 | 44 |
| `Model_DrawTwoSidedGt4Primitives` | `0x80063100` | 209 | 67 | 106 | 36 | 76 | 63 |
| `Model_DrawTwoSidedFt3WindowRecordPrimitives` | `0x80063444` | 154 | 53 | 84 | 17 | 35 | 43 |
| `Model_DrawTwoSidedGt3WindowRecordPrimitives` | `0x800636AC` | 180 | 64 | 91 | 25 | 54 | 54 |
| `Model_DrawTwoSidedFt4WindowRecordPrimitives` | `0x8006397C` | 172 | 59 | 94 | 19 | 41 | 46 |
| `Model_DrawTwoSidedGt4WindowRecordPrimitives` | `0x80063C2C` | 217 | 71 | 111 | 35 | 71 | 63 |
| `Model_DrawSemiFt3Primitives` | `0x80063F90` | 174 | 60 | 94 | 20 | 40 | 49 |
| `Model_DrawSemiGt3Primitives` | `0x80064248` | 200 | 72 | 97 | 31 | 67 | 55 |
| `Model_DrawSemiFt4Primitives` | `0x80064568` | 192 | 68 | 102 | 22 | 49 | 49 |
| `Model_DrawSemiGt4Primitives` | `0x80064868` | 237 | 82 | 116 | 39 | 81 | 71 |
| `Model_DrawSemiFt3WindowRecordPrimitives` | `0x80064C1C` | 182 | 62 | 100 | 20 | 42 | 49 |
| `Model_DrawSemiGt3WindowRecordPrimitives` | `0x80064EF4` | 208 | 69 | 103 | 36 | 76 | 58 |
| `Model_DrawSemiFt4WindowRecordPrimitives` | `0x80065234` | 200 | 68 | 110 | 22 | 46 | 54 |
| `Model_DrawSemiGt4WindowRecordPrimitives` | `0x80065554` | 245 | 79 | 122 | 44 | 90 | 71 |
| `Model_DrawTwoSidedSemiFt3Primitives` | `0x80065928` | 169 | 60 | 89 | 20 | 40 | 49 |
| `Model_DrawTwoSidedSemiGt3Primitives` | `0x80065BCC` | 195 | 71 | 93 | 31 | 67 | 55 |
| `Model_DrawTwoSidedSemiFt4Primitives` | `0x80065ED8` | 187 | 67 | 98 | 22 | 49 | 48 |
| `Model_DrawTwoSidedSemiGt4Primitives` | `0x800661C4` | 232 | 81 | 112 | 39 | 81 | 71 |
| `Model_DrawTwoSidedSemiFt3WindowRecordPrimitives` | `0x80066564` | 177 | 61 | 96 | 20 | 42 | 49 |
| `Model_DrawTwoSidedSemiGt3WindowRecordPrimitives` | `0x80066828` | 203 | 71 | 99 | 33 | 71 | 58 |
| `Model_DrawTwoSidedSemiFt4WindowRecordPrimitives` | `0x80066B54` | 195 | 67 | 106 | 22 | 46 | 54 |
| `Model_DrawTwoSidedSemiGt4WindowRecordPrimitives` | `0x80066E60` | 240 | 78 | 118 | 44 | 90 | 71 |

Their residuals are 10-44 instructions (7-18% of each function, 20-90 diff
lines, 1728 in all). The flat-shaded drivers need least (20-49 lines),
the GT4 forms most. What repeats is the original's scheduling: the
prologue's argument loads and the `mtc2 template,$6` interleaved with the
template words, the original's extra `colour` register copy, the OT entry
load before the `gte_stsxy` stores, the colour-cache offset (`normal / 2`)
computed on each path instead of once above the draw-mode branch, the
second pass's word copies interleaved with the CLUT and blend updates, and
in the semi variants the blend-flag setup block. Each is either a single
moved instruction or a choice between two equal registers, which a rule
could not make without matching one function's layout.

## Dependence check

`check` also proves that the residual keeps the program's meaning, so a
large residual is reviewable rather than trusted on the byte match alone
(`tools/project/asm_massage_deps.py`). It compares the program the residual
applies to with the result:

- delay slots are folded (a slot instruction runs just before its branch);
  labels that directly precede another label name the same place, and
  places are numbered from the end, so equal control flow compares equal;
- GCC's tail duplication is undone first: a pure instruction that is the
  last one executed on every way into a place (the fall-through and every
  branch's delay slot) moves to just after the place, provided that every
  copy has the same operation, constants and operand values (source and
  destination webs in one value class), there are at least two copies,
  nothing runs between a copy and the place, and a copy in a conditional
  branch's slot is not read on the not-taken path before it is recomputed.
  The single copy then runs exactly when one of the copies ran; the
  allocation check re-verifies every use;
- moved lines are paired by text, re-registered lines (same operation and
  constants, other registers) by the pairing under which every register
  holds the value its reader expects;
- dependences are taken between webs, not registers: RAW, WAR and WAW,
  GTE data/control registers (a GTE command reads and writes them all), and
  memory, where two accesses conflict if one stores and they may overlap
  (same base and overlapping bytes, counting constant increments of the
  base between them, the same global, or the same A2 region inferred from
  the GsARGUNIT field the base pointer came from, or from
  `gModel_dwLitColorCacheBase` for the colour cache). Regions are inferred
  per GCC live range, so a register the renamed program reuses for another
  value does not hide them, and a copy of the argument pointer counts as the
  argument; every conflicting pair must keep its order;
- allocation: in the final program every register read must be reached only
  by definitions of the value it expects, on every path; values joined by a
  register copy, a spill slot or an equal constant are interchangeable.
  This also proves pure instructions that move between blocks; a moved load
  must not pass a store that may write its word;
- a side effect may cross a branch only when it is dead on that path: a GTE
  register rewritten before the next GTE command, or a store into the packet
  being built (A6, below); moves across labels and control-flow changes
  are reported;
- insertions must be dead, rebuild a constant the readers expect, copy a
  register whose single value every reader of the copy expects (the
  original keeps `colour = template` in its own register), or be frameless
  save-area accesses; deleted pure instructions are covered by the
  allocation check; GTE commands must keep at least their original wait
  after the last GTE register write (up to the two-instruction rule).

A6 (unlinked packet words): until tag-build links a packet into the OT its
words are scratch; a path that skips the link reuses or abandons them.

Result on the 60 massaged functions:

| Functions | Moves / re-registered / insertions / deletions / merged | Result |
| --- | --- | --- |
| 12 wireframe drivers | 0 / 0 / 1 (the dead `nortop` load) / 0 / 0 | proven |
| 16 shared polygon drivers | 10-25 / 3-12 / 0-3 (dead or rebuilt constants; save-area accesses counted apart) / 0-3 / 0-1 (the semi window GT3 and the two semi GT4 forms merge one) | proven |
| 32 direct polygon drivers | 4-18 / 4-26 / 1-2 (dead, rebuilt constants, or the `colour` register copy in sixteen of them) / 0-4 / 0-1 | proven |

All 60 residuals are proven with no assumption beyond A1-A6. In the three
semi drivers GCC duplicates the code-byte shift (`srl $s3,$s0,24`) into
the two ways into the join after the semi setup (one in a delay slot, one
behind an extra label); the original computes it once after the join. The
merge proof accepts it: identical copies reading the same template value,
nothing between them and the join, and the slot copy's result is
recomputed before any read on the not-taken path.

## Scheduling experiment

A list scheduler per basic block (dependence DAG from the checker's
conflict model, original order breaking ties) was tried in the physical
stage before `fill-load-delay`, with three priority orders taken from the
retail code. Residual diff lines after re-deriving every edit file:

| Priority | 12 wireframes | 16 polygons |
| --- | --- | --- |
| none (current rules) | 12 (1 each) | 966 |
| loads, ALU, GTE, stores | 372 | 1446 |
| stores last | 200 | 1152 |
| loads first | 116 | 972 |

Every variant made both families worse: the original's order is not a
fixed priority (it interleaves loads, GTE operations and stores
differently per block), and reordering blocks GCC already got right costs
more than it gains. The scheduler was left out.

## Evidence

The build gate for the integrated units:

```text
$ make check-asm-massage | tail -1
asm_massage check: 60/60 MATCH and proven
```

GCC's instruction counts for the wireframes (before massaging) were 104
(+2 nops) for the FT3/GT3 drivers, 118 (+4) for FT4/GT4, 128 (+3) for shared
GT3 and 149 (+4) for shared GT4. The sixteen polygon drivers go from
210-276 GCC instructions to their retail 143-205. The full report for one
shared polygon driver:

```text
Model_DrawSharedGt3Primitives: gcc -O2 215 instructions (+1 nops), massaged 146, retail 146
  register map entries: 52; residual: 20 removed, 20 added lines
  instruction count per rule (before->after): aligned-word-copy 216->204, gte-direct-read 204->202, unspill 202->189, undo-strength-reduction 189->183, word-compose 183->167, halfword-words 167->170, tag-build 170->165, zero-extend-shifts 165->166, reuse-load 166->163, loop-constant 163->164, store-delay-slot 164->163, gte-load-early 163->160, drop-self-moves 160->159, frameless 159->146
  final instructions by origin: aligned-word-copy+increment-sink+undo-strength-reduction=4, aligned-word-copy+reuse-load+undo-strength-reduction=3, at-branch-constants=2, at-constants=3, at-constants+invariant-last+unspill=1, delay=2, dmpsx-native=2, fill-load-delay+invariant-last=1, frameless=6, gcc=47, gte-direct-read=2, gte-load-early=6, halfword-words=10, increment-sink=1, increment-sink+undo-strength-reduction=2, invariant-last=3, invariant-last+unspill=1, loop-constant=2, loop-load-forwarding+tag-build=1, residual=20, store-delay-slot+tag-build=1, tag-build=8, undo-strength-reduction=7, undo-strength-reduction+unspill=1, unspill=2, word-compose=6, zero-extend-shifts=2
  relocations: R_MIPS_HI16 gModel_dwPolyGt3Template, R_MIPS_LO16 gModel_dwPolyGt3Template, R_MIPS_HI16 gModel_dwTpageBlendRateBits, R_MIPS_LO16 gModel_dwTpageBlendRateBits, R_MIPS_HI16 gModel_dwSecondPassClutBits, R_MIPS_LO16 gModel_dwSecondPassClutBits, R_MIPS_HI16 gModel_bCurrentDrawMode, R_MIPS_LO16 gModel_bCurrentDrawMode, R_MIPS_HI16 gModel_dwLitColorCacheBase, R_MIPS_LO16 gModel_dwLitColorCacheBase, R_MIPS_26 .text, R_MIPS_HI16 gGraphics_pPacketWorkCursor, R_MIPS_LO16 gGraphics_pPacketWorkCursor
  dependence check: 10 moves, 8 re-registered, 0 insertions (0 dead, 0 constants, 0 copies, 2 saves), 0 deletions, 0 merged duplicates; all dependences preserved
  MATCH 0x248 bytes at 0x80067354
```

`check` refuses C that breaks the source contract, compiles it with the
massaged profile's flags, applies the edit file, assembles with
`mipsel-none-elf-as -EL -mips1 -G0`, links at the function address with the
referenced symbols' addresses from `config/slus_01411/symbols.txt`, and
compares every word with the retail executable. It also requires the object's
`.text` relocations to be exactly the `%hi`/`%lo` pairs the retail code uses
on known symbols, in order (a `j` to a label inside the function relocates
against `.text`; the linked bytes check its target).

## Risks

- Rules preserve the C's meaning under their preconditions and assumptions
  A1-A5; the residual is now checked too (see the dependence check), under
  A6. It proves all 60 residuals. Control-flow rewrites other than merging
  a duplicated instruction at a join are still reported, not proven.
- A1-A6 hold for the game's data but are not C guarantees.
- The scheduling rules (`guard-chain`, `preheader-order`,
  `fill-load-delay`, `store-delay-slot`, `gte-load-early`,
  `invariant-last`) encode this family's hand-scheduling conventions. They
  are generic in form and only move instructions when dataflow allows, but
  another family may need them disabled or new ones; only the exact match
  shows that the conventions fit.
- Web names depend on GCC's allocation; a C edit renumbers them. `derive`
  regenerates the map and residual and `check` proves the result, so the
  reviewable parts are the C, the rules and the size of the residual.
- Rules are pattern matchers over GCC 2.8.1 output; a different C spelling
  can stop a rule from firing (it then leaves the code alone and the
  residual grows, which `check` reports).

## Build integration

- Profile: `gcc_2_8_1_o2_massaged` in
  `config/slus_01411/compiler_profiles.json`: the GCC 2.8.1 compiler, plain
  `-O2 -G0 -mel -mips1 -mcpu=R3000 -msoft-float -fno-builtin -Dpsx -D__psx__
  -D__psx`, `data_limit` 0 and `"asm_massage": true`, with no MASPSX keys
  (`validate_effective_profile` rejects `maspsx_flags`, `aspsx_version`,
  `assembly_filter` and the inline-macro and hand-written keys beside it).
- Build: `compile_c` hands such units to `compile_massaged_c`, which runs
  `gcc -S`, then `asm_massage.massage_unit` with every edit file of the unit,
  then GNU as. The regional builders import the same `compile_c`, so the
  Japanese and European executables build the unit the same way.
- Edit files: by convention, not a manifest field. A source
  `<unit>.c` keeps one `<unit>.massage/<function>.massage` per
  function, named after the function. `massage_unit` requires an edit for
  every function GCC emitted (a source built as two objects split by a
  define, as `polygon_renderers.c` is in Japan and Europe, leaves the other
  object's edits unused; the contract check matches the directory against
  the manifest), processes them in GCC's (source) order, gives each function its own label prefix
  (`.L<function>_<n>`) and rejects any data directive, so a unit with
  string literals or data cannot silently lose it.
- Incremental cache: `build_incremental.py` adds the hashes of
  `asm_massage.py` and `asm_massage_rules.py` to the massaged profile's
  context and the edit directory's digest to each unit's signature.
- Contracts: `make check-matching-source-contracts` limits massaged profiles
  to the resident manifest, rejects hard-register variables and the
  `handwritten_asm.h`/`gte_native.h` includes, accepts only asm statements
  whose template, constraints and clobbers equal one of
  `src/psyq/inline_c.h`'s, and requires one edit file per manifest function
  whose `source`, North American `address`/`size` and `rules` are valid.
  `make check-asm-massage` (also run by `make audit` and the matching CI
  build) runs `asm_massage.py check` on every massaged unit: exact bytes,
  relocations and the dependence proof, failing on any unproven residual.
  `integrate_verified_match.py` refuses massaged profiles; such units are
  integrated by hand.
- The build never runs `derive` and never reads the retail executable.

## Next steps

All three families are integrated, with `make check-asm-massage` as the
gate: the wireframes with one-line residuals, the shared and direct polygon
drivers with 20-90 diff lines of the original's scheduling, all proven
mechanically, so review can focus on the C and the rules. A scheduler does
not shrink the polygon residuals.

The one hand-written routine left is the shared cache-preparation stage,
`Model_PrepareSharedPrimitiveCaches`. `gte-direct-write` now covers its
`mtc2 template,$6`, but its vertex and normal loops are software-pipelined:
each iteration moves the vector loaded on the previous one into the GTE
(`lw lo,0(src); lw hi,4(src)` ahead of `mtc2 lo,$0; mtc2 hi,$1`), so the
last iteration reads the entry past the end of the range. Plain C either
reads past the array (not portable C) or loads each vector where it is
used, and turning that into the pipelined loop is a loop rotation with a
speculative load that the dependence check cannot prove. So it keeps the
`gcc_2_8_1_g0_handwritten_asm` profile, and with it `handwritten_asm.h`,
`gte_native.h` and `tools/project/handwritten_asm_contract.py`.
