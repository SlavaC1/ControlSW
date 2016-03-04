
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include "REDApp_utils.h"

#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/obj_mac.h>
#include <openssl/bn.h>
#include <openssl/sha.h>

#include <Basic.h>      /* CUnit */

const int randomLen = 8;
const int tagSerialNumberLen = 8;
const int REDrandomLen = 8;
const int weightDataLen = 28;
const int signatureLen = 72;
const int WeightSIGoffset = 28;
const int WeightRNDoffset = 16;
const int WeightOffset = 24;


/* Stratasys Demo ECDSA K-283 Private Key */
const unsigned char priv_ks_bin[36] =
{
  0x01, 0x3D, 0x44, 0x45, 0xA8, 0x4D, 0x8D, 0x6F,
  0xAD, 0xFB, 0x25, 0x22, 0xD2, 0xA1, 0xE7, 0xE9,
  0x05, 0xE9, 0x05, 0xBD, 0x7E, 0x8E, 0x37, 0x90,
  0xCE, 0x5B, 0x79, 0x1F, 0x42, 0xC3, 0xA1, 0xD1,
  0x63, 0x9E, 0x77, 0x7A
};

/* 16 Demo ECDSA K-283 Private Keys (to emulate cartridge chips) */
const unsigned char priv_kc_bin[16][36] =
{
  { 0x01, 0xA6, 0xB0, 0x24, 0x68, 0xD3, 0x57, 0x9F,
    0x10, 0xDB, 0x28, 0xD8, 0x1F, 0x21, 0xF6, 0x60,
    0xE1, 0x5B, 0x8C, 0xB4, 0x82, 0x26, 0xD7, 0x3F,
    0x02, 0x98, 0xDE, 0x3D, 0x06, 0x5F, 0xB2, 0x2A,
    0xDD, 0x9B, 0x95, 0xB5 },
  { 0x01, 0x73, 0x5D, 0x68, 0x1E, 0x6A, 0xEF, 0xF6,
    0x27, 0x0B, 0x1F, 0xB9, 0x06, 0xCA, 0xCF, 0x15,
    0x47, 0xB1, 0x5C, 0x24, 0x66, 0xEF, 0x1E, 0x86,
    0xF1, 0x63, 0x44, 0x72, 0x70, 0xEB, 0xCD, 0xFA,
    0x9B, 0x1C, 0xB4, 0xAE },
  { 0x01, 0x10, 0x06, 0xEF, 0xEE, 0xF4, 0x72, 0xBC,
    0xA7, 0x04, 0x25, 0xBA, 0x70, 0x82, 0x75, 0x3E,
    0x0A, 0xD4, 0x5D, 0x20, 0xE5, 0x31, 0xB9, 0xC2,
    0x86, 0xBF, 0xA5, 0x95, 0x93, 0xF3, 0x06, 0x39,
    0x3E, 0x0F, 0x29, 0x3B },
  { 0x01, 0x59, 0x27, 0x3A, 0xC0, 0xBE, 0x59, 0x42,
    0xC7, 0xE8, 0x22, 0x7A, 0x01, 0x6D, 0xBA, 0x48,
    0x61, 0x9B, 0xD3, 0x2B, 0x00, 0xD4, 0xD4, 0x1A,
    0x64, 0x53, 0x78, 0x77, 0x3A, 0x63, 0x35, 0xD5,
    0xA2, 0xA3, 0x31, 0x01 },
  { 0x00, 0x81, 0xD3, 0x63, 0x81, 0x19, 0x7C, 0x93,
    0x3A, 0x40, 0xD4, 0x51, 0x96, 0x2E, 0xA2, 0x86,
    0x99, 0x4B, 0x92, 0x5F, 0xE4, 0x81, 0xA8, 0x69,
    0x20, 0xFC, 0xFB, 0xE5, 0x9F, 0xF9, 0xAC, 0xC9,
    0x41, 0xA4, 0x02, 0xE1 },
  { 0x00, 0xC9, 0x39, 0xDD, 0xF7, 0xE2, 0x4F, 0x0F,
    0x21, 0xE7, 0x5C, 0x05, 0x17, 0x98, 0x28, 0x11,
    0x5F, 0xA6, 0xA0, 0x56, 0xFB, 0x17, 0x43, 0x01,
    0x8C, 0x1E, 0x5C, 0x60, 0xD9, 0x05, 0xCC, 0xDA,
    0xAD, 0x3F, 0x6B, 0x3D },
  { 0x01, 0x79, 0xC0, 0xFF, 0x84, 0x95, 0x87, 0x81,
    0x0E, 0x06, 0x2A, 0xED, 0xA7, 0x29, 0xF8, 0x66,
    0x2B, 0x7E, 0x98, 0x53, 0x7C, 0x36, 0x63, 0x86,
    0x50, 0x68, 0x5B, 0x5F, 0xF0, 0x57, 0xCF, 0x08,
    0x89, 0x50, 0x36, 0xD9 },
  { 0x00, 0xC9, 0x8F, 0x1B, 0x82, 0xEA, 0xF1, 0x56,
    0x79, 0x9F, 0x8B, 0xD9, 0x3D, 0x6A, 0xE1, 0xEF,
    0x33, 0x42, 0x04, 0x92, 0x7F, 0x1D, 0xB7, 0x6F,
    0xEE, 0x58, 0x7F, 0xC9, 0x75, 0xE2, 0xFF, 0xDC,
    0x85, 0xF0, 0xE3, 0x31 },
  { 0x01, 0xDD, 0xCE, 0xCE, 0x92, 0x23, 0x12, 0x8E,
    0x84, 0x23, 0x58, 0x54, 0x7D, 0x6D, 0x98, 0xF0,
    0x8A, 0x9B, 0x2C, 0x14, 0xB9, 0x5B, 0xA3, 0xA3,
    0xBB, 0x24, 0xEE, 0x52, 0x05, 0xD1, 0xE6, 0xFD,
    0xD7, 0x6D, 0xEF, 0x6C },
  { 0x01, 0x19, 0x91, 0x43, 0xA3, 0x0C, 0xFB, 0x27,
    0x85, 0x5C, 0x8C, 0x4A, 0xEE, 0x68, 0x5E, 0x51,
    0x21, 0xF9, 0xAB, 0xE0, 0x40, 0x98, 0xC6, 0x71,
    0x8F, 0x39, 0x99, 0xD3, 0x63, 0x2D, 0xBD, 0xBC,
    0xC7, 0x7D, 0x58, 0x17 },
  { 0x01, 0xD4, 0xDD, 0xB1, 0x3C, 0x8B, 0xFD, 0x5E,
    0x43, 0xA0, 0xFA, 0x8D, 0x61, 0x59, 0xEE, 0x5A,
    0x44, 0xDC, 0x9E, 0xD9, 0xE3, 0xB5, 0xCE, 0x7F,
    0x43, 0x1B, 0x63, 0x12, 0x87, 0x53, 0xC4, 0xE9,
    0x12, 0xE2, 0xC1, 0x64 },
  { 0x00, 0x2E, 0xE2, 0xB2, 0xD1, 0x4B, 0x9D, 0xB7,
    0x0A, 0x18, 0x84, 0x9E, 0x3A, 0x5E, 0xA8, 0xC1,
    0x6A, 0xEC, 0x93, 0x7F, 0xEB, 0xEC, 0x66, 0xA7,
    0xF4, 0xD4, 0xDB, 0x41, 0xB4, 0xC0, 0xDD, 0xE5,
    0x20, 0xF3, 0xBD, 0xAB },
  { 0x00, 0xC0, 0xAE, 0x68, 0x88, 0x08, 0x8A, 0x56,
    0x97, 0x0B, 0x9A, 0x3B, 0xAB, 0xCD, 0x4F, 0x0F,
    0x9E, 0x38, 0xB4, 0x7B, 0x63, 0xE0, 0xAD, 0x38,
    0xC1, 0xF8, 0x3B, 0x8A, 0x66, 0x60, 0xA3, 0x40,
    0x66, 0xCC, 0x78, 0xA9 },
  { 0x01, 0x11, 0xA9, 0xD6, 0xAA, 0xC4, 0xEB, 0x97,
    0x69, 0xED, 0x74, 0x2C, 0xBF, 0x13, 0x2C, 0x34,
    0xF4, 0x05, 0x5A, 0x9A, 0x1C, 0xAE, 0xB2, 0x7F,
    0x6D, 0xCD, 0x9D, 0x81, 0x47, 0x52, 0xE5, 0xE0,
    0x08, 0xC3, 0x07, 0xCA },
  { 0x00, 0x6B, 0x13, 0xA8, 0x07, 0xEF, 0xA3, 0xE1,
    0xC7, 0xA8, 0xA0, 0x10, 0x33, 0x90, 0x34, 0xCE,
    0x40, 0xF8, 0x24, 0x08, 0x07, 0xCF, 0x23, 0x21,
    0x45, 0xA8, 0xA5, 0xAD, 0xF8, 0xE5, 0xE8, 0xE6,
    0x80, 0x4B, 0x92, 0x58 },
  { 0x00, 0x4B, 0xFF, 0x3D, 0xC1, 0x86, 0xED, 0xFF,
    0xA9, 0x93, 0x42, 0xD1, 0xC5, 0x60, 0x97, 0x7E,
    0xDB, 0x37, 0xC1, 0xF0, 0x8E, 0xBA, 0xC1, 0x2F,
    0xAF, 0xDA, 0xA5, 0xB7, 0x81, 0x1E, 0xE8, 0x65,
    0xDA, 0x70, 0x35, 0x48 }
};

