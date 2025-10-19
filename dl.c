#include <stdlib.h>
#include <stdio.h>
#include "dl.h"

DL_LL *dl_push(DL_LL *list, int block_number)
{
	DL_LL *node = (DL_LL*)malloc(sizeof(DL_LL));
	node->block_number = block_number;
	
	if (list) node->next = list;
	
	return node;
}

DL_LL *dl_pop(DL_LL *list)
{
	DL_LL *temp = list->next;
	
	free(list);
	
	return temp;
}

DL_LL *dl_lookup(DL_HM *hashmap, int key)
{
	DL_HM_LL *current;
	int hm_index = key % CACHE_SIZE;
	current = &hashmap->HashMap[hm_index];
	while (current!=NULL)
	{
		if (current->key==key)
		{
			return current->list;
		} else current = current->next;
	}
	
	return NULL;
}

void dl_insert(DL_HM *hashmap, int key, int block_number)
{
	DL_HM_LL *node = malloc(sizeof(DL_HM_LL));
	node->key = key;
	dl_push(node->list, block_number);
	// TODO: Check if key exists in HashMap first!!
	
	node->next = &hashmap->HashMap[key % CACHE_SIZE];
	
	hashmap->HashMap[key % CACHE_SIZE] = *node;
}

void dl_delete(DL_HM *hashmap, int key)
{
	DL_HM_LL *curr = &hashmap->HashMap[key % CACHE_SIZE];
	DL_HM_LL *prev;
	
	while (curr!=NULL)
	{
		if (curr->key==key)
		{
			break;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}
	
	printf("Removing key %d from dirty list!\n", key);
	if (prev) {
		prev->next = curr->next;
	} else {
		hashmap->HashMap[key % CACHE_SIZE] = *curr->next;
	}
	free(curr);
}
