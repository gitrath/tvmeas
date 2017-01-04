#ifndef RECEIVER_H
#define RECEIVER_H

#include <time.h>
#include "gpib.h"
#include "vector.h"


#define SPECTRUM_ANALYZER_MODEL_HP8562A
/*#define SPECTRUM_ANALYZER_MODEL_HP8593A*/

#ifdef SPECTRUM_ANALYZER_MODEL_HP8562A
  #define TRACEPOINTS 601
#else  /* assume HP8593A */
  #define TRACEPOINTS 401
#endif


class Receiver
{
public:
  virtual double   setFreq(double f_hz);
  virtual double   getAmpl(void);

protected:
  static double freq;

};



class SpecAnalyzer : public Receiver
{

public:
  enum DetectionMode { SAMPLE, POSITIVE, NEGATIVE };
  enum TriggerMode   { EXTERNAL, FREE };

  SpecAnalyzer();


  virtual double setFreq         (double f_Hz);
  double         setSpan         (double span_Hz);
  double         setRBW          (double RBW_Hz);
  double         setVBW          (double VBW_Hz);
  double         setSweepTime    (double t_sec);

  double setRefLevel     (double ref_level_dBm);

  void   setTriggerMode  (TriggerMode mode);
  void   setDetectionMode(DetectionMode mode);

  virtual double getAmpl();
  double         getFreq();
  double         getSpan();
  double         getRBW();
  double         getVBW();
  double         getSweepTime();
  DetectionMode  getDetectionMode();

  void   readTrace( Vector<double> &amplVec );

  double searchFreq       (double initFreq_Hz, double initSpan_Hz = 10e6);
  double getLastSearchFreq();

  void   turnDisplayOff ();
  void   turnDisplayOn  ();
  void   local          ();   // put Spectrum Analyzer in local mode (rather that remote mode)

protected:
  static int     hasBeenInitialized; // yes=1, no=0

  static double  resolutionBW;
  static double  videoBW;
  static double  span;
  static double  sweepTime;
  static double  attenuation;

  static double  lastSearchFreq;

  static TriggerMode   triggerMode;
  static DetectionMode detectionMode;

  static clock_t theTime;

  static GPIB    gpib;

  void    waitTilDone ();
  void    wait        (double t );
  void    resetTime   ();
  double  time        ();
  void    sendCmd     (const char *cmd, double x, const char *units);
  void    sendCmd     (const char *cmd );
  double  readNum     (const char *cmd );
  void    setSingleSweep    ();
  void    setAutoSweep      ();
  double  setAttenuation    (double atten_dB);
  void    checkError        ();

  TriggerMode getTriggerMode();

};

typedef SpecAnalyzer HP8562AReceiver;

#endif