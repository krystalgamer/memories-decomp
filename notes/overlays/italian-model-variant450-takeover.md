# MODEL450 maintenance takeover of #6996

The original PR selects four already-accepted C helpers in each Italian
MODEL174 stage-9/10 image: ribbons (`0xDD8`, 2,492 bytes), bands (`0x1794`,
1,848 bytes), quads (`0x27C0`, 1,480 bytes), and lines (`0x2D88`, 900 bytes).
The first two use the accepted French bodies; the other two use accepted
Spanish bodies. All use `gcc_2_8_1_g0_split`, GCC 2.8.1 / MASPSX 2.81.

Its reviewed head `77ffc63289275ed957396335aa8333cba4463c17` was source-clean
but blocked by HTTP 522 during CI input preparation. A later workflow retry
was denied. This maintenance does not repeat or circumvent that restricted
action. A meaningful refreshed integration receives its own ordinary validation
on a new master-based branch.

## French applicability and current inventory

All eight source/profile/address/size selections are already present in the
corresponding accepted French registrations. There is no additional French
port to make and no duplicated French implementation is added.

Since the original PR, Italian sectors `48224` and `48234` became registered
as `italian_model_image_model_48224_8013b000` and
`italian_model_image_model_48234_8017b000`. Reapplying the old additions would
duplicate those physical modules. The refreshed patch promotes the selections
inside these existing registrations, keeping the full manifest at 3,584 modules
and preserving names, archive slices, load addresses, headers and retail hashes.

This is maintenance of previously reviewed work, not the start of the Italian
campaign. French remains the active decompilation campaign. No source, header,
compiler profile, other regional registration or generated report changes.

## Acceptance and retained ownership

Before tracked promotion, temporary layouts using the current names reproduced
both complete `0x5000`-byte images. All eight input objects own real `STT_FUNC`
symbols of the original sizes, and final owners retain their addresses and
non-absolute sections. Source fingerprints were refreshed from the unchanged
accepted bodies after independently checking their current output.

The entry `0x4..0xDD8`, helper `0x1ECC..0x27C0`, and helper
`0x310C..0x3940` stay generated assembly. The `0x3940..0x5000` raw suffix
is preserved. Resident bindings remain the target-specific starts selected
by the original PR. Eight C instances contribute 13,440 bytes.

The original instance and attempt ledgers retain their evidence and now refer
to the current raw-image owners. Production regressions check physical
uniqueness, exact French-selection equivalence, unchanged-source fingerprints,
complete hashes, and sized input/final owners. The original PR stays open
until a verified replacement is reviewed; no self-merge is performed.
