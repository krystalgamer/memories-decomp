# Japanese MODEL variant wrappers

Each file here builds a Japanese `MODEL.MRG` image from the US source of the
same name in `../../model_variant/` (or, for header 162, the French source it
already includes). The Japanese image differs from its US counterpart only in
one or more rodata padding bytes after the work-size format string, so the
wrapper defines that string's `MODEL<N>_STRING_PAD` (or `MODEL89_ORDER_PAD`)
selector before including the US source. Slot-1 wrappers repeat the US slot-1
renames. Without a selector the US sources keep their US padding.

The Japanese manifests in `config/slpm_86398/overlays/` select these wrappers
with the same named profile as the US image at the same sector. Resident
references link through the Japanese `*_linker_symbols.txt` files described in
`notes/overlays/japanese-model-us-layouts.md`.
