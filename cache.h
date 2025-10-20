#ifndef CACHE_H
#define CACHE_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

#include "pci.h"
#include "fl.h"
#include "lru.h"
#include "dl.h"

typedef struct cache_entry_t
{
	// Does this block belong to a file/directory with an associated inode, or is it a system block, used solely by the filesystem?
	bool is_data_block;
	bool dirty_bit;
	int pin_count;
	uint64_t block_number;
	uint64_t inode_number;
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
	DL_HM *dirty_list;
} cache;

void*
get_block(DiskInterface* disk, cache *cache, uint64_t inum, uint64_t pnum);

void
write_block(cache *cache, void *buf, uint64_t inum, uint64_t pnum);

cache*
alloc_cache();

void
free_cache(cache *cache);

#endif
