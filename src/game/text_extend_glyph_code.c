#include "../types.h"
#include "../unmatched.h"
#include "text_constants.h"
#include "text_stream_commands.h"

/* Small globals other units own, declared here as common symbols the
 * way the original read them through $gp. */
volatile s32 gText_CommandDisposition;
volatile u16 gText_CurrentStreamCode;

void Text_ExtendGlyphCode(TextStreamOwner *owner)
{
    volatile u16 *code = &gText_CurrentStreamCode;

    /* Combine the F0-F5 prefix with the next stream byte and publish the
     * extended code before signaling completion. */
    *code = ((*code - TEXT_SINGLE_BYTE_GLYPH_LIMIT) << 8)
        | *owner->streams[owner->stream_index]++;
    gText_CommandDisposition = -1;
}
