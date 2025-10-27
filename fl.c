#include <stdlib.h>
#include "fl.h"

FL_LL *fl_push(FL_LL *list, int index)
{
	FL_LL *node = (FL_LL*)malloc(sizeof(FL_LL));
	node->index = index;
	
	node->next = list;
	
	return node;
}

FL_LL *fl_pop(FL_LL *list)
{
	FL_LL *temp = list->next;
	
	arc4random_buf(list, sizeof(struct FL_LL));
	free(list);
	
	return temp;
}

