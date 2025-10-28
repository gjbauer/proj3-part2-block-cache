#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "pci.h"

// Look up a block number in the primary cache index (PCI)
// Returns the cache entry array index if found, -1 if not found
// PCI is used to quickly find the position of a block in the cache entries array
int pci_lookup(PCI_HM *hashmap, uint64_t block_number)
{
	PCI_LL *current;
	int hm_index = block_number % HASHMAP_SIZE;
	current = hashmap->HashMap[hm_index];
	while (current!=NULL)
	{
		if (current->block_number==block_number)
		{
			printf("Cache hit!\n");
			return current->index;
		} else current = current->next;
	}
	
	printf("Cache miss!\n");
	return -1;
}

// Insert a block number and its cache entry index into the primary cache index (PCI)
// This allows quick lookup of where a block is stored in the cache entries array
void pci_insert(PCI_HM *hashmap, uint64_t block_number, uint64_t index)
{
	PCI_LL *node = malloc(sizeof(PCI_LL));
	node->block_number = block_number;
	node->index = index;
	
	node->next = hashmap->HashMap[block_number % HASHMAP_SIZE];
	
	hashmap->HashMap[block_number % HASHMAP_SIZE] = node;
}

// Remove a block number from the primary cache index (PCI)
// Called when a block is evicted from the cache
void pci_delete(PCI_HM *hashmap, uint64_t block_number)
{
	PCI_LL *curr = hashmap->HashMap[block_number % HASHMAP_SIZE];
	PCI_LL *prev;
	
	while (curr!=NULL)
	{
		if (curr->block_number==block_number)
		{
			break;
		} else {
			prev = curr;
			curr = curr->next;
		}
	}
	
	printf("Removing key %lu from primary cache index!\n", block_number);
	if (prev) {
		prev->next = curr->next;
	} else {
		hashmap->HashMap[block_number % HASHMAP_SIZE] = curr->next;
	}
	free(curr);
}
