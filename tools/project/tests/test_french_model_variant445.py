from tools.project.tests import test_french_model_variant435 as family435


class FrenchModelVariant445Tests(family435.FrenchModelVariant435Tests):
    family = 445
    source_family = 428
    module_count = 12
    distinct_images = 12
    binding_count = 36
    tail_start = 0x3594
    spans = ((4, 0x1034), (0x1034, 0x17A8), (0x17A8, 0x1C8C),
             (0x1C8C, 0x21F0), (0x21F0, 0x24F8), (0x24F8, 0x2874),
             (0x2874, 0x2BD8), (0x2BD8, 0x3594))
    helpers = ((0x17A8, 1252, "sheets", "func_8013C7AC"),
               (0x21F0, 776, "spokes", "func_8013D1FC"),
               (0x24F8, 892, "rings", "func_8013D508"),
               (0x2874, 868, "quad", "func_8013D888"))
    reachable_helpers = {0x17A8}
    local_call_targets = {0x1034, 0x17A8, 0x1C8C, 0x2BD8}
    models_by_stage = ((7, (187, 596)), (9, (239, 361, 368, 478)))
    entry_anchors = {0x0C: 0x00809821, 0x14: 0x0260B021, 0x20: 0x26D80C78,
                     0x28: 0x26D80DA8, 0x30: 0x26D81108, 0x38: 0x26D81348,
                     0x65C: 0x27180090, 0x820: 0x27180098, 0x838: 0x2AE20002,
                     0xB30: 0x27180090, 0xB70: 0x2B020006, 0xC58: 0x27180090,
                     0xC68: 0x2B020004, 0xC98: 0xAEC015B8, 0xCB0: 0xAEC015BC}
