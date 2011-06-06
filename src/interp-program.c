#include "interp-program.h"
#include <error.h>

struct context *ctx;

int
main (int argc, char *argv[])
{
  context_init_default ();
  char *body = "";
  if (argc > 1)
    {
      if (strcmp (argv[1], "-e") == 0)
	{
	  body = argv[2];
	}
      else
	{
	  body = malloc (2 * 4096);
	  FILE *in_file;
	  if (!(in_file = fopen (argv[1], "r")))
	    {
	      perror ("Error opening input file");
	      exit (1);
	    }
	  fread (body, 2, 4096, in_file);
	  if (!feof (in_file))
	    {
	      fprintf (stderr,
		       "Error: file larger than current maximum size (8192).\n");
	      exit (1);
	    }
	}
      struct cons *result = interp (sexp_parse_str (&ctx->symbols, body));

      if (box_type (result) == &Q_z1)
	{
	  return *((char *) result->first);
	}
      else
	{
	  return 0;
	}
    }
  else
    {
      fprintf (stderr, "Interactive mode not implemented yet.\n");
      return 0;
    }
}
