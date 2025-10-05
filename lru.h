#ifndef LRU_H
#define LRU_H

typedef struct LRU_List
{
	int index;
	struct LRU_List *next;
	struct LRU_List *prev;
} LRU_List;

void push(LRU_List *list, int index);
int pop(LRU_List *list);

#endif
