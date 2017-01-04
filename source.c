#include <stdio.h>
#include <assert.h>
#include "source.h"

#define HP8350BMINPOWER -5
#define HP8350BMAXPOWER 20
#define HP8350BMINFREQ  .01e9
#define HP8350BMAXFREQ  20.4e9


double Source::freq;
int    HP8350BSource::hasBeenInitialized = 0; // 1=yes, 0=no
GPIB   HP8350BSource::gpib("HP8350B");
double HP8350BSource::power;


HP8350BSource::HP8350BSource( double f, double pow )
{
  if (!hasBeenInitialized)
    {
     hasBeenInitialized = 1;
     gpib.write("ip;");

     freq  =  0;
     power = -500;

     setFreq(f);
     setPower(pow);
    }
}



void HP8350BSource::setFreq( double f)
{
  char cmdStr[100];

  assert(f>0);
  assert(f<=HP8350BMAXFREQ);
  assert(f>=HP8350BMINFREQ);

  if (freq != f)
    {
     int n = sprintf(cmdStr,"cw %lg hz;",f);
     assert (n != EOF);

     gpib.write(cmdStr);

     freq = f;
    }
}


void HP8350BSource::setPower( double pow )
{
  char cmdStr[100];

  assert(pow >= HP8350BMINPOWER);
  assert(pow <= HP8350BMAXPOWER);

  if (power != pow)
    {
     int n = sprintf(cmdStr,"pl %lg dm;",pow);
     assert (n != EOF);
     gpib.write(cmdStr);

     power = pow;
    }
}


void HP8350BSource::local( void )
{
  gpib.local();
}


Source::Source(double f)
{
  setFreq         (f);
}


void Source::setFreq(double f)
{
  assert(f>=0);
  freq = f;
}







