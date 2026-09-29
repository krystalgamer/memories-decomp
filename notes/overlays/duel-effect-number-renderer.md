# Duel-effect projected number renderer

The complete Spanish `func_801566D4` interval,
`0x801566D4..0x80156AD4`, is 1,024 bytes of matching C in
`src/overlays/duel_effects/number_renderer.c`. It uses the existing
`gcc_2_8_1_g0_split` profile: GCC 2.8.1 and MASPSX 2.81. The existing drawing
header owns the glyph-table declaration, keeping source-local `extern`s out
of the C file as required by #6459. No compiler profile, inline assembly or
external reference declaration changes are needed.

## Evidence and preserved behavior

The existing canonical declaration takes a signed word value, RGB bytes,
an `SVECTOR` offset and unsigned halfword mode, size and bias. The digit-count
callee still receives its signed-halfword argument and the digit converter
still receives the absolute value through its unsigned-halfword contract.
Do not broaden these helper contracts or introduce new input clamping.

The renderer constructs twenty local vertices, uses eight digit halfwords
and retains two complete 40-byte `POLY_FT4` packets in the original
0x160-byte frame. It projects four adjacent vertices per glyph. The first
glyph selects index 10 for a positive value and 11 otherwise; subsequent
glyphs come from the digit buffer. The otherwise unread whole-packet copy
is intentional: retail executes that copy.

With zero bias, nonnegative projection flags permit modes 1 and 0 only.
Both branches assign **both** `draw_packet` and `draw_mode` before one common
`func_80152F9C` call. The packet temporary and its declaration order are
required for the observed old-GCC branch/register allocation. Collapsing
the two assignments into a mode-only selection produced 1,016 bytes with
53 differing words; duplicating direct calls produced 1,032 bytes with
64 differing words. Assigning both arguments reproduced all 256 instructions.
Moving the vertex increment before that common call retained the correct
size but changed 13 words, and was rejected.

With nonzero bias the depth is `depth + 1 - bias`; negative depth or
projection flags suppress submission. Mode 1 submits with blend 1;
every other mode submits with blend 0. The submitted depth is the
unsigned-halfword conversion of its arithmetic right shift by two.
Every path advances the vertex index by two. These paths are not
interchangeable with the zero-bias mode restrictions.

## Actual data ownership

| Owner | Address | Bytes | Interpretation |
|---|---|---:|---|
| Glyph rectangles | `0x8015B3C0` | 96 | Twelve canonical 8-byte `RECT` entries |
| Texture words | `0x8015B748` | 84 | Existing canonical 21-pair union |
| Ordering-table pointer | `0x8015B7F4` | 4 | Existing generated-data pointer |

The glyph rectangles have x/y/width/height at offsets 0/2/4/6. All twelve
observed dimensions are 15 by 15; their 96-byte extent ends at `0x8015B420`.
That following address is separate effect-1 configuration, not part of
the glyph table. The renderer reads texture pair 9 at offsets 0x24/0x26
of the canonical union rather than inventing another prefix declaration.

Twenty target-GCC constants verified these sizes and offsets, packet
coordinate/tpage/clut offsets, vertex/digit array extents and the 32-bit
SDK `long`. Each of the three data symbols has exactly one non-executable
generated-data input-object definition, with its measured size and original
bytes. The final symbols are real section-backed objects at their original
addresses, not linker-script absolute aliases. The final linker combines
code and data into `.module`; its executable flag is not the input-data
ownership test. The renderer object defines no data, BSS or constant storage.

The digit count, digit conversion, alternating-sign helper and FT4 submitter
remain four real executable overlay functions with inventory-backed extents.
All 75 accepted Spanish C entries from `b73bc0b514b15310250a6305f81e44284aeb10f8`
are preserved unchanged.

## Validation and scope

Before registration, the candidate and every accepted C group independently
reproduced the complete 90,112-byte bank. Production
`make spanish-match-overlays` then reproduced every configured Spanish
image. All seven actual terrain copies at `7193 + 240 * terrain` were
compared in full against the production bank:
`a58fb697a7886af81be33974b3f60348d950e9f7a1c7127216ab03e2b87f38b3`.
After placing the glyph declaration in the canonical drawing header, all
33 configured Spanish, French, English PAL, Japanese and North American
overlay images were rebuilt sequentially and remained exact. The renderer's
owner/layout checks and the then-current 133 duel regressions also passed.

After the additive rebase preserving accepted effects 2/21, this adds one
function / 1,024 bytes: 76 of 85 bank functions / 46,548 C bytes, or 200 of
209 configured Spanish function instances / 102,400 C bytes.
Nine bank functions remain unmatched, including the curve
generator. No other regional renderer is registered by this change.
Boot, MODEL/SU and opaque overworld fragments remain separate open scope.

