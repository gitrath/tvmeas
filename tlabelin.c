#include <assert.h>
#include <string.h>
#include "tlabelin.h"
#include <strstream.h>

#define INPUTLINE_BUFSIZE 128

static int labelLen( const char *s )
{
  int length;
  const char *ptr;

  for(length=0, ptr=s; *ptr != '\0'; ptr++)
    {
     if( *ptr!='~' ) length++;
    }

  return length;
}


TLabeledInputLine::TLabeledInputLine( const char *label,
   int x, int y, int length, const char *defaultStr, const char *units) :
      TInputLine( TRect( x,y,x+length,y+1), INPUTLINE_BUFSIZE )
{
  if ( defaultStr )
    {
     setData( (void *) defaultStr );
    }

    char str[ 80 ];
    ostrstream s(str, sizeof(str) );
    s << ' ' << units;

    tunits = new TStaticText( TRect( x+length,y,x+length+1+strlen(units),y+1), s.str());

    char str_label[ 80 ];
    ostrstream s_label(str_label, sizeof(str_label) );
    s_label << label  << ": ";

    tlabel = new TLabel( TRect( x-labelLen(label)-3,y,x-1,y+1), s_label.str(), this);

};



TLabeledNumericInputLine::TLabeledNumericInputLine( const char *label,
  int x, int y, int length, double defaultValue, const char *units) :
  TLabeledInputLine( label,x,y,length, 0, units)
{

  char defaultStr[20]; ostrstream  defaultStr_s( defaultStr, sizeof(defaultStr) );
  defaultStr_s << defaultValue << '\0';

  setData( defaultStr_s.str() );

}


double TLabeledNumericInputLine::getValue()
{
  char *s = new char[ dataSize() ]; assert(s);
  getData( s );

  istrstream sstream( s, dataSize() );

  double value;

  assert( sstream.good() );
  sstream >> value;
  assert( sstream.eof() || sstream.good() );

  delete s;

  return value;
}


void  TLabeledNumericInputLine::handleEvent( TEvent& event )
{

  if ( event.what == evKeyDown )
    {
      char c = event.keyDown.charScan.charCode;

      if ( c < ' ' || (c >= '0' && c <= '9') || c == '.' || c == 'e' || c== 'E' || c=='-')
        {
         TLabeledInputLine::handleEvent( event );
        }
    }
  else
    {
      TLabeledInputLine::handleEvent( event );
    }
}