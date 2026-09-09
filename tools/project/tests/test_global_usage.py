from __future__ import annotations

from pathlib import Path
import sys
import tempfile
import unittest

REPOSITORY = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(REPOSITORY / "tools/project"))

from global_usage import load_declarations


class SharedDeclarationTests(unittest.TestCase):
    def setUp(self) -> None:
        if Path.cwd().resolve() != REPOSITORY:
            raise RuntimeError("run these tests from the repository root")
        temporary = tempfile.TemporaryDirectory(
            prefix="global-usage-test-", dir=REPOSITORY / "tmp"
        )
        self.addCleanup(temporary.cleanup)
        self.header = Path(temporary.name) / "unmatched.h"

    def test_loads_shared_global_widths_and_arrays(self) -> None:
        self.header.write_text(
            "extern u8 D_80010000;\n"
            "extern u16 D_80010002[];\n"
            "void func_80020000(void);\n",
            encoding="utf-8",
        )

        widths, arrays = load_declarations(
            self.header, set()
        )

        self.assertEqual(
            widths, {"D_80010000": "8", "D_80010002": "16"}
        )
        self.assertEqual(arrays, {"D_80010002"})


if __name__ == "__main__":
    unittest.main()
