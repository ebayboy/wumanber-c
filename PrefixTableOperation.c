#include <stdlib.h>
#include <string.h>
#include "PrefixTableOperation.h"

static PrefixTableOperation prefixTableOperation = NULL;

// key: char 类型, 只支持英文, 如果是中文字符会溢出
void push(PrefixTable prefixTable, char subString[2], char* pattern)
{
    int key = subString[0] * 256 + subString[1];

	if (subString[0] < 0 || subString[0] > 255) {
		return;
	}

    PatternNode node = (PatternNode)malloc(sizeof(PatternNodeStruct));
	if (node == NULL) {
		return ;
	}
    node->pattern = pattern;
    node->length = strlen(pattern);
    node->next = NULL;
    // new prefix
    if(!prefixTable[key])
    {
        prefixTable[key] = node;
    }
    // have the same prefix
    else
    {
        PatternNode current = prefixTable[key];
        while(current->next)
        {
            current = current->next;
        }
        current->next = node;
    }
}

PatternNode pop(PrefixTable prefixTable, char subString[2])
{
    int key = subString[0] * 256 + subString[1];
	if (subString[0] < 0 || subString[0] > 255) {
		return NULL;
	}

    return prefixTable[key];
}

PrefixTableOperation PrefixTableOperationFactory()
{
    if(!prefixTableOperation)
    {
        prefixTableOperation = (PrefixTableOperation)malloc(sizeof(PrefixTableOperationStruct));
		if (prefixTableOperation == NULL) {
			return NULL;
		}
        prefixTableOperation->push = push;
        prefixTableOperation->pop = pop;
    }
    return prefixTableOperation;
}

__attribute__((destructor))
void PrefixTableOperationDestructor()
{
	if (prefixTableOperation) {
		free(prefixTableOperation);
		prefixTableOperation = NULL;
	}
}
