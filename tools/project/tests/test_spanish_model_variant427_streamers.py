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

from overlay_sources import c_segments  # noqa: E402

START, END = 0x2DF8, 0x35BC


class SpanishModelVariant427StreamersTests(unittest.TestCase):
    def setUp(self):
        self.config = ROOT / "config/sles_03951"
        manifest = json.loads((self.config / "overlays.json").read_text())
        self.modules = [m for m in manifest["modules"] if m.get("linker_symbols") ==
                        "config/sles_03951/overlays/model_variant427_linker_symbols.txt"]

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

    def test_registration_reuses_the_accepted_french_source(self):
        self.assertEqual(len(self.modules), 2)
        for module in self.modules:
            slot = int(module["name"].endswith("slot1"))
            base = 0x8013B000 + slot * 0x40000
            layout = ROOT / module["layout"]
            source = "src/overlays/french_model_variant/variant427_streamers" + ("_slot1" if slot else "") + ".c"
            self.assertIn(source, [s["source"] for s in c_segments(ROOT, layout)])
            entry = {"address": f"0x{base+START:X}", "size": "0x7C4", "profile": "gcc_2_8_1_g0_split", "source": source}
            mapping = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            self.assertIn(entry, mapping["functions"])
            french = ROOT / module["layout"].replace("sles_03951", "sles_03948")
            self.assertIn(entry, json.loads(french.with_name(french.stem + "_matching_c.json").read_text())["functions"])
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                row, = [r for r in csv.DictReader(handle) if int(r["address"], 0) == base + START]
            self.assertEqual((row["status"], int(row["size"], 0)), ("matching_c", END - START))
        self.assertFalse((ROOT / "src/overlays/spanish_model_variant/variant427_streamers.c").exists())

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
                self.skipTest("Build MODEL427 overlays before checking relocations")
            self.assertEqual((directory / f"build/{module['name']}.bin").read_bytes(), image)
            selected, = [s for s in c_segments(ROOT, ROOT / module["layout"]) if "french_model_variant/variant427_streamers" in s["source"]]
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
                self.assertEqual(len(calls), 24)
                self.assertEqual(set(calls), {0x8005C018, 0x800842A8, 0x80085558, 0x80086258, 0x80086628, 0x800866F8, 0x800875F8, 0x80087868, 0x80087958, 0x80087CB8, 0x80089928})
                self.assertEqual(jumps, [(124, 156), (1220, 1432)])


if __name__ == "__main__":
    unittest.main()
