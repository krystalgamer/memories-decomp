#ifndef MEMORIES_DECOMP_DISC_LOCATION_H
#define MEMORIES_DECOMP_DISC_LOCATION_H

#include "../types.h"
#include "../psyq/libcd.h"
#include "../psyq/libds.h"

/* A disc position (minute, second, sector, track). libcd and libds each
 * declare the same four bytes under their own name, and the transfer and movie
 * runtimes hand one position to both libraries, so the storage is a union of
 * the two SDK types. */
typedef union {
    CdlLOC cd;
    DslLOC ds;
} DiscLocation;

#endif
