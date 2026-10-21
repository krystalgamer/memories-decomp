# Model draw state and shadow fan

`gModel_bCurrentDrawMode` is the former `D_8009AFE4`.
`Model_DrawSlot` captures the slot's `+0xE11` byte before clearing that byte
unless it is four. Template tinting can publish mode three independently
of the slot byte. Units-only, shadow and continuation paths read the captured
mode; it is not a Boolean flag or a newly classified enum.

The initialized `u8` `.sdata` owner and the renderer's tentative COMMON
declaration remain separate. The renderer retains its exact
`gcc_2_8_1_g8_split_comm` profile, including MASPSX's COMMON handling.
No storage, qualifier, field layout or publication-order change is implied.

`gModel_wDrawTintRedGreen` and `gModel_bDrawTintBlue` are the former
`D_8009AFE6` and `D_8009AFE8`. The template stamp writes red and green
through byte views of the `u16` word and writes blue separately. The neutral
path publishes `0x80` per channel. The shadow fan reads those same values
for its centre and divided rim colours. Their initialized `.sdata` types,
byte ordering, unknown neighbours and byte-view accesses remain unchanged;
no RGB structure or wider allocation is introduced.

`gModel_aShadowFanSteps` is the former `D_800914E8`. Its unsized `u16`
declaration and sixteen initialized halfwords stay exact. The renderer still
reads eight signed x/z Q12 pairs through `ModelShadowFanStep`; storage is
not retyped to a complete array of that view.

## Native bindings

The NA owner labels and explicit Japanese provider bindings acquire the
semantic names. The eight obsolete Japanese/European renderer macros retire
so the C objects publish canonical references.

European link scripts expose the semantic names as aliases to their original
providers: `D_8009BF6C`, `D_8009BF6E`, `D_8009BF70` and `D_800925CC`.
The existing providers and their per-image bindings remain unchanged.
This preserves the original resolution instead of guessing a numeric UK
address or copying a continental address. No new object or storage follows
from a linker alias. Full applicable native image acceptance remains a
PR-side gate; no local UK retail verification is claimed.

All ordering-table identities and renderer views accepted before this
component are retained. The unrelated primitive pointer, remaining handler
bytes and diagnostics stay address-named.
