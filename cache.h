#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include "config.h"

typedef struct cache_entry_t
{
	bool dirty_bit;
	int pin_count;
	void *page_data;
	struct LRU_List *lru_pos;
} cache_entry_t;

void*
get_block(DiskInterface* disk, PCI_HM *hashmap, cache_entry_t *cache, FL_LL *free_list, LRU_List *lru, int pnum);

#endif
