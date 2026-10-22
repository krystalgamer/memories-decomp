# Direct model polygon renderer family

The thirty-two game-owned handwritten callbacks selected by
`Model_GetPrimitiveHandler` have names derived from their actual polygon
templates, backface handling, command bits and record layouts. They remain
`handwritten_asm`; no body is converted to C.

## Complete feature matrix

Names use `Model_Draw`, optional `TwoSided`, optional `Semi`,
`Ft3/Gt3/Ft4/Gt4`, optional `WindowRecord`, then `Primitives`.

| Packed group | Material behavior | Eight NA entries |
| --- | --- | --- |
| `0x00000000` | Base family | `612C0`, `6151C`, `617E0`, `61A84`, `61DDC`, `62058`, `6233C`, `62600` |
| `0x00100000` | No backface-area rejection | `62978`, `62BC0`, `62E70`, `63100`, `63444`, `636AC`, `6397C`, `63C2C` |
| `0x00200000` | Forced semi-transparency | `63F90`, `64248`, `64568`, `64868`, `64C1C`, `64EF4`, `65234`, `65554` |
| `0x00300000` | Both features | `65928`, `65BCC`, `65ED8`, `661C4`, `66564`, `66828`, `66B54`, `66E60` |

Addresses above carry the `0x800` prefix. Each row lists FT3, GT3, FT4,
GT4, then their four window-record variants. This research-table order
does not replace the original selector switch/goto order.

All thirty-two bodies independently reference the corresponding
`gModel_dwPolyFt3/Ft4/Gt3/Gt4Template`. The no-backface variants omit NCLIP
area rejection while their paired base variants retain it. `TwoSided` does
not mean that every FLAG/depth/clip test is disabled or that normals are
reversed.

The semi-transparent variants explicitly OR `0x02000000` into the first
polygon's command/template word. Existing TPAGE blend selection and optional
additional passes remain unchanged. Base variants are not guaranteed opaque:
global template/fade state can still enable blending.

No depth-cue behavior is inferred from descriptor bits. All thirty-two
functions use NCCS/NCCT-class operations, without
NCDS/NCDT/DPCS/DPCT/DCPL/INTPL. The separate fog-special and shared renderer
families are not renamed here.

`WindowRecord` means the extra input texture-window word. These filled
renderers submit its E2 state command and the existing reset command, unlike
wireframe callbacks that merely skip the field. Record strides remain:
FT3 `0x14`, GT3/FT4 `0x18`, GT4 `0x1C`, plus four bytes for window records.
Draw-mode branches may compute/reuse cached lighting without submitting a
polygon; names do not promise drawing on every invocation.

## Preserve the existing declaration differences

`Model_DrawGt3Primitives` and `Model_DrawGt4Primitives`, formerly
`func_8006151C` and `func_80061A84`, retain their accepted
`u32 *(GsARGUNIT_NORMAL *)` interfaces from the existing local SDK-based
headers. The fog-special C drivers' existing delegation calls and return
values keep that exact shape.

The other thirty retain their existing address-use `void (void)`
declarations. Similar assembly does not justify inventing typed interfaces
for them, and the two typed exceptions are not generalized away.
`direct_polygon_renderers.h` reuses the old typed headers without changing
any existing header bytes.

All caller-scratch save/restore offsets, return values, packet-work-cursor
stores, lighting/cache branches, culling/FLAG tests, window commands,
additional-pass behavior, packet links and terminators are untouched.
No wrapper, direct call, prototype repair or handwritten instruction is added.

## Native boundaries and acceptance

All 224 records across seven images are explicitly game-owned
`handwritten_asm`. Every native boundary is resolved through the existing
binding before consulting the function inventory; neither a NA suffix nor
apparent family contiguity supplies an address. For example, the two typed
German callbacks are at `0x8004CD98` and `0x8004D05C`, away from the other
direct drivers.

Canonical exports and compatibility bindings preserve the original logical
reference names. The selector and base mapper change only identifiers and
the isolated include, as do the existing fog delegation calls. Source
profiles, matching manifests, statuses, extents and historical evidence remain.

The existing central-declaration validator recognizes compatibility aliases
without rewriting frozen `unmatched.h`. Admission requires exactly one direct
linker assignment to a game-owned `handwritten_asm` inventory target at the
address encoded in the old numeric name. The statement must have its terminating
semicolon; an unterminated final fragment never qualifies. The complete statement
and LHS must match, and the old linker identifier must occur only once; a dotted or prefixed
symbol is not that legacy name. Statement delimiters inside quoted linker symbols
are not boundaries, and dotted, dollar-containing and hyphenated unquoted names
are counted whole. An exact quoted legacy name counts as a use, so a quoted
override cannot hide a second definition. Escaped or unterminated quoted names
conservatively reject compatibility admission rather than guessing their meaning.
Chains, duplicate/malformed aliases,
wrong addresses, missing targets and matching-C targets do not qualify.
Ordinary stale and matching-C central declarations are still rejected.
The existing policy-test module covers these metadata-only cases without
adding C fixtures or a new test harness.

Every existing header, overlay C/import map/metadata, test and trace remains
unchanged. Full comparisons preserve complete caller compiler/MASPSX output
and complete generated fallback assembly with only declared identities
reversed, not instruction/directive filtering. A clean full resident image
match and the PR's fresh regional/overlay image checks remain mandatory.
