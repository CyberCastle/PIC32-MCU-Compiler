#include <stdlib.h>
#include <assert.h>
#include "cloog/cloog.h"

#define ALLOC(type) (type*)malloc(sizeof(type))
#define ALLOCN(type,n) (type*)malloc((n)*sizeof(type))

/**
 * CloogInfos structure:
 * this structure contains all the informations necessary for pretty printing,
 * they come from the original CloogProgram structure (language, names), from
 * genereral options (options) or are built only for pretty printing (stride).
 * This structure is mainly there to reduce the number of function parameters,
 * since most pprint.c functions need most of its field.
 */
struct clooginfos
{ Value * stride ;           /**< The stride for each iterator. */
  int  nb_scattdims ;        /**< Scattering dimension number. */
  int * scaldims ;           /**< Boolean array saying whether a given
                              *   scattering dimension is scalar or not.
                              */
  CloogNames * names ;       /**< Names of iterators and parameters. */
  CloogOptions * options ;   /**< Options on CLooG's behaviour. */
  CloogMatrix *equal;        /**< Matrix of equalities. */
} ;

typedef struct clooginfos CloogInfos ;

struct clast_term *new_clast_term(Value c, const char *v)
{
    struct clast_term *t = (struct clast_term *) malloc(sizeof(struct clast_term));
    t->expr.type = expr_term;
    value_init(t->val);
    value_assign(t->val, c);
    t->var = v;
    return t;
}

struct clast_binary *new_clast_binary(enum clast_bin_type t, 
				      struct clast_expr *lhs, Value rhs)
{
    struct clast_binary *b = (struct clast_binary *) malloc(sizeof(struct clast_binary));
    b->expr.type = expr_bin;
    b->type = t;
    b->LHS = lhs;
    value_init(b->RHS);
    value_assign(b->RHS, rhs);
    return b;
}

struct clast_reduction *new_clast_reduction(enum clast_red_type t, int n)
{
    int i;
    struct clast_reduction *r;
    r = (struct clast_reduction *) malloc(sizeof(struct clast_reduction)+(n-1)*sizeof(struct clast_expr *));
    r->expr.type = expr_red;
    r->type = t;
    r->n = n;
    for (i = 0; i < n; ++i)
	r->elts[i] = NULL;
    return r;
}

static void free_clast_root(struct clast_stmt *s)
{
    struct clast_root *r = (struct clast_root *)s;
    assert(CLAST_STMT_IS_A(s, stmt_root));
    cloog_names_free(r->names);
    free(r);
}

struct clast_stmt_op stmt_root = { free_clast_root };

struct clast_root *new_clast_root(CloogNames *names)
{
    struct clast_root *r = (struct clast_root *) malloc(sizeof(struct clast_root));
    r->stmt.op = &stmt_root;
    r->stmt.next = NULL;
    r->names = cloog_names_copy(names);
    return r;
}

static void free_clast_assignment(struct clast_stmt *s);

struct clast_stmt_op stmt_ass = { free_clast_assignment };

static void free_clast_assignment(struct clast_stmt *s)
{
    struct clast_assignment *a = (struct clast_assignment *)s;
    assert(CLAST_STMT_IS_A(s, stmt_ass));
    free_clast_expr(a->RHS);
    free(a);
}

struct clast_assignment *new_clast_assignment(const char *lhs,
					      struct clast_expr *rhs)
{
    struct clast_assignment *a = (struct clast_assignment *) malloc(sizeof(struct clast_assignment));
    a->stmt.op = &stmt_ass;
    a->stmt.next = NULL;
    a->LHS = lhs;
    a->RHS = rhs;
    return a;
}

static void free_clast_user_stmt(struct clast_stmt *s);

struct clast_stmt_op stmt_user = { free_clast_user_stmt };

static void free_clast_user_stmt(struct clast_stmt *s)
{
    struct clast_user_stmt *u = (struct clast_user_stmt *)s;
    assert(CLAST_STMT_IS_A(s, stmt_user));
    cloog_clast_free(u->substitutions);
    free(u);
}

struct clast_user_stmt *new_clast_user_stmt(CloogStatement *stmt, 
					    struct clast_stmt *subs)
{
    struct clast_user_stmt *u = (struct clast_user_stmt *) malloc(sizeof(struct clast_user_stmt));
    u->stmt.op = &stmt_user;
    u->stmt.next = NULL;
    u->statement = stmt;
    u->substitutions = subs;
    return u;
}

static void free_clast_block(struct clast_stmt *s)
{
    struct clast_block *b = (struct clast_block *)s;
    assert(CLAST_STMT_IS_A(s, stmt_block));
    cloog_clast_free(b->body);
    free(b);
}

struct clast_stmt_op stmt_block = { free_clast_block };

struct clast_block *new_clast_block (void)
{
    struct clast_block *b = (struct clast_block *) malloc(sizeof(struct clast_block));
    b->stmt.op = &stmt_block;
    b->stmt.next = NULL;
    b->body = NULL;
    return b;
}

static void free_clast_for(struct clast_stmt *s)
{
    struct clast_for *f = (struct clast_for *)s;
    assert(CLAST_STMT_IS_A(s, stmt_for));
    free_clast_expr(f->LB);
    free_clast_expr(f->UB);
    value_clear(f->stride);
    cloog_clast_free(f->body);
    free(f);
}

struct clast_stmt_op stmt_for = { free_clast_for };

struct clast_for *new_clast_for(const char *it, struct clast_expr *LB, 
				struct clast_expr *UB, Value stride)
{
    struct clast_for *f = (struct clast_for *) malloc(sizeof(struct clast_for));
    f->stmt.op = &stmt_for;
    f->stmt.next = NULL;
    f->iterator = it;
    f->LB = LB;
    f->UB = UB;
    f->body = NULL;
    value_init(f->stride);
    value_assign(f->stride, stride);
    return f;
}

static void free_clast_guard(struct clast_stmt *s)
{
    int i;
    struct clast_guard *g = (struct clast_guard *)s;
    assert(CLAST_STMT_IS_A(s, stmt_guard));
    cloog_clast_free(g->then);
    for (i = 0; i < g->n; ++i) {
	free_clast_expr(g->eq[i].LHS);
	free_clast_expr(g->eq[i].RHS);
    }
    free(g);
}

struct clast_stmt_op stmt_guard = { free_clast_guard };

struct clast_guard *new_clast_guard(int n)
{
    int i;
    struct clast_guard *g = (struct clast_guard *)
      malloc(sizeof(struct clast_guard) + (n-1) * sizeof(struct clast_equation));
    g->stmt.op = &stmt_guard;
    g->stmt.next = NULL;
    g->then = NULL;
    g->n = n;
    for (i = 0; i < n; ++i) {
	g->eq[i].LHS = NULL;
	g->eq[i].RHS = NULL;
    }
    return g;
}

