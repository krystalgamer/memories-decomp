import csv
import hashlib
import re
import struct

from tools.project.tests import test_french_model_variant435 as family435
from tools.project.tests import test_french_model_variant373 as layouts


class FrenchModelVariant782Tests(family435.FrenchModelVariant435Tests):
    family = 782
    slot_header_delta = 97
    module_count = 12
    distinct_images = 12
    binding_count = 27
    tail_start = 0x154C
    spans = ((4, 0x14B0),)
    helpers = ((4, 5292, "entry", "func_8013B004"),)
    reachable_helpers = {4}
    local_call_targets = set()
    models_by_stage = ((7, (58, 217, 453, 563, 612)), (9, (78,)))
    entry_anchors = {
        4: 0x27BDFDB8, 0x14A8: 0x03E00008, 0x14AC: 0x27BD0248,
        0x14B0: 4096, 0x14B4: 4096, 0x14B8: 4096, 0x14BC: 0,
    }



    def test_raw_storage_has_real_extents(self):
        super().test_raw_storage_has_real_extents()
        for module in self.modules:
            layout = family435.ROOT / module["layout"]
            base = int(module["load_address"], 0)
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            self.assertIn(
                f"D_{base + 0x14C0:X} = 0x{base + 0x14C0:X}; // type:u8 size:0x8C defined:true",
                symbols)
            self.assertIn(f"[0x14C0, data, overlays/{module['name']}/image_view]", layout.read_text())
            self.assertIn(
                f"D_{base + 0x14B0:X} = 0x{base + 0x14B0:X}; // type:u32 size:0x10 defined:true",
                symbols)
            suffix = "_slot1" if module["name"].endswith("1") else ""
            self.assertIn(f"[0x14B0, .rodata, overlays/french_model_variant/variant782_entry{suffix}]",
                          layout.read_text())

    def test_selected_descriptors_and_resident_calls(self):
        path = family435.ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        descriptors = {
            0: (60, 0, 512, 0, 80, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 194, 196, 404),
            2: (60, 0, 512, 0, 60, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 72, 78, 150),
            3: (60, 0, 512, 0, 60, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 72, 76, 200),
            4: (60, 0, 512, 0, 60, 64, 128, 0, 1024, 16, 64, 200, 24, 64, 64, 136, 140, 300),
        }
        arguments = {58: 0, 217: 2, 453: 4, 563: 0, 612: 2, 78: 3}
        observed = set()
        with path.open("rb") as archive:
            for module in self.modules:
                row = self.instances[module["name"]]
                slot, stage = int(row["slot"]), int(row["stage"])
                argument = arguments[int(row["model"])]
                self.assertEqual(int(row["command_word"]), 314000 + argument)
                archive.seek((int(row["record"]) * 276 + 275) * 2048
                             + 0x110 + (stage - 7) // 2 * 4)
                self.assertEqual(struct.unpack("<i", archive.read(4))[0], 314000 + argument)
                archive.seek(module["sector_offset"] * 2048)
                data = archive.read(20480)
                self.assertEqual(struct.unpack_from("<8hi8hi", data, 0x154C + 40 * argument),
                                 descriptors[argument])
                observed.add(argument)
                bindings = (family435.ROOT / module["linker_symbols"]).read_text()
                addresses = {int(value, 16) for value in re.findall(r"= (0x[0-9A-F]+);", bindings)}
                words = struct.unpack("<1323I", data[4:0x14B0])
                calls = [0x80000000 | ((word & 0x3FFFFFF) << 2) for word in words if word >> 26 == 3]
                self.assertEqual(len(calls), 74)
                self.assertEqual(set(calls), addresses)
                context = 0x80136000 + slot * 0x40000
                for start, size in ((0x80100000 + slot * 0x40000, 96 * 2048),
                                    (0x8013A000 + slot * 0x40000, 2 * 2048),
                                    (int(module["load_address"], 0), 20480)):
                    self.assertTrue(context + 0x9B0 <= start or start + size <= context)
        self.assertEqual(observed, set(descriptors))

    def test_target_compiled_context_layout(self):
        checks = {
            "sizeof(Model782Config)": 40, "sizeof(Model782State)": 0x9B0,
            "sizeof(MATRIX)": 32, "sizeof(SVECTOR)": 8, "sizeof(VECTOR)": 16,
            "sizeof(GsIMAGE)": 28, "sizeof(POLY_FT4)": 40,
            "sizeof(GsBOXF)": 16, "sizeof(DVECTOR)": 4,
        }
        for typename, fields in (
            ("Model782Config", (("growth_rate", 16), ("debris_count", 20), ("dot_count", 28),
                                ("initial_delay", 32), ("burst_end", 34), ("duration", 36))),
            ("Model782State", (("origin", 4), ("debris_positions", 12),
                               ("debris_velocities", 0x20C), ("dot_positions", 0x40C),
                               ("dot_velocities", 0x60C), ("screen", 0x80C),
                               ("texture", 0x90C), ("level", 0x920), ("depths", 0x924),
                               ("animation", 0x9A4), ("elapsed", 0x9A6), ("phase", 0x9A8),
                               ("growth_age", 0x9AA), ("shade", 0x9AC))),
        ):
            for field, value in fields:
                checks[f"(u32)&(({typename} *)0)->{field}"] = value
        self.assertEqual(len(checks), 29)
        layouts.FrenchModelVariant373Tests.assert_target_layout(self, checks, "variant782_entry.h")
