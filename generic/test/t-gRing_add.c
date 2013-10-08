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

    Copyright (C) 2013 William Hart

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <gmp.h>
#include "flint.h"
#include "fmpz.h"
#include "generic.h"

int
main(void)
{
    int i, result;
    flint_rand_t state;

    flint_printf("add....");
    fflush(stdout);

    flint_randinit(state);

    generic_init();

    /* add gZZ */
    for (i = 0; i < 10000 * flint_test_multiplier(); i++)
    {
        fmpz * r, * a, * b;

        a = _new(gZZ);
        b = _new(gZZ);
        r = _new(gZZ);

        gZZ->test_random(a, state);
        gZZ->test_random(b, state);
        
        gZZ->add(r, a, b);
        gZZ->sub(r, r, b);
        result = gZZ->equal(r, a);
        if (!result)
        {
           printf("FAIL:\n");
           abort();
        }

        _delete(gZZ, a);
        _delete(gZZ, b);
        _delete(gZZ, r);
    }
    
    /* add gZZ[x] */
    for (i = 0; i < 10000 * flint_test_multiplier(); i++)
    {
        gRing_t * R = new_gPolyRing(gZZ);
        
        gPoly_t * r = _new(R);
        gPoly_t * a = _new(R);
        gPoly_t * b = _new(R);
        
        R->test_random(a, state);
        R->test_random(b, state);
        
        R->add(r, a, b);
        R->sub(r, r, b);
        result = R->equal(r, a);
        if (!result)
        {
           printf("FAIL:\n");
           abort();
        }

        _delete(R, a);
        _delete(R, b);
        _delete(R, r);
        
        clear_gPolyRing(R);
    }
   
    /* add gZZ[x,y] */
    for (i = 0; i < 10000 * flint_test_multiplier(); i++)
    {
        gRing_t * R = new_gPolyRing(gZZ);
        gRing_t * S = new_gPolyRing(R);
        
        gPoly_t * r = _new(S);
        gPoly_t * a = _new(S);
        gPoly_t * b = _new(S);
        
        S->test_random(a, state);
        S->test_random(b, state);
        
        S->add(r, a, b);
        S->sub(r, r, b);
        result = S->equal(r, a);
        if (!result)
        {
           printf("FAIL:\n");
           abort();
        }

        _delete(S, a);
        _delete(S, b);
        _delete(S, r);
        
        clear_gPolyRing(S);
        clear_gPolyRing(R);
    }

    generic_clear();

    flint_randclear(state);
    flint_cleanup();
    flint_printf("PASS\n");
    return 0;
}
