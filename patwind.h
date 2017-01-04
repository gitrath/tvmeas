#ifndef PATWIND_H
#define PATWIND_H

#include <stdlib.h>
#include <assert.h>
#include "avector.h"
#include "antpat.h"
#include "tlabelin.h"

#define Uses_TDialog
#define Uses_TEvent
#define Uses_TListViewer
#define Uses_TRect
#define Uses_TScrollBar
#include <tv.h>

#define START_DISPLAY_LABEL 1
#define STOP_DISPLAY_LABEL  6

class PatternWindow : public TDialog, public AntennaPattern
{
public:

  PatternWindow( const AntennaPattern &pat );

  virtual void handleEvent(TEvent& event);
  virtual void close();

  void        setFileName( const char *fileName);
  const char *getFileName() const;

  void updateLabels();

  Boolean needsSaving;

protected:
  TLabeledInputLine *inputLine[STOP_DISPLAY_LABEL-START_DISPLAY_LABEL+1];

  Vector<char> FileName;


  ushort save();
  ushort saveAs();

};


class PatternScroller : public TListViewer
{
public:

  PatternScroller( const TRect& bounds, TScrollBar *aVScrollBar,
                    AVector<double> *angleVec, AVector<double> *amplVec ); // constructor

  virtual void getText( char *dest, short item, short maxLen );

protected:

  AVector<double> *angleVecPtr;
  AVector<double> *amplVecPtr;
  double         maxAmpl;

};


#endif