/********************************************************************************/
/** Utility Functions ***********************************************************/
/********************************************************************************/

void sha256(const void *message, size_t length, unsigned char *digest)
{
  SHA256_CTX sha256;
  SHA256_Init(&sha256);
  SHA256_Update(&sha256, message, length);
  SHA256_Final(digest, &sha256);
}



void createPubKC(byte *pubKC, byte tagNo)
{
  const EC_GROUP *ecgroup = NULL;
  EC_POINT *pub_key = NULL;
  BIGNUM *priv_bn;
  BIGNUM *pub_bn;
  BN_CTX *ctx;
  EC_KEY *eckey;
  unsigned char pub_bin[73];

  /* create private key from binary representation (priv_kc_bin array) */
  priv_bn = BN_bin2bn(&(priv_kc_bin[tagNo])[0], sizeof(priv_kc_bin[tagNo]), NULL);
  eckey = EC_KEY_new_by_curve_name(NID_sect283k1);
  ecgroup = EC_KEY_get0_group(eckey);
  pub_key = EC_POINT_new(ecgroup);
  EC_KEY_set_private_key(eckey, priv_bn);

  /* calculate public key from private key */
  ctx = BN_CTX_new();
  EC_POINT_mul(ecgroup, pub_key, priv_bn, NULL, NULL, ctx);
  EC_KEY_set_public_key(eckey, pub_key);
  pub_bn = EC_POINT_point2bn(ecgroup, pub_key, 4, NULL, ctx);
  BN_bn2bin(pub_bn, pub_bin);

  /* skip 1 byte 0x04, 36 bytes for X coordinate, 36 bytes for Y coordinate */
  memcpy(&pubKC[0], &pub_bin[1], 72);
}

