#define  TC_FAR     far
#define  TC_CDECL   cdecl

/* Turbo C and Turbo C++ Declarations                      */
/*                                                         */
/*                                                         */

extern  int     ibsta;  /* status word                     */
extern  int     iberr;  /* GPIB error code                 */
extern  int     ibcnt;  /* number of bytes sent            */

/* Optionally include the following declarations in your   */
/* program.                                                */

/* GPIB Commands                                           */

#define UNL  0x3f  /* GPIB unlisten command                */
#define UNT  0x5f  /* GPIB untalk command                  */
#define GTL  0x01  /* GPIB go to local                     */
#define SDC  0x04  /* GPIB selected device clear           */
#define PPC  0x05  /* GPIB parallel poll configure         */
#define GET  0x08  /* GPIB group execute trigger           */
#define TCT  0x09  /* GPIB take control                    */
#define LLO  0x11  /* GPIB local lock out                  */
#define DCL  0x14  /* GPIB device clear                    */
#define PPU  0x15  /* GPIB parallel poll unconfigure       */
#define SPE  0x18  /* GPIB serial poll enable              */
#define SPD  0x19  /* GPIB serial poll disable             */
#define PPE  0x60  /* GPIB parallel poll enable            */
#define PPD  0x70  /* GPIB parallel poll disable           */

/* GPIB status bit vector :                                */
/*       global variable ibsta and wait mask               */

#define ERR     (1<<15) /* Error detected                  */
#define TIMO    (1<<14) /* Timeout                         */
#define END     (1<<13) /* EOI or EOS detected             */
#define SRQI    (1<<12) /* SRQ detected by CIC             */
#define RQS     (1<<11) /* Device needs service            */
#define CMPL    (1<<8)  /* I/O completed                   */
#define LOK     (1<<7)  /* Local lockout state             */
#define REM     (1<<6)  /* Remote state                    */
#define CIC     (1<<5)  /* Controller-in-Charge            */
#define ATN     (1<<4)  /* Attention asserted              */
#define TACS    (1<<3)  /* Talker active                   */
#define LACS    (1<<2)  /* Listener active                 */
#define DTAS    (1<<1)  /* Device trigger state            */
#define DCAS    (1<<0)  /* Device clear state              */

/* Error messages returned in global variable iberr        */

#define EDVR  0  /* DOS error                              */
#define ECIC  1  /* Function requires GPIB board to be CIC */
#define ENOL  2  /* Write function detected no Listeners   */
#define EADR  3  /* Interface board not addressed correctly*/
#define EARG  4  /* Invalid argument to function call      */
#define ESAC  5  /* Function requires GPIB board to be SAC */
#define EABO  6  /* I/O operation aborted                  */
#define ENEB  7  /* Non-existent interface board           */
#define EOIP 10  /* I/O operation started before previous  */
            	  /* operation completed                    */
#define ECAP 11  /* No capability for intended operation   */
#define EFSO 12  /* File system operation error            */
#define EBUS 14  /* Command error during device call       */
#define ESTB 15  /* Serial poll status byte lost           */
#define ESRQ 16  /* SRQ remains asserted                   */
#define ETAB 20  /* The return buffer is full              */

/* EOS mode bits                                           */

#define BIN  (1<<12) /* Eight bit compare                  */
#define XEOS (1<<11) /* Send EOI with EOS byte             */
#define REOS (1<<10) /* Terminate read on EOS              */

/* Timeout values and meanings                             */

#define TNONE    0   /* Infinite timeout (disabled)        */
#define T10us    1   /* Timeout of 10 us (ideal)           */
#define T30us    2   /* Timeout of 30 us (ideal)           */
#define T100us   3   /* Timeout of 100 us (ideal)          */
#define T300us   4   /* Timeout of 300 us (ideal)          */
#define T1ms     5   /* Timeout of 1 ms (ideal)            */
#define T3ms     6   /* Timeout of 3 ms (ideal)            */
#define T10ms    7   /* Timeout of 10 ms (ideal)           */
#define T30ms    8   /* Timeout of 30 ms (ideal)           */
#define T100ms   9   /* Timeout of 100 ms (ideal)          */
#define T300ms  10   /* Timeout of 300 ms (ideal)          */
#define T1s     11   /* Timeout of 1 s (ideal)             */
#define T3s     12   /* Timeout of 3 s (ideal)             */
#define T10s    13   /* Timeout of 10 s (ideal)            */
#define T30s    14   /* Timeout of 30 s (ideal)            */
#define T100s   15   /* Timeout of 100 s (ideal)           */
#define T300s   16   /* Timeout of 300 s (ideal)           */
#define T1000s  17   /* Timeout of 1000 s (maximum)        */

