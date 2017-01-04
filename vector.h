#ifndef Vector_h
#define Vector_h

#include <assert.h>
#include <fstream.h>
#include "vectbase.h"

template <class Type>
class Vector : public VectorBase<Type>
{
public:
  //***Constructors***
  Vector(int l=0)                 : VectorBase<Type>(l) {};
  Vector(int l, Type fillValue)   : VectorBase<Type>(l,fillValue) {};
  Vector( const Vector &vec )     : VectorBase<Type>(vec) {};
  Vector( const Type *ptr); // zero terminated array

  Vector& operator=( const Vector &vec );
  Vector& operator=( const Type *ptr   ); // ptr is a zero terminated array

  operator const Type* () const { return s; };

  operator== ( const Vector &vec ) const;

  operator== ( const Type *ptr   ) const; // ptr is a zero terminated array
  operator!= ( const Type *ptr   ) const;

  //*** IO ***
  void write( ostream &stream = cout) const;
  void write( const char *fileName) const;
  void read ( istream &stream);
  void read ( const char *fileName);

};


template <class Type>
void Vector<Type>::Vector( const Type *ptr) : VectorBase<Type>()
{
 for(int l=0; ptr[l] != 0 ;l++);

 l++; // add one to include 0 at end of array

 init( l );
 for( int i=0; i< size(); i++)
   {
    s[i] = ptr[i];
   }
}


template <class Type>
Vector<Type>& Vector<Type>::operator=( const Vector<Type> &vec )
{
 VectorBase<Type>::operator=( vec );

 return *this;
// if (this == &vec) return *this;
// setSize( vec.size() );
// for( int i=0; i< size(); i++)
//   {
//    s[i] = vec.s[i];
//   }
// return *this;
}



template <class Type>
Vector<Type>& Vector<Type>::operator=( const Type *ptr )   // useful for strings ( Vector<char> )
{
 for(int l=0; ptr[l] != 0 ;l++);

 l++; // add one to include 0 at end of array

 setSize( l );
 for( int i=0; i< size(); i++)
   {
    s[i] = ptr[i];
   }

 return *this;
}

template <class Type>
Vector<Type>::operator== ( const Vector<Type> &vec ) const
{
  if ( size() != vec.size() )
    return 0;
  for( int i=0; i<size(); i++)
    {
     if ( s[i] != vec.s[i] )
       return 0;
    }
  return 1;
}

template <class Type>
Vector<Type>::operator== ( const Type *ptr   ) const // good for string comparisons when Type=char
{
  assert( ptr );
  assert( s[ size()-1 ] == 0 );  // vector itself must be zero terminated
  for( int i=0; i< size()-1; i++)
    {
     if( ptr[i] == 0 ) return 0;
     if( s[i] != ptr[i] ) return 0;
    }

  if ( ptr[ size()-1 ] != 0 ) return 0;

  return 1;
}

template <class Type>
Vector<Type>::operator!= ( const Type *ptr   ) const
{
  return !operator==( ptr);
}


template <class Type>
void Vector<Type>::write( ostream &stream) const
{
  assert( stream.good() );

  for(int i=0;i<size();i++)
    {
      stream << s[i] << endl;
    }

  assert(stream.good());
}

template <class Type>
void Vector<Type>::write( const char *fileName) const
{
  assert( fileName);

  ofstream stream( fileName, ios::out);
  assert( stream.good());

  write( stream );
}

template <class Type>
void Vector<Type>::read( istream &stream)
{
  assert( stream.good() );
  Type x;

  for (int i=0; stream >> x; i++)
    {
      force(i,x);
    }

  assert( stream.eof() );

  resize(i);
}

template <class Type>
void Vector<Type>::read( const char *fileName)
{
  assert(fileName);

  ifstream stream( fileName, ios::in);
  assert( stream.good());

  read( stream );
}

typedef Vector<char> VectorString;

#endif






