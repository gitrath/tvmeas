#ifndef TVMEAS_H
#define TVMEAS_H

//#include "list.h"
#include "patwind.h"
#include "simpmeas.h"
#include "fullmeas.h"
#include "plotdiag.h"
#include "pwinlist.h"

#define Uses_TApplication
#define Uses_TDeskTop
#define Uses_TDialog
#define Uses_TEvent
#define Uses_TMenuBar
#define Uses_TRect
#define Uses_TStatusLine

#include <tv.h>


class MyApplication : public TApplication
{

public:

  MyApplication();

  virtual void handleEvent( TEvent& event );
  virtual void outOfMemory();

  static TMenuBar *initMenuBar( TRect );

  void openFile();

  static void addPatternWindow( const AntennaPattern &pat, const char *fileName = "", Boolean needsToBeSaved = True );
  static void deletePatternWindow( PatternWindow *patWind );

  static PatternWindowList patternWindowList;

  void mySuspend() { suspend(); }
  void myResume()  { resume();  }

protected:
  //static List< PatternWindow * > patternWindowList;

  static SimpleMeasureDialog simpleMeasureDialog;

  static FullMeasureDialog fullMeasureDialog;

  static PlotDialog plotDialog;

  ushort quit();

};


const cmAbout               = 200;

const cmOpen                = 101;
const cmSave                = 102;
const cmSaveAs              = 103;
const cmSaveAll             = 104;
const cmPlotGraph           = 105;
const cmVideoMode           = 106;
const cmGo                  = 108;
const cmMoreOptions         = 109;
const cmLessOptions         = 110;
const cmHome                = 111;
const cmPlotterPlot         = 112;
const cmSpoolPlot           = 113;
const cmScreenPlot          = 114;
const cmQuitIfNoWindowsOpen = 115;

Boolean fileExists( char *name );


#endif