# Shared card preview resources

`gCardPreview_Resources`, formerly `D_800EA0E8`, names the existing unsized
`DuelEffectResourceRecord` array. This changes no type, element layout,
bound, allocation or address.

The request/callback pair selects a card and its resource slot, then
uploads the record's four rectangles. The constructor publishes the linked
primary/secondary preview objects and card/display metadata. The detail
panel consumes that same record; clear/release helpers retain their exact
pointer values and lifecycle order. Battle and placement update display
stats and read back preview regions. The viewer, Library and Password
initialize/use these same preview resources.

The initializer explicitly prepares slots 0 and 1; the viewer also uses
slot 3. These observed indices are not an inferred array extent. The
declaration stays `[]`. `gCardPreview_Resource1` remains its existing slot-one/prefix
alias at one `0x40` stride; no alias, prefix size or independently owned
Library state is widened or reinterpreted.

| Build | Actual preview resource address |
| --- | --- |
| North America | `0x800EA0E8` |
| Japan | `0x800E9FC8` |
| European English | `0x800EB050` |
| Other European languages | `0x800EB470` |

Every main binding and Password resident import uses that build's actual
address. Existing legacy linker/import aliases remain. The misleading
French suffix alias `D_800EA0E8_French` at the North American address remains
unchanged and is not selected as the French resource store.

All existing source and fixture changes are identifier-only, including
Password's first-record pointer publication. Library's compiler-shaped
integer induction/cast-backs and every intentionally narrow/raw object view
are untouched. The unsized owner, same-type element, incorrect-object
pointer rejection and forbidden whole-array `sizeof` are checked with the
supported compiler.

Tiny MIPS data-relocation probes link against each actual Password import
script and require the exact emitted little-endian address word. They also
check the absolute symbol/type under the accepted 32-bit/sign-extended
`nm` forms. Removing only the named import must fail even while the old
alias remains. Every Password A/B manifest must select the checked script.
These probes neither build nor execute cloud overlays.

Existing upload, requester, object-lifecycle, stat-meter and type/ABI
witnesses retain their original numerical/event/mutation oracles. They do
not execute the complete renderer, whose compiler-shaped constructs must
not be modernized or native-run, nor emulate GPU, real I/O or engine
behavior. Critical constructor, callback, renderer and Library assembly
remains identical after the sole global-name normalization; complete
retail executable matching remains the acceptance gate.
