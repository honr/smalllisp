
#include <stdio.h>
#include <stdlib.h>
#include "cons.h"

struct cons Q_u4 = {.first = "Unsigned Int32",.next = NULL };
struct cons Q_STRING = {.first = "String",.next = NULL };
struct cons Q_CONS = {.first = "Cons",.next = NULL };

struct cons *
int_alloc (int i)
{
  int *j = malloc (sizeof (int));
  *j = i;
  return cons_alloc (j, &Q_u4);
}

int
main ()
{
  struct cons *c;
  printf ("cons-alloc:\n");

  c = int_alloc (0x10);
  c = cons_alloc (c, NULL);	// make a list out of it
  printf ("%d\n", *(int *) ((struct cons *) c->first)->first);
  printf ("--------------------\n");

  printf ("cons-insert-tail:\n");
  struct cons *curs = cons_alloc (NULL, NULL);
  cons_insert_tail (curs, int_alloc (10));
  cons_insert_tail (curs, int_alloc (20));
  cons_insert_tail (curs, int_alloc (30));
  cons_insert_tail (curs, int_alloc (40));
  c = (struct cons *) curs->first;
  printf ("%d\n", *(int *) ((struct cons *) c->first)->first);
  printf ("%d\n", *(int *) ((struct cons *) c->next->first)->first);
  printf ("%d\n", *(int *) ((struct cons *) c->next->next->first)->first);
  printf ("%d\n",
	  *(int *) ((struct cons *) c->next->next->next->first)->first);
  printf ("--------------------\n");

  return 0;
}
