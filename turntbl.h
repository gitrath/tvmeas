#ifndef TURNTBL_H
#define TURNTBL_H

#include <time.h>
#include "pc23.h"

#define MAXACCELERATION     20    // (deg/s^2) max turntable rotational acceleration
#define MAXVELOCITY         30    // (deg/s) max turntable rotational velocity
#define OFFSETFROMHOMEPOS   0     // default deg offset from home pos to line up pedastal with chamber


class Turntable
{
public:
           Turntable   (                                     );

  void     goHome        ();
  void     goHomeIfNeeded();
  void     setHomeOffset (double homeOffset_deg = OFFSETFROMHOMEPOS );

  void     setVel        (double vel_deg_per_sec = MAXVELOCITY );
  double   getVel        (void                                 );
  void     setAcc        (double acc = MAXACCELERATION         );
  double   getAcc        (void                                 );
  void     moveTo        (double phi_deg = 0                   );
  double   getPos        (void                                 );
  void     move          (double phi_deg                       );
  void     fastMoveTo    (double phi_deg = 0                   );

  void     timedMove          (double start_deg, double dist_deg, double time_sec);
  double   getFastestTimedMove( double dist_deg );

protected:
  static long int absPos;
  static float    acceleration;
  static float    velocity;
  static float    homeOffset;
  static PC23     pc23;
  static int      homedAlready;  // boolean: 1=yes, 0=no
  static clock_t  theTime;

  void     defineHome( double relMovePhi );

  double   moveTime         ( double phiToTravel        );
  void     sendCmd          ( char * cmdStr             );
  void     sendCmd          ( char * cmdStr, double x   );
  void     sendCmd          ( char * cmdStr, long int x );
  int      isMoving         ( void                      );
  void     waitUntilStopped ( double timeOut            );
  void     wait             ( double waitTime           );
  void     resetTime        ( );
  double   time             ( );

  void     verifyPC23CardInstalled();

};


#endif