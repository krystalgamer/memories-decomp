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

As in the Spanish build, all 1,140 game functions are C.

`SU.MRG` is byte-identical to the Spanish one, and `WA_MRG.MRG` is not, so
runtime overlays are left for separate measurement. The German workflow checks
the inputs and the resident match with the #6510 secrets.
