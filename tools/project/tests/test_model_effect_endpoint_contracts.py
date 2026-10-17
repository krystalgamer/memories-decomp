"""Check the effect endpoint sequence helper's record-pointer contract."""

import os
from pathlib import Path
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[3]
COMPILER = ROOT / "tools/toolchains/gcc-2.8.1-psx/bin/mips-sony-psx-gcc"


@unittest.skipUnless(COMPILER.is_file(), "requires supported GCC 2.8.1")
class ModelEffectEndpointContractTests(unittest.TestCase):
    def test_both_sequence_parameters_use_the_endpoint_record(self):
        endpoint = "ModelEffectEndpoint *"
        cases = (
            (endpoint, endpoint, True),
            ("void *", endpoint, False),
            (endpoint, "void *", False),
        )
        with tempfile.TemporaryDirectory(
            prefix="effect-endpoint-contract-", dir=ROOT / "tmp"
        ) as name:
            directory = Path(name)
            environment = os.environ.copy()
            environment["TMPDIR"] = str(directory)
            for first, second, accepted in cases:
                with self.subTest(first=first, second=second):
                    source = directory / "probe.c"
                    source.write_text(
                        '#include "../../src/types.h"\n'
                        '#include "../../src/game/model_effect_state.h"\n'
                        f"void (*checked)(s32, {first}, {second}, s32)"
                        " = Model_QueueEffectEndpointSequences;\n"
                    )
                    result = subprocess.run(
                        [
                            str(COMPILER), "-S", "-O2", "-G0", "-Werror",
                            "-o", str(directory / "probe.s"), str(source),
                        ],
                        cwd=ROOT, env=environment, text=True,
                        capture_output=True, timeout=60,
                    )
                    if accepted:
                        self.assertEqual(result.returncode, 0, result.stderr)
                    else:
                        self.assertNotEqual(result.returncode, 0, result.stderr)
                        self.assertIn("incompatible pointer type", result.stderr)


if __name__ == "__main__":
    unittest.main()