void free_clast_term(struct clast_term *t)
{
    value_clear(t->val);
    free(t);
}

void free_clast_binary(struct clast_binary *b)
{
    value_clear(b->RHS);
    free_clast_expr(b->LHS);
    free(b);
}

void free_clast_reduction(struct clast_reduction *r)
{
    int i;
    for (i = 0; i < r->n; ++i)
	free_clast_expr(r->elts[i]);
    free(r);
}

void free_clast_expr(struct clast_expr *e)
{
    if (!e)
	return;
    switch (e->type) {
    case expr_term:
	free_clast_term((struct clast_term*) e);
	break;
    case expr_red:
	free_clast_reduction((struct clast_reduction*) e);
	break;
    case expr_bin:
	free_clast_binary((struct clast_binary*) e);
	break;
    default:
	assert(0);
    }
}

void free_clast_stmt(struct clast_stmt *s)
{
    assert(s->op);
    assert(s->op->free);
    s->op->free(s);
}

void cloog_clast_free(struct clast_stmt *s)
{
    struct clast_stmt *next;
    while (s) {
	next = s->next;
	free_clast_stmt(s);
	s = next;
    }
}

static int clast_term_equal(struct clast_term *t1, struct clast_term *t2)
{
    if (t1->var != t2->var)
	return 0;
    return value_eq(t1->val, t2->val);
}

static int clast_binary_equal(struct clast_binary *b1, struct clast_binary *b2)
{
    if (b1->type != b2->type)
	return 0;
    if (value_ne(b1->RHS, b2->RHS))
	return 0;
    return clast_expr_equal(b1->LHS, b2->LHS);
}

static int clast_reduction_equal(struct clast_reduction *r1, struct clast_reduction *r2)
{
    int i;
    if (r1->type == clast_red_max && r2->type == clast_red_min && 
	    r1->n == 1 && r2->n == 1)
	return clast_expr_equal(r1->elts[0], r2->elts[0]);
    if (r1->type != r2->type)
	return 0;
    if (r1->n != r2->n)
	return 0;
    for (i = 0; i < r1->n; ++i)
	if (!clast_expr_equal(r1->elts[i], r2->elts[i]))
	    return 0;
    return 1;
}

int clast_expr_equal(struct clast_expr *e1, struct clast_expr *e2)
{
    if (!e1 && !e2)
	return 1;
    if (!e1 || !e2)
	return 0;
    if (e1->type != e2->type)
	return 0;
    switch (e1->type) {
    case expr_term:
	return clast_term_equal((struct clast_term*) e1, 
				(struct clast_term*) e2);
    case expr_bin:
	return clast_binary_equal((struct clast_binary*) e1, 
				  (struct clast_binary*) e2);
    case expr_red:
	return clast_reduction_equal((struct clast_reduction*) e1, 
				     (struct clast_reduction*) e2);
    default:
	assert(0);
    }
}


/******************************************************************************
 *                        Equalities spreading functions                      *
 ******************************************************************************/


/* Equalities are stored inside a CloogMatrix data structure called "equal".
 * This matrix has (nb_scattering + nb_iterators + 1) rows (i.e. total
 * dimensions + 1, the "+ 1" is because a statement can be included inside an
 * external loop without iteration domain), and (nb_scattering + nb_iterators +
 * nb_parameters + 2) columns (all unknowns plus the scalar plus the equality
 * type). The ith row corresponds to the equality "= 0" for the ith dimension
 * iterator. The first column gives the equality type (0: no equality, then
 * EQTYPE_* -see pprint.h-). At each recursion of pprint, if an equality for
 * the current level is found, the corresponding row is updated. Then the
 * equality if it exists is used to simplify expressions (e.g. if we have 
 * "i+1" while we know that "i=2", we simplify it in "3"). At the end of
 * the pprint call, the corresponding row is reset to zero.
 */


/**
 * clast_equal_type function :
 * This function returns the type of the equality in the constraint (line) of
 * (equal) for the element (level). An equality is 'constant' iff all other
 * factors are null except the constant one. It is a 'pure item' iff one and
 * only one factor is non null and is 1 or -1. Otherwise it is an 'affine
 * expression'.
 * For instance:
 *   i = -13 is constant, i = j, j = -M are pure items,
 *   j = 2*M, i = j+1 are affine expressions.
 * When the equality comes from a 'one time loop', (line) is ONE_TIME_LOOP.
 * This case require a specific treatment since we have to study all the
 * constraints.
 * - equal is the matrix of equalities,
 * - level is the column number in equal of the element which is 'equal to',
 * - line is the line number in equal of the constraint we want to study;
 *   if it is -1, all lines must be studied.
 **
 * - July     3rd 2002: first version, called pprint_equal_isconstant. 
 * - July     6th 2002: adaptation for the 3 types. 
 * - June    15th 2005: (debug) expr = domain->Constraint[line] was evaluated
 *                      before checking if line != ONE_TIME_LOOP. Since
 *                      ONE_TIME_LOOP is -1, an invalid read was possible.
 * - October 19th 2005: Removal of the once-time-loop specific processing.
 */
static int clast_equal_type(CloogMatrix *equal, int level, int line)
{ 
  int i, one=0 ;
  
  /* There is only one non null factor, and it must be +1 or -1 for
   * iterators or parameters.
   */ 
  for (i=1;i<=equal->NbColumns-2;i++)
    if (value_notzero_p(equal->p[line][i]) && (i != level))
      { if ((value_notone_p(equal->p[line][i])
	     && value_notmone_p(equal->p[line][i]))
	    || (one != 0))
    return EQTYPE_EXAFFINE ;
    else
    one = 1 ;
  }
  /* if the constant factor is non null, it must be alone. */
  if (one != 0)
    { if (value_notzero_p(equal->p[line][equal->NbColumns-1]))
    return EQTYPE_EXAFFINE ;
  }
  else
  return EQTYPE_CONSTANT ;
  
  return EQTYPE_PUREITEM ;
}


/**
 * clast_equal_allow function:
 * This function checks whether the options allow us to spread the equality or
 * not. It returns 1 if so, 0 otherwise.
 * - equal is the matrix of equalities,
 * - level is the column number in equal of the element which is 'equal to',
 * - line is the line number in equal of the constraint we want to study,
 * - the infos structure gives the user all options on code printing and more.
 **
 * - October 27th 2005: first version (extracted from old pprint_equal_add).
 */
static int clast_equal_allow(CloogMatrix *equal, int level, int line, CloogInfos *infos)
{ 
  if ((!infos->options->csp && !infos->options->esp) ||
      (level < infos->options->fsp))
  return 0 ;
  
  if (infos->options->csp &&
      (clast_equal_type(equal,level,line) == EQTYPE_EXAFFINE) &&
      !infos->options->esp)
  return 0 ;

  return 1 ;
}


