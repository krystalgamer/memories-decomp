#ifndef _LIBAPI_H_
#define _LIBAPI_H_

/*
 *	File:libapi.h
 *	Copyright (C) 1997 by Sony Computer Entertainment Inc.
 *			All rights Reserved
 */
/*
 * $PSLibId: Run-time Library Release 4.6$
 */

#ifndef _R3000_H
#include "r3000.h"
#endif
#ifndef _ASM_H
#include "asm.h"
#endif
#ifndef _KERNEL_H
#include "kernel.h"
#endif

/* don't change these macros and structures which is referred in controler code */

/*
 * Prototypes
 */

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

/* prototypes added by suzu 96/03/01 and changed by hakama 96/06/06*/

extern PSXLONG SetRCnt(unsigned PSXLONG, unsigned short, PSXLONG);
extern PSXLONG GetRCnt(unsigned PSXLONG);
extern PSXLONG ResetRCnt(unsigned PSXLONG);
extern PSXLONG StartRCnt(unsigned PSXLONG);
extern PSXLONG StopRCnt(unsigned PSXLONG);

extern PSXLONG OpenEvent(unsigned PSXLONG,PSXLONG,PSXLONG,PSXLONG (*func)());
extern PSXLONG CloseEvent(PSXLONG);
extern PSXLONG WaitEvent(PSXLONG);
extern PSXLONG TestEvent(PSXLONG);
extern PSXLONG EnableEvent(PSXLONG);
extern PSXLONG DisableEvent(PSXLONG);
extern void DeliverEvent(unsigned PSXLONG, unsigned PSXLONG);
extern void UnDeliverEvent(unsigned PSXLONG, unsigned PSXLONG);

extern PSXLONG OpenTh(PSXLONG (*func)(), unsigned PSXLONG , unsigned PSXLONG);
extern int CloseTh(PSXLONG);
extern int ChangeTh(PSXLONG);

extern PSXLONG open(char *, unsigned PSXLONG);
extern PSXLONG close(PSXLONG);
extern PSXLONG lseek(PSXLONG, PSXLONG, PSXLONG);
extern PSXLONG read(PSXLONG, void *, PSXLONG);
extern PSXLONG write(PSXLONG, void *, PSXLONG);
extern PSXLONG ioctl(PSXLONG, PSXLONG, PSXLONG);
extern struct DIRENTRY * firstfile(char *, struct DIRENTRY *);
extern struct DIRENTRY * nextfile(struct DIRENTRY *);
extern PSXLONG erase(char *);


extern PSXLONG undelete(char *);
extern PSXLONG format(char *);
extern PSXLONG rename(char *, char *);
extern PSXLONG cd(char *);

extern PSXLONG LoadTest(char *, struct EXEC *);
extern PSXLONG Load(char *, struct EXEC *);
extern PSXLONG Exec(struct EXEC *, PSXLONG, char **);
extern PSXLONG LoadExec(char *, unsigned PSXLONG, unsigned PSXLONG);

extern PSXLONG InitPAD(char *,PSXLONG ,char *,PSXLONG);
extern PSXLONG StartPAD(void);
extern void StopPAD(void);
extern void EnablePAD(void);
extern void DisablePAD(void);

extern void FlushCache(void);
extern void ReturnFromException(void);
extern int  EnterCriticalSection(void);
extern void ExitCriticalSection(void);
extern void Exception(void);
extern void SwEnterCriticalSection(void);
extern void SwExitCriticalSection(void);

extern unsigned PSXLONG SetSp(unsigned PSXLONG);
extern unsigned PSXLONG GetSp( void );
extern unsigned PSXLONG GetGp( void );
extern unsigned PSXLONG GetCr( void );
extern unsigned PSXLONG GetSr( void );
extern unsigned PSXLONG GetSysSp(void);

extern PSXLONG SetConf(unsigned PSXLONG,unsigned PSXLONG,unsigned PSXLONG);
extern void GetConf(unsigned PSXLONG *,unsigned PSXLONG *,unsigned PSXLONG *);

extern PSXLONG _get_errno(void);
extern PSXLONG _get_error(PSXLONG);

extern void SystemError( char, PSXLONG);
extern void SetMem(PSXLONG);

extern PSXLONG Krom2RawAdd( unsigned PSXLONG );
extern PSXLONG Krom2RawAdd2(unsigned short);

extern void _96_init(void);
extern void _96_remove(void);
extern void _boot(void);

extern void ChangeClearPAD( PSXLONG );

/* prototypes added by shino 96/05/22 */
extern void InitCARD(PSXLONG val);
extern PSXLONG StartCARD(void);
extern PSXLONG StopCARD(void);
extern void _bu_init(void);
extern PSXLONG _card_info(PSXLONG chan);
extern PSXLONG _card_clear(PSXLONG chan);
extern PSXLONG _card_load(PSXLONG chan);
extern PSXLONG _card_auto(PSXLONG val);
extern void _new_card(void);
extern PSXLONG _card_status(PSXLONG drv);
extern PSXLONG _card_wait(PSXLONG drv);
extern unsigned PSXLONG _card_chan(void);
extern PSXLONG _card_write(PSXLONG chan, PSXLONG block, unsigned char *buf);
extern PSXLONG _card_read(PSXLONG chan, PSXLONG block, unsigned char *buf);
extern PSXLONG _card_format(PSXLONG chan);	/* added by iwano 98/03/24 */



#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif /* _LIBAPI_H_ */

/* don't add stuff after this */
