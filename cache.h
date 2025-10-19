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
	cache_entry_t cache[CACHE_SIZE];
	PCI_HM pci;
	LRU_List *lru;
	FL_LL *free_list;
	DL_LL *dirty_list;
} cache;

void*
get_block(DiskInterface* disk, PCI_HM *hashmap, cache_entry_t *cache, FL_LL *free_list, LRU_List *lru, int pnum);

#endif
