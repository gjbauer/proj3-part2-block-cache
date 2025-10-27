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
	
	cache *cache = alloc_cache();
	
	// TODO: Implement functions for our data structures and test them here...
	// TODO: After testing, integrate it with our B-Tree...
	while (true) {
		printf("Select:\n(1) to simulate read\n(2) to simulate write\n(3) to simulate fsync\n(4) to simulate sync\n> ");
		int choice, block, inode;
		scanf("%d", &choice);
		switch (choice) {
			case 1:
				printf("Block to read: ");
				scanf("%d", &block);
				printf("Inode to read: ");
				scanf("%d", &inode);
				get_block(disk, cache, inode, block);
				break;
			case 2:
				printf("Block to write: ");
				scanf("%d", &block);
				printf("Inode to write: ");
				scanf("%d", &inode);
				void *empty = malloc(4096);
				write_block(disk, cache, empty, inode, block);
				free(empty);
				break;
			case 3:
				printf("Inode to sync: ");
				scanf("%d", &inode);
				cache_fsync(disk, cache, inode);
				break;
			case 4:
				cache_sync(disk, cache);
				break;
			default:
				free_cache(cache);
				disk_close(disk);
				return 0;  // Exit program
		}
	}
}

