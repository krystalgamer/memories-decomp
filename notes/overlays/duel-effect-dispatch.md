# Spanish duel-bank dispatcher

## Canonical resident/bank handoff

The complete provider and resident updater now call the existing entry
`DuelEffect_DispatchRequest`, which every resident and bank symbol file
binds by name, and keep the separate measured caller/provider ABI views. Canonical request members
`phase` and `progress` retain signed-halfword/unsigned-byte storage at
`0x1A/0x1D`; progress is a handler-defined count, not final completion.
See the [full handoff contract](../duel-effect-request-handoff.md).
All 25 independent effect-ID comparisons, 24 distinct handlers, prior setup, special
argument forwarding and current owner/callee assertions remain unchanged.
The guard's 25 distinct overlay callees include the texture-packing helper;
the handler call-site count is 26 because case twenty has two alternatives.

## Scoped active render-state C views

`render_state_semantic_views.h` exposes three independently stored context
values. Every regional `duel_effects_symbols.txt` binds each name at its
own address in place of the former label; all storage, relocations,
inventories and generated-data owners remain unchanged.
The pointer keeps `G32`, the vector keeps its SDK layout and padding, and
the priority stays `u16`.

| C name | Former label | NA address | JP address | UK / FR / ES address |
|---|---|---|---|---|
| `gDuelEffect_pOrderingTable` | `D_8015B7F4` | `0x8015B88C` | `0x8016988C` | `0x8015B7F4` |
| `gDuelEffect_DrawOffset` | `D_8015B7F8` | `0x8015B7D0` | `0x801697D0` | `0x8015B7F8` |
| `gDuelEffect_wBasePriority` | `D_8015B800` | `0x8015B7C8` | `0x801697C8` | `0x8015B800` |

The NTSC objects are not contiguous in the PAL order. These names do not
introduce a packed context object, new allocation or linker alias.

The dispatcher selects the active ordering table from request words 8/12.
All 26 pointer writes remain in their original cases, including both writes
in case nine. There is no new initialization or fallback for unknown IDs.

The offset receives the request's three halfwords without writing vector
padding. Packet helpers add its X/Y components to projected coordinates;
effect initializers also copy all three components, and effects 16/23 use
the Z sign to select orientation. Effect 24 overwrites it with its center.
It is therefore neither a screen-only coordinate nor an immutable request
origin; all copies, reads and writes remain uncached and in source order.

The base-priority halfword is passed to SDK polygon/gradient-line sorters
and resident packet submission, not tested as a bit mask. The historical
dispatcher-only "flags" label below describes the input copy, not a
recovered general flag contract. Fullscreen submissions use the base
unchanged. Generic offset quads use narrowed `base + 1`; the FT4 SDK path
uses the base, while the GT4 SDK path uses narrowed `base + 1`. The other
mode branches retain their own base/depth behavior. No increment, clamp,
mask, mode normalization or unified submission path is introduced.

All twenty complete C consumers migrate only these three identifiers and
one added include per file. Named-profile GCC/MASPSX output, native symbols
and disassembly/relocations remain identical for every complete object.
The original headers and ownership guards remain unchanged. Three scoped
semantic records append at actual NA addresses; all prior registry bytes
and every field in all five bank inventories remain unchanged.

## Complete accepted live-handler definitions

The six already accepted live-traced names in `handler_semantic_views.h`
now also name their complete real C definitions, rather than only dispatcher
calls. Each provider includes that unchanged header after its private
declarations; the symbol files bind each name where the former native
function label stood. No new semantic inference, extra name, wrapper,
inventory change, registry row or data allocation is introduced.

| Definition | Source | Former label | NA address | Size |
|---|---|---|---|---|
| `DuelEffect_RunFusion` | `effect_1.c` | `func_80157794` | `80156438` | `67C` |
| `DuelEffect_RunAttackSlash` | `effect_2.c` | `func_80153200` | `80153DD8` | `8DC` |
| `DuelEffect_RunDestroyFlames` | `tile_effect.c` | `func_80149F90` | `8014AA68` | `954` |
| `DuelEffect_RunTerrainChange` | `effect_10.c` | `func_8014C8FC` | `8014D5D4` | `A7C` |
| `DuelEffect_RunGuardianStarBonus` | `gather_effect.c` | `func_801481A8` | `8014A06C` | `9FC` |
| `DuelEffect_RunBoardDestruction` | `effect_17.c` | `func_80148BA4` | `80158E84` | `13EC` |

