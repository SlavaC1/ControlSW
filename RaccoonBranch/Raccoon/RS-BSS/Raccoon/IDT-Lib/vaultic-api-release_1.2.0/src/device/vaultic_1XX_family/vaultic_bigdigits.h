/* bigdigits.h */

/*
  Most of this multi-precision arithmetic library was developed by David Ireland, 
  the author of BIGDIGITS library, copyright (c) 2001-8 by D.I. Management
  Services Pty Limited - www.di-mgt.com.au. 
  
  The bigdigits library version 1.0 has been extended by Dang Nguyen Duc and posted 
  on public domain with permission from Davia Ireland.
*/


#ifndef _BIGDIGITS_H_
#define _BIGDIGITS_H_ 1

#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>

/* data type macros */
#define UINT 	unsigned int
#define BYTE	unsigned char
#define DWORD 	unsigned long

/* define all return codes here */
/* some C preprocessors to customize the lib operation */
#define STRONG_RANDOM	1	/* use cryptographic prng rather than rand() function */
#define LINUX_URANDOM	1	/* use linux's /dev/urandom for cryptographic prng */
#define N_TEST_PRIME 200	/* The number of rounds for Miller-Rabin primality test */

//int prng(BYTE* buf, int buf_size);
	/* generate pseudo-random numbers using linux's /dev/urandom */

//void mcrypto_msg(const char *s);
	/* enable MCRYPTO_DEBUG to dump simple debug message to stdout */

//void mcrypto_dump(char *desc, BYTE *p, UINT len);
	/* more detail debug information */


/* Define type of DIGIT here */
#ifdef __APPLE__
typedef unsigned int DIGIT_T;
typedef unsigned short HALF_DIGIT_T;
/* Sizes to suit your machine - todo: move to mcrypto.h */
#define MAX_DIGIT 	(DIGIT_T)(0xffffffff)
#define MAX_HALF_DIGIT 	(DIGIT_T)(0xffff)
#define BITS_PER_DIGIT 	32
#define HIBITMASK 	(DIGIT_T)(0x80000000)
#else
typedef unsigned long DIGIT_T;
typedef unsigned short HALF_DIGIT_T;
#define MAX_DIGIT 	(DIGIT_T)(0xffffffff)
#define MAX_HALF_DIGIT 	(DIGIT_T)(0xffff)
#define BITS_PER_DIGIT 	32
#define HIBITMASK 	(DIGIT_T)(0x80000000)
#endif
/* Max number of digits expected in a mp array ~ 2048 bits */
#define MAX_DIG_LEN 64	

/*	temp storage to be used in:
	mpModulo, mpShortMod, mpModMult, mpGcd,
	mpModInv, mpIsPrime, have max number of digits
*/

/* Useful macros - todo: move to mcrypto.h */
#define LOHALF(x) ((DIGIT_T)((x) & 0xffff))
#define HIHALF(x) ((DIGIT_T)((x) >> 16 & 0xffff))
#define TOHIGH(x) ((DIGIT_T)((x) << 16))

#define ISODD(x) ((x) & 0x1)
#define ISEVEN(x) (!ISODD(x))

#define mpISODD(x, n) (x[0] & 0x1)
#define mpISEVEN(x, n) (!(x[0] & 0x1))

#define mpNEXTBITMASK(mask, n) if(mask==1){mask=HIBITMASK;n--;}else{mask>>=1;}

#define NBYTE(len)	(len)*BITS_PER_DIGIT / 8	
#define NDIGIT(n)	((n) % BITS_PER_DIGIT) ? ((n) / BITS_PER_DIGIT) + 1 : ((n) / BITS_PER_DIGIT)

/* memory management rountines */
DIGIT_T *mpMalloc(UINT ndigits);
	/* allocate memory for big integer with ndigits digits */

DIGIT_T *mpMallocB(UINT nbits, UINT *ndigits);
	/* allocate memory for big integer with nbits-bit long */

void mpFree(DIGIT_T *p);
	/* free memory allocated to p */

/*	Multiple precision calculations	
	Using known, equal ndigits
	except where noted
*/

DIGIT_T mpAdd(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], UINT ndigits);
	/* Computes w = u + v, returns carry */

DIGIT_T mpSubtract(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], UINT ndigits);
	/* Computes w = u - v, returns borrow */

int mpMultiply(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], UINT ndigits);
	/* Computes product w = u * v 
	   u, v = ndigits long; w = 2 * ndigits long */

int mpDivide(DIGIT_T q[], DIGIT_T r[], const DIGIT_T u[], UINT udigits, const DIGIT_T v[], UINT vdigits);
	/* Computes quotient q = u / v and remainder r = u mod v 
	   q, r, u = udigits long; v = vdigits long
	   Warning: Trashes q and r first */

