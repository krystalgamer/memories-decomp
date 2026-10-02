import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant445 as family445
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant445Tests(family445.FrenchModelVariant445Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    binding_count = 37
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


class SpanishModelVariant445SpiralTests(family445.FrenchModelVariant445SpiralTests):
    family_class = SpanishModelVariant445Tests
    keep_legacy_alias = True

    def test_spiral_colors_original_context_and_narrowed_growth(self):
        family = self.family_class()
        family.setUp()
        archive_path = family435.ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive_path.exists():
            self.skipTest("legal Spanish MODEL input required")
        anchors = {
            0xC: 0x00809821, 0x14: 0x0260B021, 0x48: 0x26D71418,
            0x84: 0xAFB60080, 0x8C: 0x04A0030E,
            0x23C: 0x0C020BBA, 0x240: 0x02E02021,
            0x3B0: 0x8FB80080, 0x3B8: 0x27040004, 0x3BC: 0x03001821,
            0x3C8: 0xA0800058, 0x3CC: 0xA0800059, 0x3D0: 0xA080005A,
            0x3D4: 0xA0800050, 0x3D8: 0xA0800051, 0x3E0: 0xA0800052,
            0x3EC: 0x90420000, 0x3F4: 0xA0620058, 0x400: 0x90420001,
            0x408: 0xA0620059, 0x414: 0x90420002, 0x41C: 0xA062005A,
            0x428: 0x90420004, 0x430: 0xA0620050, 0x43C: 0x90420005,
            0x444: 0xA0620051, 0x450: 0x90420006, 0x458: 0xA0620052,
            0x460: 0x2A620002, 0x474: 0x28A2000C, 0x478: 0x2718007C,
            0xC70: 0x24031000, 0xC78: 0xA6C015A4, 0xC80: 0xA6C015A8,
            0xC8C: 0xA6C315B0, 0xE9C: 0x8C430030, 0xEA8: 0x0043102B,
            0xEAC: 0x14400003, 0xEF8: 0x0C016FC9, 0xF0C: 0x0C016FC9,
            0xF14: 0xAEC21588, 0x2FC0: 0x00021343, 0x2FC8: 0x24480004,
            0x3540: 0x00021180, 0x3548: 0xA50215A8, 0x354C: 0x00021400,
            0x3550: 0x00021403, 0x3554: 0x28420400, 0x3560: 0xA50215A8,
        }
        with archive_path.open("rb") as archive:
            for module in family.modules:
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(hashlib.sha256(data).hexdigest(), module["sha256"])
                for offset, word in anchors.items():
                    self.assertEqual(struct.unpack_from("<I", data, offset)[0], word,
                                     (module["name"], hex(offset)))