Addresses and sizes are hexadecimal. This finishes the existing binding
surface; it does not claim new human-trace evidence or broader artwork
ownership. Each entire C source reverses byte-for-byte to accepted master
after undoing only its definition name and one include. Parameters retain
their original types and names, including the attack-slash signed-halfword
third argument. Every statement, private work/config declaration, SDK call,
arithmetic operation, fallback/counter path, progress/completion store and
regional conditional remains untouched.

In particular, board destruction retains its accepted jagged-column helper
call/stat header and integer-word Exodia scratch binding. The native bolt
definition/utility header, original attempt history and regression remain
byte-exact. The dispatcher, six-view semantic header, all five inventories,
complete semantic registry, owner guard and native maps/profiles are unchanged.
No pending card-flash or fallback component is imported. Native compiler,
MASPSX, symbol and relocation comparison covers all six complete providers;
full-image acceptance remains mandatory.

## Original dispatcher matching evidence

`func_80146258` (`80146258..80146760`, 1,288 bytes) is complete matching C
under `gcc_2_8_1_g0_split`. The independent Spanish integration also reuses
the unchanged 148-byte French `rect_vertices.c` accepted in #6546.

Against accepted `45c9a270f`, every prior 54 Spanish manifest entry is
unchanged. The addition is two functions / 1,436 bytes: **56/85 bank C
functions / 14,320 bytes**, 29 explicit assembly boundaries, and **180/209
configured-overlay instances / 70,172 bytes**. This is not exhaustive runtime
completion; boot and other dynamic loads remain separate work.

## Recovered dispatch and argument contracts

The entry copies three offset halfwords, preserves vector padding, and
copies a flag halfword. For nonnegative phase it initializes exactly 21
texture-page/CLUT pairs using `func_8014F564`. It then executes 25 independent
effect comparisons. Unrecognized effects still perform the preceding setup.
Negative phase skips texture initialization, not effect dispatch.

The dispatcher reuses the canonical 32-byte `DuelEffectRequest` from
`src/game/duel_effect_request.h`, rather than declaring another prefix view.
`DuelEffect_UpdateRequests` passes that record and populates its words at
8/12 from the projected/screen ordering tables. Explicit pointer casts
reverse those existing integer-word stores without changing the resident
layout or its neutral field names. The dispatcher reads offset halfwords at
0/2/4, flags at 16, and a signed variant at 18. Only cases 2 and 13 pass the variant:
their callees `80153200` and `801503F8` save incoming `$a2` before overwriting
argument registers.

Case 9 preserves both original writes to the active ordering-table pointer.
The `$a2` value at its call is **not a third parameter**: `801593A8` ignores
incoming `$a2`, overwriting it with 8 before its first call. Treating this
scratch register as a third parameter caused the last five mismatching
instructions. The final declaration has two parameters, and no volatile
access or register-forcing workaround is used.

Case 20 passes one for nonnegative phase and preserves a negative phase.
Keeping two explicit source calls in the `if`/`else` lets GCC merge their
common call tail into the exact target. A ternary call argument was eight
bytes short and changed saved-register allocation.

`SetGeomOffset` uses the existing Spanish resident binding `80087838`
(24 bytes, still `sdk_asm`). Its calls use 160/108. No resident SDK function
is newly classified as game C.

## Canonical data views and actual storage owners

The setup loop establishes these extents independently of guessed headers:

| Address | Extent | Evidence |
| --- | ---: | --- |
| `8015A1E4` | 588 | 21 SDK `GsIMAGE` records, stride 28 |
| `8015A430` | 42 | 21 mode halfwords, stride 2 |
| `8015B748` | 84 | 21 output pairs, stride 4 |
| `8015B7F4` | 4 | Active ordering-table pointer |
| `8015B7F8` | 8 | SDK vector, three written halfwords and retained padding |
| `8015B800` | 2 | Flags |

The extents above describe the original dispatcher-only proof. The subsequent
[effect-eleven recovery](duel-effect-eleven.md) establishes that mode 20 at
`8015A458` is also the low halfword of the first of five variant `GsIMAGE`
records. The current real owner at `8015A430` therefore spans **180 bytes**,
with a 42-byte `modes` view and a 140-byte image view beginning at offset 40.
`image_inputs.h` expresses that overlap as one canonical union, and both
French and Spanish symbol maps give the owner its complete `0xB4` extent.
There is no independent `D_8015A458` data definition or absolute alias.
The dispatcher's object text, function extent and relocations are unchanged.

