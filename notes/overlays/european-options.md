# English European PAL options module

The English European (SLES-03947) `WA_MRG.MRG` holds five identical
12,288-byte options images at sectors 10170, 10211, 10252, 10293 and 10334,
SHA-256 `ae3d3263f18be7a3661a191a22d9dc980019c149b01ea8752bbd4b19401a80ee`.
One module, `european_options`, is registered with the other four as
duplicate offsets, like the French, German, Italian and Spanish modules.

## Difference from the French image

The English image is not the French one
(`3083d6f5fcbd5d695e2466a4a52f9bdb5f1c54193334b9b3c89ff2507c8b4cd2`), but
the 251 differing bytes are almost all resident addresses:

- In the code (`0x0004-0x1040`), 75 instructions differ: 48 `jal` targets
  and 27 `%lo` offsets. The opcodes, registers, `lui` halves, function sizes
  and offsets are the same.
- In the raw module data (`0x1040-0x1D78` and `0x1F74-0x3000`), most
  differences are the same kind of resident references in leftover code
  (36 `jal` targets plus `%lo` offsets). The rest are a few initialized
  bytes: `0x104F` and `0x1F83-0x1F87`. These ranges stay raw data. The two
  suffix functions at `0x1D78` and `0x1F3C` are byte-identical.

There is no language-count, layout or behaviour difference, so nothing in
the shared C needs a region define. The module reuses the French layout,
symbols and all sixteen `src/overlays/pal_options` sources with their French
profiles. Only `options_linker_symbols.txt` changes: it binds the same 33
resident names to their SLES-03947 addresses from `config/sles_03947`.
Thirty-one of them differ from French; `gText_abColorSlots` and
`gDisplay_abResourceBank0` do not. All sixteen functions match on the first
build.

The English resident already calls `Options_Init` (`0x801686AC`) and
`Options_Update` (`0x80168E1C`) through explicit `config/sles_03947/symbols.txt`
entries. The other European overlay entry points, such as `FreeDuel_Entry`
and `NameEntry_Main`, are bound the same way, so those entries stay where
they are. The other PAL residents bind the same names in `link_symbols.ld`.
