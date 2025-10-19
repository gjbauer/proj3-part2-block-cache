#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "cache.h"

void*
get_block(DiskInterface* disk, cache *cache, int pnum)
{
	int rv = pci_lookup(cache->pci, pnum);
	if (rv==-1) {
		if (cache->free_list==NULL) {
			// TODO: Evict from cache
			//pci_delete(PCI_HM *hashmap, int key);
		}
		int index = cache->free_list->index;
		cache->free_list = fl_pop(cache->free_list);
		cache->cache[index].dirty_bit = false;
		cache->cache[index].pin_count = 0;
		cache->cache[index].page_data = malloc(BLOCK_SIZE);
		printf("Copying page %d into the cache!\n", pnum);
		memcpy(cache->cache[index].page_data, disk_get_block(disk, pnum), BLOCK_SIZE);
		cache->cache[index].lru_pos = lru_push(cache->lru, index);
		cache->lru = cache->cache[index].lru_pos;
		pci_insert(cache->pci, pnum, index);
		return cache->cache[rv].page_data;
	} else {
		LRU_List *ptr = cache->cache[rv].lru_pos->next;
		int index;
		if (ptr) index = lru_pop(ptr);
		else index = lru_pop(cache->cache[rv].lru_pos);
		LRU_List *lru_pos = malloc(sizeof(LRU_List));
		lru_pos->index = index;
		cache->cache[rv].lru_pos = lru_pos;
		return cache->cache[rv].page_data;
	}
}

cache* alloc_cache()
{
	cache *cache = malloc(sizeof(cache));
	cache->pci = malloc(sizeof(PCI_HM));
	cache->dirty_list = malloc(sizeof(DL_HM));
	return cache;
}

void free_cache(cache *cache)
{
	free(cache->pci);
	free(cache->dirty_list);
	free(cache);
	return cache;
}
