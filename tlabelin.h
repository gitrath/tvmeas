#ifndef tlabelin_h
#define tlabelin_h

#define Uses_TEvent
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_TRect
#define Uses_TStaticText
#define Uses_TView
#include <tv.h>


class TLabeledInputLine : public TInputLine
{
public:

  TLabeledInputLine( const char *label, int x, int y, int length,
                     const char *defaultStr=0, const char *units="");

  TLabel      *tlabel;
  TStaticText *tunits;

};

class TLabeledNumericInputLine : public TLabeledInputLine
{
public:

  TLabeledNumericInputLine( const char *label, int x, int y, int length,
                     double defaultValue, const char *units="");

  double getValue();

  virtual void handleEvent( TEvent& event );

};

#endif