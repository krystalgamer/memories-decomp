OUTPUT_ARCH(mips)
ENTRY(entrypoint)

INCLUDE tmp/splat/undefined_funcs_auto.txt
INCLUDE tmp/splat/undefined_syms_auto.txt
INCLUDE config/slus_01411/c_symbols.ld

_gp = 0x8009AF08;
loaded_image_end = 0x801E0000;

SECTIONS
{
    .header 0x00000000 : AT(0x000000)
    {
        KEEP(*header.o(.data))
    }
    ASSERT(SIZEOF(.header) == 0x800, "PS-X EXE header size mismatch")

    .initial_data 0x80010000 : AT(0x000800)
    {
        KEEP(*initial_data.o(.data))
    }
    ASSERT(SIZEOF(.initial_data) == 0x194C, "initial data size mismatch")

    /* Pre-text read-only data owned by a matching C object. A compiler
       generated jump table lands in that object's .rodata, and retail places
       it inside the leading data region rather than after the text, so it
       gets its own output section at its own VRAM. SUBALIGN is scoped here
       because GCC marks the table 8-byte aligned while retail places it on a
       4-byte boundary; applying it to the whole blob would move other data. */
    .jtbl_rodata 0x8001194C : AT(0x00214C) SUBALIGN(4)
    {
        KEEP(*c_80070738.o(.rodata))
    }
    ASSERT(SIZEOF(.jtbl_rodata) == 0x1C, "jump table rodata size mismatch")

    .initial_data_tail 0x80011968 : AT(0x002168)
    {
        KEEP(*initial_data_002168.o(.data))
    }
    ASSERT(SIZEOF(.initial_data_tail) == 0x1070, "initial data tail mismatch")

    .text 0x800129D8 : AT(0x0031D8)
    {
        KEEP(*(.text))
    }
    ASSERT(SIZEOF(.text) == 0x7DCFC, "resident text size mismatch")

    .text_padding 0x800906D4 : AT(0x080ED4)
    {
        FILL(0x00);
        . += 0xC;
    }
    ASSERT(SIZEOF(.text_padding) == 0xC, "text padding size mismatch")

    .initialized_data 0x800906E0 : AT(0x080EE0)
    {
        KEEP(*initialized_data.o(.data))
    }
    ASSERT(SIZEOF(.initialized_data) == 0xA9B0, "initialized data size mismatch")

    .bss_image 0x8009B090 : AT(0x08B890)
    {
        KEEP(*bss_image.o(.data))
    }
    ASSERT(SIZEOF(.bss_image) == 0x63698, "BSS image size mismatch")

    .reserved_zero 0x800FE728 : AT(0x0EEF28)
    {
        KEEP(*reserved_zero.o(.data))
    }
    ASSERT(SIZEOF(.reserved_zero) == 0x3B8D8, "reserved zero size mismatch")

    .overlay_slots 0x8013A000 : AT(0x12A800)
    {
        KEEP(*overlay_slot_*.o(.data))
    }
    ASSERT(SIZEOF(.overlay_slots) == 0x72000, "overlay slot size mismatch")

    .tail_data 0x801AC000 : AT(0x19C800)
    {
        KEEP(*tail_data.o(.data))
    }
    ASSERT(SIZEOF(.tail_data) == 0x34000, "tail data size mismatch")

    ASSERT(
        LOADADDR(.tail_data) + SIZEOF(.tail_data) == 0x1D0800,
        "rebuilt PS-X EXE size mismatch"
    )

    /DISCARD/ :
    {
        *(.comment)
        *(.gnu.attributes)
        *(.MIPS.abiflags)
        *(.note*)
        *(.pdr)
        *(.reginfo)
    }
}
