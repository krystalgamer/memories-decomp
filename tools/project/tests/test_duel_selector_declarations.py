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

    def test_opponent_owner_exposes_all_codegen_views(self) -> None:
        text = (ROOT / "src/game/ai_opponent_data.h").read_text()
        for declaration in (
            "extern s8 gDuel_bOpponentID[AI_OPPONENT_DATA_FIELD_COUNT];",
            "extern s8 gDuel_bOpponentID[];",
            'extern s8 gDuel_bOpponentID __attribute__((section(".data")));',
        ):
            self.assertIn(declaration, text)

    def test_terrain_owner_exposes_all_codegen_views(self) -> None:
        text = (ROOT / "src/game/duel_terrain_boost.h").read_text()
        for declaration in (
            "extern u8 gDuel_bTerrain[];",
            'extern u8 gDuel_bTerrain __attribute__((section(".data")));',
        ):
            self.assertIn(declaration, text)


if __name__ == "__main__":
    unittest.main()
