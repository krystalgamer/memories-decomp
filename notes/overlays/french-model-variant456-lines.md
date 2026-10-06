# French MODEL456 two-group lines

The six French header-456/606 images reuse the unchanged accepted
[Spanish two-group line helper](spanish-model-variant456-lines.md).
This registers six matching C instances at `+0x315C..+0x34D4`: 888 bytes each,
5,328 instruction bytes in total. Both slots use the existing Spanish source
and its `gcc_2_8_1_g0_split` profile; no new C body or regional macro is needed.

The [instance ledger](french-model-variant456-lines-instances.csv) records the
six physical images: model 163 at stages 7/8 and models 460/536 at stages 9/10.
The French archive and complete image hashes are independently checked, not
inferred from Spanish archive identity. The
[attempt ledger](french-model-variant456-lines-attempts.csv) records the six
exact reuse results and unchanged source/dependency fingerprints.

All seven function boundaries in each image are supported by the identical
complete payload and independently checked closed control flow. Only the
selected helper becomes C. Thirty-six other function instances remain
generated assembly; the header and suffix retain real raw-data owners.
The helper remains an orphan with no demonstrated direct caller or stored
function pointer. This is not proof of dead code or complete overlay recovery.

The regional tests share the accepted target-layout, original-context,
descriptor, loader, selected-object, relocation, and resident-owner checks.
They require independent French complete-image equality and the actual
section-defined four-byte loader pointers, rather than absolute aliases or
candidate location alone. The shared Spanish implementation and its recovery
history remain unchanged.
