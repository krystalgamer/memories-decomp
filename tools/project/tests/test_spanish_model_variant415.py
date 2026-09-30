import re

from tools.project.tests import test_french_model_variant415 as family415
from tools.project.tests import test_french_model_variant435 as family435
from tools.project.progress import load_spanish_overlay_inventories


class SpanishModelVariant415Tests(family415.FrenchModelVariant415Tests):
    region = "spain"
    module_prefix = "spanish"
    config_name = "sles_03951"
    resident_name = "SLES_039.51"
    load_inventories = staticmethod(load_spanish_overlay_inventories)
    helpers = ((0x18A4, 1920, "bands", "func_8013C8A4"), *family415.FrenchModelVariant415Tests.helpers)
    reachable_helpers = {0x18A4, *family415.FrenchModelVariant415Tests.reachable_helpers}
    source_directories = {"bands": "spanish_model_variant"}
    standalone_helpers = {"bands"}
    entry_anchors = {
        **family415.FrenchModelVariant415Tests.entry_anchors,
        0x18A4: 0x27BDFED8, 0x18AC: 0x00809821, 0x18E8: 0x26711854,
        0x18EC: 0x8E6319EC, 0x1900: 0x86621A18, 0x1948: 0x267404E0,
        0x195C: 0x27A800C8, 0x1960: 0xAFA800FC,
        0x1980: 0x001280C0, 0x19B4: 0x26020048, 0x19E0: 0x26100090,
        0x1BB4: 0x24C50048, 0x1BBC: 0x24C60090,
        0x1BC4: 0x00108080, 0x1BC8: 0x260200FC, 0x1BD4: 0x26020120,
        0x1BE0: 0x27A200F0, 0x1BF0: 0x000310C0,
        0x1BF4: 0x00431021, 0x1BF8: 0x00021080, 0x1BFC: 0x260700D8,
        0x1C00: 0x8FA800FC, 0x1C14: 0xAFA2001C, 0x1C2C: 0x28630009,
        0x1C34: 0xAE0201A4, 0x1C48: 0x269401C8,
        0x1D84: 0x27A300C8, 0x1D8C: 0xAEA00000,
        0x1DCC: 0x96020120, 0x1DD8: 0x86020122,
        0x1DFC: 0x960200FC, 0x1E08: 0x860200FE,
    }

    def test_band_indexed_loads_and_stack_owned_flags(self):
        source = (family435.ROOT / "src/overlays/spanish_model_variant/variant415_bands.c").read_text()
        for expression in ("poly->x1 = band->sc[j + 1];", "poly->y1 = band->sc[j + 1] >> 16;",
                           "poly->x3 = band->sb[j + 1];", "poly->y3 = band->sb[j + 1] >> 16;",
                           "PSXLONG flag[1][9];", "&flag[i][j]"):
            self.assertIn(expression, source)
        for expression in ("col->sc[1]", "col->sb[1]", "band->flag"):
            self.assertNotIn(expression, source)

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)
