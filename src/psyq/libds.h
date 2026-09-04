/* $PSLibId: Run-time Library Release 4.6$ */
/*
 * libds.h
 *	Copyright(C) 1996 1997, Sony Computer Entertainment Inc.
 *	All Rights Reserved.
 */

#ifndef _LIBDS_H_
#define _LIBDS_H_

#include "../types.h"

/*
 * CD-ROM Mode (used int CdlSetmode)
 */
#define DslModeStream	0x100	/* Normal Streaming                     */
#define DslModeStream2	0x120	/* SUB HEADER information includes      */
#define DslModeSpeed	0x80	/* 0: normal speed	1: double speed	*/
#define DslModeRT	0x40	/* 0: ADPCM off		1: ADPCM on	*/
#define DslModeSize1	0x20	/* 0: 2048 byte		1: 2340byte	*/
#define DslModeSize0	0x10	/* 0: -			1: 2328byte	*/
#define DslModeSF	0x08	/* 0: Channel off	1: Channel on	*/
#define DslModeRept	0x04	/* 0: Report off	1: Report on	*/
#define DslModeAP	0x02	/* 0: AutoPause off	1: AutoPause on */
#define DslModeDA	0x01	/* 0: CD-DA off		1: CD-DA on	*/

/*
 * Status contents
 */
#define DslStatPlay		0x80	/* playing CD-DA */
#define DslStatSeek		0x40	/* seeking */
#define DslStatRead		0x20	/* reading data sectors */
#define DslStatShellOpen	0x10	/* once shell open */
#define DslStatSeekError	0x04	/* seek error detected */
#define DslStatStandby		0x02	/* spindle motor rotating */
#define DslStatError		0x01	/* command error detected */

/*
 * Macros for DsGetDiskType()
 */
#define DslStatNoDisk		0x01
#define DslOtherFormat		0x02
#define DslCdromFormat		0x04

/*
 * CD-ROM Primitive Commands
 */
#define DslNop			0x01	/* no operation */
#define DslSetloc		0x02	/* set head position */
#define DslPlay			0x03	/* play CD-DA */
#define DslForward		0x04	/* forward DA play */
#define DslBackward		0x05	/* backward DA play */
#define	DslReadN		0x06	/* read data with retry */
#define DslStandby		0x07	/* start spindle motor */
#define DslStop			0x08	/* stop spindle motor */
#define DslPause		0x09	/* pause */
#define DslMute			0x0b	/* mute on */
#define DslDemute		0x0c	/* mute off */
#define DslSetfilter		0x0d	/* set subheader filter */
#define DslSetmode		0x0e	/* set mode */
#define DslGetparam		0x0f	/* get mode */
#define DslGetlocL		0x10	/* get head position (data sector) */
#define DslGetlocP		0x11	/* get head position (DA sector) */
#define DslGetTN		0x13	/* get number of TOC */
#define DslGetTD		0x14	/* get TOC data */
#define DslSeekL		0x15	/* logical seek */
#define DslSeekP		0x16	/* phisical seek */
#define DslReadS		0x1B	/* read data without retry */

/*
 * Interrupts
 */
#define DslNoIntr		0x00	/* No interrupt */
#define DslDataReady		0x01	/* Data Ready */
#define DslComplete		0x02	/* Command Complete */
#define DslAcknowledge		0x03	/* Acknowledge (reserved) */
#define DslDataEnd		0x04	/* End of Data Detected */
#define DslDiskError		0x05	/* Error Detected */

#define DslNoResult		0x06
#define DslFinished		0x07

#ifndef btoi
#define btoi( b )	( ( b ) / 16 * 10 + ( b ) % 16 )
#endif
#ifndef itob
#define itob( i )	( ( i ) / 10 * 16 + ( i ) % 10 )
#endif

/*
 * Position
 */
#define DslMAXTOC	100

/*
 * Callback
 */
typedef void ( *DslCB )( u8, u8* );
typedef void ( *DslRCB )( u8, u8*, u32* );

/*
 * Location
 */
typedef struct {
	u8 minute;		/* minute (BCD) */
	u8 second;		/* second (BCD) */
	u8 sector;		/* sector (BCD) */
	u8 track;		/* track (void) */
} DslLOC;

/*
 * ADPCM Filter
 */
typedef struct {
	u8 file;		/* file ID (always 1) */
	u8 chan;		/* channel ID */
	u16 pad;
} DslFILTER;

/*
 * Attenuator
 */
typedef struct {
	u8 val0;		/* volume for CD(L) -> SPU (L) */
	u8 val1;		/* volume for CD(L) -> SPU (R) */
	u8 val2;		/* volume for CD(R) -> SPU (L) */
	u8 val3;		/* volume for CD(R) -> SPU (R) */
} DslATV;

/*
 * Low Level File System for DsSearchFile()
 */
