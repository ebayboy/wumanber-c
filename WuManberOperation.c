#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "WuManberOperation.h"
#include "ShiftTable.h"
#include "ShiftTableOperation.h"
#include "PrefixTable.h"
#include "PrefixTableOperation.h"

static WuManberOperation wuManberOperation = NULL;

void buildShiftTable(WuManber wuManber, char** patterns, int count)
{

    ShiftTableOperation tableOperation = ShiftTableOperationFactory(); //指向static变量
	if (tableOperation == NULL) {
		return;
	}

    int minLength = INT_MAX;
    for(int i=0; i<count; i++)
    {
        int length = strlen(patterns[i]);
        if(minLength > length)
        {
            minLength = length;
        }
    }

    wuManber->minPatternLength = minLength;
    printf(" - Min pattern length: %d\n\n", minLength);

    ShiftTable table = ShiftTableFactory();

    for(int i=0; i<count; i++)
    {
        // block size = 2
        for(int j=0; j<minLength-1; j++)
        {

            int shift = minLength - (j + 1 + 1);

            char subString[2] = {patterns[i][j], patterns[i][j + 1]};

            tableOperation->put(table, subString, shift);
        }
    }

    wuManber->shiftTable = table;
}

void buildPrefixTable(WuManber wuManber, char** patterns, int count)
{

    PrefixTableOperation tableOperation = PrefixTableOperationFactory(); //指向static变量
	if (tableOperation == NULL) {
		return;
	}

    PrefixTable table = PrefixTableFactory();

    // block size = 2
    // pattern length must >= 2
    for(int i=0; i<count; i++)
    {
        char prefix[2] = {patterns[i][0], patterns[i][1]};
        tableOperation->push(table, prefix, patterns[i]);
    }

    wuManber->prefixTable = table;
}

MatchingResult search(WuManber wuManber, char* content)
{

    ShiftTableOperation shiftTableOperation = ShiftTableOperationFactory();
    PrefixTableOperation prefixTableOperation = PrefixTableOperationFactory();

    MatchingResult result = (MatchingResult)malloc(sizeof(MatchingResultStruct));
    if (result == NULL)
    {
        return NULL;
    }

    result->next = NULL;
    MatchingResult head = result;

    // if subString not in shiftTable, skip this value
    int skip = wuManber->minPatternLength - 2 + 1;
    int start = wuManber->minPatternLength - 1;

    int contentLength = strlen(content);

    while(start < contentLength)
    {

        char subString[2] = {content[start - 1], content[start]};

        int shift = shiftTableOperation->get(wuManber->shiftTable, subString);

        // don't have this bad char
        if(shift == -1)
        {
            start += skip;
        }
        // match tail, lookup prefix table
        else if(shift == 0)
        {

            // get prefix index
            int prefixStart = start - (wuManber->minPatternLength - 1);
            // prefix
            char prefix[2] = {content[prefixStart], content[prefixStart + 1]};

            PatternNode node = prefixTableOperation->pop(wuManber->prefixTable, prefix);
            // if has this prefix, match the whole pattern
            while(node)
            {
                int flag = 0;
                // not need to match prefix, so i = 2
                for(int i=2; i<+node->length; i++)
                {
                    if(node->pattern[i] != content[prefixStart + i])
                    {
                        flag = 1;
                        break;
                    }
                }
                if(!flag)
                {

                    MatchingResult res = (MatchingResult)malloc(sizeof(MatchingResultStruct));
                    if (res == NULL)
                    {
                        return NULL;
                    }
#if 0
                    res->pattern = malloc(node->length);
                    if (res->pattern)
                    {
                        res->pattern = node->pattern;
                    }
#else
					res->pattern = node->pattern;
#endif
                    res->start = prefixStart + 1;
                    res->end = prefixStart + node->length - 1 + 1;
                    res->next = NULL;

                    result->next = res;
                    result = res;
                    //printf("match %s at %d ~ %d\n", node->pattern, prefixStart, prefixStart + node->length - 1);
                }
                node = node->next;
            }
            start++;
        }
        // bad char shift
        else
        {
            start += shift;
        }
    }

    return head;
}

void destroy(WuManber wuManber)
{
	if (wuManber == NULL) {
		return;
	}

    // free shift table
	if (wuManber->shiftTable) {
		free(wuManber->shiftTable);
		wuManber->shiftTable = NULL;
	}

    // free prefix table
    for(int i=0; i<65536; i++)
    {
        if(wuManber->prefixTable[i])
        {
            PatternNode current = wuManber->prefixTable[i];
            while(current)
            {
                PatternNode temp = current;
                current = current->next;
                free(temp);
				temp = NULL;
            }
        }
    }
	if (wuManber->prefixTable) {
		free(wuManber->prefixTable);
	}

    // free wumanber
	if (wuManber) { 
		free(wuManber);
		wuManber = NULL;
	}
}

WuManberOperation WuManberOperationFactory()
{
    if(!wuManberOperation)
    {
        wuManberOperation = (WuManberOperation)malloc(sizeof(WuManberOperationStruct));
        if (wuManberOperation == NULL)
        {
            return NULL;
        }
        wuManberOperation->buildShiftTable = buildShiftTable;
        wuManberOperation->buildPrefixTable = buildPrefixTable;
        wuManberOperation->search = search;
        wuManberOperation->destroy = destroy;
    }
    return wuManberOperation;
}

__attribute__((destructor))
void WuManberOperationDestructor()
{
	if (wuManberOperation) {
		free(wuManberOperation);
		wuManberOperation = NULL;
	}
}


