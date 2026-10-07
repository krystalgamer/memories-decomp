import csv
import hashlib
import json

from tools.project.hashing import sha256_file
from tools.project.progress import load_french_overlay_inventories
from tools.project.tests import test_spanish_model_variant456_lines as spanish
from tools.project.verify_inputs import load_checksum_manifest


class FrenchModelVariant456LinesTests(spanish.SpanishModelVariant456LinesTests):
    region = "france"
    config_name = "sles_03948"
    resident_name = "SLES_039.48"
    module_prefix = "french"
    load_inventories = staticmethod(load_french_overlay_inventories)
    binding_count = 34
    streamers = False
    ribbons = False
    sheets = False

    def terminal_attempts(self):
        with (spanish.ROOT / "notes/overlays/french-model-variant456-lines-attempts.csv").open() as handle:
            attempts = list(csv.DictReader(handle))
        self.assertEqual(len(attempts), 6)
        self.assertEqual({row["module"] for row in attempts}, set(self.instances))
        self.assertEqual({row["result"] for row in attempts}, {"matched"})
        return attempts

    def test_independent_french_archive_and_accepted_spanish_payload_identity(self):
        archive = spanish.ROOT / "game/france/DATA/MODEL.MRG"
        if not archive.is_file():
            self.skipTest("Legal French MODEL input required")
        checksums = load_checksum_manifest(self.config / "files.sha256")
        self.assertEqual(sha256_file(archive), checksums["game/france/DATA/MODEL.MRG"])
        accepted = {module["name"]: module for module in json.loads(
            (spanish.ROOT / "config/sles_03951/overlays.json").read_text())["modules"]}
        for module, image in self.legal_images():
            donor = accepted[module["name"].replace("french_", "spanish_", 1)]
            self.assertEqual((module["sha256"], module["sector_offset"], module["load_address"]),
                             (donor["sha256"], donor["sector_offset"], donor["load_address"]))
            self.assertEqual(hashlib.sha256(image).hexdigest(), donor["sha256"])
