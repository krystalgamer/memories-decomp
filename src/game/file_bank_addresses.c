#include "../types.h"
#include "european_text_bank_views.h"
#include "file_transfer.h"
#include "high_memory_addresses.h"

/* The read-only transfer-destination words at 0x800101D8, between
   duel_scene's and library's read-only data. They are eight bytes (twenty in
   the European builds), which a -G8 unit would put in .sdata, so they are a
   -G0 unit of their own. See notes/high-memory-load-addresses.md. */
u8 *const gFile_pScreenBankBase = (u8 *)0x80168000;
#ifdef VERSION_JAPAN
u8 *const gFile_pDuelBankBase = (u8 *)0x80154000;
#else
u8 *const gFile_pDuelBankBase = (u8 *)0x80146000;
#endif
#ifdef VERSION_EUROPE
u8 *const gFile_pBootTextStage5Destination = (u8 *)0x801B0000;
u8 *const gFile_pBootTextStage6Destination = (u8 *)0x801C0000;
u8 *const gFile_pBootTextStage7Destination = (u8 *)0x801D5800;
#endif
