#include <stdlib.h>
#include "ShiftTableOperation.h"

static ShiftTableOperation shiftTableOperation = NULL;

void put(ShiftTable shiftTable, char subString[2], int shift)
{

    int key = subString[0] * 256 + subString[1];

    if (subString[0] < 0 || subString[0] > 255)
    {
        return;
    }

    // new subString
    if(shiftTable[key] == -1)
    {
        shiftTable[key] = shift;
    }
    // already have the same subString
    // select min
    else
    {
        if(shiftTable[key] > shift)
        {
            shiftTable[key] = shift;
        }
    }
}

int get(ShiftTable shiftTable, char subString[2])
{
    int key = subString[0] * 256 + subString[1];
    if (subString[0] < 0 || subString[0] > 255)
    {
        return -1;
    }
    return shiftTable[key];
}

ShiftTableOperation ShiftTableOperationFactory()
{
    if(!shiftTableOperation)
    {
        shiftTableOperation = (ShiftTableOperation)malloc(sizeof(ShiftTableOperationStruct));
        if (shiftTableOperation == NULL)
        {
            return NULL;
        }
        shiftTableOperation->put = put;
        shiftTableOperation->get = get;
    }
    return shiftTableOperation;
}

__attribute__((destructor))
void ShiftTableOperationDestructor()
{
	if (shiftTableOperation) {
		free(shiftTableOperation);
		shiftTableOperation = NULL;
	}
}
