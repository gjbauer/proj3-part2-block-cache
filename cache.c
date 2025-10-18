#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "pci.h"
#include "fl.h"
#include "lru.h"
#include "cache.h"

void*
get_block(DiskInterface* disk, PCI_HM *hashmap, cache_entry_t *cache, FL_LL *free_list, LRU_List *lru, int pnum)
{
	int rv = pci_lookup(hashmap, pnum);
	if (rv==-1) {
		if (free_list==NULL) {
			// TODO: Evict from cache
		}
		int index = free_list->index;
		free_list = fl_pop(free_list);
		cache[index].dirty_bit = false;
		cache[index].pin_count = 0;
		cache[index].page_data = malloc(BLOCK_SIZE);
		printf("Copying page %d into the cache!\n", pnum);
		memcpy(cache[index].page_data, disk_get_block(disk, pnum), BLOCK_SIZE);
		cache[index].lru_pos = lru_push(lru, index);
		lru = cache[index].lru_pos;
	} else {
		// TODO: Update LRU list to place page at the front!!
		return cache[rv].page_data;
	}
}
