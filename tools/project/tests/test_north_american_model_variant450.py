import csv
import hashlib
import json
from pathlib import Path
import re
import sys
import unittest

import yaml

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "tools/project"))

from tools.project.overlay_sources import c_segments


CONFIG = ROOT / "config/slus_01411"
SPANS = (
    (0x4, 0xDDC),
    (0xDDC, 0x1770),
    (0x1770, 0x1E7C),
    (0x1E7C, 0x2754),
    (0x2754, 0x2D20),
    (0x2D20, 0x30A8),
    (0x30A8, 0x38DC),
)
# Shared sources that later gained French-only blocks. The historical
# attempt fingerprints cover the North American projection (the
# `#ifdef VERSION_FRENCH` arms removed); the complete regional source is
# pinned separately.
REGIONALIZED_SOURCES = {
    "variant433_rings.c": "4059332fed86b8dd27b5c2d725a7c98aaac797a73ab30c86cb37b2037dea47a8",
}


def north_american_projection(text):
    out, state = [], None
    for line in text.splitlines(keepends=True):
        directive = line.strip()
        if state is None and directive == "#ifdef VERSION_FRENCH":
            state = "french"
            continue
        if state is not None and directive.startswith("#if"):
            raise AssertionError(f"nested conditional in a VERSION_FRENCH block: {line!r}")
        if state == "french" and directive == "#else":
            state = "north_american"
            continue
        if state is not None and directive == "#endif":
            state = None
            continue
        if state != "french":
            out.append(line)
    if state is not None:
        raise AssertionError("unterminated VERSION_FRENCH block")
    return "".join(out)


IMAGES = (
    (9, 0, 48224, "827003f63d7c9d5026895b66cdf2b3e4b6412ecf4a897efaa72827c7c392d1a9"),
    (10, 1, 48234, "f14a1530ad7caf52efa725e09b6db29e2be5412b30cbbe51156e2e7aea8701d0"),
)
HELPERS = (
    (0x4, 0xDD8, "entry", "variant433"),
    (0xDDC, 0x994, "ribbons", "variant433"),
    (0x1770, 0x70C, "bands", "variant433"),
    (0x1E7C, 0x8D8, "rings", "variant433"),
    (0x2754, 0x5CC, "quads", "variant450_ntsc"),
    (0x2D20, 0x388, "lines", "variant450_ntsc"),
    (0x30A8, 0x834, "coils", "variant433"),
)
IMPORTS = {
    0x80058F10,
    0x80084130,
    0x80084320,
    0x800855D0,
    0x800862D0,
    0x80087320,
    0x80087670,
    0x800877B0,
    0x80087910,
    0x800879D0,
    0x80087D30,
    0x800899A0,
}


