#include "box.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// struct cons Q_NUMBER = {.first.p = "Number",.next = NULL };
struct cons Q_STRING = {.first.p = "String",.next = NULL };
struct cons Q_CONS = {.first.p = "Cons",.next = NULL };
struct cons Q_MACRO_INTERP = {.first.p = "Macro (interpretee)",.next = NULL };
struct cons Q_FUNCTION_INTERP = {.first.p = "Function (interpretee)",.next =
    NULL };
struct cons Q_LAMBDA_INTERP = {.first.p = "Lambda (interpretee)",.next = NULL };
struct cons Q_SYMBOL = {.first.p = "Symbol",.next = NULL };
struct cons Q_KEYWORD = {.first.p = "Keyword",.next = NULL };

// struct cons Q_SYMROOT = {.first.p = "Symbol (not bound)",.next = NULL };

struct cons Q_u1 = {.first.p = "Unsigned Int8",.next = NULL };
struct cons Q_u2 = {.first.p = "Unsigned Int16",.next = NULL };
struct cons Q_u4 = {.first.p = "Unsigned Int32",.next = NULL };
struct cons Q_u8 = {.first.p = "Unsigned Int64",.next = NULL };
struct cons Q_z1 = {.first.p = "Signed Int8",.next = NULL };
struct cons Q_z2 = {.first.p = "Signed Int16",.next = NULL };
struct cons Q_z4 = {.first.p = "Signed Int32",.next = NULL };
struct cons Q_z8 = {.first.p = "Signed Int64",.next = NULL };
struct cons Q_y4 = {.first.p = "Float",.next = NULL };
struct cons Q_y8 = {.first.p = "Double",.next = NULL };

struct cons Q_RGB3 = {.first.p = "RGB Triplet",.next = NULL };

// () is not nil!  should we fix that?

// should be a symbol.
struct cons Q_VAL_NONNIL = {.first.p = "true",.next = &Q_STRING };	// bad
struct cons Q_VAL_UNQUOTE = {.first.p = "unquote",.next = &Q_KEYWORD };	// bad
struct cons Q_VAL_UNQUOTE_LIST = {.first.p = "unquote",.next = &Q_KEYWORD };	// bad

// similar to keywords, except these do not start with ':'


#define MAX_NUMBER_STR 256	// maximum size of buffer to print numbers
static char box__string_buff_[MAX_NUMBER_STR];

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int
str_split (char *xy, char *cur, char **xp, char **yp)
{
  int x_len = cur - xy;
  char *x = malloc (x_len + 1);
  memcpy (x, xy, x_len);
  x[x_len] = 0;
  int y_len = strlen (xy) - x_len;
  char *y = malloc (y_len + 1);
  memcpy (y, cur, y_len);
  y[y_len] = 0;
  *xp = x;
  *yp = y;
  return y_len;
}

inline char *
strcat_rf (char **p_dest, const char *dest_limit, const char *src)
/* If dest is NULL do nothing and just return NULL.  Otherwise, copy
   the string from src to dest, up to memory location dest_limit.  If
   '\0' does not occur within that range (src string is too long),
   return NULL.  */
{
  register char *d = *p_dest;
  register const char *s = src;
  register int n = dest_limit - d - 1;

  if (d && (n > 0))
    {
      while (n--)
	{
	  if (!*s)
	    {
	      *p_dest = d;
	      return d;
	    }
	  *d++ = *s++;
	}
    }
  *p_dest = NULL;
  return NULL;
}

/* int sprintf_rf (char** p_dest, const char* dest_limit, const char* format, ...)
{
  // sprintf (*p_dest, format, ...);
  return 0;
} */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

inline struct cons *
cons_alloc (void *first, struct cons *next)
{
  struct cons *c = malloc (sizeof (struct cons));
  c->first.p = first;
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
  /* cursor->first.p points to the HEAD of the list,
     cursor->next points to the CURRENT position, or nil if not specified
     - when both are non-nil, inserts there and updates cursor->next (tail).
     - when HEAD is non-nil but CURRENT is nil,
     finds the tail first and insert there.
     - when both are nil, sets both HEAD and CURRENT to the new cons. */

  struct cons *consed_item = cons_alloc (item, NULL);
  if (cursor->first.p)
    {
      if (cursor->next)
	{
	  cursor->next = (cursor->next->next = consed_item);
	}
      else
	{
	  cursor->next = (cons_last (cursor->first.c)->next = consed_item);
	}
    }
  else
    {
      cursor->first.p = (cursor->next = consed_item);
    }
}

