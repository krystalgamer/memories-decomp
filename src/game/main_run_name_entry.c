#include "../types.h"
#include "../overlays/password/name_entry_keyboard.h"
#include "file_transfer.h"
#include "main_services.h"

extern u8 D_8009B269;

u8 D_8009B26C;

void Main_RunNameEntry(void)
{
    u8 flags = D_8009B26C;
    u8 value;

    if ((flags & 0x40) == 0) {
        D_8009B26C = flags | 0x40;
        File_RequestNameEntryPackage();
        NameEntry_Init();
    }
    if (NameEntry_PollCompletion()) {
        value = D_8009B269;
        D_8009B26C = value;
    }
}
