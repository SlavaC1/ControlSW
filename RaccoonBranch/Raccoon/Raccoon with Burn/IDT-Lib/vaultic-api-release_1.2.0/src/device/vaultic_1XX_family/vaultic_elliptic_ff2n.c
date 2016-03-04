/* --------------------------------------------------------------------------
 * VaultIC API
 * Copyright (C) Inside Secure, 2011. All Rights Reserved.
 * -------------------------------------------------------------------------- */
/*--------------------------------------------------------------------*/
/* elliptic.c  : ellipic curve arithmetic implementation source file  */
/* Author      : Dang Nguyen Duc - nguyenduc@icu.ac.kr		      */
/* Last Update : 04/03/2001					      */
/* Reference   : "Elliptic Curve Cryptography" by I. F. Blake et al.  */
/* TO DO       :						      */
/*--------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "vaultic_elliptic_ff2n.h"
#include "vaultic_ff2n.h"
#include "vaultic_bigdigits.h"

E2n_Point* e2n_point_init(E2n_Curve *E)
{
	E2n_Point *P = NULL;
	
	P = (E2n_Point *)malloc(sizeof(E2n_Point));
	
	P->x = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	P->y = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));

	return P;
}
	

int e2n_point_copy(E2n_Curve *E, E2n_Point *Q, E2n_Point *P)
{
	ff2n_set_equal(Q->x, P->x, E->len);
	ff2n_set_equal(Q->y, P->y, E->len);
	
	return 0;
}

int e2n_point_zero(E2n_Curve *E, E2n_Point *P)
{
	/* P = infinity */
	ff2n_set_zero(P->x, E->len);
	ff2n_set_zero(P->y, E->len);
	
	return 0;
}


int e2n_point_inv(E2n_Curve *E, E2n_Point *Q, E2n_Point *P)
{
	/* Q = -P: Xq = Xp, Yq = Xp + Yp */
	ff2n_set_equal(Q->x, P->x, E->len);
	ff2n_add(Q->y, P->x, P->y, E->len);
	
	return 0;
}

int e2n_point_double(E2n_Curve *E, E2n_Point* Q, E2n_Point *P)
{
	FF2N_ELT *lamda;
	FF2N_ELT *T1;
	FF2N_ELT *T2;
	
	/* If P = Infinity */
	if(ff2n_is_zero(P->x, E->len) && ff2n_is_zero(P->y, E->len))
	{
		e2n_point_zero(E, Q);
		
		return 0;
	}
	
	/* Allocate memory */
	lamda = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	T1 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	T2 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	
	/* lamda = Yp/Xp + Xp */
	ff2n_inverse(T1, P->x, E->n, E->len);		/* T1 = Xp^-1 */
	ff2n_mul(T2, T1, P->y, E->n, E->len);		/* T2 = Yp*T1 */
	ff2n_add(lamda, T2, P->x, E->len);		/* lamda = T2 + Xp */
	
	/* Xq = lamda^2 + lamda + a */
	ff2n_sqr2(T1, lamda, E->n, E->len);		/* T1 = lamda^2 */
	ff2n_add(T2, T1, lamda, E->len);		/* T2 = T1 + lamda */
	ff2n_add(Q->x, T2, E->a, E->len);		/* Xq = T2 + a */
	
	/* Yq = (Xp + Xq)*lamda + Xq + Yp */
	ff2n_add(T1, Q->x, P->x, E->len);		/* T1 = Xp + Xq */
	ff2n_mul(T2, T1, lamda, E->n, E->len);		/* T2 = T1 * lamda */
	ff2n_add(T1, T2, Q->x, E->len);			/* T1 = T2 + Xq */
	ff2n_add(Q->y, T1, P->y, E->len);		/* Yq = T1 + Yp */
	
	free(lamda);
	free(T1);
	free(T2);
	
	return 0;
}

