#include "../../types.h"

#define VERSION_JAPAN
#define VERSION_JAPAN_OPTIONS_INIT
#define gOptions_bOutputType gJapanese_OptionsOutputType
#define D_8009B380 gJapanese_OptionsOutputCursor
#define D_8009B388 gJapanese_OptionsSelectionCursor
#define gOptions_bSelection gJapanese_OptionsSelection
#define gOptions_bState gJapanese_OptionsState
#define Options_InitTextDisplay func_8003BAAC
#define Options_Init func_8003BBF4
#include "../options_screen.c"
