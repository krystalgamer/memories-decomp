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
| `0x0090-0x01BC` | `0x80168090-0x801681BC` | Region-divergent assembly |
| `0x01BC-0x0254` | `0x801681BC-0x80168254` | Matching C |
| `0x0254-0x0A0C` | `0x80168254-0x80168A0C` | Region-divergent assembly |
| `0x0A0C-0x10A0` | `0x80168A0C-0x801690A0` | Matching C |
| `0x10A0-0x2800` | `0x801690A0-0x8016A800` | Module data |

The seven matching functions reuse `src/overlays/free_duel/screen_runtime.c`
through narrow European wrappers. `FreeDuel_PlaceCursor` and `FreeDuel_Init`
remain generated assembly because their European bodies are not exact matches.

The European main-menu executable is the sixteen-sector `SU.MRG` slice
beginning at sector `98`. The same payload repeats at sectors `234`, `370`,
`506`, and `642` for the other language packages. Its verified layout is:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x001C` | `0x80180000-0x8018001C` | Module identifier and rodata |
| `0x001C-0x0B4C` | `0x8018001C-0x80180B4C` | Two matching C functions |
| `0x0B4C-0x0DA4` | `0x80180B4C-0x80180DA4` | Region-divergent assembly |
| `0x0DA4-0x1050` | `0x80180DA4-0x80181050` | Four matching C functions |
| `0x1050-0x1408` | `0x80181050-0x80181408` | Region-divergent assembly |
| `0x1408-0x187C` | `0x80181408-0x8018187C` | Matching C |
| `0x187C-0x1E10` | `0x8018187C-0x80181E10` | Region-divergent assembly |
| `0x1E10-0x20C0` | `0x80181E10-0x801820C0` | Four matching C functions |
| `0x20C0-0x2408` | `0x801820C0-0x80182408` | Region-divergent assembly |
| `0x2408-0x35B8` | `0x80182408-0x801835B8` | Matching C |
| `0x35B8-0x3740` | `0x801835B8-0x80183740` | Region-divergent assembly |
| `0x3740-0x4784` | `0x80183740-0x80184784` | Fourteen matching C functions |
| `0x4784-0x8000` | `0x80184784-0x80188000` | Module state and remaining data |

The twenty-six matching functions reuse the shared main-menu sources through
European wrappers. The five larger region-divergent bodies remain generated
assembly.

The six-sector pre-coup overworld module begins at `WA_MRG.MRG` sector `9762`.
Its verified layout is:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x0004` | `0x80168000-0x80168004` | Module identifier |
| `0x0004-0x11A8` | `0x80168004-0x801691A8` | Thirteen matching C functions |
| `0x11A8-0x1618` | `0x801691A8-0x80169618` | Location table and live state |
| `0x1618-0x17D0` | `0x80169618-0x801697D0` | Preserved generated assembly |
| `0x17D0-0x1E54` | `0x801697D0-0x80169E54` | Two matching C functions |
| `0x1E54-0x3000` | `0x80169E54-0x8016B000` | Alternate table and module data |

All fifteen inventoried functions reuse the shared overworld sources. European
wrappers isolate each function so the relocated resident and module symbols do
not change the North American or Japanese objects.
