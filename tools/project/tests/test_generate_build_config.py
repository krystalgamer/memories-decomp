from __future__ import annotations

import json
from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

import generate_build_config
from generate_build_config import GenerationError


class DataSourceTests(unittest.TestCase):
    """The manifest that hands initialized data to C translation units."""

    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="generate-config-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name) / "workspace"
        (self.root / "config/slus_01411").mkdir(parents=True)
        (self.root / "src/game").mkdir(parents=True)
        (self.root / "src/game/file_names.c").write_text("/* fixture */\n")
        self.write_units([{"source": "src/game/file_names.c", "profile": "g0"}])
        self.segments = [
            {
                "name": "initialized_data",
                "subsegments": [
                    [0x80EE0, "data", "initialized_data"],
                    [0x80F04, ".data", "game/file_names"],
                    [0x80FAC, "data", "initialized_data_1"],
                ],
            }
        ]

    def write_units(self, units: list[dict[str, str]]) -> None:
        (self.root / "config/slus_01411/data_c.json").write_text(
            json.dumps({"schema": 1, "units": units}) + "\n"
        )

    def profiles(self) -> dict[str, str]:
        return generate_build_config.load_data_profiles(self.root, {"g0"})

    def collect(self, text_sources: list[dict[str, object]] | None = None):
        return generate_build_config.collect_data_sources(
            self.root, self.segments, self.profiles(), text_sources or []
        )

    def test_blobs_and_owned_sections_are_collected_in_order(self) -> None:
        self.assertEqual(
            self.collect(),
            [
                {
                    "kind": "asm",
                    "source": "tmp/splat/asm/data/initialized_data.data.s",
                    "object": "initialized_data.o",
                },
                {
                    "kind": "c",
                    "source": "src/game/file_names.c",
                    "object": "c_game_file_names.o",
                    "profile": "g0",
                },
                {
                    "kind": "asm",
                    "source": "tmp/splat/asm/data/initialized_data_1.data.s",
                    "object": "initialized_data_1.o",
                },
            ],
        )

    def test_generated_segments_are_skipped(self) -> None:
        self.segments[0]["name"] = "text"
        self.write_units([])
        self.assertEqual(self.collect(), [])

    def test_owned_section_without_a_profile_is_rejected(self) -> None:
        self.write_units([])
        with self.assertRaises(GenerationError) as error:
            self.collect()
        self.assertIn("data_c.json profile", str(error.exception))

    def test_unmapped_manifest_entry_is_rejected(self) -> None:
        (self.root / "src/game/other.c").write_text("/* fixture */\n")
        self.write_units(
            [
                {"source": "src/game/file_names.c", "profile": "g0"},
                {"source": "src/game/other.c", "profile": "g0"},
            ]
        )
        with self.assertRaises(GenerationError) as error:
            self.collect()
        self.assertIn("unmapped units", str(error.exception))

    def test_a_text_unit_cannot_also_own_data(self) -> None:
        text_sources = [{"kind": "c", "source": "src/game/file_names.c"}]
        with self.assertRaises(GenerationError) as error:
            self.collect(text_sources)
        self.assertIn("cannot own", str(error.exception))

    def test_unknown_profile_is_rejected(self) -> None:
        self.write_units([{"source": "src/game/file_names.c", "profile": "g9"}])
        with self.assertRaises(GenerationError) as error:
            self.collect()
        self.assertIn("unknown profile", str(error.exception))


if __name__ == "__main__":
    unittest.main()
