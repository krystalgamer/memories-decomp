# Model primitive decoder input records

The resident primitive decoder at `0x8004D134` now takes the SDK
`GsTYPEUNIT` descriptor and `GsARGUNIT` context instead of a halfword
pointer and byte pointer. `func_8004CB0C` supplies both directly from the
`GsScanUnit` dispatch: tags 0 and 1 select the normal/shared primitive
paths, while other tags use different handlers.

Only the descriptor's low 16 type bits are read, exactly as before. The
typed `u16` narrowing deliberately retains those halfword loads, including
the repeated selector read inside the loop. No interpretation of the
descriptor's other flags is added.

## Common SDK context prefix

The tracked Psy-Q `libhmd.h` declares `primp` at offset zero of the common
`GsARGUNIT` prefix. The normal and shared variants both put `primtop` at
`+0x14`. The decoder uses only those two pointers; it does not reinterpret
the variant-specific vertex/normal tables beyond them. Assertions check
that both concrete SDK variants retain the established `primtop` offset.

The decoder owner's `ModelPrimitiveBlockHeader` view describes the eight bytes
reached through `primp`: an uninterpreted first halfword, the unsigned
record count at `+2`, and the signed word offset at `+4`. Size and offset
assertions preserve the two existing loads. The record address remains
`primtop + word_offset * 4` in bytes: no extra flag mask, unsigned
reinterpretation or guessed primitive-field layout is introduced.

This agrees with the neighboring normal primitive handlers' use of
`arg->primp`, its second halfword and `arg->primtop`, without copying their
different offset masking into this decoder. The eight selector arms,
variable record strides, halfword edits, index maxima and byte totals all
retain their existing logic.

The European path formerly reused the byte-typed context parameter as a
per-record halfword cursor. It now names that cursor separately as bytes,
rather than forming a potentially unaligned SDK-structure pointer into a
primitive. The context itself stays an SDK context throughout its lifetime.
No regional target address or compiler profile is changed.

`tools/project/tests/test_model_primitive_context_headers.py` compiles the
owner header with GCC 2.8.1, accepts normal/shared SDK context prefixes and
rejects the old raw descriptor/context function-pointer contracts. Full
North American executable matching is the code-generation acceptance
gate; regional exact-match results are left to PR CI.
