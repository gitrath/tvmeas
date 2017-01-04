#include <stdio.h>
#include <dir.h>
#include <string.h>
#include <stdlib.h>
#include <strstream.h>
#include <iomanip.h>
#include "patwind.h"
#include "plotpat.h"
#include "waitdiag.h"
#include "tvmeas.h"

#define Uses_MsgBox
#define Uses_TDeskTop
#define Uses_TFileDialog
#define Uses_TKeys
#define Uses_TLabel
#define Uses_TView
#include <tv.h>


PatternWindow::PatternWindow( const AntennaPattern &pat) :
  TDialog( TRect(1,1,41,21),
           pat.getLabel( AntennaPattern::FILE_NAME )),
  TWindowInit( PatternWindow::initFrame ),
  AntennaPattern( pat )
{
    needsSaving = True;

    static int winCount = 0;

    moveTo( 1+(winCount % 37), 1+(winCount % 5 ) );
    winCount++;

    for(int i=0; i<STOP_DISPLAY_LABEL-START_DISPLAY_LABEL+1; i++)
      {
       LabelIndex index = (LabelIndex) (i+START_DISPLAY_LABEL);

       inputLine[i] = new TLabeledInputLine(
                       getLabelName(index), 14+5-3, 1+i,21+1, getLabel(index) );
       insert( inputLine[i] );
       insert( inputLine[i]->tlabel );
       insert( inputLine[i]->tunits );
      }


    TRect r = getClipRect();    // get exposed view bounds
    r.grow( -1, -1 );           // shrink to fit inside window frame

    r.b.x -= 1;

    r.a.y += 1+STOP_DISPLAY_LABEL-START_DISPLAY_LABEL+1+1;

    TScrollBar *vScrollBar = new TScrollBar( TRect( r.b.x, r.a.y, r.b.x+1, r.b.y ) );

    PatternScroller *scroller =  new PatternScroller( r, vScrollBar,
            &angleVec, &amplVec );

    TView *frameView = new TView( TRect( r.a.x, r.a.y-1, r.b.x+1, r.b.y) );

    frameView->options |= ofFramed;

    insert( frameView );

    insert( vScrollBar );

    insert( scroller );

    insert( new TLabel( TRect( r.a.x, r.a.y-1, r.b.x, r.a.y ), "     Angle   Abs Ampl   Rel Ampl", scroller));

}





void PatternWindow::handleEvent(TEvent& event)
{
  TDialog::handleEvent( event );

  switch ( event.what )
    {
     case evCommand:
       switch( event.message.command )
         {
          case cmSave:
            save();
            break;
          case cmSaveAs:
            saveAs();
            break;
          default:
            break;
         }
       break;
     case evBroadcast:
       switch( event.message.command )
         {
          case cmReceivedFocus:
            if ( event.message.infoPtr == this )
              {
               enableCommand( cmSave );
               enableCommand( cmSaveAs );
              }
            break;
         }
       break;
     case evKeyDown:
       switch (event.keyDown.keyCode)
         {
          case kbDown:
            if( current != inputLine[ STOP_DISPLAY_LABEL-START_DISPLAY_LABEL ] )
              {
               event.keyDown.keyCode = kbTab;
               putEvent(event);
               clearEvent(event);
              }
            break;
          case kbUp:
            if( current != inputLine[0] )
              {
               event.keyDown.keyCode = kbShiftTab;
               putEvent(event);
               clearEvent(event);
              }
            break;
         }
     break;

   }
}

void PatternWindow::updateLabels()
{
  for(int i=0; i<STOP_DISPLAY_LABEL-START_DISPLAY_LABEL+1; i++)
    {
     LabelIndex index = (LabelIndex) (i+START_DISPLAY_LABEL);

     assert( strlen( inputLine[i]->data) < 128 );

     if ( strcmp( inputLine[i]->data, getLabel(index) ) )
       {
        setLabel(index, inputLine[i]->data );
        needsSaving = True;
       }
    }
}



