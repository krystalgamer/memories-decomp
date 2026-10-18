"""Guard the actual helper's u16 storage versus full s32 transfer calculation."""

import os
from pathlib import Path
import platform
import shutil
import subprocess
import tempfile
import unittest

from tools.project.tests.test_library_display_objects import START


ROOT = Path(__file__).resolve().parents[3]
FIXTURE = r"""
typedef char ilp32[sizeof(void *) == 4 ? 1 : -1];
typedef struct {
    u8 before[16];
    DuelEffectResourceRecord records[4];
    u8 gap_0[16];
    FileTransferDescriptor descriptor;
    u8 gap_1[16];
    volatile u32 flags;
    u8 after[16];
} GuardedState;
extern GuardedState state;
#define gCardPreview_Resources state.records
#define D_8009B0F4_abs state.flags
"""
WITNESS = r"""
GuardedState state, expected;
static s32 input;
static u32 slot, error, calls, initial_flags;
void CardPreview_HandleResourceTransfer(FileTransferDescriptor *descriptor, s32 mode)
{
    error = 1;
}
FileTransferDescriptor *File_TryRequestAsyncTransfer(
    s32 bank, u8 *destination, s32 position, s32 sectors,
    FileTransferCallback callback, s32 a, s32 b)
{
    s64 exact = ((s64)input - 1) * 7 + CARD_COUNT;
    if (bank != 0 || destination != 0 || position != (s32)exact || sectors != 7 ||
        callback != CardPreview_HandleResourceTransfer || a != 0 || b != 0 || calls != 0 ||
        state.records[slot].card_id != ((u32)input & 0xFFFF) ||
        state.flags != initial_flags) error = 2;
    calls++;
    return &state.descriptor;
}
int main(void)
{
    static const u32 high[] = {0, 0x10000, 0xFFFF0000, 0x20000, 0xFFFE0000};
    u32 h, low, i, offset, cases = 0;
    for (slot = 0; slot < 4; slot++)
        for (h = 0; h < 5; h++)
            for (low = 0; low < 65536; low++) {
                input = (s32)(high[h] | low);
                for (i = 0; i < sizeof(state); i++) ((u8 *)&state)[i] = i * 37 + low + slot;
                state.descriptor.status_flags = (u32)input * 4099U ^ (slot << 28);
                state.flags = initial_flags = 0xAA55AA55 ^ low;
                for (i = 0; i < sizeof(state); i++) ((u8 *)&expected)[i] = ((u8 *)&state)[i];
                offset = (u8 *)&expected.records[slot] - (u8 *)&expected;
                ((u8 *)&expected)[offset + 0x30] = low;
                ((u8 *)&expected)[offset + 0x31] = low >> 8;
                expected.descriptor.callback_data = (void *)slot;
                expected.flags = state.descriptor.status_flags | 0x10;
                calls = error = 0;
                if (CardPreview_RequestResourceTransfer(slot, input) != &state.descriptor || error || calls != 1)
                    return 1;
                for (i = 0; i < sizeof(state); i++)
                    if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 1;
                cases++;
            }
    return cases == 1310720 ? 0 : 2;
}
"""


class PreviewResourceCardIdTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_complete_helper_and_regression_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/duel_effect_resource_setup.c").read_text()
        start = source.index("FileTransferDescriptor *CardPreview_RequestResourceTransfer(")
        end = source.index("\n}\n", start) + 3
        prefix, original = source[:start], source[start:end]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "truncated-position-input"),
            ("-O2", "missing-active-bit"), ("-O2", "wide-id-store"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = original
                if mutation == "truncated-position-input":
                    code = code.replace(
                        "DUEL_RESOURCE_TRANSFER_POSITION(card_id)",
                        "DUEL_RESOURCE_TRANSFER_POSITION(gCardPreview_Resources[slot].card_id)",
                    )
                elif mutation == "missing-active-bit":
                    code = code.replace(
                        "object->status_flags | FILE_TRANSFER_STATE_PRIMARY_ACTIVE",
                        "object->status_flags",
                    )
                elif mutation == "wide-id-store":
                    code = code.replace(
                        "gCardPreview_Resources[slot].card_id = card_id;",
                        "*(u32 *)&gCardPreview_Resources[slot].card_id = card_id;",
                    )
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, original)
                with tempfile.TemporaryDirectory(prefix="preview-resource-id-", dir=ROOT / "tmp") as name:
                    directory = Path(name)
                    operations = directory / "operations.c"
                    operations.write_text(prefix + FIXTURE + code)
                    witness = directory / "witness.c"
                    witness.write_text(prefix + FIXTURE + WITNESS)
                    start_file = directory / "start.S"
                    start_file.write_text(START)
                    binary = directory / "witness"
                    environment = dict(os.environ, TMPDIR=str(directory))
                    result = subprocess.run(
                        [shutil.which("cc"), "-m32", "-std=gnu89", optimization,
                         "-nostdlib", "-fno-pie", "-no-pie", "-fno-stack-protector",
                         "-ffreestanding", "-fno-builtin", "-fno-strict-aliasing",
                         "-Werror=incompatible-pointer-types", "-I", str(ROOT / "src/game"),
                         str(operations), str(witness), str(start_file), "-o", str(binary)],
                        cwd=ROOT, env=environment, text=True, capture_output=True, timeout=60,
                    )
                    self.assertEqual(result.returncode, 0, result.stderr[-4000:])
                    result = subprocess.run([str(binary)], cwd=ROOT, env=environment, timeout=180)
                    self.assertEqual(result.returncode, 1 if mutation else 0)


if __name__ == "__main__":
    unittest.main()
