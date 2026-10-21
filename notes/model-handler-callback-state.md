# Model handler callback-state views

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
the canonical binding names are wired. Function spelling bridges remain
until their own API migration; provider and caller must migrate together.
