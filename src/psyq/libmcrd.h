#ifndef _MEMCARD_H_
#define _MEMCARD_H_
/*
 * File:libmcrd.h	Rev. 4.2
*/
/*
 * $PSLibId: Run-time Library Release 4.6$
 */
#include "../types.h"

#ifndef _R3000_H
#include "r3000.h"
#endif
#ifndef _ASM_H
#include "asm.h"
#endif
#include "kernel.h"

typedef void (*MemCB)( PSXLONG cmds, PSXLONG rslt );

#define McFuncExist		(1)
#define McFuncAccept		(2)
#define McFuncReadFile		(3)
#define McFuncWriteFile		(4)
#define McFuncReadData		(5)
#define McFuncWriteData		(6)

#define	McErrNone		(0)
#define	McErrCardNotExist	(1)
#define	McErrCardInvalid	(2)
#define	McErrNewCard		(3)
#define	McErrNotFormat		(4)
#define	McErrFileNotExist	(5)
#define	McErrAlreadyExist	(6)
#define	McErrBlockFull		(7)
#define	McErrExtend		(0x8000)

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void MemCardInit( PSXLONG val );
void MemCardEnd( void );
void MemCardStart(void);
void MemCardStop(void);
PSXLONG MemCardExist( PSXLONG chan );
PSXLONG MemCardAccept( PSXLONG chan );
PSXLONG MemCardOpen( PSXLONG chan, char* file, PSXLONG flag );
void MemCardClose(void);
PSXLONG MemCardReadData( unsigned PSXLONG* adrs, PSXLONG ofs, PSXLONG bytes );
PSXLONG MemCardReadFile( PSXLONG chan, char* file, unsigned PSXLONG* adrs, PSXLONG ofs, PSXLONG bytes );
PSXLONG MemCardWriteData( unsigned PSXLONG* adrs, PSXLONG ofs, PSXLONG bytes );
PSXLONG MemCardWriteFile( PSXLONG chan, char* file, unsigned PSXLONG* adrs, PSXLONG ofs ,PSXLONG bytes );
PSXLONG MemCardCreateFile( PSXLONG chan, char* file, PSXLONG blocks );
PSXLONG MemCardDeleteFile( PSXLONG chan, char* file );
PSXLONG MemCardFormat( PSXLONG chan );
PSXLONG MemCardUnformat(PSXLONG chan);
PSXLONG MemCardSync( PSXLONG mode, PSXLONG* cmds, PSXLONG* rslt );
MemCB MemCardCallback( MemCB func );
PSXLONG MemCardGetDirentry( PSXLONG chan, char* name, struct DIRENTRY* dir, PSXLONG* files, PSXLONG ofs, PSXLONG max );

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif


#endif /* _MEMCARD_H_ */
