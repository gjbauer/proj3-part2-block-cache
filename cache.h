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
#include "gdl.h"

typedef struct cache_entry_t
{
	bool dirty_bit;
	int pin_count;
	uint64_t block_number;
	uint64_t inode_number;
	void *page_data;
	struct LRU_List *lru_pos;
	struct GDL *gdl_pos;
} cache_entry_t;

typedef struct cache
{
	int cache_size;
	int lru_size;
	int gdl_size;
	cache_entry_t *cache;
	PCI_HM *pci;
	LRU_List *lru;
	FL_LL *free_list;
	DL_HM *dirty_list;
	struct GDL *gdl;
} cache;

/* In this case, we push to the head of the list and pop from the tail.
 * In the other case we can push and pop from the head. */
LRU_List *lru_push(cache *cache, int index);
int64_t lru_pop(cache *cache, LRU_List *list);

/* In this case, we push to the head of the list, and pop from
 * wherever in the list the given node is placed.
 */
GDL *gdl_push(cache *cache, int index);
void gdl_pop(cache *cache, GDL *list);

void*
get_block(DiskInterface* disk, cache *cache, uint64_t inum, uint64_t pnum);

void
write_block(DiskInterface* disk, cache *cache, void *buf, uint64_t inum, uint64_t pnum);

void cache_fsync(DiskInterface* disk, cache *cache, uint64_t inum);

void cache_sync(DiskInterface* disk, cache *cache);

cache*
alloc_cache();

void
free_cache(cache *cache);

#endif