int e2n_point_add(E2n_Curve *E, E2n_Point *R, E2n_Point *P, E2n_Point *Q)
{
	FF2N_ELT *lamda;
	FF2N_ELT *T1;
	FF2N_ELT *T2;
	
	/* If P = Q, call e2n_point_double instead */
	if(ff2n_is_equal(P->x, Q->x, E->len) && ff2n_is_equal(P->y, Q->y, E->len))
		return e2n_point_double(E, R, P);
		
	/* if P = infinity */
	if(ff2n_is_zero(P->x, E->len) && ff2n_is_zero(P->y, E->len))
	{
		e2n_point_copy(E, R, Q);
		
		return 0;
	}
	
	/* if Q = infinity */
	if(ff2n_is_zero(Q->x, E->len) && ff2n_is_zero(Q->y, E->len))
	{
		e2n_point_copy(E, R, P);
		
		return 0;
	}
	
	/* if P = -Q */	
	T1 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	ff2n_add(T1, Q->x, Q->y, E->len);
	if(ff2n_is_equal(P->x, Q->x, E->len) && ff2n_is_equal(P->y, T1, E->len))
	{
		e2n_point_zero(E, R);
		
		free(T1);
		return 0;
	}
	
	/* Otherwise - Allocate memory */
	lamda = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	T2 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	
	/* Compute lamda = (Yp + Yq) / (Xp + Xq) */
	ff2n_add(T1, P->x, Q->x, E->len);		/* T1 =  Xp + Xq */
	ff2n_inverse(T2, T1, E->n, E->len);		/* T2 = T1^-1 */
	ff2n_add(T1, P->y, Q->y, E->len);		/* T1 = Yq + Yq */
	ff2n_mul(lamda, T1, T2, E->n, E->len);		/* lamda = T1 * T2 */
	
	/* Compute Xr = a + lamda^2 + lamda + Xp + Xq */
	ff2n_sqr2(T1, lamda, E->n, E->len);		/* T1 = lamda^2 */
	ff2n_add(T2, T1, E->a, E->len);			/* T2 = T1 + a */
	ff2n_add(T1, T2, lamda, E->len);		/* T1 = T2 + lamda */
	ff2n_add(T2, T1, P->x, E->len);			/* T2 = T1 + Xp */
	ff2n_add(R->x, T2, Q->x, E->len);		/* Xr = T2 + Xq */
	
	/* Yr = (Xp + Xr)*lamda + Xr + Yp */
	ff2n_add(T1, R->x, P->x, E->len);		/* T1 = Xp + Xq */
	ff2n_mul(T2, T1, lamda, E->n, E->len);		/* T2 = T1 * lamda */
	ff2n_add(T1, T2, R->x, E->len);			/* T1 = T2 + Xq */
	ff2n_add(R->y, T1, P->y, E->len);		/* Yq = T1 + Yp */
	
	free(lamda);
	free(T1);
	free(T2);
	
	return 0;
}

int e2n_point_sub(E2n_Curve *E, E2n_Point *R, E2n_Point *P, E2n_Point *Q)
{
	E2n_Point *T;
	
	T = e2n_point_init(E);
	
	e2n_point_inv(E, T, Q);
	
	e2n_point_add(E, R, P, T);
	
	free(T->x);
	free(T->y);
	free(T);
	
	return 0;
}
	
int e2n_point_mul(E2n_Curve *E, E2n_Point *Q, E2n_Point *P, DIGIT_T *k, UINT klen)
{
	/* scalar multiplication using binary method */
	DIGIT_T mask;
	int i;
	E2n_Point *R;
	
	/* Allocate temp memory */
	R = e2n_point_init(E);
			
	/* Set Q = Infinity */
	e2n_point_zero(E, Q);

	while(klen--)
	{
		mask = HIBITMASK;
		for(i=BITS_PER_DIGIT-1;i>=0;i--)
		{
			e2n_point_double(E, R, Q);
			if(k[klen] & mask)
			{
				e2n_point_add(E, Q, R, P);
			}
			else
			{
				ff2n_set_equal(Q->x, R->x, E->len);
				ff2n_set_equal(Q->y, R->y, E->len);
			}		
			mask >>= 1;
		}
	}
	
	free(R->x);
	free(R->y);
	free(R);
	
	return 0;
}

int e2n_point_gen(E2n_Curve *E, E2n_Point *P)
{
    	/* need ff2n_square_root function */
	
    	return 0;
}


int e2n_is_on_curve(E2n_Curve *E, E2n_Point *P)
{
	FF2N_ELT *T1;
	FF2N_ELT *T2;
	FF2N_ELT *T3;
	FF2N_ELT *T4;
	int ret;
	
	/* If P = Infinity */
	if(ff2n_is_zero(P->x, E->len) && ff2n_is_zero(P->y, E->len))
		return 1;
	
	T1 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	T2 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	T3 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	T4 = (FF2N_ELT *)malloc(FF2N_NBYTE(E->len));
	
	/* T1 = Yp^2 + Yp*Xp */
	ff2n_sqr2(T1, P->y, E->n, E->len);
	ff2n_mul(T2, P->x, P->y, E->n, E->len);
	ff2n_add(T1, T1, T2, E->len);
	
	/* T2 = Xp^3 + a*Xp^2 + b */
	ff2n_sqr2(T2, P->x, E->n, E->len);
	ff2n_mul(T3, T2, E->a, E->n, E->len);
	ff2n_mul(T4, T2, P->x, E->n, E->len);
	ff2n_add(T2, T3, T4, E->len);
	ff2n_add(T2, T2, E->b, E->len);
	
	ret = ff2n_is_equal(T1, T2, E->len);
	
	free(T1);
	free(T2);
	free(T3);
	free(T4);
	
	return ret;
}
