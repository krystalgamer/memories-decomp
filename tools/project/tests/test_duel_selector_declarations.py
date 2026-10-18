from pathlib import Path
import re
import unittest

from c_type_definitions import mask_comments_and_literals


ROOT = Path(__file__).resolve().parents[3]
SYMBOLS = ("gDuel_bOpponentID", "gDuel_bTerrain")
DECLARATION = re.compile(
    r"\bextern\s+[^;]*\b(?P<name>gDuel_bOpponentID|gDuel_bTerrain)\b[^;]*;"
)


class DuelSelectorDeclarationTests(unittest.TestCase):
    def test_c_sources_use_owner_headers(self) -> None:
        declarations = []
        for path in sorted((ROOT / "src").rglob("*.c")):
            text = mask_comments_and_literals(
                path.read_text(encoding="utf-8", errors="surrogateescape")
            )
            for match in DECLARATION.finditer(text):
                declarations.append(
                    (
                        str(path.relative_to(ROOT)),
                        match.group("name"),
                        text.count("\n", 0, match.start()) + 1,
                    )
                )

        self.assertEqual(declarations, [])




if __name__ == "__main__":
    unittest.main()
