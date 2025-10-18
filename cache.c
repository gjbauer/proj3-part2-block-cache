#include "disk.h"
#include "pci.h"
#include "cache.h"

void*
get_block(DiskInterface* disk, PCI_HM *hashmap, cache_entry_t *cache, int pnum)
{
	int rv = pci_lookup(hashmap, pnum);
	if (rv==-1) {
	} else {
		return cache[rv].page_data;
	}
}
