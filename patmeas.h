#ifndef PATMEAS_H
#define PATMEAS_H

#include "source.h"
#include "receiver.h"
#include "turntbl.h"
#include "antpat.h"

class PatternMeasurer
{
public:
   PatternMeasurer();

   void takeMeasurement( Vector<double> &angleVec,
                         Vector<double> &amplVec );

   void takeMeasurement( AntennaPattern *pat );

   void setFreq         ( double start_freq_Hz = 12.45e9  );
   void setStartAngle   ( double start_angle_deg   = -180 );
   void setDegOfRotation( double rotation_dist_deg =  360 );

   void setHomeOffset   ( double home_offset_deg = 0 );

protected:
   HP8350BSource  source;
   SpecAnalyzer   specAnalyzer;
   Turntable      turntable;

   double freq;
   double startAngle;
   double degOfRotation;
   double homeOffset;

};



#endif
