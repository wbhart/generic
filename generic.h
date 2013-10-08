/*=============================================================================

    This file is part of FLINT.

    FLINT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    FLINT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FLINT; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

=============================================================================*/
/******************************************************************************

    Copyright (C) 2013 William

******************************************************************************/

#ifndef GENERIC_H
#define GENERIC_H

#include "flint.h"

#define INDEX(_ptr, _i, _size) ((char *)(_ptr) + ((_i) * (_size)))

typedef void (*bin_fn)(void *, void *, void *);
typedef void (*mono_fn)(void *);
typedef void (*unary_fn)(void *, void *);
typedef void (*fit_fn)(void *, slong length);
typedef int (*bool_fn)(void *);
typedef int (*binbool_fn)(void *, void *);
typedef void (*random_fn)(void *, flint_rand_t state);

typedef struct gRing_t
{
   bin_fn add;
   bin_fn sub;
   bin_fn mul;
   void (*init)(struct gRing_t *, void *);
   mono_fn clear;
   fit_fn fit_length;
   unary_fn set;
   unary_fn neg;
   bool_fn is_zero;
   binbool_fn equal;
   random_fn test_random;

   size_t size;
   struct gRing_t * param;
} gRing_t;

typedef void (*init_fn)(gRing_t *, void *);
   
extern gRing_t * gZZ;

/******************************************************************************

    Ring initialisation 

******************************************************************************/

void generic_init();

void generic_clear(void);

gRing_t * new_gRing(size_t size, init_fn init, mono_fn clear, unary_fn set, unary_fn neg,
                         bool_fn is_zero, binbool_fn equal, random_fn test_random, 
                                   bin_fn add, bin_fn sub, bin_fn mul);

void clear_gRing(gRing_t * R);

static __inline__
void * _new(gRing_t * R)
{
   void * r = flint_malloc(R->size);
   R->init(R->param, r);
   return r;
}

static __inline__
void _delete(gRing_t * R, void * r)
{
   R->clear(r);
   flint_free(r);
}

/******************************************************************************

    Polynomials 

******************************************************************************/

typedef struct
{
   gRing_t * R;
   void ** coeff;
   slong length;
   slong alloc;
} gPoly_t;

gRing_t * new_gPolyRing(gRing_t * R);

void clear_gPolyRing(gRing_t * R);

void gPoly_init(gRing_t * R, gPoly_t * poly);

void gPoly_fit_length(gPoly_t * poly, ulong len);

void gPoly_clear(gPoly_t * poly);

int gPoly_is_zero(gPoly_t * poly);

int gPoly_equal(gPoly_t * poly1, gPoly_t * poly2);

void gPoly_test_random(gPoly_t * poly, flint_rand_t state);

void gPoly_add(gPoly_t * r, gPoly_t * a, gPoly_t * b);

void gPoly_sub(gPoly_t * r, gPoly_t * a, gPoly_t * b);

void gPoly_mul(gPoly_t * r, gPoly_t * a, gPoly_t * b);

void gPoly_set(gPoly_t * r, gPoly_t * a);

void gPoly_neg(gPoly_t * r, gPoly_t * a);

#endif

