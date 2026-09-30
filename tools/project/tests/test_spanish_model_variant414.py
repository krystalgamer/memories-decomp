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

    def test_bindings_cover_fallback_functions_as_well_as_c(self):
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = (family435.ROOT / module["linker_symbols"]).read_text()
            for name, address in re.findall(r"(\w+) = (0x[0-9A-F]+); // type:func absolute:true", symbols):
                self.assertIn(f"{name} = {address};", bindings)