The renderer C and shared drawing header remain byte-identical to the
previously published head. The accepted-base refresh repeats complete
Spanish production matching, real-owner/layout checks, duel regressions,
metadata checks and clean resident matching; it does not substitute
previous-head CI for checks on the newly published head.

The historical failures, four final argument-order probes, source hashes,
whole-bank preflight and production owner/layout receipts remain local
under `tmp/number87-proof`, `tmp/number-renderer-full` and
`tmp/number89-*.json`. No retail bytes or generated artifacts are tracked.

## Accepted effect 11 integration

Additive rebase onto accepted `e9295667f` preserves all 76 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed implementation source/header files remain byte-identical.
The combined branch has 77/85 bank C functions / 51,596 bytes,
8 explicit assembly boundaries and 201/209 configured C
instances / 107,448 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Accepted effect 17 integration

Additive rebase onto accepted `99a615a8d` preserves all 77 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed implementation source/header files remain byte-identical.
The combined branch has 78/85 bank C functions / 56,696 bytes,
7 explicit assembly boundaries and 202/209 configured C
instances / 112,548 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Accepted effects 7/13 integration

Additive rebase onto accepted `c61e79ad3` preserves all 79 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed implementation source/header files remain byte-identical.
The combined branch has 80/85 bank C functions / 62,288 bytes,
5 explicit assembly boundaries and 204/209 configured C
instances / 118,140 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Accepted effects 16/20 integration

Additive rebase onto accepted `9312d3260` preserves all 80 accepted Spanish
entries, including every accepted lifecycle, real owner and test.
The previously reviewed implementation source/header files remain byte-identical.
The combined branch has 81/85 bank C functions / 65,088 bytes,
4 explicit assembly boundaries and 205/209 configured C
instances / 120,940 bytes. Complete production Spanish images,
linked ownership, metadata and duel regressions are checked on this head.

## Independent French registration

The French renderer independently matches the same complete 1,024-byte
interval using the unchanged shared C, headers and named compiler profile.
Its accepted base is `3ed6b0f15`. Together with the separately verified
effect 16/20 lifecycle, the French registration adds 3,824 C bytes while
preserving all 77 accepted French entries unchanged: 79/85 bank functions,
56,852 C bytes, and 203/209 configured function instances / 112,704 C bytes.
Pending matching branches are not included in those totals.

The independent preflight links both candidates with the accepted French C
and generated assembly, reproducing the entire 90,112-byte bank with the
hash above. It checks all 79 linked function extents and all 45 complete
C input objects. The 96-byte glyph table, 84-byte texture union and four-byte
ordering-table pointer each retain exactly one real generated-data input
definition and their full original bytes. No new resident alias, shared
declaration, macro, compiler flag or source-local external is needed.

The terminal preflight and production receipts are retained locally under
`tmp/coverage-probes/number-sixteen/`; the original rejected French
experiments remain under `tmp/coverage-probes/`. Production acceptance
repeats all seven configured French images and all seven terrain copies,
exact input/final ownership and clean resident matching. This registration
does not close the six remaining bank boundaries or resolve boot,
MODEL/SU and overworld-tail coverage.

The subsequent additive integration of accepted `273e05386` retains all nine
updated French PAL-wrapper/contour bindings and the current drawing header.
The two newly matched bodies remain unchanged. Current-head verification
checks 46 complete bank C objects, all 79 bank owners, six real data owners,
all seven French images, 171 duel regressions and 16 progress regressions.
Configured totals remain 203 functions / 112,704 C bytes; the progress
regression expectations now match those totals.

## French accepted effect 24 integration

The additive merge of accepted `0832a2be4` preserves all 78 accepted French
bank entries and adds only the two reviewed functions: 80/85 bank functions,
60,808 C bytes and 204/209 configured instances / 116,660 C bytes.
The complete shared C/header tree remains identical to this accepted base,
including the newly accepted North American registrations. The French
pair still uses its original shared sources; no additional wrapper is needed.
Production acceptance repeats all seven complete French images and terrain
copies, all 47 complete bank C objects, function/data ownership and focused
regressions. Five bank boundaries and broader runtime coverage remain open.

### Accepted French effect 11

The additive integration of accepted `4329554d9` retains all 79 accepted
French entries, including effect 11, and adds only the two reviewed
functions. The unchanged shared source/header tree yields 81/85 bank
functions / 65,856 C bytes and 205/209 configured instances / 121,708 bytes.
Complete French images, all 48 whole bank C objects, actual function/data
owners and combined regressions are checked again. Four bank boundaries
and broader runtime coverage remain open; pending effects are not stacked.