/**
 * clast_equal_add function:
 * This function updates the row (level-1) of the equality matrix (equal) with
 * the row that corresponds to the row (line) of the matrix (matrix). It returns
 * 1 if the row can be updated, 0 otherwise.
 * - equal is the matrix of equalities,
 * - matrix is the matrix of constraints,
 * - level is the column number in matrix of the element which is 'equal to',
 * - line is the line number in matrix of the constraint we want to study,
 * - the infos structure gives the user all options on code printing and more.
 **
 * - July     2nd 2002: first version. 
 * - October 19th 2005: Addition of the once-time-loop specific processing.
 */
static int clast_equal_add(CloogMatrix *equal, CloogMatrix *matrix, int level, int line,
			   CloogInfos *infos)
{ 
  int i ;
  Value numerator, denominator, division, modulo ;

  /* If we are in the case of a loop running once, this means that the equality
   * comes from an inequality. Here we find this inequality.
   */
  if (line == ONE_TIME_LOOP)
  { for (i=0;i<matrix->NbRows;i++)
      if ((value_notzero_p(matrix->p[i][0]))&&
	  (value_notzero_p(matrix->p[i][level])))
    { line = i ;
      
      /* Since in once-time-loops, equalities derive from inequalities, we
       * may have to offset the values. For instance if we have 2i>=3, the
       * equality is in fact i=2. This may happen when the level coefficient is
       * not 1 or -1 and the scalar value is not zero. In any other case (e.g.,
       * if the inequality is an expression including outer loop counters or
       * parameters) the once time loop would not have been detected
       * because of floord and ceild functions.
       */
      if (value_ne_si(matrix->p[i][level],1) &&
          value_ne_si(matrix->p[i][level],-1) &&
	  value_notzero_p(matrix->p[i][matrix->NbColumns-1]))
      { value_init_c(numerator) ;
        value_init_c(denominator) ;
        value_init_c(division) ;
        value_init_c(modulo) ;
        
	value_assign(denominator,matrix->p[i][level]) ;
	value_absolute(denominator,denominator) ; 
        value_assign(numerator,matrix->p[i][matrix->NbColumns-1]) ;   
        value_modulus(modulo,numerator,denominator) ;
        value_division(division,numerator,denominator) ;
	
	/* If MODULO is negative, decrease DIVISION so that the result of
	   division is rounded to minus infinity.  */
	if (value_neg_p (modulo))
	  value_decrement (division, division);

	value_assign (matrix->p[i][matrix->NbColumns-1], division);
        
	if (value_pos_p(matrix->p[i][level]))
	  value_set_si(matrix->p[i][level], 1) ;
	else
	  value_set_si(matrix->p[i][level], -1) ;
	
	value_clear_c(numerator) ;
        value_clear_c(denominator) ;
        value_clear_c(division) ;
        value_clear_c(modulo) ;
      }
            
      break ;
    }
  }
  
  /* We update the line of equal corresponding to level:
   * - the first element gives the equality type,
   */
  value_set_si(equal->p[level-1][0], clast_equal_type(matrix,level,line)) ;
  /* - the other elements corresponding to the equality itself
   *   (the iterators up to level, then the parameters and the scalar).
   */
  for (i=1;i<=level;i++)
    value_assign(equal->p[level-1][i], matrix->p[line][i]) ;
  for (i=0;i<cloog_names_nb_parameters (infos->names)+1;i++)
    value_assign(equal->p[level-1][equal->NbColumns-i-1],
				matrix->p[line][matrix->NbColumns-i-1]) ;
  
  cloog_matrix_equality_update(equal,level,cloog_names_nb_parameters (infos->names)) ;
  
  return (clast_equal_allow(equal,level,level-1,infos)) ;
}
  

/**
 * clast_equal_del function :
 * This function reset the equality corresponding to the iterator (level)
 * in the equality matrix (equal).
 * - July 2nd 2002: first version. 
 */
static void clast_equal_del(CloogMatrix * equal, int level)
{ 
  int i ;
  
  for (i=0;i<equal->NbColumns;i++)
    value_set_si(equal->p[level-1][i], 0) ;
}


/**
 * clast_bound_from_constraint function:
 * This function returns a clast_expr containing the printing of the
 * 'right part' of a constraint according to an element.
 * For instance, for the constraint -3*i + 2*j - M >=0 and the element j,
 * we have j >= (3*i + M)/2. As we are looking for integral solutions, this
 * function should return 'ceild(3*i+M,2)'.
 * - matrix is the polyhedron containing all the constraints,
 * - line_num is the line number in domain of the constraint we want to print,
 * - level is the column number in domain of the element we want to use,
 * - names structure gives the user some options about code printing,
 *   the number of parameters in domain (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 **
 * - November 2nd 2001: first version. 
 * - June    27th 2003: 64 bits version ready.
 */
