# Spanish MODEL headers 402/552

Models 6 and 551 load this family in stages 7/8 from compact records 6 and
501. Four independently verified ten-sector images load at `0x8013B000`
and `0x8017B000`. The [instance ledger](spanish-model-variant402-instances.csv)
records their sectors, headers, hashes and normal commands.

The Spanish manifests reuse the accepted French rings and bands sources,
their local headers and slot-one wrappers without changes. Each helper and
slot compiles independently under the named GCC 2.8.1 / MASPSX 2.81
`gcc_2_8_1_g0_split` profile; all four complete Spanish images match.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0xA5C` | 2648 | Unmatched assembly | Yes |
| `0xA5C..0x1048` | 1516 | Unmatched assembly | No |
| `0x1048..0x169C` | 1620 | Unmatched assembly | No |
| `0x169C..0x1B38` | 1180 | Rings C | No |
| `0x1B38..0x2060` | 1320 | Bands C | Yes |

Entry calls only `0x1B38`; the other retained functions were checked from
their own boundaries, not described as entry-call reachable. Every instruction
in each span is covered by direct control flow with one terminal return and
no unresolved indirect jump. This does not prove all possible runtime entries.

Eight C instances contribute **10,000 instruction bytes**. Twelve other
function instances / 23,136 bytes remain explicitly unmatched assembly.
Each four-byte header and **12,192-byte unclassified suffix** at
`0x2060..0x5000` retains real storage. No unknown bytes are classified away.

## Independent layout and ownership evidence

Thirty-two target-compiled constants and 51 instruction anchors per Spanish
image verify the local and SDK views. Rings start at context `0x58`, use
two **144-byte records**, and have scale at record offset 128. They are not
the 152-byte rings used by family 337. The two bands at `0x438` use
**280-byte records**, with two rows of seventeen canonical `SVECTOR`s,
scale at 272 and cycle count at 276.

Normal metadata requests 568000 pass initialization argument zero. The
20-byte configuration view at image `0x215C` has part count one and unsigned
duration 92. Each view remains inside its own suffix owner; this is not a
generic descriptor-array capacity claim.

The rings use nested positive-depth and depth-below-2048 checks, rather than
a combined range expression; exact code generation depends on that control
flow. Their projection flag is not tested. The bands preserve their observed
curve, color, scale and cycle expressions. Existing bodies and declarations
are shared rather than forked.

All 35 resident bindings have real selected input objects, sized final
function symbols and retail-identical bytes in a clean Spanish resident.
Three matching initializer/controller/loader owners and the actual context
pointer words at `0x80010024/28` are also verified. These point to
`0x80136000/0x80176000`; the rings' `0x8B0` accessed view does not overlap
the selected model, primary or secondary loads. Entry uses additional fields.
Neither this partial view nor load separation establishes allocated capacity
or whole-game lifetime isolation.

The [terminal attempt ledger](spanish-model-variant402-attempts.csv) records
the source hash for each independently compiled helper/slot. Earlier source
recovery remains in the [French ledger](french-model-variant402-attempts.csv).
Full-image hashes, actual compiler/assembly/data ownership and these partial
inventories remain distinct from exhaustive Spanish or seven-release coverage.
