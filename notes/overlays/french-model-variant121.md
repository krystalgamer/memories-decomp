# French MODEL121 entry

The instance ledger identifies 24 distinct 20,480-byte images with headers
121/251. Models 281, 502, 509, 514 and 641 select stages 7/8; models 39, 128,
227, 477, 514, 557 and 708 select stages 9/10. Each slot has one unique
4,260-byte closed entry and a 16-byte source-owned unit-scale vector.
The 1,065 instructions contain 57 direct calls to 25 resident destinations,
with no local or indirect calls. The slot wrapper renames the entry,
literal and two raw-storage views.

All 24 complete images were independently linked from the actual C objects
and three disjoint raw owners. The resulting 48 C contributions and 72 raw
owners reproduce every byte, without masking. Each resident callee body
also agrees with the exact French resident ELF. The added `SetPolyGT3`
binding is the existing SDK function at French `0x80082E68`: its five
instructions store packet length 9 at byte 3 and opcode `0x34` at byte 7.
No resident implementation or speculative SDK declaration is added.

## Local views and native behavior

The descriptor is 24 bytes: RGB at `0..2`, part at `3`, count at `4`,
an unidentified byte at `5`, then signed halfwords for sprite size at `6`,
spread at `8`, ring radius at `0xA`, ring depth at `0xC`, travel duration at
`0xE`, fade duration at `0x10`, particle stagger at `0x12`, initial delay
at `0x14`, and ring duration at `0x16`. Archive commands are `52000 +
argument`; the entry selects `argument % 100`. Arguments below 100 generate
a rainbow palette, while arguments 100 or greater use descriptor RGB.
All nine selected descriptors have been checked in all 24 images.

The `0x544`-byte state view contains a guest-width descriptor pointer,
64 positions at `4`, 64 destinations at `0x204`, 26 ring vectors at `0x404`,
a 104-byte palette/texture union at `0x4D4`, completion at `0x53C` and
elapsed time at `0x540`. This is an access view, not a recovered allocation
extent. The native frame is 1,000 bytes; 32 target-compiled constants cover
the local and shared layouts.

Only 24 colors are initialized. The last ring triangle deliberately reads
its second RGB from color index 24, overlapping the first packed texture
handle at `0x534`. The explicit union preserves that observed overlap
without an out-of-bounds 24-element array or an invented initialized color.
Packed handles are unsigned words; their high halves use native unsigned
loads. The color code bytes are not initialized.

Initialization zeroes the configured number of positions, creates random
destinations and a ring center plus 25 outer points, then uploads two
textures. Elapsed time starts at negative initial delay. During the
negative-time phase, exactly 24 positions receive the active part's
translation, even though selected descriptor counts range from 15 to 64.
Do not substitute the configured count for this native constant.

Each active particle draws four quads from a three-by-three vertex grid.
The palette index is `(time / 2) % 24`; depth beyond 450 attenuates RGB.
All three RGB scalars are computed before packet stores. Particle scale
grows from 512, and movement divides the remaining coordinate difference
by remaining travel time before multiplying by two. Inactive particles
reset to the current active-part translation.

The ring rotates and ramps scale during fade-in and fade-out, with a
short-circuit full-scale plateau. Its 26 projections supply 24 triangles.
Screen indices are `0, i + 1, i + 2`, but depth and flag indices are
`0, i, i + 1`; this native discrepancy is preserved. Ring UVs wrap within
the upper half of the texture. Frame-step calls retain their native
ordering, and the terminal threshold narrows `travel_duration / 3` to
signed 16 bits. Completion returns 1 once, then 2; preceding phases
return 0 or 4.

## Matching evidence and remaining coverage

Computing RGB before packet writes removed 60 excess bytes. The combined
plateau condition recovered 12 missing bytes. Segment-local rainbow
expressions `(i - 8) * 3` and `(i - 16) * 3` recovered native induction and
scratch-register allocation; algebraically equivalent `i * 3 - 24/48`
expressions did not. Declaring the genuine direction scalar before the OT
pointer recovered the final spill order. Unsigned packed handles recovered
the final two high-half loads. The literal retains its address-based name.

The 31-row ledger preserves 14 paired experiments, one explicitly blocked
pre-link attempt, and two canonical matches. The blocked attempt completed
layout checks and slot-0 compilation but lacked the `SetPolyGT3` binding;
it claims no completed comparison or slot-1 experiment. The accepted source
uses the authoritative `gcc_2_8_1_g0_split` GCC 2.8.1/MASPSX 2.81 profile.
No forced registers, artificial stores, fake dependencies, inline assembly
or guessed allocator ownership are used.

Each image retains its raw four-byte header, two 28-byte GsIMAGE records at
`0x10B8..0x10F0`, and an explicitly unclassified 16,144-byte suffix at
`0x10F0..0x5000`. Descriptors are local views into that suffix; their
storage is not promoted to C. The combined 387,456 suffix bytes are not
declared data-only or excluded from remaining game-code coverage.
This adds 24 C instances, 102,240 instruction bytes and 384 literal bytes,
not exhaustive French runtime completion.
