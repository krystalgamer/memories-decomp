from __future__ import annotations

import json
from pathlib import Path
import sys
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from build_regional_baseline import (  # noqa: E402
    RegionalBuildConfiguration,
    owned_data_sections,
    owned_data_units,
)


class OwnedDataUnitsTests(unittest.TestCase):
    def setUp(self) -> None:
        (REPOSITORY / "tmp").mkdir(exist_ok=True)
        temporary = tempfile.TemporaryDirectory(dir=REPOSITORY / "tmp")
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        region = self.root / "config/region"
        region.mkdir(parents=True)
        (region / "split.yaml").write_text(
            "segments:\n"
            "  - name: data\n"
            "    subsegments:\n"
            "      - [0x1000, .data, game/table]\n"
            "      - [0x1100, .data, game/plain]\n",
            encoding="utf-8",
        )
        (region / "data_c.json").write_text(
            json.dumps(
                {
                    "units": [
                        {
                            "source": "src/game/table.c",
                            "profile": "p",
                            "defines": ["TABLE_SIZE=4"],
                        },
                        {"source": "src/game/plain.c", "profile": "p"},
                    ]
                }
            ),
            encoding="utf-8",
        )
        self.configuration = RegionalBuildConfiguration(
            splat_directory="build",
            output_directory="out",
            matching_config="config/region/matching_c.json",
            output_name="IMAGE",
            linker_script="image.ld",
            asm_directory="asm",
            expected_size=0,
        )

    def write_region_defines(self, defines: list[str]) -> None:
        (self.root / "config/region/matching_c.json").write_text(
            json.dumps({"schema": 1, "defines": defines, "functions": []}),
            encoding="utf-8",
        )

    def test_units_carry_region_then_entry_defines(self) -> None:
        self.write_region_defines(["VERSION_EUROPE"])
        units = owned_data_units(self.root, self.configuration, [])
        self.assertEqual(
            {unit["source"]: unit.get("defines") for unit in units},
            {
                "src/game/plain.c": ["VERSION_EUROPE"],
                "src/game/table.c": ["VERSION_EUROPE", "TABLE_SIZE=4"],
            },
        )

    def test_read_only_and_block_style_listings_are_owned(self) -> None:
        """A main-segment .rodata unit counts, written in either YAML style."""
        (self.root / "config/region/split.yaml").write_text(
            "segments:\n"
            "- name: main\n"
            "  subsegments:\n"
            "  - - 0x800\n"
            "    - .rodata\n"
            "    - game/glyphs\n"
            "  - [0x838, .rodata, game/text_unit]\n"
            "  - [0x843, pad]\n",
            encoding="utf-8",
        )
        self.assertEqual(
            owned_data_sections(self.root, self.configuration),
            {
                "build/build/src/game/glyphs.o(.rodata)",
                "build/build/src/game/text_unit.o(.rodata)",
            },
        )

    def test_units_without_any_defines_carry_none(self) -> None:
        self.write_region_defines([])
        units = owned_data_units(self.root, self.configuration, [])
        self.assertEqual(
            {unit["source"]: unit.get("defines") for unit in units},
            {"src/game/plain.c": None, "src/game/table.c": ["TABLE_SIZE=4"]},
        )


if __name__ == "__main__":
    unittest.main()
