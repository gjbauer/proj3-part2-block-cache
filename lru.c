#include <stdlib.h>
#include "lru.h"

LRU_List *lru_push(LRU_List *list, int index)
{
	LRU_List *node = (LRU_List*)malloc(sizeof(LRU_List));
	node->index = index;
	
	if (list)
	{
		node->next = list;
		node->prev = list->prev;
		list->prev = node;
	}
	
	return node;
}

int lru_pop(LRU_List *list)
{
	int index = (list->prev) ? (list->prev->index) : (index = list->index);
	
	if (list->prev)
	{	
		LRU_List *temp = list->prev;
	
		list->prev = list->prev->prev;
	
		free(temp);
	}
	else free(list);
	
	return index;
}

