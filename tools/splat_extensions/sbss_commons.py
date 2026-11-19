"""`<unit>_commons` block in .sbss; see bss_commons.py."""

from __future__ import annotations

from splat.segtypes.segment import Segment

PSXSegBss_commons = Segment.get_extension_segment_class("bss_commons")
assert PSXSegBss_commons is not None


class PSXSegSbss_commons(PSXSegBss_commons):
    def get_linker_section(self) -> str:
        return ".sbss"
