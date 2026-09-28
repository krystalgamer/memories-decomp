# Spanish resident build

`make spanish-match` validates the Spanish inputs and image map, cleans the
split/build output, compiles the configured C units, links the entire
`SLES_039.51`, and requires SHA-256
`b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`.
`make spanish-inventory` additionally regenerates `functions.csv` from the
matching manifest, exact linked C symbol extents, and generated fallback
assembly. Runtime modules have their own [overlay build](overlays/README.md).

The resident integration uses 561 shared C units and regional wrappers:
1,140 functions and 357,700 bytes, without duplicating function bodies or adding
compiler flags. Their named GCC 2.8.1/MASPSX 2.81 profiles are unchanged.
The ordered per-function ranges in `matching_c.json` completely cover each
grouped object's text. Of these units, 42 now supply their own read-only or
initialized small data; none of their C definitions is replaced by an absolute
linker alias.

All 1,140 eligible resident game functions and the 124 function instances in
the original six configured overlays are matching C. This is not exhaustive
runtime completion: the newly registered duel-effect bank adds 85 provisional
function boundaries, of which 17 match and 68 remain assembly. Boot and other
dynamic-load coverage still need investigation. The 61 evidence-backed
handwritten resident functions and 623 Psy-Q/CRT functions remain assembly;
no newly found bank function is reclassified to claim completion.

The address-wrapper batch adds 20 functions / 2,332 bytes from nine units,
bringing that batch to 1,093 resident C functions / 343,460 bytes. Five thin
[Spanish wrappers](../../src/game/spanish/README.md) specialize measured
literal addresses; four more existing units are reused unchanged. All bodies
and compiler profiles are shared, and all new linked function extents are
verified by the inventory gate.

The dialog-layout batch adds another 17 functions / 5,604 text bytes and two
C-owned jump tables totaling 68 bytes. Seven wrappers select Spanish dialog
geometry while sharing the existing function bodies. All 27 previously
configured source/profile objects affected by the new geometry defaults remain
byte-identical, and the complete North American executable still matches.

## Evidence and exclusions

Each selected object's European linked instructions were searched against
Spanish resident text while masking only its ELF relocation fields. Exactly
one full-unit match was required; unmatched or ambiguous units remained
generated assembly. The initial 1,291 external symbol bindings were recovered
from the Spanish instruction operands, accounting for paired HI16/LO16
relocations and the measured GP change from `0x8009BE84` to `0x8009C298`.
Object-relative local references are resolved by the linker.

The first full link identified two necessary corrections, not source changes:
HI16/LO16 pairs must recover the whole address rather than assume a small
regional delta; and auto-generated in-image and external symbols both need
the `spanish_` namespace to avoid collisions with old address-based C names.
After those corrections, the complete executable matches, including all raw
regions and generated fallback assembly.

## C-owned jump tables and small data

The second batch adds 97 functions / 82,096 text bytes from 39 unchanged source
units, together with 2,460 bytes emitted by those same C objects:

| Owned section | Units | Functions | Text bytes | Data bytes |
|---|---:|---:|---:|---:|
| `.rodata` | 36 | 85 | 74,256 | 2,437 |
| `.sdata` | 3 | 12 | 7,840 | 23 |
| Total | 39 | 97 | 82,096 | 2,460 |

The read-only data comprises the shared jump tables and constant strings for
duel scenes, file transfers, library menus, rewards, card sorting, memory-card
handling, model loading, and AI script dispatch. The small-data units own duel
trap state, card-effect tables, and model debug state. Spanish section
placement is recovered from actual instruction relocations, then each object's
text and owned data are compared against the retail image before integration.

The split now names those C-owned sections explicitly and retains binary gaps.
The same `SUBALIGN(2)` as the existing regional builds is required: the first
isolated probes using default input alignment displaced several jump tables.
Odd-length strings and byte tables need explicit zero-byte `pad` subsegments
before the next raw region; otherwise its two-byte alignment inserts extra
bytes. Correcting that layout yields the full executable hash without changing
source or compiler options.

The seven exported owned-data symbols remain section-defined in the final ELF,
and all 1,140 matching function entries resolve to exact-size linked C symbols.
External references to already matched functions must not become absolute
linker assignments: the inventory's linked-symbol gate detects that mistake
even if the executable hash matches.

There are no remaining resident C targets. All previously classified handwritten/SDK
assembly is unchanged. These numbers do not claim the raw gaps as C data.

The three `debug_effect_screen.c` functions now match through the Spanish
wrapper: the earlier conflicting bindings came from applying the European
coordinate/page offsets (2/4) to the Spanish state window (4/6). All 18
GP-relative references now agree on the measured base `0x8009C2C0` after
accounting for the changed object relocation addends. No inconsistent aliases
or out-of-bounds declarations are used; the accessed window remains raw data,
not claimed C-owned data.
Candidate shape scans, relocation evidence and failed-link logs remain local
under `tmp/`.

## Graphics storage and model-handler bindings

The graphics/model batch adds four functions / 848 text bytes from three
unchanged shared C units. `graphics_frame.c` supplies both frame functions
(696 bytes) and its two initialized viewport halfwords at `0x8009C4C0` and
`0x8009C4C2`. Despite the input section name `.sbss`, those explicit
initializers produce four PROGBITS bytes. The separate `viewport_state` code
segment follows the existing North American split pattern and keeps both
symbols section-defined, with their former absolute assignments removed.
The preceding and following raw ranges remain intact.

