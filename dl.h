#ifndef DL_H
#define DL_H
#include "config.h"

/* There is no reason in particular why our hashhap has to be the same size as our cache,
 * other than simply not having defined another macro...which we can. We should get rid of
 * the CACHE_SIZE macro towards the end of our implementation after we define a size for 
 * our hashmap and create a function to dynamically choose a size for the cache base upon
 * system RAM.
 */

struct DL_LL
{
	int block_number;
	struct DL_LL *next;
};

struct DL_HM_LL
{
	int key;
	struct DL_LL *list;
	struct DL_HM_LL *next;
};

typedef struct DL_HM
{
	struct DL_HM_LL HashMap[CACHE_SIZE];
} DL_HM;

int dl_lookup(DL_HM *hashmap, int key);
void dl_insert(DL_HM *hashmap, int key, int block_number);
void dl_delete(DL_HM *hashmap, int key);

#endif
