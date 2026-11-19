"""Splat segment type for a unit's zero-initialized globals (`<unit>_commons`).

A `_comm` translation unit declares its zero-initialized globals as GCC
tentative definitions, and MASPSX re-emits them as `.comm` symbols that the
linker resolves against the labels this subsegment provides. The block is
written the way an assembler allocates commons: one label and one `.space`
per symbol of the region's symbol file, at its retail address. Splat's own
`bss`/`sbss` types disassemble the same bytes as data words, which drops any
label in a trailing halfword, so they cannot list a block that ends mid-word.

The image keeps the bytes (a PSX image is loaded whole), so every byte of
the block must be zero; anything else is data and belongs elsewhere. The
assembly keeps the `<name>.bss.s` / `<name>.sbss.s` path of the built-in
types, so the build treats both alike.
"""

from __future__ import annotations

from pathlib import Path
from typing import List, Optional

from splat.disassembler.disassembler_section import make_bss_section
from splat.segtypes.common.bss import CommonSegBss
from splat.util import log, options, symbols


# MASPSX aligns a common by its size, to at most 16 bytes.
MAX_COMMON_ALIGNMENT_POWER = 4


def alignment_power(address: int) -> int:
    """The largest power of two, up to 16, that the address is aligned to."""
    power = 0
    while power < MAX_COMMON_ALIGNMENT_POWER and not address & (1 << power):
        power += 1
    return power


def first_nonzero_offset(block: bytes) -> Optional[int]:
    for offset, value in enumerate(block):
        if value:
            return offset
    return None


class PSXSegBss_commons(CommonSegBss):
    def asm_out_path(self) -> Path:
        section = self.get_linker_section().removeprefix(".")
        return options.opts.data_path / self.dir / f"{self.name}.{section}.s"

    def get_asm_file_header(self) -> List[str]:
        # The block's labels are as aligned as their retail addresses, so the
        # linker compares those, not a byte-aligned section, with the
        # alignment of each matching common. The start already has this
        # alignment, so the directive never moves the block.
        assert isinstance(self.vram_start, int), self.vram_start
        header = super().get_asm_file_header()
        header.extend([f".align {alignment_power(self.vram_start)}", ""])
        return header

    def disassemble_data(self, rom_bytes: bytes) -> None:
        if self.is_auto_segment:
            return
        if not isinstance(self.rom_start, int) or not isinstance(self.rom_end, int):
            log.error(
                f"Segment '{self.name}' (type '{self.type}') requires a rom start and end"
            )
        if not isinstance(self.vram_start, int):
            log.error(
                f"Segment '{self.name}' (type '{self.type}') requires a vram address"
            )
        segment_rom_start = self.get_most_parent().rom_start
        assert isinstance(segment_rom_start, int), segment_rom_start

        nonzero = first_nonzero_offset(rom_bytes[self.rom_start : self.rom_end])
        if nonzero is not None:
            log.error(
                f"Segment '{self.name}' (type '{self.type}') holds a nonzero byte at "
                f"rom 0x{self.rom_start + nonzero:X}; commons are zero-initialized, "
                "so end the block before it"
            )

        self.spim_section = make_bss_section(
            self.rom_start,
            self.rom_end,
            self.vram_start,
            self.vram_start + self.rom_end - self.rom_start,
            self.name,
            segment_rom_start,
            self.get_exclusive_ram_id(),
        )
        self.configure_disassembler_section(self.spim_section)
        self.spim_section.analyze()
        self.spim_section.set_comment_offset(self.rom_start)

        for spim_sym in self.spim_section.get_section().symbolList:
            symbols.create_symbol_from_spim_symbol(
                self.get_most_parent(),
                self,
                spim_sym.contextSym,
                force_in_segment=True,
            )
