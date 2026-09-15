#ifndef MEMORIES_DECOMP_MOVIE_STREAM_REQUESTS_H
#define MEMORIES_DECOMP_MOVIE_STREAM_REQUESTS_H

#include "../types.h"
#include "../psyq/libcd.h"

/* File_Exists fills this CdlFILE record before Movie_StartFileStream reads its
 * sector location and byte size. The alias adds no second linker symbol. */
extern CdlFILE gMovie_FileLookupResult asm("D_800F5750");

void func_8005C388(
    s32 index, s32 arg1, s32 end_frame, s32 arg3, s32 arg4
);
/* Starts a movie from a disc file looked up by name. File_Exists fills the
 * CdlLOC and byte size in gMovie_FileLookupResult; the stream runs from there
 * to the file's start sector plus its size rounded up to whole sectors,
 * handing the frame range and the two mode arguments to func_8005B8A0
 * unchanged. Returns -1 when the file is not found, otherwise
 * func_8005B8A0's result. */
s32 Movie_StartFileStream(s32 path, s32 start_frame, s32 end_frame, s32 arg3,
                          s32 arg4);

#endif
