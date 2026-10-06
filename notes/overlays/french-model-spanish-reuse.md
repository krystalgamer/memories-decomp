# French MODEL images reusing accepted Spanish C

After the all-release registration (#7120), every French MODEL image has a
physical registration. **121 French layouts (124 physical images)** are
byte-identical to a Spanish MODEL image at the same load address that already
has accepted, verified C. They used to be unclassified raw-byte baselines; now each uses
the accepted Spanish layout, unchanged except for French names and paths.

- C sources, headers, and the named profile (`gcc_2_8_1_g0_split`) are unchanged.
  Nothing is region-conditional.
- Each French layout keeps its own physical identity: archive, sector, and any
  grouped `duplicate_sector_offsets` for identical payloads. When Spain registers
  identical images per model record, they share identical C manifests, and the
  donor sharing the French primary sector is preferred.
- Each family's linker-binding file is copied byte-for-byte. Every bound address
  is a French resident function start; some bindings keep Spanish alias names.
- Functions that Spain still leaves as assembly stay assembly here, and header
  and suffix bytes keep their raw-data owners. The replaced raw-byte layouts are
  removed.

The [ledger](french-model-spanish-reuse.csv) lists every French module, its
Spanish donor, the replaced raw-byte layout, sectors, hash, and C counts.

Verification rebuilt all 3,594 configured French overlay images; every one
matched exactly. `test_french_model_spanish_reuse` checks the ledger, manifest
and physical keys, and that the metadata equals the donor's. After a build it
also checks each image hash and that every C function is linked from the
selected C object.

French progress gains **241 C instances / 333,676 instruction bytes**. The
782 newly inventoried functions include the assembly functions that came with
the donor layouts. Coverage remains configured-image progress, not an
exhaustive executable-code census.
