from pathlib import Path
import re
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DebugEffectSmallDataTests(unittest.TestCase):
    @staticmethod
    def initialized_data_boundaries(split: str) -> tuple[int, int]:
        start_match = re.search(
            r"- \[(0x[0-9A-Fa-f]+), data, initialized_data_80091958\]",
            split,
        )
        end_match = re.search(
            r"- \[(0x[0-9A-Fa-f]+), \.sdata, game/main_services\]",
            split,
        )
        if start_match is None or end_match is None:
            raise AssertionError("initialized-data boundaries are missing")
        return int(start_match.group(1), 0), int(end_match.group(1), 0)

    def assert_complete_sdk_initialized_data(self, split: str) -> None:
        start, end = self.initialized_data_boundaries(split)
        self.assertEqual(start, 0x82158)
        self.assertEqual(end, 0x8B708)
        self.assertEqual(end - start, 0x95B0)

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

    def test_sdk_initialized_data_has_complete_measured_extent(self) -> None:
        split = (ROOT / "config/slus_01411/split.yaml").read_text()
        self.assert_complete_sdk_initialized_data(split)

    def test_sdk_initialized_data_rejects_shifted_boundaries(self) -> None:
        split = (ROOT / "config/slus_01411/split.yaml").read_text()
        variants = (
            split.replace(
                "- [0x82158, data, initialized_data_80091958]",
                "- [0x8215C, data, initialized_data_80091958]",
            ),
            split.replace(
                "- [0x8b708, .sdata, game/main_services]",
                "- [0x8b704, .sdata, game/main_services]",
            ),
        )
        for variant in variants:
            with self.subTest():
                with self.assertRaises(AssertionError):
                    self.assert_complete_sdk_initialized_data(variant)


if __name__ == "__main__":
    unittest.main()
