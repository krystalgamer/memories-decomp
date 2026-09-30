from tools.project.tests import test_french_model_variant418 as family418


class FrenchModelVariant433Tests(family418.FrenchModelVariant418Tests):
    family = 433
    source_family = 416
    module_count = 4
    distinct_images = 4
    tail_start = 0x30B8
    spans = ((4, 0x1050), (0x1050, 0x1810), (0x1810, 0x1C64),
             (0x1C64, 0x21CC), (0x21CC, 0x26C8), (0x26C8, 0x29D8),
             (0x29D8, 0x2D54), (0x2D54, 0x30B8))
    helpers = ((0x26C8, 784, "spokes", "func_8013D6D4"),
               (0x29D8, 892, "rings", "func_8013D9E8"),
               (0x2D54, 868, "quad", "func_8013DD68"))
    local_call_targets = {0x1050, 0x1810, 0x1C64, 0x21CC}
    models_by_stage = ((7, (180, 440)),)
    entry_anchors = {
        offset + (0x24 if offset >= 0x498 else 0): word
        for offset, word in family418.FrenchModelVariant418Tests.entry_anchors.items()
    }
    entry_anchors.update({
        0x20: 0x26D810B0, 0x28: 0x26D81278, 0x30: 0x26D815D8,
        0x38: 0x26D81818, 0x7A4: 0x2AE20003,
    })
