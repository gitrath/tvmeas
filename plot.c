#include <assert.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "plot.h"


Plot::Plot()
{
  curX = -1; // not really sure where plotter defaults to
  curY = -1; // not really sure where plotter defaults to

  penColor = NONE;
  curColor = BLACK;
  curUpDown = UP;

}


void Plot::penMove( int relX, int relY )
{
  if (relX == 0 && relY == 0 ) return;

  penMoveTo( curX + relX, curY + relY );
}



void Plot::move  ( double x_inch, double y_inch )
{
  int relX = mapX(x_inch);
  int relY = mapY(y_inch);

  if (relX == 0 && relY == 0) return;

  penUp();
  penMove( relX, relY );
}


void Plot::moveTo ( double x_inch, double y_inch )
{
  int newX = mapX(x_inch);
  int newY = mapY(y_inch);

  if ( newX == curX && newY == curY ) return;

  penUp();
  penMoveTo( newX, newY );
}


void Plot::line ( double x_inch, double y_inch )
{
  int relX = mapX(x_inch);
  int relY = mapY(y_inch);

  if ( relX == 0 && relY == 0 ) return;

  penDown();
  penMove( relX, relY );
}


void Plot::lineTo ( double x_inch, double y_inch)
{
  int newX = mapX(x_inch);
  int newY = mapY(y_inch);

  if ( newX == curX && newY == curY ) return;

  penDown();
  penMoveTo( newX, newY );
}


void Plot::line( double x1_inch, double y1_inch, double x2_inch, double y2_inch )
{
  int x1 = mapX( x1_inch );
  int y1 = mapY( y1_inch );
  int x2 = mapX( x2_inch );
  int y2 = mapY( y2_inch );

  if (x1==x2 && y1 == y2) return;

  long int d1 = ((long int)(x1 - curX))*((long int)(x1 - curX)) +
                ((long int)(y1 - curY))*((long int)(y1 - curY));

  long int d2 = ((long int)(x2 - curX))*((long int)(x2 - curX)) +
                ((long int)(y2 - curY))*((long int)(y2 - curY));


  if ( d1 <= d2 )
    {
     if ( d1 != 0 )
       {
        penUp();
        penMoveTo( x1, y1 );
       }
     penDown();
     penMoveTo( x2, y2 );
    }
  else
    {
     if ( d2 != 0 )
       {
        penUp();
        penMoveTo( x2, y2 );
       }
     penDown();
     penMoveTo( x1, y1 );
    }
}


void Plot::setColor( DrawColor color )
{
  curColor = color;
}



void Plot::nextColor()
{
  DrawColor color = curColor;

  assert( color >= 0 && color <= 6);
  color = DrawColor( color+1 );

  if ( color > 6 ) color = DrawColor(1);
  setColor( color );
}




void Plot::text (double x_inch, double y_inch, const char *str, TextJustify justify )
{
  penText( mapX(x_inch), mapY(y_inch), str, justify);
}


void Plot::rectangle( double x1, double y1, double x2, double y2)
{
  moveTo( x1, y1 );

  lineTo( x2, y1 );
  lineTo( x2, y2 );
  lineTo( x1, y2 );
  lineTo( x1, y1 );
}

void Plot::circle( double x, double y, double radius )
{
  assert( radius >= 0);   // allow for radius = 0

  const double max_error = .01; // inch ( max distance between an actual circle to chords making it up )

  errno = 0; // for error checking only

  int intervals;

  if( radius > max_error)
    {
      assert(  (radius-max_error)/radius >= -1 );
      assert(  (radius-max_error)/radius <=  1 );
      intervals = int(ceil( 2*M_PI/acos( (radius-max_error)/radius ) ) );
    }
  else
    {
      intervals = 8;
    }

  assert( !errno );
  if ( intervals < 8) intervals =8;

  moveTo( x+radius, y);

  for(int i=0; i<intervals; i++)
    {
     double angle = 2*M_PI/intervals*(i+1);

     lineTo( x+radius*cos(angle), y+radius*sin(angle) );
    }
}


double Plot::rayTextRadiusCalc( double r, double th, double vx, double vy )
{
  double b = cos(th)*vx+sin(th)*vy;

  return -b + sqrt( b*b - (vx*vx+vy*vy-r*r) );
}

double Plot::max( double a, double b)
{
  if (a > b) return a;
  return b;
}

void Plot::rayText ( double center_x, double center_y,
                     double radius, double ang, const char *str )
{
  int len = strlen( str );

  //double h2 = double(LETTER_HEIGHT)/double(MAX_PLOTTER_Y)*double(PLOTTER_HEIGHT)/2.0;
  //double w2 = len*double(LETTER_WIDTH)/double(MAX_PLOTTER_X)*double(PLOTTER_WIDTH)/2.0;

  double h2 = getLetterHeight()/2.0;
  double w2 = len*getLetterWidth()/2.0;

  radius += .05; // inches

  double a  = rayTextRadiusCalc( radius, ang, -w2, -h2 );

  a = max( a, rayTextRadiusCalc( radius, ang, -w2,  h2 ));

  a = max( a, rayTextRadiusCalc( radius, ang,  w2,  h2 ));

  a = max( a, rayTextRadiusCalc( radius, ang,  w2, -h2 ));

  assert( a > radius );

  double x = center_x + a*cos(ang);
  double y = center_y + a*sin(ang);

  //rectangle( x-w2, y-h2, x+w2, y+h2 );

  text( x, y, str, CENTERMIDDLE );
}

