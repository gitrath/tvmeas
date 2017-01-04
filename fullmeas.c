#include <assert.h>
#include <iomanip.h>
#include <strstream.h>
#include <string.h>
#include "fullmeas.h"
#include "patmeas.h"
#include "antpat.h"
#include "receiver.h"
#include "patwind.h"
#include "tvmeas.h"
#include "waitdiag.h"
#include "vector.h"
#include "lin3circ.h"

#define Uses_MsgBox
#define Uses_TButton
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TProgram
#define Uses_TRect
#define Uses_TSItem

#include <tv.h>


FullMeasureDialog::FullMeasureDialog() :
  TDialog( TRect( 15, 0, 15+49,0+23 ), "Measure Antenna Pattern"),
  TWindowInit( FullMeasureDialog::initFrame )
{
  flags &= ~wfClose;  // no close box on title bar

  int x = 26;
  int y = 2;


  startFreqInputLine    = new TLabeledNumericInputLine( "Start Frequency", x,y,9,12.5, "GHz");
  stopFreqInputLine     = new TLabeledNumericInputLine( "Stop Frequency", x, y+1, 9, 12.6, "GHz");
  numFreqPointsInputLine= new TLabeledNumericInputLine( "# Frequency Points", x, y+2, 9, 1);
  startAngleInputLine   = new TLabeledNumericInputLine( "Start Angle", x, y+4, 9, -180, "Deg");
  degOfRotationInputLine= new TLabeledNumericInputLine( "Degrees of Rotation", x, y+5, 9, 360, "Deg");
  homeOffsetInputLine   = new TLabeledNumericInputLine( "Turntable Home Offset", x, y+7, 9, 0, "Deg");
  amplOffsetInputLine   = new TLabeledNumericInputLine( "Amplitude Offset", x, y+8, 9, 0, "dB");

  insert( startFreqInputLine );
  insert( startFreqInputLine->tlabel );
  insert( startFreqInputLine->tunits );

  insert( stopFreqInputLine );
  insert( stopFreqInputLine->tlabel );
  insert( stopFreqInputLine->tunits );

  insert( numFreqPointsInputLine );
  insert( numFreqPointsInputLine->tlabel );
  insert( numFreqPointsInputLine->tunits );

  insert( startAngleInputLine );
  insert( startAngleInputLine->tlabel );
  insert( startAngleInputLine->tunits );

  insert( degOfRotationInputLine );
  insert( degOfRotationInputLine->tlabel );
  insert( degOfRotationInputLine->tunits );

  insert( homeOffsetInputLine );
  insert( homeOffsetInputLine->tlabel );
  insert( homeOffsetInputLine->tunits );


  insert( amplOffsetInputLine );
  insert( amplOffsetInputLine->tlabel );
  insert( amplOffsetInputLine->tunits );

  x = 26-7;
  y = 2+10;

  antennaNameInputLine  = new TLabeledInputLine( "Antenna Name", x,y,22,"" );
  commentInputLine      = new TLabeledInputLine( "Comment", x,y+1,22,"" );

  insert( antennaNameInputLine );
  insert( antennaNameInputLine->tlabel );
  insert( antennaNameInputLine->tunits );

  insert( commentInputLine );
  insert( commentInputLine->tlabel );
  insert( commentInputLine->tunits );

  x = 2;
  y += 4;
  int w= 12;

  detectModeRadioButtons = new TRadioButtons( TRect( x,y,x+w,y+3),
            new TSItem( "Sample",
             new TSItem( "Pos",
              new TSItem( "Neg", 0 ))));

  insert( detectModeRadioButtons );

  insert( new TLabel( TRect( x, y-1, x+w, y), "Detect Mode", detectModeRadioButtons ));

  x = 16;
  w= 14;

  planeRadioButtons = new TRadioButtons( TRect( x,y,x+w,y+3),
            new TSItem( "E-Plane",
             new TSItem( "H-Plane",
              new TSItem( "Other", 0 ))));

  planeRadioButtons->press(2);
  insert( planeRadioButtons );

  insert( new TLabel( TRect( x, y-1, x+w, y), "Plane", planeRadioButtons ));

  x = 32;
  w = 15;

  polarizationRadioButtons = new TRadioButtons( TRect( x,y,x+w,y+4),
            new TSItem( "Co-Pol",
             new TSItem( "Cross-Pol",
              new TSItem( "Circular",
               new TSItem( "Other", 0 )))));

  polarizationRadioButtons->press(3);
  insert( polarizationRadioButtons );

  insert( new TLabel( TRect( x, y-1, x+w, y), "Polarization", polarizationRadioButtons ));

  x = 40;
  y = 9;
  w = 8;

  insert( new TButton( TRect( x, y, x+w, y+2), "~H~ome", cmHome,
                    bfNormal ));

  x = 1;
  y = 20;
  w = 16;

  insert( new TButton( TRect( x, y, x+w, y+2), "~L~ess Options", cmLessOptions,
                    bfNormal ));

  x = 21;
  y = 20;
  w = 10;

  insert( new TButton( TRect( x, y, x+w, y+2), "~G~o", cmGo,
                    bfDefault ));


}

