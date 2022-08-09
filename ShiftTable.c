#include <stdlib.h>
#include "ShiftTable.h"

ShiftTable ShiftTableFactory() {

  int length = 65536;
  ShiftTable table = (ShiftTable)malloc(sizeof(int) * length);
  if (table == NULL) {
	  return NULL;
  }

  for(int i=0;i<length;i++) {
    table[i] = -1;
  }

  return table;
}
