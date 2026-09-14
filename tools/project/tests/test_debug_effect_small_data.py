from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DebugEffectSmallDataTests(unittest.TestCase):
    def test_last_game_small_data_blob_is_c_owned(self) -> None:
        split = (ROOT / "config/slus_01411/split.yaml").read_text()
        self.assertIn(
            "- [0x8b72a, .sdata, game/debug_effect_screen]",
            split,
        )
        self.assertNotIn("initialized_data_8009af2a", split)
        self.assertNotIn("- [0x8b72e, pad]", split)

    def test_preview_state_owns_the_complete_six_byte_window(self) -> None:
        source = (ROOT / "src/game/debug_effect_screen.c").read_text()
        self.assertIn(
            "u8 gDebugEffect_abPreviewState[6] "
            '__attribute__((section(".sdata"))) = {0};',
            source,
        )
        self.assertIn(
            "#define gDebugEffect_abCoordinates "
            "(&gDebugEffect_abPreviewState[2])",
            source,
        )
        self.assertIn(
            "#define gDebugEffect_bPage gDebugEffect_abPreviewState[4]",
            source,
        )

    def test_obsolete_overlapping_linker_aliases_are_retired(self) -> None:
        linker = (ROOT / "config/slus_01411/c_symbols.ld").read_text()
        unmatched = (ROOT / "src/unmatched.h").read_text()
        for symbol in ("D_8009AF2A", "D_8009AF2C", "D_8009AF2D"):
            self.assertNotIn(symbol, linker)
            self.assertNotIn(symbol, unmatched)


if __name__ == "__main__":
    unittest.main()
