#include "specials.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define BUFF_SIZE 4096		// for _special_str
#define MAX_NUMBER_STR_SIZE 256

struct cons *
_special_list (struct cons *params)
{
  return cons_alloc (params, &Q_CONS);
}


struct cons *
_special_inc (struct cons *params)
{				/* Only supports long and double for now. */
  struct cons *item = params->first;
  if (item->next == &Q_z8)
    {
      return long_alloc (long_unbox (item) + 1);
    }
  else if (item->next == &Q_y8)
    {
      return double_alloc (double_unbox (item) + 1.0);
    }
  else
    {
      fprintf (stderr, "incompatible/unimplemented item type\n");
      return NULL;
    }
}

struct cons *
_special_dec (struct cons *params)
{				/* Only supports long and double for now. */
  struct cons *item = params->first;
  if (item->next == &Q_z8)
    {
      return long_alloc (long_unbox (item) - 1);
    }
  else if (item->next == &Q_y8)
    {
      return double_alloc (double_unbox (item) - 1.0);
    }
  else
    {
      fprintf (stderr, "incompatible/unimplemented item type\n");
      return NULL;
    }
}

struct cons *
_special_plus (struct cons *params)
{				// fn.
  // only supporting long, double and rgb3 for now.
  struct cons *item;
  long result_long = 0;
  double result_double = 0.0;
  struct rgb3 result_rgb = { 0, 0, 0 };
  struct cons *mode = &Q_z8;
  struct cons *specifier;

  for (; params; params = params->next)
    {
      item = params->first;
      // item = interp_eval_box (params->first);
      specifier = item->next;
      if (mode == &Q_y8)
	{
	  if (specifier == &Q_y8)
	    {
	      result_double += double_unbox (item);
	    }
	  else if (specifier == &Q_z8)
	    {
	      result_double += long_unbox (item);
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type\n");
	    }
	}
      else if (mode == &Q_z8)
	{
	  if (specifier == &Q_y8)
	    {
	      result_double = result_long + double_unbox (item);
	      mode = &Q_y8;
	    }
	  else if (specifier == &Q_z8)
	    {
	      result_long += long_unbox (item);
	    }
	  else if (specifier == &Q_RGB3 && !result_long)
	    {
	      result_rgb = rgb_add (result_rgb, rgb_unbox (item));
	      mode = &Q_RGB3;
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type\n");
	    }
	}
      else if (mode == &Q_RGB3)
	{
	  if (specifier == &Q_RGB3)
	    {
	      result_rgb = rgb_add (result_rgb, rgb_unbox (item));
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type\n");
	    }
	}
    }

  if (mode == &Q_y8)
    {
      return double_alloc (result_double);
    }
  else if (mode == &Q_z8)
    {
      return long_alloc (result_long);
    }
  else if (mode == &Q_RGB3)
    {
      return rgb_alloc (result_rgb);
    }
  else
    {
      return NULL;
    }
}