All eight tentative COMMON definitions in this unit resolve to independently
measured runtime addresses wholly inside startup-cleared BSS
(`0x8009C408..0x800FFC30`). They do not claim additional initialized data.
The final ELF's `.viewport_state` is a four-byte PROGBITS section matching the
retail zeros; this ownership check supplements the complete executable hash.

The two 76-byte model-handler units were ambiguous under relocation-masked
shape scanning. Existing caller bindings fix their entries at `0x8005F530`
and `0x8005F7D4`. Their distinct selector callees are verified through the same
complete, already matched source/profile groups: `Model_GetPrimitiveHandler`
at `0x8005F0CC` and `func_800608B8` at `0x8005F57C`. Both then link exactly
with fixed callee addresses, without selecting arbitrary aliases merely to
fit the masked bodies. No C source or compiler profile changes are needed.

## Localized script images

The four-function script-image group at `0x8002DFE8..0x8002E3C0` adds
984 matching text bytes through one thin Spanish wrapper. It shares the
European callback geometry and sector base. The request function additionally
remaps image ids `0x10` and `0x11` according to the existing language byte at
`0x8009C44B`, before recording the id and selecting the transfer table.
The shared implementation gates this behavior with `SCRIPT_IMAGE_LOCALIZED_IDS`.

The same complete 984-byte group is byte-identical in the French executable,
although it was still assembly in the French matching manifest when checked.
No French metadata is changed here. The North American, Japanese and European
source/profile objects remain byte-identical; the localized store preserves
the existing public function signature. See the
[wrapper evidence](../../src/game/spanish/README.md#localized-script-images)
for the remap table and rejected probes.

## Anchored empty handlers

Six eight-byte handlers now use four unchanged shared source groups. Their
identical `jr ra; nop` bodies cannot establish identity by themselves.
Instead, each complete group fills the exact gap between already matched
European/Spanish neighbor groups with the same source, profile, function
order and sizes:

| Spanish range | Shared source | Preceding function | Following function |
|---|---|---|---|
| `0x80028474..0x8002847C` | `func_800283EC.c` | `DuelEffect_UpdateDialogState` | `DuelEffect_UpdateCardViewerState` |
| `0x8002BC30..0x8002BC38` | `european/library_runtime_8002BAAC.c` | `func_8002BAA0` | `func_8002BAB4` |
| `0x8002C734..0x8002C744` | `noop_callbacks.c` | `Library_CheckCardOwned` | `func_8002C570` |
| `0x8002F694..0x8002F6A4` | `european/script_noop_halt15.c` | `func_8002EF3C` | `Script_OpFadeOut` |

The library handler additionally has an existing independently recovered
direct-call binding. Script command-table slots 14 and 15 at
`0x800920A0`/`0x800920A4` independently point to `0x8002F694`/`0x8002F69C`,
preserving the two handlers' declaration order. The unreferenced empty
callbacks retain address-based names; no new semantics are claimed.

All six compiled bodies equal the retail bytes, with no relocations or
allocated data. Full executable matching and linked inventory validation
confirm their extents. No C source, compiler profile, data ownership, or
other region is changed.

## Localized result outro and resident completion

The final four functions at `0x80020CB0..0x80021614` contribute 2,404 bytes
through `spanish/duel_result_orbit_sprites.c`, using the existing shared body
and `gcc_2_8_1_g8_split` profile. A private header declares eight measured
two-by-ten sprite tables and the dynamic active-slot count without changing
the public seven-element tables used by existing regions.

`DUEL_RESULT_LOCALIZED_SPRITES` selects language tables 1..4, skips entries
whose first byte is zero, records `i + 1` before testing the kind byte, and
uses that count for the later retargeting loop. All five existing affected
North American, Japanese and European source/profile objects are unchanged.
The four-function group is also byte-identical in the French, Italian and
German retail images; this does not itself claim their build integration.

The clean full Spanish executable and the original six overlays match their target
hashes, and the linked inventory verifies every matching C function's address
and size. All prior 1,136 entries and the seven real initialized-data exports
are preserved. This completes the resident game C target inventory without
reclassifying any assembly or adding inline assembly.

All 61 game-owned handwritten assembly functions were independently compared
against the classified European inventory: equal sizes and complete
opcode/register shapes, with no European game C function sharing those
shapes. Each inventory row records its European counterpart. They remain
assembly and are excluded from C-target progress. The embedded SDK getter at
`0x8005C018..0x8005C028`, startup, and the library range
`0x80073C4C..0x800918DC` are likewise not C candidates.

The image-map boundaries are measured independently of the C selection:
entry point `0x800128CC`, final resident return at `0x800918D4`, and startup
BSS clearing of `0x8009C408..0x800FFC30`. Except for the explicitly C-owned
sections above, the initial data and remainder of the executable are preserved
as binary regions. This does not count those regions as decompiled C data or
replace runtime-overlay validation.

The Spanish CI workflow checks both resident and runtime-overlay matches.
Progress uses `functions.csv` and the matching manifests, validates all
ownership/range relationships, and retains separate overlay counts. Snapshot
refreshes remain independent work under #443.
