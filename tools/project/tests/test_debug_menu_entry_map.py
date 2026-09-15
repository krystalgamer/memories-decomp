from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]

EXPECTED_PRIMARY_STEPS = [
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
]


def primary_step_entries(text: str) -> list[str]:
    match = re.search(
        r"gDebugMenu_apfnPrimaryPageSteps\[\]\)\(void\) = \{"
        r"(?P<body>.*?)\n\};",
        text,
        re.DOTALL,
    )
    if match is None:
        raise AssertionError("primary debug-menu step table not found")
    body = re.sub(r"/\*.*?\*/|//[^\n]*", "", match.group("body"), flags=re.DOTALL)
    return [entry.strip() for entry in body.split(",") if entry.strip()]


class DebugMenuEntryMapTests(unittest.TestCase):
    def assert_primary_step_order(self, text: str) -> None:
        entries = primary_step_entries(text)
        self.assertEqual(len(entries), 21)
        self.assertEqual(entries, EXPECTED_PRIMARY_STEPS)

    def test_primary_step_order_matches_observed_labels(self) -> None:
        self.assert_primary_step_order(
            (ROOT / "src/game/frontend_step_tables.c").read_text()
        )

    def test_primary_step_order_rejects_complete_table_mutations(self) -> None:
        text = (ROOT / "src/game/frontend_step_tables.c").read_text()
        declaration = "void (*gDebugMenu_apfnPrimaryPageSteps"
        prefix, primary = text.split(declaration, 1)

        def mutate_primary(old: str, new: str) -> str:
            return prefix + declaration + primary.replace(old, new, 1)

        mutations = {
            "extra address": mutate_primary(
                "    DebugMenu_Exit,                  /* EXIT */",
                "    func_80000000,\n"
                "    DebugMenu_Exit,                  /* EXIT */",
            ),
            "non-prefixed handler": mutate_primary(
                "    DebugMenu_ResetEntryState,",
                "    func_80000000,",
            ),
            "null entry": mutate_primary(
                "    DebugMenu_ResetEntryState,",
                "    0,",
            ),
            "reordered entries": mutate_primary(
                "    DebugMenu_UpdateCampaignEntry,   /* Campaign */\n"
                "    DebugMenu_EnterDuel,             /* DUEL */",
                "    DebugMenu_EnterDuel,             /* DUEL */\n"
                "    DebugMenu_UpdateCampaignEntry,   /* Campaign */",
            ),
        }
        for name, mutated in mutations.items():
            with self.subTest(name=name):
                with self.assertRaises(AssertionError):
                    self.assert_primary_step_order(mutated)

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
