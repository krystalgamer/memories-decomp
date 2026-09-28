from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class DuelPolygonVerticesTests(unittest.TestCase):
    def test_height_lifetime_and_cursor_preserve_the_exact_lowering(self) -> None:
        source = (ROOT / "src/overlays/duel_effects/polygon_vertices.c").read_text()
        self.assertIn("s32 level, s32 offset", source)
        self.assertIn("for (i = 0; i < count; i++)", source)
        angle = source.index("angle = (4096 / count) * i")
        height = source.index("height = level - offset;")
        cosine = source.index("vertices->vx = width * ccos(angle)")
        self.assertLess(angle, height)
        self.assertLess(height, cosine)
        self.assertIn("vertices->vy = height;", source)
        self.assertIn("vertices++;", source)
        self.assertNotIn("->pad", source)
        self.assertNotIn("asm", source)


if __name__ == "__main__":
    unittest.main()
