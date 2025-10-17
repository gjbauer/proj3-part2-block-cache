#include <stdio.h>
#include "pci.h"

int pci_lookup(PCI_HM *hashmap, int key)
{
	PCI_LL *current;
	int hm_index = key % CACHE_SIZE;
	current = &hashmap->HashMap[hm_index];
	while (current!=NULL)
	{
		if (current->key==key)
		{
			printf("Cache hit!\n");
			return current->index;
		} else current = current->next;
	}
	
	printf("Cache miss!\n");
	return -1;
}

void pci_insert(PCI_HM *hashmap, int key, int index)
{
	PCI_LL *node = malloc(sizeof(PCI_LL));
	node->key = key;
	node->index = index;
	
	node->next = hashmap->HashMap[key % CACHE_SIZE];
	
	hashmap->HashMap[key % CACHE_SIZE] = node;
}

void pci_delete(PCI_HM *hashmap, int key)
{
	PCI_LL *curr = hashmap->HashMap[key % CACHE_SIZE];
	PCI_LL *prev;
	
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
	
	printf("Removing key %d from primary cache index!\n", key);
	if (prev) {
		prev->next = curr->next;
	} else {
		hashmap->HashMap[key % CACHE_SIZE] = curr->next;
	}
	free(curr);
}
