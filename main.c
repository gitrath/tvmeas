/***********************************************************************
   TVMEAS
     By Darrin D. Rath
     University of Minnesota 1993

     This code provides a windowed user interface to control instruments
     used to take antenna pattern measurements.  The software interfaces
     to a HP8562A specturm analyzer and a HP8350B sweep oscillator over
     a GPIB bus, and to a PC23 card to rotate a turntable.

     Copyright 1993 Univ. of Minnesota, Electrical Engineering Dept.

************************************************************************/

#include <iostream.h>
#include "tvmeas.h"

void main()
{
  MyApplication TVMeas;

  TVMeas.run();

  cout << "done" << endl;

}








