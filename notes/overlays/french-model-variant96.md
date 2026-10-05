# French MODEL headers 96 and 226

Models 192 and 196's stages 9/10 have exact 3,144-byte entry functions using
the named `gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81.
One implementation and a symbol-only slot wrapper supply all four entries
and their 16-byte scale constants. No shared SDK declarations, compiler
profiles or other regional implementations change.

## Loader and ownership

The [instance ledger](french-model-variant96-instances.csv) records four
independent complete 20,480-byte image hashes. Commands 27002 and 27004
select configuration indices 2 and 4. Both slot entry CFGs are closed and
cover all 786 instructions, with 39 calls to 23 resident destinations and
no local or indirect calls.

| Image offsets | Bytes | Owner |
|---|---:|---|
| `0..4` | 4 | raw module header |
| `4..0xC4C` | 3144 | sized C entry definition |
| `0xC4C..0xC5C` | 16 | C scale constant |
| `0xC5C..0xC78` | 28 | raw image descriptor view |
| `0xC78..0x5000` | 17288 | unclassified raw suffix |

Independent links reproduce all four complete, unmasked images. The C
entry has the exact linked address and ELF size. GCC emits the literal
symbol without object-size metadata, so its extent is established by its
exclusive 16-byte C rodata section and exact linked contribution. The
three raw ranges have distinct sized owners; typed descriptor views do
not claim new backing storage or classify the rest of the suffix.
All 23 actual resident callee bodies agree with the exact resident ELF.

## Measured layout and behavior

Twenty-six target-compiled assertions establish the 20-byte configuration,
existing SDK layouts and minimum `0xA34` context view. Two 65-vector ring
spans start at `0xC` and `0x214`; positions and scales start at `0x41C` and
`0x81C`. Rotation, preparation, texture, completion and elapsed fields
start at `0xA1C`, `0xA24`, `0xA28`, `0xA2C` and `0xA30`.
These are measured accessed spans, not an independent allocator-capacity
claim. The selected configurations use 32/16 ring segments and 16 centers;
all selected array accesses and positive divisors fit the recovered views.
The context does not overlap the primary, secondary or variant image loads.

Initialization constructs two rings, captures the active model-part
translation, uploads one texture and starts a negative delay. The first
nonnegative update interpolates centers toward the opposite slot, fixes its
Y coordinate at -350, doubles its Z coordinate and recovers orientation
with `ratan2`. Updates project both rings and draw gradient textured quads,
then advance each center's scale. A full-screen flash starts after
two-thirds of the center-spacing interval.

Native ordering is retained: scale advances after drawing; each scale
component is read separately; the first two vertex colors fade above scale
4096 and are not reset for every center. The shared projection array is
separate from each ring's depth and flag arrays. The 65-element stack arrays
naturally produce the native rounded eight-byte allocation spacing without
invented padding.

## Matching evidence and remaining scope

The [attempt ledger](french-model-variant96-attempts.csv) records two paired
experiments and two terminal canonical matches. The first candidate had
the exact 1,496-byte frame but 738 differing words per slot. Reusing the
native point walker across outer-ring initialization and later center
loops, and setting it before the matrix calls, recovered all instruction
bytes. No forced registers, artificial dependencies, inline assembly or
compiler changes are used.

This independent branch starts at accepted
`c176da5178c0e373642a498009f2e1dcc5c0150a`, preserves all 333 prior French
module registrations and adds four C instances: 12,576 instruction bytes
and 64 literal bytes. The four suffixes retain 69,152 unclassified bytes.
The clean French resident and all 337 production overlays match. Production
ELFs verify the four new owner sets, all 23 resident callees and three
resident controller owners. Seventy-three focused, progress, source-wiring
and extraction regressions pass. Configured totals are 1,769/1,989 C instances and 2,504,956 C
instruction bytes; these do not establish exhaustive runtime completion.
