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

/******************************************************************************

    Ring

******************************************************************************/

gRing_t * new_gPolyRing(gRing_t * R)
{
   gRing_t * r = new_gRing(sizeof(gPoly_t), (init_fn) gPoly_init, (mono_fn) gPoly_clear,
                (unary_fn) gPoly_set, (unary_fn) gPoly_neg, 
                (bool_fn) gPoly_is_zero, (binbool_fn) gPoly_equal, 
                (random_fn) gPoly_test_random,
                  (bin_fn) gPoly_add, (bin_fn) gPoly_sub, (bin_fn) gPoly_mul);

   r->fit_length = (fit_fn) gPoly_fit_length;
   r->param = R;

   return r;
}

void clear_gPolyRing(gRing_t * R)
{
   clear_gRing(R);
}

/******************************************************************************

    Elements

******************************************************************************/

void gPoly_init(gRing_t * R, gPoly_t * poly)
{
   poly->coeff = NULL;
   poly->alloc = 0;
   poly->length = 0;
   poly->R = R;
}

void gPoly_clear(gPoly_t * poly)
{
   if (poly->coeff)
   {
      slong i;
      size_t size = poly->R->size;

      for (i = 0; i < poly->alloc; i++)
         poly->R->clear(INDEX(poly->coeff, i, size));

      flint_free(poly->coeff);
   }
}

void gPoly_normalise(gPoly_t * poly)
{
   slong len = poly->length;
   size_t size = poly->R->size;

   while (len && poly->R->is_zero(INDEX(poly->coeff, len - 1, size)))
      len--;

   poly->length = len;
}

void gPoly_truncate(gPoly_t * poly, slong newlen)
{
    size_t size = poly->R->size;
    
    if (poly->length > newlen)
    {
        slong i;
        for (i = newlen; i < poly->length; i++)
           poly->R->clear(INDEX(poly->coeff, i, size));

        poly->length = newlen;
        gPoly_normalise(poly);
    }  
}

void gPoly_realloc(gPoly_t * poly, slong alloc)
{
    init_fn init = poly->R->init;
    size_t size = poly->R->size;
    
    if (alloc == 0) /* Clear up, reinitialise */
    {
        gPoly_clear(poly);
        gPoly_init(poly->R, poly);
        return;
    }

    if (poly->alloc) /* Realloc */
    {
        gPoly_truncate(poly, alloc);

        poly->coeff = (void *) flint_realloc(poly->coeff, alloc * size);
        if (alloc > poly->alloc)
        {
           slong i;
           
           for (i = poly->alloc; i < alloc; i++)
              init(poly->R->param, INDEX(poly->coeff, i, size));
        }
    }
    else /* Nothing allocated already so do it now */
    {
        slong i;
        
        poly->coeff = (void *) flint_malloc(alloc * size);
        
        for (i = 0; i < alloc; i++)
           init(poly->R->param, INDEX(poly->coeff, i, size));
    }

    poly->alloc = alloc;
}


void gPoly_fit_length(gPoly_t * poly, ulong len)
{
   if (len > poly->alloc)
   {
      if (len < 2 * poly->alloc)
         len = 2 * poly->alloc;
      gPoly_realloc(poly, len);
   }
}

int gPoly_is_zero(gPoly_t * poly)
{
   return poly->length == 0;
}

int gPoly_equal(gPoly_t * poly1, gPoly_t * poly2)
{
   slong i;
   binbool_fn equal = poly1->R->equal;
   size_t size = poly1->R->size;

   if (poly1->length != poly2->length)
      return 0;

   for (i = 0; i < poly1->length; i++)
      if (!equal(INDEX(poly1->coeff, i, size), INDEX(poly2->coeff, i, size)))
         return 0;

   return 1;
}

void gPoly_test_random(gPoly_t * poly, flint_rand_t state)
{
   slong len = n_randint(state, 5);
   slong i;
   size_t size = poly->R->size;
    
   gPoly_fit_length(poly, len);
   for (i = 0; i < len; i++)
      poly->R->test_random(INDEX(poly->coeff, i, size), state);

   poly->length = len;
   gPoly_normalise(poly);
}

