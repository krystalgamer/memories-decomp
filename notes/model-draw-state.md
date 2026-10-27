# Model draw state and shadow fan

## Dimension queries and requested draw mode

`Model_GetSlotDimensionsWithMinimums` is the former `func_80057E20`.
The renderer derives padded, smoothed dimensions from eligible unit origins,
not an exact mesh AABB. The query copies all four cached halfwords, floors
each axis against its nonzero metadata byte times sixteen, then replaces
the copied fourth metric with the largest nonnegative output axis.

`Model_GetSlotDimensionsWithOverrides` is the former `func_80059000`.
It copies the same cached words but replaces an axis whenever its metadata
byte is nonzero, even when the replacement is smaller. It then recomputes
the maximum. This is not the minimum-floor policy; the two implementations,
signed comparisons and distinct output pointer types remain separate.
The metadata fields and `ModelEffectAdjustment` type are not renamed.

`Model_SetSlotDrawMode` is the former `func_80059284`. Its unchecked integer
slot/value arguments still narrow through the original byte store at
`+0xE11`. The `int` definition and compatible `s32` declaration are retained.
No enum, Boolean interpretation or input validation is added.

The minimum query retains `gcc_2_8_1_g8` and `0x118`; the override query
retains its existing grouped source, `gcc_2_8_1_g8_split` and `0xDC`;
the setter retains `gcc_2_8_1_g0_split` and `0x28` in every native image.
The two standalone C sources and minimum-query header move to semantic
paths without extracting or reordering the grouped source.

French variant199/829 C bodies retain their historical bytes and attempt
fingerprints through one old-spelling bridge in the owning minimum-query
header. Their headers change only the include path. Existing regression
controls change identifiers/include paths only; the full numeric Lua runtime
and trace-control tree remains unchanged.

## Captured mode and tint

`gModel_bCurrentDrawMode` is the former `D_8009AFE4`.
`Model_DrawSlot` captures the slot's `+0xE11` byte before clearing that byte
unless it is four. Template tinting can publish mode three independently
of the slot byte. Units-only, shadow and continuation paths read the captured
mode; it is not a Boolean flag or a newly classified enum.

The initialized `u8` `.sdata` owner and the renderer's tentative COMMON
declaration remain separate. The renderer retains its exact
`gcc_2_8_1_g8_split_comm` profile, including MASPSX's COMMON handling.
No storage, qualifier, field layout or publication-order change is implied.

`gModel_wDrawTintRedGreen` and `gModel_bDrawTintBlue` are the former
`D_8009AFE6` and `D_8009AFE8`. The template stamp writes red and green
through byte views of the `u16` word and writes blue separately. The neutral
path publishes `0x80` per channel. The shadow fan reads those same values
for its centre and divided rim colours. Their initialized `.sdata` types,
byte ordering, unknown neighbours and byte-view accesses remain unchanged;
no RGB structure or wider allocation is introduced.

`gModel_aShadowFanSteps` is the former `D_800914E8`. Its unsized `u16`
declaration and sixteen initialized halfwords stay exact. The renderer still
reads eight signed x/z Q12 pairs through `ModelShadowFanStep`; storage is
not retyped to a complete array of that view.

## Native bindings

The NA owner labels and explicit Japanese provider bindings acquire the
semantic names. The eight obsolete Japanese/European renderer macros retire
so the C objects publish canonical references.

European link scripts expose the semantic names as aliases to their original
providers: `D_8009BF6C`, `D_8009BF6E`, `D_8009BF70` and `D_800925CC`.
The existing providers and their per-image bindings remain unchanged.
This preserves the original resolution instead of guessing a numeric UK
address or copying a continental address. No new object or storage follows
from a linker alias. Full applicable native image acceptance remains a
PR-side gate; no local UK retail verification is claimed.

All ordering-table identities and renderer views accepted before this
component are retained. The unrelated primitive pointer, remaining handler
bytes and diagnostics stay address-named.

## Runtime observation

