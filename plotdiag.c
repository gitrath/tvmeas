#include <assert.h>
#include <iostream.h>
#include <strstream.h>
#include <string.h>
#include "plotdiag.h"
#include "plotpat.h"
#include "vector.h"
#include "waitdiag.h"
#include "tvmeas.h"
#include "hpgl.h"
#include "scrnplot.h"

#define Uses_TButton
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TLabel
#define Uses_TProgram
#define Uses_TRect
#define Uses_TScrollBar
#define Uses_TSortedListBox
#define Uses_TStringCollection

#include <tv.h>


#define MAX_TITLE_LENGTH 128



PlotDialog::PlotDialog() :
  TDialog( TRect( 3, 2, 75, 19 ), "Plot Antenna Patterns"),
  TWindowInit( PlotDialog::initFrame )
{
  titleInputLine            =new TLabeledInputLine( "~T~itle", 9,2,61,"" );

  const int xx = 43+11;
  const int yy = 4;

  amplSpanInputLine         =new TLabeledNumericInputLine( "Amplitude ~S~pan", xx,yy,9,30,"dB" );
  amplGridSpacingInputLine  =new TLabeledNumericInputLine( "Amplitude ~G~rid Spacing", xx,yy+2,9,3.0,"dB" );
  amplLabelSpacingInputLine =new TLabeledNumericInputLine( "Amplitude ~L~abel Spacing", xx,yy+4,9,6.0,"dB" );
  angleGridSpacingInputLine =new TLabeledNumericInputLine( "Angle G~r~id Spacing", xx,yy+6,9,10.0,"deg" );
  angleLabelSpacingInputLine=new TLabeledNumericInputLine( "Angle L~a~bel Spacing", xx,yy+8,9,30.0,"deg" );

  insert( titleInputLine );
  insert( titleInputLine->tlabel);
  insert( titleInputLine->tunits);


  ////////////////////////////

  TScrollBar        *sb;

  const int listX  = 4;
  const int listY  = 5;
  const int listWd = 19;
  const int listHt = 8;

  sb = new TScrollBar( TRect( listX+listWd, listY, listX+listWd+1, listY+listHt )); assert(sb);

  list = new MultipleSelectListBox( TRect(listX, listY, listX + listWd,
                        listY + listHt),sb); assert(list);

  insert(sb);
  insert(list);

  insert( new TLabel( TRect( listX,listY-1,listX+listWd,listY), "~P~atterns to Plot:", list));



  ////////////////////////////

  insert( amplSpanInputLine );
  insert( amplGridSpacingInputLine );
  insert( amplLabelSpacingInputLine );
  insert( angleGridSpacingInputLine );
  insert( angleLabelSpacingInputLine );

  insert( amplSpanInputLine->tlabel );
  insert( amplGridSpacingInputLine->tlabel );
  insert( amplLabelSpacingInputLine->tlabel );
  insert( angleGridSpacingInputLine->tlabel );
  insert( angleLabelSpacingInputLine->tlabel );

  insert( amplSpanInputLine->tunits );
  insert( amplGridSpacingInputLine->tunits );
  insert( amplLabelSpacingInputLine->tunits );
  insert( angleGridSpacingInputLine->tunits );
  insert( angleLabelSpacingInputLine->tunits );


  const int by = 14;

  TButton *cancelButton;
  TButton *plotButton;
  TButton *spoolButton;
  TButton *screenButton;

  int x = 1;
  cancelButton = new TButton( TRect( x, by, x+10, by+2 ), "~C~ancel", cmCancel, bfNormal );

  x += 11;
  plotButton   = new TButton( TRect( x, by, x+19, by+2 ), "Plot to Plotter", cmPlotterPlot, bfNormal );

  x += 20;
  spoolButton  = new TButton( TRect( x, by, x+20, by+2 ), "Spool to Plotter", cmSpoolPlot, bfNormal );

  x += 21;
  screenButton = new TButton( TRect( x, by, x+18, by+2 ), "Screen Preview", cmScreenPlot, bfDefault );

  cancelButton->setState( sfDisabled, False );
  plotButton->setState  ( sfDisabled, False );
  spoolButton->setState ( sfDisabled, False );
  screenButton->setState( sfDisabled, False );



  insert( cancelButton );
  insert( plotButton   );
  insert( spoolButton  );
  insert( screenButton );

}



