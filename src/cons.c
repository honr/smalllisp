// TODO: put inline functions in the header and make them really inline.
#include "cons.h"
#include <stdlib.h>

inline struct cons *
cons_alloc (void *first, struct cons *next)
{
  struct cons *c = malloc (sizeof (struct cons));
  c->first = first;
  c->next = next;
  return c;
}

inline int
cons_count (struct cons *c)
{
  int r;
  for (r = 0; c; c = c->next, ++r);
  return r;
}

inline void
cons_free (struct cons *c)
{
  // input is allowed to be nil.
  struct cons *p = c;
  if (c)
    {
      for (p = c, c = c->next; c; p = c, c = c->next)
	{
	  free (p);
	}
      free (p);
    }
}

inline struct cons *
cons_pop (struct cons *c)
{
  // frees the first cell, returns the rest of the cons.
  struct cons *r;
  if (c)
    {
      r = c->next;
      free (c);
    }
  else
    {
      r = NULL;
    }
  return r;
}

inline void
cons_nreverse (struct cons **cp)
{
  if (*cp)
    {
      struct cons *p, *c, *n;
      for (p = NULL, c = *cp; c; n = c->next, c->next = p, p = c, c = n);
      *cp = c;
    }
}

inline struct cons *
cons_last (struct cons *c)
{
  struct cons *r;
  if (c)
    {
      for (r = c, c = c->next; c; r = c, c = c->next);
    }
  else
    {
      r = NULL;
    }
  return r;
}

inline void
cons_insert_tail (struct cons *cursor, void *item)
{
  /* cursor->first points to the HEAD of the list,
     cursor->next points to the CURRENT position, or nil if not specified
     - when both are non-nil, inserts there and updates cursor->next (tail).
     - when HEAD is non-nil but CURRENT is nil,
     finds the tail first and insert there.
     - when both are nil, sets both HEAD and CURRENT to the new cons. */

  struct cons *consed_item = cons_alloc (item, NULL);
  if (cursor->first)
    {
      if (cursor->next)
	{
	  cursor->next = (cursor->next->next = consed_item);
	}
      else
	{
	  cursor->next = (cons_last (cursor->first)->next = consed_item);
	}
    }
  else
    {
      cursor->first = (cursor->next = consed_item);
    }
}

inline struct bin *
bin_alloc (void *first, struct bin *left, struct bin *right)
{
  struct bin *b = malloc (sizeof (struct bin));
  b->first = first;
  b->left = left;
  b->right = right;
  return b;
}

inline int
bin_height (struct bin *b)
{
  if (b)
    {
      int l = bin_height (b->left);
      int r = bin_height (b->right);
      return (l > r) ? l : r;
    }
  else
    {
      return 0;
    }
}

inline void
bin_free (struct bin *b)
{
  if (b)
    {
      bin_free (b->left);
      bin_free (b->right);
      free (b);
    }
}