class NorthAmericanModelVariant450Tests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        with (ROOT / "notes/overlays/north-american-model-variant450-instances.csv").open() as handle:
            cls.instances = {row["module"]: row for row in csv.DictReader(handle)}
        modules = json.loads((CONFIG / "overlays.json").read_text())["modules"]
        cls.modules = {row["name"]: row for row in modules if row["name"] in cls.instances}

    def test_two_registered_images_and_exact_inventory(self):
        expected = {f"model_variant_174_stage{stage}_slot{slot}" for stage, slot, *_ in IMAGES}
        self.assertEqual(set(self.instances), expected)
        self.assertEqual(set(self.modules), expected)
        for stage, slot, sector, digest in IMAGES:
            name = f"model_variant_174_stage{stage}_slot{slot}"
            module, instance = self.modules[name], self.instances[name]
            base = 0x8013B000 + slot * 0x40000
            self.assertEqual((module["sector_offset"], module["sector_count"]), (sector, 10))
            self.assertEqual(module["load_address"], f"0x{base:X}")
            self.assertEqual((module["sha256"], instance["sha256"]), (digest, digest))
            self.assertEqual(module["linker_symbols"],
                             "config/slus_01411/overlays/model_variant_linker_symbols.txt")
            layout = ROOT / module["layout"]
            with layout.with_name(layout.stem + "_functions.csv").open() as handle:
                rows = list(csv.DictReader(handle))
            self.assertEqual(
                [(int(row["address"], 0) - base, int(row["size"], 0), row["status"]) for row in rows],
                [(start, end - start, "matching_c") for start, end in SPANS],
            )
            self.assertTrue(all("complete matching 20 KiB image" in row["notes"]
                                for row in rows if row["status"] == "matching_c"))

    def test_c_owners_use_cdk_profile_and_exact_extents(self):
        for stage, slot, *_ in IMAGES:
            name = f"model_variant_174_stage{stage}_slot{slot}"
            layout = ROOT / self.modules[name]["layout"]
            base = 0x8013B000 + slot * 0x40000
            mapping = json.loads(layout.with_name(layout.stem + "_matching_c.json").read_text())
            expected = []
            for offset, size, label, prefix in HELPERS:
                suffix = "_slot1" if slot else ""
                expected.append({
                    "address": f"0x{base + offset:X}",
                    "profile": "gcc_2_7_2_cdk_g0",
                    "size": f"0x{size:X}",
                    "source": f"src/overlays/model_variant/{prefix}_{label}{suffix}.c",
                })
            self.assertEqual(mapping, {"functions": expected, "schema": 1})
            self.assertEqual([row["source"] for row in c_segments(ROOT, layout)],
                             [row["source"] for row in expected])
            segments = yaml.safe_load(layout.read_text())["segments"]
            self.assertEqual(segments[-1], [0x5000])
            self.assertEqual(
                [(row["start"], row["vram"], row["subsegments"][0][1]) for row in segments[:-1]],
                [
                    (0, base, "data"),
                    (4, base + 4, "c"),
                    (0xDDC, base + 0xDDC, "c"),
                    (0x1770, base + 0x1770, "c"),
                    (0x1E7C, base + 0x1E7C, "c"),
                    (0x2754, base + 0x2754, "c"),
                    (0x2D20, base + 0x2D20, "c"),
                    (0x30A8, base + 0x30A8, "c"),
                    (0x38DC, base + 0x38DC, "data"),
                ],
            )

    def test_attempts_and_source_fingerprints(self):
        with (ROOT / "notes/overlays/north-american-model-variant450-attempts.csv").open() as handle:
            rows = list(csv.DictReader(handle))
        self.assertEqual([row["result"] for row in rows], ["text_exact"] * 14 + ["matched"] * 14)
        for row in rows:
            slot = int(row["slot"])
            label, prefix = {
                "0x4": ("entry", "variant433"),
                "0xDDC": ("ribbons", "variant433"),
                "0x1770": ("bands", "variant433"),
                "0x1E7C": ("rings", "variant433"),
                "0x30A8": ("coils", "variant433"),
                "0x2754": ("quads", "variant450_ntsc"),
                "0x2D20": ("lines", "variant450_ntsc"),
            }[row["function_offset"]]
            suffix = "_slot1" if slot else ""
            source = ROOT / f"src/overlays/model_variant/{prefix}_{label}{suffix}.c"
            text = source.read_bytes()
            pinned = REGIONALIZED_SOURCES.get(source.name)
            if pinned is None:
                self.assertNotIn(b"VERSION_FRENCH", text)
            else:
                self.assertEqual(hashlib.sha256(text).hexdigest(), pinned)
                text = north_american_projection(text.decode()).encode()
            self.assertEqual(row["fingerprint"], hashlib.sha256(text).hexdigest())
            self.assertEqual(row["profile"], "gcc_2_7_2_cdk_g0")
        directory = ROOT / "src/overlays/model_variant"

    def test_external_bindings_are_resident_function_starts(self):
        with (CONFIG / "functions.csv").open() as handle:
            starts = {int(row["address"], 0) for row in csv.DictReader(handle)}
        self.assertTrue(IMPORTS <= starts)
        for module in self.modules.values():
            layout = ROOT / module["layout"]
            symbols = layout.with_name(layout.stem + "_symbols.txt").read_text()
            bindings = {
                int(value, 16)
                for value in re.findall(
                    r"= (0x[0-9A-F]+); // type:func absolute:true", symbols
                )
            }
            self.assertEqual(bindings, IMPORTS)
            linker_bindings = {
                int(value, 16)
                for value in re.findall(
                    r"= (0x[0-9A-F]+);",
                    (ROOT / module["linker_symbols"]).read_text(),
                )
            }
            self.assertTrue(IMPORTS <= linker_bindings)


if __name__ == "__main__":
    unittest.main()
