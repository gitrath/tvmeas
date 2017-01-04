#include <assert.h>
#include <fstream.h>
#include <string.h>
#include "hpgl.h"

#define MAX_PLOTTER_X   15200
#define MAX_PLOTTER_Y   10000

#define PLOTTER_WIDTH   9.78125  // inches (measured with a ruler)
#define PLOTTER_HEIGHT  7.03125  // inches (measured with a ruler)

#define LETTER_WIDTH    182      // width of a charater in plotter units;
#define LETTER_HEIGHT   182      // height of a character in plotter units;



HPGL::HPGL( const char *fileName ) : os( fileName, ios::out )
{
  assert( os.good() );

  os << char(27) << ".Y" << endl;
  os << "IN;" << endl;
  os << "SC0," << MAX_PLOTTER_X << ",0," << MAX_PLOTTER_Y << ";" << endl;
  os << "SR0.798246,1.600000;" << endl;
  os << "PU;" << endl;

  assert( os.good() );


}



HPGL::~HPGL()
{
  setPenColor(NONE);
  os << char(27) << ".ZPG;" << endl;

  assert( os.good() );
};


double HPGL::getMaxX()
{
  return 9.75;  // tad bit under PLOTTER_WIDTH
}

double HPGL::getMaxY()
{
  return 7;     // tad bit under PLOTTER_HEIGHT
}


double HPGL::getLetterHeight()
{
  return double(LETTER_HEIGHT)/double(MAX_PLOTTER_Y)*double(PLOTTER_HEIGHT);
}


double HPGL::getLetterWidth()
{
  return double(LETTER_WIDTH)/double(MAX_PLOTTER_X)*double(PLOTTER_WIDTH);
}

int HPGL::mapX( double x )
{
  static const double scaleX = double(MAX_PLOTTER_X)/double(PLOTTER_WIDTH);

  return int(x*scaleX);
}

int HPGL::mapY( double y )
{
  static const double scaleY = double(MAX_PLOTTER_Y)/double(PLOTTER_HEIGHT);

  return int(y*scaleY);
}


void HPGL::penUp()
{
  if (curUpDown == UP) return;

  os << "PU;";  assert( os.good() );

  curUpDown = UP;
}


void HPGL::penDown()
{
  if ( penColor != curColor ) setPenColor( curColor );

  if (curUpDown == DOWN) return;

  assert( curColor != NONE );

  os << "PD;";  assert( os.good() );

  curUpDown = DOWN;
}


void HPGL::penMoveTo( int newX, int newY)
{
  if ( newX == curX && newY == curY ) return;

  assert( newX >= 0 && newX < MAX_PLOTTER_X );
  assert( newY >= 0 && newY < MAX_PLOTTER_Y );
  assert( curUpDown == UP || (curUpDown==DOWN && penColor==curColor));

  os << "PA" << newX << ',' << newY << ';' << endl;  // endl optional

  assert( os.good() );

  curX = newX; curY = newY;
}




void HPGL::penText ( int x, int y, const char *str, TextJustify just )
{
  int len = strlen( str );

  if ( just==RIGHTTOP || just==RIGHTMIDDLE || just==RIGHTBOTTOM )
    x -= int(len*LETTER_WIDTH);

  if ( just==CENTERTOP || just==CENTERMIDDLE || just == CENTERBOTTOM )
    x -= int( len/2.0*LETTER_WIDTH  );

  if ( just==LEFTTOP || just == CENTERTOP || just == RIGHTTOP )
    y -= int( LETTER_HEIGHT );

  if ( just==LEFTMIDDLE || just==CENTERMIDDLE || just == RIGHTMIDDLE )
    y -= int( LETTER_HEIGHT/ 2.0 );


  if ( penColor != curColor ) setPenColor( curColor );

  penUp();
  penMoveTo( x, y );
  os << "LB" << str << char(3) << endl;
  os << "PU;";  assert( os.good() );

  curX = -1; //something invalid
  curY = -1; //something invalid

  curUpDown = UP;

}



void HPGL::setPenColor( DrawColor color )
{
  if ( penColor == color ) return;

  assert( color >= 0 && color <= 6 );

  penUp();
  os << "SP" << int( color ) << ';' << endl;

  penColor = color;
}

