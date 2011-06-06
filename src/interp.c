#include "interp.h"
#include "cons.h"
#include "box.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// debug
#include <stdio.h>

struct cons *
fn_alloc (struct cons *params, struct cons *body)
{
  struct fn_interp *result = malloc (sizeof (struct fn_interp));
  result->params = params;
  result->body = body;
  return cons_alloc (result, &Q_LAMBDA_INTERP);
}

inline struct cons *
symbol_push (struct symbol *sym, struct cons *value)
{
  sym->vals = cons_alloc (value, sym->vals);
  ctx->binding_stack->first = cons_alloc (sym, ctx->binding_stack->first);
  // TODOD: we better return the symbol, though.
  return value;
}

inline void
symbol_pop (struct symbol *sym)
{
  struct cons *val = sym->vals->first;
  sym->vals = sym->vals->next;
  (void) val;
  // free val.
}

/* inline struct cons * */
/* symbol_define_sym (struct cons **sym, struct cons *value) */
/* { */
/*   *sym = cons_alloc (value, *sym); */
/*   ctx->binding_stack->first = cons_alloc (sym, ctx->binding_stack->first); */
/*   // TODOD: we better return the symbol, though. */
/*   return value; */
/* } */

void
symbol_define (char *key, struct cons *value)
{
  struct symbol *sym = htrie_get (ctx->symbols, key);
  if (!sym)
    {
      sym = malloc (sizeof (struct symbol));
      sym->name = key;
      sym->vals = cons_alloc (value, NULL);
      sym->parent = NULL;	// should be *ns*.
      htrie_assoc (&ctx->symbols, strdup (key), sym);
    }
  symbol_push (sym, value);
}

void
binding_stack_push ()
{
  ctx->binding_stack = cons_alloc (NULL, ctx->binding_stack);
}

void
binding_stack_pop ()
{
  struct cons *c;
  for (c = ctx->binding_stack->first; c; c = c->next)
    {
      symbol_pop (c->first);
    }
  ctx->binding_stack = cons_pop (ctx->binding_stack);
}

struct cons *
interp (struct cons *forms)
{
  struct cons *result = NULL;
  result = interp_eval_progn (forms);
  return result;
}

inline struct cons *
interp_eval_progn (struct cons *forms)
{
  struct cons *result;
  for (result = NULL; forms; forms = forms->next)
    {
      result = interp_eval_box (forms->first);
    }
  return result;
}

inline struct cons *
interp_eval_cons (struct cons *forms)
{
  struct cons *curs;
  for (curs = cons_alloc (NULL, NULL); forms; forms = forms->next)
    {
      cons_insert_tail (curs, interp_eval_box (forms->first));
    }
  struct cons *result = curs->first;
  free (curs);
  return result;
}

inline struct cons *
interp_call_evaledparams (struct cons *f, struct cons *params)
{				// broken.
  // TODO: distinguish between macros and functions.
  //       in particular, indicate when to pass evaluated params
  //       and when to send raw params.
  struct cons *result;
  if (!f)
    {
      printf ("Error: tried to eval nil\n");
      return NULL;
    }

  struct cons *specifier = f->next;
  if (specifier == &Q_FUNCTION_INTERP)
    {
      struct cons *(*f0) (struct cons * params);
      f0 = f->first;
      result = (*f0) (params);
    }
  else if (specifier == &Q_LAMBDA_INTERP)
    {
      binding_stack_push ();
      struct fn_interp *l = f->first;
      struct cons *keys;
      for (keys = l->params;
	   keys && params; keys = keys->next, params = params->next)
	{			// key must be a symbol, for now.
	  symbol_push (symbol_unbox (keys->first), params->first);
	}
      if (keys || params)
	{
	  fprintf (stderr, "Error: wrong number of arguments to fn.\n");
	}

      result = interp_eval_progn (l->body);
      binding_stack_pop ();
    }
  else
    {
      printf ("Error: tried to eval with type %s\n",
	      (char *) specifier->first);
    }
  return result;
}

inline struct cons *
interp_call (struct cons *f, struct cons *params)
{				// broken.
  // TODO: distinguish between macros and functions.
  //       in particular, indicate when to pass evaluated params
  //       and when to send raw params.
  struct cons *result;
  if (!f)
    {
      printf ("Error: tried to eval nil\n");
      return NULL;
    }

  struct cons *specifier = f->next;
  if (specifier == &Q_MACRO_INTERP)
    {
      struct cons *(*f0) (struct cons * params);
      f0 = f->first;
      result = (*f0) (params);
    }
  else if (specifier == &Q_FUNCTION_INTERP)
    {
      struct cons *(*f0) (struct cons * params);
      f0 = f->first;
      result = (*f0) (interp_eval_cons (params));
    }
  else if (specifier == &Q_LAMBDA_INTERP)
    {
      binding_stack_push ();
      struct fn_interp *l = f->first;
      struct cons *keys;
      for (keys = l->params;
	   keys && params; keys = keys->next, params = params->next)
	{			// key must be a symbol, for now.
	  symbol_push (symbol_unbox (keys->first),
		       interp_eval_box (params->first));
	}
      if (keys || params)
	{
	  fprintf (stderr, "Error: wrong number of arguments to fn.\n");
	}

      result = interp_eval_progn (l->body);
      binding_stack_pop ();
    }
  else
    {
      printf ("Error: tried to eval with type %s\n",
	      (char *) specifier->first);
    }
  return result;
}

struct cons *
interp_eval_box (struct cons *form)
{
  struct cons *result = NULL;
  if (!form)
    {
      result = NULL;
    }
  else
    {
      struct cons *specifier = form->next;
      if (specifier == &Q_CONS)
	{
	  struct cons *c = form->first;
	  if (!c)
	    {
	      result = NULL;
	    }
	  else
	    {
	      // struct cons *c_first = interp_eval_box (c->first);
	      result = interp_call (interp_eval_box (c->first), c->next);
	    }
	}
      else if (specifier == &Q_SYMBOL)
	{
	  struct symbol *sym = form->first;
	  if (sym->vals)
	    {
	      result = sym->vals->first;
	    }
	  else
	    {
	      fprintf (stderr,
		       "Error: Unable to resolve symbol `%s' in this context.\n",
		       sym->name);
	      exit (1);
	      // return NULL;
	    }
	}
      else if ((numberp (form)) ||
	       (specifier == &Q_RGB3) ||
	       (specifier == &Q_STRING) || (specifier == &Q_KEYWORD))
	{
	  result = form;
	}
      else			// should not happen
	{
	  fprintf (stderr, "Error: box of unknown type.\n");
	  return form;
	}
    }
  return result;
}
