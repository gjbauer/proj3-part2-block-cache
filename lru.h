#ifndef LRU_H
#define LRU_H

typedef struct LRU_List
{
	int index;
	struct LRU_List *next;
	struct LRU_List *prev;
} LRU_List;

/* In this case, we push to the head of the list and pop from the tail.
 * In the other case we can push and pop from the head. */
LRU_List *lru_push(LRU_List *list, int index);
int lru_pop(LRU_List *list);

#endif
