#ifndef list_h
#define list_h

#include <assert.h>
#include "vectbase.h"

template <class Type>
class List : VectorBase<Type>
{
public:
  //***Constructor***
  List(int sizeEstimate=10);

  void      addItem( const Type &item );

  Type&     operator[](int n);

  int size () const;

  void      clear();

private:
  int currentSize;
};


template <class Type>
List<Type>::List( int sizeEstimate ) : VectorBase<Type>( sizeEstimate )
{
 clear();
}


template <class Type>
void List<Type>::addItem( const Type &item )
{
  force( currentSize++, item );
}


template <class Type>
Type& List<Type>::operator[]( int n )
{
  assert( n >= 0 );
  assert( n < currentSize );

  return VectorBase<Type>::operator[]( n );
}

template <class Type>
int List<Type>::size() const
{
  return currentSize;
}

template <class Type>
void List<Type>::clear()
{
  currentSize = 0;
}


#endif