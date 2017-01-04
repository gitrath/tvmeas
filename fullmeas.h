#ifndef fullmeas_h
#define fullmeas_h

#include "antpat.h"
#include "tlabelin.h"

#define Uses_TDialog
#define Uses_TEvent
#define Uses_TRadioButtons
#include <tv.h>

class FullMeasureDialog : public TDialog
{
public:
  FullMeasureDialog();

  virtual void handleEvent( TEvent& event );

protected:
  void   go();
  void   homeTurntable();

  TLabeledNumericInputLine *startFreqInputLine;
  TLabeledNumericInputLine *stopFreqInputLine;
  TLabeledNumericInputLine *numFreqPointsInputLine;
  TLabeledNumericInputLine *startAngleInputLine;
  TLabeledNumericInputLine *degOfRotationInputLine;
  TLabeledNumericInputLine *homeOffsetInputLine;
  TLabeledNumericInputLine *amplOffsetInputLine;

  TLabeledInputLine        *antennaNameInputLine;
  TLabeledInputLine        *commentInputLine;

  TRadioButtons            *detectModeRadioButtons;
  TRadioButtons            *planeRadioButtons;
  TRadioButtons            *polarizationRadioButtons;


private:
  const char *defaultFileName(double freq);
  void        setLabels( AntennaPattern *patPtr );
};

#endif