/*  IBLN Constants                                         */

#define NO_SAD   0
#define ALL_SAD -1

/* Miscellaneous                                           */

#define S    0x08    /* parallel poll sense bit            */
#define LF   0x0a    /* ASCII linefeed character           */

#ifdef __cplusplus
extern "C" {
#endif

extern int TC_FAR TC_CDECL ibbna(int handle, char TC_FAR *bdname);
extern int TC_FAR TC_CDECL ibcac(int handle, int v);
extern int TC_FAR TC_CDECL ibclr(int handle);
extern int TC_FAR TC_CDECL ibcmd(int handle, char TC_FAR *buffer, unsigned int cnt);
extern int TC_FAR TC_CDECL ibcmda(int handle, char TC_FAR *buffer, unsigned int cnt);
extern int TC_FAR TC_CDECL ibdev(int boardID, int padval, int sadval, int tmoval, int eotval, int eosval);
extern int TC_FAR TC_CDECL ibdma(int handle, int v);
extern int TC_FAR TC_CDECL ibeos(int handle, int v);
extern int TC_FAR TC_CDECL ibeot(int handle, int v);
extern int TC_FAR TC_CDECL ibfind(char TC_FAR *bdname);
extern int TC_FAR TC_CDECL ibgts(int handle, int v);
extern int TC_FAR TC_CDECL ibist(int handle, int v);
extern int TC_FAR TC_CDECL ibln(int handle, int padval, int sadval, int TC_FAR *listenflag);
extern int TC_FAR TC_CDECL ibloc(int handle);
extern int TC_FAR TC_CDECL ibonl(int handle, int v);
extern int TC_FAR TC_CDECL ibpad(int handle, int v);
extern int TC_FAR TC_CDECL ibpct(int handle);
extern int TC_FAR TC_CDECL ibppc(int handle, int v);
extern int TC_FAR TC_CDECL ibrd(int handle, char TC_FAR *buffer, unsigned int cnt);
extern int TC_FAR TC_CDECL ibrda(int handle, char TC_FAR *buffer, unsigned int cnt);
extern int TC_FAR TC_CDECL ibrdf(int handle, char TC_FAR *flname);
extern int TC_FAR TC_CDECL ibrpp(int handle, char TC_FAR *ppr);
extern int TC_FAR TC_CDECL ibrsc(int handle, int v);
extern int TC_FAR TC_CDECL ibrsp(int handle, char TC_FAR *spr);
extern int TC_FAR TC_CDECL ibrsv(int handle, int v);
extern int TC_FAR TC_CDECL ibsad(int handle, int v);
extern int TC_FAR TC_CDECL ibsic(int handle);
extern int TC_FAR TC_CDECL ibsre(int handle, int v);
extern int TC_FAR TC_CDECL ibstop(int handle);
extern int TC_FAR TC_CDECL ibtmo(int handle, int v);
extern int TC_FAR TC_CDECL ibtrap(int mask, int mode);
extern int TC_FAR TC_CDECL ibtrg(int handle);
extern int TC_FAR TC_CDECL ibwait(int handle, unsigned int mask);
extern int TC_FAR TC_CDECL ibwrt(int handle, char TC_FAR *buffer, unsigned int cnt);
extern int TC_FAR TC_CDECL ibwrta(int handle, char TC_FAR *buffer, unsigned int cnt);
extern int TC_FAR TC_CDECL ibwrtf(int handle, char TC_FAR *flname);

extern int TC_FAR TC_CDECL ibpoke(int handle, unsigned int option, unsigned int value);
extern int TC_FAR TC_CDECL ibdiag(int handle, char TC_FAR *buffer, unsigned int cnt);
extern int TC_FAR TC_CDECL ibxtrc(int handle, char TC_FAR *buffer, unsigned int cnt);

#ifdef __cplusplus
}
#endif
