#ifndef lin3circ_h
#define lin3circ_h

#include "antpat.h"

void ThreeLinearToCircular( const AntennaPattern &pat_h,
                            const AntennaPattern &pat_d,
                            const AntennaPattern &pat_v,
                            AntennaPattern *pat_max, AntennaPattern *pat_min );




#endif