void signMessage(const char *message, int messageLen, unsigned char *signature, const unsigned char *priv_key_bin, int priv_key_bin_size)
{
  const EC_GROUP *ecgroup = NULL;
  EC_POINT *pub_key = NULL;
  BIGNUM *priv_bn;
  BN_CTX *ctx;
  EC_KEY *eckey;
  unsigned char digest[SHA256_DIGEST_LENGTH];

  ECDSA_SIG *ecdsa_sig;
  unsigned char ecdsa_sig_der[4+36+2+36];
  unsigned char *ecdsa_sig_der_p = &ecdsa_sig_der[0];
  int ecdsa_sig_der_len;
  int size1, size2;

  /* create SHA-256 digest from message (pubKC) */
  sha256(message, messageLen, &digest[0]);

  ctx = BN_CTX_new();

  /* create private key from binary representation (priv_ks_bin) */
  priv_bn = BN_bin2bn(&priv_key_bin[0], priv_key_bin_size, NULL);
  eckey = EC_KEY_new_by_curve_name(NID_sect283k1);
  ecgroup = EC_KEY_get0_group(eckey);
  pub_key = EC_POINT_new(ecgroup);
  EC_KEY_set_private_key(eckey, priv_bn);

  /* sign SHA256 digest using ECC private key */
  ecdsa_sig = ECDSA_do_sign(digest, SHA256_DIGEST_LENGTH, eckey);
  if (ecdsa_sig == NULL)
  {
    printf("Failed to generate EC Signature\n");
  }

  /* creates DER encoding of signature */
  ecdsa_sig_der_len = i2d_ECDSA_SIG(ecdsa_sig, &ecdsa_sig_der_p);

  memset(signature, 0, 72);

  /* extract raw signature */
  size1 = (int)(ecdsa_sig_der[3]);
  size2 = (int)(ecdsa_sig_der[4 + size1 + 1]);
  memcpy(&signature[36 - size1], &ecdsa_sig_der[4], size1);
  memcpy(&signature[72 - size2], &ecdsa_sig_der[4 + size1 + 2], size2);

  ECDSA_SIG_free(ecdsa_sig);

  BN_free(priv_bn);
  BN_CTX_free(ctx);
}