static struct clast_expr *clast_bound_from_constraint(CloogMatrix *matrix,
						      int line_num, int level,
						      CloogNames *names)
{ 
  int i, nb_iter, sign, nb_elts=0 ;
  char * name;
  Value numerator, denominator, temp, division ;
  struct clast_expr *e = NULL;

  value_init_c(temp) ;
  value_init_c(numerator) ;
  value_init_c(denominator) ;

  if (value_notzero_p(matrix->p[line_num][level])) {
    struct clast_reduction *r;
    /* Maybe we need to invert signs in such a way that the element sign is>0.*/
    sign = value_pos_p(matrix->p[line_num][level]) ? -1 : 1 ;

    for (i = 1, nb_elts = 0; i <= matrix->NbColumns - 1; ++i)
      if (i != level && value_notzero_p(matrix->p[line_num][i]))
	    nb_elts++;
    r = new_clast_reduction(clast_red_sum, nb_elts);
    nb_elts = 0;

    /* First, we have to print the iterators. */
    nb_iter = matrix->NbColumns - 2 - cloog_names_nb_parameters (names);
    for (i=1;i<=nb_iter;i++)
      if ((i != level) && value_notzero_p(matrix->p[line_num][i])) {
	if (i <= cloog_names_nb_scattering (names))
        name = cloog_names_scattering_elt (names, i - 1);
      else
        name = cloog_names_iterator_elt (names, i - cloog_names_nb_scattering (names) - 1);
      
      if (sign == -1)
	value_oppose(temp,matrix->p[line_num][i]) ;
      else
	value_assign(temp,matrix->p[line_num][i]) ;
      
      r->elts[nb_elts++] = &new_clast_term(temp, name)->expr;
    }    

    /* Next, the parameters. */
    for (i=nb_iter+1;i<=matrix->NbColumns-2;i++)
      if ((i != level) && value_notzero_p(matrix->p[line_num][i])) {
	name = cloog_names_parameter_elt (names, i - nb_iter - 1);
      
      if (sign == -1)
	value_oppose(temp,matrix->p[line_num][i]) ;
      else
	value_assign(temp,matrix->p[line_num][i]) ;
      
      r->elts[nb_elts++] = &new_clast_term(temp, name)->expr;
    }    

    if (sign == -1)
      {
	value_oppose(numerator,matrix->p[line_num][matrix->NbColumns - 1]) ;
	value_assign(denominator,matrix->p[line_num][level]) ;
      }
    else
      {
	value_assign(numerator,matrix->p[line_num][matrix->NbColumns - 1]) ;
	value_oppose(denominator,matrix->p[line_num][level]) ;
      }
        
    /* Finally, the constant, and the final printing. */
    if (nb_elts) {
      if (value_notzero_p(numerator))
	  r->elts[nb_elts++] = &new_clast_term(numerator, NULL)->expr;
    
      if (value_notone_p(matrix->p[line_num][level]) 
	  && value_notmone_p(matrix->p[line_num][level]))
	{ if (value_one_p(matrix->p[line_num][0]))
	    { if (value_pos_p(matrix->p[line_num][level]))
	    e = &new_clast_binary(clast_bin_cdiv, &r->expr, denominator)->expr;
          else
	    e = &new_clast_binary(clast_bin_fdiv, &r->expr, denominator)->expr;
        } else
	    e = &new_clast_binary(clast_bin_div, &r->expr, denominator)->expr;
      }
      else
	e = &r->expr;
    } else { 
      free_clast_reduction(r);
      if (value_zero_p(numerator))
	e = &new_clast_term(numerator, NULL)->expr;
      else
      { if (value_notone_p(denominator))
	  { if (value_one_p(matrix->p[line_num][0])) /* useful? */
          { value_modulus(temp,numerator,denominator) ;
            if (value_zero_p(temp))
            { value_division(temp,numerator,denominator) ;
	      e = &new_clast_term(temp, NULL)->expr;
            }
            else
            { value_init_c(division) ;
	      value_division(division,numerator,denominator) ;
	      if (value_neg_p(numerator)) {
                if (value_pos_p(matrix->p[line_num][level])) {
		    /* nb<0 need max */
		    e = &new_clast_term(division, NULL)->expr;
		} else {
                  /* nb<0 need min */
                  value_decrement(temp,division) ;
		  e = &new_clast_term(temp, NULL)->expr;
                }
	      }
              else
		{ if (value_pos_p(matrix->p[line_num][level]))
	        { /* nb>0 need max */
                  value_increment(temp,division) ;
		  e = &new_clast_term(temp, NULL)->expr;
                }
		else
		    /* nb>0 need min */
		    e = &new_clast_term(division, NULL)->expr;
              }
	      value_clear_c(division) ;
            }
          }
          else
	    e = &new_clast_binary(clast_bin_div, 
				  &new_clast_term(numerator, NULL)->expr,
				  denominator)->expr;
        }
        else
	    e = &new_clast_term(numerator, NULL)->expr;
      }
    }
  }

  value_clear_c(temp) ;
  value_clear_c(numerator) ;
  value_clear_c(denominator) ;
    
  return e;
}


/**
 * clast_equal function:
 * This function returns the content an equality matrix (equal) into a clast_stmt.
 * - the infos structure gives the user all options on code printing and more. 
 **
 * - July   2nd 2002: first version. 
 * - March 16th 2003: return now a string instead of printing directly and do
 *                    not write 'Sx()' if there is no spreading, but only 'Sx'. 
 */
static struct clast_stmt * clast_equal(CloogInfos *infos)
{ 
  int i, iterator ;
  Value type ;
  struct clast_expr *e;
  struct clast_stmt *a = NULL;
  struct clast_stmt **next = &a;
  CloogMatrix *equal = infos->equal;

  value_init_c(type) ;
  
  /* It is not necessary to print here the scattering iterators since they
   * never appear in the statement bodies.
   */
  for (i = cloog_names_nb_scattering (infos->names); i < equal->NbRows; i++)
  { if (value_notzero_p(equal->p[i][0])&&clast_equal_allow(equal,i+1,i,infos)) {
      iterator = i - cloog_names_nb_scattering (infos->names) ;
    
      /* pprint_line needs to know that the current line is an equality, so
       * we temporary remove the equality type and set it to zero (the equality
       * tag in PolyLib.
       */
      value_assign(type,equal->p[i][0]) ;
      value_set_si(equal->p[i][0], 0) ;
      e = clast_bound_from_constraint(equal, i, i+1, infos->names);
      value_assign(equal->p[i][0], type) ;
      *next = &new_clast_assignment(cloog_names_iterators (infos->names)[iterator], e)->stmt;
      next = &(*next)->next;
    }
  }
  value_clear_c(type) ;

  return a;
}


/**
 * clast_equal_cpp function:
 * This function prints the substitution data of a statement into a clast_stmt.
 * Using this function instead of pprint_equal is useful for generating
 * a compilable pseudo-code by using preprocessor macro for each statement.
 * By opposition to pprint_equal, the result is less human-readable. For
 * instance this function will print (i,i+3,k,3) where pprint_equal would
 * return (j=i+3,l=3).
 * - level is the number of loops enclosing the statement,
 * - the infos structure gives the user all options on code printing and more.
 **
 * - March    12th 2004: first version. 
 * - November 21th 2005: (debug) now works well with GMP version. 
 */
static struct clast_stmt * clast_equal_cpp(int level, CloogInfos *infos)
{ 
  int i ;
  Value type ;
  struct clast_expr *e;
  struct clast_stmt *a = NULL;
  struct clast_stmt **next = &a;
  CloogMatrix *equal = infos->equal;

  value_init_c(type) ;
  
  for (i=cloog_names_nb_scattering (infos->names);i<level-1;i++)
    { if (value_notzero_p(equal->p[i][0]))
    { /* pprint_line needs to know that the current line is an equality, so
       * we temporary remove the equality type and set it to zero (the equality
       * tag in PolyLib.
       */
      value_assign(type,equal->p[i][0]) ;
      value_set_si(equal->p[i][0], 0) ;
      e = clast_bound_from_constraint(equal, i, i+1, infos->names);
      value_assign(equal->p[i][0], type) ;
    } else {
      value_set_si(type, 1);
      e = &new_clast_term
	(type, cloog_names_iterator_elt (infos->names, i - cloog_names_nb_scattering (infos->names)))->expr;
    }
    *next = &new_clast_assignment(NULL, e)->stmt;
    next = &(*next)->next;
  }
  value_clear_c(type) ;

  return a;
}


