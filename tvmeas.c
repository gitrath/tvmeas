#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <dir.h>
#include <string.h>
#include <iostream.h>
#include "tvmeas.h"
#include "tlabelin.h"
#include "simpmeas.h"

#define Uses_MsgBox
#define Uses_TApplication
#define Uses_TButton
#define Uses_TCheckBoxes
#define Uses_TDeskTop
#define Uses_TDisplay
#define Uses_TDialog
#define Uses_TEvent
#define Uses_TFileDialog
#define Uses_TInputLine
#define Uses_TKeys
#define Uses_TLabel
#define Uses_TMenu
#define Uses_TMenuBar
#define Uses_TMenuBox
#define Uses_TMenuItem
#define Uses_TRect
#define Uses_TScreen
#define Uses_TScrollBar
#define Uses_TScroller
#define Uses_TSItem
#define Uses_TStaticText
#define Uses_TStatusDef
#define Uses_TStatusItem
#define Uses_TStatusLine
#define Uses_TSubMenu
#define Uses_TView
#define Uses_TWindow


#include <tv.h>


//Static variables of MyApplication
//List< PatternWindow *>  MyApplication::patternWindowList;
PatternWindowList       MyApplication::patternWindowList;
SimpleMeasureDialog     MyApplication::simpleMeasureDialog;
FullMeasureDialog       MyApplication::fullMeasureDialog;
PlotDialog              MyApplication::plotDialog;


//PatternWindowList myWindList;



MyApplication::MyApplication() :
    TProgInit( initStatusLine, initMenuBar, initDeskTop )
{
  disableCommand( cmSave   );
  disableCommand( cmSaveAs );
  disableCommand( cmPlotGraph );

  patternWindowList.duplicates = True;

  deskTop->insert( &simpleMeasureDialog );

  fullMeasureDialog.hide();
  deskTop->insert( &fullMeasureDialog );
  //deskTop->insert( &plotDialog );
}


TMenuBar *MyApplication::initMenuBar( TRect r )
{

    r.b.y = r.a.y+1;


    return new TMenuBar( r,
      *new TSubMenu( "~\xF0~", kbNoKey ) +
        *new TMenuItem( "~A~bout...", cmAbout, kbNoKey, hcNoContext ) +
      *new TSubMenu( "~F~ile", kbAltF ) +
//	*new TMenuItem( "~N~ew...", cmNew, kbNoKey ) +
	*new TMenuItem( "~O~pen...", cmOpen, kbAltO, hcNoContext, "Alt-O" ) +
	*new TMenuItem( "~S~ave", cmSave, kbAltS, hcNoContext, "Alt-S" ) +
	*new TMenuItem( "S~a~ve as...", cmSaveAs, kbAltA, hcNoContext, "Alt-A" ) +
	     newLine() +
	*new TMenuItem( "~P~lot Graph...", cmPlotGraph, kbAltP, hcNoContext, "Alt-P" ) +
	     newLine() +
	*new TMenuItem( "~V~ideo Mode", cmVideoMode, kbAltV ) +
	     newLine() +
	*new TMenuItem( "E~x~it", cmQuit, cmQuit, hcNoContext, "Alt-X" )


//      *new TSubMenu( "~W~indow", kbAltW ) +
//	*new TMenuItem( "~T~urntable controls", kbNoKey,new TMenu(
//                  *new TMenuItem( "~G~o Home", cmGoHome, kbNoKey ))
//                      ) +
//	*new TMenuItem( "~R~ecevier controls", cmReceiverControls, kbNoKey) +
//	*new TMenuItem( "~S~ource controls", cmSourceControls, kbNoKey)

	);

}

void MyApplication::handleEvent( TEvent& event )
{

    if( event.what == evCommand )
	{
	switch( event.message.command )
	    {
	    case cmMoreOptions:
             simpleMeasureDialog.hide();
             fullMeasureDialog.makeFirst();
             fullMeasureDialog.show();
    	      break;
	    case cmLessOptions:
             fullMeasureDialog. hide();
             simpleMeasureDialog.makeFirst();
             simpleMeasureDialog.show();
	      break;
           case cmOpen:
             openFile();
             break;

	    case cmPlotGraph:
             plotDialog.setPatternList( &patternWindowList );
             deskTop->execView( &plotDialog );
             break;
           case cmAbout:
             messageBox("\003Antenna Measurement Program\n \n\003by Darrin Rath\n\003University of Minnesota, 1993", mfInformation | mfOKButton);
             break;

           case cmVideoMode:
             ushort newMode = TScreen::screenMode ^ TDisplay::smFont8x8;

             setScreenMode(newMode);
             TMouse::setRange( TScreen::screenWidth-1, TScreen::screenHeight-1 );
             break;
           //case cmQuit:
           //  quit();
           //  return;  // don't want to get to TApplication::handleEvent()
           case cmQuitIfNoWindowsOpen:
             if( patternWindowList.getCount() == 0 )
               {
                event.message.command = cmQuit; // change current event so Tapplication::handleEvent() gets cmQuit event
               }
             break;

	    default:
	      break;
	    }
	}

    TApplication::handleEvent( event );
}


void MyApplication::outOfMemory()
{
  messageBox("Out of memory.", mfError | mfOKButton);

  exit(1);
}



void MyApplication::addPatternWindow( const AntennaPattern &pat, const char *fileName,
                                       Boolean needsToBeSaved )
{
  assert( fileName );

  PatternWindow *patWind = new PatternWindow( pat );

  patWind->setFileName( fileName );

  patWind->needsSaving = needsToBeSaved;

  patternWindowList.insert ( patWind );

  deskTop->insert( patWind );

  enableCommand( cmPlotGraph );

}


void MyApplication::deletePatternWindow( PatternWindow *patWind )
{
  patternWindowList.remove( patWind );

  if (patternWindowList.getCount() == 0 )
    {
     disableCommand( cmPlotGraph );
    }
}

ushort MyApplication::quit()
{
  int count = patternWindowList.getCount();

  PatternWindowList windowListCopy( patternWindowList );


  for(int i=0; i< count; i++)
    {
      PatternWindow* patWind = (PatternWindow *) windowListCopy.at(i);

      patWind->close();
    }

  TEvent event;

  event.what = evCommand;
  event.message.command = cmQuitIfNoWindowsOpen;
  event.message.infoPtr = 0;
  putEvent(event);

  return cmOK;  // not actually used (so return value is unimportant )
}

Boolean fileExists( char *name )
{
    struct ffblk sr;
    int ccode;

    ccode = findfirst(name, &sr, 0);
    if (!ccode)
        return True;
    else
        return False;
}


void MyApplication::openFile()
{
    TFileDialog *d;
    char *fileName;
    //TDialog *listEditor;
    const int MAXSIZE  = 150;
    char errorMsg[MAXSIZE];
    extern Boolean fileExists( char *);

    const uchar hlOpenListDlg = 2;

    d = new TFileDialog("*.PAT", "Open File",
           "~N~ame", fdOpenButton, hlOpenListDlg);
    if (validView(d) != NULL)
        {
        if (deskTop->execView(d) != cmCancel)
            {
            fileName = new char[MAXPATH];
            d->getFileName(fileName);
            if (!fileExists(fileName))
                {
                strcpy(errorMsg, "Cannot find file ");
                strcat(errorMsg, fileName);
                messageBox(errorMsg, mfError | mfOKButton);
                }
            else
                {
                 AntennaPattern pat(fileName);
                 addPatternWindow( pat, fileName, False );
                }
            delete fileName;
            }
        destroy(d);
        }
}
