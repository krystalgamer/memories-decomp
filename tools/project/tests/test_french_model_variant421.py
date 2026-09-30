from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant421Tests(family435.FrenchModelVariant435Tests):
    family = 421
    source_family = 404
    module_count = 12
    distinct_images = 12
    binding_count = 36
    tail_start = 0x406C
    spans = ((4, 0x11D0), (0x11D0, 0x1860), (0x1860, 0x247C),
             (0x247C, 0x2C28), (0x2C28, 0x310C), (0x310C, 0x367C),
             (0x367C, 0x398C), (0x398C, 0x3D08), (0x3D08, 0x406C))
    helpers = ((0x2C28, 1252, "sheets", "func_8013DCA4"),
               (0x367C, 784, "spokes", "func_8013E700"),
               (0x398C, 892, "rings", "func_8013EA14"),
               (0x3D08, 868, "quad", "func_8013ED94"))
    reachable_helpers = {0x2C28}
    local_call_targets = {0x11D0, 0x1860, 0x247C, 0x2C28, 0x310C}
    models_by_stage = ((7, (84, 162)), (9, (88, 114, 184, 369)))
    entry_anchors = {0x0C: 0x00809021, 0x14: 0x0240B021, 0x20: 0x26D80FD8,
                     0x28: 0x26D81108, 0x30: 0x26D81468, 0x38: 0x26D816A8,
                     0x61C: 0x27180090, 0x8F0: 0x27180098, 0xC04: 0x27180090,
                     0xDB0: 0x27180090, 0x910: 0x2B020002, 0xC20: 0x2BC20006,
                     0xDAC: 0x2BC20004, 0xDE0: 0xAEC01958, 0xDFC: 0xAEC01960}
