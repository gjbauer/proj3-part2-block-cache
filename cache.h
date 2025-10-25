#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"
#include "disk.h"

#include "pci.h"
#include "fl.h"
#include "lru.h"
#include "dl.h"

typedef struct cache_entry_t
{
	bool dirty_bit;
	int pin_count;
	uint64_t block_number;
	uint64_t inode_number;
	void *page_data;
	struct LRU_List *lru_pos;
} cache_entry_t;

typedef struct cache
{
	uint64_t cache_size;
	uint64_t lru_size;
	cache_entry_t *cache;
	PCI_HM *pci;
	LRU_List *lru;
	FL_LL *free_list;
	DL_HM *dirty_list;
} cache;

/* In this case, we push to the head of the list and pop from the tail.
 * In the other case we can push and pop from the head. */
LRU_List *lru_push(cache *cache, int index);
int64_t lru_pop(cache *cache, LRU_List *list);

void*
get_block(DiskInterface* disk, cache *cache, uint64_t inum, uint64_t pnum);

void
write_block(DiskInterface* disk, cache *cache, void *buf, uint64_t inum, uint64_t pnum);

cache*
alloc_cache();

void
free_cache(cache *cache);

#endif