/**
 * clast_minmax function:
 * This function returns a clast_expr containing the printing of a minimum or a
 * maximum of the 'right parts' of all constraints according to an element.
 * For instance consider the constraints:
 * -3*i  +2*j   -M >= 0
 *  2*i    +j      >= 0
 *   -i    -j +2*M >= 0
 * if we are looking for the minimum for the element j, the function should
 * return 'max(ceild(3*i+M,2),-2*i)'.
 * - matrix is the polyhedron containing all the constraints,
 * - level is the column number in domain of the element we want to use,
 * - max is a boolean set to 1 if we are looking for a maximum, 0 for a minimum,
 * - guard is set to 0 if there is no guard, and set to the level of the element
 *   with a guard otherwise (then the function gives the max or the min only
 *   for the constraint where the guarded coefficient is 0), 
 * - the infos structure gives the user some options about code printing,
 *   the number of parameters in domain (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 **
 * - November 2nd 2001: first version. 
 */
static struct clast_expr *clast_minmax(CloogMatrix *matrix,
				       int level, int max, int guard,
				       CloogInfos *infos)
{ int i, n;
  struct clast_reduction *r;
  
  for (i=0, n=0;i<matrix->NbRows;i++)
    if (((max && value_pos_p(matrix->p[i][level])) ||
	 (!max && value_neg_p(matrix->p[i][level]))) &&
	(!guard || value_zero_p(matrix->p[i][guard])) &&
	(value_notzero_p(matrix->p[i][0])))
      n++;
  if (!n)
    return NULL;
  r = new_clast_reduction(max ? clast_red_max : clast_red_min, n);

  for (i=0, n=0;i<matrix->NbRows;i++)
    if (((max && value_pos_p(matrix->p[i][level])) ||
	 (!max && value_neg_p(matrix->p[i][level]))) &&
	(!guard || value_zero_p(matrix->p[i][guard])) &&
	(value_notzero_p(matrix->p[i][0])))
      r->elts[n++] = clast_bound_from_constraint(matrix,i,level,infos->names);

  return &r->expr;
}


/**
 * insert_guard function:
 * This function inserts a guard in the clast.
 * A guard on an element (level) is :
 * -> the conjunction of all the existing constraints where the coefficient of
 *    this element is 0 if the element is an iterator,
 * -> the conjunction of all the existing constraints if the element isn't an
 *    iterator.
 * For instance, considering these constraints and the element j:
 * -3*i +2*j -M >= 0
 *  2*i      +M >= 0
 * this function should return 'if (2*i+M>=0) {'.
 * - matrix is the polyhedron containing all the constraints,
 * - level is the column number of the element in matrix we want to use,
 * - the infos structure gives the user some options about code printing,
 *   the number of parameters in matrix (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 **
 * - November  3rd 2001: first version. 
 * - November 14th 2001: a lot of 'purifications'. 
 * - July     31th 2002: (debug) some guard parts are no more redundants. 
 * - August   12th 2002: polyhedra union ('or' conditions) are now supported.
 * - October  27th 2005: polyhedra union ('or' conditions) are no more supported
 *                       (the need came from loop_simplify that may result in
 *                       domain unions, now it should be fixed directly in
 *                       cloog_loop_simplify).
 */
static void insert_guard(CloogMatrix *matrix, int level,
			 struct clast_stmt ***next, CloogInfos *infos)
{ 
  int i, j, k, l, guarded, minmax=-1, nb_and = 0, nb_iter ;
  char * name;
  CloogMatrix * copy ;
  struct clast_guard *g;
  Value one;

  if (matrix == NULL)
    return;

  value_init(one);
  value_set_si(one, 1);
  
    g = new_clast_guard(2 * (matrix->NbColumns-2));

    /* Well, it looks complicated because I wanted to have a particular, more
     * readable, ordering, obviously this function may be far much simpler !
     */
    copy = cloog_matrix_copy(matrix) ;
    
    nb_iter = copy->NbColumns - 2 - cloog_names_nb_parameters (infos->names) ;
 
    nb_and = 0 ;
    /* We search for guard parts. */
    for (i=1;i<=copy->NbColumns-2;i++)
    for (j=0;j<copy->NbRows;j++)
      if (value_notzero_p(copy->p[j][i]) &&
	  (value_zero_p(copy->p[j][level]) || (nb_iter < level))) {
      struct clast_term *t;
      if (i <= nb_iter)
	{ if (i <= cloog_names_nb_scattering (infos->names))
	  name = cloog_names_scattering_elt (infos->names, i - 1);
        else
	  name = cloog_names_iterator_elt (infos->names, i - cloog_names_nb_scattering (infos->names) - 1);
      }
      else
	name = cloog_names_parameter_elt (infos->names, i - nb_iter - 1);
      
      g->eq[nb_and].LHS = &(t = new_clast_term(one, name))->expr;
      if (value_zero_p(copy->p[j][0])) {
	/* put the "denominator" in the LHS */
	value_assign(t->val, copy->p[j][i]);
	value_set_si(copy->p[j][i], 1);
	g->eq[nb_and].sign = 0;
        g->eq[nb_and].RHS = clast_bound_from_constraint(copy,j,i,infos->names);
	value_assign(copy->p[j][i], t->val);
      } else {
        if (value_pos_p(copy->p[j][i])) {
	    minmax = 1;
	    g->eq[nb_and].sign = 1;
        } else {
	    minmax = 0;
	    g->eq[nb_and].sign = -1;
	}
      
        guarded = (nb_iter >= level) ? level : 0 ;
        g->eq[nb_and].RHS = clast_minmax(copy,i,minmax,guarded,infos) ;
      }
      nb_and ++ ;

      /* 'elimination' of the current constraint, this avoid to use one
       * constraint more than once. The current line is always eliminated,
       * and the next lines if they are in a min or a max.
       */
      for (k=i;k<=copy->NbColumns-2;k++)
	value_set_si(copy->p[j][k], 0) ;
    
      if (minmax != -1)
      for (l=j+1;l<copy->NbRows;l++)
	if (((minmax == 1) && value_pos_p(copy->p[l][i])) ||
	    ((minmax == 0) && value_neg_p(copy->p[l][i])))
      for (k=i;k<=copy->NbColumns-2;k++)
	value_set_si(copy->p[l][k], 0) ;
    }
    cloog_matrix_free(copy) ;
  
  g->n = nb_and;
  if (nb_and) {
    **next = &g->stmt;
    *next = &g->then;
  } else
    free_clast_stmt(&g->stmt);
  
  value_clear(one);
  return;
}


