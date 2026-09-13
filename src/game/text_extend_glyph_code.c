#include "../types.h"
#include "../unmatched.h"
#include "text_constants.h"
#include "text_stream_commands.h"

void Text_ExtendGlyphCode(u8 *object)
{
    TextStreamOwner *owner = (TextStreamOwner *)object;
    volatile u16 *code = &D_8009B33A;
    s32 index;
    u32 combined;
    u32 value;
    u8 **slot;
    u8 *cursor;

    index = owner->stream_index;
    combined = *code;
    slot = &owner->streams[index];
    combined -= TEXT_SINGLE_BYTE_GLYPH_LIMIT;
    cursor = *slot;
    combined <<= 8;
    value = *cursor++;
    *slot = cursor;
    value |= combined;
    /* Publish the extended code before signaling completion. */
    *code = value;
    D_8009B350 = -1;
}
