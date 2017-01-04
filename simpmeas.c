#include <assert.h>
#include <iostream.h>
#include <strstream.h>
#include "simpmeas.h"
#include "patmeas.h"
#include "antpat.h"
#include "patwind.h"
#include "tvmeas.h"
#include "waitdiag.h"

#define Uses_TButton
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TProgram
#define Uses_TRect


#include <tv.h>


SimpleMeasureDialog::SimpleMeasureDialog() :
  TDialog( TRect( 17, 5, 62, 12 ), "Measure Antenna Pattern"),
  TWindowInit( SimpleMeasureDialog::initFrame )
{
   //disableCommand( cmSave   );
   //disableCommand( cmSaveAs );

   freqInputLine = new TLabeledNumericInputLine( "Frequency", 21,2,9,12.5, "GHz");
   assert( freqInputLine );

   insert( freqInputLine );
   insert( freqInputLine->tlabel );
   insert( freqInputLine->tunits );

   TButton *button =  new TButton( TRect( 2, 8-4, 18, 10-4 ), "~M~ore Options", cmMoreOptions,
                    bfNormal );
   insert(button);

//   button->setState( sfDisabled, False );

//   insert( new TButton( TRect( 20, 8-4, 30, 10-4 ), "~C~ancel", cmCancel,
//                    bfNormal ));
   insert( new TButton( TRect( 32-15+15, 8-4, 42-15+15, 10-4), "~G~o", cmGo,
                    bfDefault ));

   flags &= ~wfClose;  // no close box on title bar

}


void SimpleMeasureDialog::handleEvent( TEvent& event )
{
  TDialog::handleEvent( event );

  switch( event.what )
    {
     case evCommand:
     	switch( event.message.command )
         {
          case cmGo:
            go();
            clearEvent( event );
            break;
         }
       break;
     case evBroadcast:
       switch( event.message.command )
         {
          case cmReceivedFocus:
            if ( event.message.infoPtr == this)
              {
               disableCommand( cmSave );
               disableCommand( cmSaveAs );
              }
            break;
         }
       break;
    }
}


void SimpleMeasureDialog::go()
{
  WaitDialog waitDialog("Taking Measurement");
  TProgram::deskTop->insert( &waitDialog );

  PatternMeasurer measure;

  AntennaPattern pat;

  measure.setFreq( freqInputLine->getValue()*1e9 );
  measure.takeMeasurement( &pat );

  TProgram::deskTop->remove( &waitDialog );


  static int patCount = 1;

  char filename[80];
  ostrstream os( filename, sizeof(filename)-1 );
  os << "PAT" << patCount << ".PAT" << '\0';
  patCount++;

  //pat.setLabel( AntennaPattern::FILE_NAME, os.str() );

  MyApplication::addPatternWindow( pat, os.str() );


}

