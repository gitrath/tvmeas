#include <assert.h>
#include <string.h>
#include "patwind.h"
#include "pwinlist.h"

int PatternWindowList::compare(void *key1, void *key2)
{
  PatternWindow *win1 = (PatternWindow *) key1;
  PatternWindow *win2 = (PatternWindow *) key2;

  return strcmp( win1->getLabel( AntennaPattern::FILE_NAME),
                 win2->getLabel( AntennaPattern::FILE_NAME));

}