void  PlotDialog::setPatternList( PatternWindowList *patWindCollection)
{
  TStringCollection *coll = new TStringCollection( 100, 10 );
  coll->duplicates = True;

  short topWindowItem = -1;  // -1 means no top window item found

  for(int i=0; i< patWindCollection->getCount(); i++)
    {
      PatternWindow *patWind = (PatternWindow *) patWindCollection->at(i);

      Vector<char> fileName = patWind->getLabel( AntennaPattern::FILE_NAME );

      char *p = new char[ strlen(fileName)+1 ];
      strcpy( p,fileName);

      coll->insert( p );

      if ( TProgram::deskTop->current == patWind )
        {
         topWindowItem = i;
        }

    }

  list->newList(coll);

  if ( topWindowItem >= 0 )
    {
     list->toggleSelect( topWindowItem ); // select it
    }

}



const char *PlotDialog::getTitle()
{
  static char buf[ MAX_TITLE_LENGTH+1 ];

  //cout << endl << titleInputLine->dataSize() << endl;

  assert(titleInputLine->dataSize() <= MAX_TITLE_LENGTH);

  titleInputLine->getData( buf );

  return buf;
}



void PlotDialog::doPlot(PlotDestination destination)
{

  WaitDialog waitDialog( "Plotting" );
  TProgram::deskTop->insert( &waitDialog );

  Plot *plot_driver = 0;

  switch (destination)
    {
     case DIRECT_TO_PLOTTER:
       plot_driver = new HPGL();
       break;
     case SPOOL_TO_PLOTTER:
       plot_driver = new HPGL("plot.hp");
       break;
     case TO_SCREEN:
       plot_driver = new ScreenPlot();
       break;
    }


  assert(plot_driver);

  PlotPattern plot(plot_driver);

  plot.setTitle( getTitle() );

  plot.setMinMax( -amplSpanInputLine->getValue(), 0 );
  plot.setAmplGrid( amplGridSpacingInputLine->getValue() );
  plot.setAngleGrid( angleGridSpacingInputLine->getValue() );
  plot.setAmplLabels( amplLabelSpacingInputLine->getValue() );
  plot.setAngleLabels( angleLabelSpacingInputLine->getValue() );

  for( int i=0; i< (list->list())->getCount()   ; i++)
    {
     if ( list->isSelected(i) )
       {
        PatternWindow *patWind = (PatternWindow *) MyApplication::patternWindowList.at( i );
        patWind->updateLabels();  // make sure inputlines correspond to label fields of AntennaPattern
        plot.addPattern( patWind );
       }
    }

  plot.plot();

  delete plot_driver;

  if (destination == SPOOL_TO_PLOTTER)
    {
     system("dmp");
     system("pf plot.hp");
    }

  TProgram::deskTop->remove( &waitDialog );

}


void PlotDialog::handleEvent(TEvent& event)
{


  switch ( event.what )
    {
     case evCommand:
       switch( event.message.command )
         {
          case cmPlotterPlot:
            doPlot( DIRECT_TO_PLOTTER );
            event.message.command = cmOK;
            break;
          case cmSpoolPlot:
            doPlot( SPOOL_TO_PLOTTER );
            event.message.command = cmOK;
            break;
          case cmScreenPlot:
            doPlot( TO_SCREEN );
            event.message.command = cmOK;
            break;
          default:
            break;
         }
       break;
     case evBroadcast:
       break;
     case evKeyDown:
       break;

   }

  TDialog::handleEvent( event );

}

