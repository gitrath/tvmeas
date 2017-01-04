/*-------------------------------------------------------------*/
/*                                                             */
/*   Turbo Vision 1.0                                          */
/*   Turbo Vision Demo                                         */
/*   Copyright (c) 1991 by Borland International               */
/*                                                             */
/*   BGI support file for use with Turbo Vision programs. See  */
/*   TVBGI.CPP for an example of how to use this file.         */
/*-------------------------------------------------------------*/

#if !defined( __GRAPHAPP_H )
#define __GRAPHAPP_H

#include <tv.h>

#if !defined( __GRAPHICS_H )
#include <graphics.h>
#endif  // __GRAPHICS_H


Boolean graphAppInit( int, int, char *, Boolean);
void graphAppDone(void);
Boolean graphicsStart(void);
void graphicsStop(void);

#endif  // __GRAPHAPP_H
