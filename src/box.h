#ifndef BOX_H
#define BOX_H

#include <string.h>
#include <stdlib.h>

/* unary cell */
struct cons
{
  void *first;
  struct cons *next;
};

struct cons *cons_alloc (void *first, struct cons *next);
int cons_count (struct cons *c);
void cons_free (struct cons *c);
void cons_nconcat (struct cons *dest, struct cons *tail);
struct cons *cons_pop (struct cons *c);
void cons_nreverse (struct cons **c);
struct cons *cons_last (struct cons *c);
void cons_insert_tail (struct cons *cursor, void *item);

#ifdef DEBUG
void dbg_cons_print (struct cons *c);
#endif // DEBUG

/* binary cell */
struct bin
{
  void *first;
  struct bin *left;
  struct bin *right;
};

struct bin_a
{
  void *first;
  struct bin next[2];
};

struct bin *bin_alloc (void *first, struct bin *left, struct bin *right);
int bin_height (struct bin *b);
void bin_free (struct bin *b);

#ifdef DEBUG
void dbg_bin_print (struct bin *b);
#endif // DEBUG


// extern struct cons Q_NUMBER;
extern struct cons Q_STRING;
extern struct cons Q_CONS;
extern struct cons Q_MACRO_INTERP;
extern struct cons Q_FUNCTION_INTERP;
extern struct cons Q_LAMBDA_INTERP;
extern struct cons Q_SYMBOL;
extern struct cons Q_KEYWORD;
// extern struct cons Q_SYMROOT;

extern struct cons Q_u1;
extern struct cons Q_u2;
extern struct cons Q_u4;
extern struct cons Q_u8;
extern struct cons Q_z1;
extern struct cons Q_z2;
extern struct cons Q_z4;
extern struct cons Q_z8;
extern struct cons Q_y4;
extern struct cons Q_y8;

extern struct cons Q_RGB3;

extern struct cons Q_VAL_NONNIL;
extern struct cons Q_VAL_UNQUOTE;
extern struct cons Q_VAL_UNQUOTE_LIST;

inline struct cons *box_type (struct cons *c);

struct symbol
{
  char *name;
  struct cons *vals;
  struct symbol *parent;
};

struct fn_interp
{
  struct cons *params;
  struct cons *body;
};

inline struct cons *int_alloc (int i);
inline int int_unbox (struct cons *c);

inline struct cons *long_alloc (long i);
inline long long_unbox (struct cons *c);

inline struct cons *double_alloc (double i);
inline double double_unbox (struct cons *c);

struct rgb3
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
};

inline struct cons *rgb_alloc (struct rgb3 i);
inline struct rgb3 rgb_unbox (struct cons *c);
inline struct rgb3 rgb_add (struct rgb3 a, struct rgb3 b);
inline struct rgb3 rgb_mult (double coeff, struct rgb3 a);
inline struct rgb3 rgb_inverse (struct rgb3 a);

inline struct cons *number_alloc (char *s);
inline int numberp (struct cons *c);
inline int number_to_str (char *dest, struct cons *c);

inline struct cons *string_alloc (char *s);
inline struct cons *string_alloc_c (char *s);
inline char *string_unbox (struct cons *c);

inline int consp (struct cons *c);
inline struct cons *cons_unbox (struct cons *c);

inline int symbolp (struct cons *c);
inline struct cons *symbol_alloc (struct bin **p_symbols, char *s);
inline struct symbol *symbol_unbox (struct cons *c);

/* ******************** h-trie ******************** */

struct bin *htrie_alloc (void *val);
void *htrie_get (struct bin *b, char *key);
void *htrie_get_ref (struct bin *b, char *key);
int htrie_assoc (struct bin **b, char *key, void *val);
// returns 0 or 1 on success, -1 on error.

void htrie_dissoc (struct bin **b, char *key);
void htrie_free (struct bin *b);
struct bin *htrie_subtree (struct bin *b, char *prefix);

/* ************************************************ */


#define ASCIIRED(x) "\e[31m"x"\e[0m"
#define ASCIIGREEN(x) "\e[32m"x"\e[0m"
#define ASCIIYELLOW(x) "\e[33m"x"\e[0m"
#define ASCIIBLUE(x) "\e[34m"x"\e[0m"
#define ASCIIMAGENTA(x) "\e[35m"x"\e[0m"
#define ASCIICYAN(x) "\e[36m"x"\e[0m"
#define ASCIIGRAY(x) "\e[37m"x"\e[0m"

void box_print (struct cons *c);
// char* box_to_str (struct cons *c);

void cons_introspect (struct cons *c, int level, int expect_boxed);

/*
// for OSX
#ifndef _GNU_SOURCE

inline size_t strnlen(const char *s, size_t len)
  { size_t i;
    for (i=0; i < len && *s; i++, s++);
    return i; }

inline char* strndup (char const *s, size_t n)
  { size_t len = strnlen (s, n); // or n-1 ?
    char *t;

    if ((t = malloc (len + 1)) == NULL)
      return NULL;

    t[len] = '\0';
    memcpy (t, s, len);
    return t; }

#endif
*/

/* If dest is NULL do nothing and just return NULL.  Otherwise, copy
   the string from src to dest, up to memory location dest_limit.  If
   '\0' does not occur within that range (src string is too long),
   return NULL.  */
inline char *strcat_rf (char **p_dest, const char *dest_limit,
			const char *src);

// int sprintf_rf (char** p_dest, const char* dest_limit, const char* format, ...);

/** splits string xy at position cur into *xp and *yp.
    returns length of the remaining string (i.e., y).
*/
int str_split (char *xy, char *cur, char **xp, char **yp);

#endif
