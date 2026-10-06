# Spanish MODEL417 phased lines

The helper at `+0x1204..+0x1778` is independently reconstructed matching C:
1,396 bytes in each of eight physical MODEL417/567 loads, or 11,168 C
instruction bytes representing one unique routine. This is not a port of
accepted French or other regional C. The initial screen checked 6,111 accepted
regional C entries and 36 same-sized bodies without finding a normalized match.
The post-integration screen checks 6,121 accepted entries and 46 same-sized
bodies, again with no normalized match, excluding this recovery's own entries.

## Physical loads and retained ownership

MODEL134 and MODEL232 use stages 9/10, MODEL354 stages 7/8, and MODEL535 stages
9/10. The instances ledger records all eight distinct image hashes, physical
records, sectors, commands, and selected descriptor hashes. The exhaustive
loader scan, not just the hash-deduplicated census, establishes this set.
Commands select 68-byte descriptors at `+0x449C + (command % 1000) * 68`.

Each complete 20KiB image has eight closed, contiguous functions beginning at
`+4`, `+0x1204`, `+0x1778`, `+0x1E60`, `+0x2480`, `+0x2ACC`,
`+0x30E4`, and `+0x3750`. Code ends at `+0x43A0`; the remainder stays raw data.
The entry calls only `+0x1204`, `+0x1778`, `+0x2ACC`, and `+0x3750`.
The three other helpers are independently closed stack-prologue functions,
not entry-reachable functions. All seven nonselected functions per image stay
generated ASM. The selected source owns only its single 1,396-byte function.

## Context and behavior

The original entry captures its context in `s2` at `+0xC` and passes that
unchanged value at `+0x103C`. A signed comparison between context `+0x2128`
and descriptor `+0x1C` gates this call and the companion helper `+0x2ACC`.
The private state structure is a view through the highest field read here,
not a claim about the complete caller allocation.

Three groups at `+0x54C..+0xA2C` have stride `0x1A0`. Each contains two
endpoint sets, four rows, and six `SVECTOR` points per row. Entry initialization
writes both endpoint sets, RGB at group `+0x180`, and initial size
`4096 + i * 4096 / 3` at `+0x194`. The helper reads only the first companion's
signed size at context `+0xA98 + 0x88`. Entry initialization shows two companion
records of stride `0x98`, so the source deliberately uses a local size-only
view rather than guessing a shared record type. That pointer never advances.

Four `ratan2` calls remain even though their return values are unused.
Phase zero shrinks groups by `step * 192`, wrapping by 4096 unless the first
companion exceeds 2048. Oversized groups are also zeroed past that threshold.
Phase one draws black at scale 4096 and assigns staggered sizes
`-(i * 8192 / 3)`. Later phases grow by `step * 256`, wrap below phase five,
and clamp to 8192 from phase five onward.

The local completion flag starts at one and is cleared **only on wrapping**.
It is not an all-groups-finished reduction. The final group crossing its
threshold can change phase five to six when this flag remains one.
Preserve that control flow rather than substituting a generic completion test.

Phase-zero color fades above size 2048; later color fades above 6144.
Zero rotation and uniform scale translate by the signed 32-bit origin before
phase two and by the signed 16-bit target afterward. For each of 72 lines,
the `GsGLINE` at `+0x20BC..+0x20D0` receives attribute `0x50000000`.
`RotTransPers4` receives each endpoint and XY output twice. Before phase two
the first endpoint is black; afterward the second is black. Both nonnegative
depth and nonnegative flag are required for sorting, with depth narrowed to
16 bits.

## Reproduction evidence

The initial candidate failed compilation because it used `GsGLINE.tag`
instead of the authoritative SDK member `attribute`. No instruction comparison
is claimed for that attempt. Correcting that spelling produced an exact body.
Narrowing the companion view and independently compiling the other load slot
both remained exact. The twelve-row attempt ledger retains the failure, three
successful probes, and eight terminal full-image records.

The authoritative `gcc_2_8_1_g0_split` profile uses GCC 2.8.1 and MASPSX 2.81.
The natural 296-byte frame needs no forced registers, assembly, extra alignment,
unused allocation controls, or SDK type changes.

Regressions verify 33 target layouts, repeated header inclusion, physical
loads and descriptors, all function CFGs, original-context reaching definitions,
initialization and packet lifetimes, and the wrap-only completion flag.
They check all eight input/final function owners and both data owners per image,
every selected call/local-jump relocation, 37 resident bindings, and resident
loader/context ownership. Missing optional retail inputs are skipped explicitly.
