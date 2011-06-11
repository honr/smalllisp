
#include <stdio.h>
#include <stdlib.h>
#include "box.h"

int
main ()
{

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
  }

  printf ("cons-alloc2:\n");
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

  printf ("htrie-alloc:\n");
  {
    struct bin *b = NULL;
    htrie_assoc (&b, "abc", int_alloc (10));
    htrie_assoc (&b, "abcd", int_alloc (15));
    htrie_assoc (&b, "abb", int_alloc (20));
    htrie_assoc (&b, "aba", int_alloc (30));
    htrie_assoc (&b, "ddd", int_alloc (40));
    htrie_assoc (&b, "eff", int_alloc (50));

    printf ("get abc -> %d\n", int_unbox (htrie_get (b, "abc")));
    printf ("get abcd -> %d\n", int_unbox (htrie_get (b, "abcd")));
    printf ("get abb -> %d\n", int_unbox (htrie_get (b, "abb")));
    printf ("get aba -> %d\n", int_unbox (htrie_get (b, "aba")));
    printf ("get ddd -> %d\n", int_unbox (htrie_get (b, "ddd")));
    printf ("get eff -> %d\n", int_unbox (htrie_get (b, "eff")));
    printf ("get abde -> %p\n", htrie_get (b, "abde"));
  }
  printf ("--------------------\n");
  printf ("htrie dict test:\n");
  {
    FILE *f;
    char line_storage[1024];
    int i = 0;
    struct bin *b = NULL;
    f = fopen ("/usr/share/dict/american-english", "r");
    while (fgets (line_storage, 1024, f))
      {
	line_storage[strlen (line_storage) - 1] = 0;	// remove trailing \n.
	printf ("\"%s\"\n", line_storage);
	htrie_assoc (&b, line_storage, int_alloc (i++));
      }
    fclose (f);
    printf ("get A's -> %d\n", int_unbox (htrie_get (b, "A's")));
    printf ("get zoom -> %d\n", int_unbox (htrie_get (b, "zoom")));
  }
  printf ("--------------------\n");


  return 0;


}
