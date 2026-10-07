# French MODEL444 beams helper

`func_8013C4E8` / `func_8017C4E8` (MODEL444 family, image offset `0x14E8`,
0x8DC bytes) draws the sixteen two-point beams of the accepted Spanish
header-411 helper ([Spanish MODEL411 beams](spanish-model-variant411-beams.md)).

The raw comparison shows identical words, apart from local jump targets
and one branch. The first beam half is sorted only for a positive depth
(`blez` skip) rather than a non-negative one (`bltz`). The shared
`spanish_model_variant/variant411_beams.c` selects that test with
`MODEL_VARIANT444_BEAMS`. Its default keeps the header-411 objects unchanged,
and the Spanish attempt ledger records the replay. The French
`variant444_beams.c` wrapper and its slot-one form define it.

The helper is registered in both French images that carry it (model 175,
stage 9 slot 0 and stage 10 slot 1). `RotTransPers` was added to the French
MODEL444 binding file at its resident address `0x80087868`. All French and
Spanish overlay images rebuild exactly. The
[attempt ledger](french-model-variant444-beams-attempts.csv) records the
comparison and the exact result.