/* Computes x, y and g such that g = gcd(a,b) and a*x+b*y = g */
static void Euclid(Value a, Value b, Value *x, Value *y, Value *g)
{
    Value c, d, e, f, tmp;

    value_init(c);
    value_init(d);
    value_init(e);
    value_init(f);
    value_init(tmp);
    value_absolute(c, a);
    value_absolute(d, b);
    value_set_si(e, 1);
    value_set_si(f, 0);
    while(value_pos_p(d)) {
	value_division(tmp, c, d);
	value_multiply(tmp, tmp, f);
	value_subtract(e, e, tmp);
	value_division(tmp, c, d);
	value_multiply(tmp, tmp, d);
	value_subtract(c, c, tmp);
	value_swap(c, d);
	value_swap(e, f);
    }
    value_assign(*g, c);
    if (value_zero_p(a))
	value_set_si(*x, 0);
    else if (value_pos_p(a))
	value_assign(*x, e);
    else value_oppose(*x, e);
    if (value_zero_p(b))
	value_set_si(*y, 0);
    else {
	value_multiply(tmp, a, *x);
	value_subtract(tmp, c, tmp);
	value_division(*y, tmp, b);
    }
    value_clear(c);
    value_clear(d);
    value_clear(e);
    value_clear(f);
    value_clear(tmp);
}

/**
 * insert_modulo_guard:
 * This function inserts a modulo guard corresponding to an equality.
 * See insert_equality.
 * - matrix is the polyhedron containing all the constraints,
 * - num is the line number of the constraint in matrix we want to print,
 * - level is the column number of the element in matrix we want to use,
 * - the infos structure gives the user some options about code printing,
 *   the number of parameters in matrix (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 */
static void insert_modulo_guard(CloogMatrix *matrix, int num, int level,
				struct clast_stmt ***next, CloogInfos *infos)
{
  int i, j, k, nb_elts = 0, len, nb_iter, in_stride = 0, nb_par;
  Vector *line_vector;
  Value *line, val, x, y, g;

  if (value_one_p(matrix->p[num][level]) || value_mone_p(matrix->p[num][level]))
    return;

  value_init_c(val);
  value_init_c(x);
  value_init_c(y);
  value_init_c(g);

  len = matrix->NbColumns;
  nb_par = cloog_names_nb_parameters (infos->names);
  nb_iter = matrix->NbColumns - 2 - nb_par;

  line_vector = Vector_Alloc(len);
  line = line_vector->p;
  if (value_neg_p(matrix->p[num][level]))
    cloog_vector_copy(matrix->p[num]+1, line+1, len-1);
  else {
    value_set_si(val, -1);
    cloog_vector_scale(matrix->p[num]+1, line+1, val, len-1);
  }
  value_oppose(line[level], line[level]);
  assert(value_pos_p(line[level]));

  nb_elts = 0;
  for (i = nb_iter; i >= 1; --i) {
    if (i == level)
      continue;
    value_pmodulus(line[i],line[i],line[level]);
    if (value_zero_p(line[i]))
      continue;

    /* Look for an earlier variable that is also a multiple of line[level]
     * and check whether we can use the corresponding affine expression
     * to "reduce" the modulo guard, where reduction means that we eliminate
     * a variable, possibly at the expense of introducing other variables
     * with smaller index.
     */
    for (j = level-1; j >= 0; --j) {
      if (value_cmp_si(infos->equal->p[j][0], EQTYPE_EXAFFINE) != 0)
	continue;
      value_modulus(val, infos->equal->p[j][1+j], line[level]);
      if (value_notzero_p(val))
	continue;
      value_modulus(val, infos->equal->p[j][i], line[level]);
      if (value_zero_p(val))
	continue;
      for (k = j; k > i; --k) {
	if (value_zero_p(infos->equal->p[j][k]))
	  continue;
	value_modulus(val, infos->equal->p[j][k], line[level]);
	if (value_notzero_p(val))
	  break;
      }
      if (k > i)
	 continue;
      Euclid(infos->equal->p[j][i], line[level], &x, &y, &g);
      value_modulus(val, line[i], g);
      if (value_notzero_p(val))
	continue;
      value_division(val, line[i], g);
      value_oppose(val, val);
      value_multiply(val, val, x);
      value_set_si(y, 1);
      /* Add (infos->equal->p[j][i])^{-1} * line[i] times the equality */
      cloog_vector_combine(line+1, infos->equal->p[j]+1, line+1, y, val, i);
      cloog_vector_combine(line+len-nb_par-1,
			   infos->equal->p[j]+infos->equal->NbColumns-nb_par-1,
			   line+len-nb_par-1, y, val, nb_par+1);
      break;
    }
    if (j >= 0) {
      value_pmodulus(line[i],line[i],line[level]);
      assert(value_zero_p(line[i]));
      continue;
    }

    value_modulus(val,infos->stride[i-1],line[level]);
    /* We need to know if an element of the equality has not to be printed
     * because of a stride that guarantees that this element can be divided by
     * the current coefficient. Because when there is a constant element, it
     * is included in the stride calculation (more exactly in the strided
     * iterator new lower bound: the 'offset') and we have not to print it.
     */
    if (value_zero_p(val)) {
      in_stride = 1;
      continue;
    }

    nb_elts++;
  }
  for (i = nb_iter+1; i <= len-1; ++i) {
    value_pmodulus(line[i],line[i],line[level]);
    if (value_zero_p(line[i]))
      continue;
    if (i <= len-2)
      nb_elts++;
  }

  if (nb_elts || (value_notzero_p(line[len-1]) && (!in_stride))) {
    struct clast_reduction *r;
    struct clast_expr *e;
    struct clast_guard *g;
    char * name;

    r = new_clast_reduction(clast_red_sum, nb_elts+1);
    nb_elts = 0;

    /* First, the modulo guard : the iterators... */
    for (i=1;i<=nb_iter;i++) {
      if (i == level || value_zero_p(line[i]))
	continue;
      value_modulus(val,infos->stride[i-1],line[level]);
      if (value_zero_p(val))
	continue;

      if (i <= cloog_names_nb_scattering (infos->names))
	name = cloog_names_scattering_elt (infos->names, i - 1);
      else
	name = cloog_names_iterator_elt (infos->names, i - cloog_names_nb_scattering (infos->names) - 1);

      r->elts[nb_elts++] = &new_clast_term(line[i], name)->expr;
    }

    /* ...the parameters... */
    for (i=nb_iter+1;i<=len-2;i++) {
      if (value_zero_p(line[i]))
	continue;

      name = cloog_names_parameter_elt (infos->names, i - nb_iter - 1);
      r->elts[nb_elts++] = &new_clast_term(line[i], name)->expr;
    }

    /* ...the constant. */
    if (value_notzero_p(line[len-1]))
      r->elts[nb_elts++] = &new_clast_term(line[len-1], NULL)->expr;

    /* our initial computation may have been an overestimate */
    r->n = nb_elts;

    e = &new_clast_binary(clast_bin_mod, &r->expr, line[level])->expr;
    g = new_clast_guard(1);
    g->eq[0].LHS = e;
    value_set_si(val, 0);
    g->eq[0].RHS = &new_clast_term(val, NULL)->expr;
    g->eq[0].sign = 0;

    **next = &g->stmt;
    *next = &g->then;
  }

