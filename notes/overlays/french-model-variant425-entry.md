# French MODEL425 loader entries

MODEL0 stage 9 / slot 0 and stage 10 / slot 1 now use exact C for their
3,808-byte entries at `0x8013B004` / `0x8017B004`. Both complete
20,480-byte images reproduce retail with the existing
`gcc_2_8_1_g0_split` profile (GCC 2.8.1 and MASPSX 2.81).
This adds two C instances and 7,616 bytes. All five existing C helper
registrations and original-sized owners are unchanged. The separate
2,360-byte ribbon helper at image `+0x1DA4` remains generated assembly.

## Evidence and layout

All 952 native entry words, including zero instructions, were inspected.
The newly recovered French MODEL423 entry supplies structural evidence,
not an unverified declaration or compiler substitution. Accepted local
MODEL425 mesh and two-point-streamer records, the common sheet, and the
accepted `Funnel423` record supply the corresponding measured layouts.
The similarly numbered North American MODEL425 entry is a different effect.
No external reference types or flags are imported.

| Work offset | Observed storage |
|---|---|
| `0x0000` | Nine rows of seventeen mesh points and nine color rows |
| `0x04EC` | One `0x360`-byte ribbon record |
| `0x084C` | Twelve `0x7C`-byte spiral records |
| `0x0E1C` | Two `0x98`-byte sheets |
| `0x0F4C` | One `0x64`-byte two-point streamer |
| `0x0FB0` | Five initialized `0x118`-byte funnel records |
| `0x1528` / `0x155C` | Mesh / funnel GT4 |
| `0x15C4` | Additional initialized GT4 |
| `0x162C` / `0x1648` | G3 / G4 |
| `0x166C` / `0x16D4` | Two GT4s / two FT4s |
| `0x1734` | Matrix, including origin at `0x1748` |
| `0x1754` / `0x175C` | Target / direction |
| `0x176C` / `0x1770` | Screen delta / view direction |
| `0x179C` / `0x17A4` | Configuration / selected part pointers |
| `0x17D0` / `0x17D4` | Existing progress / distinct target-travel progress |
| `0x17F0` / `0x17F4` | Inner / outer colors |
| `0x180C` / `0x1820` | Phase / ambient intensity |
| `0x1824` / `0x1826` | Signed slot / command |

This establishes a minimum accessed context extent of `0x1828`, not
allocation capacity. Untouched packet intervals, vector pads and state gaps
remain opaque.

The command selects a `0x30`-byte configuration at raw suffix `+0xFC`
(`0x8013EA04` in slot zero). Its part index is byte `+0x10`, unsigned
sheet start is word `+0x18`, and the target-travel interval is `+0x20/+0x24`.
The three mode-one image uploads use suffix `+0xC4`, `+0x8C` and `+0x1C`.
Both explicit page/CLUT query results remain distinct.

Mesh latitude uses `(base << 2) / 8` with `base += 325` in the body,
seventeen longitudes spaced by 256, and radius/height multiplier 512.
Red and green advance by 20 per row; blue is 192.
The first spiral color pair is outer `(0,0,128)`, inner `(128,128,192)`;
the second pair is zero. Sheet geometry and ribbon initialization retain
their independently observed stores.

Five funnel records are initialized through `0x1528`, while the accepted
[funnel renderer](french-model-variant425-funnel.md) still draws four.
Inner points use logical sine/cosine shifts by four. Outer x/z use
`(u32)(trig * 9) >> 7`, with y -512. Sizes zero through three receive
`-(index * 8192 / 3)`; record four explicitly receives zero.
The single streamer has two initialized gray color rows and a zero word
at record `+0x50`.

## Runtime and target travel

The entry derives camera-facing angles, copies the opposite slot's target
halfwords, and clears target y. Slot zero offsets target z by +1400 and
subtracts signed `travel * 1400 / 1024`; the other slot offsets by -1400
and adds that quotient. The offset is a real signed-halfword temporary,
matching the stored target-coordinate width. Widening it changes the
native load and arithmetic scheduling.

After updating animation-driven elapsed time and the separate frame step,
unsigned elapsed at least configuration `+0x20` enables the travel update.
While signed travel is below 1024, it becomes
`((elapsed - start) << 10) / (end - start)` with unsigned division, then
clamps to 1024. This does not add a duration-zero guard or alter the
original division behavior.

Matrix refresh, unsigned projected x and signed projected y, unsigned
sheet timing, and phase-guarded funnel/mesh/ribbon/spiral calls are retained.
The two-point streamer is initialized but has no direct entry call;
this is not a global-unreachability claim.

Common ambient submission uses slot two. Ambient decreases by 128 and
clamps at zero below phase seven; later it uses `fade << 5`.
Phases three/four return four, phase six returns one and enters seven,
phase seven grows fade to 64 and enters eight, and phase eight returns two.

## Experiments and acceptance

The [ledger](french-model-variant425-entry-attempts.csv) records all four
immutable source/header experiments and two production-image terminals.
The first recovery was 3,816 bytes. Giving mesh longitude its own real
counter restored native initialization register ownership. Staging the
target offset through the field retained two unwanted intermediate stores;
a widened local removed those stores but changed 28 words. The actual
signed-halfword local recovered all 3,808 bytes and the 248-byte frame.
No fabricated accesses, volatile tricks, inline assembly, forced registers
or compiler-profile changes are used.

| Complete image | SHA-256 |
|---|---|
| MODEL0 stage 9, slot 0 | `308922ff8d8bd0358f1c6ad08d546d761baf11db70814d3ef881388aadc23609` |
| MODEL0 stage 10, slot 1 | `6773d536ed9f6c62822fb0fe66aa8930947c91cac4c1416b945e10ce1754fa6a` |

Both production ELFs retain all six C functions at their original addresses,
sections and sizes. The unmatched ribbon remains the assembly fallback.
No existing helper body, raw suffix, archive slice, physical registration,
resident implementation or generated report changes.
