# French MODEL425 four-funnel sibling

MODEL0 stages 9/10 contain the independently measured 1,448-byte helper
at image `+0x3360`, `func_8013E360` / `func_8017E360`. Two wrappers select
`MODEL_VARIANT425_FUNNEL` and map the function symbol before including the
accepted [MODEL423 funnel body](french-model-variant423-funnel.md).
The existing `gcc_2_8_1_g0_split` profile retains GCC 2.8.1 / MASPSX 2.81.

## Original-image differences

The four records and complete control flow are the accepted four-funnel shape:
seventeen-point inner/outer rings, per-record x/z scaling, common y scale,
signed colour attenuation, unconditional size wrapping, and spin by step *80.
There are three measured differences:

| Property | MODEL423 default | MODEL425 |
|---|---|---|
| Radius multiplier | 384 | 512 |
| Sheet-to-funnel opaque interval | `0x668` bytes | `0x64` bytes |
| Fade-to-palette opaque interval | `0x24` bytes | `0x28` bytes |

The multiplier affects the inner radius and both vertical translation terms.
Power-of-two multiplication removes six instructions, explaining 0x5A8 versus
the default 0x5C0-byte body. The first interval moves funnels and following
fields back by 1,540 bytes; the second adds four before colours/spin/phase.
Neither interval is classified as an allocation or interpreted data structure.

The conditional local view retains `Funnel423` / `Funnel423State` as the shared
type names; it does not import a guessed regional header. Its measured fields
for this family are:

| Field | Context offset |
|---|---|
| Two sheets | `0xE1C` |
| Four 0x118-byte funnels | `0xFB0` |
| GT4 packet | `0x155C` |
| Position / direction | `0x1754` / `0x175C` |
| Frame / step | `0x1788` / `0x1794` |
| Common size / global fade | `0x17B4` / `0x17C4` |
| Inner / outer colours | `0x17F0` / `0x17F4` |
| Spin / phase | `0x17F8` / `0x180C` |

This is a minimum accessed view, not an allocation-capacity claim. Fifteen
field-offset assertions and the record-size assertion are target-compiled
separately for the default and selected layouts.

## Experiment and integration record

The first candidate substituted radius and a uniform context offset shift,
giving the correct 0x5A8 extent but seventeen differing words. Original palette,
spin and phase loads established the additional four-byte gap. Correcting
that gap gave zero differing words in each actual slot. The probes and local
views remain under `tmp/fr-probe/u0/`; integration waited until #7227 accepted
the shared body, then branched from accepted `45ad19f4c`.

Production builds reproduce all 3,594 complete French overlay images.
Both new helpers have real 0x5A8-byte C object definitions and retain original
ELF addresses, without helper `.NON_MATCHING` symbols. The two accepted
MODEL423 helpers also retain their original hashes, addresses and 0x5C0 sizes.
The [new terminal ledger](french-model-variant425-funnel-attempts.csv) includes
wrapper, shared-header and shared-body fingerprints; default MODEL423 replay
rows preserve the original terminal history.

No resident binding, compiler profile, physical registration, other regional
source or generated report changes. This adds two matching-C instances and
2,896 bytes.
