# Duel-effect state runtime

`DuelEffect_UpdateState` owns a five-slot frontend effect dispatcher shared by
duel card inspection, Build Deck, Trade, and later frontend scene states. Its
three bytes separate requested lifecycle state from the handler's private
phase flags:

| Address | Name | Contract |
| --- | --- | --- |
| `0x8009B248` | `gDuel_bEffectHandlerFlags` | Per-handler initialization and presentation phases |
| `0x8009B24A` | `gDuel_bActiveEffectState` | Stable callback-table index latched at activation |
| `0x8009B254` | `gDuel_bEffectState` | Requested state plus initialized/completed lifecycle bits |

Zero in `gDuel_bEffectState` is idle. On the first update of a nonzero request,
the dispatcher copies the low state value to `gDuel_bActiveEffectState`, adds
`DUEL_EFFECT_STATE_FLAG_INITIALIZED`, clears the handler flags, and returns
active without invoking a callback. Later updates dispatch the latched index
through `gDuelEffect_apfnStateHandler`. A handler adds
`DUEL_EFFECT_STATE_FLAG_COMPLETE`; the following update clears the request and
returns idle.

Slots 0 and 1 both run `DuelEffect_UpdateDialogState`. It creates the requested
text box once, waits for the text and optional choice list, then waits for
confirmation before completing the state. Slot 2 runs
`DuelEffect_UpdateCardViewerState`, the card-detail presentation used by duel
card selection, Build Deck, and Trade. Slots 3 and 4 intentionally point at
distinct empty handlers and are requested by adjacent frontend scene states.

`DuelEffect_MarkStateInitialized` owns bit `0x80` of
`gDuel_bEffectHandlerFlags`, independently of the dispatcher's initialized bit
in `gDuel_bEffectState`. Both dialog and card-viewer handlers use that helper
to guard their one-time setup. The remaining handler bits are presentation
specific and stay unnamed.

The card-viewer implementation remains a candidate because exact code
generation requires two pinned registers. Its address-based source filename
and the `D_8009B248` declaration are retained as lexical candidate contracts;
the linked function and generated assembly use the semantic runtime names.
