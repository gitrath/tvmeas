#include "waitdiag.h"

#define Uses_TStaticText
#include <tv.h>

WaitDialog::WaitDialog( const char *title ) :
  TDialog( TRect( 22, 5, 56, 8 ), title ),
  TWindowInit( WaitDialog::initFrame )

{
  flags &= ~wfClose;

  insert( new TStaticText( TRect( 2,1,32,2), "\003Please Wait..." ) );

}
