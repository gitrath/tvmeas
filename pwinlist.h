#ifndef pwinlist_h
#define pwinlist_h

#define Uses_TSortedCollection
#define Uses_TStringCollection
#include <tv.h>

class PatternWindowList : public TStringCollection
{
public:
  enum { PAT_LIST_LIMIT = 100};
  enum { PAT_LIST_DELTA_GROW = 10};

  PatternWindowList() :
    TStringCollection( PAT_LIST_LIMIT, PAT_LIST_DELTA_GROW) {};

  virtual int compare(void *key1, void *key2);

};

#endif
