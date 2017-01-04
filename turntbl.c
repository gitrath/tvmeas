#include <iostream.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "turntbl.h"


#define GEARRATIO           90    // stepper motor to turntable gear ratio
#define STEPSPERREVOLUTION  20000 // # microsteps in 1 revolution of motor
#define GOHOMETIMEOUT       45    // max allowed sec for turntable to go home
#define EXTRAROTATIONTIME   .5    // extra time in sec to rotate in timedMove()

#define VELOCITYFACTOR      GEARRATIO/360.*STEPSPERREVOLUTION/25000.
#define ACCELERATIONFACTOR  GEARRATIO/360.*STEPSPERREVOLUTION/25000.
#define DISTANCEFACTOR      GEARRATIO/360.*STEPSPERREVOLUTION



long int Turntable::absPos;
float    Turntable::acceleration;
float    Turntable::velocity;
float    Turntable::homeOffset=OFFSETFROMHOMEPOS;
PC23     Turntable::pc23;
int      Turntable::homedAlready=0;  // boolean: 1=yes, 0=no
clock_t  Turntable::theTime;


Turntable::Turntable (void)
{
  verifyPC23CardInstalled();
  setVel();
  setAcc();
  sendCmd("mpa");              // set position absolute mode
  //setHomeOffset();
  //defineHome(0);
}

int Turntable::isMoving (void)
{
  if ( pc23.isMoving() ) return 1;
  return 0;
}


double Turntable::getPos (void)
{
  goHomeIfNeeded();

  assert( absPos == pc23.readNum("pr ") );

  double p = fmod( double(absPos)/double(DISTANCEFACTOR), 360 );  // units deg
  if ( p < 0 ) p += 360;

  assert( p >= 0 && p < 360 );

  return p;
}


double Turntable::getVel (void)
{
  return velocity;                              // units deg/s
}

double Turntable::getAcc (void)
{
  return acceleration;
}                                               // units deg/s/s

void Turntable::goHome ()
{

  sendCmd("ga",MAXACCELERATION*ACCELERATIONFACTOR); //go home acceleration
  sendCmd("gh",MAXVELOCITY*VELOCITYFACTOR        ); //go home velocity

  waitUntilStopped(GOHOMETIMEOUT);  // time to allow turntable to go home

  homedAlready = 1; // 1=yes

  defineHome( homeOffset );

}

void Turntable::goHomeIfNeeded()
{
  if (!homedAlready) goHome();
}

void Turntable::setHomeOffset( double homeOffset_deg )
{
  if (homedAlready)
    {
     if ( fabs( homeOffset_deg - homeOffset ) > .001 )
       {
        fastMoveTo( -homeOffset );
        defineHome( homeOffset_deg );
        //homedAlready = 0; // False
       }
    }

  homeOffset = homeOffset_deg;
}

void Turntable::defineHome( double relMovePhi )
{
  sendCmd("pz");                 // set zero position
  absPos = 0;

  if (relMovePhi != 0)
    {
     fastMoveTo( relMovePhi );

     sendCmd("pz");              // set zero position
     absPos = 0;
    }
}

void Turntable::setVel ( double vel )   // units deg/s
{
  assert( vel <= MAXVELOCITY );
  assert( vel > 0 );

  sendCmd("v",vel*VELOCITYFACTOR);      // set velocity
  velocity = vel;
}

void Turntable::setAcc (double acc )    // units deg/s/s
{
  assert( acc <= MAXACCELERATION );
  assert( acc > 0 );

  sendCmd("a",acc*ACCELERATIONFACTOR);  // set acceleration
  acceleration = acc;
}


