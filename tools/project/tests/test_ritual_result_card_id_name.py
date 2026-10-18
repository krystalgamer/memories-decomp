"""Guard the stored ritual result's signed halfword and seven physical bindings."""

import csv
import os
from pathlib import Path
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
NAME = "gDuel_RitualResultCardId"
REGIONS = (
    ("slus_01411", 0x8009B1A0, "c_symbols.ld"),
    ("slpm_86398", 0x8009B090, None),
    ("sles_03947", 0x8009C17E, None),
    ("sles_03948", 0x8009C59E, "link_symbols.ld"),
    ("sles_03949", 0x8009C59E, "link_symbols.ld"),
    ("sles_03950", 0x8009C59E, "link_symbols.ld"),
    ("sles_03951", 0x8009C59E, "link_symbols.ld"),
)
GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


class RitualResultCardIdNameTests(unittest.TestCase):
    def test_seven_addresses_and_existing_linker_aliases(self):
        for config, address, linker in REGIONS:
            with self.subTest(config=config):
                directory = ROOT / "config" / config
                self.assertRegex((directory / "symbols.txt").read_text(),
                                 rf"(?m)^{NAME} = 0x{address:08X};$")
                if linker:
                    text = (directory / linker).read_text()
                    self.assertIn(f"D_8009B1A0 = 0x{address:08X};", text)
                    self.assertIn(f"{NAME} = 0x{address:08X};", text)
        french = (ROOT / "config/sles_03948/symbols.txt").read_text()
        self.assertIn("D_8009B1A0_French = 0x8009B1A0;", french)
        self.assertNotIn(f"{NAME} = 0x8009B1A0;", french)
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("global", "0x8009B1A0", "confirmed"))


    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_owned_signed_halfword_width_and_type_guards(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="ritual-result-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for pointer, declaration, accepted in (
                ("s16", "s16", True), ("s32", "s16", False), ("u16", "u16", False),
            ):
                with self.subTest(pointer=pointer):
                    include = ROOT / "src/game/duel_ritual_controller.h"
                    if declaration != "s16":
                        original = include.read_text()
                        old = f"extern s16 {NAME};"
                        self.assertEqual(original.count(old), 1)
                        header = original.replace(old, f"extern {declaration} {NAME};", 1)
                        header = header.replace(
                            '#include "../types.h"', f'#include "{ROOT / "src/types.h"}"',
                        ).replace(
                            '#include "display_object.h"', f'#include "{ROOT / "src/game/display_object.h"}"',
                        )
                        include = directory / "unsigned-owner.h"
                        include.write_text(header)
                    source = directory / "probe.c"
                    source.write_text(
                        f'#include "{include}"\n'
                        f"typedef char halfword[sizeof({NAME}) == 2 ? 1 : -1];\n"
                        f"typedef char signed_word[((__typeof__({NAME}))-1 < 0) ? 1 : -1];\n"
                        f"{pointer} *G32 checked = &{NAME};\n"
                    )
                    result = subprocess.run(
                        [str(GCC), "-S", "-O2", "-G8", "-Werror", "-o",
                         str(directory / "probe.s"), str(source)],
                        cwd=ROOT, env=dict(os.environ, TMPDIR=str(directory)),
                        text=True, capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn(
                            "negative" if declaration != "s16" else "incompatible pointer type",
                            result.stderr,
                        )


if __name__ == "__main__":
    unittest.main()
