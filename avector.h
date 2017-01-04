#ifndef AVector_h
#define AVector_h

#include <assert.h>
#include "Vector.h"

template <class Type>
class AVector : public Vector<Type>
{
public:
  //***Constructors***
  AVector()                      : Vector<Type>()  {};
  AVector(int l)                 : Vector<Type>(l) {};
  AVector(int l, Type fillValue) : Vector<Type>(l,fillValue) {};
  AVector( AVector<Type> &vec )  : Vector<Type>( vec ) {};

  //***Vector by scalar -> vector operations***

  AVector&  operator += (Type  b);
  AVector&  operator -= (Type  b);
  AVector&  operator *= (Type  b);
  AVector&  operator /= (Type  b);

  //***Vector by vector -> vector operations***
  AVector&  operator += (AVector &vec);
  AVector&  operator -= (AVector &vec);
  AVector&  operator *= (AVector &vec);
  AVector&  operator /= (AVector &vec);

  //***Map function***

  //***Min & Max***
  Type maxValue(int *max_index = 0) const;
  Type minValue(int *min_index = 0) const;

};


template <class Type>
AVector<Type>& AVector<Type>::operator+=(Type b)
{
  for(int i=0; i<size(); i++) s[i] += b;
  return *this;
}

template <class Type>
AVector<Type>& AVector<Type>::operator-=(Type b)
{
  for(int i=0; i<size(); i++) s[i] -= b;
  return *this;
}

template <class Type>
AVector<Type>& AVector<Type>::operator*=(Type b)
{
  for(int i=0; i<size(); i++) s[i] *= b;
  return *this;
}

template <class Type>
AVector<Type>& AVector<Type>::operator/=(Type b)
{
  assert( b != 0);
  for(int i=0; i<size(); i++) s[i] /= b;
  return *this;
}

template <class Type>
AVector<Type>& AVector<Type>::operator+=(AVector<Type> &vec)
{
  assert( size() == vec.size() );
  for(int i=0; i<size(); i++) s[i] += vec.s[i] ;
  return *this;
}

template <class Type>
AVector<Type>& AVector<Type>::operator-=(AVector<Type> &vec)
{
  assert( size() == vec.size() );
  for(int i=0; i<size(); i++) s[i] -= vec.s[i] ;
  return *this;
}

template <class Type>
AVector<Type>& AVector<Type>::operator*=(AVector<Type> &vec)
{
  assert( size() == vec.size() );
  for(int i=0; i<size(); i++) s[i] *= vec.s[i] ;
  return *this;
}

template <class Type>
AVector<Type>& AVector<Type>::operator/=(AVector<Type> &vec)
{
  assert( size() == vec.size() );
  for(int i=0; i<size(); i++)
    {
     assert( vec.s[i] != 0);
     s[i] /= vec.s[i] ;
    }
  return *this;
}

template <class Type>
Type AVector<Type>::maxValue(int *max_index) const
{
  assert( size() > 0 );
  int    index = 0;
  double max   = s[0];

  for(int i=1; i<size(); i++)
    {
     if ( s[i] > max)
       {
        max = s[i];
        index = i;
       }
    }

  if (max_index) *max_index = index;
  return max;
}

template <class Type>
Type AVector<Type>::minValue(int *min_index) const
{
  assert( size() > 0 );
  int    index = 0;
  double min   = s[0];

  for(int i=1; i<size(); i++)
    {
     if ( s[i] < min)
       {
        min = s[i];
        index = i;
       }
    }

  if (min_index) *min_index = index;
  return min;
}

#endif
