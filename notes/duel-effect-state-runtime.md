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

Slots 0 and 1 both contain `DuelEffect_UpdateDialogState`, although zero is
the dispatcher's idle value and therefore never reaches slot 0 through this
entry point. The dialog handler creates the requested text box once, waits for
the text and optional choice list, then waits for confirmation before
completing state 1. Slot 2 runs
`DuelEffect_UpdateCardViewerState`, the card-detail presentation used by duel
card selection, Build Deck, and Trade. Slots 3 and 4 intentionally point at
distinct empty handlers and are requested by adjacent frontend scene states.

The dialog and card-viewer handlers consume a second payload family:

| Address | Name | Contract |
| --- | --- | --- |
| `0x8009B240` | `gDuel_pCardViewerBackground` | Sliding background display object |
| `0x8009B244` | `gDuel_wEffectDialogTextID` | Text resource consumed by the dialog handler |
| `0x8009B24B` | `gDuel_bCardViewerYOffset` | Shared vertical offset published with a viewer request |
| `0x8009B24C` | `gDuel_pCardViewerCard` | Card portrait display object |
| `0x8009B250` | `gDuel_pCardViewerTextBox` | Optional card-stat text box |

Build Deck and Trade publish a Y offset of `20`; the card viewer applies it to
both the portrait and the text/background layout. The viewer creates, slides,
and destroys the three pointer-owned objects as one presentation. The dialog
handler passes `gDuel_wEffectDialogTextID` directly to `TextBox_Create`. No
resident writer is currently recovered, so the producer remains unspecified.

`DuelEffect_MarkStateInitialized` owns bit `0x80` of
`gDuel_bEffectHandlerFlags`, independently of the dispatcher's initialized bit
in `gDuel_bEffectState`. Both dialog and card-viewer handlers use that helper
to guard their one-time setup. The remaining handler bits are presentation
specific and stay unnamed.

The card-viewer implementation remains a candidate because exact code
generation requires two pinned registers. Its address-based source filename
and the `D_8009B240`, `D_8009B248`, `D_8009B24C`, and `D_8009B250`
declarations are retained as lexical candidate contracts; the linked function
and generated assembly use the semantic runtime names.