/* Sign message wrong (sign correctly, then flip a bit in the signature) */
void signMessage_wrong(const char *message, int messageLen, unsigned char *signature, const unsigned char *priv_key_bin, int priv_key_bin_size)
{
  signMessage(message, messageLen, signature, priv_key_bin, priv_key_bin_size);

  /* flip a bit in the signature to make it wrong */
  signature[0] ^= 0x01;
}

/* Composes certificate */
void buildCertificate(unsigned char *certificate, unsigned char *message, unsigned char *signature)
{
  const int messageSize = 107;
  const int signatureSizeOffset = messageSize;
  const int signatureSize = 72;
  const int signatureOffset = signatureSizeOffset + 2; 

  memcpy(&certificate[0], &message[0], messageSize);

  certificate[signatureSizeOffset + 1] = signatureSize >> 8 & 0xFF;
  certificate[signatureSizeOffset] = signatureSize & 0xFF;

  memcpy(&certificate[signatureOffset], &signature[0], signatureSize);
}

/* Composes invalid certificate, with wrong message size */
void buildCertificate_invalid(char *certificate, byte *message, byte *signature)
{
  const int messageSize = 30;       /* wrong message size */
  const int signatureSizeOffset = messageSize;
  const int signatureSize = 72;
  const int signatureOffset = signatureSizeOffset + 2;

  memcpy(&certificate[0], &message[0], messageSize);

  certificate[signatureSizeOffset + 1] = messageSize >> 8 & 0xFF;
  certificate[signatureSizeOffset] = messageSize & 0xFF;

  memcpy(&certificate[signatureOffset], &signature[0], signatureSize);
}

void builedSignedChallenge(byte *signedChallenge, byte *random, byte tagNo)
{
	const int messageSize = 32;
	const int signatureSizeOffset = messageSize;
	const int signatureSize = 72;
	const int signatureOffset = signatureSizeOffset + 2;
	unsigned char signature[72];

	memcpy(&signedChallenge[8],random,8);

	signMessage(signedChallenge, messageSize, signature, priv_kc_bin[tagNo], sizeof(priv_kc_bin[tagNo]));

	signedChallenge[signatureSizeOffset + 1] = signatureSize >> 8 & 0xFF;
	signedChallenge[signatureSizeOffset] = signatureSize & 0xFF;

	memcpy(&signedChallenge[signatureOffset], &signature[0], signatureSize);

}

