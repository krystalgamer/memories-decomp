import json
from pathlib import Path
import unittest


ROOT = Path(__file__).resolve().parents[3]


class GroupedDisplayObjectQuadRendererTests(unittest.TestCase):
    def setUp(self) -> None:
        manifest = json.loads(
            (ROOT / "config/slus_01411/matching_c.json").read_text()
        )
        self.functions = {
            item["address"]: item for item in manifest["functions"]
        }

    def test_quad_renderers_have_one_owner(self) -> None:
        owner = "src/game/display_object_quad_renderers.c"
        self.assertEqual(self.functions["0x80040DD8"]["source"], owner)
        self.assertEqual(self.functions["0x80041068"]["source"], owner)
        self.assertFalse((ROOT / "src/game/func_80040DD8.c").exists())
        self.assertFalse((ROOT / "src/game/func_80041068.c").exists())

    def test_quad_renderers_remain_in_image_order(self) -> None:
        source = (
            ROOT / "src/game/display_object_quad_renderers.c"
        ).read_text()
        self.assertLess(
            source.index("void DisplayObject_RenderGouraudQuadList("),
            source.index(
                "void DisplayObject_RenderTexturedGouraudQuadList("
            ),
        )

    def test_renderer_table_uses_semantic_names(self) -> None:
        table = (
            ROOT / "src/game/display_object_list_renderer_table.c"
        ).read_text()
        self.assertIn("gDisplayObject_ListRenderers", table)
        self.assertLess(
            table.index("DisplayObject_RenderGouraudQuadList"),
            table.index("DisplayObject_RenderTexturedGouraudQuadList"),
        )


if __name__ == "__main__":
    unittest.main()
