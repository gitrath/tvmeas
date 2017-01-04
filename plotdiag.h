#ifndef plotdiag_h
#define plotdiag_h

#define Uses_TButton
#define Uses_TDialog
#define Uses_TEvent
#include <tv.h>
#include "tlabelin.h"
#include "antpat.h"
#include "patwind.h"
#include "pwinlist.h"
#include "multlbox.h"

class PlotDialog : public TDialog
{
public:
  PlotDialog();

  void  setPatternList( PatternWindowList *patWindCollection );

  enum PlotDestination { DIRECT_TO_PLOTTER, SPOOL_TO_PLOTTER, TO_SCREEN };

  void  doPlot( PlotDestination destination);

  virtual void handleEvent(TEvent& event);


protected:

  const char *getTitle();

  TLabeledInputLine *titleInputLine;

  TLabeledNumericInputLine *amplSpanInputLine;
  TLabeledNumericInputLine *amplGridSpacingInputLine;
  TLabeledNumericInputLine *angleGridSpacingInputLine;
  TLabeledNumericInputLine *amplLabelSpacingInputLine;
  TLabeledNumericInputLine *angleLabelSpacingInputLine;

  MultipleSelectListBox *list;


};

#endif