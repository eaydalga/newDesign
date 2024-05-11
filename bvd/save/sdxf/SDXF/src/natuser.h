/* doc----------------------------------------------------------------
:Project.   Structured Data eXchange Format (SDXF)
:Program.   natuser.h
:Title.     NATURAL interface to user supplied C functions
:PrType.    Header File
:Copyright. 1991-95 by Software AG
:Language.  C (ANSI)
----------------------------------------------------------------------*/
/*****************************************************************************
**
** file    : natuser.h
** purpose : NATURAL interface to user supplied C functions
**
** (c) copyright 1991-95 by Software AG
**
*****************************************************************************/

#ifndef DEF_NATUSER
#define DEF_NATUSER	1

#ifndef WORD
#define WORD  unsigned short 
#endif

#ifndef BYTE
#define BYTE  unsigned char
#endif

/*----------------------------------------------------------------------------
** NATURAL parameter field information
**--------------------------------------------------------------------------*/
typedef struct {
  char typevar;                      /* type of variable           */
  BYTE pb2;                          /* if type == (N or P) ==>    */
                                     /*    total num of digits     */
                                     /* else                       */
  union {                            /*    unused                  */
    BYTE pb[2];                      /* if type == (N or P) ==>    */
    WORD lfield;                     /*  pb[0] = #dig bef.dec.point*/
  } flen;                            /*  pb[1] = #dig aft.dec.point*/
                                     /* else                       */
                                     /*    lfield = length of      */
                                     /*             field          */
} FINFO;


/*----------------------------------------------------------------------------
** defines for platform independent user exits
**--------------------------------------------------------------------------*/
#if OS_WIN
/* WINDOWS 16-bit compiler ---------------------------------------*/
# define NATTYP_I1      char			/* 1 byte integer */
# define NATTYP_I2      int			/* 2 byte integer */
# define NATTYP_I4      long			/* 4 byte integer */
# define NATTYP_FCT     long			/* return type    */
# define NATFCT	        long             	/* return type    */
# define NATARGDEF(count,pdat,pinf) (WORD count, BYTE  ** pdat, FINFO  * pinf)
# define NATARGDCL(count,pdat,pinf) NATARGDEF(count,pdat,pinf)

extern int ncxr_callnat(char  *, WORD, BYTE  **, FINFO  *);

#else
#if WIN32
/* WINDOWS 32-bit compiler ---------------------------------------*/
# define NATTYP_I1	char			/* 1 byte integer */
# define NATTYP_I2	short			/* 2 byte integer */
# define NATTYP_I4	int			/* 4 byte integer */
# define NATTYP_FCT	long			/* return type    */
# define NATFCT		long  		        /* return type    */
# define NATARGDEF(count,pdat,pinf) (WORD count, BYTE ** pdat, FINFO * pinf)
# define NATARGDCL(count,pdat,pinf) NATARGDEF(count,pdat,pinf)

extern int ncxr_callnat(char *, WORD, BYTE **, FINFO *);

#else
#if OS_OS2
/* OS/2 32-bit compiler ------------------------------------------*/
# define NATTYP_I1	char			/* 1 byte integer */
# define NATTYP_I2	short 			/* 2 byte integer */
# define NATTYP_I4	int			/* 4 byte integer */
# define NATTYP_FCT	long			/* return type    */
# define NATFCT		long 			/* return type    */
# define NATARGDEF(count,pdat,pinf) (WORD count, BYTE ** pdat, FINFO * pinf)
# define NATARGDCL(count,pdat,pinf) NATARGDEF(count,pdat,pinf)

extern int ncxr_callnat(char *, WORD, BYTE **, FINFO *);

#else
/* UNIX ----------------------------------------------------------*/
# define NATTYP_I1	char			/* 1 byte integer */
# define NATTYP_I2	short			/* 2 byte integer */
# define NATTYP_I4	int			/* 4 byte integer */
# define NATTYP_FCT	long			/* return type    */
# define NATFCT		long			/* return type    */
# define NATARGDEF(count,pdat,pinf) (count,pdat,pinf) WORD count; BYTE ** pdat; FINFO * pinf;
# define NATARGDCL(count,pdat,pinf) ()

extern int ncxr_callnat();

#endif
#endif
#endif

#endif /* DEF_NATUSER */

