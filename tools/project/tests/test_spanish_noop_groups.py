import json
from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]
GROUPS = (
    ("src/game/func_800283EC.c", 0x80028474, 1),
    ("src/game/european/library_runtime_8002BAAC.c", 0x8002BC30, 1),
    ("src/game/noop_callbacks.c", 0x8002C734, 2),
    ("src/game/european/script_noop_halt15.c", 0x8002F694, 2),
)


class SpanishNoopGroupTests(unittest.TestCase):
    def setUp(self) -> None:
        self.europe = self.manifest("sles_03947")
        self.spanish = self.manifest("sles_03951")

    @staticmethod
    def manifest(region: str) -> list[dict]:
        return json.loads(
            (ROOT / "config" / region / "matching_c.json").read_text()
        )["functions"]

    def test_complete_groups_keep_existing_sources_profiles_and_order(self) -> None:
        for source, address, count in GROUPS:
            with self.subTest(source=source):
                eu = [e for e in self.europe if e["source"] == source]
                es = [e for e in self.spanish if e["source"] == source]
                self.assertEqual(len(eu), count)
                self.assertEqual(len(es), count)
                self.assertEqual(
                    [(e["size"], e["profile"]) for e in es],
                    [(e["size"], e["profile"]) for e in eu],
                )
                self.assertEqual(
                    [(int(e["address"], 0), int(e["size"], 0)) for e in es],
                    [(address + 8 * i, 8) for i in range(count)],
                )

    def test_both_neighbor_groups_independently_anchor_each_empty_group(self) -> None:
        for source, address, count in GROUPS:
            with self.subTest(source=source):
                start = next(i for i, e in enumerate(self.europe) if e["source"] == source)
                for neighbor, expected in (
                    (self.europe[start - 1], address),
                    (self.europe[start + count], address + count * 8),
                ):
                    eu = [e for e in self.europe if e["source"] == neighbor["source"]]
                    es = [e for e in self.spanish if e["source"] == neighbor["source"]]
                    self.assertEqual(
                        [(e["size"], e["profile"]) for e in es],
                        [(e["size"], e["profile"]) for e in eu],
                    )
                    matched = es[eu.index(neighbor)]
                    boundary = int(matched["address"], 0)
                    if neighbor is self.europe[start - 1]:
                        boundary += int(matched["size"], 0)
                    self.assertEqual(boundary, expected)

    def test_library_function_is_not_shadowed_by_its_old_absolute_alias(self) -> None:
        linker = (ROOT / "config/sles_03951/link_symbols.ld").read_text()
        self.assertIsNone(re.search(r"^\s*func_8002BAAC\s*=", linker, re.M))


if __name__ == "__main__":
    unittest.main()
