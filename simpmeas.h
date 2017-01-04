#ifndef simpmeas_h
#define simpmeas_h

#include "tlabelin.h"

#define Uses_TDialog
#define Uses_TEvent
#include <tv.h>


class SimpleMeasureDialog : public TDialog
{
public:
  SimpleMeasureDialog();

  virtual void handleEvent( TEvent& event );

protected:
  void   go();

  TLabeledNumericInputLine *freqInputLine;


};

#endif