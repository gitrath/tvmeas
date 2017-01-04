#ifndef scrnplot_H
#define scrnplot_H

#include <fstream.h>
#include "plot.h"

class ScreenPlot : public Plot
{
public:

  ScreenPlot();
  ~ScreenPlot();

  virtual double getMaxX();
  virtual double getMaxY();

  virtual double getLetterHeight();
  virtual double getLetterWidth();

protected:

  virtual int mapX( double x_inch );
  virtual int mapY( double y_inch );

  virtual void penUp();
  virtual void penDown();

  virtual void penMoveTo( int x, int y );

  virtual void penText  ( int x, int y, const char *str, TextJustify justify = LEFTBOTTOM );

  virtual void setPenColor( DrawColor color );

};


#endif
