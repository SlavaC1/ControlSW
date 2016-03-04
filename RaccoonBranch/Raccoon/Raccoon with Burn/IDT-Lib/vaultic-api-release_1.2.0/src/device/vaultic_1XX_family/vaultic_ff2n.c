/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */
/*-----------------------------------------------------------------------*/
/* Binary Field Implementation - ff2n.c (Polynomial Basis)               */
/* Author : Dang Nguyen Duc, nguyenduc@icu.ac.kr  	                 */
/* Date : 11/17/2001                                                     */ 
/* Note : All algorithms used in this program are considered as classical*/ 
/*        algorithms for polynomial arithmetic. All of them are refered  */	
/*        to "Handbook of Applied Cryptography" by Alfred Menezes        */
/* Version : 0.1a 						         */	
/* Change  :							         */	
/* To Do   :                                                             */
/*-----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "vaultic_ff2n.h"

/* Squaring Precomputation */
static const unsigned short T[256] = {
    	0x0000, 0x0001, 0x0004, 0x0005, 0x0010, 0x0011, 0x0014, 0x0015, 0x0040, 0x0041,
    	0x0044, 0x0045, 0x0050, 0x0051, 0x0054, 0x0055, 0x0100, 0x0101, 0x0104, 0x0105,
    	0x0110, 0x0111, 0x0114, 0x0115, 0x0140, 0x0141, 0x0144, 0x0145, 0x0150, 0x0151,
    	0x0154, 0x0155, 0x0400, 0x0401, 0x0404, 0x0405, 0x0410, 0x0411, 0x0414, 0x0415,
    	0x0440, 0x0441, 0x0444, 0x0445, 0x0450, 0x0451, 0x0454, 0x0455, 0x0500, 0x0501,
    	0x0504, 0x0505, 0x0510, 0x0511, 0x0514, 0x0515, 0x0540, 0x0541, 0x0544, 0x0545,
    	0x0550, 0x0551, 0x0554, 0x0555, 0x1000, 0x1001, 0x1004, 0x1005, 0x1010, 0x1011,
    	0x1014, 0x1015, 0x1040, 0x1041, 0x1044, 0x1045, 0x1050, 0x1051, 0x1054, 0x1055,
    	0x1100, 0x1101, 0x1104, 0x1105, 0x1110, 0x1111, 0x1114, 0x1115, 0x1140, 0x1141,
    	0x1144, 0x1145, 0x1150, 0x1151, 0x1154, 0x1155, 0x1400, 0x1401, 0x1404, 0x1405,
    	0x1410, 0x1411, 0x1414, 0x1415, 0x1440, 0x1441, 0x1444, 0x1445, 0x1450, 0x1451,
    	0x1454, 0x1455, 0x1500, 0x1501, 0x1504, 0x1505, 0x1510, 0x1511, 0x1514, 0x1515,
    	0x1540, 0x1541, 0x1544, 0x1545, 0x1550, 0x1551, 0x1554, 0x1555, 0x4000, 0x4001,
    	0x4004, 0x4005, 0x4010, 0x4011, 0x4014, 0x4015, 0x4040, 0x4041, 0x4044, 0x4045,
    	0x4050, 0x4051, 0x4054, 0x4055, 0x4100, 0x4101, 0x4104, 0x4105, 0x4110, 0x4111,
    	0x4114, 0x4115, 0x4140, 0x4141, 0x4144, 0x4145, 0x4150, 0x4151, 0x4154, 0x4155,
    	0x4400, 0x4401, 0x4404, 0x4405, 0x4410, 0x4411, 0x4414, 0x4415, 0x4440, 0x4441,
    	0x4444, 0x4445, 0x4450, 0x4451, 0x4454, 0x4455, 0x4500, 0x4501, 0x4504, 0x4505,
    	0x4510, 0x4511, 0x4514, 0x4515, 0x4540, 0x4541, 0x4544, 0x4545, 0x4550, 0x4551,
    	0x4554, 0x4555, 0x5000, 0x5001, 0x5004, 0x5005, 0x5010, 0x5011, 0x5014, 0x5015,
    	0x5040, 0x5041, 0x5044, 0x5045, 0x5050, 0x5051, 0x5054, 0x5055, 0x5100, 0x5101,
    	0x5104, 0x5105, 0x5110, 0x5111, 0x5114, 0x5115, 0x5140, 0x5141, 0x5144, 0x5145,
    	0x5150, 0x5151, 0x5154, 0x5155, 0x5400, 0x5401, 0x5404, 0x5405, 0x5410, 0x5411,
    	0x5414, 0x5415, 0x5440, 0x5441, 0x5444, 0x5445, 0x5450, 0x5451, 0x5454, 0x5455,
    	0x5500, 0x5501, 0x5504, 0x5505, 0x5510, 0x5511, 0x5514, 0x5515, 0x5540, 0x5541,
    	0x5544, 0x5545, 0x5550, 0x5551, 0x5554, 0x5555 };
    	
