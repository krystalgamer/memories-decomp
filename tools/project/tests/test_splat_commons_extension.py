from __future__ import annotations

from pathlib import Path
import subprocess
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
SPLAT = REPOSITORY / "tools/environments/python/bin/splat"
EXTENSIONS = REPOSITORY / "tools/splat_extensions"
VRAM = 0x80100000

CONFIG = """\
name: commons_test
options:
  basename: commons_test
  base_path: .
  target_path: module.bin
  platform: psx
  compiler: PSYQ
  endianness: little
  asm_path: asm
  src_path: src
  build_path: build
  asset_path: assets
  cache_path: cache
  data_path: data
  ld_script_path: commons_test.ld
  generated_asm_macros_directory: include
  undefined_funcs_auto_path: undefined_funcs_auto.txt
  undefined_syms_auto_path: undefined_syms_auto.txt
  symbol_addrs_path:
    - symbols.txt
  find_file_boundaries: false
  check_consecutive_segment_types: false
  extensions_path: {extensions}
  section_order: [".text", ".rodata", ".data", ".sdata", ".sbss", ".bss"]
segments:
  - name: module
    type: code
    start: 0x0
    vram: 0x{vram:08X}
    subsegments:
      - [0x0, bin, head]
      - [0x8, {kind}, unit_commons]
      - [0x{end:X}, bin, tail]
  - [0x{size:X}]
"""


class SplatCommonsExtensionTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="splat-commons-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)

    def split(
        self, kind: str, block: bytes, labels: dict[str, int]
    ) -> subprocess.CompletedProcess[str]:
        """Split head (8 bytes), the commons block, then 6 nonzero bytes."""
        end = 8 + len(block)
        image = b"\x11" * 8 + block + b"\x22" * 6
        (self.root / "module.bin").write_bytes(image)
        (self.root / "symbols.txt").write_text(
            "".join(f"{name} = 0x{VRAM + offset:08X};\n" for name, offset in labels.items()),
            encoding="utf-8",
        )
        config = self.root / "commons_test.yaml"
        config.write_text(
            CONFIG.format(
                extensions=EXTENSIONS, vram=VRAM, kind=kind, end=end, size=len(image)
            ),
            encoding="utf-8",
        )
        return subprocess.run(
            [str(SPLAT), "split", config.name],
            cwd=self.root,
            capture_output=True,
            text=True,
            check=False,
        )

    def assembly(self, section: str) -> str:
        return (self.root / f"asm/data/unit_commons.{section}.s").read_text(
            encoding="utf-8"
        )

    def test_block_ending_mid_word_keeps_every_label(self) -> None:
        result = self.split(
            "bss_commons",
            bytes(0xE),
            {"gWord": 0x8, "gPair": 0xC, "gHalfAtEnd": 0x14},
        )
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        text = self.assembly("bss")
        self.assertIn('.section .bss, "wa"', text)
        self.assertIn(".align 3", text)
        spaces = [
            (line.split()[1], next_line.split()[-1])
            for line, next_line in zip(text.splitlines(), text.splitlines()[1:])
            if line.startswith("dlabel ")
        ]
        self.assertEqual(
            spaces, [("gWord", "0x04"), ("gPair", "0x08"), ("gHalfAtEnd", "0x02")]
        )
        script = (self.root / "commons_test.ld").read_text(encoding="utf-8")
        self.assertIn("asm/data/unit_commons.bss.s.o(.bss)", script)

    def test_small_block_goes_to_sbss(self) -> None:
        result = self.split("sbss_commons", bytes(6), {"gA": 0x8, "gB": 0xC})
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)
        text = self.assembly("sbss")
        self.assertIn('.section .sbss, "wa"', text)
        self.assertIn("dlabel gB", text)
        self.assertIn(".space 0x02", text)

    def test_nonzero_byte_is_rejected(self) -> None:
        result = self.split("bss_commons", bytes(5) + b"\x01" + bytes(2), {"gA": 0x8})
        self.assertNotEqual(result.returncode, 0)
        self.assertIn("nonzero byte at rom 0xD", result.stdout + result.stderr)


if __name__ == "__main__":
    unittest.main()
