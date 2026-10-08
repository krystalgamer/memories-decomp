import csv
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from overlay_sources import c_segments

STEM = "src/overlays/french_model_variant/variant437_bands"
HEADER = "src/overlays/french_model_variant/variant449_bands.h"
BODY = "src/overlays/french_model_variant/variant449_bands.c"
MODULES = {"french_model_variant_103_stage9_slot0", "french_model_variant_103_stage10_slot1"}


class FrenchModelVariant437BandsTests(unittest.TestCase):
    def setUp(self):
        self.modules = [module for module in json.loads(
            (ROOT / "config/sles_03948/overlays.json").read_text())["modules"]
                        if module["name"] in MODULES]

    def test_registrations_and_alias(self):
        self.assertEqual({module["name"] for module in self.modules}, MODULES)
        for module in self.modules:
            base = int(module["load_address"], 0)
            source = STEM + ("_slot1" if base == 0x8017B000 else "") + ".c"
            layout = ROOT / module["layout"]
            functions = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())["functions"]
            self.assertIn({"address": f"0x{base + 0xD4C:X}", "profile": "gcc_2_8_1_g0_split",
                           "size": "0x75C", "source": source}, functions)
            self.assertIn(source, [segment["source"] for segment in c_segments(ROOT, layout)])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                row = next(row for row in csv.DictReader(handle) if int(row["address"], 0) == base + 0xD4C)
            self.assertEqual((row["status"], row["size"]), ("matching_c", "0x75C"))
            bindings = (ROOT / module["linker_symbols"]).read_text()
            self.assertIn("RotTransPers3 = 0x80087898;", bindings)
            self.assertIn("func_spanish_80087898 = 0x80087898;", bindings)

    def test_wrappers_preserve_the_shared_body(self):
        for slot in (0, 1):
            base = 0x8013B000 + slot * 0x40000
            source = STEM + ("_slot1" if slot else "") + ".c"
            self.assertEqual((ROOT / source).read_text(),
                             '#include "../../types.h"\n'
                             "#define MODEL_VARIANT437_BANDS\n"
                             f"#define func_8013D458 func_{base + 0xD4C:X}\n"
                             '#include "variant449_bands.c"\n')
        self.assertNotRegex((ROOT / BODY).read_text(), r"\b(?:asm|__asm__|register|volatile)\b")

    def test_both_target_compiled_layouts(self):
        from build_baseline import compile_c, load_compiler_profiles, tool

        if not (ROOT / "tools/toolchains/binutils-2.42/bin/mipsel-none-elf-as").exists():
            self.skipTest("Set up the project toolchain before compiling layout assertions")
        fields = {"bands": 0xE88, "quads": 0x19F0, "origin": 0x1B14,
                  "factor": 0x1B90, "scale": 0x1B98, "spread": 0x1B9C,
                  "delta": 0x1BB0, "axis_x": 0x1BC0, "axis_y": 0x1BC2,
                  "flags": 0x1BDC, "direction": 0x1C4C}
        for specialized in (False, True):
            with self.subTest(specialized=specialized):
                lines = ["#define MODEL_VARIANT437_BANDS"] if specialized else []
                lines += [f'#include "../../{HEADER}"',
                          "typedef char band_size[(sizeof(Variant449Band) == 0xB4) ? 1 : -1];"]
                for index, (field, offset) in enumerate(fields.items()):
                    offset -= 0x600 if specialized else 0
                    lines.append(f"typedef char field_{index}[((u32)&((Variant449BandView *)0)->{field} == {offset}) ? 1 : -1];")
                lines.append("s32 bands_layout_checked;")
                with tempfile.TemporaryDirectory(prefix="bands437-layout-", dir=ROOT / "tmp") as directory:
                    directory = Path(directory)
                    source = directory / "layout.c"
                    source.write_text("\n".join(lines) + "\n")
                    compile_c(ROOT, tool(ROOT, "as"),
                              {"source": source.relative_to(ROOT).as_posix(),
                               "profile": "gcc_2_8_1_g0_split", "object": "layout.o"},
                              load_compiler_profiles(ROOT),
                              object_directory=(directory / "obj").relative_to(ROOT).as_posix(),
                              asm_directory=(directory / "asm").relative_to(ROOT).as_posix())

    def test_terminal_fingerprints(self):
        with (ROOT / "notes/overlays/french-model-variant437-bands-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        exact = [row for row in rows if row["result"] == "matched"]
        self.assertEqual(len(exact), 2)
        for slot, row in enumerate(exact):
            source = ROOT / (STEM + ("_slot1" if slot else "") + ".c")
            self.assertEqual((row["slot"], row["instruction_bytes"], row["different_words"],
                              row["profile"]), (str(slot), "1884", "0", "gcc_2_8_1_g0_split"))
            self.assertEqual(row["fingerprint"], hashlib.sha256(source.read_bytes()).hexdigest())
            self.assertEqual(row["dependency_fingerprint"], hashlib.sha256((ROOT / HEADER).read_bytes()).hexdigest())
            self.assertEqual(row["body_fingerprint"], hashlib.sha256((ROOT / BODY).read_bytes()).hexdigest())

    def test_complete_images_and_sized_c_owners(self):
        from elftools.elf.elffile import ELFFile

        for module in self.modules:
            build = ROOT / "tmp/overlays" / module["name"] / "build"
            binary = build / (module["name"] + ".bin")
            if not binary.exists():
                self.skipTest("Build French overlays before checking linked owners")
            base = int(module["load_address"], 0)
            symbol = f"func_{base + 0xD4C:X}"
            self.assertEqual(hashlib.sha256(binary.read_bytes()).hexdigest(), module["sha256"])
            source = STEM + ("_slot1" if base == 0x8017B000 else "") + ".c"
            segment = next(segment for segment in c_segments(ROOT, ROOT / module["layout"])
                           if segment["source"] == source)
            with (build / segment["object"]).open("rb") as handle:
                owner, = ELFFile(handle).get_section_by_name(".symtab").get_symbol_by_name(symbol)
                self.assertEqual((owner["st_value"], owner["st_size"], owner["st_info"]["type"]),
                                 (0, 0x75C, "STT_FUNC"))
            with (build / (module["name"] + ".elf")).open("rb") as handle:
                table = ELFFile(handle).get_section_by_name(".symtab")
                linked, = table.get_symbol_by_name(symbol)
                self.assertEqual((linked["st_value"], linked["st_size"]), (base + 0xD4C, 0x75C))
                self.assertIsNone(table.get_symbol_by_name(symbol + ".NON_MATCHING"))