void Turntable::moveTo (double phi)     // units deg
{
  goHomeIfNeeded();

  double newPos = fmod( phi, 360 );
  if ( phi < 0 ) phi += 360;
  assert( phi >= 0 && phi < 360 );

  double curPos = getPos();

  double deltaPos = newPos - curPos;

  if (deltaPos > 180)   //determines which rotation direction is shortest
    {
     deltaPos -= 360;
    }
  else if (deltaPos < -180)
    {
     deltaPos += 360;
    }

  if (deltaPos > 180)   //determines which rotation direction is shortest
    {
     deltaPos -= 360;
    }
  else if (deltaPos < -180)
    {
     deltaPos += 360;
    }

  assert( deltaPos >= -180 );
  assert( deltaPos <= 180  );
  double x = fabs( deltaPos );

  if (x > 0)
    {
     absPos += deltaPos*DISTANCEFACTOR;  assert( absPos <= 99999999L && absPos >= -99999999L);
     sendCmd("d",absPos);
     sendCmd("g");

//     wait(.5);                      // allow turntable time to start

     double moveTime;               // in seconds
     const double v = getVel();
     const double a = getAcc();

     if (v*v > x*a)
      {
       moveTime = 2*sqrt(x/a);   // triangular velocity
       }
     else
       {
        moveTime = v/a + x/v;     // trapezoidal velocity
       }

     waitUntilStopped(moveTime+.5);
    }
}

void Turntable::move (double phi)       // units deg
{
  moveTo( getPos()+phi );
}


void Turntable::fastMoveTo(double phi)  // units deg
{
  double vel,acc;

  vel = getVel();
  acc = getAcc();

  setVel(MAXVELOCITY);
  setAcc(MAXACCELERATION);

  moveTo( phi );

  setVel(vel);
  setAcc(acc);
}



void Turntable::sendCmd( char *cmdStr )
{
  pc23.write( cmdStr );
  pc23.write( " "    );
}

void Turntable::sendCmd( char *cmdStr, double x)
{
  char numStr[30];
  int  n;

  n = sprintf(numStr,"%lg",x);
  assert(n != EOF );

  pc23.write( cmdStr );
  pc23.write( numStr );
  pc23.write( " "    );

}


void Turntable::sendCmd( char *cmdStr, long int x)
{
  char numStr[30];
  int  n;

  n = sprintf(numStr,"%ld",x);
  assert(n != EOF);

  pc23.write( cmdStr );
  pc23.write( numStr );
  pc23.write( " "    );

}




void Turntable::waitUntilStopped(double timeOut)
{
  resetTime();
  while( !isMoving()  && time() < .5 );  // give turntable a chance to start moving in case it already hasn't

  resetTime();
  while( isMoving() )
    {
     assert( time() < timeOut );
    }
}

void Turntable::wait(double t)
{
  resetTime();
  while( time() < t );
}


void Turntable::resetTime(void)
{
  theTime = clock();
}

double Turntable::time(void)     // seconds since last resetTime() call
{
  return (clock()-theTime)/double(CLK_TCK);
}


void Turntable::verifyPC23CardInstalled()
{
  sendCmd("rv");

  char *revisionStr = pc23.readLine();

  if (revisionStr[0] == '9' &&
      revisionStr[1] == '2' &&
      revisionStr[2] == '-') return;

  cerr << "**** Error: PC23 card not responding properly ****" << endl;
  abort();
}


void Turntable::timedMove(double start_deg, double dist_deg, double time_sec)
{
  assert( time_sec > 0);

  double v = fabs(dist_deg/time_sec);  assert( v <= MAXVELOCITY );

  assert (getAcc() > 0);

  double accelerationDistance = ( dist_deg>0 ? -1 : 1 );
  accelerationDistance *= .5*v*v/getAcc();

  fastMoveTo( start_deg + accelerationDistance );

  if ( dist_deg > 0 )
    {
     sendCmd("h+");  // set direction of rotation
    }
  else
    {
     sendCmd("h-");
    }

  setVel( v );

  char str[200];

  sprintf(str,"oxx0 mc g t0 oxx1 t%lg oxx0 v0 g mn\n",
               time_sec+EXTRAROTATIONTIME);

  sendCmd( str );


  double moveTime = 2*v/getAcc() + time_sec + EXTRAROTATIONTIME;
  waitUntilStopped(moveTime + .5);

  setVel( v );


  absPos = pc23.readNum("pr ");

}

double Turntable::getFastestTimedMove( double dist_deg )
{

  double maxVel = sqrt( getAcc() * dist_deg / 2 );

  if (maxVel > MAXVELOCITY) maxVel = MAXVELOCITY;

  return dist_deg/maxVel;
}

