#include <iostream.h>
#include <stdlib.h>
#include <assert.h>
#include <strstream.h>
#include <iomanip.h>
#include "vector.h"
#include "patmeas.h"


PatternMeasurer::PatternMeasurer()
{
  setFreq         ();
  setStartAngle   ();
  setDegOfRotation();
  setHomeOffset   ();
}


void PatternMeasurer::takeMeasurement( Vector<double> &angleVec,
                                       Vector<double> &amplVec )
{
  source.setFreq( freq );

  turntable.setHomeOffset( homeOffset );

  if ( freq != specAnalyzer.getLastSearchFreq() )
    {
     turntable.fastMoveTo(0);
     specAnalyzer.searchFreq( freq );
    }

  double bestTime = turntable.getFastestTimedMove(degOfRotation);

  specAnalyzer.setSweepTime( bestTime );
  specAnalyzer.setTriggerMode( SpecAnalyzer::EXTERNAL );
  specAnalyzer.setVBW( 100 );

  turntable.timedMove( startAngle, degOfRotation, bestTime );

  specAnalyzer.readTrace( amplVec );

  assert( amplVec.size() > 1 );
  angleVec.setSize( amplVec.size() );

  assert( angleVec.size() > 1);
  for(int i=0; i< angleVec.size(); i++)
    {
      angleVec[i] = startAngle + i*degOfRotation/(angleVec.size()-1);
    }

}


void PatternMeasurer::takeMeasurement( AntennaPattern *pat )
{

  takeMeasurement( pat->angleVec, pat->amplVec );

  pat->clearLabels();  // caution: filename label NOT cleared

  // make frequency label
  char freq_str[80]; ostrstream  freq_s( freq_str, sizeof(freq_str) );

  freq_s << freq/1e9 << " GHz" << '\0';
  pat->setLabel( AntennaPattern::FREQUENCY, freq_s.str() );


  // make date label
  struct tm *time_now;
  time_t secs_now;
  char date_str[80];

  tzset();
  time(&secs_now);

  time_now = localtime(&secs_now);
  strftime(date_str, sizeof(date_str), "%b %d %Y", time_now );

  pat->setLabel( AntennaPattern::DATE, date_str);


  // make beamwidth label
  char bw_str[80]; ostrstream  bw_s( bw_str, sizeof(bw_str) );

  double beamwidth = pat->beamwidth();

  beamwidth =  double(int(beamwidth*10))/10.0;

  if ( beamwidth < 360.0 ) // 360 deg represents a bad beamwidth
    {
     bw_s << beamwidth << " DEG BW" << '\0' ;

     pat->setLabel( AntennaPattern::BEAMWIDTH, bw_s.str());

    }


  // make detect mode label
  if ( specAnalyzer.getDetectionMode() == SpecAnalyzer::POSITIVE )
    {
     pat->setLabel( AntennaPattern::DETECT_MODE, "Pos Detect" );
    }
  else if ( specAnalyzer.getDetectionMode() == SpecAnalyzer::NEGATIVE )
    {
     pat->setLabel( AntennaPattern::DETECT_MODE, "Neg Detect" );
    }

}


void PatternMeasurer::setFreq ( double start_freq_Hz     )
{
  assert( start_freq_Hz > 0);

  freq = start_freq_Hz;
}


void PatternMeasurer::setStartAngle( double start_angle_deg)
{
  startAngle = start_angle_deg;
}


void PatternMeasurer::setDegOfRotation( double rotation_dist_deg)
{
  degOfRotation = rotation_dist_deg;
}


void PatternMeasurer::setHomeOffset( double home_offset_deg )
{
  homeOffset = home_offset_deg;
}
