#include <assert.h>
#include <string.h>
#include "scrnplot.h"
#include "graphapp.h"

#define Uses_TEvent
#define Uses_TEventQueue
#define Uses_TScreen
#define Uses_TSystemError
#include <tv.h>

#define SCREEN_WIDTH  9.75   //inches
#define SCREEN_HEIGHT 7      //inches



ScreenPlot::ScreenPlot()
{
//  extern MyApplication *TVMeas;
//  TVMeas->mySuspend();

  TSystemError::suspend();
  TEventQueue::suspend();
  TScreen::suspend();

  Boolean success = graphAppInit(DETECT, 0, "c:\\borlandc\\bgi", True);
  assert( success == True );

  success = graphicsStart();
  assert( success );

  //setfillstyle(SOLID_FILL, LIGHTGRAY);
  //::rectangle(0,0,getmaxx(),getmaxy());
  //setbkcolor( LIGHTGRAY );


}


ScreenPlot::~ScreenPlot()
{
  TEvent event;

  do
    {
     event.getKeyEvent();
    } while (event.what == evNothing);


  graphicsStop();

  graphAppDone();

  //extern MyApplication *TVMeas;
  //TVMeas->myResume();
  //TVMeas->showCursor();

  //resume();
  //TMouse::resume();

  TScreen::resume();
  TEventQueue::resume();
  TSystemError::resume();

};


double ScreenPlot::getMaxX()
{
  return 9.75;  // same as plotter for now
}

double ScreenPlot::getMaxY()
{
  return 7;     // same as plotter for now
}


double ScreenPlot::getLetterHeight()
{
  return textheight("0")/double(getmaxy())*double(SCREEN_HEIGHT);
}


double ScreenPlot::getLetterWidth()
{
  return textwidth("0")/double(getmaxx())*double(SCREEN_WIDTH);
}

int ScreenPlot::mapX( double x )
{
  static const double scaleX = double(getmaxx())/double(SCREEN_WIDTH);

  return int(x*scaleX);
}

int ScreenPlot::mapY( double y )
{
  static const double scaleY = double(getmaxy())/double(SCREEN_HEIGHT);

  return getmaxy()-1-int(y*scaleY);
}


void ScreenPlot::penUp()
{
  curUpDown = UP;
}


void ScreenPlot::penDown()
{
  if ( penColor != curColor ) setPenColor( curColor );

  if (curUpDown == DOWN) return;

  assert( curColor != NONE );

  curUpDown = DOWN;
}


void ScreenPlot::penMoveTo( int newX, int newY)
{
  if ( newX == curX && newY == curY ) return;

  assert( newX >= 0 && newX < getmaxx() );
  assert( newY >= 0 && newY < getmaxy() );
  assert( curUpDown == UP || (curUpDown==DOWN && penColor==curColor));

  if ( curUpDown == UP )
    {
     moveto( newX, newY ); assert( graphresult() == grOk );
    }
  else
    {
     lineto( newX, newY );
    }

  curX = newX; curY = newY; assert( graphresult() == grOk );
}




void ScreenPlot::penText ( int x, int y, const char *str, TextJustify just )
{
  //int len = strlen( str );

  if ( penColor != curColor ) setPenColor( curColor );

  penUp();
  penMoveTo( x, y );

  enum TextJustify { CENTERTOP, CENTERMIDDLE, CENTERBOTTOM, LEFTTOP, LEFTMIDDLE, LEFTBOTTOM, RIGHTTOP, RIGHTMIDDLE, RIGHTBOTTOM};

  int horiz_just = LEFT_TEXT;
  int vert_just = BOTTOM_TEXT;

  switch( just )
    {
     case LEFTTOP:
     case LEFTMIDDLE:
     case LEFTBOTTOM:
       horiz_just = LEFT_TEXT;
       break;
     case CENTERTOP:
     case CENTERMIDDLE:
     case CENTERBOTTOM:
       horiz_just = CENTER_TEXT;
       break;
     case RIGHTTOP:
     case RIGHTMIDDLE:
     case RIGHTBOTTOM:
       horiz_just = RIGHT_TEXT;
       break;
    }

  switch( just )
    {
     case LEFTTOP:
     case CENTERTOP:
     case RIGHTTOP:
       vert_just = TOP_TEXT;
       break;
     case LEFTMIDDLE:
     case CENTERMIDDLE:
     case RIGHTMIDDLE:
       vert_just = CENTER_TEXT;
       break;
     case LEFTBOTTOM:
     case CENTERBOTTOM:
     case RIGHTBOTTOM:
       vert_just = BOTTOM_TEXT;
       break;
    }

  settextjustify(horiz_just, vert_just ); assert( graphresult() == grOk );

  outtextxy(x, y, str);

  curX = -1; //something invalid
  curY = -1; //something invalid

  curUpDown = UP;

}



void ScreenPlot::setPenColor( DrawColor color )
{
  if ( penColor == color ) return;

  assert( color >= 0 && color <= 6 );

 // penUp();

  int screen_color;

  switch( color )
    {
     case Plot::BLACK:
       screen_color = ::LIGHTGRAY;
       break;
     case Plot::BLUEGREEN:
       screen_color = ::CYAN;
       break;
     case Plot::GREEN:
       screen_color = ::GREEN;
       break;
     case Plot::RED:
       screen_color =::LIGHTRED;
       break;
     case Plot::BLUE:
       screen_color = ::LIGHTBLUE;
       break;
     case Plot::PURPLE:
       screen_color = ::LIGHTMAGENTA;
       break;
     default:
       screen_color = 14; // shouldn't ever get here
    }




  setcolor( screen_color );

  penColor = color;
}

