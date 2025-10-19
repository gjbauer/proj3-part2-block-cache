#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include "config.h"

#include "pci.h"
#include "fl.h"
#include "lru.h"
#include "dl.h"

typedef struct cache_entry_t
{
	bool dirty_bit;
	int pin_count;
	void *page_data;
	struct LRU_List *lru_pos;
} cache_entry_t;

typedef struct cache
{
	// TODO: Adjust cache size based upon total system memory!!
	int cache_size;
	cache_entry_t cache[CACHE_SIZE];
	PCI_HM *pci;
	LRU_List *lru;
	FL_LL *free_list;
	DL_LL *dirty_list;
} cache;

void*
get_block(DiskInterface* disk, cache *cache, int pnum);

cache*
alloc_cache();

void
free_cache(cache *cache);

#endif