/* Utility function */
int ff2n_swap(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE)
{
	/* a(x) <--> b(x) */
	FF2N_ELT temp;
		
	while(COEFF_SIZE--){
		temp = a_coeff[COEFF_SIZE];
		a_coeff[COEFF_SIZE] = b_coeff[COEFF_SIZE];
		b_coeff[COEFF_SIZE] = temp;
	}
	
	return 0;
}

int ff2n_is_unit(FF2N_ELT a_coeff[], UINT COEFF_SIZE)
{
	/* return 1 if a(x) = 1, otherwise return 0 */
	register UINT i;
	
	for(i=1;i<COEFF_SIZE;i++)
		if(a_coeff[i])
			return 0;
	
	return (a_coeff[0] == 1);
}

int ff2n_is_zero(FF2N_ELT a_coeff[], UINT COEFF_SIZE)
{
	/* return 1 if a(x) = 0, otherwise return 0 */
	register UINT i;
	
	for(i=0;i<COEFF_SIZE;i++)
		if(a_coeff[i])
			return 0;
	
	return 1;
}

int ff2n_is_equal(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE)
{
	/* return 1 if a(x) = b(x), otherwise return 0 */
	register UINT i;
	
	for(i=0;i<COEFF_SIZE;i++)
		if(a_coeff[i] != b_coeff[i])
			return 0;
	
	return 1;
}

FF2N_ELT ff2n_ranfunc(FF2N_ELT min, FF2N_ELT max)
{
	static unsigned seeded = 0;
	register UINT i;
	double f;

	if (!seeded){
		/* seed with system time */
		srand((unsigned)time(NULL));
		
		i = rand() & 0xFF;
		while (i--)
			rand();
		seeded = 1;
	}
	f = (double)rand() / RAND_MAX * max;
	return (min + (UINT)f);
}