void gPoly_add(gPoly_t * r, gPoly_t * a, gPoly_t * b)
{
   slong len1 = a->length;
   slong len2 = b->length;
   slong max = FLINT_MAX(len1, len2);
   slong min = FLINT_MIN(len1, len2);
   slong i;
   size_t size = r->R->size;
    
   bin_fn add = r->R->add;
   unary_fn set = r->R->set;

   gPoly_fit_length(r, max);

   for (i = 0; i < min; i++)   /* add up to the length of the shorter poly */
      add(INDEX(r->coeff, i, size), INDEX(a->coeff, i, size), INDEX(b->coeff, i, size));

   if (a != r)           /* copy any remaining coefficients from poly1 */
      for (i = min; i < len1; i++)
         set(INDEX(r->coeff, i, size), INDEX(a->coeff, i, size));

   if (b != r)           /* copy any remaining coefficients from poly2 */
      for (i = min; i < len2; i++)
         set(INDEX(r->coeff, i, size), INDEX(b->coeff, i, size));

   r->length = max;
}

void gPoly_sub(gPoly_t * r, gPoly_t * a, gPoly_t * b)
{
   slong len1 = a->length;
   slong len2 = b->length;
   slong max = FLINT_MAX(len1, len2);
   slong min = FLINT_MIN(len1, len2);
   slong i;
   size_t size = r->R->size;
    
   bin_fn sub = r->R->sub;
   unary_fn set = r->R->set;
   unary_fn neg = r->R->neg;

   gPoly_fit_length(r, max);

   for (i = 0; i < min; i++)   /* add up to the length of the shorter poly */
      sub(INDEX(r->coeff, i, size), INDEX(a->coeff, i, size), INDEX(b->coeff, i, size));

   if (a != r)           /* copy any remaining coefficients from poly1 */
      for (i = min; i < len1; i++)
         set(INDEX(r->coeff, i, size), INDEX(a->coeff, i, size));

   if (b != r)           /* copy any remaining coefficients from poly2 */
      for (i = min; i < len2; i++)
         neg(INDEX(r->coeff, i, size), INDEX(b->coeff, i, size));

   r->length = max;
   gPoly_normalise(r);
}

void gPoly_mul(gPoly_t * r, gPoly_t * a, gPoly_t * b)
{
   slong len1 = a->length;
   slong len2 = b->length;
   slong lenr = len1 + len2 - 1;
   slong i, j;
   size_t size = r->R->size;
    
   bin_fn add = r->R->add;
   bin_fn mul = r->R->mul;
   
   void * temp = _new(r->R);
   
   if (len1 == 0 || len2 == 0)
   {
      r->length = 0;
      return;
   }

   gPoly_fit_length(r, lenr);

   for (i = 0; i < len1; i++)   
      mul(INDEX(r->coeff, i, size), INDEX(a->coeff, i, size), INDEX(b->coeff, 0, size));

   for (j = 1; j < len2; j++)
      mul(INDEX(r->coeff, j + len1 - 1, size), INDEX(a->coeff, len1 - 1, size), INDEX(b->coeff, j, size));

   for (i = 0; i < len1 - 1; i++)
   {
      for (j = 1; j < len2; j++)
      {
         mul(temp, INDEX(a->coeff, i, size), INDEX(b->coeff, j, size));
         add(INDEX(r->coeff, i + j, size), INDEX(r->coeff, i + j, size), temp);
      }
   }

   r->length = lenr;
}

void gPoly_set(gPoly_t * r, gPoly_t * a)
{
   slong len1 = a->length, i;
   size_t size = r->R->size;
    
   unary_fn set = r->R->set;

   gPoly_fit_length(r, len1);

   for (i = 0; i < len1; i++)   /* add up to the length of the shorter poly */
      set(INDEX(r->coeff, i, size), INDEX(a->coeff, i, size));

   r->length = len1;
}

void gPoly_neg(gPoly_t * r, gPoly_t * a)
{
   slong len1 = a->length, i;
   size_t size = r->R->size;
    
   unary_fn neg = r->R->neg;

   gPoly_fit_length(r, len1);

   for (i = 0; i < len1; i++)   /* add up to the length of the shorter poly */
      neg(INDEX(r->coeff, i, size), INDEX(a->coeff, i, size));

   r->length = len1;
}