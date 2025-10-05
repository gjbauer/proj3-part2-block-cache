#ifndef PCI_H
#define PCI_H
#include "config.h"

struct PCI_LL
{
	int key;
	int index;
	struct PCI_LL *next;
};

typedef struct PCI_HM
{
	struct PCI_LL HashMap[CACHE_SIZE];
} PCI_HM;

int pci_lookup(PCI_HM *hashmap, int key);
void pci_insert(PCI_HM *hashmap, int key, int index);
void pci_delete(PCI_HM *hashmap, int key);

#endif
