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
| Name entry | WA package `7903-7983`; phase `7968-7983` copies `0x7800` bytes | `0x80168000-0x8016F800` | [`name_entry.txt`](../research/Unchiga_Symbols/modules/name_entry.txt) | High |

The WA package ranges include non-code phases copied to other destinations;
the narrower loaded-code ranges above are not complete inner-file manifests.
Because several modules reuse `0x80168xxx`, their symbols remain module-scoped
and must not be added as unconditional resident executable symbols.

The name-entry package is recovered from the resident request
`File_RequestAsyncTransfer(0, 0, 0x1EDF, 0x50, func_8003BA14, 0, 0)` in
`func_8003BBF8`, which `Main_RunNameEntry` calls. Its `0x80168000` phase is
byte-identical to the password phase at `8054-8069` apart from the final
sector, so name entry and the password screen share one image and are
separate entry points rather than separate modules. See
[`../../src/overlays/name_entry/README.md`](../../src/overlays/name_entry/README.md).

## Resident load requests

Every screen package is requested through the same resident helper, whose
third and fourth arguments are the first WA sector and the sector count:

```text
File_RequestAsyncTransfer(
    0, 0, <first sector>, <sector count>, <screen callback>, 0, 0
)
```

| Screen | Requesting function | Arguments | WA package |
|---|---|---|---|
| Free Duel | `Main_InitFreeDuelMenu` | `0x1E88`, `0x57` | `7816-7903` |
| Name entry | `func_8003BBF8` | `0x1EDF`, `0x50` | `7903-7983` |
| Password | `func_8003BEB8` | `0x1F2F`, `0x56` | `7983-8069` |
| Egypt overworld | `func_8003C0C0` | `0x1FD9`, `0x9E`; first sector `+0x9E` when story flag `0x47` is set | `8153-8311`, `8311-8469` |

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