/* main functions */
/*---------------------------------------------------------------------*/
/* ff2n_get_bit : Get k-th coefficient				       */
/* input  : a(x), k						       */
/* ouput  : a(k)						       */
/*---------------------------------------------------------------------*/
int ff2n_get_bit(FF2N_ELT a_coeff[], UINT k, UINT COEFF_SIZE)
{
	register UINT i, j;
	register FF2N_ELT mask = WORD_LOW_BITMASK;
	
	i = k / BITS_PER_WORD;
	j = k % BITS_PER_WORD;
	
	mask = mask << j;
	
	if(a_coeff[i] & mask)
		return 1;

	return 0;
}
/*---------------------------------------------------------------------*/
/* ff2n_set_bit : set k-th coefficient				       */
/* input  : a(x), k						       */
/* ouput  : a(x) such that a_k = 1				       */
/*---------------------------------------------------------------------*/
int ff2n_set_bit(FF2N_ELT a_coeff[], UINT k, UINT COEFF_SIZE)
{
	register UINT i, j;
	register FF2N_ELT mask = WORD_LOW_BITMASK;
	
	i = k / BITS_PER_WORD;
	j = k % BITS_PER_WORD;
	
	mask = mask << j;
	
	a_coeff[i] |= mask;
			
	return 0;
}
/*---------------------------------------------------------------------*/
/* ff2n_clear_bit: Clear k-th coefficient			       */
/* input   : a(x), k						       */
/* ouput   : a(x) such that a_k = 0				       */
/*---------------------------------------------------------------------*/
int ff2n_clear_bit(FF2N_ELT a_coeff[], UINT k, UINT COEFF_SIZE)
{
	register UINT i, j;
	register FF2N_ELT mask = WORD_LOW_BITMASK;
	
	i = k / BITS_PER_WORD;
	j = k % BITS_PER_WORD;
	
	mask = mask << j;
	mask = ~mask;
	
	a_coeff[i] &= mask;
			
	return 0;
}  		
/*---------------------------------------------------------------------*/
/* ff2n_kMul   : left shift memory k bits or a(x) = a(x) * x^k         */  
/* input  : b(x)                                                       */
/* output : a(x) = b(x) * x^k                                          */
/*---------------------------------------------------------------------*/
int ff2n_kMul(FF2N_ELT a_coeff[], UINT a_COEFF_SIZE, FF2N_ELT b_coeff[], UINT b_COEFF_SIZE, UINT k)
{
	register UINT i;
	register UINT l;
	register UINT temp_k = k;
	register FF2N_ELT mask = WORD_HI_BITMASK;  
	FF2N_ELT temp[MAX_COEFF_SIZE];
	register UINT carry = 0;
	register UINT nextcarry = 0;

	ff2n_set_equal(temp, b_coeff, b_COEFF_SIZE);
	ff2n_set_zero(a_coeff, a_COEFF_SIZE);
	ff2n_set_equal(a_coeff, temp, b_COEFF_SIZE);
	
	if(k == 0)
		return 0;
	
	/* a WORD-LONG left shift */
	l = 0;
	while(temp_k / BITS_PER_WORD){
		for(i=a_COEFF_SIZE-1;i>l;i--){
			a_coeff[i] = a_coeff[i-1];
			a_coeff[i-1] = 0;
		}
		l++;
		temp_k -= BITS_PER_WORD;
	}
	if(temp_k == 0)
		return 0; /* no more to shift */
	/* less than a WORD-LONG left shift */
	l = BITS_PER_WORD - temp_k;
	
	for(i=1;i<k;i++) /* Construct mask */
		mask = (mask >> 1) | WORD_HI_BITMASK;
		
	for(i=k/BITS_PER_WORD;i<a_COEFF_SIZE;i++){
		nextcarry = (a_coeff[i] & mask) >> l;
		a_coeff[i] <<= temp_k;
		a_coeff[i] |= carry;
		carry = nextcarry;

	}
	
	return 0;
}
/*---------------------------------------------------------------------*/
/* ff2n_kDiv    : Divided by x^k or right shift k bits                 */  
/* input   : b(x)                                                      */
/* output  : a(x) = b(x) / x^k                                         */
/*---------------------------------------------------------------------*/
int  ff2n_kDiv(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE,  UINT k)
{
	register UINT i;
	register UINT l;
	register UINT temp_k = k;
	register FF2N_ELT mask = WORD_LOW_BITMASK;
	register UINT carry = 0;
	register UINT nextcarry = 0;

	if(a_coeff != b_coeff)
		ff2n_set_equal(a_coeff, b_coeff, COEFF_SIZE);
		
	if(k == 0)
		return 0;
		
	/* a WORD-LONG left shift */
	l = 1;
	while(temp_k / BITS_PER_WORD){
		for(i=0;i<COEFF_SIZE-l; i++) {
			a_coeff[i] = a_coeff[i + 1];
			a_coeff[i + 1] = 0;
		}
		temp_k -= BITS_PER_WORD;
		l++;
	}
	if(!temp_k)
		return 0; /* no more to shift */
	/* less than a WORD-LONG left shift */
	l = BITS_PER_WORD - temp_k;
	
	/* Construct mask */
	mask = WORD_LOW_BITMASK;
	for (i = 1; i < l; i++)
		mask = (mask << 1) | WORD_LOW_BITMASK;
		
	for(i=COEFF_SIZE - k/BITS_PER_WORD - 1;i>=0;i--){
		nextcarry = a_coeff[i] & mask;
		a_coeff[i] >>= temp_k;
		a_coeff[i] |= carry;
		carry = nextcarry;

	}
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_set_zero : make a zero polynomial		                */
/*----------------------------------------------------------------------*/
int ff2n_set_zero(FF2N_ELT a_coeff[], UINT COEFF_SIZE)
{
	register UINT i;
	
	for(i=0; i<COEFF_SIZE; i++)
		a_coeff[i] = 0;
	
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_set_unit : make a unit polynomial		                */
/*----------------------------------------------------------------------*/
int ff2n_set_unit(FF2N_ELT a_coeff[], UINT COEFF_SIZE)
{
	register UINT i;
	
	for(i=1; i<COEFF_SIZE; i++)
		a_coeff[i] = 0;

	a_coeff[0] = 1;	
	
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_set_equal : Make two polynomial the same  		        */
/*----------------------------------------------------------------------*/
int ff2n_set_equal(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE)
{
	register UINT i;
	
	for(i=0; i<COEFF_SIZE; i++)
		a_coeff[i] = b_coeff[i];
	
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_poly_deg : ff2n_poly_degree of polynomial                       */
/* input   : a(x)					                */
/* output  : ff2n_poly_degree of a(x)				        */
/*----------------------------------------------------------------------*/
int ff2n_poly_deg(FF2N_ELT a_coeff[], UINT COEFF_SIZE)
{
	register int i;
	register int j;
	register UINT mask;
	
	for(i=COEFF_SIZE-1;i>=0;i--)
		for(j=BITS_PER_WORD-1, mask = WORD_HI_BITMASK; j>=0; j--, mask >>= 1)
			if((a_coeff[i] & mask) > 0)
				return (i*BITS_PER_WORD + j);		
	return 0;
}	
/*----------------------------------------------------------------------*/
/* ff2n_add  : addition in GF(2^n)                                      */
/* input  : a(x), b(x)                                                  */
/* output : a(x) + b(x)                                              	*/
/*----------------------------------------------------------------------*/
int ff2n_add(FF2N_ELT c_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE)
{
	register UINT i;
	
	/* addition */
	for(i=0; i<COEFF_SIZE; i++)
		c_coeff[i] = a_coeff[i] ^ b_coeff[i];
	
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_sub  : subtraction in GF(2^n)                                   */
/* input  : a(x), b(x)                                                  */
/* output : a(x) - b(x)                                         	*/
/*----------------------------------------------------------------------*/
int ff2n_sub(FF2N_ELT c_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE)
{
	 register UINT i;
		
	/* subtraction */
	for(i=0; i<COEFF_SIZE; i++)
		c_coeff[i] = a_coeff[i] ^ b_coeff[i];
	
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_reduce : polynomial reduction	 				*/
/* input   : a(x), n(x) - irreducible polynomial 			*/
/* output  : a(x) mod n(x)						*/
/*----------------------------------------------------------------------*/
int ff2n_reduce(FF2N_ELT a_coeff[], UINT a_COEFF_SIZE, FF2N_ELT n_coeff[], UINT n_COEFF_SIZE)
{
	register int d;
	register UINT ndeg;
	FF2N_ELT temp[MAX_COEFF_SIZE];
				
	/* initialization */
	ff2n_set_zero(temp, MAX_COEFF_SIZE);
	ndeg = ff2n_poly_deg(n_coeff, n_COEFF_SIZE);
	
	while((d = (ff2n_poly_deg(a_coeff, a_COEFF_SIZE) - ndeg)) >= 0) {
		/* temp(x) = n(x) * x^d */
		ff2n_kMul(temp, a_COEFF_SIZE, n_coeff, n_COEFF_SIZE, d);	
		
		/* a(x) = a(x) - temp(x) */
		ff2n_sub(a_coeff, a_coeff, temp, a_COEFF_SIZE);		
	}
	
	return 0;
}

/*----------------------------------------------------------------------*/
/* ff2n_reduce163 : polynomial reduction specialized for GF(2^163)	*/
/* input       : a(x), n(x) - irreducible polynomial 			*/
/* output      : a(x) mod n(x)						*/
/*----------------------------------------------------------------------*/
int ff2n_reduce163(FF2N_ELT a_coeff[])
{
	/* n(x) must be x^163 + x^7 + x^6 + x^3 + 1 */
	register UINT i;
	register FF2N_ELT T;
	
	for(i=10;i>=6;i--) {
		T = a_coeff[i];
		a_coeff[i-6] ^= (T << 29);
		a_coeff[i-5] ^= (T <<  4) ^ (T <<  3) ^ T ^ (T >>3);
		a_coeff[i-4] ^= (T >> 28) ^ (T >> 29);
	}	
	T = a_coeff[5] & 0xFFFFFFF8;
	a_coeff[0] ^= (T << 4) ^ (T << 3) ^ T ^ (T >> 3);
	a_coeff[1] ^= (T >> 28) ^ (T >> 29);
	a_coeff[5] &= 0x00000007;
		
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_mul  : standard multiplication by shift-and-add method          */
/* input  : a(x), b(x), n(x)                                            */
/* output : a(x) * b(x) mod n(x)                                        */
/*----------------------------------------------------------------------*/ 
int ff2n_mul(FF2N_ELT p_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	FF2N_ELT temp[MAX_COEFF_SIZE];
	register UINT  i, j;
	register FF2N_ELT mask;
	UINT ndeg;
	
	ndeg = ff2n_poly_deg(n_coeff, COEFF_SIZE);
	ff2n_set_equal(temp, b_coeff, COEFF_SIZE);
	
	/* if a_0 = 0 then p(x) = b(x) otherwise p(x) = 0 */
	if(a_coeff[0] & WORD_LOW_BITMASK)
		ff2n_set_equal(p_coeff, b_coeff, COEFF_SIZE);
	else
		ff2n_set_zero(p_coeff, COEFF_SIZE);
	
	/* main loop */
	for(i=0;i<COEFF_SIZE;i++)		
		for(j=0, mask = WORD_LOW_BITMASK; j<BITS_PER_WORD; j++, mask <<= 1)
			if(i || j){	/* avoid a_0 */
				
				/* temp(x) = b(x)*x mod n(x) */
				ff2n_kMul(temp, COEFF_SIZE, temp, COEFF_SIZE, 1);
				if(ff2n_poly_deg(temp, COEFF_SIZE) == ndeg)
					ff2n_add(temp, temp, n_coeff, COEFF_SIZE);
					
				/* if a_i = 1 then p(x) = p(x) + temp(x) */
				if(a_coeff[i] & mask)
					ff2n_add(p_coeff, p_coeff, temp, COEFF_SIZE);
		
			}	
	
	return 0;
}
/*----------------------------------------------------------------------------*/
/* ff2n_rl_comb : multiplication using Right-to-Left Comb method              */
/* input    : a(x), b(x), n(x)                                                */
/* output   : a(x) * b(x) mod n(x)                                            */
/*----------------------------------------------------------------------------*/ 
int ff2n_rl_comb(FF2N_ELT p_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	/* Still has some bug */
	FF2N_ELT temp[2*MAX_COEFF_SIZE];
	FF2N_ELT B[2*MAX_COEFF_SIZE];
	register UINT i, j, k;
	register FF2N_ELT mask;
	
	ff2n_set_zero(temp, 2*COEFF_SIZE);
	ff2n_set_zero(B, 2*COEFF_SIZE);
	ff2n_set_equal(B, b_coeff, COEFF_SIZE);
	
	for(k=0;k<BITS_PER_WORD;k++) {
		mask = WORD_LOW_BITMASK << k;
		for(j=0;j<COEFF_SIZE;j++)
			if(a_coeff[j] & mask)
				for(i=j;i<2*COEFF_SIZE-1;i++)
					temp[i] = temp[i] ^ B[i-j];
		if(k!=BITS_PER_WORD-1)
			ff2n_kMul(B, 2*COEFF_SIZE, B, 2*COEFF_SIZE, 1); 
	}
	
	ff2n_reduce(temp, 2*COEFF_SIZE, n_coeff, COEFF_SIZE);
	ff2n_set_equal(p_coeff, temp, COEFF_SIZE);
	
	return 0;
}
/*----------------------------------------------------------------------------*/
/* ff2n_sqr  : squaring                                                       */
/* input : a(x), n(x)                                                         */
/* output: b(x) = a(x)^2 mod n(x)                                             */
/*----------------------------------------------------------------------------*/
int ff2n_sqr(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	UINT i, j, k;
	FF2N_ELT temp[2*MAX_COEFF_SIZE];
	
	ff2n_set_zero(temp, 2*MAX_COEFF_SIZE);
	
	/* set temp(2*i) = a(i) */
	for(i=0;i<COEFF_SIZE;i++)
		for(j=0;j<BITS_PER_WORD;j++) {
			k = i*BITS_PER_WORD + j;
			if(ff2n_get_bit(a_coeff, k, COEFF_SIZE))
				ff2n_set_bit(temp, 2*k, COEFF_SIZE);
		}
	ff2n_reduce(temp, 2*COEFF_SIZE, n_coeff, COEFF_SIZE);
	ff2n_set_equal(b_coeff, temp, COEFF_SIZE);
		
	return 0;
}   
/*----------------------------------------------------------------------------*/
/* ff2n_sqr2: squaring with precomputation                                    */
/* input : a(x), n(x)                                                         */
/* output: b(x) = a(x)^2 mod n(x)                                             */
/*----------------------------------------------------------------------------*/
int ff2n_sqr2(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	register UINT i;
	register UINT u0, u1, u2, u3;
	FF2N_ELT temp[MAX_COEFF_SIZE];
	
	ff2n_set_zero(temp, MAX_COEFF_SIZE);
	
	for(i=0;i<COEFF_SIZE;i++) {
		u0 = (a_coeff[i] & 0x000000ff);
		u1 = (a_coeff[i] & 0x0000ff00) >> 8;
		u2 = (a_coeff[i] & 0x00ff0000) >> 16;
		u3 = (a_coeff[i] & 0xff000000) >> 24;
		
		temp[i<<1] = (T[u1] << 16) | T[u0];
		temp[(i<<1)+1] = (T[u3] << 16) | T[u2]; 		
	}	
	
	ff2n_reduce(temp, 2*COEFF_SIZE, n_coeff, COEFF_SIZE);
	ff2n_set_equal(b_coeff, temp, COEFF_SIZE);
	
	return 0;
}   

/*----------------------------------------------------------------------*/
/* ff2n_inverse: Inversion                                   		*/
/* input    : a(x)                                                      */
/* output   : b(x) s.t a(x)*b(x) = 1 mod n(x)                           */
/*----------------------------------------------------------------------*/
int ff2n_inverse(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	/* Extended Euclidean Algorithm */
	FF2N_ELT c_coeff[MAX_COEFF_SIZE];
	FF2N_ELT u_coeff[MAX_COEFF_SIZE];
	FF2N_ELT v_coeff[MAX_COEFF_SIZE];
	FF2N_ELT temp[MAX_COEFF_SIZE];

	register int j;
	
	/* b(x) = 1 */
	ff2n_set_unit(b_coeff, COEFF_SIZE);
	/* c(x) = 0 */
	ff2n_set_zero(c_coeff, MAX_COEFF_SIZE);
	/* u(x) = a(x) */
	ff2n_set_equal(u_coeff, a_coeff, COEFF_SIZE);
	/* v(x) = n(x) */
	ff2n_set_equal(v_coeff, n_coeff, COEFF_SIZE);
	
	while((j = ff2n_poly_deg(u_coeff, COEFF_SIZE)) != 0) {
		j -= ff2n_poly_deg(v_coeff, COEFF_SIZE);
		if(j < 0) {
			/* u(x) <--> v(x) */
			ff2n_swap(u_coeff, v_coeff, COEFF_SIZE);
			/* b(x) <--> c(x) */
			ff2n_swap(b_coeff, c_coeff, COEFF_SIZE);
			
			j = -j;
		}
		
		/* u(x) = u(x) - v(x)*x^j */
		ff2n_kMul(temp, COEFF_SIZE,  v_coeff, COEFF_SIZE, j);
		ff2n_sub(u_coeff, u_coeff, temp, COEFF_SIZE);
		
		/* b(x) = b(x) - c(x)*x^j */
		ff2n_kMul(temp, MAX_COEFF_SIZE, c_coeff, COEFF_SIZE, j);
		ff2n_sub(b_coeff, b_coeff, temp, COEFF_SIZE);
	}

	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_inverse2: Inversion                                   	   	*/
/* input    : a(x)                                                      */
/* output   : b(x) s.t a(x)*b(x) = 1 mod n(x)                           */
/*----------------------------------------------------------------------*/
int ff2n_inverse2(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	/* dont use this yets, it still has some bug */
	/* Almost Inverse Algorithm */
	FF2N_ELT c_coeff[MAX_COEFF_SIZE];
	FF2N_ELT u_coeff[MAX_COEFF_SIZE];
	FF2N_ELT v_coeff[MAX_COEFF_SIZE];
	
	/* b(x) = 1 */
	ff2n_set_unit(b_coeff, COEFF_SIZE);
	/* c(x) = 0 */
	ff2n_set_zero(c_coeff, COEFF_SIZE);
	/* u(x) = a(x) */
	ff2n_set_equal(u_coeff, a_coeff, COEFF_SIZE);
	/* v(x) = n(x) */
	ff2n_set_equal(v_coeff, n_coeff, COEFF_SIZE);
	
	while(1) {
		while((u_coeff[0] & WORD_LOW_BITMASK) == 0){ /* x divides u(x) */
		
			ff2n_kDiv(u_coeff, u_coeff, COEFF_SIZE, 1);
			if((b_coeff[0] & WORD_LOW_BITMASK) == 0) /* x divides b(x) */
				ff2n_kDiv(b_coeff, b_coeff, COEFF_SIZE, 1);
			else {
				ff2n_add(b_coeff, b_coeff, n_coeff, COEFF_SIZE);
				ff2n_kDiv(b_coeff, b_coeff, COEFF_SIZE, 1);
			}
		}

		if(ff2n_is_unit(u_coeff, COEFF_SIZE)) /* u(x) = 1 */
			return 0;
		if(ff2n_poly_deg(u_coeff, COEFF_SIZE) < ff2n_poly_deg(v_coeff, COEFF_SIZE)){
			/* u(x) <---> v(x) */
			ff2n_swap(u_coeff, v_coeff, COEFF_SIZE);
			/* b(x) <---> c(x) */
			ff2n_swap(b_coeff, c_coeff, COEFF_SIZE);
		}
		/* u(x) = u(x) + v(x) */
		ff2n_add(u_coeff, u_coeff, v_coeff, COEFF_SIZE);
		/* b(x) = b(x) + c(x) */
		ff2n_add(b_coeff, b_coeff, c_coeff, COEFF_SIZE);
	}
	
	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_div: Division                                                   */
/* input   : a(x), b(x)                                                 */
/* output  : a(x)*inv(b(x)) mod n(x)                                    */
/*----------------------------------------------------------------------*/
int ff2n_div(FF2N_ELT c_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	FF2N_ELT temp[MAX_COEFF_SIZE];

	ff2n_inverse(temp, b_coeff, n_coeff, COEFF_SIZE);
	ff2n_mul(c_coeff, a_coeff, temp, n_coeff, COEFF_SIZE);

	return 0;
}
/*----------------------------------------------------------------------*/
/* ff2n_exp : Exponentiation 		                                */
/* input : a(x), k                                                      */
/* output: a(x)^k mod n(x)                                              */
/*----------------------------------------------------------------------*/
int ff2n_exp(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], DIGIT_T k[], UINT k_size, FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	/* left-to-right binary method */	
	register int i, j;
	register DIGIT_T mask;
	
	/* b(x) = 1 */
	ff2n_set_unit(b_coeff, COEFF_SIZE);
	
	for(i=k_size-1;i>=0;i--)
		for(mask = HIBITMASK, j=BITS_PER_DIGIT-1; j>=0; mask >>= 1, j--){
			/* b(x) = b(x)^2 */
			ff2n_sqr2(b_coeff, b_coeff, n_coeff, COEFF_SIZE);
			
			if(k[i] & mask)
				/* b(x) = b(x)*a(x) */
				ff2n_mul(b_coeff, a_coeff, b_coeff, n_coeff, COEFF_SIZE); 
		}

	return 0;
}
/*-------------------------------------------------------------------------*/
/* ff2n_random_poly : Random Element Generator				   */
/* input  : none							   */
/* ouput  : a finite field element at random				   */ 	
/*-------------------------------------------------------------------------*/
int ff2n_random_poly(FF2N_ELT a_coeff[], UINT COEFF_SIZE)
{
#if STRONG_RANDOM
//	prng((BYTE *)a_coeff, FF2N_NBYTE(COEFF_SIZE));
#else
	register UINT i;
	
	for(i=0;i<COEFF_SIZE-1;i++)
		a_coeff[i] = ranfunc(0, WORD_MAXVAL); 
#endif
	return 0;
}
/*-----------------------------------------------------------------------*/
/* ff2n_print : Print a polynomial's coefficients to screen              */
/*-----------------------------------------------------------------------*/
void ff2n_print(FF2N_ELT a_coeff[], UINT COEFF_SIZE)
{
	register UINT i = 0;
	register UINT len = COEFF_SIZE;
	
	while (len--){
		if ((i % 8) == 0 && i)
			printf("\n");
		printf("%08x ", a_coeff[len]);
		i++;
	}
	printf("\n");

}
/*-----------------------------------------------------------------------*/
/* ff2n_str2poly : Convert a hexa-string to a polynomial's coefficients  */
/*-----------------------------------------------------------------------*/
static BYTE hex2byte(char c)
{
	BYTE b;
	
	switch(c) {
	case '0': 	b = 0; break;
	case '1':	b = 1; break;
	case '2':	b = 2; break;
	case '3':	b = 3; break;
	case '4':	b = 4; break;
	case '5':	b = 5; break;
	case '6':	b = 6; break;
	case '7':	b = 7; break;
	case '8':	b = 8; break;
	case '9':	b = 9; break;
	case 'A': 	
	case 'a':	b = 10; break;
	case 'B':	
	case 'b':	b = 11; break;
	case 'C':	
	case 'c':	b = 12; break;
	case 'D':	
	case 'd':	b = 13; break;
	case 'E':	
	case 'e':	b = 14; break;
	case 'F': 	
	case 'f':	b = 15; break;
	default: b = 0;
	}
	
	return b;
}

FF2N_ELT *ff2n_str2poly(UINT COEFF_SIZE, char *s)
{
	BYTE *p;
	UINT i = 0;
	size_t slen;
	
	/* Hexa string should be 8-digit blocks */
	if((slen=strlen(s)) % (BITS_PER_WORD / 4))
		return NULL;
		
	p = (BYTE *)malloc(COEFF_SIZE*BITS_PER_WORD/8);
	memset(p, 0x00, COEFF_SIZE*BITS_PER_WORD/8);
	
	while(i<slen){
		/* read 2 characters each = 1 byte */
		p[i/2] = hex2byte(s[slen-i-1]) + (hex2byte(s[slen-i-2]) << 4);
		i+=2;
	}
	
	return (FF2N_ELT *)p;
}

/*-----------------------------------------------------------------------*/
/* ff2n_quad_eqn : solve quadretic equation. IEEE 1363 - A.4.6  	 */
/* Added by vdliem							 */
/*-----------------------------------------------------------------------*/
int ff2n_quad_eqn(FF2N_ELT H[], FF2N_ELT alpha[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	FF2N_ELT temp1[MAX_COEFF_SIZE];
	FF2N_ELT temp2[MAX_COEFF_SIZE];
	register int i;
	ff2n_set_equal(temp1,alpha, COEFF_SIZE);
	for (i=1;i<=(ff2n_poly_deg(n_coeff,COEFF_SIZE)-1)/2;i++)
	{
		ff2n_sqr2(temp2, temp1, n_coeff, COEFF_SIZE);
		ff2n_sqr2(temp1, temp2, n_coeff, COEFF_SIZE);
		ff2n_add(temp1, temp1, alpha, COEFF_SIZE);
	}
	
	ff2n_set_equal(H,temp1,COEFF_SIZE);
	return 0;

}

/*-----------------------------------------------------------------------*/
/* ff2n_sqrt: square root = repeated square n-1                 	 */
/* Added by vdliem							 */
/*-----------------------------------------------------------------------*/
int ff2n_sqrt(FF2N_ELT a[], FF2N_ELT b[], FF2N_ELT n_coeff[], UINT COEFF_SIZE)
{
	FF2N_ELT temp[MAX_COEFF_SIZE];
	register int j;
	int deg_b;
	
	ff2n_set_zero(temp, COEFF_SIZE);
	if (ff2n_is_zero(b, COEFF_SIZE)){
		ff2n_set_zero(a, COEFF_SIZE);
		return 0;
	}
	deg_b=ff2n_poly_deg(b,COEFF_SIZE);
	ff2n_set_equal(temp,b,COEFF_SIZE);
	for (j=1; j<ff2n_poly_deg(n_coeff, COEFF_SIZE);j++) 
		/*repeated square for deg(n_coeff)-1 times */
       		ff2n_sqr2(temp,temp,n_coeff,COEFF_SIZE);
	ff2n_set_equal(a,temp,COEFF_SIZE);
    return 0;

}
/*-----------------------------------------------------------------------*/