struct cons *
_special_minus (struct cons *params)
{
  // only supporting long, double and rgb3 for now.
  struct cons *item = params->first;
  long result_long = 0;
  double result_double = 0.0;
  struct rgb3 result_rgb = { 0, 0, 0 };
  struct cons *mode = item->next;
  if (mode == &Q_y8)
    {
      result_double = double_unbox (item);
    }
  else if (mode == &Q_z8)
    {
      result_long = long_unbox (item);
    }
  else if (mode == &Q_RGB3)
    {
      result_rgb = rgb_unbox (item);
    }
  params = params->next;
  if (!params)
    {
      if (mode == &Q_y8)
	{
	  return double_alloc (-result_double);
	}
      else if (mode == &Q_z8)
	{
	  return long_alloc (-result_long);
	}
      else if (mode == &Q_RGB3)
	{
	  return rgb_alloc (rgb_inverse (result_rgb));
	}
    }

  for (; params; params = params->next)
    {
      item = params->first;
      if (mode == &Q_y8)
	{
	  if (item->next == &Q_y8)
	    {
	      result_double -= double_unbox (item);
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_double -= long_unbox (item);
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
      else if (mode == &Q_z8)
	{
	  if (item->next == &Q_y8)
	    {
	      result_double = result_long - double_unbox (item);
	      mode = &Q_y8;
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_long -= long_unbox (item);
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
      else if (mode == &Q_RGB3)
	{
	  if (item->next == &Q_RGB3)
	    {
	      result_rgb =
		rgb_add (result_rgb, rgb_inverse (rgb_unbox (item)));
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
    }

  if (mode == &Q_y8)
    {
      return double_alloc (result_double);
    }
  else if (mode == &Q_z8)
    {
      return long_alloc (result_long);
    }
  else if (mode == &Q_RGB3)
    {
      return rgb_alloc (result_rgb);
    }
  else
    {
      return NULL;
    }
}

struct cons *
_special_mult (struct cons *params)
{
  // only supporting long, double and rgb3 for now.
  struct cons *item;
  long result_long = 1;
  double result_double = 1.0;
  struct rgb3 result_rgb = { 0, 0, 0 };
  struct cons *mode = &Q_z8;

  for (; params; params = params->next)
    {
      item = params->first;
      if (mode == &Q_y8)
	{
	  if (item->next == &Q_y8)
	    {
	      result_double *= double_unbox (item);
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_double *= long_unbox (item);
	    }
	  else if (item->next == &Q_RGB3)
	    {
	      result_rgb = rgb_mult (result_double, rgb_unbox (item));
	      mode = &Q_RGB3;
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
      else if (mode == &Q_z8)
	{
	  if (item->next == &Q_y8)
	    {
	      result_double = result_long * double_unbox (item);
	      mode = &Q_y8;
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_long *= long_unbox (item);
	    }
	  else if (item->next == &Q_RGB3)
	    {
	      result_rgb = rgb_mult (result_long, rgb_unbox (item));
	      mode = &Q_RGB3;
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
      else if (mode == &Q_RGB3)
	{
	  if (item->next == &Q_RGB3)
	    {
	      result_rgb = rgb_add (result_rgb, rgb_unbox (item));
	    }
	  else if (item->next == &Q_y8)
	    {
	      result_rgb = rgb_mult (double_unbox (item), result_rgb);
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_rgb = rgb_mult (long_unbox (item), result_rgb);
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
    }

  if (mode == &Q_y8)
    {
      return double_alloc (result_double);
    }
  else if (mode == &Q_z8)
    {
      return long_alloc (result_long);
    }
  else if (mode == &Q_RGB3)
    {
      return rgb_alloc (result_rgb);
    }
  else
    {
      return NULL;
    }
}

struct cons *
_special_divide (struct cons *params)
{
  // only supporting long, double and rgb3 for now.
  struct cons *item = params->first;
  long result_long = 1;
  double result_double = 1.0;
  struct rgb3 result_rgb = { 0, 0, 0 };
  struct cons *mode = item->next;
  if (mode == &Q_y8)
    {
      result_double = double_unbox (item);
    }
  else if (mode == &Q_z8)
    {
      result_long = long_unbox (item);
    }
  else if (mode == &Q_RGB3)
    {
      result_rgb = rgb_unbox (item);
    }
  params = params->next;
  if (!params)
    {
      if (mode == &Q_y8)
	{
	  return double_alloc (1.0 / result_double);
	}
      else if (mode == &Q_z8)
	{
	  return long_alloc (1 / result_long);
	}
      else if (mode == &Q_RGB3)
	{
	  return NULL;
	}
    }

  for (; params; params = params->next)
    {
      item = params->first;
      if (mode == &Q_y8)
	{
	  if (item->next == &Q_y8)
	    {
	      result_double /= double_unbox (item);
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_double /= long_unbox (item);
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
      else if (mode == &Q_z8)
	{
	  if (item->next == &Q_y8)
	    {
	      result_double = result_long / double_unbox (item);
	      mode = &Q_y8;
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_long /= long_unbox (item);
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
      else if (mode == &Q_RGB3)
	{
	  if (item->next == &Q_y8)
	    {
	      result_rgb = rgb_mult (1.0 / double_unbox (item), result_rgb);
	    }
	  else if (item->next == &Q_z8)
	    {
	      result_rgb = rgb_mult (1.0 / long_unbox (item), result_rgb);
	    }
	  else
	    {
	      fprintf (stderr, "incompatible/unimplemented item type");
	    }
	}
    }

  if (mode == &Q_y8)
    {
      return double_alloc (result_double);
    }
  else if (mode == &Q_z8)
    {
      return long_alloc (result_long);
    }
  else if (mode == &Q_RGB3)
    {
      return rgb_alloc (result_rgb);
    }
  else
    {
      return NULL;
    }
}

struct cons *
_special_println (struct cons *params)
{
  for (; params; params = params->next)
    {
      box_print (params->first);
      if (params->next)
	{
	  printf (" ");
	}
    }
  printf ("\n");
  return NULL;
}

char *
__special_str__box_to_str (char **p_buff_cur, char *buff_limit,
			   struct cons *c)
{
  if (!c)
    {
      strcat_rf (p_buff_cur, buff_limit, "nil");
    }
  else
    {
      struct cons *specifier = c->next;
      if (specifier == &Q_CONS)
	{
	  strcat_rf (p_buff_cur, buff_limit, "(");
	  struct cons *c_sub;
	  for (c_sub = (struct cons *) c->first; c_sub; c_sub = c_sub->next)
	    {
	      __special_str__box_to_str (p_buff_cur, buff_limit,
					 c_sub->first);
	      if (c_sub->next)
		{
		  strcat_rf (p_buff_cur, buff_limit, " ");
		}
	    }
	  strcat_rf (p_buff_cur, buff_limit, ")");
	}
      else if (specifier == &Q_STRING)
	{
	  strcat_rf (p_buff_cur, buff_limit, string_unbox (c));
	}
      else if (specifier == &Q_SYMBOL)
	{
	  struct symbol *sym = (struct symbol *) c->first;
	  // cons_introspect (*sym, level + 1, 0);
	  strcat_rf (p_buff_cur, buff_limit, "'");
	  strcat_rf (p_buff_cur, buff_limit, sym->name);
	}
      else if (specifier == &Q_KEYWORD)
	{
	  struct symbol *sym = c->first;
	  strcat_rf (p_buff_cur, buff_limit, sym->name);
	}
      else if (numberp (c))
	{
	  if ((buff_limit - *p_buff_cur) > MAX_NUMBER_STR_SIZE)
	    {
	      number_to_str (*p_buff_cur, c);
	      *p_buff_cur = *p_buff_cur + strlen (*p_buff_cur);
	    }
	  else
	    {
	      *p_buff_cur = NULL;
	    }
	}
      else if (specifier == &Q_RGB3)
	{
	  struct rgb3 *x = (struct rgb3 *) c->first;
	  int n;
	  if ((n = snprintf (*p_buff_cur, buff_limit - (*p_buff_cur),
			     "\"#%.2X%.2X%.2X\"", x->r, x->g, x->b)) >= 0)
	    {
	      *p_buff_cur = *p_buff_cur + n;
	    }
	  else
	    {
	      *p_buff_cur = NULL;
	    }
	}
      else
	{
	  fprintf (stderr, "[Unknown Type] %p\n", c->first);
	}
    }
  return *p_buff_cur;
}

struct cons *
_special_str (struct cons *params)
{
  char buff[BUFF_SIZE];
  char *buff_limit = buff + BUFF_SIZE;

  char *buff_cur = buff;
  // buff[0] up to [buff_limit - buff - 1] are valid.

  for (; params; params = params->next)
    {
      __special_str__box_to_str (&buff_cur, buff_limit, params->first);
    }

  if (buff_cur)
    {
      size_t s_len = buff_cur - buff;
      char *s = malloc (s_len + 1);
      memcpy (s, buff, s_len);
      s[s_len] = 0;
      struct cons *result = string_alloc (s);
      return result;
    }
  else
    {
      return NULL;
    }
}

struct cons *
_special_spth (struct cons *params)
{
  char buff[BUFF_SIZE];
  char *buff_limit = buff + BUFF_SIZE;

  char *buff_cur = buff;
  // buff[0] up to [buff_limit - buff - 1] are valid.

  for (; params; params = params->next)
    {
      __special_str__box_to_str (&buff_cur, buff_limit,
				 interp_eval_box (params->first));
      if (params->next)
	{
	  strcat_rf (&buff_cur, buff_limit, "/");
	}
    }

  if (buff_cur)
    {
      size_t s_len = buff_cur - buff;
      char *s = malloc (s_len + 1);
      memcpy (s, buff, s_len);
      s[s_len] = 0;
      struct cons *result = string_alloc (s);
      return result;
    }
  else
    {
      return NULL;
    }
}

struct cons *
_special_if (struct cons *params)
{
  if (interp_eval_box (params->first))
    {
      return interp_eval_box (params->next->first);
    }
  else
    {
      return interp_eval_progn (params->next->next);
    }
}

struct cons *
_special_if_not (struct cons *params)
{
  if (interp_eval_box (params->first))
    {
      return interp_eval_progn (params->next->next);
    }
  else
    {
      return interp_eval_box (params->next->first);
    }
}

struct cons *
_special_when (struct cons *params)
{
  if (interp_eval_box (params->first))
    {
      return interp_eval_progn (params->next);
    }
  else
    {
      return NULL;
    }
}

struct cons *
_special_when_not (struct cons *params)
{
  if (interp_eval_box (params->first))
    {
      return NULL;
    }
  else
    {
      return interp_eval_progn (params->next);
    }
}

struct cons *
_special_quote (struct cons *params)
{
  // TODO: test.
  return params->first;		// (params, &Q_CONS);
}

struct cons *
_special_quote_eval (struct cons *params)
{				// recursively assume quoted unless `unquote' appears, in which case
  // eval normally.
  if (!params->first)
    {
      return NULL;
    }

  if (!consp (params->first))
    {
      return params->first;
    }

  // params->first is a cons
  struct cons *in_cur = cons_unbox (params->first);
  struct cons *in_stack = cons_alloc (in_cur, NULL);
  struct cons *out_stack = cons_alloc (cons_alloc (NULL, NULL), NULL);

  while (in_stack)
    {
      if (in_cur)
	{
	  struct cons *in_cur_first = in_cur->first;

	  if (consp (in_cur_first))
	    {
	      if (!in_cur_first->first)	// empty list
		{
		  // cons_insert_tail (out_stack->first, in_cur_first);
		  cons_insert_tail (out_stack->first, NULL);
		  in_cur = in_cur->next;
		}
	      else
		{
		  struct cons *subcons = cons_unbox (in_cur_first);
		  struct cons *subcons_first = subcons->first;

		  // check if the first element is 'unquote or 'unquote-list
		  if (symbolp (subcons_first) &&
		      (symbol_unbox (subcons_first)->vals) &&
		      (symbol_unbox (subcons_first)->vals->first ==
		       &Q_VAL_UNQUOTE))
		    {
		      cons_insert_tail (out_stack->first,
					interp_eval_box (subcons->
							 next->first));
		      in_cur = in_cur->next;
		    }
		  else if (symbolp (subcons_first) &&
			   (symbol_unbox (subcons_first)->vals) &&
			   (symbol_unbox (subcons_first)->vals->first ==
			    &Q_VAL_UNQUOTE_LIST))
		    {
		      struct cons *q =
			((struct cons *)
			 interp_eval_box (subcons->next->first))->first;
		      for (; q; q = q->next)
			{
			  cons_insert_tail (out_stack->first, q->first);
			}
		      in_cur = in_cur->next;
		    }
		  else
		    {
		      in_stack = cons_alloc (in_cur->next, in_stack);
		      in_cur = subcons;
		      out_stack =
			cons_alloc (cons_alloc (NULL, NULL), out_stack);
		    }
		}
	    }
	  else
	    {
	      cons_insert_tail (out_stack->first, in_cur_first);
	      in_cur = in_cur->next;
	    }
	}
      else			// go back up
	{
	  in_cur = in_stack->first;
	  in_stack = cons_pop (in_stack);
	  if (in_stack)
	    {
	      struct cons *res =
		cons_alloc (((struct cons *) out_stack->first)->first,
			    &Q_CONS);
	      free (out_stack->first);
	      out_stack = cons_pop (out_stack);
	      cons_insert_tail (out_stack->first, res);
	    }
	}
    }
  struct cons *result = ((struct cons *) out_stack->first)->first;
  // cons_introspect (result, 0, 0);
  free (out_stack->first);
  free (out_stack);
  return cons_alloc (result, &Q_CONS);
}

struct cons *
_special_first (struct cons *params)
{
  struct cons *param = params->first;
  if (consp (param))
    {
      return (cons_unbox (param))->first;
    }
  return NULL;
}

struct cons *
_special_rest (struct cons *params)
{
  struct cons *param = params->first;
  if (consp (param))
    {
      return cons_alloc ((cons_unbox (param))->next, &Q_CONS);
    }
  return NULL;
}

struct cons *
_special_nth (struct cons *params)
{
  struct cons *param = params->first;
  if (consp (param))
    {
      long n = long_unbox (params->next->first);
      struct cons *p;
      for (p = cons_unbox (param); p && n; p = p->next, --n);
      if (p)
	{
	  return p->first;
	}
    }
  return NULL;
}

struct cons *
_special_cons (struct cons *params)
{
  // cons_alloc ((cons_alloc (params_parsed->first, params_parsed->next) , &Q_CONS);  // or something like that.
  struct cons *head = params->first;
  struct cons *body = params->next->first;
  if (body)
    {
      if (body->next == &Q_CONS)
	{
	  return cons_alloc (cons_alloc (head, body->first), &Q_CONS);
	}
      else
	{
	  return NULL;
	}
    }
  else
    {
      return cons_alloc (cons_alloc (head, NULL), &Q_CONS);
    }
}

struct cons *
_special_range (struct cons *params)
{
  struct cons *range_to = params->first;
  struct cons *range_from = NULL;
  struct cons *range_step = NULL;

  if (params->next)
    {
      params = params->next;
      range_from = range_to;
      range_to = params->first;
    }
  if (params->next)
    {
      params = params->next;
      range_step = params->first;
    }
  if (params->next)
    {
      fprintf (stderr, "Error: wrong number of arguments passed to `range', "
	       "which only accepts 1, 2, or 3 arguments.\n");
      return NULL;
    }
  struct cons *curs = cons_alloc (NULL, NULL);
  long n;
  long i;
  if (range_step)
    {
      if (range_step->next == &Q_z8)
	{
	  long step = long_unbox (range_step);
	  if (step > 0)
	    {
	      if (range_from->next == &Q_z8)
		{
		  long m = long_unbox (range_from);
		  if (range_to->next == &Q_z8)
		    {
		      n = long_unbox (range_to);
		    }
		  else if (range_to->next == &Q_y8)
		    {
		      n = double_unbox (range_to);
		    }
		  for (i = m; i < n; i += step)
		    {
		      cons_insert_tail (curs, long_alloc (i));
		    }
		}
	      else if (range_from->next == &Q_y8)
		{
		  double m_d = double_unbox (range_from);
		  if (range_to->next == &Q_z8)
		    {
		      n = long_unbox (range_to) - m_d;
		    }
		  else if (range_to->next == &Q_y8)
		    {
		      n = double_unbox (range_to) - m_d;
		    }
		  for (i = 0; i < n; i += step)
		    {
		      cons_insert_tail (curs, double_alloc (i + m_d));
		    }
		}
	    }
	  else if (step < 0)
	    {
	      if (range_from->next == &Q_z8)
		{
		  long m = long_unbox (range_from);
		  if (range_to->next == &Q_z8)
		    {
		      n = long_unbox (range_to);
		    }
		  else if (range_to->next == &Q_y8)
		    {
		      n = double_unbox (range_to);
		    }
		  for (i = m; i > n; i += step)
		    {
		      cons_insert_tail (curs, long_alloc (i));
		    }
		}
	      else if (range_from->next == &Q_y8)
		{
		  double m_d = double_unbox (range_from);
		  if (range_to->next == &Q_z8)
		    {
		      n = long_unbox (range_to) - m_d;
		    }
		  else if (range_to->next == &Q_y8)
		    {
		      n = double_unbox (range_to) - m_d;
		    }
		  for (i = 0; i > n; i += step)
		    {
		      cons_insert_tail (curs, double_alloc (i + m_d));
		    }
		}
	    }
	}
      else if (range_step->next == &Q_y8)
	{
	  long step = double_unbox (range_step);
	  double m_d;
	  if (range_from->next == &Q_z8)
	    {
	      long m = long_unbox (range_from);
	      m_d = m;
	      if (range_to->next == &Q_z8)
		{
		  n = (long_unbox (range_to) - m) / step;
		}
	      else if (range_to->next == &Q_y8)
		{
		  n = (double_unbox (range_to) - m) / step;
		}
	    }
	  else if (range_from->next == &Q_y8)
	    {
	      m_d = double_unbox (range_from);
	      if (range_to->next == &Q_z8)
		{
		  n = (long_unbox (range_to) - m_d) / step;
		}
	      else if (range_to->next == &Q_y8)
		{
		  n = (double_unbox (range_to) - m_d) / step;
		}
	    }
	  double item;
	  for (item = m_d, i = 0; i < n; ++i, item += step)
	    {
	      cons_insert_tail (curs, double_alloc (item));
	    }
	}
    }
  else if (range_from)
    {
      if (range_from->next == &Q_z8)
	{
	  long m = long_unbox (range_from);
	  if (range_to->next == &Q_z8)
	    {
	      n = long_unbox (range_to);
	      for (i = m; i < n; ++i)
		{
		  cons_insert_tail (curs, long_alloc (i));
		}
	    }
	  else if (range_to->next == &Q_y8)
	    {
	      n = double_unbox (range_to);
	      for (i = m; i < n; ++i)
		{
		  cons_insert_tail (curs, long_alloc (i));
		}
	    }
	}
      else if (range_from->next == &Q_y8)
	{
	  double m_d = double_unbox (range_from);
	  if (range_to->next == &Q_z8)
	    {
	      n = long_unbox (range_to) - m_d;
	      for (i = 0; i < n; ++i)
		{
		  cons_insert_tail (curs, double_alloc (i + m_d));
		}
	    }
	  else if (range_to->next == &Q_y8)
	    {
	      n = double_unbox (range_to) - m_d;
	      for (i = 0; i < n; ++i)
		{
		  cons_insert_tail (curs, double_alloc (i + m_d));
		}
	    }
	}
    }
  else
    {
      if (range_to->next == &Q_z8)
	{
	  n = long_unbox (range_to);
	}
      else if (range_to->next == &Q_y8)
	{
	  n = double_unbox (range_to);
	}
      for (i = 0; i < n; ++i)
	{
	  cons_insert_tail (curs, long_alloc (i));
	}
    }

  struct cons *result = cons_alloc (curs->first, &Q_CONS);
  free (curs);
  return result;
}

struct cons *
_special_def (struct cons *params)
{
  struct cons *key = params->first;	// must be a symbol, for now.
  struct symbol *sym = key->first;	// must be a symbol, for now.
  return symbol_push (sym, interp_eval_box (params->next->first));
}

struct cons *
_special_fn (struct cons *params)
{
  return fn_alloc (((struct cons *) params->first)->first, params->next);
}

struct cons *
_special_equals (struct cons *params)
{
  struct cons *result = interp_eval_box (params->first);
  struct cons *x;

  if (!result)
    {
      for (params = params->next; params; params = params->next)
	{
	  if (interp_eval_box (params->first))
	    {
	      return NULL;
	    }
	}
      return &Q_VAL_NONNIL;
    }

  for (params = params->next; params; params = params->next)
    {
      if (!(x = interp_eval_box (params->first)))
	{
	  return NULL;
	}
      if (x->next != result->next)	// not of the same type
	{
	  return NULL;
	}
      if (numberp (x))
	{
	  if (x->next == &Q_z8)
	    {
	      if ((long_unbox (x) != long_unbox (result)))
		return NULL;
	    }
	  else if (x->next == &Q_y8)
	    {
	      if ((double_unbox (x) != double_unbox (result)))
		return NULL;
	    }
	  /* else if (x->next == &Q_RGB3) */
	  /*    { if ((rgb3_unbox(x) != rgb3_unbox (result))) */
	  /*        return NULL; } */
	  else			// FIXME: not implemented!
	    {
	      return NULL;
	    }
	}
      else if (x->next == &Q_STRING)
	{
	  if (strcmp (x->first, result->first) != 0)
	    {
	      return NULL;
	    }
	}
      else if (x->first != result->first)	// FIXME: very crude!
	{
	  return NULL;
	}
    }
  return result;
}

struct cons *
_special_gt (struct cons *params)
{
  struct cons *result = params->first;
  struct cons *x;

  if (!result)
    {
      return NULL;
    }

  for (params = params->next; params; params = params->next)
    {
      if (!(x = params->first))
	{
	  return NULL;
	}
      if (numberp (x) && numberp (result))
	{
	  if (result->next == &Q_z8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((long_unbox (result) <= long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((long_unbox (result) <= double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  else if (result->next == &Q_y8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((double_unbox (result) <= long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((double_unbox (result) <= double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  /* else if (x->next == &Q_RGB3) ... */
	  else			// FIXME: not implemented!
	    {
	      return NULL;
	    }
	}
      else if ((result->next == &Q_STRING) && (x->next == &Q_STRING))
	{
	  if (strcmp (result->first, x->first) <= 0)
	    {
	      return NULL;
	    }
	}
      else
	{
	  return NULL;
	}
      result = x;
    }
  return result;
}

struct cons *
_special_lt (struct cons *params)
{
  struct cons *result = params->first;
  struct cons *x;

  if (!result)
    {
      return NULL;
    }

  for (params = params->next; params; params = params->next)
    {
      if (!(x = params->first))
	{
	  return NULL;
	}
      if (numberp (x) && numberp (result))
	{
	  if (result->next == &Q_z8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((long_unbox (result) >= long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((long_unbox (result) >= double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  else if (result->next == &Q_y8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((double_unbox (result) >= long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((double_unbox (result) >= double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  /* else if (x->next == &Q_RGB3) ... */
	  else			// FIXME: not implemented!
	    {
	      return NULL;
	    }
	}
      else if ((result->next == &Q_STRING) && (x->next == &Q_STRING))
	{
	  if (strcmp (result->first, x->first) >= 0)
	    {
	      return NULL;
	    }
	}
      else
	{
	  return NULL;
	}
      result = x;
    }
  return result;
}

struct cons *
_special_geq (struct cons *params)
{
  struct cons *result = params->first;
  struct cons *x;

  if (!result)
    {
      return NULL;
    }

  for (params = params->next; params; params = params->next)
    {
      if (!(x = params->first))
	{
	  return NULL;
	}
      if (numberp (x) && numberp (result))
	{
	  if (result->next == &Q_z8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((long_unbox (result) < long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((long_unbox (result) < double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  else if (result->next == &Q_y8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((double_unbox (result) < long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((double_unbox (result) < double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  /* else if (x->next == &Q_RGB3) ... */
	  else			// FIXME: not implemented!
	    {
	      return NULL;
	    }
	}
      else if ((result->next == &Q_STRING) && (x->next == &Q_STRING))
	{
	  if (strcmp (result->first, x->first) < 0)
	    {
	      return NULL;
	    }
	}
      else
	{
	  return NULL;
	}
      result = x;
    }
  return result;
}

struct cons *
_special_leq (struct cons *params)
{
  struct cons *result = params->first;
  struct cons *x;

  if (!result)
    {
      return NULL;
    }

  for (params = params->next; params; params = params->next)
    {
      if (!(x = params->first))
	{
	  return NULL;
	}
      if (numberp (x) && numberp (result))
	{
	  if (result->next == &Q_z8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((long_unbox (result) > long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((long_unbox (result) > double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  else if (result->next == &Q_y8)
	    {
	      if (((x->next == &Q_z8) &&
		   ((double_unbox (result) > long_unbox (x)))) ||
		  ((x->next == &Q_y8) &&
		   ((double_unbox (result) > double_unbox (x)))))
		{
		  return NULL;
		}
	    }
	  /* else if (x->next == &Q_RGB3) ... */
	  else			// FIXME: not implemented!
	    {
	      return NULL;
	    }
	}
      else if ((result->next == &Q_STRING) && (x->next == &Q_STRING))
	{
	  if (strcmp (result->first, x->first) > 0)
	    {
	      return NULL;
	    }
	}
      else
	{
	  return NULL;
	}
      result = x;
    }
  return result;
}

struct cons *
_special_not (struct cons *params)
{
  if (params->first)
    {
      return NULL;
    }
  else
    {
      return &Q_VAL_NONNIL;
    }
}

struct cons *
_special_let (struct cons *params)
{
  binding_stack_push ();
  struct cons *kv = ((struct cons *) params->first)->first;
  while (kv)
    {
      struct cons *key = kv->first;	// must be a symbol, for now.
      kv = kv->next;
      struct symbol *sym = key->first;
      symbol_push (sym, interp_eval_box (kv->first));
      kv = kv->next;
    }

  struct cons *result = interp_eval_progn (params->next);
  binding_stack_pop ();
  return result;
}

struct cons *
_special_ns (struct cons *params)
{
  // not implemented yet
  return NULL;
}

char *
__special_html_gen__box_to_str (char **p_buff_cur, char *buff_limit,
				struct cons *c)
{
  if (c)
    {
      struct cons *specifier = c->next;
      if (specifier == &Q_STRING)
	{
	  strcat_rf (p_buff_cur, buff_limit, string_unbox (c));
	}
      else if (specifier == &Q_KEYWORD)
	{
	  struct symbol *sym = c->first;
	  strcat_rf (p_buff_cur, buff_limit, (sym->name) + 1);
	  // + 1: skip the ':' at the beginning of the keyword.
	}
      else if (numberp (c))
	{
	  if ((buff_limit - *p_buff_cur) > MAX_NUMBER_STR_SIZE)
	    {
	      number_to_str (*p_buff_cur, c);
	      *p_buff_cur = *p_buff_cur + strlen (*p_buff_cur);
	    }
	  else
	    {
	      *p_buff_cur = NULL;
	    }
	}
      else if (specifier == &Q_RGB3)
	{
	  struct rgb3 *x = (struct rgb3 *) c->first;
	  int n;
	  if ((n = snprintf (*p_buff_cur, buff_limit - (*p_buff_cur),
			     "\"#%.2X%.2X%.2X\"", x->r, x->g, x->b)) >= 0)
	    {
	      *p_buff_cur = *p_buff_cur + n;
	    }
	  else
	    {
	      *p_buff_cur = NULL;
	    }
	}
      else
	{
	  fprintf (stderr, "[Unknown Type] %p\n", c->first);
	}
    }
  return *p_buff_cur;
}

char *__special_html_gen__form_to_str (char **p_buff_cur, char *buff_limit,
				       struct cons *c);

char *
__special_html_gen__unit_to_str (char **p_buff_cur, char *buff_limit,
				 struct cons *c)
{
  struct cons *tag = c->first;
  strcat_rf (p_buff_cur, buff_limit, "<");
  __special_html_gen__box_to_str (p_buff_cur, buff_limit, tag);
  c = c->next;
  /* struct cons* kv = ((struct cons*)  */
  /*                 (interp_eval_box (c->first)->first))->first; */
  if (c->first)
    {
      struct cons *kv = ((struct cons *) (c->first))->first;

      while (kv)
	{
	  strcat_rf (p_buff_cur, buff_limit, " ");
	  __special_html_gen__box_to_str (p_buff_cur, buff_limit, kv->first);
	  kv = kv->next;
	  strcat_rf (p_buff_cur, buff_limit, "=");
	  strcat_rf (p_buff_cur, buff_limit, "\"");
	  __special_html_gen__box_to_str (p_buff_cur, buff_limit, kv->first);
	  strcat_rf (p_buff_cur, buff_limit, "\"");
	  kv = kv->next;
	}
    }
  strcat_rf (p_buff_cur, buff_limit, ">");

  for (c = c->next; c; c = c->next)
    {
      __special_html_gen__form_to_str (p_buff_cur, buff_limit, c->first);
    }
  strcat_rf (p_buff_cur, buff_limit, "</");
  __special_html_gen__box_to_str (p_buff_cur, buff_limit, tag);
  strcat_rf (p_buff_cur, buff_limit, ">");
  return *p_buff_cur;
}


char *
__special_html_gen__form_to_str (char **p_buff_cur, char *buff_limit,
				 struct cons *c)
{
  if (c)
    {
      struct cons *specifier = c->next;
      if (specifier == &Q_CONS)
	{
	  __special_html_gen__unit_to_str (p_buff_cur, buff_limit, c->first);
	}
      else if (specifier == &Q_STRING)
	{
	  strcat_rf (p_buff_cur, buff_limit, string_unbox (c));
	}
      else if (specifier == &Q_SYMBOL)
	{
	  struct symbol *sym = c->first;
	  // cons_introspect (*sym, level + 1, 0);
	  strcat_rf (p_buff_cur, buff_limit, "'");
	  strcat_rf (p_buff_cur, buff_limit, sym->name);
	}
      else if (numberp (c))
	{
	  if ((buff_limit - *p_buff_cur) > MAX_NUMBER_STR_SIZE)
	    {
	      number_to_str (*p_buff_cur, c);
	      *p_buff_cur = *p_buff_cur + strlen (*p_buff_cur);
	    }
	  else
	    {
	      *p_buff_cur = NULL;
	    }
	}
      else if (specifier == &Q_RGB3)
	{
	  struct rgb3 *x = (struct rgb3 *) c->first;
	  int n;
	  if ((n = snprintf (*p_buff_cur, buff_limit - (*p_buff_cur),
			     "\"#%.2X%.2X%.2X\"", x->r, x->g, x->b)) >= 0)
	    {
	      *p_buff_cur = *p_buff_cur + n;
	    }
	  else
	    {
	      *p_buff_cur = NULL;
	    }
	}
      else
	{
	  fprintf (stderr, "[Unknown Type] %p\n", c->first);
	}
    }
  return *p_buff_cur;
}

struct cons *
_special_html_gen (struct cons *params)
{
  char buff[BUFF_SIZE];
  char *buff_limit = buff + BUFF_SIZE;

  char *buff_cur = buff;
  // buff[0] up to [buff_limit - buff - 1] are valid.

  for (; params; params = params->next)
    {
      __special_html_gen__form_to_str (&buff_cur, buff_limit,
				       interp_eval_box (params->first));
    }

  if (buff_cur)
    {
      size_t s_len = buff_cur - buff;
      char *s = malloc (s_len + 1);
      memcpy (s, buff, s_len);
      s[s_len] = 0;
      struct cons *result = string_alloc (s);
      return result;
    }
  else
    {
      return NULL;
    }
}

struct cons *
_special_do (struct cons *params)
{
  // (do form1 form2 form3)
  return interp_eval_progn (params);
}

struct cons *
_special_funcall (struct cons *params)
{
  // (apply f cons)
  struct cons *f = interp_eval_box (params->first);
  return interp_call (f, params->next);
}

struct cons *
_special_eval (struct cons *params)
{
  // (eval 'form1 'form2 'form3)
  return interp_eval_progn (params);
}

struct cons *
_special_filter (struct cons *params)
{
  struct cons *f = params->first;
  params = params->next;
  if (!params)
    {
      fprintf (stderr, "Error: too few arguments to `filter'\n");
      return NULL;
    }
  struct cons *curs = cons_alloc (NULL, NULL);
  struct cons *l = cons_unbox (params->first);	// must be a cons.
  struct cons *args;
  for (; l; l = l->next)
    {
      args = cons_alloc (l->first, NULL);
      if (interp_call_evaledparams (f, args))
	{
	  cons_insert_tail (curs, l->first);
	}
      free (args);
    }
  struct cons *result = cons_alloc (curs->first, &Q_CONS);
  free (curs);
  return result;
}

struct cons *
_special_map (struct cons *params)
{				// fn
  // (map f cons1 cons2 cons3)

  struct cons *f = params->first;
  params = params->next;
  if (!params)
    {
      fprintf (stderr, "Error: too few arguments to `map'\n");
      return NULL;
    }

  int i, n = cons_count (params);
  struct cons **params_a;
  struct cons **args;

  params_a = calloc (n, sizeof (struct cons));
  for (i = 0; params; params = params->next)
    {
      params_a[i++] = cons_unbox (params->first);
    }

  // *args is both an array and a linked list.
  args = calloc (n, sizeof (struct cons));
  args[n - 1] = cons_alloc (NULL, NULL);
  for (i = n - 1; i; --i)
    {
      args[i - 1] = cons_alloc (NULL, args[i]);
    }

  struct cons *curs = cons_alloc (NULL, NULL);
  i = 0;
  while (1)
    {
      if (!params_a[i])
	{
	  break;
	}
      args[i]->first = params_a[i]->first;
      params_a[i] = params_a[i]->next;

      if ((++i) == n)
	{
	  i = 0;
	  cons_insert_tail (curs, interp_call_evaledparams (f, *args));
	}
    }
  struct cons *result = cons_alloc (curs->first, &Q_CONS);
  free (curs);
  free (args);
  free (params_a);
  return result;
}

struct cons *
_special_apply (struct cons *params)
{				// fn
  // (apply f x1 x2 cons)
  struct cons *f = params->first;
  params = params->next;
  if (!params || !params->next)
    {
      return interp_call_evaledparams (f, cons_unbox (params->first));
    }
  else
    {
      struct cons *args_curs = cons_alloc (NULL, NULL);
      for (; params->next; params = params->next)
	{
	  cons_insert_tail (args_curs, params->first);
	}
      args_curs->next->next = cons_unbox (params->first);
      struct cons *args = args_curs->first;
      free (args_curs);
      return interp_call_evaledparams (f, args);
    }
  return NULL;
}

struct cons *
_special_reduce (struct cons *params)
{				// fn
  // (reduce f init-value cons)

  struct cons *f = params->first;
  params = params->next;
  if (!params || !params->next)
    {
      fprintf (stderr, "Error: too few arguments to `reduce'\n");
    }
  /* { return interp_call_evaledparams (f, cons_unbox (params->first)); } */
  else
    {
      struct cons *result = params->first;
      struct cons *l = cons_unbox (params->next->first);	// must be a cons.
      for (; l; l = l->next)
	{
	  struct cons *args =
	    cons_alloc (result, cons_alloc (l->first, NULL));
	  result = interp_call_evaledparams (f, args);
	  free (args->next);
	  free (args);
	}
      return result;
    }
  return NULL;
}

struct cons *
_special_or (struct cons *params)
{
  struct cons *result;
  for (result = NULL; !result && params; params = params->next)
    {
      result = interp_eval_box (params->first);
    }
  return result;
}

struct cons *
_special_and (struct cons *params)
{
  struct cons *result;
  for (result = &Q_VAL_NONNIL; result && params; params = params->next)
    {
      result = interp_eval_box (params->first);
    }
  return result;
}

struct timespec
gettimeofday_ts ()
{
  struct timeval now_tv;
  gettimeofday (&now_tv, NULL);
  struct timespec now_ts = { now_tv.tv_sec, (long) now_tv.tv_usec * 1000 };
  return now_ts;
}

int inline
timespec_compare (struct timespec a, struct timespec b)
{
  if (a.tv_sec > b.tv_sec)
    {
      return 1;
    }
  else if (a.tv_sec < b.tv_sec)
    {
      return -1;
    }
  else if (a.tv_nsec > b.tv_nsec)
    {
      return 1;
    }
  else if (a.tv_nsec < b.tv_nsec)
    {
      return -1;
    }
  else
    {
      return 0;
    }
}

struct timespec inline
timespec_plus (struct timespec a, struct timespec b)
{
  struct timespec c = { a.tv_sec + b.tv_sec, a.tv_nsec + b.tv_nsec };
  if (c.tv_nsec >= 1000000000)
    {
      c.tv_nsec -= 1000000000;
      c.tv_sec++;
    }
  return c;
}

struct timespec inline
timespec_minus (struct timespec a, struct timespec b)
{
  struct timespec c = { a.tv_sec - b.tv_sec, a.tv_nsec - b.tv_nsec };
  if (c.tv_nsec < 0)
    {
      c.tv_nsec += 1000000000;
      c.tv_sec--;
    }
  else if (c.tv_nsec >= 1000000000)
    {
      c.tv_nsec -= 1000000000;
      c.tv_sec++;
    }
  return c;
}

struct timespec inline
timespec_from_ms (time_t ms)
{
  struct timespec ts = { ms / 1000, (long) (ms % 1000) * 1000000 };
  return ts;
}

char *
timespec_to_str (struct timespec ts)
{
  char *s = (char *) malloc (256);
  sprintf (s, "%ld.%ld", ts.tv_sec, ts.tv_nsec);
  return s;
}

struct cons *
_special_time (struct cons *params)
{
  // (time form1 form2)
  struct cons *result = NULL;
  struct timespec cur_time, new_time;
  for (cur_time = gettimeofday_ts ();
       params; params = params->next, cur_time = new_time)
    {
      result = interp_eval_box (params->first);
      new_time = gettimeofday_ts ();
      fprintf (stderr, "Elapsed time: %s\n",
	       timespec_to_str (timespec_minus (new_time, cur_time)));
    }
  return result;
}

/** (exec "/bin/ls" (list "ls" "-la") */

struct cons *
_special_exec (struct cons *params)
{
  char *program = string_unbox (params->first);
  int i, n;
  char **argv = malloc (sizeof (char *) * 2);
  argv[0] = program;
  argv[1] = 0;
  char **envp = NULL;
  params = params->next;
  if (params)
    {
      struct cons *l;
      if (consp (params->first))
	{
	  l = cons_unbox (params->first);
	  params = params->next;
	}
      else
	{
	  struct cons *curs = cons_alloc (NULL, NULL);
	  for (; params && !consp (params->first); params = params->next)
	    // { cons_insert_tail (curs, string_alloc (keyword_name)); }
	    {
	      cons_insert_tail (curs, params->first);
	    }
	  l = curs->first;
	  free (curs);
	}

      n = cons_count (l);
      free (argv);
      argv = malloc (sizeof (char *) * (n + 1));
      for (i = 0; l; ++i, l = l->next)
	{
	  struct cons *item = l->first;
	  char *item_str;
	  if (item->next == &Q_STRING)
	    {
	      item_str = string_unbox (item);
	    }
	  else if (item->next == &Q_KEYWORD)
	    {
	      char *keyword_name = symbol_unbox (item)->name;
	      if (*keyword_name == ':')
		{
		  size_t keyword_size = strlen (keyword_name);
		  item_str = malloc (keyword_size + 2);
		  if (keyword_size < 3)
		    {
		      sprintf (item_str, "-%s", keyword_name + 1);
		    }
		  else
		    {
		      sprintf (item_str, "--%s", keyword_name + 1);
		    }
		}
	    }
	  argv[i] = item_str;
	}
      argv[n] = (char *) 0;

      if (params && consp (params->first))
	{
	  l = cons_unbox (params->first);
	  n = cons_count (l);
	  envp = malloc (sizeof (char *) * (n + 1));
	  for (i = 0; l; ++i, l = l->next)
	    {
	      envp[i] = string_unbox (l->first);
	    }
	  envp[n] = (char *) 0;
	  params = params->next;
	}
    }

  // later, more arguments will specify stdin, stdout, stderr.
  if (params)
    {
      fprintf (stderr, "Error: too many arguments (%d) to `exec'\n",
	       3 + cons_count (params));
      return NULL;
    }
  execve (program, argv, envp);
  perror ("execve returned");
  return NULL;			// should not return!
}

struct cons *
_special_mod (struct cons *params)
{
  // param must be Long for now.
  struct cons *m = params->first;
  struct cons *k = params->next->first;
  return long_alloc (long_unbox (m) % long_unbox (k));
}

struct cons *
_special_evenp (struct cons *params)
{
  // param must be Long.
  struct cons *param = params->first;
  if (long_unbox (param) & 1)
    {
      return NULL;
    }
  return param;
}

struct cons *
_special_oddp (struct cons *params)
{
  // param must be Long.
  struct cons *param = params->first;
  if (long_unbox (param) & 1)
    {
      return param;
    }
  return NULL;
}

void
context_init_default ()
{
  ctx = calloc (1, sizeof (struct context));
  ctx->binding_stack = cons_alloc (NULL, NULL);

  symbol_define ("nil", NULL);
  symbol_define ("true", &Q_VAL_NONNIL);

  symbol_define ("unquote", &Q_VAL_UNQUOTE);
  symbol_define ("~", &Q_VAL_UNQUOTE);	// temporary
  symbol_define ("unquote-list", &Q_VAL_UNQUOTE_LIST);
  symbol_define ("~@", &Q_VAL_UNQUOTE_LIST);	// temporary

  symbol_define ("*home*", string_alloc (getenv ("HOME")));
  symbol_define ("*cwd*", string_alloc (getenv ("PWD")));
  symbol_define ("*user*", string_alloc (getenv ("USER")));

  symbol_define ("list", cons_alloc (&_special_list, &Q_FUNCTION_INTERP));

  symbol_define ("inc", cons_alloc (&_special_inc, &Q_FUNCTION_INTERP));
  symbol_define ("dec", cons_alloc (&_special_dec, &Q_FUNCTION_INTERP));
  symbol_define ("+", cons_alloc (&_special_plus, &Q_FUNCTION_INTERP));
  symbol_define ("-", cons_alloc (&_special_minus, &Q_FUNCTION_INTERP));
  symbol_define ("*", cons_alloc (&_special_mult, &Q_FUNCTION_INTERP));
  symbol_define ("/", cons_alloc (&_special_divide, &Q_FUNCTION_INTERP));

  symbol_define ("println",
		 cons_alloc (&_special_println, &Q_FUNCTION_INTERP));
  symbol_define ("str", cons_alloc (&_special_str, &Q_FUNCTION_INTERP));
  symbol_define ("spth", cons_alloc (&_special_spth, &Q_MACRO_INTERP));
  symbol_define ("if", cons_alloc (&_special_if, &Q_MACRO_INTERP));
  symbol_define ("if-not", cons_alloc (&_special_if_not, &Q_MACRO_INTERP));
  symbol_define ("when", cons_alloc (&_special_when, &Q_MACRO_INTERP));
  symbol_define ("when-not",
		 cons_alloc (&_special_when_not, &Q_MACRO_INTERP));

  symbol_define ("quote", cons_alloc (&_special_quote, &Q_MACRO_INTERP));
  symbol_define ("quote-eval",
		 cons_alloc (&_special_quote_eval, &Q_MACRO_INTERP));
  symbol_define ("'", cons_alloc (&_special_quote, &Q_MACRO_INTERP));	// temporary
  symbol_define ("`", cons_alloc (&_special_quote_eval, &Q_MACRO_INTERP));	// temporary
  symbol_define ("do", cons_alloc (&_special_do, &Q_MACRO_INTERP));
  symbol_define ("funcall", cons_alloc (&_special_funcall, &Q_MACRO_INTERP));

  symbol_define ("eval", cons_alloc (&_special_eval, &Q_FUNCTION_INTERP));
  symbol_define ("filter", cons_alloc (&_special_filter, &Q_FUNCTION_INTERP));
  symbol_define ("map", cons_alloc (&_special_map, &Q_FUNCTION_INTERP));
  symbol_define ("reduce", cons_alloc (&_special_reduce, &Q_FUNCTION_INTERP));
  symbol_define ("apply", cons_alloc (&_special_apply, &Q_FUNCTION_INTERP));

  symbol_define ("time", cons_alloc (&_special_time, &Q_MACRO_INTERP));
  symbol_define ("exec", cons_alloc (&_special_exec, &Q_FUNCTION_INTERP));

  symbol_define ("and", cons_alloc (&_special_and, &Q_MACRO_INTERP));
  symbol_define ("or", cons_alloc (&_special_or, &Q_MACRO_INTERP));

  symbol_define ("first", cons_alloc (&_special_first, &Q_FUNCTION_INTERP));
  symbol_define ("rest", cons_alloc (&_special_rest, &Q_FUNCTION_INTERP));
  symbol_define ("nth", cons_alloc (&_special_nth, &Q_FUNCTION_INTERP));
  symbol_define ("cons", cons_alloc (&_special_cons, &Q_FUNCTION_INTERP));
  symbol_define ("range", cons_alloc (&_special_range, &Q_FUNCTION_INTERP));
  symbol_define ("def", cons_alloc (&_special_def, &Q_MACRO_INTERP));
  symbol_define ("fn", cons_alloc (&_special_fn, &Q_MACRO_INTERP));
  symbol_define ("let", cons_alloc (&_special_let, &Q_MACRO_INTERP));
  symbol_define ("=", cons_alloc (&_special_equals, &Q_MACRO_INTERP));
  symbol_define ("<", cons_alloc (&_special_lt, &Q_FUNCTION_INTERP));
  symbol_define ("<=", cons_alloc (&_special_leq, &Q_FUNCTION_INTERP));
  symbol_define (">", cons_alloc (&_special_gt, &Q_FUNCTION_INTERP));
  symbol_define (">=", cons_alloc (&_special_geq, &Q_FUNCTION_INTERP));
  symbol_define ("not", cons_alloc (&_special_not, &Q_FUNCTION_INTERP));
  symbol_define ("mod", cons_alloc (&_special_mod, &Q_FUNCTION_INTERP));
  symbol_define ("even?", cons_alloc (&_special_evenp, &Q_FUNCTION_INTERP));
  symbol_define ("odd?", cons_alloc (&_special_oddp, &Q_FUNCTION_INTERP));

  symbol_define ("ns", cons_alloc (&_special_ns, &Q_MACRO_INTERP));



  symbol_define ("html", cons_alloc (&_special_html_gen, &Q_MACRO_INTERP));

  // symbol_define ("", cons_alloc (&_special_, &Q_MACRO_INTERP));

}
