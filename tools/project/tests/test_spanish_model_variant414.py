import re

from tools.project.tests import test_french_model_variant414 as family414
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant414Tests(family414.FrenchModelVariant414Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = (
        (0x1228, 1896, "bands", "func_8013C228"),
        (0x1990, 1252, "sheets", "func_8013C994"),
        (0x1E74, 1392, "webs", "func_8013CE7C"),
        (0x23E4, 776, "spokes", "func_8013D3F0"),
        (0x26EC, 892, "rings", "func_8013D6FC"),
        (0x2A68, 868, "quad", "func_8013DA7C"),
    )
    reachable_helpers = {0x1228, 0x1990, 0x1E74}
    source_directories = {"bands": "spanish_model_variant"}
    standalone_helpers = {"bands"}
    entry_anchors = {
        **family414.FrenchModelVariant414Tests.entry_anchors,
        0x1230: 0x00809821, 0x126C: 0x26720D9C, 0x1284: 0x8E620F54,
        0x1288: 0x86630F6C, 0x128C: 0x8C420044, 0x12F8: 0x267404E0,
        0x1328: 0x001180C0, 0x132C: 0x02148021, 0x135C: 0x26020028,
        0x1388: 0x26100050, 0x1554: 0x001030C0, 0x155C: 0x24C50028,
        0x1564: 0x24C60050, 0x156C: 0x00108080, 0x1570: 0x2602008C,
        0x15D8: 0x2694011C, 0x15DC: 0x267404E0, 0x15FC: 0x96220078,
        0x1614: 0x96020078, 0x162C: 0x9622008C, 0x1644: 0x9602008C,
        0x16EC: 0x8E2200F4, 0x170C: 0xAE200108,
        0x173C: 0x960200A0, 0x1748: 0x860200A2,
        0x176C: 0x9602008C, 0x1778: 0x8602008E,
    }

    def test_band_preserves_indexed_next_column_loads(self):
        source = (family435.ROOT / "src/overlays/spanish_model_variant/variant414_bands.c").read_text()
        for expression in ("poly->x1 = band->sc[j + 1];", "poly->y1 = band->sc[j + 1] >> 16;",
                           "poly->x3 = band->sb[j + 1];", "poly->y3 = band->sb[j + 1] >> 16;"):
            self.assertIn(expression, source)
        for expression in ("col->sc[1]", "col->sb[1]"):
            self.assertNotIn(expression, source)
        bindings = (family435.ROOT / self.modules[0]["linker_symbols"]).read_text()
        for name, address in (("RotTransPers3", 0x80087898), ("rcos", 0x800866F8)):
            self.assertIn(f"{name} = 0x{address:X};", bindings)
            self.assertNotIn(f"func_french_{address:X}", bindings)

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)
