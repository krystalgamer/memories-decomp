# Spanish MODEL headers 414 and 564

Twenty-four independently checked Spanish secondary images reuse the
unchanged accepted French wrappers and shared header-397 sheets, webs,
spokes, rings and quad implementations. Every entry uses the named
`gcc_2_8_1_g0_split` profile, GCC 2.8.1 and MASPSX 2.81. Historical source
comments are not compiler evidence; no body, declaration or flag change
was needed for those initial five helpers. The entry-called band helper
now uses a directly selected Spanish body with independently verified
next-column expressions, under the same compiler profile.

## Images, boundaries and reachability

The [instance ledger](spanish-model-variant414-instances.csv) records all
compact archive records, stages, commands and independent hashes.

| Stages | Models |
|---|---|
| 7/8 | 2, 20, 87, 108, 138, 193, 573 |
| 9/10 | 152, 168, 170, 388, 427 |

Each image occupies ten 2,048-byte sectors at
`record * 276 + 180 + (stage - 7) * 10`, loading at
`0x8013B000/0x8017B000`. Entry is at `+4`. There are 22 distinct complete
image hashes among the 24 instances; equality was checked across whole
images, not inferred from common helper bodies.

| Offset range | Bytes | Owner | Direct entry-call path |
|---|---:|---|---|
| `0x4..0x1228` | 4,644 | generated assembly | yes |
| `0x1228..0x1990` | 1,896 | bands C | yes |
| `0x1990..0x1E74` | 1,252 | sheets C | yes |
| `0x1E74..0x23E4` | 1,392 | webs C | yes |
| `0x23E4..0x26EC` | 776 | spokes C | no |
| `0x26EC..0x2A68` | 892 | rings C | no |
| `0x2A68..0x2DCC` | 868 | quad C | no |

All 491,520 image bytes match without masks or instruction patches.
The 144 selected, sized C owners contribute 169,824 instruction bytes.
Twenty-four entry function instances remain generated assembly, totaling
111,456 bytes. Each header and each 8,756-byte suffix has a real raw owner.
The 210,144 suffix bytes remain unclassified; storage in a data segment
does not prove the absence of game code. The three retained helpers per
image are not claimed to have a direct entry-call path.

## Accessed records and descriptor evidence

One hundred six independently target-compiled constants verify the six local
record types and canonical SDK structures, including both quad packet
types, coordinate matrices, line packets and 32-bit `PSXLONG`.
Seventy Spanish instruction anchors per image check context capture,
record formation, initialization strides, loop bounds, timing reads and
the entry's webs call.

Three 416-byte web records occupy `0..0x4E0`; each has two 4x6 `SVECTOR`
grids at `0/0xC0`, color at `0x180` and scale at `0x194`.
One 284-byte `ModelVariantBandShort` at `0x4E0` ends at `0x5FC`.
Its five-point vector arrays start at `0/40/80`, projected words at
`120/140/160`, colors at `180/200`, depths at `244` and flags at `264`.
The band packet is at `0xD9C`; radius scaling reads descriptor `+0x44`
through context `+0xF54`, with phase/radius/path state at
`0xF40/0xF6C/0xF70/0xF78`.
Two 152-byte sheets at `0x5FC` end at `0x72C`. Six 144-byte rings then
end at `0xA8C`, followed by four 144-byte spokes ending at `0xCCC`.
One 144-byte quad view begins there. Ring and spoke colors are at 128
and 132 respectively. Webs and spokes read the first sheet's size through
`context + 0x5FC + 0x88 = 0x684`; this is not a field in another web.

The shared line is at `0xE98`, the sheet `POLY_GT4` at `0xDD0` and the
quad `POLY_G4` at `0xD78`. The records and direct context accesses are
partial views, not declarations of the complete allocation.

Actual metadata requests range over 580000, 580003, 580004, 580006,
580007, 580009, 580010, 580011, 580012, 580013 and 580015. The matching
controller passes request modulo 1,000 for initialization and `-1` for
updates. Every selected 72-byte descriptor at
`module + 0x2EC8 + (request % 1000) * 72` lies inside the single suffix
owner. No duplicate descriptor storage or general table capacity is
invented.

## Resident and context ownership

All 36 resident callees were checked as real selected input and linked
function owners against the fresh exact Spanish resident, not merely
absolute bindings. This includes the existing `ratan2` binding at
`0x80089928`. Three matching initializer/controller/loader owners and both
context-pointer data owners were also verified and archived locally.
The resident SHA-256 is
`b0fefd88b6510f49af4f01e6180e40371652b7ceaa5f31dcb938c942316fc790`.

The selected `spanish_raw_80010000.o` data subsection supplies pointers at
`0x80010024/28` to `0x80136000/0x80176000`, inside the mixed executable
`.main` output section. Input storage, section extents and actual bytes
establish ownership, not output-section flags or linker aliases.
Direct entry accesses establish a minimum context extent of `0xF8C`.
That view does not overlap the selected 96-sector model, two-sector
primary or ten-sector secondary loads. Allocation capacity and whole-game
lifetime isolation remain unproved.

## Integration scope

The [terminal ledger](spanish-model-variant414-attempts.csv) identifies
the ten unchanged wrappers and the two directly selected Spanish band
sources. The [band experiment ledger](spanish-model-variant414-experiments.csv)
records the initial 1,896-byte GCC 2.8.1 result with four differing loads
and its exact refinement. Using `band->sc[j + 1]` and `band->sb[j + 1]`
instead of `col->sc[1]` and `col->sb[1]` preserves the required indexed
addressing at helper offsets `0x514/0x520/0x544/0x550`. The accepted North
American body and its different compiler profile remain untouched.
Recursive dependency hashes,
compiler objects, full-image proofs and owner evidence remain under local
scratch storage; no private inputs or binary artifacts are tracked.

The Spanish regression class reuses the existing family fixture with
Spanish inputs, inventory and ledgers. French defaults remain unchanged.
It checks strict function spans, call reachability, canonical wrappers,
raw extents, complete fallback bindings, actual commands, descriptor
selection, minimum context accesses and selected load separation.

All 68 prior Spanish module records are preserved. Configured totals
become 92 images, 430/576 C instances and 367,196 C instruction bytes.
That initial registration is preserved. The band extension adds 24 C
instances and 45,504 bytes without adding or removing image registrations.
At the extension's accepted baseline, configured totals become 128 images,
640/886 C instances and 601,004 C instruction bytes. Pending work is not
stacked into these totals.

These counts do not establish exhaustive Spanish runtime coverage or
completion of the wider seven-release campaign. General progress
snapshots remain separate.
