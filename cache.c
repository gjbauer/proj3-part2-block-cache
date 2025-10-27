#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/param.h>
#include <bsd/stdlib.h>
#include "disk.h"
#include "cache.h"

void*
get_block(DiskInterface* disk, cache *cache, uint64_t inum, uint64_t pnum)
{
	int rv = pci_lookup(cache->pci, pnum);
	if (rv==-1) {
		if (cache->free_list==NULL) {
			int cache_index = lru_pop(cache, cache->lru);
			if (cache->cache[cache_index].dirty_bit)
			{
				block_type_t *block_type = (block_type_t*)cache->cache[cache_index].page_data;
				memcpy(disk_get_block(disk, cache->cache[cache_index].block_number), cache->cache[cache_index].page_data, BLOCK_SIZE);
				free(cache->cache[cache_index].page_data);
				if (block_type==BLOCK_TYPE_DATA) dl_remove_block(cache->dirty_list, cache->cache[cache_index].inode_number, cache->cache[cache_index].block_number);
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
		printf("Copying page %lu into the cache!\n", pnum);
		memcpy(cache->cache[index].page_data, disk_get_block(disk, pnum), BLOCK_SIZE);
		cache->cache[index].lru_pos = lru_push(cache, index);
		cache->lru = cache->cache[index].lru_pos;
		pci_insert(cache->pci, pnum, index);
		return cache->cache[index].page_data;
	} else {
		LRU_List *ptr = cache->cache[rv].lru_pos->next;
		int index;
		if (ptr) index = lru_pop(cache, ptr);
		else index = lru_pop(cache, cache->cache[rv].lru_pos);
		cache->cache[index].lru_pos = lru_push(cache, index);
		return cache->cache[rv].page_data;
	}
}

void
write_block(DiskInterface* disk, cache *cache, void *buf, uint64_t inum, uint64_t pnum)
{
	int index = pci_lookup(cache->pci, pnum);
	if (index==-1)
	{
		get_block(disk, cache, inum, pnum);
		index = pci_lookup(cache->pci, pnum);
	}
	block_type_t *block_type = (block_type_t*)cache->cache[index].page_data;
	memcpy(cache->cache[index].page_data, buf, BLOCK_SIZE);
	cache->cache[index].dirty_bit = true;
	if (block_type==BLOCK_TYPE_DATA) dl_insert(cache->dirty_list, inum, pnum);
}

void cache_fsync(DiskInterface* disk, cache *cache, uint64_t inum)
{
	DL_HM_LL *hmlist = cache->dirty_list->HashMap[inum % HASHMAP_SIZE];
	DL_HM_LL *prev;
	if (hmlist)
	{
		while (hmlist->inode_number!=inum)
		{
			prev = hmlist;
			hmlist = hmlist->next;
		}
		DL_LL *list;
		list = hmlist->list;
		while (list)
		{
			int index = pci_lookup(cache->pci, list->block_number);
			memcpy(disk_get_block(disk, cache->cache[index].block_number), cache->cache[index].page_data, BLOCK_SIZE);
			cache->cache[index].dirty_bit=false;
			list = dl_pop(list);
		}
		if (prev) prev->next = hmlist->next;
		arc4random_buf(hmlist, sizeof(struct DL_HM_LL));
		free(hmlist);
	}
}

cache* alloc_cache()
{
	struct sysinfo info;
	sysinfo(&info);
	int gb_ram = info.totalram / (1024 * 1024 * 1024);
	uint64_t cache_size = 0;
	if (gb_ram < 2) cache_size = (64 * 1024 * 1024) / 4096;
	else if (gb_ram > 2 && gb_ram <= 16) cache_size = info.totalram / (8 * 4096);
	else cache_size = MIN( (2*1024*1024), (info.totalram / (8 * 4096)));
	cache *cache = malloc(sizeof(struct cache));
	cache->cache_size = cache_size;
	cache->cache = malloc(cache_size * sizeof(struct cache_entry_t));
	for (int i=0; i<cache_size; i++)
	{
		cache->cache[i].dirty_bit = false;
	}
	cache->lru_size = 0;
	cache->pci = malloc(sizeof(struct PCI_HM));
	for (int i=0; i<HASHMAP_SIZE; i++)
	{
		cache->pci->HashMap[i] = NULL;
	}
	cache->dirty_list = malloc(sizeof(struct DL_HM));
	for (int i=0; i<HASHMAP_SIZE; i++)
	{
		cache->dirty_list->HashMap[i] = NULL;
	}
	for (int i=0; i<cache_size; i++) {
		printf("Pushing cache index %d to free list.\n", i);
		cache->free_list = fl_push(cache->free_list, i);
	}
	return cache;
}

void free_cache(cache *cache)
{
	for (int i=0; i<HASHMAP_SIZE; i++)
	{
		DL_HM_LL *hmlist = cache->dirty_list->HashMap[i];
		DL_HM_LL *prev;
		while (hmlist)
		{
			prev = hmlist;
			DL_LL *list;
			list = hmlist->list;
			while (list)
			{
				list = dl_pop(list);
			}
			hmlist = hmlist->next;
			arc4random_buf(prev, sizeof(struct DL_HM_LL));
			free(prev);
		}
	}
	arc4random_buf(cache->dirty_list, sizeof(struct DL_HM));
	free(cache->dirty_list);
	while (cache->free_list!=NULL)
	{
		printf("Popping cache index %d from free list.\n", cache->free_list->index);
		cache->free_list = fl_pop(cache->free_list);
	}
	for (int i=cache->lru_size; i>0; i--, cache->lru_size--)
	{
		lru_pop(cache, cache->lru);
	}
	for (int i=0; i<HASHMAP_SIZE; i++)
	{
		PCI_LL *prev;
		while (cache->pci->HashMap[i])
		{
			prev = cache->pci->HashMap[i];
			cache->pci->HashMap[i] = cache->pci->HashMap[i]->next;
			arc4random_buf(prev, sizeof(struct PCI_LL));
			free(prev);
		}
	}
	arc4random_buf(cache->pci, sizeof(struct PCI_HM));
	free(cache->pci);
	arc4random_buf(cache->cache, cache->cache_size * sizeof(struct cache_entry_t));
	free(cache->cache);
	arc4random_buf(cache, sizeof(struct cache));
	free(cache);
}
