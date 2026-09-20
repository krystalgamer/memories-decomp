#include "../types.h"
#include "model_word_memory.h"

void Model_FillWords(s32 *destination, s32 value, u32 count)
{
    while (count-- != 0) {
        *destination++ = value;
    }
}

void Model_CopyWords(s32 *destination, const s32 *source, u32 count)
{
    while (count-- != 0) {
        *destination++ = *source++;
    }
}
