from tools.project.tests import test_spanish_model_variant432 as spanish

from progress import load_french_overlay_inventories


class FrenchModelVariant432Tests(spanish.SpanishModelVariant432Tests):
    config_path = spanish.ROOT / "config/sles_03948"
    region = "french"
    archive_path = "game/france/DATA/MODEL.MRG"
    load_inventories = staticmethod(load_french_overlay_inventories)
    matching_helpers = ((0x134C, 848, "variant432_draw"),)
