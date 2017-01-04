#ifndef multlbox_h
#define multlbox_h

#include "vectbase.h"

#define Uses_TEvent
#define Uses_TLabel
#define Uses_TRect
#define Uses_TScrollBar
#define Uses_TSortedCollection
#define Uses_TSortedListBox

#include <tv.h>

class MultipleSelectListBox : public TSortedListBox
{
public:

 MultipleSelectListBox( const TRect&, TScrollBar *aScrollBar );

 void newList(TSortedCollection *aList);

 virtual TPalette& getPalette() const;
 virtual void handleEvent(TEvent& event);
 virtual void draw();


 void    toggleSelect( short item );
 virtual Boolean isSelected( short item );

protected:
 VectorBase<Boolean> selectFlag;

};


#endif