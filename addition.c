/*******************************************************************************
This software was developed by employees of the National Institute of Standards
and Technology (NIST), an agency of the Federal Government. Pursuant to Title 17
United States Code Section 105, works of NIST employees are not subject to
copyright protection in the United States and are considered to be in the public
domain. Permission to freely use, copy, modify, and distribute this software and
its documentation without fee is hereby granted, provided that this notice and
disclaimer of warranty appears in all copies.

THE SOFTWARE IS PROVIDED 'AS IS' WITHOUT ANY WARRANTY OF ANY KIND, EITHER
EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY
THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND FREEDOM FROM INFRINGEMENT,
AND ANY WARRANTY THAT THE DOCUMENTATION WILL CONFORM TO THE SOFTWARE, OR ANY
WARRANTY THAT THE SOFTWARE WILL BE ERROR FREE. IN NO EVENT SHALL NIST BE LIABLE
FOR ANY DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR
CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY CONNECTED
WITH THIS SOFTWARE, WHETHER OR NOT BASED UPON WARRANTY, CONTRACT, TORT, OR
OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY PERSONS OR PROPERTY OR
OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED FROM, OR AROSE OUT OF THE
RESULTS OF, OR USE OF, THE SOFTWARE OR SERVICES PROVIDED HEREUNDER.

Distributions of NIST software should also include copyright and licensing
statements of any third-party software that are legally bundled with the code
in compliance with the conditions of those licenses.
*******************************************************************************/

/**
   \file   addition.c
   \brief  Explore floating-point rounding errors
   Addition is an associative operation, meaning that \$a+b+c = (a+b)+c = a+(b+c)\$.
   This program demonstrates that, for \$ b=1 \$ and \$ c=-1 \$, this only holds
   true for values of \$ a \$ that are powers of 2.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

/* Define functions to convert float to binary
   based on http://www.exploringbinary.com/converting-floating-point-numbers-to-binary-strings-in-c/
   by Rick Regan (6 May 2009)
*/

/* FP2BIN_STRING_MAX covers the longest binary string (2^-1074 plus "0." and string terminator) */
#define FP2BIN_STRING_MAX 1077

void fp2bin_i(double fp_int, char* binString)
{
        int bitCount = 0;
        int i;
        char binString_temp[FP2BIN_STRING_MAX];

        do {
                binString_temp[bitCount++] = '0' + (int)fmod(fp_int,2);
                fp_int = floor(fp_int/2);
        } while (fp_int > 0);

         /* Reverse the binary string */
         for (i=0; i<bitCount; i++)
                 binString[i] = binString_temp[bitCount-i-1];

         binString[bitCount] = 0; /* Null terminator */
}

void fp2bin_f(double fp_frac, char* binString)
{
    int bitCount = 0;
    double fp_int;

    while (fp_frac > 0) {
    	fp_frac*=2;
        fp_frac = modf(fp_frac,&fp_int);
        binString[bitCount++] = '0' + (int)fp_int;
    }
    binString[bitCount] = 0; /* Null terminator */
}

void fp2bin(double fp, char* binString)
{
        double fp_int, fp_frac;

        /* Separate integer and fractional parts */
        fp_frac = modf(fp,&fp_int);

        /* Convert integer part, if any */
        if (fp_int != 0)
                fp2bin_i(fp_int,binString);
        else
                strcpy(binString,"0");

        strcat(binString,"."); /* Radix point */

        /* Convert fractional part, if any */
        if (fp_frac != 0)
                fp2bin_f(fp_frac,binString+strlen(binString)); /* Append */
        else
                strcpy(binString+strlen(binString),"0");
}

/* Test associative addition of 1, -1, and a number less than 1 */
int main()
{
    double x;
    int check;
    char binStrA[FP2BIN_STRING_MAX] = {0};
    char binStrD[FP2BIN_STRING_MAX] = {0};
	#ifdef __GNU_MP__
    mpfr_t a, b, c, d, l, r;
    #else
    float *a, *b, *c, *d, *l, *r;
    #endif

    const char i[36] = {'a', '\0',
                        'b', 'i', 'n', '(', 'a', ')', '\0',
                        'b', 'i', 'n', '(', 'a', '+', 'b', '+', 'c', ')', '\0',
                        '(', 'a', '+', 'b', ')', '+', 'c', '\0',
                        'a', '+', '(', 'b', '+', 'c', ')', '\0'};

    printf("|  %-12s %-29s| %-29s   %-12s  %-12s| equal |\n",
               i+0,  i+2,    i+9,  i+20, i+28);

    for (x = 1; x < 17; x++) {
        check = 0;

		#ifdef __GNU_MP__
        mpfr_set_default_prec(4*sizeof(float)); /* expect 7 digits of precision */

        /* initialize variables */
        mpfr_init_set_d(a, 1. / x, MPFR_RNDD);
        mpfr_init_set_d(b, 1, MPFR_RNDD);
        mpfr_init_set_d(c,-1, MPFR_RNDD);
        mpfr_init_set_d(d, 0, MPFR_RNDD);
        mpfr_init_set_d(l, 0, MPFR_RNDD);
        mpfr_init_set_d(r, 0, MPFR_RNDD);

        /* compute associative expressions */
        mpfr_add(d, a, b, MPFR_RNDD);
        mpfr_add(d, d, c, MPFR_RNDD);

        mpfr_add(l, a, b, MPFR_RNDD);
        mpfr_add(l, l, c, MPFR_RNDD);

        mpfr_add(r, b, c, MPFR_RNDD);
        mpfr_add(r, r, a, MPFR_RNDD);

        /* report values */
        fp2bin(mpfr_get_flt(a, MPFR_RNDD), binStrA);
        fp2bin(mpfr_get_flt(d, MPFR_RNDD), binStrD);

        if (mpfr_get_flt(d, MPFR_RNDD) == mpfr_get_flt(l, MPFR_RNDD) &&
            mpfr_get_flt(l, MPFR_RNDD) == mpfr_get_flt(r, MPFR_RNDD) ) {
            check = 1;
        }

        printf("| %12.9f  %-29s| %-29s  %12.9f  %12.9f | %-5d |\n",
               mpfr_get_flt(a, MPFR_RNDD), binStrA, binStrD,
               mpfr_get_flt(l, MPFR_RNDD), mpfr_get_flt(r, MPFR_RNDD), check);

        /* clean up */
        mpfr_clear(a);
        mpfr_clear(b);
        mpfr_clear(c);
        mpfr_clear(d);
        mpfr_clear(l);
        mpfr_clear(r);

        #else
        /* initialize variables */
        a = (float*)malloc(sizeof(float));
        b = (float*)malloc(sizeof(float));
        c = (float*)malloc(sizeof(float));
        d = (float*)malloc(sizeof(float));
        l = (float*)malloc(sizeof(float));
        r = (float*)malloc(sizeof(float));

        *a = 1. / x;
        *b = 1;
        *c =-1;

        /* compute associative expressions */
        *d =  *a + *b + *c;
        *l = (*a + *b)+ *c;
        *r =  *a +(*b + *c);

        /* report values */
        fp2bin(*a, binStrA);
        fp2bin(*d, binStrD);
		if (*d == *l && *l == *r) {
            check = 1;
        }
        printf("| %12.9f  %-29s| %-29s  %12.9f  %12.9f | %-5d |\n",
                   *a, binStrA, binStrD, *l,    *r,    check);

        /* clean up */
        free(a);
        free(b);
        free(c);
        free(d);
        free(l);
        free(r);

		#endif
    }

	#ifdef __GNU_MP__
    mpfr_free_cache();
    #endif

    return 0;
}
