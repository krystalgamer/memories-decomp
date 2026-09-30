/*
 * File:fs.h
 */
/*
 * $PSLibId: Run-time Library Release 4.6$
 */

#ifndef _FS_H
#define _FS_H

#if defined(_LANGUAGE_C)||defined(LANGUAGE_C)||defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)

/* device table */
struct device_table {
	char *G32 dt_string;	/* device name */
	int dt_type;		/* device "type" */
	int dt_bsize;		/* file system type */
	char *G32 dt_desc;		/* device description */
	int (*G32 dt_init)();	/* device init routine */
	int (*G32 dt_open)();	/* device open routine */
	int (*G32 dt_strategy)();	/* device strategy routine, returns cnt */
	int (*G32 dt_close)();	/* device close routine */
	int (*G32 dt_ioctl)();	/* device ioctl routine */
	int (*G32 dt_read)();	/* fs read routine, returns count */
	int (*G32 dt_write)();	/* fs write routine, return count */
	int (*G32 dt_delete)();	/* file delete routine */
	int (*G32 dt_undelete)();	/* file delete routine */
	int (*G32 dt_firstfile)();	/* directory serach routine */
	int (*G32 dt_nextfile)();	/* directory serach routine */
	int (*G32 dt_format)();
	int (*G32 dt_cd)();
	int (*G32 dt_rename)();
	int (*G32 dt_remove)();
	int (*G32 dt_else)();
};
#endif /* LANGUAGE_C */

/* device types */
#define	DTTYPE_CHAR	0x1	/* character device */
#define	DTTYPE_CONS	0x2	/* can be console */
#define	DTTYPE_BLOCK	0x4	/* block device */
#define DTTYPE_RAW	0x8	/* raw device that uses fs switch */
#define DTTYPE_FS	0x10


/* character device flags */
#define	DB_RAW		0x1	/* don't interpret special chars */
#define	DB_STOPPED	0x2	/* stop output */
#define	DB_BREAK	0x4	/* cntl-c raise console interrpt */

/* character device buffer */
#define	CBUFSIZE	256

#if defined(_LANGUAGE_C)||defined(LANGUAGE_C)||defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
struct device_buf {
	int db_flags;		/* character device flags */
	char *G32 db_in;		/* pts at next free char */
	char *G32 db_out;		/* pts at next filled char */
	char db_buf[CBUFSIZE];	/* circular buffer for input */
};
#endif /* LANGUAGE_C */

/* circular buffer functions */
#define	CIRC_EMPTY(x)	((x)->db_in == (x)->db_out)
#define	CIRC_FLUSH(x)	((x)->db_in = (x)->db_out = (x)->db_buf)
#define	CIRC_STOPPED(x)	((x)->db_flags & DB_STOPPED)


/* io block */
#if defined(_LANGUAGE_C)||defined(LANGUAGE_C)||defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
struct	iob {
	int	i_flgs;
	int	i_unit;		/* pseudo device unit */
	char	*G32 i_ma;		/* memory address of i/o buffer */
	unsigned int	i_cc;		/* character count of transfer */
	unsigned PSXLONG	i_offset;	/* seek offset in file */
	int	i_fstype;	/* file system type */
	int	i_errno;	/* error # return */
	struct device_table *G32 i_dp;	/* pointer into device_table */
        unsigned PSXLONG    i_size;
        PSXLONG    i_head;
        PSXLONG    i_fd;		/* file descriptor */
};
#endif /* LANGUAGE_C */

#ifndef NULL
#define NULL 0
#endif

/* Request codes */
#define	READ	1
#define	WRITE	2

#define NIOB	16	/* max number of open files */

/*
extern int _nulldev();
extern int _nodev();
*/

#endif /* _FS_H */


