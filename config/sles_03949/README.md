# German resident build

`make german-match` validates the German executable and image map, cleans the
split/build output, compiles the configured C units, links the entire
`SLES_039.49`, and requires SHA-256
`d9ba940664c6f2c908b3f10d9a8232b0ea830155150a3ff234a2ac12ea3f07cc`.
`make german-inventory` additionally regenerates `functions.csv` from the
matching manifest, exact linked C symbol extents, and generated fallback
assembly.

## Derived from the Spanish build

The German executable differs from the Spanish `SLES_039.51` in four 32-bit
words and nowhere else:

| Address | Spanish | German | What it is |
|---|---|---|---|
| `0x8001030C` | `03951` | `03949` | serial string in raw initialized data |
| `0x80012AA4` | `li v0,4` | `li v0,2` | `Main_Init` language index |
| `0x80030AC0` | `li v0,4` | `li v0,2` | debug language entry wrap value |
| `0x80043FDC` | `li a0,4` | `li a0,2` | boot sequence language request |

So this configuration is `config/sles_03951` with the executable, its region
hashes and the generated-symbol prefix changed:
- `symbols.txt`, `link_symbols.ld`, the split boundaries and every matching
  source are the Spanish ones.
- The three units carrying the language index are
  `src/game/german/` wrappers that set `BUILD_LANGUAGE_INDEX` to 2 over the
  European source, the same shape as the Spanish (4) and Italian (3) ones.
- Every other Spanish wrapper is reused as is, including
  `src/game/spanish/dialog_choice_cursor.c`, since its bytes are identical here.

Compiled against the Spanish wrappers, each German index unit differs in
exactly one instruction, the `li` above.

As in the Spanish build, all 1,140 eligible resident game C targets match.
The 61 handwritten and 623 Psy-Q/CRT functions retain their classifications.

`SU.MRG` is byte-identical to the Spanish one, and `WA_MRG.MRG` is not.
All six independently measured [runtime overlays](overlays/README.md)
nevertheless have the same payloads as Spanish. Their separate exact builds
cover all 124 inventoried C function instances / 55,852 bytes.
`make german-match-overlays` verifies the actual German archive hashes and
complete modules. The resident and archive-only overlay workflows use the
corresponding #6510 secrets.

The progress generator now validates both German resident and overlay
inventories. `make regional-progress-split` includes the German resident split;
README/global-usage snapshots remain separate #443 updates.
