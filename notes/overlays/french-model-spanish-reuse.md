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

Either release may decompile a shared function first, so a French copy may lag
behind newer Spanish C or be ahead of it. Where both have C at the same
address, the entries must be identical (size, profile, and source). Every
French binding must be at a French resident function start.

Verification rebuilt all 3,594 configured French overlay images; every one
matched exactly. `test_french_model_spanish_reuse` checks the ledger, manifest
and physical keys, and that shared C entries agree with the donor's. After a build it
also checks each image hash and that every C function is linked from the
selected C object.

French progress gains **253 C instances / 357,292 instruction bytes**. The
782 newly inventoried functions include the assembly functions that came with
the donor layouts. Coverage remains configured-image progress, not an
exhaustive executable-code census.

## Catch-up with later Spanish MODEL variant C

French and Spanish `MODEL.MRG` have the same SHA-256, so every Spanish MODEL
variant function matched after the registration above has identical French
bytes. A later pass brought **172 C instances / 386,024 instruction bytes**
into 123 existing French variant layouts. For each one, the Spanish layout
already had accepted C at the same address and size, while the French layout
kept assembly.

- Each French segment now uses the same `src/overlays/spanish_model_variant/`
  source and named profile as Spain. The `matching_c.json` entry and the
  function-inventory status and notes are copied from the Spanish donor.
- French-only C, such as the MODEL459 rays and lines, stays unchanged.
- Each touched family linker file gains only the missing Spanish bindings,
  such as `RotTransPers3` and `GsGetActiveBuff`. No binding changes value, and
  every bound address is a French resident function start.
- Three Spanish per-record images (MODEL238 stage 8 and MODEL269 stages 9/10)
  are not registered again. Their sectors are already
  `duplicate_sector_offsets` of the French MODEL47 and MODEL43 layouts, which
  carry all of their C and more.

All 3,594 French overlay images were rebuilt and matched exactly. Each ported
function is a linked `STT_FUNC` symbol at its inventory address and size.
