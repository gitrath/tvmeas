#ifndef vectbase_h
#define vectbase_h

#include <assert.h>
#include <fstream.h>


template <class Type>
class VectorBase
{
public:
  //***Constructors***
  VectorBase(int l=0);
  VectorBase(int l, Type fillValue);
  VectorBase( const VectorBase &vec );

  //***Destructor***
  ~VectorBase();

  //***Operators***
  Type&       operator[](int n);
  const Type& operator[](int n) const;
  VectorBase& operator=( const VectorBase &vec );


  //***VectorBase size functions***
  int size () const;
  void resize ( int  newLength );
  void setSize( int  newLength );

  void fill   ( Type fillValue );

  void force  ( int i, const Type &x);

protected:
  Type *s;
  void init( int l);

private:
  int len;
  void init( const Type *, int l);
};

template <class Type>
VectorBase<Type>::VectorBase(int l)
{
  init(l);
}

template <class Type>
VectorBase<Type>::VectorBase(int l, Type fill_value)
{
  init(l);
  fill( fill_value );
}


template <class Type>
VectorBase<Type>::VectorBase( const VectorBase<Type> &vec )
{
  init( vec.size() );
  for( int i=0; i<size(); i++)
    s[i] = vec.s[i];
}



template <class Type>
VectorBase<Type>::~VectorBase()
{
  assert( s != (Type *)-1 );
  //cout << "destructing a vector of length = " << size() << endl;
  delete [] s;
  //s = (Type *) -1;  // NOT necessary, but useful for error checking
}

template <class Type>
void VectorBase<Type>::init(const Type *ptr, int l)
{
  init(l);
  for( int i=0; i< size(); i++)
    s[i] = ptr[i];
}

template <class Type>
inline int VectorBase<Type>::size() const
{
  return len;
}

template <class Type>
inline Type& VectorBase<Type>::operator[](int n)
{
  assert(n>=0);
  assert(n<len);
  assert(s);
  return s[n];
}

template <class Type>
inline const Type& VectorBase<Type>::operator[](int n) const
{
  assert(n>=0);
  assert(n<len);
  assert(s);
  return s[n];
}


template <class Type>
VectorBase<Type>& VectorBase<Type>::operator=( const VectorBase<Type> &vec )
{
 if (this == &vec) return *this;
 setSize( vec.size() );
 for( int i=0; i< size(); i++)
   {
    s[i] = vec.s[i];
   }
 return *this;
}



template <class Type>
void VectorBase<Type>::resize(int newLength)
{
  assert(newLength >= 0);
  if (newLength == size() ) return;

  Type* new_s = new Type [newLength];
  assert(new_s);

  int minl = (size() < newLength) ? size() : newLength;

  for(int i=0;i<minl;i++)  new_s[i] = s[i];

  assert( s != (Type *)-1 );
  delete [] s;
  s   = new_s;
  len = newLength;
}

template <class Type>
void VectorBase<Type>::setSize(int newLength)
{
  assert(newLength >= 0);
  if( newLength != size() )
    {
     assert( s != (Type *)-1 );
     delete [] s;
     init( newLength );
    }
}

template <class Type>
void VectorBase<Type>::fill( Type fill_value )
{
  for(int i=0; i< size(); i++)
    s[i] = fill_value;
}

template <class Type>
void VectorBase<Type>::init(int l)
{
  assert(l>=0);
  if( l == 0 )
    {
     s   = 0;
     len = 0;
    }
  else
    {
     s = new Type[ len = l ];
     assert(s);
    }
}


template <class Type>
void VectorBase<Type>::force( int i, const Type &x )
{
  if (i < size())
    {
     s[i] = x;
    }
  else
    {
     int sizeGuess = size()*2;
     if (sizeGuess==0) sizeGuess=256;  // should probably be a constant
     if (sizeGuess < i ) sizeGuess=i*2;
     resize(sizeGuess);
     assert(i < size());
     s[i] = x;
    }
}



#endif






