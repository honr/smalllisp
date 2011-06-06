#include "interp.h"

#include <stdio.h>
#include <stdlib.h>
#include "cons.h"
#include "box.h"
#include "specials.h"

struct context *ctx;

struct cons *
_test (char *title, char *body)
{
  // *foo -> section
  // .foo -> subsection
  // #foo -> introspected section
  // +foo  -> introspected subsection
  static int section = 1;
  static int subsection = 1;
  static char *section_name = "";
  if ((*title == '*') || (*title == '#'))
    {
      section_name = title + 1;
      subsection = 0;
      if (section > 1)
	{
	  printf (ASCIIRED ("----------------------------------------"
			    "----------------------------------------") "\n");
	}
      printf (ASCIIRED ("%d.%-3d %s:") "\n",
	      ++section, subsection, section_name);

    }
  else
    {
      printf (ASCIIMAGENTA ("- - - - - - - - - - - - - - - - - - - -") "\n");
      printf (ASCIIMAGENTA ("%d.%-3d %s/%s:") "\n",
	      section, ++subsection, section_name, title + 1);
    }

  printf ("%s\n", body);
  struct cons *root = sexp_parse_str (&ctx->symbols, body);
  printf (ASCIIMAGENTA ("=>") " ");
  struct cons *result = interp (root);
  if ((title[0] == '#') || (title[0] == '+'))
    {
      cons_introspect (result, 0, 1);
    }
  printf ("\n");

  return result;
}

int
main ()
{
  context_init_default ();
  _test ("#add", "(+ 3x001 (- 3xABC 3x111))");
  _test ("+add", "(+ 0 3x001)");
  _test ("+add", "(+ 1.0 (+ 4 5) 2 3.0)");
  _test ("+mult rgb", "(* 3x123 -1)");
  _test ("+divide rgb", "(+ (/ 3x0188FF 2) (/ 3x0188FF 2))");
  _test ("*println", "(println 10 20 30 3xABC)");
  _test (".str", "(println (str 1 2 3 \"ab c\" 3x445 1.0))");
  _test (".spth", "(println (spth *home* \"Desktop\"))");

  _test ("*if",
	 "(println (if nil 2 3) \".\" (if 0 \"0->true\" \"0->false\"))");
  _test (".if", "(println (if nil 2 3 4))");
  _test (".if-not", "(println (if-not nil 2 3 4))");
  _test (".when", "(println (when nil 2 3 4))");
  _test (".when-not", "(println (when-not nil 2 3 4))");

  // broken
  _test ("*quote", "(println (quote x))");
  _test (".", "(println (quote nil))");

  _test ("*first", "(println (first (list 1 2 3)))");
  _test (".rest", "(println (str (rest (list 1 2 3))))");
  _test (".cons", "(println (str (cons 10 (list 1 2 3))))");
  _test (".cons", "(println (cons 1 (cons 2 nil)))");

  _test ("*range", "(println (str (range 10)))");
  _test (".", "(println (str (range 10.0)))");
  _test (".", "(println (str (range -10)))");
  _test (".", "(println (str (range -10.0)))");

  _test (".", "(println (str (range 10 20)))");
  _test (".", "(println (str (range 10.0 20)))");
  _test (".", "(println (str (range 10 20.0)))");
  _test (".", "(println (str (range 10.0 20.0)))");

  _test (".", "(println (str (range 10 -20)))");
  _test (".", "(println (str (range 10.0 -20)))");
  _test (".", "(println (str (range 10 -20.0)))");
  _test (".", "(println (str (range 10.0 -20.0)))");

  _test (".", "(println (str (range 10 20 2)))");
  _test (".", "(println (str (range 10.0 20 2)))");
  _test (".", "(println (str (range 10 20.0 2)))");
  _test (".", "(println (str (range 10.0 20.0 2)))");
  _test (".", "(println (str (range 10 20 2.0)))");
  _test (".", "(println (str (range 10.0 20 2.0)))");
  _test (".", "(println (str (range 10 20.0 2.0)))");
  _test (".", "(println (str (range 10.0 20.0 2.0)))");

  _test (".", "(println (str (range 20 10 -2)))");
  _test (".", "(println (str (range 20.0 10 -2)))");
  _test (".", "(println (str (range 20 10.0 -2)))");
  _test (".", "(println (str (range 20.0 10.0 -2)))");
  _test (".", "(println (str (range 20 10 -2.0)))");
  _test (".", "(println (str (range 20.0 10 -2.0)))");
  _test (".", "(println (str (range 20 10.0 -2.0)))");
  _test (".", "(println (str (range 20.0 10.0 -2.0)))");

  _test ("*not", "(println (if (not (not nil)) 1 2))");
  _test (".", "(println true nil (not nil) (not true))");
  _test (".", "(println true nil (not nil) (not true))");

  _test ("*let", "(println (let (x 10 y 20) (- 1.0 x y 3.0)))");
  _test (".def", "(def x 18) (println x (def x 20) x)");
  _test (".fn", "(def f (fn (x) (- x 1))) (println (f 20))");
  _test (".fn",
	 "(let (x 90) (def f (fn (x) (- x 1)))) (println (let (x 30) (f 20)))");
  _test (".fn",
	 "(def fact (fn (n) (if (= n 1) 1 (* n (fact (- n 1)))))) (println (fact 1) (fact 2) (fact 3))");

  _test ("*equality",
	 "(println (= nil) (= nil nil) (= 1) (= 1 1) (= 1 1.0) (let (n 1) (= 1 n)))");
  _test (".strings",
	 "(println (= \"hello\") (= \"abcd\" \"ab\") (= \"\" \"xyz\") (= \"abc\" nil))");

  _test (".greater than",
	 "(println (> 1 2) (> 2 1) (> 1 1) (> 2 1.0) (> 2.0 1) (> 2.0 1.0))");
  _test (".less than",
	 "(println (< 1 2) (< 2 1) (< 1 1) (< 2 1.0) (< 2.0 1) (< 2.0 1.0))");
  _test (".greater than or equal",
	 "(println (>= 1 2) (>= 2 1) (>= 1 1) (>= 2 1.0) (>= 2.0 1) (>= 2.0 1.0))");
  _test (".less than or equal",
	 "(println (<= 1 2) (<= 2 1) (<= 1 1) (<= 2 1.0) (<= 2.0 1) (<= 2.0 1.0))");

  return 0;
}
