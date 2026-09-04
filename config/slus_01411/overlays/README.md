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

Run `make match-overlays` to extract the configured modules, split them with
their module-specific Splat layouts, assemble and link every generated source,
and compare each rebuilt binary byte-for-byte with its verified archive slice.
