#include <stdlib.h>
#include "fl.h"

// Push a new index onto the free list
// Returns the new head of the list
FL_LL *fl_push(FL_LL *list, int index)
{
	FL_LL *node = (FL_LL*)malloc(sizeof(FL_LL));
	node->index = index;
	
	node->next = list;
	
	return node;
}

// Remove and return the head of the free list
// Securely wipes the removed node before freeing
FL_LL *fl_pop(FL_LL *list)
{
	FL_LL *temp = list->next;
	
	arc4random_buf(list, sizeof(struct FL_LL));
	free(list);
	
	return temp;
}

