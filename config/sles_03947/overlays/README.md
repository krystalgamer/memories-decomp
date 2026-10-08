# European Overlay Build Configuration

European runtime modules are independent from the resident `SLES_039.47`
executable and from the North American and Japanese overlay manifests. Run
`make european-match-overlays` to extract every configured European archive
slice, rebuild it from its module-specific layout, and require an exact binary
match.

The Free Duel module is the five-sector `WA_MRG.MRG` slice beginning at sector
`9304`. Its verified layout is:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x0004` | `0x80168000-0x80168004` | Module identifier |
| `0x0004-0x0090` | `0x80168004-0x80168090` | Matching C |
| `0x0090-0x01BC` | `0x80168090-0x801681BC` | Matching C |
| `0x01BC-0x0A0C` | `0x801681BC-0x80168A0C` | Matching C |
| `0x0A0C-0x10A0` | `0x80168A0C-0x801690A0` | Matching C |
| `0x10A0-0x2800` | `0x801690A0-0x8016A800` | Module data |

All nine matching functions reuse `src/overlays/free_duel/screen_runtime.c`
through narrow European wrappers. `FreeDuel_PlaceCursor` selects the European
flagged results text box without changing the North American or Japanese body.

The European main-menu executable is the sixteen-sector `SU.MRG` slice
beginning at sector `98`. The same payload repeats at sectors `234`, `370`,
`506`, and `642` for the other language packages. Its verified layout is:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x001C` | `0x80180000-0x8018001C` | Module identifier and rodata |
| `0x001C-0x0B4C` | `0x8018001C-0x80180B4C` | Two matching C functions |
| `0x0B4C-0x0DA4` | `0x80180B4C-0x80180DA4` | Matching C |
| `0x0DA4-0x1050` | `0x80180DA4-0x80181050` | Four matching C functions |
| `0x1050-0x1408` | `0x80181050-0x80181408` | Matching C |
| `0x1408-0x187C` | `0x80181408-0x8018187C` | Matching C |
| `0x187C-0x1E10` | `0x8018187C-0x80181E10` | Matching C |
| `0x1E10-0x20C0` | `0x80181E10-0x801820C0` | Four matching C functions |
| `0x20C0-0x2408` | `0x801820C0-0x80182408` | Matching C |
| `0x2408-0x35B8` | `0x80182408-0x801835B8` | Matching C |
| `0x35B8-0x4784` | `0x801835B8-0x80184784` | Fifteen matching C functions |
| `0x4784-0x8000` | `0x80184784-0x80188000` | Module state and remaining data |

All thirty-one inventoried functions reuse the shared main-menu sources
through European wrappers.

The six-sector pre-coup and post-coup overworld modules begin at `WA_MRG.MRG`
sectors `9762` and `9920`, respectively. Their executable ranges and initial
state are identical; the alternate location tables and remaining module data
differ. Both use this verified layout:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x0004` | `0x80168000-0x80168004` | Module identifier |
| `0x0004-0x11A8` | `0x80168004-0x801691A8` | Thirteen matching C functions |
| `0x11A8-0x1618` | `0x801691A8-0x80169618` | Location table and live state |
| `0x1618-0x17D0` | `0x80169618-0x801697D0` | Preserved generated assembly |
| `0x17D0-0x1E54` | `0x801697D0-0x80169E54` | Two matching C functions |
| `0x1E54-0x3000` | `0x80169E54-0x8016B000` | Alternate table and module data |

All fifteen inventoried functions in each variant reuse the shared overworld
sources. European wrappers isolate each function so the relocated resident and
module symbols do not change the North American or Japanese objects.

The password/name-entry runtime occurs as two 15-sector `WA_MRG.MRG` slices,
beginning at sectors `9374` and `9460`. Their executable and shared data bytes
are identical through module offset `0x7060`; only the final `0x7A0`-byte raw
tail differs. Both verified layouts use:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x0004` | `0x80168000-0x80168004` | Leading module word |
| `0x0004-0x0018` | `0x80168004-0x80168018` | Matching C jump table |
| `0x0018-0x2850` | `0x80168018-0x8016A850` | Matching C |
| `0x2850-0x2904` | `0x8016A850-0x8016A904` | Matching C |
| `0x2904-0x7060` | `0x8016A904-0x8016F060` | Remaining generated module data |
| `0x7060-0x7800` | `0x8016F060-0x8016F800` | Variant-specific raw tail |

All 27 inventoried game-owned functions in each variant reuse the shared password
sources through selective European wrappers. The European text-entry records
use the resident 22-byte `EuropeanDuelEffectEntry` layout. Narrow regional
constants retain the European glyph width and screen bounds, message-box flag,
preview position, and text-box completion mask without changing the existing
North American or Japanese builds.

## Reviewed MODEL450 maintenance

The existing raw-image registrations at `MODEL.MRG` sectors `48224` and `48234`
select four previously reviewed shared helpers per slot: ribbons, bands, quads
and lines. These selections already match in French. Refreshing these physical
owners supersedes the stale canonical-module additions in #6999 without
duplicating either image or changing the 3,581-module registry.

The entry and two intervening helpers remain generated assembly, and the
`0x3940..0x5000` suffix remains raw. Sources, compiler profiles, image hashes
and regional call bindings are unchanged. See
[`european-model-variant450-takeover.md`](../../../notes/overlays/european-model-variant450-takeover.md)
for the independent complete-image and sized-C-owner evidence.
