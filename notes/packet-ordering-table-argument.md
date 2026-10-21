# Packet dispatcher ordering-table argument

`DisplayObject_SubmitPacket`'s third argument is now the actual `GsOT *`
descriptor, not an integer, texture or depth. All three measured caller
views retain their existing first/second/fifth parameter types and use
this same descriptor pointer. The SDK headers are included by the owner
so each declaration is self-contained.

`Graphics_BeginFrame` publishes the four typed descriptors through
`gGraphics_apOrderingTables`; the update, sprite-sheet, Gouraud and card callbacks select
those actual pointers. The direct card path also obtains its table from
the existing typed `D_800E9D98` slot view. The SDK sorter calls consume the
descriptor itself; the subdivision path uses its `org` pointer plus the
existing signed low-halfword priority.

All six actual resident caller translation units are wired. Already typed
table entries/local pointers are passed without the old integer cast.
At that dispatcher checkpoint, strips, sprite-sheet, card-frame and
detail-panel wrappers kept their word-shaped APIs and explicitly
converted only at this call boundary. The later strip/sheet descriptor
APIs are documented in `sprite-renderer-table-arguments.md`; the card
wrappers remain unchanged. There are no direct overlay C/header callers.

The dispatcher keeps every expression, case, SDK call, cast/view macro,
priority narrowing and operation order. `mode` still supplies its existing
handler/depth bits; neither its type nor the deliberately signed
`(s16)mode` depth expression is altered. No global/table type, allocation,
descriptor layout, profile, callback ABI or metadata changes.

Supported-compiler probes accept the descriptor in all three views and
reject both the old integer carrier and an ordering-table entry pointer.
All prior packet/origin/type/arity/visibility/swap negatives retain their
oracles while using the correct third type.

A separate witness uses four real guarded `GsOT` descriptors, a real
sprite and a real allocated depth-word array. It executes only the three
actual SDK-call boundary statements with observing stubs and the actual
`org + pri` expression: 1,048,576 sequences per `-O0`/`-O2`, across all
low-halfword depths and four high-word patterns. Exact pointer identity,
call counts, signed-depth resolution and all input/guard bytes are
checked. Negatives reject a wrong descriptor, unsigned depth and using
`tag` instead of `org`.

That witness is not a dispatcher, renderer, real sort, SDK runtime, GTE,
GPU or engine execution. Complete dispatcher and all six caller compiler/
MASPSX baselines remain byte-identical; full retail matching and normal
regional/overlay PR checks remain the acceptance gates.