void FullMeasureDialog::handleEvent( TEvent& event )
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
          case cmHome:
            homeTurntable();
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


void FullMeasureDialog::go()
{
  WaitDialog waitDialog("Taking Measurement");
  TProgram::deskTop->insert( &waitDialog );

  PatternMeasurer measure;

  AntennaPattern pat;

  measure.setStartAngle   ( startAngleInputLine->getValue() );
  measure.setDegOfRotation( degOfRotationInputLine->getValue() );
  measure.setHomeOffset   ( homeOffsetInputLine->getValue() );

  SpecAnalyzer specAnal;

  ushort value;
  detectModeRadioButtons->getData( &value );

  switch( value )
    {
     case 0:
       specAnal.setDetectionMode( SpecAnalyzer::SAMPLE);
       break;
     case 1:
       specAnal.setDetectionMode( SpecAnalyzer::POSITIVE);
       break;
     case 2:
       specAnal.setDetectionMode( SpecAnalyzer::NEGATIVE);
       break;
    }

  Boolean isCPMeasurement;
  polarizationRadioButtons->getData( &value );
  if ( value == 2 ) // circular polarization
    {
     isCPMeasurement = True;
    }
  else
    {
     isCPMeasurement = False;
    }

  double amplOffset = amplOffsetInputLine->getValue();

  int numFreq = int( numFreqPointsInputLine->getValue() );
  assert( numFreq > 0 );

  double startFreq = startFreqInputLine->getValue();
  double stopFreq  = stopFreqInputLine->getValue();

  for( int i=0; i<numFreq; i++)
    {
     double freq = startFreq;

     if (numFreq > 1 )
       {
        freq += i*(stopFreq-startFreq)/double(numFreq-1);
       }

     freq *= 1e9;

     measure.setFreq( freq );

     if( isCPMeasurement == True )
       {
        char *msg[3] = { "\003Rotate transmit antenna to\n\003horizontal polarization",
                         "\003Rotate transmit antenna to\n\00345 deg diagonal polarization",
                         "\003Rotate transmit antenna to\n\003vertical polarization" };

        AntennaPattern pat_max;
        AntennaPattern pat_min;

        {
          AntennaPattern pat_cp[3];

          for(int j=0; j<3; j++)
            {
             messageBox( msg[j], mfOKButton | mfConfirmation );

             measure.takeMeasurement( &pat_cp[j] );
             pat_cp[j].amplVec += amplOffset;
            }



          ThreeLinearToCircular( pat_cp[0], pat_cp[1], pat_cp[2],
                                 &pat_max, &pat_min );

          pat_max.setLabel( AntennaPattern::FREQUENCY, pat_cp[0].getLabel( AntennaPattern::FREQUENCY) );
          pat_min.setLabel( AntennaPattern::FREQUENCY, pat_cp[0].getLabel( AntennaPattern::FREQUENCY) );

        }

        setLabels( &pat_max );
        setLabels( &pat_min );

        pat_max.setLabel( AntennaPattern::POLARIZATION, "CP Max" );
        pat_min.setLabel( AntennaPattern::POLARIZATION, "CP Min" );


        MyApplication::addPatternWindow( pat_max, defaultFileName(freq) );
        MyApplication::addPatternWindow( pat_min, defaultFileName(freq) );
       }

     else // isCPMeasurement == False
       {

        measure.takeMeasurement( &pat );

        pat.amplVec += amplOffset;

        setLabels( &pat );

        MyApplication::addPatternWindow( pat, defaultFileName(freq) );
       }

     waitDialog.makeFirst();

    }

  TProgram::deskTop->remove( &waitDialog );

}