  Vector_Free(line_vector);

  value_clear_c(val);
  value_clear_c(x);
  value_clear_c(y);
  value_clear_c(g);
}


/**
 * insert_equality function:
 * This function inserts an equality 
 * constraint according to an element in the clast.
 * An equality can be preceded by a 'modulo guard'.
 * For instance, consider the constraint i -2*j = 0 and the
 * element j: pprint_equality should return 'if(i%2==0) { j = i/2 ;'.
 * - matrix is the polyhedron containing all the constraints,
 * - num is the line number of the constraint in matrix we want to print,
 * - level is the column number of the element in matrix we want to use,
 * - the infos structure gives the user some options about code printing,
 *   the number of parameters in matrix (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 **
 * - November 13th 2001: first version.
 * - June 26th 2003: simplification of the modulo guards (remove parts such as
 *                   modulo is 0, compare vivien or vivien2 with a previous
 *                   version for an idea).
 * - June 29th 2003: non-unit strides support.
 * - July 14th 2003: (debug) no more print the constant in the modulo guard when
 *                   it was previously included in a stride calculation.
 */
static void insert_equality(CloogMatrix *matrix, int num,
			    int level, struct clast_stmt ***next, CloogInfos *infos)
{
  struct clast_expr *e;
  struct clast_assignment *ass;

  insert_modulo_guard(matrix, num, level, next, infos);

  if (!clast_equal_add(infos->equal,matrix,level,num,infos))
  { /* Finally, the equality. */
		
    /* If we have to make a block by dimension, we start the block. Function
     * pprint knows if there is an equality, if this is the case, it checks
     * for the same following condition to close the brace.
     */
    if (infos->options->block) {
      struct clast_block *b = new_clast_block();
      **next = &b->stmt;
      *next = &b->body;
    }
		
    e = clast_bound_from_constraint(matrix,num,level,infos->names);
    if (level <= cloog_names_nb_scattering (infos->names))
      ass = new_clast_assignment(cloog_names_scattering_elt (infos->names, level - 1), e);
    else
	ass = new_clast_assignment
	  (cloog_names_iterator_elt (infos->names, level - cloog_names_nb_scattering (infos->names) - 1), e);

    **next = &ass->stmt;
    *next = &(**next)->next;
  }

  return;
}


/**
 * insert_for function:
 * This function inserts a for loop in the clast.
 * A loop header according to an element is the conjonction of a minimum and a
 * maximum on the element (they give the loop bounds).
 * For instance, considering these constraints and the element j:
 * i + j -9*M >= 0
 *    -j +5*M >= 0
 *     j -4*M >= 0
 * this function should return 'for (j=max(-i+9*M,4*M),j<=5*M;j++) {'.
 * - matrix is the polyhedron containing all the constraints,
 * - level is the column number of the element in matrix we want to use,
 * - the infos structure gives the user some options about code printing,
 *   the number of parameters in matrix (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 **
 * - July 2nd 2002: first version (pick from pprint function). 
 * - March 6th 2003: infinite domain support. 
 * - June 29th 2003: non-unit strides support.
 */
static void insert_for(CloogMatrix *matrix, int level,
		       struct clast_stmt ***next, CloogInfos *infos)
{
  char * iterator ;
  struct clast_expr *e1;
  struct clast_expr *e2;
  struct clast_assignment *ass;
  
  if (level <= cloog_names_nb_scattering (infos->names))
    iterator = cloog_names_scattering_elt (infos->names, level - 1);
  else
    iterator = cloog_names_iterator_elt 
      (infos->names, level - cloog_names_nb_scattering (infos->names) - 1);
  
  e1 = clast_minmax(matrix,level,1,0,infos) ;
  e2 = clast_minmax(matrix,level,0,0,infos) ;

  /* If min and max are not equal there is a 'for' else, there is a '='.
   * In the special case e1 = e2 = NULL, this is an infinite loop
   * so this is not a '='.
   */
  if (!clast_expr_equal(e1, e2) || !infos->options->otl || (!e1 && !e2)) {
    struct clast_for *f = new_clast_for(iterator, e1, e2, infos->stride[level-1]);
    **next = &f->stmt;
    *next = &f->body;
  }
  else if (!clast_equal_add(infos->equal,matrix,level,ONE_TIME_LOOP,infos)) {
    if (infos->options->block) {
	struct clast_block *b = new_clast_block();
	**next = &b->stmt;
	*next = &b->body;
    }
    ass = new_clast_assignment(iterator, e1);
    **next = &ass->stmt;
    *next = &(**next)->next;
  }

  return;    
}


/**
 * insert_scalar function:
 * This function inserts assignments to the scalar values
 * that follows the level (level). It finds by scanning (loop) by inner level,
 * the first CloogBlock data structure (at this step, all blocks has the same
 * scalar vector information after (level)), and prints all the adjacent
 * scalar values following (level), if it is required by options in (info).
 * - loop is the loop structure to begin the search for a block,
 * - level is the current loop level,
 * - scalar points to the number of scalar values already visited,
 * - the infos structure gives the user options about code printing and more.
 **
 * - September 12th 2005: first version.
 */
static void insert_scalar(CloogLoop *loop, int level, int *scalar,
			  struct clast_stmt ***next, CloogInfos *infos)
{ 
  struct clast_block *b;
  struct clast_term *t;
  
  if ((!infos->options->csp) &&
      (level+(*scalar) <= infos->nb_scattdims) &&
      (infos->scaldims[level+(*scalar)-1]))
    {
      while (cloog_loop_block (loop) == NULL)
	loop = cloog_loop_inner (loop) ;

      while ((level+(*scalar) <= infos->nb_scattdims) &&
	     (infos->scaldims[level+(*scalar)-1])) { 
	if (infos->options->block) {
	  b = new_clast_block();
	  **next = &b->stmt;
	  *next = &b->body;
	}
      
	t = new_clast_term (cloog_loop_block (loop)->scaldims[*scalar], NULL);
	**next = &new_clast_assignment(cloog_names_scalar_elt (infos->names, *scalar),
				       &t->expr)->stmt;
	*next = &(**next)->next;
	(*scalar) ++ ;
      
      }
    }
  
  return;
}


/**
 * insert_block function:
 * This function inserts a statement block.
 * - block is the statement block,
 * - level is the number of loops enclosing the statement,
 * - the infos structure gives the user some options about code printing,
 *   the number of parameters in domain (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 **
 * - September 21th 2003: first version (pick from pprint function). 
 */
