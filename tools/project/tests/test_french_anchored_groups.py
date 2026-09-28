import json
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]
EMPTY_GROUPS = (
    ("src/game/func_800283EC.c", 0x80028474, 1),
    ("src/game/european/library_runtime_8002BAAC.c", 0x8002BC30, 1),
    ("src/game/noop_callbacks.c", 0x8002C734, 2),
    ("src/game/european/script_noop_halt15.c", 0x8002F694, 2),
)


class FrenchAnchoredGroupTests(unittest.TestCase):
    @staticmethod
    def manifest(region: str) -> list[dict]:
        return json.loads(
            (ROOT / "config" / region / "matching_c.json").read_text()
        )["functions"]

    def test_complete_groups_reuse_spanish_profiles_sizes_and_order(self) -> None:
        spanish = self.manifest("sles_03951")
        french = self.manifest("sles_03948")
        groups = EMPTY_GROUPS + (
            ("src/game/spanish/script_image_objects.c", 0x8002DFE8, 4),
            ("src/game/func_8004A6D8.c", 0x8004AB68, 1),
        )
        for source, address, count in groups:
            with self.subTest(source=source):
                es = [row for row in spanish if row["source"] == source]
                fr = [row for row in french if row["source"] == source]
                self.assertEqual(len(es), count)
                self.assertEqual(len(fr), count)
                self.assertEqual(
                    [(row["size"], row["profile"]) for row in es],
                    [(row["size"], row["profile"]) for row in fr],
                )
                for row in fr:
                    self.assertEqual(int(row["address"], 0), address)
                    address += int(row["size"], 0)

    def test_both_complete_neighbors_anchor_each_empty_group(self) -> None:
        spanish = self.manifest("sles_03951")
        french = self.manifest("sles_03948")
        for source, address, count in EMPTY_GROUPS:
            start = next(i for i, row in enumerate(spanish) if row["source"] == source)
            for neighbor, before in (
                (spanish[start - 1], True),
                (spanish[start + count], False),
            ):
                with self.subTest(source=source, before=before):
                    es = [row for row in spanish if row["source"] == neighbor["source"]]
                    fr = [row for row in french if row["source"] == neighbor["source"]]
                    self.assertEqual(
                        [(row["size"], row["profile"]) for row in es],
                        [(row["size"], row["profile"]) for row in fr],
                    )
                    anchor = fr[es.index(neighbor)]
                    boundary = int(anchor["address"], 0)
                    if before:
                        boundary += int(anchor["size"], 0)
                    self.assertEqual(boundary, address if before else address + count * 8)

    def test_previously_external_functions_are_not_absolute_aliases(self) -> None:
        linker = (ROOT / "config/sles_03948/link_symbols.ld").read_text()
        for name in ("func_8002BAAC", "func_8004A6D8"):
            self.assertIsNone(re.search(rf"^\s*{name}\s*=", linker, re.M))


if __name__ == "__main__":
    unittest.main()
