# Direct card-frame table descriptor

`Duel_DrawCardFrame` receives the actual selected
`GsOT *ordering_table` as its second parameter and passes it to the
already-typed packet dispatcher. This is not a depth, table entry or
texture identifier.

The projected-card caller selects `D_800E9D98[0]` through the existing
typed interior-slot array view. It now passes that pointer directly,
retaining the volatile scratch stores, RTPS macros, second record reload
and projected coordinate stores. Those scheduling-sensitive operations
are otherwise untouched.

The other caller is `Duel_DrawCardAtOutputPosition`. That secondary draw
wrapper keeps its existing `DisplayObjectPosition *` first view and
`s32` second word-shaped callback API. It adds one explicit descriptor
conversion only while forwarding to the internal helper. No generic
secondary-callback or overlay ABI is retyped.

Both signed-halfword source coordinate pairs still promote to the same
full `s32` helper arguments. The internal helper's first parameter remains
the canonical display object, and its ten packet submissions now forward
the descriptor directly. There is no graphics, mode, constant, control,
scratch state, field width, cast-view, layout, global or profile change.

Supported-compiler controls accept the internal descriptor and reject the
old integer/entry-pointer/narrow-position contracts; the distinct word
callback contract remains accepted and its pointer variant rejected.
The native witness executes only the two actual call statements with an
observing card-render stub: 524,288 sequences per `-O0`/`-O2`, across
four descriptors, two real full/narrow object views and every coordinate
halfword pattern. It checks exact identity, both signed coordinate pairs,
two calls and all input/guard bytes. Negatives reject wrong table slot,
unsigned output X and wrong object view.

That witness does not execute the renderer, RTPS/GTE, dispatcher, SDK
runtime, GPU, list/update logic or engine. Full direct/projected caller
compiler and MASPSX identity and whole-executable matching remain the
acceptance gates; regional/overlay checks stay on the PR.
