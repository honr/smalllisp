
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cons.h"
#include "box.h"
#include "htrie.h"

int
main ()
{
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
    f = fopen ("/tmp/dict", "r");
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