inline struct bin *
bin_alloc (void *first, struct bin *left, struct bin *right)
{
  struct bin *b = malloc (sizeof (struct bin));
  b->first.p = first;
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

inline struct cons *
box_type (struct cons *c)
{
  return c ? c->next : NULL;
}

inline struct cons *
int_alloc (int i)
{
  int *j = malloc (sizeof (int));
  *j = i;
  return cons_alloc (j, &Q_z4);
}

inline int
int_unbox (struct cons *c)
{
  return *((int *) c->first.p);
}

inline struct cons *
long_alloc (long i)
{
  union quan j;
  j.z8 = i;
  return cons_alloc (j.p, &Q_z8);
}

inline long
long_unbox (struct cons *c)
{
  return c->first.z8;
}

inline struct cons *
double_alloc (double i)
{
  union quan j;
  j.y8 = i;
  return cons_alloc (j.p, &Q_y8);
}

inline double
double_unbox (struct cons *c)
{
  return c->first.y8;
}

inline struct cons *
rgb_alloc (struct rgb3 i)
{
  struct rgb3 *j = malloc (sizeof (struct rgb3));
  *j = i;
  return cons_alloc (j, &Q_RGB3);
}

inline struct rgb3
rgb_unbox (struct cons *c)
{
  return *((struct rgb3 *) c->first.p);
}

inline struct rgb3
rgb_add (struct rgb3 a, struct rgb3 b)
{
  struct rgb3 result = {.r = a.r + b.r,
    .g = a.g + b.g,
    .b = a.b + b.b
  };
  return result;
}

inline struct rgb3
rgb_mult (double coeff, struct rgb3 a)
{
  struct rgb3 result = {.r = a.r * coeff,
    .g = a.g * coeff,
    .b = a.b * coeff
  };
  return result;
}

inline struct rgb3
rgb_inverse (struct rgb3 a)
{
  struct rgb3 result = {.r = -a.r,
    .g = -a.g,
    .b = -a.b
  };
  return result;
}

inline struct cons *
number_alloc (char *s)
{
  if (s)
    {
      if (strchr (s, '.'))
	{
	  union quan j;
	  j.y8 = strtod (s, NULL);
	  return cons_alloc (j.p, &Q_y8);
	}
      /* else if ((sr = strchr (s, '/')))
         {} */
      else if (s[0] == '0' && s[1] == 'x')
	{
	  union quan j;
	  j.z8 = strtol (s, NULL, 16);
	  return cons_alloc (j.p, &Q_z8);
	}
      else if (s[0] == '3' && s[1] == 'x')	// color
	{
	  struct rgb3 *x = calloc (1, sizeof (struct rgb3));
	  if (strlen (s) == 8)
	    {
	      long l = strtol (s + 2, NULL, 16);
	      x->r = l >> 16;
	      x->g = l >> 8;
	      x->b = l;
	    }
	  else if (strlen (s) == 5)
	    {
	      long l = strtol (s + 2, NULL, 16);
	      x->r = (l >> 8) & 0x0F;
	      x->r |= x->r << 4;
	      x->g = (l >> 4) & 0x0F;
	      x->g |= x->g << 4;
	      x->b = l & 0x0F;
	      x->b |= x->b << 4;
	    }
	  else
	    {
	    }
	  return cons_alloc (x, &Q_RGB3);
	}
      else
	{
	  union quan j;
	  j.z8 = strtol (s, NULL, 10);
	  return cons_alloc (j.p, &Q_z8);
	}
    }
  else
    {
      return NULL;
    }
}

inline int
numberp (struct cons *c)
{
  if (c)
    {
      struct cons *type_specifier = c->next;
      return (type_specifier &&
	      ((type_specifier == &Q_u1) ||
	       (type_specifier == &Q_u2) ||
	       (type_specifier == &Q_u4) ||
	       (type_specifier == &Q_u8) ||
	       (type_specifier == &Q_z1) ||
	       (type_specifier == &Q_z2) ||
	       (type_specifier == &Q_z4) ||
	       (type_specifier == &Q_z8) ||
	       (type_specifier == &Q_y4) || (type_specifier == &Q_y8)));
    }
  else
    {
      return 0;
    }
}

inline int
number_to_str (char *dest, struct cons *c)
{
  struct cons *type_specifier = c->next;
  if (type_specifier == &Q_u1)
    {
      return sprintf (dest, "%d", c->first.u1);
    }
  else if (type_specifier == &Q_z1)
    {
      return sprintf (dest, "%d", c->first.z1);
    }
  else if (type_specifier == &Q_u2)
    {
      return sprintf (dest, "%d", c->first.u2);
    }
  else if (type_specifier == &Q_u4)
    {
      return sprintf (dest, "%d", c->first.u4);
    }
  else if (type_specifier == &Q_u8)
    {
      return sprintf (dest, "%ld", c->first.u8);
    }
  else if (type_specifier == &Q_z1)
    {
      return sprintf (dest, "%d", c->first.z1);
    }
  else if (type_specifier == &Q_z2)
    {
      return sprintf (dest, "%d", c->first.z2);
    }
  else if (type_specifier == &Q_z4)
    {
      return sprintf (dest, "%d", c->first.z4);
    }
  else if (type_specifier == &Q_z8)
    {
      return sprintf (dest, "%ld", c->first.z8);
    }
  else if (type_specifier == &Q_y4)
    {
      return sprintf (dest, "%f", c->first.y4);
    }
  else if (type_specifier == &Q_y8)
    {
      return sprintf (dest, "%f", c->first.y8);
    }
  else
    {
      return 0;
    }
}

inline struct cons *
string_alloc (char *s)
{
  return cons_alloc (s, &Q_STRING);
}

inline struct cons *
string_alloc_c (char *s)
{
  int len = strlen (s);
  char *s2 = malloc (len + 1);
  memcpy (s2, s, len);
  return cons_alloc (s2, &Q_STRING);
}

inline char *
string_unbox (struct cons *c)
{
  return (char *) c->first.p;
}

inline int
consp (struct cons *c)
{
  return (c->next == &Q_CONS);
}

inline struct cons *
cons_unbox (struct cons *c)
{
  return (struct cons *) c->first.p;
}


inline int
symbolp (struct cons *c)
{
  return (c->next == &Q_SYMBOL);
}

inline struct cons *
symbol_alloc (struct bin **p_symbols, char *s)
{
  struct symbol *sym = htrie_get (*p_symbols, s);
  if (!sym)
    {
      sym = malloc (sizeof (struct symbol));
      sym->name = s;
      sym->vals = NULL;
      sym->parent = NULL;	// should be *ns*.
      htrie_assoc (p_symbols, strdup (s), sym);
    }

  if (*s == ':')
    {
      return cons_alloc (sym, &Q_KEYWORD);
    }
  else
    {
      return cons_alloc (sym, &Q_SYMBOL);
    }
}

inline struct symbol *
symbol_unbox (struct cons *c)
{
  return (struct symbol *) c->first.p;
}

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
      for (cur = (char *) b->first.p; *key && (*key == *cur); ++cur, ++key);

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
      for (cur = (char *) b->first.p; *key && (*key == *cur); ++cur, ++key);

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
	  for (cur = (*bp)->first.p; *key && (*cur == *key); ++cur, ++key);

	  if (*cur)		// split at cur
	    {
	      char *x;
	      char *y;
	      str_split ((char *) (*bp)->first.p, cur, &x, &y);
	      free ((*bp)->first.p);
	      (*bp)->first.p = y;
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


void
cons_introspect (struct cons *c, int level, int expect_boxed)
{
  int i;
  *box__string_buff_ = 0;
  if (expect_boxed)
    {
      for (i = level; i; --i)
	{
	  printf ("    ");
	}
      if (!c)
	{
	  printf ("NULL\n");
	}
      else
	{
	  struct cons *specifier = c->next;
	  if (specifier == &Q_CONS)
	    {
	      printf (ASCIIYELLOW ("^Cons") "\n");
	      cons_introspect (c->first.p, level + 1, 0);
	    }
	  else if (specifier == &Q_STRING)
	    {
	      printf (ASCIIYELLOW ("^String") " \"%s\"\n", string_unbox (c));
	    }
	  else if (specifier == &Q_SYMBOL)
	    {
	      struct symbol *sym = c->first.p;
	      printf (ASCIIYELLOW ("^Symbol") " '%s\n", sym->name);
	    }
	  else if (specifier == &Q_KEYWORD)
	    {
	      struct symbol *sym = c->first.p;
	      printf (ASCIIYELLOW ("^Keyword") " %s\n", sym->name);
	    }
	  else if (numberp (c))
	    {
	      number_to_str (box__string_buff_, c);
	      printf (ASCIIYELLOW ("^Number") " %s\n", box__string_buff_);
	    }
	  else if (specifier == &Q_RGB3)
	    {
	      unsigned char *x = (unsigned char *) c->first.p;
	      printf (ASCIIYELLOW ("^RGB") " \"#%.2X%.2X%.2X\"\n", x[0], x[1],
		      x[2]);
	    }
	  else
	    {			// printf ("[Unknown Type] %p\n", c->first.p);
	      printf (ASCIIRED ("guesswork!") " %p\n", c->first.p);
	      cons_introspect (c->first.p, level + 1, 1);

	    }
	}
    }
  else
    {
      for (i = level - 1; i; --i)
	{
	  printf ("    ");
	}
      printf ("  " ASCIIYELLOW ("(") "\n");
      for (; c; c = c->next)
	{
	  cons_introspect (c->first.p, level, 1);
	}
      for (i = level - 1; i; --i)
	{
	  printf ("    ");
	}
      printf ("  " ASCIIYELLOW (")") "\n");
    }
}

void
box_print (struct cons *c)
{
  *box__string_buff_ = 0;
  if (!c)
    {
      printf ("nil");
    }
  else
    {
      struct cons *specifier = c->next;
      if (specifier == &Q_CONS)
	{
	  printf ("(");
	  for (c = c->first.p; c; c = c->next)
	    {
	      box_print (c->first.p);
	      if (c->next)
		{
		  printf (" ");
		}
	    }
	  printf (")");
	}
      else if (specifier == &Q_STRING)
	{
	  // printf ("\"%s\"", string_unbox (c));
	  printf ("%s", string_unbox (c));
	}
      else if (specifier == &Q_SYMBOL)
	{
	  struct symbol *sym = c->first.p;
	  printf ("'%s", sym->name);
	}
      else if (specifier == &Q_KEYWORD)
	{
	  struct symbol *sym = c->first.p;
	  printf ("%s", sym->name);
	}
      else if (numberp (c))
	{
	  number_to_str (box__string_buff_, c);
	  printf ("%s", box__string_buff_);
	}
      else if (specifier == &Q_RGB3)
	{
	  unsigned char *x = (unsigned char *) c->first.p;
	  printf ("\"#%.2X%.2X%.2X\"", x[0], x[1], x[2]);
	}
      else
	{
	  fprintf (stderr, "[Unknown Type] %p\n", c->first.p);
	}
    }
}

/*
char*
box_to_str (struct cons *c)
{
  // this bogus. buff overflow can easily happen.
  char* s = NULL;
  *box__string_buff_ = 0;

  if (!c)
    {
      s = strdup ("nil");
    }
  else
    {
      struct cons *specifier = c->next;
      if (specifier == &Q_CONS)
	{
	  *box__string_buff_ = 0;
	  for (c = (struct cons*) c->first.p; c; c = c->next)
	    { strcat (box__string_buff_, box_to_str (c->first.p));
	      if (c->next)
		{ strcat (box__string_buff_, " "); }
	    }
	}
      else if (specifier == &Q_STRING)
	{
	  s = string_unbox (c);
	}
      else if (specifier == &Q_SYMBOL)
	{
	  struct cons **sym = (struct cons **) c->first.p;
	  // cons_introspect (*sym, level + 1, 0);
	  if ((*sym)->next == &Q_SYMROOT)
	    {
	      sprintf (box__string_buff_, "'%s", (char *) (*sym)->first.p);
	      s = strdup (box__string_buff_);
	    }
	  else
	    {
	      fprintf (stderr, "Error: symbol with value\n");
	    }
	}
      else if (numberp (c))
	{
	  number_to_str (box__string_buff_, c);
	  s = strdup (box__string_buff_);
	}
      else if (specifier == &Q_RGB3)
	{
	  unsigned char *x = (unsigned char *) c->first.p;
	  sprintf (box__string_buff_, "\"#%.2X%.2X%.2X\"\n", x[0], x[1], x[2]);
	  s = strdup (box__string_buff_);
	}
      else
	{			
	  fprintf (stderr, "[Unknown Type] %p\n", c->first.p);
	}
    }
  return s;
}
*/
