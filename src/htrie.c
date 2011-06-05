#include "htrie.h"
#include <stdlib.h>

// debug
#include <stdio.h>

inline struct bin *
chartree_get (void **ctree, char ch)
{
  int i;
  for (i = sizeof (char) << 1;
       ctree && i; ctree = ctree[ch & 0x0F], ch >>= 4, --i);
  return (struct bin *) ctree;
}

/** assoc ch -> leaf in chartree ctree. return pointer to leaf. */
inline void **
chartree_assoc (void ***p_ctree, char ch, struct bin *leaf)
{
  unsigned char ch_high = (ch & 0xF0) >> 4;
  unsigned char ch_low = ch & 0x0F;
  if (!*p_ctree)
    {
      *p_ctree = calloc (16, sizeof (void *));
    }
  void **ctree = *p_ctree;
  if (!ctree[ch_low])
    {
      ctree[ch_low] = calloc (16, sizeof (void *));
    }
  ctree = ctree[ch_low];
  ctree[ch_high] = leaf;
  return &ctree[ch_high];
}

/******************************************************************************/

struct bin *
htrie_alloc (void *val)
{
  return NULL;
}

void *
htrie_get (struct bin *b, char *key)
{
  /* structure:
     a-node: [common|NULL NULL|value tree(b-node)]
     tree(b-node): [16 sub-tree (b-node)]
     sub-tree(b-node): [16 leaves (a-node)]
     leaf(a-node): same as above, i.e., [common|NULL NULL|value next(b-tree)].
   */
  while (b)
    {
      char *cur;
      for (cur = (char *) b->first; *key && (*key == *cur); ++cur, ++key);

      if (*cur)
	{
	  return NULL;
	}
      else if (*key)		// continue with subtree
	{
	  b = chartree_get ((void **) b->right, *key);
	}
      else			// possible match. result could be NULL if not a real match.
	{
	  return b->left;
	}
    }
  // if we haven't returned so far that means the given key matches nothing.
  return NULL;
}

void *
htrie_get_ref (struct bin *b, char *key)
{
  while (b)
    {
      char *cur;
      for (cur = (char *) b->first; *key && (*key == *cur); ++cur, ++key);

      if (*cur)
	{
	  return NULL;
	}
      else if (*key)		// continue with subtree
	{
	  b = chartree_get ((void **) b->right, *key);
	}
      else			// possible match. result could be NULL if not a real match.
	{
	  return &b->left;
	}
    }
  // if we haven't returned so far that means the given key matches nothing.
  return NULL;
}

int
htrie_assoc (struct bin **bp, char *key, void *val)
{
  while (1)
    {
      if (!*bp)			// empty! create a new one.
	{
	  *bp = bin_alloc (strdup (key), val, NULL);
	  return 1;
	}
      else			// not empty. modify current one.
	{
	  char *cur;
	  for (cur = (*bp)->first; *key && (*cur == *key); ++cur, ++key);

	  if (*cur)		// split at cur
	    {
	      char *x;
	      char *y;
	      str_split ((char *) (*bp)->first, cur, &x, &y);
	      free ((*bp)->first);
	      (*bp)->first = y;
	      void **ctree = NULL;
	      chartree_assoc (&ctree, *y, *bp);
	      *bp = bin_alloc (x, NULL, (struct bin *) ctree);

	      if (*key)
		{
		  struct bin *leaf = bin_alloc (strdup (key), val, NULL);
		  chartree_assoc ((void ***) (&(*bp)->right), *key, leaf);
		}
	      else
		{
		  (*bp)->left = val;
		}

	      return 2;
	    }
	  else if (*key)	// cur ran out. key has remains.
	    // go to sub-tree in *bp
	    {
	      struct bin *b = chartree_get ((void **) (*bp)->right, *key);
	      bp = (struct bin **) chartree_assoc ((void ***) (&(*bp)->right),
						   *key, b);
	    }
	  else			// both ran out. match. just update.
	    {
	      (*bp)->left = val;
	      return 0;
	    }
	}
    }
  return -1;
}

void
htrie_dissoc (struct bin **b, char *key)
{
  // dissoc and elevate subtree if possible.
}

void
htrie_free (struct bin *b)
{
}

struct bin *
htrie_subtree (struct bin *b, char *prefix)
{
  return NULL;
}
