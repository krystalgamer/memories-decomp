# Reviewed MODEL450 maintenance takeover of #6999

The original head `2cb27d43a8a054727869c6325203edf9bead2f8d` was reviewed
clean apart from required CI stopping in input preparation with HTTP 522.
The subsequently denied workflow retry is not repeated or bypassed.

All four source/profile/address/size selections in both slots are already
accepted in French: ribbons (`0xDD8`, 2,492 bytes), bands (`0x1794`, 1,848),
quads (`0x27C0`, 1,480), and lines (`0x2D88`, 900). There is no new French
implementation or registration to add. Existing bodies and named
GCC 2.8.1 / MASPSX 2.81 profiles remain unchanged.

Current English PAL sectors `48224` and `48234` are already registered as
`european_model_image_model_48224_8013b000` and
`european_model_image_model_48234_8017b000`. The old canonical additions would
duplicate these physical owners. This master-based replacement promotes the
reviewed selections inside existing registrations, keeping names, offsets,
addresses, hashes and the 3,581-module count intact.

Before tracked promotion, current accepted sources independently reproduced
both complete `0x5000`-byte images and all eight sized input/final `STT_FUNC`
owners, with no absolute-symbol ownership fallback. Regional bindings from
the reviewed patch remain unchanged: 35 distinct external English resident
owners and 75 differing JAL words per image, not 75 distinct functions.
The original ledgers now refer to current owners and source fingerprints.

The entry and helpers at `0x1ECC` and `0x310C` remain generated assembly;
the suffix `0x3940..0x5000` stays raw. Fourteen previously unclassified
functions are inventoried, eight as C and six as assembly. Eight C instances
add 13,440 bytes, yielding 223 inventoried functions / 217 C / 151,096 bytes.
Tests cover physical uniqueness, exact French-selection equivalence,
source fingerprints, whole-image hashes and actual input/final owners.

No source, header, compiler profile, other regional registration or generated
report changes. The original PR remains open until verified supersession is
reviewed; no self-merge is performed. French remains the active campaign.
