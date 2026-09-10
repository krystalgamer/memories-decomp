# Overlay Build Configuration

Each runtime module has an independent Splat configuration because overlays
reuse load addresses and are not part of the resident executable link.

The Free Duel configuration expects the verified module image at
`tmp/overlays/free_duel/module.bin` and splits it into:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x0004` | `0x80168000-0x80168004` | Module identifier |
| `0x0004-0x1030` | `0x80168004-0x80169030` | MIPS text |
| `0x1030-0x2800` | `0x80169030-0x8016A800` | Module data |

The `0x1030` boundary is supported by the final return at `0x80169028`, the
known data symbol at `0x80169030`, and the verified runtime load range ending
at `0x8016A800`.

The password configuration separates its non-code prefix, executable body,
and trailing module data:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x00B4` | `0x80168000-0x801680B4` | Module tables and strings |
| `0x00B4-0x5400` | `0x801680B4-0x8016D400` | MIPS text |
| `0x5400-0x7800` | `0x8016D400-0x8016F800` | Module data |

The first function prologue begins at `0x801680B4`. The trailing boundary is
required by references to `D_8016D400` and contains the confirmed
`gPassword_abDigits` symbol at `0x8016D410`.

Both Egypt overworld variants share one layout shape. They are separate
modules because the resident loader picks the second package when campaign
story flag `0x47` is set, and their images differ:

| File range | Runtime range | Content |
|---:|---:|---|
| `0x0000-0x0004` | `0x80168000-0x80168004` | Module identifier |
| `0x0004-0x1E54` | `0x80168004-0x80169E54` | MIPS text |
| `0x1E54-0x3000` | `0x80169E54-0x8016B000` | Module data |

The `0x1E54` boundary is the word after the final `jr $ra` and its delay slot,
which is identical in both variants.

Run `make match-overlays` to extract the configured modules, split them with
their module-specific Splat layouts, assemble and link every generated source,
and compare each rebuilt binary byte-for-byte with its verified archive slice.

## C-owned data

Each `<module>_data_c.json` lists data-only translation units and their named
compiler profiles, using the same `schema: 1` / `units` shape as resident
`data_c.json`. A module with no data-only C units needs no data manifest.
For example, Free Duel maps its leading word with:

```yaml
      - [0x0, .data, overlays/free_duel/module_header]
```

```json
{
  "schema": 1,
  "units": [
    {
      "profile": "gcc_2_8_1_g0",
      "source": "src/overlays/free_duel/module_header.c"
    }
  ]
}
```

The dotted section type makes Splat link the C object's section instead of
disassembling an input blob. `.rodata`, `.data`, `.sdata`, `.sbss` and `.bss`
are supported. Keep the inline `[offset, type, path]` subsegment form used by
these layouts: the metadata-only wiring check reads this form without Splat
or a YAML dependency. A unit supplying several sections is compiled once.
Units already in `<module>_matching_c.json` get their profile there, including
any explicitly mapped data sections; do not repeat them in the data manifest.

Both the build and `make check-metadata` require bidirectional wiring:
every `c` subsegment needs a matching manifest entry, every data-only owned
section needs a data manifest entry, and neither manifest may name an unmapped
source. Duplicate data entries, conflicting grouped profiles, repeated
source/section pairs, missing files and unknown profiles are errors. Raw
`data`/`sdata`/`bss` blobs stay on the generated-assembly path.

Splat's linker section order still determines placement. Do not map the same
object section in separate segments: the first occurrence consumes it.
Keep ordinary data-only definitions in their own TU with a corresponding
header. For small data that needs `%gp_rel`, keep definitions in the owning
code TU and explicitly map its `.sdata`/`.sbss`. Always inspect the resulting
object sections and linker script and require a complete module match.

The first C-owned range is only the four-byte word at `0x80168000` in Free
Duel (`0x13`), password (`0x15`) and both overworld variants (`0x14`).
The variants compile the same `overworld/module_header.c` independently.
These retain the address-based `D_80168000` spelling: the values suggest a
module identifier but do not establish its semantics. There are no current
C consumers or linker assignments to retire for these words. Their owning
headers declare the exact four-byte object.

The main-menu prefix remains assembly-owned. `MainMenu_UpdateTradeScreen`
in `trade_update.c` declares `D_80180000[]` and reads element 1 as a comparator
block, reaching past the first word into `module_rodata`. Treating that
declaration as a four-byte object would assert a false boundary. Bulk module
data also remains assembly-owned pending evidence-backed object boundaries;
this initial mapping does not complete issue #2602.
