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
