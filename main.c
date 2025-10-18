#include <stdio.h>
#include <stdlib.h>
#include "disk.h"
#include "lru.h"
#include "pci.h"
#include "fl.h"
#include "cache.h"
#include "dl.h"

int main()
{
	DiskInterface* disk = disk_open("my.img");
	cache_entry_t cache[CACHE_SIZE];
	PCI_HM *pci = malloc(sizeof(PCI_HM));
	LRU_List *lru;
	
	FL_LL *list;
	for (int i=0; i<CACHE_SIZE; i++) {
		printf("Pushing cache index %d to free list.\n", i);
		list = fl_push(list, i);
	}
	
	// TODO: Implement functions for our data structures and test them here...
	// TODO: After testing, integrate it with our B-Tree...
	while (true) {
		printf("Select:\n(1) to simulate read\n(2) to simulate write\n(3) to simulate sync\n> ");
		int choice, block, inode;
		scanf("%d", &choice);
		switch (choice) {
			case 1:
				printf("Block to read: ");
				scanf("%d", &block);
				// TODO: Declare/insert cache functions
				get_block(disk, pci, cache, list, lru, block);
				break;
			case 2:
				printf("Block to write: ");
				scanf("%d", &block);
				printf("Inode to write: ");
				scanf("%d", &inode);
				// TODO: Declare/insert cache functions
				break;
			case 3:
				printf("Inode to sync: ");
				scanf("%d", &inode);
				// TODO: Declare/insert cache functions
				break;
			default:
				return 0;  // Exit program
		}
	}
}