int mpSquare(DIGIT_T w[], const DIGIT_T u[], UINT ndigits);
	/* Compute w = u^2 */

int mpModulo(DIGIT_T r[], const DIGIT_T u[], UINT udigits, const DIGIT_T v[], UINT vdigits);
	/* Computes r = u mod v 
	   u = udigits long; r, v = vdigits long */

int mpModMult(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], const DIGIT_T m[], UINT ndigits);
	/* Computes a = (x * y) mod m */

int mpModSquare(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T p[], UINT ndigits);
	/* Compute w = u^2 mod p */
	
int mpModExp(DIGIT_T y[], const DIGIT_T x[], const DIGIT_T n[], const DIGIT_T d[], UINT ndigits);
	/* Computes y = x^n mod d */

int mpModInv(DIGIT_T inv[], const DIGIT_T u[], const DIGIT_T v[], UINT ndigits);
	/*	Computes inv = u^-1 mod v */

int mpModAdd(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], const DIGIT_T m[], UINT ndigits);
	/* Compute w = u + v mod m */

int mpModSubtract(DIGIT_T w[], const DIGIT_T u[], const DIGIT_T v[], const DIGIT_T m[], UINT ndigits);
	/* Compute w = u - v mod m */

int mpJacobi(int *val, const DIGIT_T a[], const DIGIT_T m[], UINT len);
	/* Compute Jacobian symbol val = (a/m) where m is an odd integer */
	
int mpLegendre(int *val, const DIGIT_T a[], const DIGIT_T p[], UINT len);
	/* Compute Legendre symbol val = (a/p) where p is an odd prime */

int mpModSquareRoot(DIGIT_T x[], const DIGIT_T a[], const DIGIT_T p[], UINT S, const DIGIT_T Q[], const DIGIT_T V[], const DIGIT_T a1[], UINT len);
	/* 
	   Compute modulo p square root of a, x^2 = a mod p 
	   a1 = a^-1 mod p
	   p-1 = Q*2^S, Q is odd
	   V = W^Q mod p where W is a quadratic nonresidue modulo p
	   V, S, Q are computed using mpModSquareRootPre
	   a1 can be computed by mpModInv or mpModExp (a^(-1) = a^(p-2) mod p)
	*/

int mpModSquareRootPre(UINT *S, DIGIT_T Q[], DIGIT_T V[], const DIGIT_T p[], UINT len);
	/* Precomputation for mpModSquareRoot */

