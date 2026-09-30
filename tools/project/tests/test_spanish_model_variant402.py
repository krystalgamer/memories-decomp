import struct

from tools.project.tests import test_french_model_variant402 as reference
from tools.project.tests import test_spanish_model_variant338 as shared


class SpanishModelVariant402Tests(shared.SpanishModelVariant338Tests):
    family = 402
    module_count = 4
    tail_start = 0x2060
    models_by_stage = ((7, (6, 551)),)
    helpers = ((0x169C, 1180, "rings"), (0x1B38, 1320, "bands"))
    spans = ((4, 0xA5C), (0xA5C, 0x1048), (0x1048, 0x169C),
             (0x169C, 0x1B38), (0x1B38, 0x2060))
    reachable_helpers = {0x1B38}
    entry_calls = {0x1B38}
    entry_anchors = reference.FrenchModelVariant402Tests.entry_anchors

    def check_descriptor(self, data, base, request):
        self.assertEqual(request, 568000)
        config = base + 0x215C
        self.assertEqual(struct.unpack_from("<II", data, 0x84),
                         (0x3C020000 | ((config+0x8000) >> 16 & 0xFFFF),
                          0x24420000 | (config & 0xFFFF)))
        offset = 0x215C + request % 1000 * 20
        self.assertTrue(self.tail_start <= offset and offset+20 <= len(data))
        self.assertEqual(struct.unpack_from("<H", data, offset+12)[0], 1)
        self.assertEqual(struct.unpack_from("<I", data, offset+16)[0], 92)
        self.assertEqual(0x58 + 2 * 144, 0x178)
        self.assertEqual(0x438 + 2 * 280, 0x668)
