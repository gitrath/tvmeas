#ifndef SOURCE_H
#define SOURCE_H

#include "gpib.h"

class Source
{
public:
  Source(double f_Hz=15e9);

  virtual void setFreq          (double f_Hz     );

protected:
  static double freq;
};


class HP8350BSource : public Source
{
public:
  HP8350BSource  (double f_Hz=12.5e9, double power_dBm=15 );

  virtual void setFreq   (double f_Hz  );
  void         setPower  (double p_dBm );
  void         local     ();

protected:
  static int    hasBeenInitialized;
  static GPIB   gpib;
  static double power;

};


#endif