int mpGcd(DIGIT_T g[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits);
	/* Computes g = gcd(x, y) */

int mpEqual(const DIGIT_T a[], const DIGIT_T b[], UINT ndigits);
	/* Returns true if a == b, else false */

int mpCompare(const DIGIT_T a[], const DIGIT_T b[], UINT ndigits);
	/* Returns sign of (a - b) */

int mpIsZero(const DIGIT_T a[], UINT ndigits);
	/* Returns true if a == 0, else false */

int mpIsOne(const DIGIT_T a[], UINT ndigits);
	/* Returns true if a == 1, else false */

int mpSwap(DIGIT_T a[], DIGIT_T b[], UINT len);
	/* Swap two integers */

/* Bitwise operations */

DIGIT_T mpShiftLeft(DIGIT_T a[], const DIGIT_T b[], UINT x, UINT ndigits);
	/* Computes a = b << x */

DIGIT_T mpShiftRight(DIGIT_T a[], const DIGIT_T b[], UINT x, UINT ndigits);
	/* Computes a = b >> x */

void mpAND(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits);	
	/* Bitwise AND */

void mpOR(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits);	
	/* Bitwise OR */
	
void mpXOR(DIGIT_T a[], const DIGIT_T x[], const DIGIT_T y[], UINT ndigits);
	/* Bitwise XOR */

void mpComplement(DIGIT_T a[], const DIGIT_T b[], UINT ndigits);
	/* Bitwise Complement */
	
/* Other mp utilities */

void mpSetZero(DIGIT_T a[], UINT ndigits);
	/* Sets a = 0 */

void mpSetDigit(DIGIT_T a[], DIGIT_T d, UINT ndigits);
	/* Sets a = d where d is a single digit */

void mpSetEqual(DIGIT_T a[], const DIGIT_T b[], UINT ndigits);
	/* Sets a = b */

UINT mpSizeof(const DIGIT_T a[], UINT ndigits);
	/* Returns size of significant non-zero digits in a */

UINT mpBitLength(const DIGIT_T d[], UINT ndigits);
	/* Return actual bit length of d */

int mpIsPrime(const DIGIT_T w[], UINT ndigits, UINT t);
	/* Returns true if w is a probable prime 
	   t tests using FIPS-186-2/Rabin-Miller */

UINT mpSolinasPrime(DIGIT_T p[], UINT ndigits, UINT bit_len);
	/* generate Solinas' prime of the form p = 2^a + 2^b + 1, return b if succeeded and 0 if failed */

/* mpShort = mp x single digit calculations */

DIGIT_T mpShortAdd(DIGIT_T w[], const DIGIT_T u[], DIGIT_T d, UINT ndigits);
	/* Computes w = u + d, returns carry */

DIGIT_T mpShortSub(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, UINT ndigits);
	/* Computes w = u - v, returns borrow */

DIGIT_T mpShortMult(DIGIT_T p[], const DIGIT_T x[], DIGIT_T y, UINT ndigits);
	/* Computes product p = x * y */

DIGIT_T mpShortDiv(DIGIT_T q[], const DIGIT_T u[], DIGIT_T v, UINT ndigits);
	/* Computes q = u / v, returns remainder */

DIGIT_T mpShortMod(const DIGIT_T a[], DIGIT_T d, UINT ndigits);
	/* Return r = a mod d */

int mpShortCmp(const DIGIT_T a[], DIGIT_T b, UINT ndigits);
	/* Return sign of (a - b) where b is a single digit */

int mpShortModMult(DIGIT_T w[], const DIGIT_T u[], DIGIT_T v, DIGIT_T m[], UINT ndigits);
	/* Compute w = u*v mod m */

/* Short division using only half-digit divisor */

DIGIT_T mpHalfDiv(DIGIT_T q[], const DIGIT_T a[], HALF_DIGIT_T d, UINT ndigits);
	/* Computes q = a mod d, returns remainder */

DIGIT_T mpHalfMod(const DIGIT_T a[], HALF_DIGIT_T d, UINT ndigits);
	/* Return r = a mod d */

DIGIT_T spDivide(DIGIT_T *q, DIGIT_T *r, DIGIT_T u[2], DIGIT_T v);
	/* Computes quotient q = u / v, remainder r = u mod v */

int spModExp(DIGIT_T *exp, DIGIT_T x, DIGIT_T n, DIGIT_T d);
	/* Computes exp = x^n mod d */

int spModMult(DIGIT_T *a, DIGIT_T x, DIGIT_T y, DIGIT_T m);
	/* Computes a = (x * y) mod m */

int spModInv(DIGIT_T *inv, DIGIT_T u, DIGIT_T v);
	/* Computes inv = u^-1 mod v */

DIGIT_T spGcd(DIGIT_T x, DIGIT_T y);
	/* Returns gcd(x, y) */

int spIsPrime(DIGIT_T w, UINT t);
	/* Returns true if w is prime, else false; try t tests */

DIGIT_T spPseudoRand(DIGIT_T lower, DIGIT_T upper);
	/* Returns single pseudo-random digit between lower and upper */

/* Utilities */

void mpPrint(const DIGIT_T *p, UINT len);
	/* Print all digits incl leading zero digits */
	
void mpPrintNL(const DIGIT_T *p, UINT len);
	/* Print all digits with newlines */
	
void mpPrintTrim(const DIGIT_T *p, UINT len);
	/* Print but trim leading zero digits */
	
void mpPrintTrimNL(const DIGIT_T *p, UINT len);
	/* Print, trim leading zeroes, add newlines */

void mpMakeRandom(DIGIT_T a[], UINT ndigits);
	/* Generate a pseudorandom number */
	
BYTE *mpASC2BIN(const char *s, UINT len, UINT *nread);
	/* convert an ascii string to a big integer of length len */

char *mpBIN2ASC(const BYTE *p, UINT len);
	/* convert a big integer of length len to an ascii string */

BYTE *mpHex2Bin(const char *s, UINT len, UINT *nread);
	/* Convert an hexa string to binary data */

char *mpBin2Hex(const BYTE *p, UINT len);
	/* Convert binary data to an hexa string */
	
BYTE *mpDec2Bin(const char *s, UINT *nread);
	/* Convert a decimal string to binary data */

BYTE *mpOct2Hex(const char *s, UINT *nread);
	/* Convert an octal string to binary data */

char *mpBase64Encode(DIGIT_T *p, UINT len);
	/* convert binary data to an integer string in base64 */

DIGIT_T *mpBase64Decode(UINT *len, char *str);
	/* convert an integer string in base 64 to binary data */

size_t mpConvFromOctets(DIGIT_T a[], size_t ndigits, const unsigned char *c, size_t nbytes);
/* Converts nbytes octets into big digit a of max size ndigits
   Returns actual number of digits set (may be larger than mpSizeof)
*/

size_t mpConvToOctets(const DIGIT_T a[], size_t ndigits, unsigned char *c, size_t nbytes);
/* Convert big digit a into string of octets, in big-endian order,
   padding on the left to nbytes or truncating if necessary.
   Return number of octets required excluding leading zero bytes.
*/


#ifdef __cplusplus
}
#endif

#endif	/* _BIGDIGITS_H_ */
