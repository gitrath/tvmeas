#include "field.h"
#include "lin3circ.h"


void ThreeLinearToCircular( const AntennaPattern &pat_h,
                            const AntennaPattern &pat_d,
                            const AntennaPattern &pat_v,
                            AntennaPattern *pat_max, AntennaPattern *pat_min )
{
  assert( pat_h.size() == pat_d.size() );
  assert( pat_h.size() == pat_v.size() );

  pat_max->amplVec.setSize ( pat_h.size() );
  pat_max->angleVec.setSize( pat_h.size() );
  pat_min->amplVec.setSize ( pat_h.size() );
  pat_min->angleVec.setSize( pat_h.size() );

  for(int i=0;i<pat_h.size();i++)
    {
      double angle = pat_h.angleVec[i];


      assert( angle - pat_d.angleVec[i] < .0001 ); // another bad hack!
      assert( angle - pat_v.angleVec[i] < .0001 );

      double Eh = pat_h.amplVec[i];
      double Ed = pat_d.amplVec[i];
      double Ev = pat_v.amplVec[i];

      Eh = pow( 10., Eh/20);
      Ed = pow( 10., Ed/20);
      Ev = pow( 10., Ev/20);

      double E1 = Eh;
      double E2 = Ev;
      double d;

      if ( 2*Ed*Ed - Eh*Eh - Ev*Ev > 2*Eh*Ev )
	{
         // cout << "2*Ed*Ed - Eh*Eh - Ev*Ev > 2*Eh*Ev  @ angle=" << angle << endl;
	  d = 0;
	}
      else if ( -2*Ed*Ed + Eh*Eh + Ev*Ev > 2*Eh*Ev )
	{
         // cout << "-2*Ed*Ed + Eh*Eh + Ev*Ev > 2*Eh*Ev @ angle=" << angle << endl;
	  d = M_PI;

        }
      else
        {
         d = acos( (2*Ed*Ed - Eh*Eh - Ev*Ev)/(2*Eh*Ev) );

        }

      Field f = Field(E1,complex(E2*cos(d),E2*sin(d)));


      double max = maxField(f);
      double min = minField(f);


      assert( max >= min );


      //if ( max < Eh) cout << "max < Eh when angle=" << angle << endl;
      //if ( max < Ed) cout << "max < Ed when angle=" << angle << endl;
      //if ( max < Ev) cout << "max < Ev when angle=" << angle << endl;

      //if ( min > Eh) cout << "min > Eh when angle=" << angle << endl;
      //if ( min > Ed) cout << "min > Ed when angle=" << angle << endl;
      //if ( min > Ev) cout << "min > Ev when angle=" << angle << endl;



      /*  assert( maxField(f) >= Eh );
      assert( maxField(f) >= Ed );
      assert( maxField(f) >= Ev );
      assert( minField(f) <= Eh );
      assert( minField(f) <= Ed );
      assert( minField(f) <= Ev ); */

      pat_max->angleVec[i] = angle;
      pat_min->angleVec[i] = angle;

      pat_max->amplVec[i] = 20*log10(max);
      pat_min->amplVec[i] = 20*log10(min);

    }

}
