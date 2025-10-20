#include <stdlib.h>
#include <string.h>
#include "disk.h"
#include "cache.h"
#include "fl.h"

void*
get_block(DiskInterface* disk, cache *cache, uint64_t inum, uint64_t pnum)
{
	int rv = pci_lookup(cache->pci, pnum);
	if (rv==-1) {
		if (cache->free_list==NULL) {
			// TODO: Evict from cache
			int cache_index = lru_pop(cache->lru);
			if (cache->cache[cache_index].dirty_bit)
			{
				// TODO: Write back to disk
				memcpy(disk_get_block(disk, cache->cache[cache_index].block_number), cache->cache[cache_index].page_data, BLOCK_SIZE);
				if (cache->cache[cache_index].is_data_block) dl_remove_block(cache->dirty_list, cache->cache[cache_index].inode_number, cache->cache[cache_index].block_number);
			}
			pci_delete(cache->pci, cache->cache[cache_index].block_number);
			cache->free_list = fl_push(cache->free_list, cache_index);
		}
		int index = cache->free_list->index;
		cache->free_list = fl_pop(cache->free_list);
		cache->cache[index].dirty_bit = false;
		cache->cache[index].pin_count = 0;
		cache->cache[index].block_number = pnum;
		cache->cache[index].inode_number = inum;
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

void
write_block(cache *cache, void *buf, uint64_t inum, uint64_t pnum)
{
	int index = pci_lookup(cache->pci, pnum);
	memcpy(cache->cache[index].page_data, buf, BLOCK_SIZE);
	cache->cache[index].dirty_bit = true;
	dl_insert(cache->dirty_list, inum, pnum);
}

cache* alloc_cache()
{
	cache *cache = malloc(sizeof(cache));
	cache->pci = malloc(sizeof(PCI_HM));
	cache->dirty_list = malloc(sizeof(DL_HM));
	for (int i=0; i<CACHE_SIZE; i++) {
		printf("Pushing cache index %d to free list.\n", i);
		cache->free_list = fl_push(cache->free_list, i);
	}
	return cache;
}

void free_cache(cache *cache)
{
	free(cache->pci);
	free(cache->dirty_list);
	free(cache);
}
