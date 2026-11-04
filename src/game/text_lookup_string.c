#include "../types.h"
#include "text_constants.h"
#include "text_string_bank_views.h"
#include "european_text_bank_views.h"
#include "text_stream_commands.h"

#ifdef VERSION_EUROPE
u8 *Text_LookupString(s32 unused_word, s32 string_id)
{
    /* The European banks, as in TextBox_BuildStep. */
    if (string_id <= 0x7FFF) {
        if (string_id >= 0x500) {
            string_id -= 0x100;
            return (u8 *)(((u32)gText_AlternateStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) |
                gText_PrimaryStringOffsets[string_id]);
        }
        return (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[string_id]);
    }
    return (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
        gText_GlobalStringOffsets[string_id - 0x8000]);
}
#else
u8 *Text_LookupString(s32 unused_word, s32 string_id)
{
    s32 index = string_id;
#ifdef VERSION_JAPAN_TEXT_LOOKUP_STRING
    if (index & TEXT_GLOBAL_STRING_ID_BASE)
        return (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
            gText_GlobalStringOffsets[index & (TEXT_GLOBAL_STRING_ID_BASE - 1)]);
#else
    if (index > 0xCFFF)
        return (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) |
            gText_PrimaryStringOffsets[index - 0xD000]);
    if (index > (TEXT_GLOBAL_STRING_ID_BASE - 1))
        return (u8 *)(((u32)gText_GlobalStringOffsets & TEXT_BANK_ADDRESS_MASK) |
            gText_GlobalStringOffsets[index - TEXT_GLOBAL_STRING_ID_BASE]);
#endif
    if (index >= 0x500)
        index -= 0x100;
#ifdef VERSION_JAPAN_TEXT_LOOKUP_STRING
    return (u8 *)(((u32)gText_PrimaryStringOffsets & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[index]);
#else
    return (u8 *)(((u32)gText_PrimaryStringAddressAnchor & TEXT_BANK_ADDRESS_MASK) | gText_PrimaryStringOffsets[index]);
#endif
}
#endif

u8 TextStream_ReadByte(TextStreamOwner *object)
{
    return *object->streams[object->stream_index]++;
}
