#include <dos.h>
#include <stdio.h>
#include <assert.h>
#include "pc23.h"


PC23::PC23( unsigned int addr )
{
  init( addr );
}

void PC23::init( unsigned int addr )
{
  address = addr;

  writeCB(STOP);                 // stop the w_dog timer

  resetTime();
  while( !(readSB() & FAIL ))     // wait for the fail bit
    {
     assert( time() < 5.0 );
    }

  writeCB( START  );             // restart the w_dog timer
  writeCB( CB     );             // clear the control byte

  resetTime();
  while( (readSB() & START_M) != RESTART )
    {
     assert( time() < 5.0 );     // wait for axis to stop and IDB ready
    }


  writeCB( INTCLR );             // reset interrupt output
  writeCB( CB     );             // clear the control byte

}



void PC23::writech  ( char alpha )
{
  resetTime();
  while ( !IDBReady() )  // wait for PC23 IDB to be ready
    {
     assert( time() < 5.0 );
    }

  writeIDB( alpha );             // write char to IDB
  writeCB ( CHAR_READY );        // tell PC23 that a char is ready in IDB

  resetTime();
  while ( IDBReady() )           // wait for PC23 IDB NOT to be ready
    {
     assert( time() < 5.0 );
    }

  writeCB ( CB );                // clear the control byte

  resetTime();
  while ( !IDBReady() )          // wait for PC23 IDB to be ready
    {
     assert( time() < 5.0 );
    }
}



char  PC23::readch   ( void )
{
  char alpha;

  assert( ODBReady() );          // ODB must contain a char already

  alpha = readODB();             // read byte from ODB

  writeCB(ACK);                  // tell PC23 byte was accepted

  resetTime();
  while ( ODBReady() )           // wait until PC23 make ODB NOT ready
    {
     assert ( time() < 5.0 );
    }

  writeCB( CB );                 // clear the control byte

  return alpha;
}


unsigned char PC23::IDBReady(void)
{
  return readSB() & IDB_M;
}

unsigned char PC23::ODBReady(void)
{
  return readSB() & ODB;
}

void PC23::writeIDB ( unsigned char byte )
{
  outportb( address, byte );
}

unsigned char PC23::readODB  ( void      )
{
  return inportb ( address );
}


void PC23::writeCB  ( unsigned char byte )
{
  outportb( address+1, byte );
}

unsigned char PC23::readSB   ( void      )
{
  return inportb ( address + 1 );
}



void   PC23::resetTime(void)
{
  theTime = clock();
}

double PC23::time(void)
{
  return (clock()-theTime)/double(CLK_TCK);
}


void  PC23::write     ( const char *command   )
{
  while (*command)
    {
     writech(*command++);
    }
}


char  *PC23::readLine ( int maxN )
{
  assert(maxN > 0);

  static char *buf = 0;
  static int  bufSize;

  if (buf == 0)
    {
     buf     = new char[maxN+1]; //Allocate memory for buffer on 1st call
     bufSize = maxN+1;
    }
  else if (maxN > bufSize)
    {
     delete buf;
     buf     = new char[maxN+1]; //Reallocate memory if needed
     bufSize = maxN+1;
    }

  assert(buf != 0);


  char *bufPtr = buf;
  for(int i=0; i<maxN; i++ )
    {
     resetTime();
     while ( !ODBReady() )
       {
        assert ( time() < 3.0 );  // allow PC23 time to have a char ready
       }

     if ( (*bufPtr++ = readch()) == '\r' ) break;

    }

  *(--bufPtr) = '\0';
  return buf;
}


long int PC23::readNum( const char *cmdStr )
{
  long int result;
  int    n;

  if (cmdStr != 0) write( cmdStr );

  n = sscanf(readLine(),"%ld",&result);
  assert(n == 1);

  return result;
}


int PC23::isMoving(void)
{
 if (readSB() & AXIS1STOPPED)
   {
    return 0;                  // axis 1 is NOT moving
   }
 return 1;                     // axis 1 is moving
}