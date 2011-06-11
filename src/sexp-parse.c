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

#define ACTION_HOLD   0x00	// hold onto current char.
#define ACTION_PASS   0x01	// skip over current char.
#define ACTION_FLUSH  0x02	// flush previous chunk into bufout
#define ACTION_BEGIN  0x04	// start the chunk.
#define ACTION_TERM   0x08	// terminate the chunk.
#define ACTION_PAREN  0x10	// paren
#define ACTION_PREFIX 0x20	// paren
#define ACTION_UP     0x80	// paren/... up

#define TOKEN_STRING  'S'
#define TOKEN_SYMBOL  'Y'
#define TOKEN_NUMBER  'N'
#define TOKEN_NUMBER_MAYBE  'M'
// could also be '(', ')'.

inline void
_sexp_parse__open (struct cons **stackp, struct bin **p_symbols,
		   char specifier)
{
  *stackp = cons_alloc (cons_alloc (NULL, NULL), *stackp);
  if (specifier == '[')
    {
      cons_insert_tail ((*stackp)->first, string_alloc ("vector"));
    }
  else if (specifier == '{')
    {
      cons_insert_tail ((*stackp)->first, string_alloc ("hash-map"));
    }
  else if (specifier == '\'')
    {
      cons_insert_tail ((*stackp)->first, symbol_alloc (p_symbols, "quote"));
    }
  else if (specifier == '`')
    {
      cons_insert_tail ((*stackp)->first,
			symbol_alloc (p_symbols, "quote-eval"));
    }
  else if (specifier == '~')
    {
      cons_insert_tail ((*stackp)->first,
			symbol_alloc (p_symbols, "unquote"));
    }
}

inline void
_sexp_parse__close (struct cons **stackp, struct bin **p_symbols)
{
  struct cons *stack = *stackp;
  struct cons *c = cons_alloc (((struct cons *) stack->first)->first,
			       &Q_CONS);
  free (stack->first);
  *stackp = cons_pop (stack);
  cons_insert_tail ((*stackp)->first, c);
}

inline void
_sexp_parse__token (struct cons **stackp, struct bin **p_symbols,
		    char specifier, char *token)
{
  if (specifier == TOKEN_SYMBOL)
    {
      cons_insert_tail ((*stackp)->first, symbol_alloc (p_symbols, token));
    }
  else if (specifier == TOKEN_STRING)
    {
      cons_insert_tail ((*stackp)->first, string_alloc (token));
    }
  else if (specifier == TOKEN_NUMBER)
    {
      cons_insert_tail ((*stackp)->first, number_alloc (token));
      free (token);
    }
  else
    {
      fprintf (stderr, "%c:%s, WTF?!\n", specifier, token);
    }
}

struct longstack {
  struct longstack* next;
  union {
    long z8;
    double y8;
    char ch[8];
    void* pvoid;
  } val;
};

inline struct longstack* 
longstack_alloc (long val, struct longstack* next)
{
  struct longstack *c = malloc (sizeof (struct longstack));
  c->val.z8 = val;
  c->next = next;
  return c;
}

inline struct longstack *
longstack_pop (struct longstack *c)
{
  // frees the first cell, returns the rest.
  struct longstack *r;
  if (c)
    {
      r = c->next;
      free (c);
      return r;
    }
  return NULL;
}

