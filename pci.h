#ifndef PCI_H
#define PCI_H
#include "config.h"

typedef struct PCI_LL
{
	uint64_t block_number;
	uint64_t index;
	struct PCI_LL *next;
} PCI_LL;

typedef struct PCI_HM
{
	struct PCI_LL HashMap[CACHE_SIZE];
} PCI_HM;

int pci_lookup(PCI_HM *hashmap, uint64_t block_number);
void pci_insert(PCI_HM *hashmap, uint64_t block_number, uint64_t index);
void pci_delete(PCI_HM *hashmap, uint64_t block_number);

#endif
