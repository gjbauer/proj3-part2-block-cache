#ifndef DL_H
#define DL_H
#include "config.h"

/* There is no reason in particular why our hashhap has to be the same size as our cache,
 * other than simply not having defined another macro...which we can. But, we set our indices
 * equal to our cache size in case if one file happens to take up our entire cache...unlikely
 * but certainly within the realm of possibility. Perhaps this should be another linked-list
 * instead? */

struct DL_LL
{
	int key;
	int indices[CACHE_SIZE];
	struct DL_LL *next;
};

typedef struct DL_HM
{
	struct DL_LL HashMap[CACHE_SIZE];
} DL_HM;

int dl_lookup(DL_HM *hashmap, int key);
void dl_insert(DL_HM *hashmap, int key, int index);
void dl_delete(DL_HM *hashmap, int key);

#endif