struct cons *
sexp_parse_str (struct bin **p_symbols, char *buf)
{
  struct cons *stack = cons_alloc (cons_alloc (NULL, NULL), NULL);
  char c, *buf_cur, *bufout_cur, *bufout = malloc (BUFF_SIZE);
  char state, action, specifier = 0, specifier_next, paren_type;
  struct longstack *parenstack, *prefixstack;
  for (buf_cur = buf, bufout_cur = bufout,	// set buffer cursors
       state = STATE_SPACE,	// initial state: whitespace
       parenstack = NULL,       // start with empty stack.
       prefixstack = NULL,    // stack to track number of prefixes
       c = ' ';			// anything other than 0.
       c;			// break after character 0 is sighted.
       ++buf_cur)
    {
      c = *buf_cur;
      action = 0;

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
		  fprintf (stderr,
			   "Error: tried to extend quotation past EOF.\n");
		  return NULL;
		}
	      else
		{
		  action = ACTION_PASS | ACTION_FLUSH | ACTION_TERM;
		}
	    }
	  // how about STATE_COMMENT ?

	  state = STATE_SPACE;
	}
      else if (state & STATE_COMMENT)
	{
	  action = ACTION_PASS;
	  if ((c == '\n') || (c == '\r'))
	    {
	      state = STATE_SPACE;
	    }
	}
      else if (state & STATE_ESCAPE)	// only in string
	{
	  action = ACTION_HOLD;
	  state ^= STATE_ESCAPE;
	}
      else if (state & STATE_STRING)
	{
	  if (c == '"')
	    {
	      action = ACTION_FLUSH | ACTION_TERM | ACTION_PASS;
	      state = STATE_SPACE;
	    }
	  else if (c == '\\')
	    {
	      action = ACTION_FLUSH | ACTION_PASS;
	      state |= STATE_ESCAPE;
	    }
	  else
	    {
	      action = ACTION_HOLD;
	    }
	}
      else if (c == ';')
	{
	  action = ACTION_PASS;
	  if (state & STATE_TOKEN)
	    {
	      action |= ACTION_FLUSH | ACTION_TERM;
	    }
	  state = STATE_COMMENT;
	}
      else if (c == '"')
	{
	  action = ACTION_PASS | ACTION_BEGIN;
	  specifier_next = TOKEN_STRING;
	  if (state & STATE_TOKEN)
	    {
	      action |= ACTION_FLUSH | ACTION_TERM;
	    }
	  state = STATE_TOKEN | STATE_STRING;
	}
      else if ((c == '(') || (c == '[') || (c == '{'))
	{
	  action = ACTION_PASS | ACTION_PAREN | ACTION_UP;
	  if (state & STATE_TOKEN)
	    {
	      action |= ACTION_FLUSH | ACTION_TERM;
	    }
	  paren_type = c;
	  state = STATE_SPACE;
	}
      else if ((c == ')') || (c == ']') || (c == '}'))
	{
	  action = ACTION_PASS | ACTION_PAREN;
	  if (state & STATE_TOKEN)
	    {
	      action |= ACTION_FLUSH | ACTION_TERM;
	    }
	  // paren_type = c;
	  if (c == ')')
	    { paren_type = '('; }
	  else if (c == ']')
	    { paren_type = '['; }
	  else if (c == '}')
	    { paren_type = '{'; }
	  else
	    { fprintf (stderr, "Error: unexpected paren type: %c\n", paren_type);
	      exit (4); }
	  state = STATE_SPACE;
	}
      /* else if ((c == '\'') || (c == '`') || (c == '~'))
	{
	  action = ACTION_PASS | ACTION_PAREN | ACTION_UP | ACTION_PREFIX;
	  if (state & STATE_TOKEN)
	    {
	      action |= ACTION_FLUSH | ACTION_TERM;
	    }
	  paren_type = c;
	  state = STATE_SPACE;
	  }*/
      else
	{
	  if ((c == ',') || isspace (c))
	    {
	      action = ACTION_PASS;
	      if (state & STATE_TOKEN)
		{
		  action |= ACTION_FLUSH | ACTION_TERM;
		  state = STATE_SPACE;
		}
	    }
	  else
	    {
	      action = ACTION_HOLD;
	      if (state == STATE_SPACE)
		{
		  state = STATE_TOKEN;
		  action |= ACTION_BEGIN;
		  if (isdigit (c))
		    {
		      specifier_next = TOKEN_NUMBER;
		    }
		  else if ((c == '.') || (c == '-') || (c == '+'))
		    {
		      specifier_next = TOKEN_NUMBER_MAYBE;
		    }
		  else
		    {
		      specifier_next = TOKEN_SYMBOL;
		    }
		}
	    }
	}

      if (action & ACTION_FLUSH)	// copy or flush
	{
	  memcpy (bufout_cur, buf, buf_cur - buf);
	  bufout_cur += buf_cur - buf;
	  buf = buf_cur;
	}
      if (action & ACTION_TERM)
	{
	  // extract and copy current token from bufout.
	  int s_len = bufout_cur - bufout;
	  char *s = malloc (s_len + 1);
	  memcpy (s, bufout, s_len);
	  s[s_len] = 0;
	  bufout_cur = bufout;	// move cursor to the beginning

	  if (specifier == TOKEN_NUMBER_MAYBE)
	    {
	      if (((s[0] == '.') && (isdigit (s[1]))) ||
		  (((s[0] == '-') || (s[0] == '+')) &&
		   (isdigit (s[1]) || ((s[1] == '.') && (isdigit (s[2]))))))
		{
		  specifier = TOKEN_NUMBER;
		}
	      else
		{
		  specifier = TOKEN_SYMBOL;
		}
	    }

	  _sexp_parse__token (&stack, p_symbols, specifier, s);
	  // _sexp_parse__close (&stack, p_symbols);
	}
      if (action & ACTION_BEGIN)
	{
	  specifier = specifier_next;
	}
      if (action & ACTION_PASS)
	{
	  buf++;
	}
      if (action & ACTION_PAREN)
	{
	  if (action & ACTION_UP)
	    {
	      _sexp_parse__open (&stack, p_symbols, paren_type);
	      parenstack = longstack_alloc ((long) paren_type, parenstack);
	    }
	  else
	    {
	      _sexp_parse__close (&stack, p_symbols);
	      if ((char) parenstack->val.z8 != paren_type)
		{
		  fprintf (stderr, "Error: mismatched parenthesis %c vs %c\n",
			   (char) parenstack->val.z8, paren_type);
		  exit (1);
		}
	      parenstack = longstack_pop (parenstack);
	    }
	}
    }

  free (bufout);
  struct cons *root = ((struct cons *) stack->first)->first;
  free (stack->first);
  free (stack);
  return root;
}
