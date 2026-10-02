import re

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant445 as family445
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant445Tests(family445.FrenchModelVariant445Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = tuple(helper for helper in family445.FrenchModelVariant445Tests.helpers
                    if helper[0] != 0x2BD8)
    reachable_helpers = family445.FrenchModelVariant445Tests.reachable_helpers - {0x2BD8}
    source_directories = {"bands": "spanish_model_variant"}
    standalone_helpers = frozenset({"bands"})
    entry_anchors = {
        **family445.FrenchModelVariant445Tests.entry_anchors,
        0x1034: 0x27BDFED8, 0x103C: 0x00809821,
        0x1068: 0x86641562, 0x106C: 0x86651560,
        0x1078: 0x26711418, 0x107C: 0x8E63157C,
        0x1090: 0x866315B0, 0x10CC: 0x26740AB0,
        0x10E0: 0x27A800C8, 0x10E4: 0xAFA800FC,
        0x1104: 0x001280C0, 0x1138: 0x26020048,
        0x1164: 0x26100090, 0x118C: 0x8E62153C,
        0x1198: 0x8E621540, 0x11A4: 0x8E621544,
        0x11B0: 0x8E631550, 0x11B4: 0x8E6215B4,
        0x11F8: 0x8E631554, 0x1240: 0x8E631558,
        0x1338: 0x24C50048, 0x1340: 0x24C60090,
        0x1348: 0x00108080, 0x134C: 0x260200FC,
        0x1358: 0x26020120, 0x1364: 0x27A200F0,
        0x1374: 0x000310C0, 0x1378: 0x00431021,
        0x137C: 0x00021080, 0x1380: 0x260700D8,
        0x1384: 0x8FA800FC, 0x1398: 0xAFA2001C,
        0x13B0: 0x28630009, 0x13B8: 0xAE0201A4,
        0x13CC: 0x269401C8, 0x1508: 0x27A300C8,
        0x1510: 0xAEA00000, 0x1550: 0x96020120,
        0x155C: 0x86020122, 0x1580: 0x960200FC,
        0x158C: 0x860200FE, 0x1690: 0x8E6315BC,
        0x16B4: 0x8E631590, 0x16B8: 0x8E621580,
        0x1760: 0xA66215B0, 0x1774: 0xAE6215BC,
    }

    def test_band_indexed_loads_and_stack_owned_flags(self):
        source = (family435.ROOT / "src/overlays/spanish_model_variant/variant445_bands.c").read_text()
        for expression in ("poly->x1 = band->sc[j + 1];", "poly->y1 = band->sc[j + 1] >> 16;",
                           "poly->x3 = band->sb[j + 1];", "poly->y3 = band->sb[j + 1] >> 16;",
                           "PSXLONG flag[1][9];", "&flag[i][j]"):
            self.assertIn(expression, source)
        for expression in ("col->sc[1]", "col->sb[1]", "band->flag"):
            self.assertNotIn(expression, source)
        for module in self.modules:
            self.assertIn("rcos = 0x800866F8;", (family435.ROOT / module["linker_symbols"]).read_text())

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)