static void insert_block(CloogBlock *block, int level,
			 struct clast_stmt ***next, CloogInfos *infos)
{
    CloogStatement * statement ;
    struct clast_stmt *subs;
   
    if (!block)
	return;

    for (statement = cloog_block_stmt (block); statement;
	 statement = cloog_statement_next (statement)) {
      if (infos->options->cpp == 0)
	subs = clast_equal(infos);
      else
	subs = clast_equal_cpp(level,infos);

      **next = &new_clast_user_stmt(statement, subs)->stmt;
      *next = &(**next)->next;
    }
}

static CloogMatrix *
cloog_simplify_domain_matrix_with_equalities (CloogDomain *domain, int level,
					      CloogMatrix *equal, int nb_parameters)
{
  CloogMatrix *temp, *res;

  /* FIXME: the access to ->_polyhedron is a hack to avoid exporting
     the CloogMatrix in a .h file: the whole clast.c file should be
     rewritten specifically to Polylib and PPL.  */

  temp = cloog_upol_domain2matrix (cloog_domain_upol (domain));
  cloog_matrix_normalize (temp, level);
  res = cloog_matrix_simplify (temp, equal, level, nb_parameters);
  cloog_matrix_free(temp);

  return res;
}


/**
 * insert_loop function:
 * This function concerts the content of a CloogLoop structure (loop) into a
 * clast_stmt (inserted at **next).
 * The iterator (level) of
 * the current loop is given by 'level': this is the column number of the
 * domain corresponding to the current loop iterator. The data of a loop are
 * written in this order:
 * 1. The guard of the loop, i.e. each constraint in the domain that do not
 *    depend on the iterator (when the entry in the column 'level' is 0).
 * 2. The iteration domain of the iterator, given by the constraints in the
 *    domain depending on the iterator, i.e.:
 *    * an equality if the iterator has only one value (possibly preceded by
 *      a guard verifying if this value is integral), *OR*
 *    * a loop from the minimum possible value of the iterator to the maximum
 *      possible value.
 * 3. The included statement block.
 * 4. The inner loops (recursive call).
 * 5. The following loops (recursive call).
 * - level is the recursion level or the iteration level that we are printing,
 * - the infos structure gives the user some options about code printing,
 *   the number of parameters in domain (nb_par), and the arrays of iterator
 *   names and parameters (iters and params). 
 **
 * - November   2nd 2001: first version. 
 * - March      6th 2003: infinite domain support. 
 * - April     19th 2003: (debug) NULL loop support. 
 * - June      29th 2003: non-unit strides support.
 * - April     28th 2005: (debug) level is level+equality when print statement!
 * - June      16th 2005: (debug) the N. Vasilache normalization step has been
 *                        added to avoid iteration duplication (see DaeGon Kim
 *                        bug in cloog_program_generate). Try vasilache.cloog
 *                        with and without the call to cloog_matrix_normalize,
 *                        using -f 8 -l 9 options for an idea.
 * - September 15th 2005: (debug) don't close equality braces when unnecessary.
 * - October   16th 2005: (debug) scalar value is saved for next loops.
 */
static void insert_loop(CloogLoop * loop, int level, int scalar,
			struct clast_stmt ***next, CloogInfos *infos)
{
    int i, equality=0, scalar_level;
    CloogMatrix * matrix;
    struct clast_stmt **top = *next;

    /* It can happen that loop be NULL when an input polyhedron is empty. */
    if (loop == NULL)
	return;

    /* The matrix has not always a shape that allows us to generate code from it,
    * thus we normalize it, we also simplify it with the matrix of equalities.
    */ 
    matrix = cloog_simplify_domain_matrix_with_equalities
      (cloog_loop_domain (loop), level, infos->equal, cloog_names_nb_parameters (infos->names));
    value_assign(infos->stride[level-1], loop->stride);

    /* First of all we have to print the guard. */
    insert_guard(matrix,level, next, infos);

    /* Then we print scalar dimensions. */ 
    scalar_level = scalar ;
    insert_scalar(loop,level,&scalar, next, infos);

    if ((matrix->NbColumns - 2 - cloog_names_nb_parameters (infos->names) >= level)) {
	/* We scan all the constraints to know in which case we are :
	 * [[if] equality] or [for].
	 */
	for (i=0;i<matrix->NbRows;i++)
	  if (value_zero_p(matrix->p[i][0]) &&
	      value_notzero_p(matrix->p[i][level]))
	{ /* If there is an equality, we can print it directly -no ambiguity-.
	   * PolyLib can give more than one equality, we use just the first one
	   * (this is a PolyLib problem, but all equalities are equivalent).
	   */
	  insert_equality(matrix,i,level, next, infos);
	  equality = 1 ;   
	  break ;
	}
      
	if (!equality)
	    insert_for(matrix, level, next, infos);
    }

    /* Finally, if there is an included statement block, print it. */
    insert_block(cloog_loop_block (loop), level+equality, next, infos);

    /* Go to the next level. */
    if (cloog_loop_inner (loop))
      insert_loop(cloog_loop_inner (loop), level+1,scalar, next, infos);

    clast_equal_del(infos->equal,level);
    cloog_matrix_free(matrix);

    /* Go to the next loop on the same level. */
    while (*top)
	top = &(*top)->next;
    if (cloog_loop_next (loop))
      insert_loop(cloog_loop_next (loop), level,scalar_level, &top,infos);
}


struct clast_stmt *cloog_clast_create(CloogProgram *program,
				      CloogOptions *options)
{
    CloogInfos *infos = ALLOC(CloogInfos);
    int i, nb_levels;
    struct clast_stmt *root = &new_clast_root(cloog_program_names (program))->stmt;
    struct clast_stmt **next = &root->next;

    infos->names    = cloog_program_names (program);
    infos->options  = options;
    infos->scaldims = cloog_program_scaldims (program);
    infos->nb_scattdims = cloog_program_nb_scattdims (program);

    /* Allocation for the array of strides, there is a +1 since the statement can
    * be included inside an external loop without iteration domain.
    */ 
    nb_levels = cloog_names_nb_scattering (cloog_program_names (program)) 
      + cloog_names_nb_iterators (cloog_program_names (program)) + 1;
    infos->stride = ALLOCN(Value, nb_levels);
    for (i = 0; i < nb_levels; ++i)
	value_init_c(infos->stride[i]);

    infos->equal = 
      cloog_matrix_alloc (nb_levels, nb_levels + cloog_names_nb_parameters (cloog_program_names (program)) + 1);
	
    insert_loop (cloog_program_loop (program), 1, 0, &next, infos);

    cloog_matrix_free(infos->equal);

    for (i = 0; i < nb_levels; ++i)
	value_clear_c(infos->stride[i]);
    free(infos->stride);
    free(infos);

    return root;
}
