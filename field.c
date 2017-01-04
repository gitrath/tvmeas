#include <assert.h>
#include <iostream.h>
#include "Field.h"


static double sqr( double x)
{
return x*x;
}


Field::Field( complex theta_comp, complex phi_comp)
{
  theta_component = theta_comp;
  phi_component = phi_comp;
}


static double calc_tmax( double E1, double E2, double d )
{
  
  if ((E1*E1+E2*E2*cos(2*d)) == 0) E1*=1.0001; // bad hack!!
  
  double t1 = .5*atan( -E2*E2 * sin(2*d) / (E1*E1+E2*E2*cos(2*d)) );
  double t2 = t1 + M_PI/2.0;

  if ( (sqr(E1*cos(t1)) + sqr(E2*cos(t1+d))) >
       (sqr(E1*cos(t2)) + sqr(E2*cos(t2+d))) )
    {
      return t1;
    }
  else
    {
      return t2;
    }
}


double tau( Field f )
{
  double E1 = abs( f.theta() );
  double E2 = abs( f.phi()   );
  
  assert( abs( f.theta() ) != 0 );
  double d  = arg( f.phi() / f.theta() );
  
  double tmax = calc_tmax( E1,E2,d );

  assert( E1 != 0);
  assert( cos(tmax) != 0);
  
  return atan( E2*cos(tmax+d)/E1/cos(tmax) );
}

double epsilon( Field f)
{
  assert( axialRatio(f) != 0);
  return atan( 1.0/axialRatio(f) );
}

double axialRatio( Field f )
{
  double E1 = abs( f.theta() );
  double E2 = abs( f.phi()   );

  assert( abs( f.theta() ) != 0 );
  double d  = arg( f.phi() / f.theta() );
  
  double tmax = calc_tmax( E1,E2,d );
  double tmin = tmax + M_PI/2.0;

  assert( ( sqr(E1*cos(tmin))+sqr(E2*cos(tmin+d)))  != 0);
  return sqrt( ( sqr(E1*cos(tmax))+sqr(E2*cos(tmax+d)))/
	       ( sqr(E1*cos(tmin))+sqr(E2*cos(tmin+d))) );
}


double maxField( Field f )
{
  double E1 = abs( f.theta() );
  double E2 = abs( f.phi()   );

  assert( abs( f.theta() ) != 0 );
  double d  = arg( f.phi() / f.theta() );

//  cout << "                  <" << E1 << " " << E2 << " " << d << endl;

  double tmax = calc_tmax( E1,E2,d );
  
  return sqrt( ( sqr(E1*cos(tmax))+sqr(E2*cos(tmax+d))) );
}

double minField( Field f )
{
  double E1 = abs( f.theta() );
  double E2 = abs( f.phi()   );

  assert( abs( f.theta() ) != 0 );
  double d  = arg( f.phi() / f.theta() );
  
  double tmax = calc_tmax( E1,E2,d );
  double tmin = tmax + M_PI/2.0;

  return sqrt( ( sqr(E1*cos(tmin))+sqr(E2*cos(tmin+d))) );
}




  
     
  
  
