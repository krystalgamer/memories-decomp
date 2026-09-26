#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_OPTIONS_INIT_TEXT_DISPLAY
#define OPTIONS_TEXT_BOX_STRING 0xEE
#define D_8009B380 gJapanese_OptionsOutputCursor
#define D_8009B388 gJapanese_OptionsSelectionCursor
#define Options_InitTextDisplay func_8003BAAC
#define Options_Init func_8003BBF4
#include "../options_screen.c"
