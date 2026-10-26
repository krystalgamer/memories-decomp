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
21 packed texture pairs only for nonnegative phase, then retains all 25
independent effect-ID comparisons and exactly 24 distinct handlers. The
26 syntactic handler call sites include case twenty's two alternatives;
the guard's 25 distinct overlay callees also include the texture helper.
Unknown
IDs still receive preceding setup. Case nine retains both OT writes; only
cases two and thirteen receive the request's signed auxiliary halfword.
Case twenty retains both explicit calls and its nonnegative-to-one command
mapping. No dispatch table, new fallback, cached context or renderer
reordering is introduced.

## Request ordering-table and base-priority bindings

The rendering handoff names three more proven members of the same request:
ordering_table_2 is the original s32 field_08 at +0x08,
ordering_table_1 is the original s32 field_0C at +0x0C, and
base_priority is the original s16 field_10 at +0x10. The numeric suffixes
identify the actual gGraphics_apOrderingTables elements, not guessed
foreground/background ownership or an exclusive effect category.

DuelEffect_AllocateRequest (0x8002C604) writes table [2] to the first word,
loads table [1] through its original local into the second word and stores
8 as base priority. DuelEffect_UpdateRequests (0x8002C6C8) refreshes both
addresses for every active entry before dispatch, retaining the original
table-[1] store, active-request publication, table-[2] store order. These
are current per-dispatch bindings, not pointers latched only on allocation.
All integer-word declarations, pointer casts, local widths and separate
table loads remain unchanged; the names do not convert them to C pointers.

DuelEffect_DispatchRequest reads the stored signed priority halfword through
its original u16 local and writes the existing u16 gDuelEffect_wBasePriority.
Do not retype the member, sign-extend the load, add a mask or normalize the
local. The named render-state view and complete screen/packet helpers prove
the role: they submit to gDuelEffect_pOrderingTable at base priority or the
original wrapped base-priority-plus-one. Their separate flags/mode operands
and FT4/GT4 branch-specific arithmetic remain unchanged.

The complete 25-case dispatcher retains all 26 ordering-table stores:
13 from each binding. In particular case nine still stores table [2]
and then table [1] before inverted recovery; the first store is not
deleted as redundant. Cases that set geometry offset (160, 108) retain
that call and its order. Unknown IDs still receive the original offset,
priority and nonnegative-phase texture setup, with no new default.
No comparison, handler, auxiliary argument, command mapping, completion
rule or packet submission changes.

Only the two complete producer/dispatcher C units change under 32
owner-qualified non-comment member identifiers and two existing updater
comment identifiers. The request header changes three declaration names,
the existing +0x08 assertion's member and its label, without changing any
type, width, size, offset, annotation, assertion expression or numeric
expectation. Other field_08/field_0C/field_10 owners are untouched,
including ritual scratch, glyph/view/cursor state and display callbacks.
There are zero fixture edits and no local cloud-fixture execution.
Payload coordinates and the signed auxiliary halfword remain anonymous:
these bindings do not resolve their handler-dependent meanings.

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

The payload members at `0x00/0x02/0x04` and signed auxiliary halfword
remain anonymous. The rendering bindings named above retain both original
integer-word OT addresses and the base-priority halfword representation.
The coordinate ambiguity recorded in the original request header is not
resolved by naming those independent bindings.

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
all previous lifecycle/data work. It normally integrates the subsequently
accepted Fusion/Attack Slash data squash
`f353063c35a3ef75ed0a724c405855e696e291bc`, preserving its complete registry
prefix, both full canonical providers, four data views and original owners.
It also normally integrates the subsequently accepted remaining live-handler
data squash `e975a55339e541083b98365ae44e25e0530b224a`, retaining all nine
data-owner records before the dispatcher row, all four complete providers
and their original storage. Terrain's accepted data names/include coexist
with the progress member rename and canonical live/fallback calls.

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
complete canonical-callee/data-owner guard and all 25 overlay callee identities
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
