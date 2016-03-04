/*--------------------------------------------------------------------*/
/* elliptic.h  : ellipic curve arithmetic implementation header file  */
/* Author      : Dang Nguyen Duc - nguyenduc@icu.ac.kr		      */
/* Last Update : 03/02/2007					      */
/*--------------------------------------------------------------------*/

#ifndef _ELLIPTIC_FF2N_H_
#define _ELLIPTIC_FF2N_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "vaultic_ff2n.h"
#include "vaultic_bigdigits.h"

/* coordinate data structure */
typedef struct {
		  	FF2N_ELT *x;
		  	FF2N_ELT *y;
		} E2n_Point;

typedef struct {
		  	FF2N_ELT *X;	/* x = X/Z^2 */
		  	FF2N_ELT *Y;	/* y = Y/Z^3 */
			FF2N_ELT *Z;
		} E2n_Projective_Point;

/* elliptic curve over GF(2^n) affine coordinate : y^2 + xy = x^3 + a*x^2 + b */
/*                         projective coordinate : Y^2 + XYZ = X^3 + aX^2Z^2 + bZ^6 */
typedef struct {
			UINT		len;	/* Length of field element in DWORD */
			FF2N_ELT	*n;	/* Irreducible polynomial */		
			FF2N_ELT	*a;	
			FF2N_ELT	*b;	/* must be non-zero */
			E2n_Point	G;	/* base point */
			UINT		rlen;	/* length of r in DWORD */
			DIGIT_T		*r;	/* base point order */
			DIGIT_T		h;	/* h*r = order of curve */
		} E2n_Curve;

#define E2N_NPARAM		9 + 2	    	/* Number of lines in domain parameter file */
#define E2N_MAX_LINE_LEN	256+1		/* Max length of a line in domain parameter file */

/* function prototypes */

E2n_Point* e2n_point_init(E2n_Curve *E);
	/* Allocate Memory for a Point */

int e2n_point_copy(E2n_Curve *E, E2n_Point *P, E2n_Point *Q);
	/* Make P = Q */

int e2n_point_zero(E2n_Curve *E, E2n_Point *P);
	/* Make P = infinity */

int e2n_point_inv(E2n_Curve *E, E2n_Point *Q, E2n_Point *P);
	/* Point Inversion */

int e2n_point_add(E2n_Curve *E, E2n_Point *R, E2n_Point *P, E2n_Point *Q);
	/* Point Addition */

int e2n_point_sub(E2n_Curve *E, E2n_Point *R, E2n_Point *P, E2n_Point *Q);
	/* Point Subtraction */
	
int e2n_point_double(E2n_Curve *E, E2n_Point* Q, E2n_Point *P);
	/* Point Doubling */
	
int e2n_point_mul(E2n_Curve *E, E2n_Point *Q, E2n_Point *P, DIGIT_T *k, UINT klen);
	/* Point Multiplication */

int e2n_point_mulw(E2n_Curve *E, E2n_Point *Q, E2n_Point *P, DIGIT_T *k, UINT klen);
	/* Point Multiplication Using Sliding Window Method */
	
int e2n_point_gen(E2n_Curve *E, E2n_Point *P);
	/* Random Point Generation */

int e2n_point_order(E2n_Curve *E, E2n_Point *P, DIGIT_T *k);
	/* Compute order of P - todo */

int e2n_curve_gen(E2n_Curve *E);
	/* Verifiable Pseudorandom Elliptic Curve Generation - todo */

/* Utility Function */
int e2n_is_on_curve(E2n_Curve *E, E2n_Point *P);
	/* Return 1 if P lies on E, otherwise return 0 */
	
int e2n_load_curve(char *fname, E2n_Curve *E);
	/* Load Elliptic Curve Domain Parameters From File */

int e2n_save_curve(char *fname, E2n_Curve *E);
	/* Save Elliptic Curve Domain Parameters To File */

#ifdef __cplusplus
}
#endif

#endif
