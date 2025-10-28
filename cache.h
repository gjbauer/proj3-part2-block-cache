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

/**
 * Represents a single cache entry containing a disk block
 */
typedef struct cache_entry_t
{
	bool dirty_bit;              // True if block has been modified and needs writeback
	int pin_count;               // Reference count for preventing eviction
	uint64_t block_number;       // Disk block number this entry represents
	uint64_t inode_number;       // Inode that owns this block (for data blocks)
	void *page_data;             // Pointer to the actual cached block data
	struct LRU_List *lru_pos;    // Position in LRU list for eviction policy
	struct GDL *gdl_pos;         // Position in global dirty list
} cache_entry_t;

/**
 * Main cache structure managing all cached disk blocks
 */
typedef struct cache
{
	int cache_size;              // Total number of cache entries
	int lru_size;                // Current size of LRU list
	int gdl_size;                // Current size of global dirty list
	cache_entry_t *cache;        // Array of cache entries
	PCI_HM *pci;                 // Primary Cache Index: maps block_number -> cache_index
	LRU_List *lru;               // LRU list head for eviction policy
	FL_LL *free_list;            // Free list of available cache slots
	DL_HM *dirty_list;           // Dirty list: maps inode_number -> dirty blocks
	GDL *gdl;                    // Global dirty list for sync operations
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

/**
 * Retrieve a block from cache, loading from disk if necessary
 */
void*
get_block(DiskInterface* disk, cache *cache, uint64_t inum, uint64_t pnum);

/**
 * Write data to a cached block, marking it dirty
 */
void
write_block(DiskInterface* disk, cache *cache, void *buf, uint64_t inum, uint64_t pnum);

/**
 * Sync all dirty blocks for a specific inode to disk
 */
void cache_fsync(DiskInterface* disk, cache *cache, uint64_t inum);

/**
 * Sync all dirty blocks in the cache to disk
 */
void cache_sync(DiskInterface* disk, cache *cache);

/**
 * Allocate and initialize a new cache structure
 */
cache*
alloc_cache();

/**
 * Free all memory associated with a cache structure
 */
void
free_cache(cache *cache);

#endif
