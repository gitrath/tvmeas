#ifndef PC23_H
#define PC23_H

#include <time.h>


#define ACK            0xE0
#define ALDONE         0x07
#define BFULL          0x80    /* checks the buffer status - RESERVED */
#define CB             0x60    /* completes handshakes with status byte */
#define CB1            0x61
#define FAIL           0x20    /* board fail bit */
#define CHAR_READY     0x70    /* character ready bit */
#define AXIS1STOPPED   0x02    /* Axis 1 stopped bit */
#define DEFAULT_ADDR   0x300   /* default PC23 address is 768 decimal */

#define IDB1           0x71
#define IDB_M          0x10          /* input data buffer mask */
#define INTCLR         0x20          /* clear interrup bit */
#define ODB            0x08          /* output data buffer bit */
#define RESTART        0x17          /* byte to restart the pc23 */
#define CR             0x0D          /* crriage return */
#define START          0x40          /* byte to start the pc23 */
#define START_M        0x7F          /* start mask */
#define STOP           0x64          /* byte to halt the pc23's processor */


class PC23
{
public:
            PC23     ( unsigned int addr = DEFAULT_ADDR );
   void     init     ( unsigned int addr     );
   void     write    ( const char *command   );
   char    *readLine ( int maxN=80           );
   long int readNum  ( const char *command=0 );
   int      isMoving ( void                  );
   void     kill     ( void                  );
   void     stop     ( void                  );

protected:
   unsigned int address;
   void     writech  ( char alpha );
   char     readch   ( void       );

   inline void           writeIDB ( unsigned char byte );
   inline unsigned char  readODB  ( void               );
   inline void           writeCB  ( unsigned char byte );
   inline unsigned char  readSB   ( void               );

   inline unsigned char  IDBReady ( void      );
   inline unsigned char  ODBReady ( void      );

   clock_t  theTime;
   void     resetTime(void);
   double   time(void);



};

#endif

