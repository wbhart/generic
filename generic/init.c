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

#include "generic.h"
#include "fmpz.h"

gRing_t * gZZ;

gRing_t * new_gRing(size_t size, init_fn init, mono_fn clear, unary_fn set, unary_fn neg, 
                 bool_fn is_zero, binbool_fn equal, random_fn test_random, 
                            bin_fn add, bin_fn sub, bin_fn mul)
{
   gRing_t * r = flint_malloc(sizeof(gRing_t));

   r->size = size;
   
   r->init = init;
   r->clear = clear;
   r->set = set;
   r->neg = neg;
   r->is_zero = is_zero;
   r->equal = equal;
   r->test_random = test_random;
   r->add = add;
   r->sub = sub;
   r->mul = mul;

   return r;
}

void clear_gRing(gRing_t * R)
{
   flint_free(R);
}

void gZZ_test_random(fmpz_t f, flint_rand_t state)
{
   fmpz_randtest(f, state, 200);
}

extern fmpz * __new_fmpz();

fmpz * _new_gZZ(gRing_t * R)
{
   fmpz * f = __new_fmpz();
   fmpz_init(f);
   return f;
}

extern void __free_fmpz(fmpz * f);

void _delete_gZZ(fmpz * f)
{
   fmpz_clear(f);
   __free_fmpz(f);
}

void gZZ_init(gRing_t * R, fmpz * f)
{
   fmpz_init(f);
}

void generic_init(void)
{
   gZZ = new_gRing(sizeof(fmpz), (init_fn) gZZ_init, (mono_fn) fmpz_clear,
            (unary_fn) fmpz_set, (unary_fn) fmpz_neg, 
        (bool_fn) fmpz_is_zero, (binbool_fn) fmpz_equal, (random_fn) gZZ_test_random,
                          (bin_fn) fmpz_add, (bin_fn) fmpz_sub, (bin_fn) fmpz_mul);  
}

void generic_clear(void)
{
   clear_gRing(gZZ);
}