#ifndef GPIB_H
#define GPIB_H

class GPIB
{
public:
          GPIB     ( const char *udname = 0 );
   void   write    ( const char *command );
   char  *read     ( int maxN=80);
   char  *slowRead ( int manN=80);
   double readNum  ( const char *command=0 );
   void   clear    ();
   void   local    ();

 protected:
   int  ud;
   void trapError(const char *);

};


#endif