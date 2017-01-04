#ifndef field_H
#define field_H

#include <math.h>
#include <complex.h>

class Field
{
public:
  Field( complex theta_comp, complex phi_comp);
  
  complex theta(void)  { return theta_component; }
  complex phi(void)    { return phi_component; }

  void operator+=( Field field )
    {
      theta_component += field.theta();
      phi_component += field.phi();
    }

//  Field operator*( complex z )
//    {
//      return Field( theta_component*z, phi_component*z );
//    }

//  Field operator/(complex z )
//    {
//      return Field( theta_component/z, phi_component/z );
//    }
  
protected:
  complex theta_component;
  complex phi_component;
};

inline complex polar_deg( double r, double ang )
{
  return complex( r*cos(ang*M_PI/180), r*sin(ang*M_PI/180) );
}

double tau        ( Field f );  // Calculates tilt angle of polarization;
double epsilon    ( Field f );  // Calculates circularity of polarization;
double axialRatio ( Field f );  // Calculates axial ratio of polarization;

double maxField( Field f );  // Calcs max field received by a linear antenna
double minField( Field f );  // Calcs min field received by a linear antenna

#endif











