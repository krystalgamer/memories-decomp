"""Check the card-frame renderer name without executing graphics."""

import csv
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile
import unittest


ROOT = Path(__file__).resolve().parents[3]
GCC = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"
NAME = "Duel_DrawCardFrame"
REGIONS = (
    ("slus_01411", 0x80016784, 0x80016D04, ""),
    ("slpm_86398", 0x80016670, 0x80016BF0, "japanese/"),
    ("sles_03947", 0x8001669C, 0x80016C1C, "european/"),
    ("sles_03948", 0x800166A0, 0x80016C20, "european/"),
    ("sles_03949", 0x800166A0, 0x80016C20, "european/"),
    ("sles_03950", 0x800166A0, 0x80016C20, "european/"),
    ("sles_03951", 0x800166A0, 0x80016C20, "european/"),
)


class DuelCardFrameNameTests(unittest.TestCase):
    def test_seven_bindings_keep_contiguous_group_ownership(self):
        for region, address, callback, tier in REGIONS:
            with self.subTest(region=region):
                directory = ROOT / "config" / region
                with (directory / "functions.csv").open(newline="") as handle:
                    entry, = [row for row in csv.DictReader(handle)
                              if int(row["address"], 0) == address]
                self.assertEqual(
                    (entry["name"], entry["status"], entry["module"], int(entry["size"], 0)),
                    (NAME, "matching_c", "game", 0x580),
                )
                owner = "src/game/" + tier + "duel_card_and_status_draw.c"
                rows = json.loads((directory / "matching_c.json").read_text())["functions"]
                group = [row for row in rows if row["source"] == owner]
                self.assertEqual(
                    [(int(row["address"], 0), int(row["size"], 0), row["profile"]) for row in group[:2]],
                    [(address, 0x580, "gcc_2_8_1_g8_split"),
                     (callback, 0x28, "gcc_2_8_1_g8_split")],
                )
                self.assertEqual(address + 0x580, callback)
                # The three life-point and deck-count functions follow in the same
                # unit, then the two selected-card checks and the stat calculator.
                self.assertEqual(len(group), 8)
                self.assertEqual(int(group[2]["address"], 0), callback + 0x28)
                self.assertTrue((ROOT / owner).is_file())
                self.assertIsNotNone(
                    re.search(rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func",
                              (directory / "symbols.txt").read_text()),
                    region,
                )

    def test_constructor_callers_and_complete_renderer_support_the_name(self):
        self.assertFalse((ROOT / "src/game/func_80016784.h").exists())
        with (ROOT / "notes/semantic-symbol-map.csv").open(newline="") as handle:
            entry, = [row for row in csv.DictReader(handle) if row["name"] == NAME]
        self.assertEqual((entry["kind"], entry["address"], entry["confidence"]),
                         ("function", "0x80016784", "high"))

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_real_owner_exposes_new_name_and_rejects_old_or_missing_declaration(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="card-frame-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for owner, symbol, accepted in (
                (True, NAME, True),
                (True, "func_80016784", False),
                (False, NAME, False),
            ):
                with self.subTest(owner=owner, symbol=symbol):
                    source = directory / "probe.c"
                    include = (
                        '#include "../../src/game/duel_draw_card_frame.h"\n' if owner else
                        '#include "../../src/game/display_object.h"\n'
                        '#include "../../src/game/display_object_packet_submit.h"\n'
                    )
                    source.write_text(include + f"__typeof__(&{symbol}) checked = {symbol};\n")
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
                        self.assertIn("undeclared", result.stderr)


if __name__ == "__main__":
    unittest.main()
