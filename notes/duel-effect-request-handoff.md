# Resident and bank request phase/progress handoff

The resident request updater and the complete bank dispatcher share an
already recovered 32-byte `DuelEffectRequest`. This component names the
proven handoff, not new effects, gameplay outcomes or guessed coordinate
semantics.

| Contract | Former spelling | Canonical spelling | Preserved view |
|---|---|---|---|
| Bank entry | `func_80146258` / resident `func_801462B0` | `DuelEffect_DispatchRequest` | Same real function and native labels |
| Dispatch command | `DuelEffectRequest.field_1A` | `phase` | Signed 16-bit member at `0x1A` |
| Handler progress | `DuelEffectRequest.field_1D` | `progress` | Unsigned byte at `0x1D` |

## The actual cross-image entry

The NA resident linker binds `func_801462B0` to `0x801462B0`; the Japanese
resident binds that same caller identity to `0x801542B0`; PAL binds it to
`0x80146258`. The bank maps bind its native source label `func_80146258`
to those same regional addresses. Its full extent is `0x508` / 1,288
bytes. It remains the real C provider in `dispatch.c`, not an absolute
function alias or wrapper.

Both existing declaration headers retain their native compatibility
prototypes and add the canonical C name with the original `__asm__` label.
Both declarations use explicit `extern`, following existing semantic-view
bindings and keeping each real native owner visible to the unchanged
tracked-symbol policy.
The resident view in `external_funcs.h` deliberately remains
`(s16, s16, s32, DuelEffectRequest *)`; the bank view in `dispatch.h` remains
`(s32, s32, void *, DuelEffectRequest *)`. They are separate translation-unit
views of the measured handoff, not newly normalized prototypes. The
resident's signed halfword loads, `s32 savedF1A`, integer buffer cast,
argument expressions and all call-site instructions remain exact.

The dispatcher still copies the request's three neutral payload halfwords
to the draw offset and the original halfword to base priority. It initializes
21 packed texture pairs only for nonnegative phase, then retains all 26
independent effect comparisons and exactly 25 distinct handlers. Unknown
IDs still receive preceding setup. Case nine retains both OT writes; only
cases two and thirteen receive the request's signed auxiliary halfword.
Case twenty retains both explicit calls and its nonnegative-to-one command
mapping. No dispatch table, new fallback, cached context or renderer
reordering is introduced.

## Signed phase is a command, not a Boolean

The allocator sets phase to zero. Before the first dispatch, the updater
captures its signed value in the existing `s32` local, sets INITIALIZED
and replaces the stored phase with -1. The *captured* value still reaches
the first call. Later calls read the stored value again. No sign, width,
mask or reset order changes.

Resident creators use this member for numeric variants, sides, card IDs
or debug operands according to the selected handler. Terrain later writes
-2 to request dissolution after progress; that is not request completion.
Swords writes a positive phase while clearing an enabled slot, or
`~(abs(phase) % 2)` after initialization, preserving the signed slot command
and the later `abs(phase + 1)` lookup. Its final fade test remains separate.
The name does not assert a uniform enum, card identity or monotonic state.

The payload members at `0x00/0x02/0x04`, both integer-word OT addresses,
base-priority halfword and signed auxiliary halfword remain unchanged.
In particular, the coordinate ambiguity recorded in the original request
header is not resolved by renaming unrelated fields.

## Progress is distinct from lifetime and finished status

The allocator clears the unsigned byte. Ordinary handlers often publish
one, but row strike publishes its hit count and Swords publishes one or
two. Resident row-strike logic waits for an exact next count, Swords
waits for a threshold of two, and other controllers use zero/nonzero.
It is neither a Boolean nor a universal final-completion field.

Terrain publishes readiness before the resident sends its negative dissolve
command. Stop Defense waits for progress before changing a card, then
separately waits for ACTIVE to clear. The updater frees a request only
when the distinct shared `gDuelEffect_bRequestFinished` byte equals one.
All flags, comparisons, counters, early returns, progress stores and
gameplay mutation timing remain in their original order.

All nine resident translation units and all sixteen bank member consumers
use the canonical member names, plus the dispatcher provider. Other
`field_1A` members belong to display geometry or deliberately retained
partial legacy types and are not blanket-renamed. No union, compatibility
macro or duplicate request layout is added.

## Exact scope and historical evidence

This independent component starts from accepted music-grouping master
`caeafc5637a5782ba94701fa9e588f5d9ef9bc6f`. It preserves accepted fallback,
canonical live definitions, all MODEL/grouping registrations/deletions and
all previous lifecycle/data work. Pending Fusion/Attack Slash and remaining
live-handler data components are not imported.

For each of the 26 complete actual C sources, the only changes are these
member/function identifiers, including their comments. Forward substitution
of those identifiers in the accepted complete source reconstructs the
candidate byte-for-byte. The complete objects cover 60 real functions /
67,608 instruction bytes. Their existing named G8 split, G8 split/comm
and bank G0 split profiles remain authoritative.

Seven existing request controls receive only the same literal member-name
substitutions. Their C models, cases, expected values, raw offsets, width
checks, mutation operators, compiler setup and scenario counts are unchanged.
No new synthetic/generated-C/source-string harness, fixture or test case is
introduced, and cloud tests are not executed locally.

Only the intended dispatcher name cell changes in each of the five bank
inventories; addresses, sizes, statuses and owners remain exact. Native
maps, manifest profiles, data definitions, the six live semantic bindings,
complete canonical-callee/data-owner guard and all 25 handler identities
are unchanged. The complete accepted registry remains an exact prefix
before one dispatcher row; member semantics are documented here rather
than pretending struct offsets are new global addresses.

The native bolt definition/utility header retain fingerprint
`92265855122745bd0783dea64ff14758454b424223167da70446e6d09851524e`.
The original attempt ledger, Spanish regression, focused null-offset probe
and its replay stay byte-exact. Historical notes may retain the original
member/entry spellings above; they are not claims of different storage.

Actual native compiler/MASPSX assembly, symbols and disassembly/relocations
are compared for all 26 complete sources. Full-image acceptance remains
mandatory. Native conservation is not a local Japanese/PAL/MODEL or cloud
execution claim.
