#ifndef SEXP_PARSE_H
#define SEXP_PARSE_H

#include "box.h"

struct cons *sexp_parse_str (struct bin **p_symbols, char *buf);

#endif
