/* $PSLibId: Run-time Library Release 4.6$ */
#ifndef _LIBPRESS_H_
#define _LIBPRESS_H_

#include "../types.h"

/*
 *  (C) Copyright 1995 Sony Corporation,Tokyo,Japan. All Rights Reserved
 *
 * 		libpress.h: Prototypes for libpress
 *
 */
/* DecDCTvlc Table */
typedef	u16 DECDCTTAB[34816];

/* DecDCTEnv */
typedef struct {
	u8	iq_y[64];	/* IQ (Y): zig-zag order */
	u8	iq_c[64];	/* IQ (Cb,Cr): zig-zag order */
	short	dct[64];	/* IDCT coef (reserved) */
} DECDCTENV;

typedef struct {
    short *src;			/* 16-bit strait PCM */
    short *dest;		/* PlayStation original waveform data */
    short *work;		/* scratch pad or NULL */
    long   size;		/* size (unit: byte) of source data */
    long   loop_start;		/* loop start point (unit: byte) of source data */
    char   loop;		/* whether loop or not */
    char   byte_swap;		/* source data is 16-bit big endian (1) / little endian (0) */
    char   proceed;		/* proceeding ? whole (0) / start (1) / cont. (2) / end (4) */
    char   quality;		/* quality ? middle (0) / high (1) */
} ENCSPUENV;


#define ENCSPU_ENCODE_ERROR    (-1)
#define ENCSPU_ENCODE_WHOLE     0
#define ENCSPU_ENCODE_START    (1<<0)
#define ENCSPU_ENCODE_CONTINUE (1<<1)
#define ENCSPU_ENCODE_END      (1<<2)

#define ENCSPU_ENCODE_LOOP    1
#define ENCSPU_ENCODE_NO_LOOP 0

#define ENCSPU_ENCODE_ENDIAN_LITTLE 0
#define ENCSPU_ENCODE_ENDIAN_BIG    1

#define ENCSPU_ENCODE_MIDDLE_QULITY 0
#define ENCSPU_ENCODE_HIGH_QULITY   1


#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
extern void DecDCTReset(int mode);
extern DECDCTENV *DecDCTGetEnv(DECDCTENV *env);
extern DECDCTENV *DecDCTPutEnv(DECDCTENV *env);
extern int DecDCTBufSize(u32 *bs);
extern int DecDCTvlc(u32 *bs, u32 *buf);
extern int DecDCTvlc2(u32 *bs, u32 *buf, DECDCTTAB table);
extern int DecDCTvlcSize(int size);
extern int DecDCTvlcSize2(int size);
extern void DecDCTvlcBuild(u16 *table);
extern void DecDCTin(u32 *buf, int mode);
extern void DecDCTout(u32 *buf, int size);
extern int DecDCTinSync( int mode) ;
extern int DecDCToutSync( int mode) ;
extern int DecDCTinCallback(void (*func)());
extern int DecDCToutCallback(void (*func)());

extern long EncSPU (ENCSPUENV *env);
extern long EncSPU2(ENCSPUENV *env);
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif
#endif /* _LIBPRESS_H_ */
