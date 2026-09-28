import csv
import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
GROUPS = (
    ("main_init", 0x80012A44, 1, 404),
    ("debug_menu_sound_entry", 0x800309F4, 3, 1284),
    ("main_run_boot_sequence", 0x80043D7C, 3, 776),
    ("dialog_choice_cursor", 0x80036EF0, 2, 420),
)


class FrenchLocalizedRuntimeTests(unittest.TestCase):
    def test_complete_groups_keep_verified_profiles_sizes_and_order(self) -> None:
        manifests = {
            region: json.loads(
                (ROOT / "config" / region / "matching_c.json").read_text()
            )["functions"]
            for region in ("sles_03948", "sles_03951")
        }
        for name, address, count, size in GROUPS:
            with self.subTest(source=name):
                spanish_source = f"src/game/spanish/{name}.c"
                french_source = (
                    spanish_source if name == "dialog_choice_cursor"
                    else f"src/game/french/{name}.c"
                )
                es = [row for row in manifests["sles_03951"] if row["source"] == spanish_source]
                fr = [row for row in manifests["sles_03948"] if row["source"] == french_source]
                self.assertEqual(len(es), count)
                self.assertEqual(len(fr), count)
                self.assertEqual(
                    [(row["size"], row["profile"]) for row in es],
                    [(row["size"], row["profile"]) for row in fr],
                )
                start = address
                for row in fr:
                    self.assertEqual(int(row["address"], 0), address)
                    address += int(row["size"], 0)
                self.assertEqual(address - start, size)

    def test_french_wrappers_select_measured_language_without_copies(self) -> None:
        for name, _, _, _ in GROUPS:
            if name == "dialog_choice_cursor":
                continue
            with self.subTest(source=name):
                wrapper = ROOT / f"src/game/french/{name}.c"
                lines = [line for line in wrapper.read_text().splitlines() if line]
                self.assertEqual(lines, [
                    '#include "../../types.h"',
                    "#define BUILD_LANGUAGE_INDEX 1",
                    f'#include "../european/{name}.c"',
                ])

    def test_compiled_main_keeps_verified_crt_boundaries(self) -> None:
        with (ROOT / "config/sles_03948/functions.csv").open() as handle:
            rows = {row["address"]: row for row in csv.DictReader(handle)}
        for address, size in (
            ("0x800128CC", "0xA0"),
            ("0x8001296C", "0x70"),
            ("0x800129DC", "0x68"),
        ):
            with self.subTest(address=address):
                row = rows[address]
                self.assertEqual(
                    (row["size"], row["status"], row["module"]),
                    (size, "sdk_asm", "psyq/crt"),
                )


if __name__ == "__main__":
    unittest.main()