/* Executes Tag Authentication procedure */
int authenticateTag(hasp_handle_t handle, byte tagNo, int isActive)
{
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char signedChallenge[106];
  unsigned char certificate[181];
  unsigned char random[8];
  int result = 0 /* false */;

  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(random));
  //create public key
  createPubKC(pubKC, tagNo);
  
  //create message to sign
  strcpy(message,info);
  strcat(message,pubKC);

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  CU_ASSERT(verifyTag(handle, tagNo, isActive, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
  CU_ASSERT(result);

  if (result)
  {
    memset(signedChallenge, 0, sizeof(signedChallenge));
    
	builedSignedChallenge(signedChallenge,random, tagNo);

    CU_ASSERT(verifyChallenge(handle, tagNo, isActive, signedChallenge, sizeof(signedChallenge), &result) == HASP_STATUS_OK);
    CU_ASSERT(result);
  }

  return result;
}


void buildWeightData(byte *weightData, byte *random, byte *tagSerialNumber, byte *REDrandom, int weight)
{
  memcpy(weightData,random,randomLen);
  memcpy(&(weightData[randomLen]),tagSerialNumber,tagSerialNumberLen);
  memcpy(&(weightData[WeightRNDoffset]),REDrandom,REDrandomLen);

  /* weight is 32 bit little endian */
  weightData[WeightOffset + 3] = (byte)(weight >> 24);
  weightData[WeightOffset + 2] = (byte)(weight >> 16);
  weightData[WeightOffset + 1] = (byte)(weight >> 8);
  weightData[WeightOffset] = (byte)weight;
}

/* Composes signed weight */
void buildSignedWeight(byte *signedWeight,  byte *weightData, byte *signature)
{
  /* copy weightData to signedWeight */
  memcpy(signedWeight, weightData, weightDataLen);

  /* signatureLen is always 72 in 16 bit little endian */
  signedWeight[WeightSIGoffset + 1] = (byte)(signatureLen >> 8);
  signedWeight[WeightSIGoffset] = (byte)signatureLen;

  /* copy signature to signedWeight */
  memcpy(&signedWeight[WeightSIGoffset+2], &signature[0], signatureLen);
}

/* executes updateWeight procedure */
hasp_status_t updateWeight(hasp_handle_t handle, byte tagNo, int isActive, int consumption, int weight, int *pResult)
{
  hasp_status_t status;
  char signedWeight[102];
  char weightData[28];
  char random[8];
  char tagSerialNumber[8];
  char REDrandom[8];
  char signature[72];

  memset(signedWeight, 0, sizeof(signedWeight));
  memset(random, 0, sizeof(random));
  memset(tagSerialNumber, 0, sizeof(tagSerialNumber));
  memset(REDrandom, 0, sizeof(REDrandom));
  memset(signature, 0, sizeof(signature));
  
  //Invoke updateConsumption
  status = updateConsumption(handle, tagNo, isActive, consumption, REDrandom, pResult);
  if (*pResult)
  {
	buildWeightData(weightData,random,tagSerialNumber,REDrandom,weight);
    
	signMessage(weightData, sizeof(weightData), signature, priv_kc_bin[tagNo], sizeof(priv_kc_bin[tagNo]));

    buildSignedWeight(signedWeight, weightData, signature);

    //Invoke verifyWeight
    status = verifyWeight(handle, tagNo, isActive, signedWeight, sizeof(signedWeight), pResult);
  }

  return status;
}

/* Sample of fault in Update Weight Procedure - incorrect random is sent back */
hasp_status_t updateWeight_wrongRandom(hasp_handle_t handle, byte tagNo, int isActive, int consumption, int weight, int *pResult)
{
  hasp_status_t status;
  char signedWeight[86];
  char random[8];
  char signature[72];
  int i;

  memset(signedWeight, 0, sizeof(signedWeight));
  memset(random, 1, sizeof(random));

  //Invoke updateConsumption
   status = updateConsumption(handle, tagNo, isActive, consumption, random, pResult);

  if (*pResult)
  {
    signMessage(random, sizeof(random), signature, priv_kc_bin[tagNo], sizeof(priv_kc_bin[tagNo]));

    /* changing random to demonstrate fault in verifyWeight */
    for (i = 0; i < 8; i++)
    {
      random[i] = 'a';
    }

    buildSignedWeight(signedWeight, weight, random, signature);

    //Invoke verifyWeight
    status = verifyWeight(handle, tagNo, isActive, signedWeight, sizeof(signedWeight), pResult);
  }

  return status;
}

/********************************************************************************/
/** End Utility Functions *******************************************************/
/********************************************************************************/



/********************************************************************************/
/*** Unit test functions ********************************************************/
/********************************************************************************/

hasp_handle_t handle = HASP_INVALID_HANDLE_VALUE;

int init_suite1(void)
{
  hasp_status_t status;

  status = openAoC(&handle);
  if (status != HASP_STATUS_OK)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

int clean_suite1(void)
{
  closeAoC(handle);
  return 0;
}

void testSuccessfullAuthenticateTag(void)
{
  int tagNo;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char signedChallenge[106];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  /* Tag initial authentication with isActive = false */
  for (tagNo = 0; tagNo < 16; tagNo++)
  {
    memset(certificate, 0, sizeof(certificate));
    memset(random, 0, sizeof(random));

	//fill info with demo information
	memset(info, tagNo, sizeof(info));

    createPubKC(pubKC, tagNo);

	//create message to sign
	memcpy(message,info,sizeof(info));
	memcpy(&(message[35]),pubKC,sizeof(pubKC));

    signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

    buildCertificate(certificate, message, signature);

    CU_ASSERT(verifyTag(handle, tagNo, 0 /* false */, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
    CU_ASSERT(result == 1);

    if (result == 1)
    {
      memset(signature, 0, sizeof(signature));
      
	  builedSignedChallenge(signedChallenge,random, tagNo);

      CU_ASSERT(verifyChallenge(handle, tagNo, 0 /* false */, signedChallenge, sizeof(signedChallenge), &result) == HASP_STATUS_OK);
      CU_ASSERT(result == 1);
    }
  }
}

void testSuccessfullAuthenticateTagActive(void)
{
  int tagNo;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char signedChallenge[106];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  /* Tag initial authentication with isActive = false */
  for (tagNo = 0; tagNo < 8; tagNo++)
  {
    memset(certificate, 0, sizeof(certificate));
    memset(random, 0, sizeof(random));

    //fill info with demo information
	memset(info, tagNo, sizeof(info));

    createPubKC(pubKC, tagNo);

	//create message to sign
	memcpy(message,info,sizeof(info));
	memcpy(&(message[35]),pubKC,sizeof(pubKC));

    signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

    buildCertificate(certificate, message, signature);

    CU_ASSERT(verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
    CU_ASSERT(result == 1);

    if (result == 1)
    {
       memset(signature, 0, sizeof(signature));

       builedSignedChallenge(signedChallenge,random, tagNo);

       CU_ASSERT(verifyChallenge(handle, tagNo, 1 /* true */, signedChallenge, sizeof(signedChallenge), &result) == HASP_STATUS_OK);
       CU_ASSERT(result == 1);
    }
  }
}

/* Adding additional tag when all slots are occupied */
void testScenario2_1(void)
{
  int tagNo;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;
  hasp_status_t status;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  tagNo = 12;
  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  status = verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result);
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
}

void testScenario2_2(void)
{
  int tagNo;
  int result = 0;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  //printf("Scenario 2: Removing a tag\n");
  tagNo = 6;
  //printf("Invoke removeTag\n");
  CU_ASSERT(removeTag(handle, tagNo, &result) == HASP_STATUS_OK);
  CU_ASSERT(result == 1);
}

/* Removing a tag that doesn't exist - should fail */
void testScenario2_3(void)
{
  hasp_status_t status;
  int tagNo;
  int result = 0;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  //printf("Scenario 2: Removing a tag that doesn't exist - should fail\n");
  tagNo = 6;
  //printf("Invoke removeTag\n");

  status = removeTag(handle, tagNo, &result);
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
}

/* Adding a tag successfully after a tag was removed */
void testScenario2_4(void)
{

  int tagNo = 12;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char signedChallenge[106];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  CU_ASSERT(verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
  CU_ASSERT(result == 1);

  if (result == 1)
  {
    memset(signature, 0, sizeof(signature));

    builedSignedChallenge(signedChallenge,random, tagNo);

    CU_ASSERT(verifyChallenge(handle, tagNo, 1 /* true */, signedChallenge, sizeof(signedChallenge), &result) == HASP_STATUS_OK);
    CU_ASSERT(result == 1);
  }
}

/* updating a tag */
void testScenario2_5(void)
{
  int tagNo = 12;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char signedChallenge[106];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  CU_ASSERT(verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
  CU_ASSERT(result == 1);

  if (result == 1)
  {
    memset(signature, 0, sizeof(signature));

    builedSignedChallenge(signedChallenge,random, tagNo);

    CU_ASSERT(verifyChallenge(handle, tagNo, 1 /* true */, signedChallenge, sizeof(signedChallenge), &result) == HASP_STATUS_OK);
    CU_ASSERT(result == 1);
  }
}

/* verification is not passed */
void testScenario3_1(void)
{
  int tagNo;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  tagNo = 1;
  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  /* incorrect signing of the message */
  signMessage_wrong(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  CU_ASSERT(verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
  CU_ASSERT(result == 0); /* verify message not passed */
}

/* Certificate cannot be parsed or signature length is invalid */
void testScenario3_2(void)
{
  int tagNo;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;
  hasp_status_t status;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  tagNo = 12;
  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate_invalid(certificate, message, signature);

  status = verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result);
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
}

/* tagNo is invalid (less than min or greater than max) */
void testScenario3_3(void)
{
  int tagNo;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;
  hasp_status_t status;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  tagNo = 18;
  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  status = verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result);
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
}

/* tagNo is valid, but tag cannot be found */
void testScenario3_4(void)
{
  int tagNo;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;
  hasp_status_t status;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  tagNo = 13;
  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  status = verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result);
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
}

/* Verification is not passed */
void testScenario4_1(void)
{
  int tagNo = 1;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char signedChallenge[106];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  CU_ASSERT(verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
  CU_ASSERT(result == 1);

  if (result == 1)
  {
	
	builedSignedChallenge(signedChallenge,random, tagNo);
    /* wrong signing of challenge */
    memset(&signedChallenge[34],'1',72);

    CU_ASSERT(verifyChallenge(handle, tagNo, 1 /* true */, signedChallenge, sizeof(signedChallenge), &result) == HASP_STATUS_OK);
    CU_ASSERT(result == 0);
  }
}

/* tagNo != lastTagNo or isActive != lastIsActive */
void testScenario4_2(void)
{
  int tagNo = 1;
  unsigned char info[35];
  unsigned char pubKC[72];
  unsigned char message[107];
  unsigned char signature[72];
  unsigned char signedChallenge[106];
  unsigned char certificate[181];
  unsigned char random[8];
  int result;
  hasp_status_t status;

  CU_ASSERT(handle != HASP_INVALID_HANDLE_VALUE);

  memset(certificate, 0, sizeof(certificate));
  memset(random, 0, sizeof(random));

  //fill info with demo information
  memset(info, tagNo, sizeof(info));

  createPubKC(pubKC, tagNo);

  //create message to sign
  memcpy(message,info,sizeof(info));
  memcpy(&(message[35]),pubKC,sizeof(pubKC));

  signMessage(message, sizeof(message), signature, priv_ks_bin, sizeof(priv_ks_bin));

  buildCertificate(certificate, message, signature);

  CU_ASSERT(verifyTag(handle, tagNo, 1 /* true */, certificate, sizeof(certificate), random, &result) == HASP_STATUS_OK);
  CU_ASSERT(result == 1);

  if (result == 1)
  {
    builedSignedChallenge(signedChallenge,random, tagNo);

    tagNo = tagNo + 1;

    status = verifyChallenge(handle, tagNo, 1 /* true */, signedChallenge, sizeof(signedChallenge), &result);
    CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
  }
}

/*  First update weight (Consumption 0)                                          */
void testScenario5_1(void)
{
  int tagNo;
  int result = 0 /* false */;
  int *p = &result;

  tagNo = 5;
  CU_ASSERT(updateWeight(handle, tagNo, 1 /* true */, 0, 100, p) == HASP_STATUS_OK);
  CU_ASSERT(result == 1);
}

/*  Update weight (Consumption != 0)                                             */
void testScenario5_2(void)
{
  int tagNo;
  int result = 0 /* false */;
  int *p = &result;

  tagNo = 5;
  CU_ASSERT(updateWeight(handle, tagNo, 1 /* true */, 10, 90, p) == HASP_STATUS_OK);
  CU_ASSERT(result == 1);
}


//Scenario 6:  Failure scenarios for verifyWeight                              
//  1. random is not equal to lastRandom                                         
void testScenario6_1(void)
{
  int tagNo;
  hasp_status_t status;
  int result = 0;
  int *p = &result;

  //random is not equal to lastRandom\n");
  tagNo = 6;
  status = updateWeight_wrongRandom(handle, tagNo, 1 /* true */, 0, 200, p );
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
  
}

//  2. New weight is not equal to subtraction of consumption from weight        
void testScenario6_2(void)
{
  int tagNo;
  hasp_status_t status;
  int result = 0;
  int *p = &result;

  //New weight is not equal to subtraction of consumption from weight. (Consumption = 20, Weight = 80 last weight was 90)
  tagNo = 5;
  status = updateWeight(handle, tagNo, 1 /* true */, 20, 80, p);
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
}

// 3. New weigh is 0.                                                           
void testScenario6_3(void)
{
  int tagNo;
  hasp_status_t status;
  int result = 0;
  int *p = &result;


  //New weigh is 0
  tagNo = 7;
  status = updateWeight(handle, tagNo, 1 /* true */, 0, 0, p);
  CU_ASSERT(status == HASP_VM_METHOD_EXCEPTION_OCCUR || status == HASP_STATUS_OK && result == 0);
}



int main(int argc, char *argv[])
{
  CU_pSuite pSuite;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("Suite_1", init_suite1, clean_suite1);
   if (pSuite == NULL)
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "16 test of Authenticate Tag isActive false", testSuccessfullAuthenticateTag)) ||
     (NULL == CU_add_test(pSuite, "8 test of Authenticate Tag isActive true", testSuccessfullAuthenticateTagActive)) ||
     (NULL == CU_add_test(pSuite, "Adding additional tag when all slots are occupied", testScenario2_1)) ||
     (NULL == CU_add_test(pSuite, "Removing a tag", testScenario2_2)) ||
     (NULL == CU_add_test(pSuite, "Removing a tag that doesn't exist - should fail", testScenario2_3)) ||
     (NULL == CU_add_test(pSuite, "Adding a tag successfully after a tag was removed", testScenario2_4)) ||
     (NULL == CU_add_test(pSuite, "updating a tag", testScenario2_5))||
     (NULL == CU_add_test(pSuite, "Verification is not passed", testScenario3_1)) ||
     (NULL == CU_add_test(pSuite, "Certificate cannot be parsed or signature length is invalid", testScenario3_2)) ||
     (NULL == CU_add_test(pSuite, "tagNo is invalid (less than min or greater than max)", testScenario3_3)) ||
     (NULL == CU_add_test(pSuite, "tagNo is valid, but tag cannot be found", testScenario3_4))||
     (NULL == CU_add_test(pSuite, "Verification is not passed", testScenario4_1)) ||
     (NULL == CU_add_test(pSuite, "tagNo != lastTagNo or isActive != lastIsActive", testScenario4_2)) ||
	 (NULL == CU_add_test(pSuite, "First update weight (Consumption 0)", testScenario5_1)) ||
	 (NULL == CU_add_test(pSuite, "Update weight (Consumption != 0)", testScenario5_2)) ||
	 (NULL == CU_add_test(pSuite, "random is not equal to lastRandom", testScenario6_1)) ||
	 (NULL == CU_add_test(pSuite, "New weight is not equal to subtraction of consumption from weight ", testScenario6_2)) ||
	 (NULL == CU_add_test(pSuite, "New weigh is 0 ", testScenario6_3)) )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

  /* Run all tests using the CUnit Basic interface */
  CU_basic_run_tests();
  CU_cleanup_registry();

  /* getchar(); */

  return CU_get_error();

}  /* main */