In two traced 3-D battles (#418, interpreter CPU, setup confirmed),
`Model_SetSlotDrawMode` only ever stored three: into slot 2 from
`0x8004ED1C`, `0x8004FDC4` and `0x8004FDE4`, and into slot 0 from
`0x8004EEA4`, `0x8004EF18` and `0x8004EFB4`. Polling slot 2's `+E11` saw
it return to zero each frame after a store, as `Model_DrawSlot` clears
it. Slot 2 also read four at each switch into duel main mode `0x03` and
back to zero on the way into a battle, without a setter call, so another
writer stores four directly.

A later five-scene run (#7930; four 3-D battles after the board scene)
identified that other writer. `Model_InitSlot` stores four from its
`flags & 0x7F` argument at NA `0x8004CBA4` for slot 2 while the board
scene (scene mode `195`) is active. It stores zero for every slot in battle
scenes (`193`). `Model_DrawSlotUnitsOnly` stores one around its units-only
draw and then restores the earlier byte, including four.
`Model_SetSlotAmbient` stores three 607 times in battles and zero twice.
`Model_DrawSlot` cleared the byte 10,234 times. All 928 setter calls again
stored three, from the same six return addresses inside
`Model_UpdateAnimatedBattle`. The run queued no tint requests, and the
human saw no colour change in any battle.

The code consumers fit this split. Zero takes the neutral template path,
one returns before the tint/shadow stages, three is the value that the
non-neutral template stamp also publishes, and four survives the per-frame
clear. These are observed values. They do not add an enum, and they do not
show that no other value is ever requested.

## Draw-unit matrix and animation-scan local views

The COMPLETE Model_DrawSlot definition now distinguishes ordering_table,
head_unit and coordinate_unit from the existing matrix work. The same
head-unit cursor still walks HMD entries in sorting AND both later
origin passes; coordinate_unit remains separate block-local pointers.
The later loops keep every root/parent exclusion, primitive-list test,
count-before-content test, fallback, signed extremum and narrowing.
These names do not make those passes mesh bounds or change cache freshness.

hidden_matrix is still cleared with the literal 0x20 before the masked
unit path selects it. local_to_world and local_to_screen retain their
declaration order and distinct GsGetLwsUnit output positions. The do/while(0)
call boundary, GsGetLsUnit alternative, light-matrix publication and final
local-to-screen selection remain. Mask tests, missing-primitive skips,
GsSortUnit order and ordering-table length changes are untouched.

The separate parent-child animation scan names sequence_index,
sequence_count and coordinate_rewrite_index. The rewrite expression,
u8 count snapshot, first-match break, aframe==0xFFFF test, extended-mask bit
and selected duration-row condition remain exact. scale_existing_matrix
names ONLY the branch that selects ScaleMatrix rather than the existing
coordinate-matrix update; it is not a new animation-completion policy.
Neither path changes the render_scale view or coordinate ownership.

Inside the lighting block, matrix_index walks the same NINE halfwords,
coefficient_magnitude retains the original ABS expression, and
maximum_magnitude stays u16. matrix_halfwords remains s16*, with the same
signed shifts/divisions and all cursor increments. Preserve the regional
reset placement: Japanese retail leaves the advanced cursor in place for
the zero-maximum writes, while the other images reset it first. No repair,
normalization helper, widened arithmetic or changed zero-case is added.

That matrix-local component changed only scoped bare locals and THREE
matching words in existing comments. At that point, function-scope
a/b/w/h/d/x/y/z/count/v/t retained their mixed mask, position, span, shadow
and tint roles; no binding was split or merged. The separate k/cp texture
bindings were unchanged then. The later phase-local views below extend
those names without changing the earlier matrix work. All arguments,
members, types, constants,
expressions, block lifetimes, macros, inline mirror helper, COMMON globals,
headers, SDK declarations, callers, profiles and native imports remain.
The complete tint, shadow, animation continuation and stone-texture paths
retain their original code, apart from the shared pointer-view names.
All fixtures/controls/oracles/mutations/numerics/flags remain unchanged;
none was imported or executed locally and no new harness is introduced.


## Complete fade, origin-span, shadow and texture local views

The COMPLETE Model_DrawSlot definition now also distinguishes the two
block-local signed blend_mode bindings from the outer u8 texture_state.
They were three DIFFERENT fl bindings: both tint stamp modes remain
separate from the stone-texture countdown/state. packed_template_color and
blend_mode_index likewise remain two separate locals in the two tint blocks.
The fade names fade_step/fade_red/next_red/next_green/next_blue retain the
old snapshot, stamp-before-advance order, source reads, strict comparisons
and final masked blue-byte comparison. Macro definitions and global
template/COMMON owners stay byte-identical.

metadata_bytes remains s8*, with the original casts and FOUR signed offset
reads. scan_index is still ONE shared binding for HMD units, shadow edges
and texture/palette loops; do not split its lifetimes. origin_count still
counts different eligible sets in the two origin passes: the second
direct-child count precedes its content test. min_origin/max_origin axis
names retain s16 storage and comparisons against full workm translations.
cached_dimension and dimension_step keep the original narrowing, zero path,
signed comparisons, 20-step cap and three separate update chains.
The deliberate mixed-role a/b/w/h/d/x/y/z and sxy_z bindings stay untouched;
no name implies tight mesh bounds, finite inputs or fresh positive spans.

The shadow block distinguishes center/next/previous edge vectors, triangle,
dimension query, clamped extent and projection depth/flags. Preserve vector/
packet declaration order, RGB center/rim arithmetic, 300..800 extent clamps,
unchecked slot2/background height reads and optional field clamps. The
NormalClip call and its write to sxy_z remain even though its result is NOT
the packet gate: the original depth>=0 and flags>=0 comparisons are still
the only projection-result tests. The skip label STILL copies the complete
next edge to the previous edge, including rejected/clamped fan segments.
No new backface, screen, null, background-active or shadow-visibility policy.

The texture block distinguishes texture_band_rect/texture_words from
palette_strip_rect/palette_words. texture_row and texture_word_index walk
the original two 256x4 bands; the two pixel_cursor bindings remain separate.
FE still ORs bit8000 only into nonzero texture words, with all original GPU
idle/transfer retry loops before/after each read/write. The other branch
still decrements/stores the byte state BEFORE constructing its 2x8 strip,
grays exactly 16 words and clears bit8000, then uploads at the original base
row. Keep European versus NA/JP coordinates and row cursors unchanged.
No new transfer completion, row-capacity or palette ownership guarantee.

The final remap_slot/remap_unit/remap_handler/group_bytes/next_group_bytes
names preserve the fresh slot read, active/count/null gates, ALL raw group
words, callback arguments and exact-1 chain terminator; zero is not a new
terminator. The earlier origin-pass group_bytes is a DIFFERENT lexical
binding. Playback-direction stop, final FE publication, animation/units-only/
part-mask early returns and every ordering-table length publication remain.

Only scoped bare locals across this WHOLE definition change. The earlier
matrix/sequence views, parameters, public headers, other functions, member
tokens, types, declarations/order/lifetimes, casts, constants, operators,
labels, calls, macros, comments, storage and named profiles remain.
One existing semantic NOTES cell is appended; all identities/evidence/
confidence and other rows stay. No fixture/control/oracle/mutation changes,
synthetic/generated-C/source-string behavioral harness, retired fixture
revival or local cloud execution.
