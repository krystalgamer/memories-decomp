# Model handler callback-state views

`Model_UpdateSlotControl` is the former `func_800559D4`.
The scene drives both duel slots and the Library drives slot zero. The
function invokes loaded primary/variant entries with their slot data,
advances the returned control state and selects the existing peer animation
reaction or fallback. Statistics comparisons, source lifetimes, random
calls, callback order and fallback behavior remain exact.

It stays in the native ten-function slot-control objects under
`gcc_2_8_1_g8_split`. NA/European size is `0x748`; Japan retains `0x6C0`
and its existing disabled trace macro. No function is extracted or reordered.

`Model_GetHandlerCallbackState` and `Model_SetHandlerCallbackState` are
the former `func_8005F18C` and `func_8005F198` (Japanese setter
`func_8004EBE8`). Both retain `0xC` extents and the same
`gcc_2_8_1_g8_split` profile in each 23-function state object. Their `s32`
return/argument and original byte load/narrowing are unchanged. The setter
still accepts arbitrary values; the observed one/zero bracketing is not
converted to Boolean validation or nested save/restore.

`gModel_bHandlerCallbackState` is the former `D_8009B07C`.
The slot-control dispatcher and load initializer publish one before invoking
loaded primary/variant control entries and zero afterward. The complete
resident direct-call census has eight calls to the existing setter, all
in those two functions; it does not claim exhaustive indirect/overlay use.
Queue reset also clears the byte.

Effect requests and key queues combine this state with a separate byte.
Those comparisons retain their exact order and equality tests; the other
byte remains unclassified. This name introduces no Boolean normalization,
nesting depth, save/restore policy or callback ownership.

The initialized `u8` remains in `.sdata`. Its three trailing padding bytes
remain separate one-byte objects: combining them into an array changes
alignment and section extent. Existing GP-relative and wider snapshot views
retain their original types and bounds.

`gModel_Slot0MetadataView` is the former `D_800F3938`, an interior alias at
`gModel_aSlots + 0xCF8`. Its existing `ModelSlotCF8TailView` reaches the
loaded metadata and tail flags through `+0xDFE/+0xDFF`. Slot control and
preset-yaw selection retain the separate label and original slot stride.
The name neither allocates storage nor identifies individual values/flags;
it does not replace the view with a member expression or a new type.

All seven images retain the original native alias addresses and the same
`+0xCF8` relation. Six obsolete Japanese/European data macros retire once
the canonical binding names are wired. The Japanese setter's provider and
slot-control caller spelling bridges retire together with the API migration.
Existing controls change identifiers only; the complete numeric Lua runtime
and trace control remain unchanged. No new harness or field type is added.
