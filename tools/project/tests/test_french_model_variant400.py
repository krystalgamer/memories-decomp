import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import platform
import re
import shutil
import struct
import subprocess
import tempfile
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[3]
CONFIG = ROOT / "config/sles_03948"
SOURCE = ROOT / "src/overlays/french_model_variant/variant400_sheets.c"
PROFILE = "gcc_2_8_1_g0_split"
IMAGES = (
    (189, 7, 0, 52344, "e893b2c350b3e37d8c6900e203d359195cf30e4480fa4aefae653a71b97cfb5d"),
    (189, 8, 1, 52354, "598e524fb00a6cf956e45cc8b996d3d934c9c1d46d2f889626807aa61acb5344"),
    (258, 7, 0, 71388, "99452abe45adef1cf375312298e762a55e22dcde342b2fb49aa7f5b6fd8f1158"),
    (258, 8, 1, 71398, "88883c17fc3371c99fe256c03f16fcafefdf1bf75a7673b4a14b74adabee1e4b"),
)
SPANS = ((4, 0xA88), (0xA88, 0x1838), (0x1838, 0x1DB8), (0x1DB8, 0x2748))


class FrenchModelVariant400Tests(unittest.TestCase):
    def setUp(self):
        self.modules = {row["name"]: row for row in json.loads(
            (CONFIG / "overlays.json").read_text())["modules"]}
        with (ROOT / "notes/overlays/french-model-variant400-instances.csv").open() as handle:
            self.instances = {row["module"]: row for row in csv.DictReader(handle)}

    def test_four_independent_manifest_identities(self):
        names = {f"french_model_variant_{model}_stage{stage}_slot{slot}"
                 for model, stage, slot, *_ in IMAGES}
        self.assertEqual(set(self.instances), names)
        for model, stage, slot, sector, digest in IMAGES:
            name = f"french_model_variant_{model}_stage{stage}_slot{slot}"
            module, row = self.modules[name], self.instances[name]
            self.assertEqual(module["archive"], "game/france/DATA/MODEL.MRG")
            self.assertEqual(module["archive_sha256"],
                             "0c3f90cf4a3b4776d1188054a6cd5d89c5bc364215dac15c4730ef74edbc8da3")
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector, 10))
            self.assertEqual(module["load_address"], f"0x{0x8013B000 + slot * 0x40000:X}")
            self.assertEqual(module["sha256"], digest)
            self.assertEqual(module["linker_symbols"],
                             "config/sles_03948/overlays/model_variant475_linker_symbols.txt")
            self.assertEqual(module["output"], f"tmp/overlays/{name}/module.bin")
            self.assertEqual(module["layout"],
                             f"config/sles_03948/overlays/{name.removeprefix('french_')}.yaml")
            self.assertEqual((int(row["model"]), int(row["stage"]), int(row["slot"]),
                              int(row["header"]), int(row["sector"]), row["sha256"]),
                             (model, stage, slot, 400 + slot * 150, sector, digest))
            self.assertEqual((int(row["command_word"]), int(row["descriptor_selector"])),
                             (566000 + (model == 258), int(model == 258)))

    def test_one_c_owner_and_preserved_assembly_raw_extents(self):
        from tools.project.overlay_sources import c_segments

        for name in self.instances:
            module = self.modules[name]
            base = int(module["load_address"], 0)
            slot = (base - 0x8013B000) // 0x40000
            source = "src/overlays/french_model_variant/variant400_sheets" + ("_slot1" if slot else "") + ".c"
            layout = ROOT / module["layout"]
            manifest = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertEqual(manifest, {"schema": 1, "functions": [{
                "address": f"0x{base + 0x1838:X}", "size": "0x580",
                "profile": PROFILE, "source": source,
            }]})
            segment, = c_segments(ROOT, layout)
            self.assertEqual((segment["source"], segment["profile"]), (source, PROFILE))
            segments = yaml.safe_load(layout.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            self.assertEqual(
                [(row["start"], row["vram"], row["subsegments"][0][:2]) for row in segments[:-1]],
                [(offset, base + offset, [offset, kind]) for offset, kind in (
                    (0, "data"), (4, "asm"), (0xA88, "asm"), (0x1838, "c"),
                    (0x1DB8, "asm"), (0x2748, "data"))])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                inventory = list(csv.DictReader(handle))
            self.assertEqual(
                [(int(row["address"], 0), int(row["size"], 0), row["status"]) for row in inventory],
                [(base + start, end - start, "matching_c" if start == 0x1838 else "unmatched_asm")
                 for start, end in SPANS])
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            for offset, size in ((0, 4), (0x2748, 0x28B8)):
                self.assertIn(f"D_{base + offset:X} = 0x{base + offset:X}; "
                              f"// type:u8 size:0x{size:X} defined:true", symbols)

    def test_terminal_fingerprints_and_symbol_only_wrapper(self):
        with (ROOT / "notes/overlays/french-model-variant400-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        terminals = {(int(row["function_offset"], 0), int(row["slot"])): row
                     for row in rows if row["result"] == "matched"}
        self.assertEqual(set(terminals), {(0x1838, 0), (0x1838, 1)})
        for slot in (0, 1):
            source = SOURCE.with_name("variant400_sheets" + ("_slot1" if slot else "") + ".c")
            row = terminals[(0x1838, slot)]
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual((row["profile"], row["instruction_bytes"], row["different_words"]),
                             (PROFILE, "1408", "0"))
        self.assertEqual(SOURCE.with_name("variant400_sheets_slot1.c").read_text(),
                         '#include "../../types.h"\n\n#define func_8013C838 func_8017C838\n'
                         '#include "variant400_sheets.c"\n')
        self.assertEqual(sum(row["result"] == "mismatch" for row in rows), 32)

    def test_guest_descriptor_loads_and_retained_behavior(self):
        text = SOURCE.read_text()
        self.assertNotIn("MODEL_VARIANT_WORD(MODEL_VARIANT_WORD(", text)
        offsets = re.findall(
            r"MODEL_VARIANT_WORD\(\*\(u8 \*G32 \*\)\(work \+ 0x205C\), (0x[0-9A-F]+)\)", text)
        self.assertEqual(offsets, ["0x0C", "0x10", "0x0C", "0x14"])
        for expression in (
            "ModelVariantSheet *quad;", "work + 0x1130", "work + 0x1F4C",
            "i < 7", "i < 6", "k < 4", "node += 880", "quad->size / 8",
            "depth = depth * 8 / 10;", "depth >= 0 && flag >= 0",
            "ReadRotMatrix(&light);", "SetRotMatrix(&light);",
            "quad->size < 4096", "quad->size < 8192",
            "(u32)MODEL_VARIANT_WORD(work, 0x2048)",
        ):
            self.assertIn(expression, text)
        self.assertEqual(text.count("ScaleMatrix("), 2)
        self.assertNotIn("extern ", text)

    def test_native_guest_pointer_zero_extension(self):
        compiler = shutil.which("clang")
        if compiler is None or platform.machine().lower() not in ("x86_64", "amd64", "aarch64"):
            self.skipTest("64-bit x86/ARM clang required for native guest-pointer check")
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french400-pointer-") as name:
            directory = Path(name)
            source = directory / "pointer.c"
            source.write_text(
                '#include "../../src/types.h"\n#include "../../src/port_ptr.h"\n'
                'int main(void) {\n'
                '    u8 work[0x2060] = {0};\n'
                '    *(u8 *G32 *)(work + 0x205C) = (u8 *)0x8013D844u;\n'
                '    return sizeof(u8 *G32) != 4 ||\n'
                '        (u64)*(u8 *G32 *)(work + 0x205C) != 0x8013D844u;\n'
                '}\n')
            binary = directory / "pointer"
            subprocess.run([compiler, "-DMEMORIES_PC", "-fms-extensions", "-Werror",
                            str(source), "-o", str(binary)], cwd=ROOT, check=True, capture_output=True)
            subprocess.run([str(binary)], cwd=ROOT, check=True, capture_output=True)

    def test_target_compiled_sheet_and_guest_pointer_layout(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("pyelftools required for target layouts")
        from elftools.elf.elffile import ELFFile
        from tools.project.build_baseline import TOOLCHAIN, compile_c, load_compiler_profiles, tool

        profiles = load_compiler_profiles(ROOT)
        for path in (profiles[PROFILE]["compiler"], f"{TOOLCHAIN}/mipsel-none-elf-as",
                     "tools/vendor/maspsx/maspsx.py"):
            if not (ROOT / path).is_file():
                self.skipTest(f"target layout requires {path}")
        checks = {"sizeof(ModelVariantSheet)": 152, "sizeof(u8 *G32)": 4,
                  "sizeof(GsCOORDINATE2)": 80, "sizeof(POLY_GT4)": 52, "sizeof(PSXLONG)": 4}
        for field, offset in (("v0", 0), ("v1", 32), ("v2", 64), ("v3", 96),
                              ("outer", 128), ("inner", 132), ("size", 136)):
            checks[f"(u32)&((ModelVariantSheet *)0)->{field}"] = offset
        with tempfile.TemporaryDirectory(dir=ROOT / "tmp", prefix="french400-layout-") as name:
            directory = Path(name).relative_to(ROOT)
            source = directory / "layout.c"
            (ROOT / source).write_text(
                '#include "../../src/types.h"\n'
                '#include "../../src/overlays/model_variant/model_variant.h"\n'
                'const u32 layouts[] = {' + ", ".join(checks) + "};\n")
            obj = compile_c(ROOT, tool(ROOT, "as"),
                            {"source": str(source), "object": "layout.o", "profile": PROFILE},
                            profiles, object_directory=str(directory), asm_directory=str(directory / "asm"))
            with obj.open("rb") as handle:
                elf = ELFFile(handle)
                symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layouts")
                self.assertIsInstance(symbol["st_shndx"], int)
                self.assertEqual(symbol["st_value"], 0)
                self.assertEqual(struct.unpack(f"<{len(checks)}I", elf.get_section(symbol["st_shndx"]).data()),
                                 tuple(checks.values()))

    def test_retail_descriptors_and_closed_function_boundaries(self):
        from tools.project.overlay_function_inventory import walk_function

        path = ROOT / "game/france/DATA/MODEL.MRG"
        if not path.exists():
            self.skipTest("legal French MODEL input required")
        with path.open("rb") as archive:
            for model, stage, slot, sector, digest in IMAGES:
                base = 0x8013B000 + slot * 0x40000
                archive.seek(sector * 2048)
                payload = archive.read(20480)
                self.assertEqual(hashlib.sha256(payload).hexdigest(), digest)
                self.assertEqual(struct.unpack_from("<I", payload)[0], 400 + slot * 150)
                archive.seek((model * 276 + 275) * 2048 + 0x110)
                command, = struct.unpack("<I", archive.read(4))
                selector = int(model == 258)
                self.assertEqual(command, 566000 + selector)
                self.assertEqual(command % 1000, selector)
                descriptor = 0x2844 + selector * 32
                self.assertGreaterEqual(descriptor, 0x2748)
                self.assertLessEqual(descriptor + 32, len(payload))
                self.assertEqual(struct.unpack_from("<3I", payload, descriptor + 12),
                                 (76, 240, 300) if selector == 0 else (116, 320, 760))
                for start, end in SPANS:
                    result = walk_function(payload, base, start, end - start)
                    self.assertTrue(result["closed"])
                    self.assertEqual(result["visited"], set(range(start, end, 4)))
                    self.assertEqual(result["returns"], 1)
                    self.assertEqual(result["problems"], set())
                self.assertEqual(struct.unpack_from("<I", payload, 0x1838)[0], 0x27BDFEF0)
                self.assertEqual(struct.unpack_from("<I", payload, 0x1DB4)[0], 0x27BD0110)
