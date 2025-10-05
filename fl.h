#ifndef FL_H
#define FL_H

typedef struct FL_LL
{
	int index;
	struct FL_LL *next;
} FL_LL;

void fl_push(FL_LL *list, int index);
int fl_pop(FL_LL);

#endif
