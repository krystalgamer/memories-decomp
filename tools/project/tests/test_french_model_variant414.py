from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant414Tests(family435.FrenchModelVariant435Tests):
    family = 414
    source_family = 397
    module_count = 24
    distinct_images = 22
    binding_count = 36
    tail_start = 0x2DCC
    spans = ((4, 0x1228), (0x1228, 0x1990), (0x1990, 0x1E74),
             (0x1E74, 0x23E4), (0x23E4, 0x26EC), (0x26EC, 0x2A68),
             (0x2A68, 0x2DCC))
    helpers = ((0x1990, 1252, "sheets", "func_8013C994"),
               (0x23E4, 776, "spokes", "func_8013D3F0"),
               (0x26EC, 892, "rings", "func_8013D6FC"),
               (0x2A68, 868, "quad", "func_8013DA7C"))
    reachable_helpers = {0x1990}
    local_call_targets = {0x1228, 0x1990, 0x1E74}
    models_by_stage = ((7, (2, 20, 87, 108, 138, 193, 573)),
                       (9, (152, 168, 170, 388, 427)))
    entry_anchors = {0x0C: 0x00809021, 0x14: 0x0240B021, 0x20: 0x26D805FC,
                     0x28: 0x26D8072C, 0x30: 0x26D80A8C, 0x38: 0x26D80CCC,
                     0x6A4: 0x27180090, 0x978: 0x27180098, 0xC8C: 0x27180090,
                     0xDB4: 0x27180090, 0xDB0: 0x2BC20004, 0xDD4: 0xAEC00F74}
