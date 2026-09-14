from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DebugMenuEntryMapTests(unittest.TestCase):
    def test_primary_step_order_matches_observed_labels(self) -> None:
        text = (ROOT / "src/game/frontend_step_tables.c").read_text()
        match = re.search(
            r"gDebugMenu_apfnPrimaryPageSteps\[\]\)\(void\) = \{"
            r"(?P<body>.*?)\n\};",
            text,
            re.DOTALL,
        )
        self.assertIsNotNone(match)
        entries = re.findall(r"^\s+(DebugMenu_\w+),", match.group("body"), re.MULTILINE)
        self.assertEqual(
            entries,
            [
                "DebugMenu_ResetEntryState",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_UpdateCampaignEntry",
                "DebugMenu_EnterDuel",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_UpdateSoundEntry",
                "DebugMenu_UpdateBustUpEntry",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_EnterDeckEditor",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_UpdateTitleEntry",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_UpdateMovieEntry",
                "DebugMenu_UpdateLoadEntry",
                "DebugMenu_UpdateSaveEntry",
                "DebugMenu_UpdateTradeEntry",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_EnterMappedMode",
                "DebugMenu_Exit",
            ],
        )

    def test_entry_constants_cover_both_columns(self) -> None:
        text = (ROOT / "src/game/frontend_debug_tables.h").read_text()
        values = [
            int(value)
            for value in re.findall(
                r"^#define DEBUG_MENU_ENTRY_(?!COUNT\b)\w+ (\d+)$",
                text,
                re.MULTILINE,
            )
        ]
        self.assertEqual(values, list(range(20)))


if __name__ == "__main__":
    unittest.main()
