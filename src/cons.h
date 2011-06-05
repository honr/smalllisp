#ifndef CONS_H_
#define CONS_H_

/* unary cell */
struct cons
{
  void *first;
  struct cons *next;
};

struct cons *cons_alloc (void *first, struct cons *next);
int cons_count (struct cons *c);
void cons_free (struct cons *c);
void cons_nconcat (struct cons *dest, struct cons* tail);
struct cons *cons_pop (struct cons *c);
void cons_nreverse (struct cons **c);
struct cons *cons_last (struct cons *c);
void cons_insert_tail (struct cons *cursor, void *item);

#ifdef DEBUG
void dbg_cons_print (struct cons *c);
#endif // DEBUG

/* binary cell */
struct bin
{
  void *first;
  struct bin *left;
  struct bin *right;
};

struct bin_a
{
  void *first;
  struct bin next[2];
};

struct bin *bin_alloc (void *first, struct bin *left, struct bin *right);
int bin_height (struct bin *b);
void bin_free (struct bin *b);

#ifdef DEBUG
void dbg_bin_print (struct bin *b);
#endif // DEBUG


/* typedefs
   style guide: only use in application code.
 */
/* typedef struct cons cons; */
/* typedef struct bin bin; */

#endif // CONS_H_