void FullMeasureDialog::homeTurntable()
{
  WaitDialog waitDialog("Homing Turntable");
  TProgram::deskTop->insert( &waitDialog );

  Turntable turntable;

  turntable.setHomeOffset( homeOffsetInputLine->getValue() );

  turntable.goHome();

  TProgram::deskTop->remove( &waitDialog );

}

const char *FullMeasureDialog::defaultFileName(double freq)
{
  static char fileName[20]; // should actually be 13??

  ostrstream os( fileName, 20-1 );  // should actually be 13??

  char planeChar = 'O';  // for other

  ushort value;
  planeRadioButtons->getData( &value );

  switch( value )
    {
     case 0:
       planeChar = 'E'; // for E-Plane
       break;
     case 1:
       planeChar = 'H'; // for H-Plane
       break;
    }

  os << planeChar;

  char polarizationChar = 'O'; // for other

  polarizationRadioButtons->getData( &value );

  switch( value )
    {
     case 0:
       polarizationChar = 'C'; // for Co-Pol
       break;
     case 1:
       polarizationChar = 'X'; // for Cross-Pol
       break;
     case 2:
       polarizationChar = 'R'; // for Circular
       break;
    }

  os << polarizationChar;

  char temp[80];
  ostrstream temp_os( temp, 80);

  temp_os.setf( ios::scientific );
  temp_os.setf( ios::showpoint  );
  temp_os << setprecision(10) << freq;

  assert( strlen( temp_os.str() ) >= 6);

  os << (temp_os.str())[0];
  os << (temp_os.str())[2];
  os << (temp_os.str())[3];
  os << (temp_os.str())[4];
  os << (temp_os.str())[5];

  os << 'A';  // initial 8th char of file name

  os << ".PAT" << '\0';

  assert( strlen( fileName ) == 12 );

  // find an 8th char to make fileName unique among PatternWindow names and file names on the hard drive

  for( char c='A'; c<= 'Z'; c++)
    {
     fileName[7] = c;

     Boolean duplicate = False;

     for( int i=0; i< MyApplication::patternWindowList.getCount(); i++)
      {
       PatternWindow *patWind = (PatternWindow *) MyApplication::patternWindowList.at(i);
       if ( !strcmp(patWind->getLabel( AntennaPattern::FILE_NAME ),
                   fileName ) )
         {
          duplicate = True;
          break;
         }
       }

     if ( duplicate ) continue;

     if( !fileExists( fileName ) ) break;

    }

  assert( c <= 'Z' );


  return fileName;
}


void FullMeasureDialog::setLabels( AntennaPattern *patPtr )
{
  patPtr->setLabel( AntennaPattern::ANTENNA, antennaNameInputLine->data );

  patPtr->setLabel( AntennaPattern::COMMENT, commentInputLine->data );

  char *planeStr = "";

  ushort value;
  planeRadioButtons->getData( &value );

  switch( value )
    {
     case 0:
       planeStr = "E-Plane";
       break;
     case 1:
       planeStr = "H-Plane";
       break;
    }

  patPtr->setLabel( AntennaPattern::PLANE, planeStr );

  char *polarizationStr = "";

  polarizationRadioButtons->getData( &value );

  switch( value )
    {
     case 0:
       polarizationStr = "Co-Pol";
       break;
     case 1:
       polarizationStr = "Cross-Pol";
       break;
     case 2:
       polarizationStr = "Circular";
       break;
    }

  patPtr->setLabel( AntennaPattern::POLARIZATION, polarizationStr );

}