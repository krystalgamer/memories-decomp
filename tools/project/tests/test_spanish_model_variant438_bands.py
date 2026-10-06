import csv
import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import sys
import unittest

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from build_baseline import compile_c, load_compiler_profiles, tool  # noqa: E402
from overlay_sources import c_segments  # noqa: E402

START, END = 0x3638, 0x3DE8
LAYOUT = (
    ("sizeof(ModelVariantBand)", 0x1C8), ("O(ModelVariantBand,sa)", 0xD8), ("O(ModelVariantBand,otz)", 0x1A4),
    ("O(Bands438Timing,grow_start)", 0x20), ("O(Bands438Timing,fade_end)", 0x2C), ("O(Bands438Timing,radius)", 0x40),
    ("sizeof(POLY_GT4)", 52), ("O(Bands438State,bands)", 0x1770), ("O(Bands438State,quad)", 0x20D8),
    ("O(Bands438State,origin)", 0x2258), ("O(Bands438State,direction)", 0x226C),
    ("O(Bands438State,projected)", 0x227C), ("O(Bands438State,frame)", 0x2298),
    ("O(Bands438State,time)", 0x229C), ("O(Bands438State,timing)", 0x22B4),
    ("O(Bands438State,size)", 0x22D8), ("O(Bands438State,progress)", 0x22DC),
    ("O(Bands438State,phase)", 0x22E8), ("sizeof(Bands438State)", 0x22EC),
)


