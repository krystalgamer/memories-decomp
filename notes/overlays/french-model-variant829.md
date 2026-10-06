# French MODEL829 entries

The two 20,480-byte runtime images for model485 (record435), stages7/8,
contain exact 3,972-byte C entries and 16-byte source-owned VECTOR literals.
The headers are **829 and926**, a measured delta97, not130. Both command
words are361000 and directly select descriptor0. The full images, actual
linked C definitions, disjoint raw owners and20 resident callee bodies were
verified independently before canonical integration.

## Ownership

| Offset | Size | Owner |
| --- | --- | --- |
| 0 | 4 | Raw module header |
| 4 | 3972 | Matching C entry |
| 0xF88 | 16 | C VECTOR literal |
| 0xF98 | 112 | Raw four-GsIMAGE view |
| 0x1008 | 16376 | Unclassified suffix |

This contributes7,944 C instruction bytes and32 literal bytes. The32,752
suffix bytes remain unclassified; descriptor access is not exhaustive
ownership or code classification. Instance hashes and loader sectors are
recorded in [the instance ledger](french-model-variant829-instances.csv).

## Recovered behavior and declarations

The26-byte descriptor has eleven unsigned halfwords, two bytes and a final
unsigned startup-delay halfword. Both selected descriptors decode to
`80,100,80,40,80,48,64,128,64,96,15,8,0,100`. Unknown fields retain
offset-based names. No reference-project types or compiler flags are used.

The0x504-byte state contains four centers, four velocities and four
32-vector clouds. Its12-byte status union covers the global phase, all
four per-center states and the active count. The aligned `first_pair`
view expresses the native terminal word load without pointer type-punning.
An8-byte local union reuses authoritative ModelEffectAdjustment and SVECTOR
views: func_80057E20 returns adjustment dimensions, not a slot origin.
Forty target-compiled layout checks cover descriptor, context and SDK types.

Constructor depth is `-direction * 354`, while velocity targets
`direction * 450`. The random X remainder is tripled before multiplication
by amplitude. Its signed remainder lies in[-4095,4095], making the doubled
plus original expression safe; the largest unsigned-halfword amplitude
product also fits s32. The two random calls retain their order.

Rendering captures the frame step before overriding it with1. Startup
delay returns before PushMatrix. Four linear sprite cases deliberately
duplicate animated cases1/3, allowing the compiler's native common tail.
Main animated tile selection reuses the inner index; cloud tile selection
has a separate temporary. SDK addVector preserves the measured vector
base computations. The matrix chains omit MulMatrix2. Packet submission
tests depth only, not the projection flag.

Cloud counts narrow to u16 before capping32. The cloud pass includes state2,
and its Y subtraction is8 per update rather than frame-step scaled. RGB
and cloud fade preserve native delayed clamping and narrowing. Only
global phase and states[0] participate in the packed terminal comparisons:
0x00020002 returns2;0x00010000 sets phase1 and returns1.

## Experiment evidence

[The attempt ledger](french-model-variant829-attempts.csv) preserves
fourteen paired source experiments and two canonical terminal records.
All use the authoritative `gcc_2_8_1_g0_split` profile, GCC2.8.1 and
MASPSX2.81. Candidate sources and unmasked diffs remain local under tmp/.

The linear dispatch, SDK vector additions and step-before-OT declaration
recovered the exact size and stack. Reusing the main animated index
reduced37 differing words to7. Signedness and narrowing hypotheses failed;
captured amplitude staging changed scheduling. A constructor-local random
sample retained across the second rand, together with doubled remainder
plus original remainder, recovered every instruction without forced
registers, artificial stores or fake dependencies. Both literals and all
41 ordered calls to20 resident destinations also match.

Canonical sources are
`src/overlays/french_model_variant/variant829_entry.c`,
`variant829_entry.h` and `variant829_entry_slot1.c`.
Regressions are in `tools/project/tests/test_french_model_variant829.py`.
