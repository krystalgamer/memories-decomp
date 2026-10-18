from tools.project.tests import test_french_model_variant423_funnel as family423


class FrenchModelVariant425FunnelTests(family423.FrenchModelVariant423FunnelTests):
    source_stem = "src/overlays/french_model_variant/variant425_funnel"
    module_names = {"french_model_variant_0_stage9_slot0", "french_model_variant_0_stage10_slot1"}
    function_offset = 0x3360
    function_size = 0x5A8
    ledger_name = "french-model-variant425-funnel-attempts.csv"
    ledger_row_count = 2
    layout_defines = ("#define MODEL_VARIANT425_FUNNEL",)
    layout_fields = {
        ("Funnel423", "outer"): 0x88, ("Funnel423", "size"): 0x110,
        ("Funnel423State", "sheets"): 0xE1C,
        ("Funnel423State", "funnels"): 0xFB0,
        ("Funnel423State", "quad"): 0x155C,
        ("Funnel423State", "position"): 0x1754,
        ("Funnel423State", "direction"): 0x175C,
        ("Funnel423State", "frame"): 0x1788,
        ("Funnel423State", "step"): 0x1794,
        ("Funnel423State", "size"): 0x17B4,
        ("Funnel423State", "fade"): 0x17C4,
        ("Funnel423State", "inner_color"): 0x17F0,
        ("Funnel423State", "outer_color"): 0x17F4,
        ("Funnel423State", "spin"): 0x17F8,
        ("Funnel423State", "phase"): 0x180C,
    }