`DuelEffectTextureTable` is the single declaration owner of the texture
storage: a union of the accepted 48-byte named prefix and `u16 pairs[21][2]`.
Existing consumers use the named member at offset zero, without changing any
accepted field offset. This avoids separate incompatible array/struct
declarations and avoids indexing beyond the old prefix.

The six symbols remain real definitions in the generated input `.data`
object, not C definitions or absolute aliases. Explicit symbol extents let
Splat resolve interior references such as `8015B778` as `D_8015B748 + 0x30`;
the original bytes and reference operands remain unchanged. The final linked
symbols have all six exact sizes. The final module section also contains
code, so data ownership is checked against the non-executable input data
object rather than inferred from the final combined section flags.

A target-GCC layout probe verifies sizes 84/48/32/28 for the union, prefix,
request and image record; request offsets 8/12/16/18; and retained prefix
offsets 40/46 for `page0`/`clut1`.

## Experiments and acceptance

Full snapshots and per-word diagnostics are retained privately under
`tmp/dispatch-probe/`. Sizes and differences below describe complete
functions, not accepted partial ranges.

| Attempt | Change | Bytes | Differing words |
| --- | --- | ---: | ---: |
| 1 | Direct dispatch with ternary phase and incorrectly inferred third argument | 1280 | 112 |
| 2 | Buffer alias and assignment-expression argument | 1280 | 143 |
| 3 | Explicit phase branches; original parameter lifetime | 1288 | 5 |
| 4 | Projected-table local | 1288 | 5 |
| 5 | Sequenced argument expression | 1288 | 5 |
| 6 | Named no-follow-jumps profile | 1288 | 5 |
| 7 | Re-read context field as the third argument | 1292 | 190 |
| 8 | Shared screen-table local | 1288 | 5 |
| 9 | Preload both tables; first store optimized away | 1280 | 190 |
| 10 | Locally qualified volatile view; rejected | 1292 | 191 |
| 11 | Ordinary active-table pointer view | 1288 | 5 |
| 12 | Named first-scheduling-disabled profile | 1284 | 288 |
| 13 | Correct two-argument callee and direct stores | 1288 | 0 |
| 14 | Canonical union-backed texture table | 1288 | 0 |
| 15 | Reuse the full resident request record instead of a local prefix | 1288 | 0 |

The private complete-bank link and production Spanish build reproduce all
90,112 bytes, SHA-256
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
Exact object/final-ELF function extents, all six data owners and retail bytes,
and all 25 distinct overlay callees are verified. Assembly callees retain
their inventory status and real executable-section definitions.

Every affected regional build (Spanish, French, English PAL and Japanese)
matches after the shared view change. Spanish retail copies, metadata policy
and 65 targeted tests also pass. The randomized curve, polygon generators and
number renderer remain nonmatching assembly; none is included in this change.

## Independent French reuse

At accepted-master cutoff `1aafeffd6`, French reuses `dispatch.c`, its context
and the canonical texture-table view unchanged. `SetGeomOffset = 0x80087838`
is independently named in the French resident linker map; its complete
24-byte resident function remains `sdk_asm`.

The French full-bank proof reproduces the same 90,112-byte hash above. All
25 distinct overlay callees retain their real executable definitions and
original inventory status. All six data symbols have the exact extents above
in both the final ELF and the non-executable generated input-data object;
their bytes equal the French retail slice. No callee or data storage is
replaced with an absolute alias.

