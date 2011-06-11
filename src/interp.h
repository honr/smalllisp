#ifndef INTERP_H
#define INTERP_H

#include "box.h"
#include "sexp-parse.h"

struct context
{
  struct bin *symbols;
  struct cons *binding_stack;
};

struct cons *fn_alloc (struct cons *params, struct cons *body);

extern struct context *ctx;

inline struct cons *symbol_define_sym (struct cons **sym, struct cons *value);
inline struct cons *symbol_push (struct symbol *sym, struct cons *value);
inline void symbol_pop (struct symbol *sym);

void symbol_define (char *key, struct cons *value);
void binding_stack_push ();
void binding_stack_pop ();

struct cons *interp_call_evaledparams (struct cons *f, struct cons *params);
struct cons *interp_call (struct cons *f, struct cons *params);
struct cons *interp_eval_box (struct cons *form);
struct cons *interp_eval_progn (struct cons *forms);
struct cons *interp_eval_cons (struct cons *forms);

struct cons *interp (struct cons *forms);

#endif
