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
} cache_entry_y;

#endif
