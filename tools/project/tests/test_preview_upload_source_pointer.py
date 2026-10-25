"""Exercise actual transfer callback phases with real buffers and a LoadImage stub."""

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
    u8 *G32 buffer;
    u8 after[16];
} GuardedState;
extern GuardedState state;
#define gCardPreview_Resources state.records
#define gFile_dwTransferState state.flags
#define gFile_TransferBufferBase state.buffer
"""
WITNESS = r"""
GuardedState state, expected;
static union { u32 alignment; u8 bytes[0x10000]; } staging;
static u32 slot, value, calls, error, base_offset;
static const u32 offsets[4] = {0, 0x2640, 0x2840, 0x2AE0};
static void put16(void *p, u32 v) { ((u8 *)p)[0] = v; ((u8 *)p)[1] = v >> 8; }
static void rectangle(u8 *p, u32 x, u32 y, u32 w, u32 h)
{
    put16(p, x); put16(p + 2, y); put16(p + 4, w); put16(p + 6, h);
}
int LoadImage(RECT *rect, u32 *source)
{
    u32 i;
    if (calls >= 4) { error = 1; return 0; }
    if (rect != &state.records[slot].rects[calls] ||
        (u8 *)source != staging.bytes + base_offset + calls * 0x100 + offsets[calls])
        error = 2;
    for (i = 0; i < sizeof(RECT); i++)
        if (((u8 *)rect)[i] != ((u8 *)&expected.records[slot].rects[calls])[i]) error = 3;
    if (state.flags != expected.flags) error = 4;
    calls++;
    state.buffer = staging.bytes + base_offset + calls * 0x100;
    return 0;
}
int main(void)
{
    static const s32 modes[4] = {0, 1, -1, 2147483647};
    u32 mode, i, cases = 0;
    u8 *rects;
    for (i = 0; i < sizeof(staging.bytes); i++) staging.bytes[i] = i * 13 + 7;
    for (slot = 0; slot < 4; slot++)
        for (mode = 0; mode < 4; mode++)
            for (value = 0; value < 65536; value++) {
                for (i = 0; i < sizeof(state); i++) ((u8 *)&state)[i] = i * 37 + slot + value;
                base_offset = (value & 3) * 4;
                state.buffer = staging.bytes + base_offset;
                state.descriptor.callback_data = (void *)slot;
                state.flags = value * 4099U ^ (slot << 28);
                state.records[slot].src_x = value;
                state.records[slot].src_y = value ^ 0x8000;
                state.records[slot].field_2C = value ^ 0xFFFF;
                state.records[slot].field_2E = value ^ 0x5A5A;
                for (i = 0; i < sizeof(state); i++) ((u8 *)&expected)[i] = ((u8 *)&state)[i];
                if (mode == 0) {
                    expected.descriptor.phase_size = 0x3800;
                    expected.flags &= 0xFFDCFFFF;
                    expected.descriptor.alternate_sector_destination = (u32)state.buffer;
                    expected.descriptor.sector_destination = (u32)state.buffer;
                    expected.descriptor.transfer_mode = 1;
                } else {
                    rects = (u8 *)&expected.records[slot] + 8;
                    rectangle(rects, value, value ^ 0x8000, 0x33, 0x60);
                    rectangle(rects + 8, value ^ 0xFFFF, value ^ 0x5A5A, 0x100, 1);
                    rectangle(rects + 16, value, (value ^ 0x8000) + 0x60, 0x18, 0xE);
                    rectangle(rects + 24, value + 0x38, value ^ 0x8000, 8, 0x58);
                    expected.buffer = staging.bytes + base_offset + 0x400;
                }
                calls = error = 0;
                CardPreview_HandleResourceTransfer(&state.descriptor, modes[mode]);
                if (error || calls != (mode == 0 ? 0 : 4)) return 1;
                for (i = 0; i < sizeof(state); i++)
                    if (((u8 *)&state)[i] != ((u8 *)&expected)[i]) return 1;
                cases++;
            }
    for (i = 0; i < sizeof(staging.bytes); i++)
        if (staging.bytes[i] != (u8)(i * 13 + 7)) return 1;
    return cases == 1048576 ? 0 : 2;
}
"""


class PreviewUploadSourcePointerTests(unittest.TestCase):

    @unittest.skipUnless(
        platform.system() == "Linux" and platform.machine() in ("x86_64", "i386", "i686")
        and shutil.which("cc"), "requires a Linux x86 compiler and ILP32 execution",
    )
    def test_actual_complete_callback_with_reload_and_scaling_controls(self):
        if Path.cwd().resolve() != ROOT:
            raise RuntimeError("run these tests from the repository root")
        source = (ROOT / "src/game/card_preview_handle_resource_transfer.c").read_text()
        start = source.index("void CardPreview_HandleResourceTransfer(")
        prefix, original = source[:start], source[start:]
        for optimization, mutation in (
            ("-O0", None), ("-O2", None), ("-O2", "scaled-palette-offset"),
            ("-O2", "stale-buffer-base"), ("-O2", "wrong-rectangle"),
            ("-O2", "wrong-flag-mask"),
        ):
            with self.subTest(optimization=optimization, mutation=mutation):
                code = original
                if mutation == "scaled-palette-offset":
                    code = code.replace("gFile_TransferBufferBase + 0x2640", "gFile_TransferBufferBase + 0x9900")
                elif mutation == "stale-buffer-base":
                    code = code.replace("image_source = gFile_TransferBufferBase + 0x2640;",
                                        "image_source = image_source + 0x2640;")
                elif mutation == "wrong-rectangle":
                    code = code.replace("rect = &e->rects[1];", "rect = &e->rects[0];")
                elif mutation == "wrong-flag-mask":
                    code = code.replace("0xFFDCFFFF", "0xFFDDFFFF")
                else:
                    self.assertIsNone(mutation)
                if mutation:
                    self.assertNotEqual(code, original)
                with tempfile.TemporaryDirectory(prefix="preview-upload-pointer-", dir=ROOT / "tmp") as name:
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
