#ifndef HTRIE_H
#define HTRIE_H

#include "cons.h"
#include "box.h"

struct bin *htrie_alloc (void *val);
void *htrie_get (struct bin *b, char *key);
void *htrie_get_ref (struct bin *b, char *key);
int htrie_assoc (struct bin **b, char *key, void *val);
// returns 0 or 1 on success, -1 on error.

void htrie_dissoc (struct bin **b, char *key);
void htrie_free (struct bin *b);
struct bin *htrie_subtree (struct bin *b, char *prefix);

#endif