Together with the independently recovered 384-byte
[height ring](duel-effect-geometry.md#height-ring-follow-up), the two additions
contribute **1,672 bytes**. All 57 previously accepted French entries remain
unchanged: **59/85 bank C functions / 15,360 bytes**, **26 assembly
boundaries**, and **183 configured C instances / 71,212 bytes**. Existing
regional implementations and profiles are unchanged. Boot, other dynamic
loads and remaining bank assembly still prevent a French completion claim.

## North American human route evidence (#418)

The human traces accepted in #7865 (`dcd8ee6743`) correlate numbered
dispatcher episodes with visible actions. These are three separate routes,
not three recordings of the same duel. The IDs run captured 21 episodes /
1,163 dispatch calls; the texture run captured 18 episodes / 1,087 calls;
the atlas run captured three episodes / 152 calls. All three observed
execution breakpoints, live resident-update controls and the guarded NA
dispatcher, with no foreign-module hits. The configured interpreter setting
was reported, not an independent query of the active CPU engine.

| Decimal effect ID | Actual NA handler | Native source symbol | Human observation | Observed initial phases |
|---|---|---|---|---|
| 0 | `0x80155104` | `func_80154688` | Equip animation: Beast Fangs on several monsters, and Mystical Moon on The Wicked Worm Beast; card movement/spin, brief light and stat increase. | `0` |
| 1 | `0x80156438` | `func_80157794` | Successful two-card fusion, for either player: cards converge/spin, bright light, then the resulting monster. | `0` |
| 2 | `0x80153DD8` | `func_80153200` | Slash/impact during monster attacks and direct attacks. A slash does not imply a destroyed card: the stronger monster in IDs episode 15 did not shake. | `0,1,4,5` |
| 3 | `0x8014AA68` | `func_80149F90` | Defeated monsters go up in flames, in separate episodes following their slash/impact. | `0` |
| 10 | `0x8014D5D4` | `func_8014C8FC` | Playing Mountain: the field glows white, particles rise, then the Mountain field appears. | `2` |
| 14 | `0x8014A06C` | `func_801481A8` | Guardian-star advantage: bright light, guardian symbols moving inward and a stat increase, on either player's monster. | `1,3,5` |
| 17 | `0x80158E84` | `func_80148BA4` | Playing Dark Hole in texture episode 11; this selects the statically recovered field-card vortex lifecycle. | `0` |

Native suffixes in this table are not NA execution addresses. Initial phases
are distinguished from negative continuation/termination values. These
observations identify the sampled gameplay routes, not every phase or every
card that can select a handler. In particular, Mountain does not identify
the other terrain configurations, and Dark Hole does not identify effect
17's phase-one configuration. No broader lifecycle or request-field rename
is implied.

### Semantic handler views

Six handlers have a single caller-backed purpose and get C views in
`handler_semantic_views.h`. The regional inventories and symbol files carry
the name at each native address in place of the former label. Each name rests on the human observation above plus
the complete static requester list:

| Effect | Name | Former label | Every static requester |
|---:|---|---|---|
| 1 | `DuelEffect_RunFusion` | `func_80157794` | `DuelScene_UpdateCardPlacement`'s fusion branch |
| 2 | `DuelEffect_RunAttackSlash(work, phase, damage)` | `func_80153200` | `DuelScene_UpdateBattle`, battle and direct-attack sites |
| 3 | `DuelEffect_RunDestroyFlames` | `func_80149F90` | `DuelScene_UpdateBattle`, after a losing slash |
| 10 | `DuelEffect_RunTerrainChange` | `func_8014C8FC` | `DuelEffect_ApplyTerrain`, for every terrain |
| 14 | `DuelEffect_RunGuardianStarBonus` | `func_801481A8` | `DuelScene_UpdateBattle`'s guardian-star branch |
| 17 | `DuelEffect_RunBoardDestruction` | `func_80148BA4` | `DuelEffect_ApplyBoardDestruction` (Dark Hole and the Dragon Capture Jar phase) |

The debug effect screen can request any ID and is not counted. The third
argument is named only on the effect-2 view. It does not name `field_12`
for effect 13 or for any other request.

Effect 0 (`func_80154688`) keeps its native name. Besides the observed
equip placement, `DuelScene_UpdateExodiaResult` (`duel_draw_resolution.c`)
requests it with phase 9. The same equip call site also selects effect 4
for cards 301 and 302. No sampled route covers the Exodia lifecycle, so an
equip-only name would overclaim.

The attack handler's signed request variant was observed as zero, positive
and negative, including `1200`, `200` and `-700` in monster attacks.
Direct-attack episodes used phases four or five and positive variants
`1150`, `1600`, `2500` and `3300`. These are bounded observations of the
existing `field_12` argument, not a meaning assigned to every request's
variant field.

The texture run's final Beast Fangs episode was still running when its
three-minute window expired: only six dispatch calls were captured. Its
partial slot usage must not be treated as a different complete equip
animation. The atlas run ended as the human started a 3-D battle. That is
consistent with its intended finish condition: after seeing duel mode, the
script prints its document when the main mode leaves the duel. It did not
report a script error.

The completed scripts, results and their script-specific replay tests were
removed after extracting this evidence. The raw observations remain in
#7865's history; the durable texture associations and the atlas limitation
are recorded in [the texture note](duel-effect-textured-quads.md#human-texture-observations-418).
