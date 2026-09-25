#include "../../types.h"
#include "../../psyq/stdio.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_DUEL_COLLECT_MATCHING_FIELD_CARD_OBJECTS
/* The "Ex %d\n" format the Japanese image keeps at 0x8009AE84. */
extern char D_8009AE84[];
#define DUEL_FIELD_CARD_OBJECTS_EX_FORMAT D_8009AE84
#define D_801A7AD8 gJapanese_DuelCardRecords
#define Duel_CollectMatchingFieldCardObjects func_8002C780
#include "../duel_field_card_objects.c"
