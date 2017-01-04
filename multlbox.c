#include <assert.h>
#include "multlbox.h"

#define Uses_TDrawBuffer
#define Uses_TEvent
#define Uses_TKeys
#define Uses_TPoint

#include <tv.h>


#define cpMultipleSelectListBox "\x1A\x1A\x1B\x1C\x1D\x0A\x11"


MultipleSelectListBox::MultipleSelectListBox( const TRect &r, TScrollBar *sb ) :
  TSortedListBox( r, 1, sb )
{

}


void MultipleSelectListBox::newList(TSortedCollection *aList)
{
  TSortedListBox::newList( aList );

  selectFlag.setSize( aList->getCount() );
  selectFlag.fill( False );

}


Boolean MultipleSelectListBox::isSelected( short item)
{
 assert( item >= 0 );
 assert( item < selectFlag.size() );

 return selectFlag[item];
}

void MultipleSelectListBox::toggleSelect( short item )
{
 assert( item >= 0 );

 if ( item >= selectFlag.size() ) return;

 if( selectFlag[item] == False )
   {
    selectFlag[item] = True;
   }
 else
   {
    selectFlag[item] = False;
   }
}


TPalette& MultipleSelectListBox::getPalette() const
{
    static TPalette palette( cpMultipleSelectListBox, sizeof( cpMultipleSelectListBox )-1 );
    return palette;
}

void MultipleSelectListBox::handleEvent( TEvent& event )
{
    TPoint mouse;
    ushort colWidth;
    short  oldItem, newItem;
    ushort count;
    int mouseAutosToSkip = 4;

    TView::handleEvent(event);

    if( event.what == evMouseDown )
        {
        colWidth = size.x / numCols + 1;
        oldItem =  focused;
        mouse = makeLocal( event.mouse.where );
        newItem = mouse.y + (size.y * (mouse.x / colWidth)) + topItem;

        Boolean changeSelectFlagTo = True;
        if ( newItem < selectFlag.size() && newItem >= 0)
          {
           changeSelectFlagTo = (selectFlag[newItem]==True) ? False : True;
           selectFlag[newItem] = changeSelectFlagTo;
           drawView();
          }

        count = 0;
        do  {
            if( newItem != oldItem )
              {
                if (newItem < selectFlag.size() && newItem >= 0 )
                  selectFlag[newItem] = changeSelectFlagTo;

                focusItemNum( newItem );
                drawView();
              }

            oldItem = newItem;
            mouse = makeLocal( event.mouse.where );
            if( mouseInView( event.mouse.where ) )
              {
                newItem = mouse.y + (size.y * (mouse.x / colWidth)) + topItem;
              }
            else
                {
                if( numCols == 1 )
                    {
                    if( event.what == evMouseAuto )
                        count++;
                    if( count == mouseAutosToSkip )
                        {
                        count = 0;
                        if( mouse.y < 0 )
                            newItem = focused - 1;
                        else
                            if( mouse.y >= size.y )
                                newItem = focused + 1;
                        }
                    }
                else
                    {
                    if( event.what == evMouseAuto )
                        count++;
                    if( count == mouseAutosToSkip )
                        {
                        count = 0;
                        if( mouse.x < 0 )
                            newItem = focused - size.y;
                        else if( mouse.x >= size.x )
                            newItem = focused + size.y;
                        else if( mouse.y < 0 )
                            newItem = focused - focused % size.y;
                        else if( mouse.y > size.y )
                            newItem = focused - focused % size.y + size.y - 1;
                        }
                    }
                }
            } while( mouseEvent( event, evMouseMove | evMouseAuto ) );
        focusItemNum( newItem );
        if( event.mouse.doubleClick && range > focused )
            selectItem( focused );
        clearEvent( event );
        }
    else if( event.what == evKeyDown )
        {
        if (event.keyDown.charScan.charCode ==  ' ' && focused < range )
            {
            selectItem( focused );
            toggleSelect( focused );
            newItem = focused;
            drawView();
            }
        else
            {
            switch (ctrlToArrow(event.keyDown.keyCode))
                {
                case kbUp:
                    newItem = focused - 1;
                    break;
                case kbDown:
                    newItem = focused + 1;
                    break;
                case kbRight:
                    if( numCols > 1 )
                        newItem = focused + size.y;
                    else
                        return;
                    break;
                case kbLeft:
                    if( numCols > 1 )
                        newItem = focused - size.y;
                    else
                        return;
                    break;
                case kbPgDn:
                    newItem = focused + size.y * numCols;
                    break;
                case  kbPgUp:
                    newItem = focused - size.y * numCols;
                    break;
                case kbHome:
                    newItem = topItem;
                    break;
                case kbEnd:
                    newItem = topItem + (size.y * numCols) - 1;
                    break;
                case kbCtrlPgDn:
                    newItem = range - 1;
                    break;
                case kbCtrlPgUp:
                    newItem = 0;
                    break;
                default:
                    return;
                }
            }
        focusItemNum(newItem);
        clearEvent(event);
        }
    else if( event.what == evBroadcast )
        {
        if( (options & ofSelectable) != 0 )
            {
            if( event.message.command == cmScrollBarClicked &&
                  ( event.message.infoPtr == hScrollBar || 
                    event.message.infoPtr == vScrollBar ) )
                select();
            else if( event.message.command == cmScrollBarChanged )
                {
                if( vScrollBar == event.message.infoPtr )
                    {
                    focusItemNum( vScrollBar->value );
                    drawView();
                    }
                else if( hScrollBar == event.message.infoPtr )
                    drawView();
                }
            }
        }
}


void MultipleSelectListBox::draw()
{
 short i, j, item;
 ushort color;
 short colWidth, curCol, indent;
 TDrawBuffer b;
 uchar scOff;

 ushort unsel_unfocus_color = getColor(1);
 ushort unsel_focus_color   = getColor(7);
 ushort sel_unfocus_color   = getColor(6);
 ushort sel_focus_color     = getColor(3);


    if( hScrollBar != 0 )
        indent = hScrollBar->value;
    else
        indent = 0;

    colWidth = size.x / numCols + 1;
    for( i = 0; i < size.y; i++ )
        {
        for( j = 0; j < numCols; j++ )
            {
            item =  j * size.y + i + topItem;
            curCol = j * colWidth;
            if( (state & (sfSelected | sfActive)) == (sfSelected | sfActive) &&
                focused == item &&
                range > 0)
                {
                if ( isSelected(item) )
                  {
                   color = sel_focus_color;
                  }
                else
                  {
                   color = unsel_focus_color;
                  }

                setCursor( curCol + 1, i );
                scOff = 0;
                }
            else if( item < range && isSelected(item) )
                {
                color = sel_unfocus_color;
                scOff = 2;
                }
            else
                {
                color = unsel_unfocus_color;
                scOff = 4;
                }

            b.moveChar( curCol, ' ', color, colWidth );
            if( item < range )
                {
                char text[256];
                getText( text, item, colWidth + indent );
                char buf[256];
                memmove( buf, text+indent, colWidth );
                buf[colWidth] = EOS;
                b.moveStr( curCol+1, buf, color );
                if( showMarkers )
                    {
                    b.putChar( curCol, specialChars[scOff] );
                    b.putChar( curCol+colWidth-2, specialChars[scOff+1] );
                    }
                }
            else if( i == 0 && j == 0 )
                b.moveStr( curCol+1, "<empty>", getColor(1) );

            b.moveChar( curCol+colWidth-1, 179, getColor(5), 1 );
            }
        writeLine( 0, i, size.x, 1, b );
        }
}


