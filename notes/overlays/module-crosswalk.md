# Runtime Module Crosswalk

This note joins the project-owned archive/load trace with the external
per-module symbol evidence under `notes/research/Unchiga_Symbols/modules/`.
It records only mappings supported by both the loaded address range and module
behavior.

| Module | Archive evidence | Loaded code range | External symbols | Confidence |
|---|---|---|---|---|
| Free Duel | WA package `7816-7903`; phase `7898-7903` copies `0x2800` bytes | `0x80168000-0x8016A800` | [`free_duel.txt`](../research/Unchiga_Symbols/modules/free_duel.txt) | High |
| Password/password shop | WA package `7983-8069`; phase `8054-8069` copies `0x7800` bytes | `0x80168000-0x8016F800` | [`password.txt`](../research/Unchiga_Symbols/modules/password.txt) | High |
| Egypt overworld, before/after the coup | WA packages `8153-8311` and `8311-8469`; phases `8153-8159` and `8311-8317` copy `0x3000` bytes | `0x80168000-0x8016B000` | [`overworld.txt`](../research/Unchiga_Symbols/modules/overworld.txt) | High |
| Main menu | SU phase `98-114` copies `0x8000` bytes and contains the observed `0x80180xxx` call targets | `0x80180000-0x80188000` | [`main_menu.txt`](../research/Unchiga_Symbols/modules/main_menu.txt) | High |
| Name entry | Archive package not yet recovered from the resident loader trace | Verified WA screen bank `0x80168000-0x8017A000`; known symbols begin at `0x8016868C` | [`name_entry.txt`](../research/Unchiga_Symbols/modules/name_entry.txt) | Medium |

The WA package ranges include non-code phases copied to other destinations;
the narrower loaded-code ranges above are not complete inner-file manifests.
Because several modules reuse `0x80168xxx`, their symbols remain module-scoped
and must not be added as unconditional resident executable symbols.

The name-entry row deliberately leaves its archive range unknown. A future
mapping should identify the resident load request and verify its payload
against the disc before assigning sectors.

## Identified packages not ready for source scopes

Some WA packages have a screen or subsystem attribution but still lack the
code-phase boundaries needed for `src/overlays/<module>/`:

| Attribution | WA package | Confirmed evidence | Missing evidence |
|---|---|---|---|
| Library | sectors `7629-7767` | `Main_RunLibraryMenu` calls resident loader `func_8002BFCC`, which requests this 138-sector package | Per-phase destinations, exact executable slice, and module-scoped symbols |
| Campaign scene loader | sectors `7767-7816` | `func_8002FD10` requests the package; its `0x1000` phase at `0x801A8000` is the campaign event script | Any executable phase attributable to a runtime code module |

These attributions belong in the loader map, but they do not yet justify
`library/` or campaign source directories. In particular, the campaign event
script is runtime data interpreted by resident code, not an overlay merely
because it is loaded from WA.
