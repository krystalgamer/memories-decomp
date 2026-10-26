import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]
SOURCE = ROOT / "src/overlays/duel_effects/row_attack_effects.c"


class DuelEffect23Tests(unittest.TestCase):
    def test_spanish_registration_reuses_the_complete_french_source(self) -> None:
        entries = []
        for region in ("sles_03948", "sles_03951"):
            manifest = json.loads(
                (ROOT / f"config/{region}/overlays/duel_effects_matching_c.json").read_text()
            )
            entries.append([row for row in manifest["functions"] if row["address"] == "0x80152048"])
        self.assertEqual(entries[0], entries[1])
        self.assertEqual(entries[1], [{
            "address": "0x80152048", "size": "0xE7C",
            "source": "src/overlays/duel_effects/row_attack_effects.c",
            "profile": "gcc_2_8_1_g0_split",
        }])


if __name__ == "__main__":
    unittest.main()
