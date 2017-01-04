#include <stdlib.h>
#include <iostream.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

extern "C" {
#include "decl.h"
}

#include "gpib.h"


GPIB::GPIB(const char *udname )
{

 assert( udname );

 assert(strlen(udname) <= 7);
 assert(strlen(udname) > 0);

 ud = ibfind((char *)udname);
 trapError("ibfind() failed during initialization");
 assert (ud >= 0);

 clear();

}


void GPIB::write(const char *command )
{

  int length=strlen(command);
  assert (length > 0);

  ibwrt(ud,(char *)command,length);
  trapError("ibwrt() failed");
  assert (ibcnt == length);
}


char *GPIB::read(int maxN)
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

  ibrd( ud, buf, maxN );
  trapError("ibrd() failed");

  assert(ibcnt <= maxN);
  buf[ibcnt] = '\0';

  return buf;
}


char *GPIB::slowRead(int manN)
{
  ibtmo (ud, T10s);
  trapError("ibtmo() failed to change time out time to 10 sec");

  int prevTimeOut = iberr;
  assert( prevTimeOut>=0 && prevTimeOut <=17 );

  char *returnStr = read( manN );

  ibtmo (ud, prevTimeOut);
  trapError("ibtmo() failed to return timeout time to original value");

  return returnStr;
}


double GPIB::readNum(const char * cmdStr)
{
  double result;
  int    n;

  if (cmdStr != 0) write( cmdStr );

  n = sscanf(read(),"%lg",&result);
  assert(n == 1);

  return result;
}


void GPIB::clear(void)
{
  ibclr (ud);
  trapError("ibclr() failed when trying to reset GPIB device");
}


void GPIB::local(void)
{
  ibloc (ud);
  trapError("ibloc() failed when trying to put GPIB device in local mode");
}


void GPIB::trapError(const char *errorMsg)
{
  if (ibsta & ERR)
    {
     cerr << "GPIB error: " << errorMsg << '\n';
     char *errStr;
     switch (iberr)
       {
        case 0:
          errStr = "DOS error";
          break;
        case 1:
          errStr = "Function requires GPIB board to be Controller-In-Charge";
          break;
        case 2:
          errStr = "Write function detected no listeners";
          break;
        case 3:
          errStr = "GPIB board is not addressed correctly";
          break;
        case 4:
          errStr = "Invalid argument to function call";
          break;
        case 5:
          errStr = "GPIB board not System Controller as required";
          break;
        case 6:
          errStr = "I/O operation aborted";
          break;
        case 7:
          errStr = "Non-existent GPIB board";
          break;
        case 10:
          errStr = "I/O started before previous operation completed";
          break;
        case 11:
          errStr = "No capability for operation";
          break;
        case 12:
          errStr = "File system error";
          break;
        case 14:
          errStr = "Command error during device call";
          break;
        case 15:
          errStr = "Serial poll status byte(s) lost";
          break;
        case 16:
          errStr = "SRQ stuck in the ON position";
          break;
        default:
          errStr = "unknown error specified by iberr";
       }
     cerr << "   " << errStr << '\n';
     cerr << "   " << "iberr=" << iberr << "  ibcnt=" << ibcnt <<
                      "  ibsta=" << ibsta << '\n';
     abort();
    }

}