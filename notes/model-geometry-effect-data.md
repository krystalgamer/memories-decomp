# Model geometry-effect configuration data

The subdivided, disc and burst handlers retain their existing configuration
views over four independently stored initialized objects. Naming does not
change a view into a new storage definition or infer meanings for unread
bytes.

`gModel_SubdividedEffectConfigWords` is the former `D_800915E8`.
Its seven `u32` words remain the 28-byte owner. The existing local
`ModelSubdividedEffectConfig` view reads three six-byte colour channels,
signed height/radius minima and growth/fade durations. Its cast, alignment,
duration arithmetic and regional behavior remain exact.

`gModel_DiscEffectConfig` is the former `D_80091604`, still the established
12-byte `ModelDiscEffectConfig` instance. The handler reads its rim colour,
minimum radius, initial/full level and target scale. The last zero halfword
remains unread and unclassified.

`gModel_BurstImageData` is the former `D_80091610`. All 98 halfwords and
196 initialized bytes remain in the original unsized `u16` owner. Current C
uses only its head through `ModelBurstImage`; the six trailing descriptor-
shaped records are not dropped, assigned a new runtime role or retyped as a
complete `GsIMAGE`/`ModelBurstImage` array.

`gModel_BurstRingPaletteWords` is the former `D_800916D4`. The three `u32`
words retain all twelve bytes. The existing nine-byte `ModelBurstPalette`
view stores one byte per ring for each channel. It does not justify regrouping
the owner into RGB records or removing its trailing bytes.

Native aliases preserve the original binding values and region-specific
providers. The European subdivided configuration spelling bridge retires
when its canonical binding is wired. Reader source grouping, profiles,
initializers, qualifiers, existing casts and control paths are unchanged.
No regional table contents are assumed equal to NA.
