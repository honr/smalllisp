
#include <stdio.h>
#include <stdlib.h>
#include "cons.h"
#include "box.h"

int
main ()
{
  printf ("cons-alloc:\n");
  struct cons *c =		// (798 "foo")
    cons_alloc (int_alloc (798),
		cons_alloc (string_alloc ("foo"),
			    NULL));
  printf ("798 == %d\n", int_unbox (c->first));
  printf ("foo == %s\n", string_unbox (c->next->first));
  printf ("--------------------\n");

  printf ("cons-insert-tail:\n");
  struct cons *curs = cons_alloc (NULL, NULL);
  cons_insert_tail (curs, int_alloc (10));
  cons_insert_tail (curs, int_alloc (20));
  cons_insert_tail (curs, int_alloc (30));
  cons_insert_tail (curs, int_alloc (40));
  c = (struct cons *) curs->first;
  free (curs);
  printf ("10 == %d\n", int_unbox (c->first));
  printf ("20 == %d\n", int_unbox (c->next->first));
  printf ("30 == %d\n", int_unbox (c->next->next->first));
  printf ("40 == %d\n", int_unbox (c->next->next->next->first));
  printf ("--------------------\n");

  return 0;


}
