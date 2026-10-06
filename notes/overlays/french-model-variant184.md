# French MODEL variant184 entries

The six stage9/10 runtime images for models169,524,591 use headers184/314.
Each contributes one closed3,824-byte entry and a16-byte source-owned unit
scale vector under the existing `gcc_2_8_1_g0_split` profile. The local
GCC2.8.1/MASPSX2.81 pipeline and shared SDK declarations are unchanged.

## Evidence and ownership

All956 instructions in each slot's entry agree with native bytes. Each entry
has51 ordered direct calls to25 resident destinations and no local or indirect
calls. Independent linking compares every byte of each20,480-byte image,
including12 actual C entry/literal contributions and18 disjoint raw owners.
The resident executable and all25 imported callee bodies are independently
checked against retail bytes and the actual resident ELF.

The owned spans are the four-byte raw module header, C entry at0x4..0xEF4,
C unit-scale literal at0xEF4..0xF04,84 raw bytes containing three image records
at0xF04..0xF58, and the unclassified suffix at0xF58..0x5000.
This adds22,944 C instruction bytes and96 literal bytes.
The99,312 suffix bytes across all six images remain unclassified; access
views and complete-image identity do not imply exhaustive runtime coverage.

The28-byte descriptor view is selected by the actual loader command:
model524 uses115000 and models169/591 use115001. All six selected records are
decoded independently. Their particle counts are80 and24; their spread and
travel/burst divisors are positive. Unread descriptor bytes3..5 retain unknown
names. The command-group byte still receives `command / 100`.

The0x738-byte context view contains80 observed positions and80 observed
targets, with an unclassified0x80-byte gap after each array. It does not assert
96-element ownership from address spacing. The copied opposing-slot origin,
34 contiguous ring vertices, three packed unsigned texture words, completion
byte, elapsed time and update counter retain their measured offsets.
Thirty-two target-compiled size/offset assertions protect this view.
Allocator ownership is not inferred.

## Native behavior preserved

Construction retains the otherwise unused initial active-slot query, all
random and trigonometric call ordering, staged signed division and origin
translation. The ring consists of two17-vertex banks, not two34-vertex banks.
The first experiment's reason used that imprecise phrase; its frozen source
already allocated and projected only34 vertices.

The moving pass captures the current model part before launch, draws an
arched sprite, and then updates each coordinate using division before frame
step multiplication. Travel colors are captured separately from the burst
fade quotient temporaries. The otherwise unused POLY_F4 initialization remains
a real SDK call. Neither rendering matrix chain introduces MulMatrix2.

The burst sprite and ring share their matrix, scale and faded RGB. Ring
visibility intentionally reads only the two flags from the first bank.
The first vertex flag is captured before combining the second, preserving
native load order without artificial stores or dependencies.

The final duration comparison is strictly greater-than. A leading negative
time branch with an explicit else preserves the completion-state reloads:
first completion increments the byte and returns1; later completion returns2;
the intermediate phase returns4.

## Refinement and acceptance

The28-row ledger preserves thirteen paired experiments plus two canonical
matches. Native bank cursors recovered the680-byte frame. Cursor-before-index
updates removed scheduling bubbles. Unsigned packed texture words recovered
three unsigned halfword loads; branch-local travel colors recovered the native
register lifetimes. The negative terminal gate and first-flag capture resolved
the remaining mismatches.

There are no forced registers, artificial stores, fake dependencies, inline
assembly, source-local extern declarations or imported reference-header types.
The bank-cursor structure follows accepted local MODEL177 code while retaining
MODEL184's independently recovered flat34-element arrays and two-flag gate.
Final integration requires a fresh clean French resident and every configured
overlay, actual linked ownership, focused regressions and repository policies.
