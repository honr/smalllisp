#include "sexp-parse.h"
#include "box.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// debug
#include <stdio.h>

#define BUFF_SIZE 4096

#define STATE_SPACE   0x00
#define STATE_TOKEN   0x01
#define STATE_STRING  0x02
#define STATE_ESCAPE  0x04
#define STATE_COMMENT 0x08
// maybe comment = space | escape ?

#define TOKEN_STRING  'S'
#define TOKEN_SYMBOL  'Y'

#define PAREN_FRAGILE  0x0100
// paren is fragile

struct cons *
sexp_parse_str (struct bin **p_symbols, char *buf)
{
  struct cons *stack = cons_alloc (cons_alloc (NULL, NULL), NULL);
  char c, *buf_cur, *bufout_cur, *bufout = malloc (BUFF_SIZE);
  char state, specifier = 0, specifier_next, paren_type;
  struct cons *parenstack;

  inline void action_pass ()
  {
    ++buf;
  }

  inline void action_begin ()
  {
    specifier = specifier_next;
  }

  inline void action_token_flush ()	// copy or flush
  {
    memcpy (bufout_cur, buf, buf_cur - buf);
    bufout_cur += buf_cur - buf;
    buf = buf_cur;
  }

  inline void action_paren_open ()
  {
    int paren_fragilep = 0;
    stack = cons_alloc (cons_alloc (NULL, NULL), stack);
    if (paren_type == '[')
      {
	cons_insert_tail (stack->first.c, symbol_alloc (p_symbols, "vector"));
      }
    else if (paren_type == '{')
      {
	cons_insert_tail (stack->first.c,
			  symbol_alloc (p_symbols, "hash-map"));
      }
    else if (paren_type == '\'')
      {
	paren_fragilep = PAREN_FRAGILE;
	paren_type = '(';
	cons_insert_tail (stack->first.c, symbol_alloc (p_symbols, "quote"));
      }
    else if (paren_type == '`')
      {
	paren_fragilep = PAREN_FRAGILE;
	paren_type = '(';
	cons_insert_tail (stack->first.c,
			  symbol_alloc (p_symbols, "quote-eval"));
      }
    else if (paren_type == '~')
      {
	paren_fragilep = PAREN_FRAGILE;
	paren_type = '(';
	cons_insert_tail (stack->first.c,
			  symbol_alloc (p_symbols, "unquote"));
      }

    parenstack = cons_alloc (NULL, parenstack);
    parenstack->first.z2 = paren_type | paren_fragilep;
  }

  inline void action_paren_close ()
  {
    // _sexp_parse__close (&stack, p_symbols);
    //
    struct cons *stack_old = stack;
    struct cons *c = cons_alloc (stack_old->first.c->first.p, &Q_CONS);
    free (stack_old->first.p);
    stack = cons_pop (stack_old);
    cons_insert_tail (stack->first.c, c);
    //

    if ((parenstack->first.z2 & 0xFF) != paren_type)
      {
	fprintf (stderr, "Error: mismatched parenthesis %c vs %c\n",
		 parenstack->first.z2 & 0xFF, paren_type);
	exit (1);
      }
    parenstack = cons_pop (parenstack);
  }

  inline void action_prefix_close ()
  {
    while (parenstack->first.z2 & PAREN_FRAGILE)
      {
	action_paren_close ();
      }
  }

  inline void action_token_terminate ()
  {
    action_token_flush ();

    // extract and copy current token from bufout.
    int token_len = bufout_cur - bufout;
    char *token = malloc (token_len + 1);
    memcpy (token, bufout, token_len);
    token[token_len] = 0;
    bufout_cur = bufout;	// move cursor to the beginning

    if (specifier == TOKEN_SYMBOL)
      {
	if (isdigit (token[0]) ||
	    ((token[0] == '.') && (isdigit (token[1]))) ||
	    (((token[0] == '-') || (token[0] == '+')) &&
	     (isdigit (token[1]) || ((token[1] == '.') &&
				     (isdigit (token[2]))))))
	  {
	    cons_insert_tail (stack->first.c, number_alloc (token));
	    free (token);
	  }
	else
	  {
	    cons_insert_tail (stack->first.c,
			      symbol_alloc (p_symbols, token));
	  }
      }
    else if (specifier == TOKEN_STRING)
      {
	cons_insert_tail (stack->first.c, string_alloc (token));
      }
    else
      {
	fprintf (stderr, "%c:%s, WTF?!\n", specifier, token);
      }

    action_prefix_close ();
  }

  for (buf_cur = buf, bufout_cur = bufout,	// set buffer cursors
       state = STATE_SPACE,	// initial state: whitespace
       parenstack = NULL,	// start with empty stack.
       c = ' ';			// anything other than 0.
       c;			// break after character 0 is sighted.
       ++buf_cur)
    {
      c = *buf_cur;

      // State Transitions:
      if (c == 0)
	{
	  if (parenstack)
	    {
	      fprintf (stderr, "Error: EOF while reading.\n");
	      return NULL;
	    }
	  else if (state & STATE_TOKEN)
	    {
	      if (state & STATE_STRING)
		{
		  fprintf (stderr, "Error: reached EOF while in a string.\n");
		  return NULL;
		}
	      else
		{
		  action_token_terminate ();
		  action_pass ();
		}
	    }
	  // how about STATE_COMMENT ?

	  state = STATE_SPACE;
	}
      else if (state & STATE_COMMENT)
	{
	  action_pass ();
	  if ((c == '\n') || (c == '\r'))
	    {
	      state = STATE_SPACE;
	    }
	}
      else if (state & STATE_ESCAPE)	// only in string
	{
	  // action_hold (); // do nothing!
	  state ^= STATE_ESCAPE;
	}
      else if (state & STATE_STRING)
	{
	  if (c == '"')
	    {
	      action_token_terminate ();
	      action_pass ();
	      state = STATE_SPACE;
	    }
	  else if (c == '\\')
	    {
	      action_token_flush ();
	      action_pass ();
	      state |= STATE_ESCAPE;
	    }
	  else
	    {
	      // action_hold (); // do nothing
	    }
	}
      else if (c == ';')
	{
	  if (state & STATE_TOKEN)
	    {
	      action_token_terminate ();
	    }
	  action_pass ();
	  state = STATE_COMMENT;
	}
      else if (c == '"')
	{
	  specifier_next = TOKEN_STRING;
	  if (state & STATE_TOKEN)
	    {
	      action_token_terminate ();
	    }
	  action_begin ();
	  action_pass ();

	  state = STATE_TOKEN | STATE_STRING;
	}
      else if ((c == '(') || (c == '[') || (c == '{') ||
	       (c == '\'') || (c == '`') || (c == '~'))
	{
	  if (state & STATE_TOKEN)
	    {
	      action_token_terminate ();
	    }
	  paren_type = c;
	  action_pass ();
	  action_paren_open ();
	  state = STATE_SPACE;
	}
      else if ((c == ')') || (c == ']') || (c == '}'))
	{
	  if (state & STATE_TOKEN)
	    {
	      action_token_terminate ();
	    }
	  // paren_type = c;
	  if (c == ')')
	    {
	      paren_type = '(';
	    }
	  else if (c == ']')
	    {
	      paren_type = '[';
	    }
	  else if (c == '}')
	    {
	      paren_type = '{';
	    }
	  else
	    {
	      fprintf (stderr, "Error: unexpected paren type: %c\n",
		       paren_type);
	      exit (4);
	    }
	  action_pass ();
	  action_prefix_close ();
	  action_paren_close ();
	  state = STATE_SPACE;
	}
      else
	{
	  if ((c == ',') || isspace (c))
	    {
	      if (state & STATE_TOKEN)
		{
		  action_token_terminate ();
		  state = STATE_SPACE;
		}
	      action_pass ();
	    }
	  else
	    {
	      if (state == STATE_SPACE)
		{
		  state = STATE_TOKEN;
		  specifier_next = TOKEN_SYMBOL;
		  // we will later check if it is a number.
		  action_begin ();
		}
	    }
	}
    }

  free (bufout);
  struct cons *root = stack->first.c->first.p;
  free (stack->first.p);
  free (stack);
  return root;
}
