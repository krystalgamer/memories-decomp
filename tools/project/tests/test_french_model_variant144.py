import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant144Tests(family435.FrenchModelVariant435Tests):
    family = 144
    slot_header_delta = 130
    module_count = 2
    distinct_images = 2
    binding_count = 31
    tail_start = 0xEDC
    spans = ((4, 0xEB0),)
    helpers = ((4, 3756, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((9, (492,)),)
    entry_anchors = {
        4: 0x27BDFC28, 0x94: 0x2454FFFF, 0x164: 0x00839821,
        0x260: 0x2A820040, 0x2CC: 0xA3C00224, 0x624: 0x3202FFFF,
        0x9F8: 0x24020040, 0xA4C: 0x87C2021E,
        0xBB4: 0x00042140, 0xBB8: 0x2482001F,
        0xBBC: 0xA3A40084, 0xBC4: 0xA3A40094,
        0xE68: 0x14400004, 0xE70: 0xA3C20224,
        0xEA8: 0x03E00008, 0xEAC: 0x27BD03D8,
        0xEB0: 4096, 0xEB4: 4096, 0xEB8: 4096, 0xEBC: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, kind, size in ((0xEB0, "u32", 16), (0xEC0, "u8", 28)):
                self.assertIn(
                    f"D_{base + offset:X} = 0x{base + offset:X}; // type:{kind} size:0x{size:X} defined:true",
                    symbols)
            self.assertIn(f"[0xEC0, data, overlays/{module['name']}/image_view]", layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        expected = (
            255,255,255,255,64,64,128,128,128,31,32,-1,-1,-1,-1,-1,-1,300,50,60,90,80,124,
            70,-1,-1,-1,-1,-1,-1,-1,30,-1,-1,-1,-1,-1,-1,-1,
            7,-1,-1,-1,-1,-1,-1,-1,164,40)
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                self.assertEqual(int(row["command_word"]), 75100)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                command, = struct.unpack("<i", archive.read(4))
                self.assertEqual(command, 75100)
                argument = command % 1000
                self.assertEqual(argument, 100)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                descriptor = struct.unpack_from("<9Bx40h", data, 0xEDC + 90 * (argument % 100))
                self.assertEqual(descriptor, expected)
                self.assertEqual(descriptor[9:17], (31,32,-1,-1,-1,-1,-1,-1))
                self.assertEqual(descriptor[23:31], (70,-1,-1,-1,-1,-1,-1,-1))
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(v, 16) for v in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<939I", data[4:0xEB0])
                calls = [0x80000000 | ((w & 0x3FFFFFF) << 2) for w in words if w >> 26 == 3]
                self.assertEqual(set(calls), addresses)
                self.assertEqual(len(calls), 51)
                self.assertEqual(len(addresses), 30)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x228 <= start or start + size <= context)

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model144Config)": 90, "sizeof(Model144State)": 0x228,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(POLY_FT4)": 40, "sizeof(POLY_G4)": 36, "sizeof(GsBOXF)": 16,
            "sizeof(ModelTintColor)": 4, "sizeof(GsIMAGE)": 28,
            "sizeof(DVECTOR)": 4, "sizeof(PSXLONG)": 4,
            "sizeof(((Model144Config *)0)->parts) / sizeof(s16)": 8,
            "sizeof(((Model144Config *)0)->tint_starts) / sizeof(s16)": 8,
            "sizeof(((Model144Config *)0)->tint_durations) / sizeof(s16)": 8,
            "sizeof(((Model144Config *)0)->tint_ramp_durations) / sizeof(s16)": 8,
            "sizeof(((Model144State *)0)->points) / sizeof(SVECTOR)": 64,
            "sizeof(((Model144State *)0)->texture) / sizeof(s32)": 1,
        }
        for typename, fields in (
            ("Model144Config", (
                ("red", 0), ("particle_red", 3), ("glow_red", 6), ("unknown_09", 9),
                ("parts", 10), ("radius", 26), ("half_size", 28),
                ("gradient_duration", 30), ("particle_duration", 32),
                ("glow_fade_in", 34), ("glow_duration", 36), ("tint_starts", 38),
                ("tint_durations", 54), ("tint_ramp_durations", 70),
                ("main_delay", 86), ("glow_delay", 88),
            )),
            ("Model144State", (
                ("config", 0), ("unknown_004", 4), ("points", 28), ("texture", 540),
                ("elapsed", 544), ("started", 548), ("frame_step_override", 549),
            )),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 41)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant144_entry.h")
