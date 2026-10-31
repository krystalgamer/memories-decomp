# Japanese MODEL primary wrappers

`primary64.c` and `primary64_slot1.c` build the Japanese header-141 primary
images (`MODEL.MRG` sectors 39136 and 39138) from the shared
`../../spanish_model_primary/primary64.c`. The Japanese build places each
billboard quad from `-height * 3 / 4` to `height / 4` instead of
`-height - 150` to `height - 150`, which the shared source selects with
`VERSION_JAPAN`. The function is therefore 0x74 bytes longer, so the wrappers
rename the scale, image, and descriptor storage to their Japanese addresses.
Slot-1 wrappers repeat the US slot-1 renames.