#define DslMAXFILE	64	/* max number of files in a directory */
#define DslMAXDIR	128	/* max number of total directories */
#define DslMAXLEVEL	8	/* max levels of directories */

typedef struct {
	DslLOC pos;		/* file location */
	u32 size;		/* file size */
	char name[ 16 ];	/* file name (body) */
} DslFILE;

#ifndef _LIBCD_H_
/*
 * Streaming Structures
 */
typedef struct {
	u16 id;
	u16 type;
	u16 secCount;
	u16 nSectors;
	u32 frameCount;
	u32 frameSize;

	u16 width;
	u16 height;
	u32 dummy1;
	u32 dummy2;
	DslLOC loc;
} StHEADER;		/* CD-ROM STR �\����*/

#define StFREE		0x0000
#define StREWIND	0x0001
#define StCOMPLETE	0x0002
#define StBUSY		0x0003
#define StLOCK		0x0004

#define EDC		0
#define SECTOR_SIZE	( 512 )	/* Sector Size (word) */
#define HEADER_SIZE	( 8 )	/* Header Size (word) */

#define StSTATUS	0x00
#define StVER		0x00
#define StTYPE		0x01
#define StSECTOR_OFFSET	0x02
#define StSECTOR_SIZE	0x03
#define StFRAME_NO	0x04
#define StFRAME_SIZE	0x06

#define StMOVIE_WIDTH	0x08
#define StMOVIE_HEIGHT	0x09

/*
 * �X�g���[�~���O���C�u�����v���g�^�C�v�錾
 */

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

void StSetRing( u32* ring_addr, u32 ring_size );
void StClearRing( void );
void StUnSetRing( void );
void StSetStream( u32 mode, u32 start_frame, u32 end_frame,
  void ( *func1 )(), void ( *func2 )() );
void StSetEmulate( u32* addr, u32 mode, u32 start_frame,
  u32 end_frame, void ( *func1 )(), void ( *func2 )() );
u32 StFreeRing( u32* base );
u32 StGetNext( u32** addr, u32** header );
u32 StGetNextS( u32** addr, u32** header );
u16 StNextStatus( u32** addr, u32** header );
void StRingStatus( short* free_sectors, short* over_sectors );
void StSetMask( u32 mask, u32 start, u32 end );
void StCdInterrupt( void );
int StGetBackloc( DslLOC* loc );
int StSetChannel( u32 channel );

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	/* _LIBCD_H_ */

/* **** �V�X�e���X�e�[�^�X *****/

#define DslReady	1
#define DslBusy		2
#define DslNoCD		3

/* **** �L���[�ɓo�^�ł���R�}���h�̍ő吔 *****/
#define DslMaxCOMMANDS	8

/* **** �R�}���h���s���ʂ̍ő吔 *****/
#define DslMaxRESULTS	8

/* **** DS �֐��v���g�^�C�v *****/

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif

int DsInit( void );
int DsReset( void );
void DsClose( void );
int DsCommand( u8 com, u8* param, DslCB cbsync, int count );
int DsPacket( u8 mode, DslLOC* pos, u8 com, DslCB func, int count );
DslCB DsSyncCallback( DslCB func );
DslCB DsReadyCallback( DslCB func );
int DsSync( int id, u8* result );
int DsReady( u8* result );
void DsFlush( void );
int DsSystemStatus( void );
int DsQueueLen( void );
u8 DsStatus( void );
int DsShellOpen( void );

int DsMix( DslATV* vol );
int DsGetSector( void* madr, int size );
int DsGetSector2( void* madr, int size );
int DsGetToc( DslLOC* loc );
void ( *DsDataCallback( void ( *func )() ) );
int DsDataSync( int mode );
DslLOC* DsIntToPos( int i, DslLOC* p );
int DsPosToInt( DslLOC* p );
int DsSetDebug( int level );
DslLOC* DsLastPos( DslLOC* p );
u8 DsLastCom( void );

char* DsComstr( u8 com );
char* DsIntstr( u8 intr );

int DsStartReadySystem( DslRCB func, int count );
void DsEndReadySystem( void );
int DsReadySystemMode( int mode );

int DsControlF( u8 com, u8* param );
int DsControl( u8 com, u8* param, u8* result );
int DsControlB( u8 com, u8* param, u8* result );

int DsRead( DslLOC* pos, int sectors, u32* buf, int mode );
int DsReadSync( u8* result );
DslCB DsReadCallback( DslCB func );
void DsReadBreak( void );
int DsRead2( DslLOC* pos, int mode );

DslFILE* DsSearchFile( DslFILE* fp, char* name );
int DsReadFile( char* file, u32* addr, int nbyte );
struct EXEC* DsReadExec( char* file );
int DsPlay( int mode, int* tracks, int offset );

int DsGetDiskType( void );

#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif

#endif	/* _LIBDS_H_ */
