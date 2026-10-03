# Remaining configured USA MODEL functions

Snapshot of `master` at `8bd720af1` (2026-10-03), after #6957 merged. The USA resident game C targets are already fully matched. The remaining configured work is in MODEL variants: **190 unresolved function instances in 154 images**, totaling **746,904 bytes**. These ranges form sixteen address/size layouts; matching one shared body can therefore advance multiple images. This is an inventory of configured images, not an exhaustive census of MODEL.MRG or other runtime banks.

| Headers | Images | Unresolved instances | Bytes | Offsets and lengths |
|---|---:|---:|---:|---|
| 407/557 | 30 | 30 | 144,720 | `0x4+0x12D8` |
| 418/568 | 26 | 26 | 109,928 | `0x4+0x1084` |
| 397/547 | 24 | 24 | 111,552 | `0x4+0x1228` |
| 422/572 | 14 | 14 | 17,360 | `0x1234+0x4D8` |
| 428/578 | 12 | 12 | 49,776 | `0x4+0x1034` |
| 404/554 | 12 | 24 | 93,360 | `0x4+0x11D0`, `0x1864+0xC94` |
| 398/548 | 10 | 20 | 63,120 | `0x4+0x12B4`, `0x12B8+0x5F4` |
| 423/573 | 4 | 4 | 15,440 | `0x4+0xF14` |
| 416/566 | 4 | 4 | 16,704 | `0x4+0x1050` |
| 425/575 | 4 | 4 | 17,872 | `0x4+0x1174` |
| 448/598 | 4 | 8 | 34,880 | `0x4+0x1250`, `0x1678+0xFC0` |
| 443/593 | 2 | 6 | 29,312 | `0x4+0xCFC`, `0x20C8+0xF3C`, `0x3004+0x1D08` |
| 415/565 | 2 | 4 | 9,872 | `0x4+0xDAC`, `0xDB0+0x59C` |
| 401/551 | 2 | 4 | 11,968 | `0x4+0xFE0`, `0xFE4+0x780` |
| 459/609 | 2 | 4 | 14,680 | `0x4+0x135C`, `0x1714+0x950` |
| 414/564 | 2 | 2 | 6,360 | `0x17DC+0xC6C` |

Offsets are relative to each image's load address; each value after `+` is the function length. The two headers normally identify the slot-0 and slot-1 forms. Header 443 has a separate large layout for model 125. Families with several unresolved ranges need both their entry controller and companion renderers reconstructed.

The header-422 ring renderer is now preserved as a [build-integrated candidate](../../src/candidates/model_variant_185_pos0_slot0/func_8013C234.c), with its [retail assembly target](../../src/candidates_target/model_variant_185_pos0_slot0/func_8013C234.S) and [compiler, build, target-byte and fourteen callee-contract fingerprints](../../config/slus_01411/candidates.json). It emits all 310 target instructions, with twelve differing words confined to scale/radius and translation setup. The retail assembly remains linked, so the matching totals above do not change. Seven models use this family in both slots: 185, 391, 436, 504, 594, 367 and 395.

To reproduce the comparison and check that the candidate has not drifted:

```sh
make build-overlays
tools/environments/python/bin/python tools/project/overlay_diff.py \
    model_variant_185_pos0_slot0 0x8013C234 \
    src/candidates/model_variant_185_pos0_slot0/func_8013C234.c \
    --profile gcc_2_7_2_cdk_g0
tools/environments/python/bin/python tools/project/candidate_builds.py --overlays
```

The comparison reports `DIFF` while this remains a candidate. The build check verifies the recorded candidate fingerprint and that its external symbols exist in the module ELF; it does not claim matching C. A future promotion must replace the assembly ownership and verify every complete image in both slots. See [MODEL variants](model-variants.md) for family evidence and images still left unregistered.
