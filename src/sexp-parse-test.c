#include "sexp-parse.h"

#include <stdio.h>
#include <stdlib.h>
#include "box.h"

int
main ()
{

  {
    struct cons *c = NULL;
    c = cons_alloc (string_alloc ("hello"), c);
    c = cons_alloc (string_alloc ("world"), c);
    c = cons_alloc (int_alloc (10), c);
    c = cons_alloc (int_alloc (42), c);
    c = cons_alloc (cons_alloc (c, &Q_CONS), c);

    // cons_introspect (string_alloc ("hoy"), 1, 1);
  }
  printf ("sexp-parse-str rudimentary:\n");
  {
    struct bin *symbols = NULL;
    sexp_parse_str (&symbols, "a b c\n");
    sexp_parse_str (&symbols, "hello world "
		    ":keyword "
		    "\"string \\\"escaped\\\" ; \" "
		    ";; comment\n" "not-comment\n" "end\n");
    sexp_parse_str (&symbols, "(a b c) "
		    "( A B C ( D ) ) \n"
		    "xy(a(b ( c)d)) (e )f\"foo()\"(bar\"11\"bax)");

    // sexp_parse_str ("foo\"bar\"baz\"quad\"(+ 10 20 x y +)\n");
    sexp_parse_str (&symbols, "\"foo\"foo\"bar\"baz\"quad\"(+ 10 20 x y)\n");
  }
  printf ("--------------------\n");

  printf ("sexp-parse-str 2.1 String:\n");
  {
    struct bin *symbols = NULL;
    char *s = "\"hello string\"";
    printf ("parse %s:\n", s);
    struct cons *root = sexp_parse_str (&symbols, s);
    cons_introspect (root, 1, 0);
  }
  printf ("--------------------\n");

  printf ("sexp-parse-str 2.2 Number:\n");
  {
    struct bin *symbols = NULL;
    char *s = "4242 1024 0x10 3x456";
    printf ("parse %s:\n", s);
    struct cons *root = sexp_parse_str (&symbols, s);
    cons_introspect (root, 1, 0);
  }
  printf ("--------------------\n");

  printf ("sexp-parse-str 2.3 Symbol:\n");
  {
    struct bin *symbols = NULL;
    char *s = "some-symbol or the other";
    printf ("parse %s:\n", s);
    struct cons *root = sexp_parse_str (&symbols, s);
    cons_introspect (root, 1, 0);
  }
  printf ("--------------------\n");

  printf ("sexp-parse-str 2.4.1 Cons:\n");
  {
    struct bin *symbols = NULL;
    char *s = "(a b c (e f g) d) x (y) (+ 1 2 3)";
    printf ("parse %s:\n", s);
    struct cons *root = sexp_parse_str (&symbols, s);
    cons_introspect (root, 1, 0);
  }
  printf ("--------------------\n");

  return 0;


}
