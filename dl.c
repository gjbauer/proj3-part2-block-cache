#include <stdlib.h>
#include <stdio.h>
#include <bsd/stdlib.h>
#include "dl.h"

DL_LL *dl_push(DL_LL *list, uint64_t block_number)
{
	DL_LL *node = (DL_LL*)malloc(sizeof(DL_LL));
	node->block_number = block_number;
	
	if (list) node->next = list;
	
	return node;
}

DL_LL *dl_pop(DL_LL *list)
{
	DL_LL *temp = list->next;
	
	arc4random_buf(list, sizeof(struct DL_LL));
	
	free(list);
	
	return temp;
}

DL_HM_LL *dl_lookup(DL_HM *hashmap, uint64_t inode_number)
{
	DL_HM_LL *current;
	int hm_index = inode_number % HASHMAP_SIZE;
	current = hashmap->HashMap[hm_index];
	while (current!=NULL)
	{
		if (current->inode_number==inode_number)
		{
			return current;
		} else current = current->next;
	}
	
	return NULL;
}

DL_LL *dl_find_block(DL_LL *list, uint64_t block_number)
{
	DL_LL *curr = list;
	while (curr!=NULL)
	{
		if (curr->block_number == block_number) return curr;
		curr=curr->next;
	}
	
	return NULL;
}

void dl_insert(DL_HM *hashmap, uint64_t inode_number, uint64_t block_number)
{
	DL_HM_LL *node = dl_lookup(hashmap, inode_number);
	if (node==NULL) {
		node = malloc(sizeof(DL_HM_LL));
		node->inode_number = inode_number;
	
		node->next = hashmap->HashMap[inode_number % HASHMAP_SIZE];
	
		hashmap->HashMap[inode_number % HASHMAP_SIZE] = node;
	}
	DL_LL *entry = dl_find_block(node->list, block_number);
	if (entry==NULL) dl_push(node->list, block_number);
}

void dl_delete(DL_HM *hashmap, uint64_t inode_number)
{
	DL_HM_LL *curr = hashmap->HashMap[inode_number % HASHMAP_SIZE];
	DL_HM_LL *prev;
	
	while (curr!=NULL)
	{
		if (curr->inode_number==inode_number)
		{
			break;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}
	
	printf("Removing inode %lu from dirty list!\n", inode_number);
	if (prev) {
		prev->next = curr->next;
	} else {
		hashmap->HashMap[inode_number % HASHMAP_SIZE] = curr->next;
	}
	free(curr);
}

void dl_remove_block(DL_HM *hashmap, uint64_t inode_number, uint64_t block_number)
{
	DL_HM_LL *list = dl_lookup(hashmap, inode_number);
	if (list)
	{
		DL_LL *curr = list->list;
		DL_LL *prev=NULL;
		while (curr!=NULL)
		{
			if (curr->block_number == block_number) break;
			prev=curr;
			curr=curr->next;
		}
		if (prev)
		{
			prev->next = curr->next;
		}
		free(curr);
		if (!list->list)  dl_delete(hashmap, inode_number);
	}
}