class SpanishModelVariant438BandsTests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"] if m.get("linker_symbols") ==
                        "config/sles_03951/overlays/model_variant438_linker_symbols.txt"]

    def legal_images(self):
        archive = ROOT / "game/spain/DATA/MODEL.MRG"
        if not archive.is_file():
            self.skipTest("Legal Spanish MODEL input required")
        with archive.open("rb") as handle:
            for module in self.modules:
                handle.seek(module["sector_offset"] * 2048)
                image = handle.read(20480)
                self.assertEqual(hashlib.sha256(image).hexdigest(), module["sha256"])
                yield module, image

    def test_metadata_and_terminal_fingerprints(self):
        self.assertEqual(len(self.modules), 12)
        body = ROOT / "src/overlays/spanish_model_variant/variant438_bands.c"
        dependency = hashlib.sha256(body.read_bytes() + body.with_suffix(".h").read_bytes() +
                                    (ROOT / "src/overlays/model_variant/model_variant.h").read_bytes() +
                                    (ROOT / "src/game/gpu_packets.h").read_bytes()).hexdigest()
        with (ROOT / "notes/overlays/spanish-model-variant438-bands-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual([(int(r["instruction_bytes"]), int(r["different_words"])) for r in attempts],
                         [(1920, 452), (1972, 476)] + [(1968, 0)] * 14)
        for module in self.modules:
            slot = int(module["name"].endswith("slot1"))
            base = 0x8013B000 + slot * 0x40000
            layout = ROOT / module["layout"]
            source = "src/overlays/spanish_model_variant/variant438_bands" + ("_slot1" if slot else "") + ".c"
            self.assertIn(source, [s["source"] for s in c_segments(ROOT, layout)])
            mapping = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertIn({"address": f"0x{base+START:X}", "size": "0x7B0",
                           "profile": "gcc_2_8_1_g0_split", "source": source}, mapping["functions"])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                row, = [r for r in csv.DictReader(handle) if int(r["address"], 0) == base + START]
            self.assertEqual((row["status"], int(row["size"], 0)), ("matching_c", END - START))
            terminal = [r for r in attempts if r["module"] == module["name"]][-1]
            self.assertEqual((terminal["result"], terminal["profile"], terminal["instruction_bytes"]),
                             ("matched", "gcc_2_8_1_g0_split", "1968"))
            self.assertEqual(terminal["fingerprint"], hashlib.sha256((ROOT / source).read_bytes()).hexdigest())
            self.assertEqual(terminal["dependency_fingerprint"], dependency)

    def test_target_layout_and_repeated_header_inclusion(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile
        directory = ROOT / "tmp/model438-bands-layout-test"
        directory.mkdir(parents=True, exist_ok=True)
        source = directory / "layout.c"
        source.write_text('#include "../../src/types.h"\n' +
                          '#include "../../src/overlays/spanish_model_variant/variant438_bands.h"\n' * 2 +
                          "#define O(t,f) ((u32)&((t *)0)->f)\n" +
                          "u32 layout[] = {" + ",".join(expression for expression, _ in LAYOUT) + "};\n")
        obj = compile_c(ROOT, tool(ROOT, "as"),
                        dict(source=str(source.relative_to(ROOT)), profile="gcc_2_8_1_g0_split", object="layout.o"),
                        load_compiler_profiles(ROOT), object_directory=str(directory.relative_to(ROOT)),
                        asm_directory=str(directory.relative_to(ROOT)))
        with (ROOT / obj).open("rb") as handle:
            elf = ELFFile(handle)
            symbol, = elf.get_section_by_name(".symtab").get_symbol_by_name("layout")
            expected = struct.pack("<" + "I" * len(LAYOUT), *(value for _, value in LAYOUT))
            data = elf.get_section(symbol["st_shndx"]).data()[symbol["st_value"]:]
            self.assertEqual(data[:len(expected)], expected)

    def test_selected_relocations_against_all_images_when_built(self):
        if importlib.util.find_spec("elftools") is None:
            self.skipTest("Optional pyelftools required")
        from elftools.elf.elffile import ELFFile
        for module, image in self.legal_images():
            slot = int(module["name"].endswith("slot1"))
            base = 0x8013B000 + slot * 0x40000
            directory = ROOT / f"tmp/overlays/{module['name']}"
            linked_path = directory / f"build/{module['name']}.elf"
            if not linked_path.is_file():
                self.skipTest("Build MODEL438 overlays before checking relocations")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "variant438_bands" in s["source"]]
            with linked_path.open("rb") as handle, (directory / "build" / selected["object"]).open("rb") as obj_handle:
                linked, compiled = ELFFile(handle), ELFFile(obj_handle)
                symbols, original = linked.get_section_by_name(".symtab"), compiled.get_section_by_name(".symtab")
                own, = original.get_symbol_by_name(f"func_{base+START:X}")
                self.assertEqual((own["st_value"], own["st_size"], own["st_info"]["type"]),
                                 (0, END - START, "STT_FUNC"))
                final, = symbols.get_symbol_by_name(f"func_{base+START:X}")
                self.assertEqual((final["st_value"], final["st_size"]), (base + START, END - START))
                text = bytearray(compiled.get_section(own["st_shndx"]).data()[:END - START])
                calls, jumps = [], []
                for relocation in compiled.get_section_by_name(".rel.text").iter_relocations():
                    self.assertEqual(relocation["r_info_type"], 4)
                    site = relocation["r_offset"]
                    symbol = original.get_symbol(relocation["r_info_sym"])
                    word, = struct.unpack_from("<I", text, site)
                    addend = (word & 0x3FFFFFF) << 2
                    if symbol["st_shndx"] == "SHN_UNDEF":
                        resolved, = symbols.get_symbol_by_name(symbol.name)
                        address = resolved["st_value"] + addend
                        calls.append(address)
                    else:
                        address = base + START + symbol["st_value"] + addend
                        jumps.append((site, address - base - START))
                    struct.pack_into("<I", text, site, (word & 0xFC000000) | ((address >> 2) & 0x3FFFFFF))
                self.assertEqual(bytes(text), image[START:END])
                self.assertEqual(len(calls), 16)
                self.assertEqual(set(calls), {0x8005C018, 0x800842A8, 0x80085558, 0x80086258, 0x80086628, 0x800866F8,
                                              0x800872A8, 0x800875F8, 0x80087738, 0x80087898, 0x80087CB8, 0x80089928})
                self.assertEqual(jumps, [(128, 208), (428, 652), (1796, 1916)])


if __name__ == "__main__":
    unittest.main()
