"""Check the constructor's semantic name and both measured declaration arms."""

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
NAME = "Duel_CreateCardDisplayObject"
REGIONS = (
    ("slus_01411", 0x80017F04, "src/game/duel_init_scene.c",
     ((0x800179F4, 0x3C0), (0x80017DB4, 0x88), (0x80017E3C, 0xC8), (0x80017F04, 0x100),
      (0x80018004, 0x7C), (0x80018080, 0xD0), (0x80018150, 0x9C), (0x800181EC, 0x70))),
    ("slpm_86398", 0x80017D80, "src/game/japanese/duel_init_scene.c",
     ((0x80017878, 0x3B8), (0x80017C30, 0x88), (0x80017CB8, 0xC8), (0x80017D80, 0x100),
      (0x80017E80, 0x7C), (0x80017EFC, 0xD0), (0x80017FCC, 0x9C), (0x80018068, 0x70))),
    ("sles_03947", 0x80017E90, "src/game/european/duel_init_scene.c",
     ((0x80017988, 0x3B8), (0x80017D40, 0x88), (0x80017DC8, 0xC8), (0x80017E90, 0x100),
      (0x80017F90, 0x7C), (0x8001800C, 0xD0), (0x800180DC, 0x9C), (0x80018178, 0x70))),
) + tuple(
    (region, 0x80017E94, "src/game/european/duel_init_scene.c",
     ((0x8001798C, 0x3B8), (0x80017D44, 0x88), (0x80017DCC, 0xC8), (0x80017E94, 0x100),
      (0x80017F94, 0x7C), (0x80018010, 0xD0), (0x800180E0, 0x9C), (0x8001817C, 0x70)))
    for region in ("sles_03948", "sles_03949", "sles_03950", "sles_03951")
)


class CardDisplayConstructorNameTests(unittest.TestCase):
    def test_seven_bindings_and_original_group_profiles_order_and_extents(self):
        for region, address, owner, expected_group in REGIONS:
            with self.subTest(region=region):
                directory = ROOT / "config" / region
                with (directory / "functions.csv").open(newline="") as handle:
                    entry, = [row for row in csv.DictReader(handle)
                              if int(row["address"], 0) == address]
                self.assertEqual(
                    (entry["name"], entry["status"], entry["module"], int(entry["size"], 0)),
                    (NAME, "matching_c", "game", 0x100),
                )
                rows = json.loads((directory / "matching_c.json").read_text())["functions"]
                group = sorted((row for row in rows if row["source"] == owner),
                               key=lambda row: int(row["address"], 0))
                self.assertEqual(
                    [(int(row["address"], 0), int(row["size"], 0)) for row in group],
                    list(expected_group),
                )
                self.assertTrue(all(row["profile"] == "gcc_2_8_1_g8_split" for row in group))
                self.assertTrue((ROOT / owner).is_file())
                for left, right in zip(expected_group, expected_group[1:]):
                    self.assertEqual(sum(left), right[0])
                self.assertIsNotNone(
                    re.search(rf"(?m)^{NAME}\s*=\s*0x{address:08X};.*type:func",
                              (directory / "symbols.txt").read_text()), region,
                )
        self.assertIn("game/japanese/duel_init_scene",
                      (ROOT / "config/slpm_86398/split.yaml").read_text())
        self.assertFalse((ROOT / "src/game/japanese/duel_create_card_display_object.c").exists())
        self.assertFalse((ROOT / "src/game/european/duel_card_display_state.c").exists())
        self.assertFalse((ROOT / "src/game/japanese/func_80017f04.c").exists())

    @unittest.skipUnless(GCC.is_file(), "requires supported GCC 2.8.1")
    def test_actual_owner_arity_return_and_name_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        with tempfile.TemporaryDirectory(prefix="card-factory-name-", dir=ROOT / "tmp") as name:
            directory = Path(name)
            for owner, declaration, accepted, diagnostic in (
                (True, f"DuelCardDisplayObject *(*checked)(DuelCardRecord *, s32, s32) = {NAME};", True, ""),
                (True, f"DuelCardDisplayObject *use(DuelCardRecord *card) {{ return {NAME}(card); }}", False,
                 "too few arguments"),
                # Duel_CreateHandCardDisplayObject's record-only call goes through an
                # unprototyped cast so the caller's $a1/$a2 pass through.
                (True, f"DuelCardDisplayObject *use(DuelCardRecord *card) "
                       f"{{ return ((DuelCardDisplayObject *(*)()){NAME})((u8 *)card); }}", True, ""),
                (True, f"DuelCardDisplayObject *use(DuelCardRecord *card) "
                       f"{{ return ((u8 *(*)()){NAME})((u8 *)card); }}", False,
                 "incompatible pointer type"),
                (True, f"u8 *(*checked)(DuelCardRecord *, s32, s32) = {NAME};",
                 False, "incompatible pointer type"),
                (True, f"DuelCardDisplayObject *(*checked)(DuelCardRecord *, s16, s16) = {NAME};",
                 False, "incompatible pointer type"),
                (True, "__typeof__(&func_80017F04) checked = func_80017F04;", False, "undeclared"),
                (False, f"__typeof__(&{NAME}) checked = {NAME};", False, "undeclared"),
            ):
                with self.subTest(owner=owner, declaration=declaration):
                    source = directory / "probe.c"
                    include = "duel_card_display_state.h" if owner else "duel_card.h"
                    source.write_text(f'#include "../../src/game/{include}"\n' + declaration + "\n")
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
                        self.assertIn(diagnostic, result.stderr)


if __name__ == "__main__":
    unittest.main()
