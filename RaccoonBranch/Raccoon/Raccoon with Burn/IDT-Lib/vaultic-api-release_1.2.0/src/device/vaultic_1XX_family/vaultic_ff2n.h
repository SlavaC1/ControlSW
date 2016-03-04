/*------------------------------------------------------*/
/* Finite Field Implemetation Header file - ff2n.h      */
/* Author : Dang Nguyen Duc - nguyenduc@icu.ac.kr       */
/* Date   : 11/17/2001                                  */        
/*------------------------------------------------------*/              
#ifndef _FF2N_H_
#define _FF2N_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vaultic_bigdigits.h"

#ifdef __APPLE__
typedef	unsigned int FF2N_ELT;	/* coefficient datatype */
#else
typedef	unsigned long FF2N_ELT;	/* coefficient datatype */
#endif

/* added by Liem */
typedef FF2N_ELT *ff2n_p;
typedef FF2N_ELT ff2n_t;

#define WORD_MAXVAL             0xffffffff
#define WORD_HI_BITMASK         0x80000000
#define WORD_LOW_BITMASK        0x00000001
#define BITS_PER_WORD	32
#define MAX_COEFF_SIZE	64 + 1
#define FF2N_NBYTE(len)	len*BITS_PER_WORD / 8

/* Some Utility function */
int ff2n_swap(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE);
	/* Swap a(x) <---> b(x) */

int ff2n_is_unit(FF2N_ELT a_coeff[], UINT COEFF_SIZE);
	/* Return 1 if a(x) = 1 */ 
	
int ff2n_is_zero(FF2N_ELT a_coeff[], UINT COEFF_SIZE);
	/* Return 1 if a(x) = 0 */ 
	
int ff2n_is_equal(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE);
	/* Return 1 if a(x) = b(x) */ 

int ff2n_get_bit(FF2N_ELT a_coeff[], UINT k, UINT COEFF_SIZE);
	/* Return k-th coefficient of a(x) */
	
int ff2n_set_bit(FF2N_ELT a_coeff[], UINT k, UINT COEFF_SIZE);
	/* Set k-th coefficient of a(x) to 1 */
	
int ff2n_clear_bit(FF2N_ELT a_coeff[], UINT k, UINT COEFF_SIZE);
	/* Set k-th coefficient of a(x) to 0 */
	
int ff2n_kMul(FF2N_ELT a_coeff[], UINT a_COEFF_SIZE, FF2N_ELT b_coeff[], UINT b_COEFF_SIZE, UINT k);
	/* a(x) = a(x) * x^k */

int ff2n_kDiv(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE,  UINT k);
	/* a(x) = a(x) / x^k */
	
int ff2n_set_zero(FF2N_ELT a_coeff[], UINT COEFF_SIZE);
	/* Set a(x) = 0 */
	
int ff2n_set_unit(FF2N_ELT a_coeff[], UINT COEFF_SIZE);
	/* Set a(x) = 1 */
	
int ff2n_set_equal(FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE);
	/* Make a(x) = b(x) */
	
int ff2n_poly_deg(FF2N_ELT a_coeff[], UINT COEFF_SIZE);
	/* Return degree of a(x) */

int ff2n_add(FF2N_ELT c_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE);
	/* Compute c(x) = a(x) + b(x) */
	
int ff2n_sub(FF2N_ELT c_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], UINT COEFF_SIZE);
	/* Compute c(x) = a(x) - b(x) */

int ff2n_reduce(FF2N_ELT a_coeff[], UINT a_COEFF_SIZE, FF2N_ELT n_coeff[], UINT n_COEFF_SIZE);
	/* Compute a(x) = a(x) mod n(x) */

int ff2n_reduce163(FF2N_ELT a_coeff[]);
	/* Special Reduction Algorithm for GF(2^163) */

int ff2n_mul(FF2N_ELT p_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial multiplication using Shift and Add: p(x) = a(x)*b(x) mod n(x) */

int ff2n_rl_comb(FF2N_ELT p_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial multiplication - Right to Left Comb: p(x) = a(x)*b(x) mod n(x) */

int ff2n_inverse(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial Inversion using Extended Euclidean Algorithm: b(x) = a(x)^-1 mod n(x) */

int ff2n_inverse2(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial Inversion - Almost Inverse Algorithm: b(x) = a(x)^-1 mod n(x) */

int ff2n_div(FF2N_ELT c_coeff[], FF2N_ELT a_coeff[], FF2N_ELT b_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial Division: c(x) = a(x) / b(x) mod n(x) */

int ff2n_exp(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], DIGIT_T k[], UINT k_size, FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial exponentiation: b(x) = a(x)^k mod n(x) */

int ff2n_sqr(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial squaring: b(x) = a(x)^2 mod n(x) */

int ff2n_sqr2(FF2N_ELT b_coeff[], FF2N_ELT a_coeff[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
	/* Polynomial squaring with precomputation: b(x) = a(x)^2 mod n(x) */

FF2N_ELT ff2n_ranfunc(FF2N_ELT min, FF2N_ELT max);
	/* helper for ff2n_random_poly */

int ff2n_random_poly(FF2N_ELT a_coeff[], UINT COEFF_SIZE);
	/* Polynomial squaring - precomputation */
	
FF2N_ELT* ff2n_str2poly(UINT COEFF_SIZE, char *str);
	/* Hexa-string to polynomial's coefficients */

void ff2n_print(FF2N_ELT a_coeff[], UINT COEFF_SIZE);
	/* Print polynomial's coefficients */

/* Added by vdliem						*/
int ff2n_quad_eqn(FF2N_ELT H[], FF2N_ELT alpha[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
    /*Solve quadretic equation*/

int ff2n_sqrt(FF2N_ELT a[], FF2N_ELT b[], FF2N_ELT n_coeff[], UINT COEFF_SIZE);
    /*Compute square root*/	

#ifdef __cplusplus
}
#endif

#endif
