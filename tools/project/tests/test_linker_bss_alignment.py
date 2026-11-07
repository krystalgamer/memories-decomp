from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest


REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from build_baseline import finalize_linker_script  # noqa: E402
from build_regional_baseline import regional_linker_script  # noqa: E402

ALIGN = "        . = ALIGN(., 8);\n"

# A cut-down splat script: a small-data block, then a .bss block holding a
# compiled unit, a generated <unit>_bss_commons subsegment, a plain _commons
# subsegment covering partial storage and an extracted image.
SCRIPT = """\
    .main 0x80010000 : AT(main_ROM_START) SUBALIGN(2)
    {
        main_SBSS_START = .;
        build/src/game/early.o(.sbss);
        build/asm/data/unit_commons.sbss.o(.sbss);
        main_SBSS_END = .;
        build/src/game/late.o(.data);
        build/src/game/late.o(.bss);
        main_BSS_START = .;
        build/src/game/early.o(.bss);
        build/asm/data/unit_bss_commons.bss.o(.bss);
        build/asm/data/partial_commons.bss.o(.bss);
        build/assets/bss_image.o(.bss);
        build/src/game/late.o(.data);
        build/src/game/late.o(.bss);
        main_BSS_END = .;
    }
"""


class FinalizeLinkerScriptTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="linker-bss-alignment-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.script = self.root / "tmp/splat/slus_01411.ld"
        self.script.parent.mkdir(parents=True)
        self.script.write_text(SCRIPT, encoding="utf-8")

    def finalized(self) -> list[str]:
        finalize_linker_script(self.root)
        return self.script.read_text(encoding="utf-8").splitlines(keepends=True)

    def preceding(self, lines: list[str], placement: str) -> str:
        return lines[lines.index(placement) - 1]

    def test_aligns_compiled_units_and_generated_bss_commons(self) -> None:
        lines = self.finalized()
        self.assertEqual(
            self.preceding(lines, "        build/src/game/early.o(.bss);\n"), ALIGN
        )
        self.assertEqual(
            self.preceding(
                lines, "        build/asm/data/unit_bss_commons.bss.o(.bss);\n"
            ),
            ALIGN,
        )

    def test_leaves_partial_commons_images_and_small_data_unaligned(self) -> None:
        lines = self.finalized()
        for placement in (
            "        build/asm/data/partial_commons.bss.o(.bss);\n",
            "        build/assets/bss_image.o(.bss);\n",
            "        build/src/game/early.o(.sbss);\n",
            "        build/asm/data/unit_commons.sbss.o(.sbss);\n",
        ):
            self.assertNotEqual(self.preceding(lines, placement), ALIGN, placement)

    def test_keeps_only_the_last_listing_of_each_object(self) -> None:
        lines = self.finalized()
        self.assertEqual(lines.count("        build/src/game/late.o(.bss);\n"), 1)
        self.assertEqual(lines.count("        build/src/game/late.o(.data);\n"), 1)
        self.assertGreater(
            lines.index("        build/src/game/late.o(.bss);\n"),
            lines.index("        main_BSS_START = .;\n"),
        )

    def test_is_idempotent(self) -> None:
        first = self.finalized()
        self.assertEqual(self.finalized(), first)
        self.assertEqual(first.count(ALIGN), 3)


class RegionalLinkerScriptTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="regional-bss-alignment-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name)
        self.generated = self.root / "generated.ld"
        self.generated.write_text(SCRIPT, encoding="utf-8")

    def lines(self, owned: set[str]) -> list[str]:
        output = regional_linker_script(
            self.generated, self.root / "regional.ld", [], owned
        )
        return output.read_text(encoding="utf-8").splitlines(keepends=True)

    def test_aligns_generated_bss_commons(self) -> None:
        lines = self.lines(set())
        placement = "        build/asm/data/unit_bss_commons.bss.o(.bss);\n"
        self.assertEqual(lines[lines.index(placement) - 1], ALIGN)

    def test_aligns_owned_compiled_units_and_drops_the_rest(self) -> None:
        lines = self.lines({"build/src/game/early.o(.bss)"})
        placement = "        build/src/game/early.o(.bss);\n"
        self.assertEqual(lines[lines.index(placement) - 1], ALIGN)
        self.assertNotIn("        build/src/game/late.o(.bss);\n", lines)

    def test_leaves_partial_commons_and_images_unaligned(self) -> None:
        lines = self.lines(set())
        for placement in (
            "        build/asm/data/partial_commons.bss.o(.bss);\n",
            "        build/assets/bss_image.o(.bss);\n",
        ):
            self.assertNotEqual(lines[lines.index(placement) - 1], ALIGN, placement)
        self.assertEqual(lines.count(ALIGN), 1)


if __name__ == "__main__":
    unittest.main()
