# Reviewed MODEL450 maintenance takeover of #6997

The original head `1c01dd252c0e6f6c5edac7e1f225c681ea75f4fa` already fixed the
reviewer's German-versus-Italian image wording. Its remaining required check
failed in input preparation with HTTP 522; regional jobs were skipped. This
refresh does not repeat or circumvent the subsequently denied workflow retry.

All four source/profile/address/size selections in both slots are already
accepted in French: ribbons (`0xDD8`, 2,492 bytes), bands (`0x1794`, 1,848),
quads (`0x27C0`, 1,480), and lines (`0x2D88`, 900). There is no new French
implementation or registration to add. Existing bodies and named
GCC 2.8.1 / MASPSX 2.81 profiles remain unchanged.

Current German sectors `48224` and `48234` are already registered as
`german_model_image_model_48224_8013b000` and
`german_model_image_model_48234_8017b000`. The old patch's canonical additions
would duplicate these physical owners. This master-based replacement promotes
the reviewed selections inside the existing registrations, keeping names,
archive offsets, load addresses, hashes and the 3,584-module count intact.

Before tracked promotion, current accepted sources independently reproduced
both complete `0x5000`-byte images and all eight sized input/final `STT_FUNC`
owners, with no absolute-symbol ownership fallback. The original ledgers now
refer to the current owners and current source fingerprints.

The entry and helpers at `0x1ECC` and `0x310C` remain generated assembly;
the suffix `0x3940..0x5000` stays raw. Eight C instances add 13,440 bytes.
Tests cover physical uniqueness, exact French-selection equivalence,
source fingerprints, whole-image hashes and actual input/final owners.

This maintains previously reviewed work rather than starting the German
campaign. French remains active. No source, header, compiler profile, other
regional registration or generated report changes. The original PR is kept
open until verified supersession is reviewed; no self-merge is performed.