void PatternWindow::close()
{
  updateLabels();

  ushort answer = cmNo;

  if ( needsSaving == True )
    {
     char msg[80];

     ostrstream os( msg, sizeof(msg) );

     os << FileName << " not saved. Save? " << '\0';

     answer = messageBox( os.str() , mfWarning | mfYesNoCancel );

     if (answer == cmYes )
       {
        answer = saveAs();
       }
    }


  if ( answer != cmCancel )
    {
     MyApplication::deletePatternWindow( this );
     TDialog::close();
    }
}


ushort PatternWindow::save()
{
  updateLabels();

  if ( strlen( getFileName() ) > 0 )
    {
      WaitDialog waitDialog("Saving Pattern");
      TProgram::deskTop->insert( &waitDialog );

      AntennaPattern::write( getFileName() );
      needsSaving = False;

      TProgram::deskTop->remove( &waitDialog );

      return cmOK;
    }

  return saveAs();
}


ushort PatternWindow::saveAs()
{
  updateLabels();

  TFileDialog *d;

  const char *defaultFile = strlen(getFileName())>0 ? getFileName() : "*.PAT" ;

  ushort result = cmCancel;

  d = new TFileDialog(defaultFile, "Save file as",
           "~N~ame", fdOKButton, 101);

  assert( d );

  if (TProgram::deskTop->execView(d) != cmCancel)
    {
      char fileName[MAXPATH];

      d->getFileName(fileName);
      if ( fileExists( fileName ) )
        {
         const  int MAXSIZE  = 150;
         char   msg[MAXSIZE];

         strcpy( msg, "Overwrite existing file:\n  " );
         strcat( msg,  fileName );
         ushort answer = messageBox( msg, mfWarning | mfYesNoCancel );

         if (answer == cmYes)
           {
            AntennaPattern::write( fileName );
            needsSaving = False;
            setFileName( fileName );
            result = cmOK;
           }
       }
     else
       {
         AntennaPattern::write( fileName );
         needsSaving = False;
         setFileName( fileName );
         result = cmOK;
       }
   }
 destroy( d );


 return result;
}



void PatternWindow::setFileName( const char *fileName)
{
  assert( fileName );

  FileName = fileName;

  const char *name = fileName;
  while( *name != '\0' ) name++;
  while( name>fileName && *(name-1) != '\\' && *(name-1) != '/' && *(name-1) != ':' ) name--;

  setLabel( AntennaPattern::FILE_NAME, name );

  delete  (char *) TWindow::title;
  TWindow::title = newStr( name );

  redraw();
}

const char *PatternWindow::getFileName() const
{
  assert( FileName.size() > 0 );

  return FileName;
}


PatternScroller::PatternScroller( const TRect& bounds, TScrollBar *aVScrollBar ,
              AVector<double> *angleVec, AVector<double> *amplVec) :
       TListViewer( bounds, 1, 0, aVScrollBar )
{

    growMode = gfGrowHiX | gfGrowHiY;
    //options = options | ofFramed;

    angleVecPtr = angleVec;
    amplVecPtr  = amplVec;
    maxAmpl = amplVec->maxValue();

    assert( angleVecPtr->size() == amplVecPtr->size() );

    setRange( amplVecPtr->size() );
}


void PatternScroller::getText( char *dest, short item, short maxLen )
{

  assert( item >= 0);
  assert( item < amplVecPtr->size() );

  ostrstream os( dest, maxLen );

  os.setf( ios::showpoint );

  os << ' ';

  os << setprecision(2) << setw(9) << (*angleVecPtr)[item] << "  ";

  os << setprecision(2) << setw(9) << (*amplVecPtr)[item] << "  ";

  os << setprecision(2) << setw(9) << ((*amplVecPtr)[item]-maxAmpl) << '\0';

}


