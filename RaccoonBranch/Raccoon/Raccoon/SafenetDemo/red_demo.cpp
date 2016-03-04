
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

extern "C" {
#include "REDApp_utils.h"
}

/********************************************************************************/
/** Utility Functions ***********************************************************/
/********************************************************************************/

/* Composes Certificate */
void buildCertificate(byte *certificate, byte *message, byte *signature)
{
  int messageSize = 72;
  int signatureSize = 72;
  int signatureSizeOffset = messageSize;
  int signatureOffset = signatureSizeOffset + 2;
  int i;
  short temp = 0;

  for (i = 0; i < messageSize; i++)
  {
    certificate[i] = message[i];
  }

  certificate[signatureSizeOffset + 1] = messageSize >> 8 & 0xFF;
  certificate[signatureSizeOffset] = messageSize & 0xFF;

  temp = (certificate[signatureSizeOffset + 1] & 0xFF) << 8 | (certificate[signatureSizeOffset] & 0xFF);

  for (i = 0; i < signatureSize; i++)
  {
    certificate[signatureOffset + i] = signature[i];
  }
}

/* Composes invalid Certificate, with wrong message size */
void buildCertificate_invalid(byte *certificate, byte *message, byte *signature)
{
  int messageSize = 30;       /* wrong message size */
  int signatureSize = 72;
  int signatureSizeOffset = messageSize;
  int signatureOffset = signatureSizeOffset + 2;
  int i;
  short temp = 0;

  for (i = 0; i < messageSize; i++)
  {
    certificate[i] = message[i];
  }

  certificate[signatureSizeOffset + 1] = messageSize >> 8 & 0xFF;
  certificate[signatureSizeOffset] = messageSize & 0xFF;

  temp = (certificate[signatureSizeOffset + 1] & 0xFF) << 8 | (certificate[signatureSizeOffset] & 0xFF);

  for (i = 0; i < signatureSize; i++)
  {
    certificate[signatureOffset + i] = signature[i];
  }
}

/* Mock Public Key */
void createPubKC(byte *pubKC, byte b)
{
  int i;

  for (i = 0; i < 72; i++)
  {
    pubKC[i] = b;
  }
}

/* Mock message signing as right, mock verifyMessage in Java is looking for 11 in third byte */
void signMessage(byte *message, byte *signature)
{
  signature[2] = 11;
}

/*  Mock message signing as wrong */
void signMessage_wrong(byte *message, byte *signature)
{
  signature[2] = 12;
}

/* Executes Tag Authentication procedure */
bool authenticateTag(hasp_handle_t handle, byte tagNo, bool isActive)
{
  hasp_status_t status;
  byte pubKC[72];
  byte signature[72];
  byte certificate[146];
  byte random[8];
  bool result = false;

  memset(certificate, 0, sizeof(certificate));
  memset(random, 1, sizeof(random));

  createPubKC(pubKC, 15);
  signMessage(pubKC, signature);

  buildCertificate(certificate, pubKC, signature);

  printf("Invoke verifyTag\n");
  status = verifyTag(handle, tagNo, isActive, certificate, sizeof(certificate), random, &result);
  printf("status: %u, result: %d\n", status, result);

  if (result)
  {
    signMessage(random, signature);

    printf("Invoke verifyChallenge\n");
    status = verifyChallenge(handle, tagNo, isActive, signature, sizeof(signature), &result);
    printf("status: %u, result: %d\n", status, result);
  }

  return result;
}

/* Sample of fault in Tag Authentication Procedure - incorrect signed message */
bool authenticateTag_wrongMessage(hasp_handle_t handle, byte tagNo, bool isActive)
{
  hasp_status_t status;
  byte pubKC[72];
  byte signature[72];
  byte certificate[146];
  byte random[8];
  bool result = false;

  memset(certificate, 0, sizeof(certificate));
  memset(random, 1, sizeof(random));

  createPubKC(pubKC, 15);

  /* incorrect signing of the message */
  signMessage_wrong(pubKC, signature);

  buildCertificate(certificate, pubKC, signature);

  printf("Invoke verifyTag\n");
  status = verifyTag(handle, tagNo, isActive, certificate, sizeof(certificate), random, &result);
  printf("status: %u, result: %d\n", status, result);

  if (result)
  {
    signMessage(random, signature);

    printf("Invoke verifyChallenge\n");
    status = verifyChallenge(handle, tagNo, isActive, signature, sizeof(signature), &result);
    printf("status: %u, result: %d\n", status, result);
  }

  return result;
}

/* Another sample of fault in Tag Authentication Procedure - sending wrong random to verifyChallenge */
bool authenticateTag_wrongRandomSig(hasp_handle_t handle, byte tagNo, bool isActive)
{
  hasp_status_t status;
  byte pubKC[72];
  byte signature[72];
  byte certificate[146];
  byte random[8];
  bool result = false;

  memset(certificate, 0, sizeof(certificate));
  memset(random, 1, sizeof(random));

  createPubKC(pubKC, 15);
  signMessage(pubKC, signature);

  buildCertificate(certificate, pubKC, signature);

  printf("Invoke verifyTag\n");
  status = verifyTag(handle, tagNo, isActive, certificate, sizeof(certificate), random, &result);
  printf("status: %u, result: %d\n", status, result);

  if (result)
  {
    /* incorrect signing of the message */
    signMessage_wrong(random, signature);

    printf("Invoke verifyChallenge\n");
    status = verifyChallenge(handle, tagNo, isActive, signature, sizeof(signature), &result);
    printf("status: %u, result: %d\n", status, result);
  }

  return result;
}

/* Another sample of fault in Tag Authentication Procedure - sending incorrct tagNo */
bool authenticateTag_changeTagNo(hasp_handle_t handle, byte tagNo, bool isActive)
{
  hasp_status_t status;
  byte pubKC[72];
  byte signature[72];
  byte certificate[146];
  byte random[8];
  bool result = false;

  memset(certificate, 0, sizeof(certificate));
  memset(random, 1, sizeof(random));

  createPubKC(pubKC, 15);
  signMessage(pubKC, signature);

  buildCertificate(certificate, pubKC, signature);

  printf("Invoke verifyTag\n");
  status = verifyTag(handle, tagNo, isActive, certificate, sizeof(certificate), random, &result);
  printf("status: %u, result: %d\n", status, result);

  if (result)
  {
    signMessage(random, signature);

    /* changing of tagNo */
    tagNo = tagNo + 1;

    printf("Invoke verifyChallenge\n");
    status = verifyChallenge(handle, tagNo, isActive, signature, sizeof(signature), &result);
    printf("status: %u, result: %d\n", status, result);
  }

  return result;
}

/* Another sample of fault in Tag Authentication Procedure - cerificate cannot be parsed (message size is invalid) */
bool authenticateTag_invalidCertificate(hasp_handle_t handle, byte tagNo, bool isActive)
{
  hasp_status_t status;
  byte pubKC[72];
  byte signature[72];
  byte certificate[146];
  byte random[8];
  bool result = false;

  memset(certificate, 0, sizeof(certificate));
  memset(random, 1, sizeof(random));

  createPubKC(pubKC, 15);
  signMessage(pubKC, signature);

  /* builds invalid certificate */
  buildCertificate_invalid(certificate, pubKC, signature);

  printf("Invoke verifyTag\n");
  status = verifyTag(handle, tagNo, isActive, certificate, sizeof(certificate), random, &result);
  printf("status: %u, result: %d\n", status, result);

  if (result)
  {
    signMessage(random, signature);

    printf("Invoke verifyChallenge\n");
    status = verifyChallenge(handle, tagNo, isActive, signature, sizeof(signature), &result);
    printf("status: %u, result: %d\n", status, result);
  }

  return result;
}

/* Composes signed weight */
bool buildSignedWeight(byte *signedWeight, int weight, byte *random, byte *signature)
{
  int weightLen = 4;
  int WeightOffset = 0;
  int randomLen = 8;
  int randomOffset = WeightOffset + weightLen;
  int signatureLen = 72;
  int signatureLenOffset = randomOffset + randomLen;
  int signatureOffset = signatureLenOffset + 2;
  int i;
  short temp = 0;

  signedWeight[WeightOffset + 3] = weight >> 27 & 0xFF;
  signedWeight[WeightOffset + 2] = weight >> 16 & 0xFF;
  signedWeight[WeightOffset + 1] = weight >> 8 & 0xFF;
  signedWeight[WeightOffset] = weight & 0xFF;

  /* just to check the number is OK */
  temp = (signedWeight[WeightOffset + 3] << 24) |
         (signedWeight[WeightOffset + 2] & 0xFF) << 16 |
         (signedWeight[WeightOffset + 1] & 0xFF) << 8 |
         (signedWeight[WeightOffset + 0] & 0xFF);

  for (i = 0; i < randomLen; i++)
  {
    signedWeight[randomOffset + i] = random[i];
  }

  signedWeight[signatureLenOffset + 1] = signatureLen >> 8 & 0xFF;
  signedWeight[signatureLenOffset] = signatureLen & 0xFF;

  temp = (short)((signedWeight[signatureLenOffset + 1] & 0xFF) << 8 | (signedWeight[signatureLenOffset + 0] & 0xFF));

  for (i = 0; i < signatureLen; i++)
  {
    signedWeight[signatureOffset + i] = signature[i];
  }

  return true;
}

/* Executes Updates Weight Procedure */
bool updateWeight(hasp_handle_t handle, byte tagNo, bool isActive, int consumption, int weight)
{
  hasp_status_t status;
  byte signedWeight[86];
  byte random[8];
  byte signature[72];
  bool result = false;

  memset(signedWeight, 0, sizeof(signedWeight));
  memset(random, 1, sizeof(random));

  printf("Invoke updateConsumption\n");
  status = updateConsumption(handle, tagNo, isActive, consumption, random, &result);
  printf("status: %u, result: %d\n", status, result);

  if (result)
  {
    signMessage(random, signature);

    buildSignedWeight(signedWeight, weight, random, signature);

    printf("Invoke verifyWeight\n");
    status = verifyWeight(handle, tagNo, isActive, signedWeight, sizeof(signedWeight), &result);
    printf("status: %u, result: %d\n", status, result);
  }

  return result;
}

/* Sample of fault in Update Weight Procedure - incorrect random is sent back */
bool updateWeight_wrongRandom(hasp_handle_t handle, byte tagNo, bool isActive, int consumption, int weight)
{
  hasp_status_t status;
  byte signedWeight[86];
  byte random[8];
  byte signature[72];
  bool result = false;
  int i;

  memset(signedWeight, 0, sizeof(signedWeight));
  memset(random, 1, sizeof(random));

  printf("Invoke updateConsumption\n");
  status = updateConsumption(handle, tagNo, isActive, consumption, random, &result);
  printf("status: %u, result: %d\n", status, result);

  if (result)
  {
    signMessage(random, signature);

    /* changing random to demonstrate fault in verifyWeight */
    for (i = 0; i < 8; i++)
    {
      random[i] = 'a';
    }

    buildSignedWeight(signedWeight, weight, random, signature);

    printf("Invoke verifyWeight\n");
    status = verifyWeight(handle, tagNo, isActive, signedWeight, sizeof(signedWeight), &result);
    printf("status: %u, result: %d\n", status, result);
  }

  return result;
}

/********************************************************************************/
/** End Utility Functions *******************************************************/
/********************************************************************************/

/***********************************************************************/
/** Scenario Functions *************************************************/
/***********************************************************************/

/***********************************************************************/
/*  Scenario 1:  Successful initialization:                            */
/*  16 + 16 calls with isActive = false                                */
/*  followed by 8 + 8 calls with isActive = true                       */
/***********************************************************************/
void scenario1(hasp_handle_t handle)
{
  int i, tagNo;

  printf("Scenario 1: Successful initialization\n");

  /* Tag initial authentication with isActive = false */
  for (i = 0; i < 16; i++)
  {
    tagNo = i;
    authenticateTag(handle, tagNo, false);
  }

  /* Re-authentication with isActive = true */
  for (i = 0; i < 8; i++)
  {
    tagNo = i;
    authenticateTag(handle, tagNo, true);
  }

  printf("End Scenario 1\n");
}

/***********************************************************************/
/*  Scenario 2:  Testing removeTag                                     */
/*  1. Adding additional tag when all slots are occupied - should fail.*/
/*  2. Removing a tag.                                                 */
/*  3. Removing a tag that doesn't exist - should fail                 */
/*  4. Adding a tag successfully after a tag was removed.              */
/*  5. updating a tag.                                                 */
/***********************************************************************/
void scenario2(hasp_handle_t handle)
{
  hasp_status_t status;
  int tagNo;
  bool result = false;

  printf("Scenario 2: Adding additional tag when all slots are occupied - should fail\n");
  tagNo = 12;
  result = authenticateTag(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("Scenario 2: Removing a tag\n");
  tagNo = 6;
  printf("Invoke removeTag\n");
  status = removeTag(handle, tagNo, &result);
  printf("removeTag status: %u, result: %d\n\n", status, result);

  printf("Scenario 2: Removing a tag that doesn't exist - should fail\n");
  tagNo = 6;
  printf("Invoke removeTag\n");
  status = removeTag(handle, tagNo, &result);
  printf("removeTag status: %u, result: %d\n\n", status, result);

  printf("Scenario 2: Adding a tag successfully after a tag was removed\n");
  tagNo = 12;
  result = authenticateTag(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("Scenario 2: updating a tag\n");
  tagNo = 12;
  result = authenticateTag(handle, tagNo, true);
  printf("update tag result %d\n\n", result);

  printf("End Scenario 2\n");
}

/***********************************************************************/
/*  Scenario 3:  Failure scenarios for verifyTag                       */
/*  1. Verification is not passed                                      */
/*  2. Certificate cannot be parsed or signature length is invalid     */
/*  3. tagNo is invalid (less than min or greater than max)            */
/*  4. tagNo is valid, but tag cannot be found                         */
/***********************************************************************/
void scenario3(hasp_handle_t handle)
{
  int tagNo;
  bool result = false;

  printf("Scenario 3: Verification is not passed\n");
  tagNo = 6;
  result = authenticateTag_wrongMessage(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("Scenario 3: Certificate cannot be parsed or signature length is invalid\n");
  tagNo = 7;
  result = authenticateTag_invalidCertificate(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("Scenario 3: tagNo is invalid (less than min or greater than max)\n");
  tagNo = 18;
  result = authenticateTag(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("Scenario 3: tagNo is valid, but tag cannot be found\n");
  tagNo = 13;
  result = authenticateTag(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("End Scenario 3\n");
}

/***********************************************************************/
/*  Scenario 4: Failure scenarios for verifyChallange                  */
/*  1. Verification is not passed                                      */
/*  2. tagNo != lastTagNo or isActive != lastIsActive                  */
/***********************************************************************/
void scenario4(hasp_handle_t handle)
{
  int tagNo;
  bool result = false;

  printf("Scenario 4: Verification is not passed\n");
  tagNo = 1;
  result = authenticateTag_wrongRandomSig(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("Scenario 4: tagNo != lastTagNo or isActive != lastIsActive\n");
  tagNo = 1;
  result = authenticateTag_changeTagNo(handle, tagNo, true);
  printf("add tag result %d\n\n", result);

  printf("End Scenario 4\n");
}

/*********************************************************************************/
/*  Scenario 5: Successful Scenarios for updateConsumption/verifyWeight pair     */
/*  First update weight (Consumption 0)                                          */
/*  Update weight (Consumption != 0)                                             */
/*********************************************************************************/
void scenario5(hasp_handle_t handle)
{
  int tagNo;
  bool result = false;

  printf("Scenario 5: Scenarios for updateConsumption/verifyWeight pair\n");

  printf("Successful first update weight:  Consumption = 0, Weight = 100\n");
  tagNo = 5;
  result = updateWeight(handle, tagNo, true, 0, 100);
  printf("Update Weight result %d\n\n", result);

  printf("Successful update weight:  Consumption = 10, Weight = 90\n");
  tagNo = 5;
  result = updateWeight(handle, tagNo, true, 10, 90);
  printf("Update Weight result %d\n\n", result);

  printf("End Scenario 5\n");
}

/*********************************************************************************/
/*  Scenario 6:  Failure scenarios for verifyWeight                              */
/*  1. random is not equal to lastRandom                                         */
/*  2. New weight is not equal to subtraction of consumption from weight         */
/*  3. New weigh is 0.                                                           */
/*********************************************************************************/
void scenario6(hasp_handle_t handle)
{
  int tagNo;
  bool result = false;

  printf("Scenario 6: Failure scenarios for verifyWeight\n");

  printf("random is not equal to lastRandom\n");
  tagNo = 6;
  result = updateWeight_wrongRandom(handle, tagNo, true, 0, 200);
  printf("Update Weight result %d\n\n", result);

  printf("New weight is not equal to subtraction of consumption from weight. (Consumption = 20, Weight = 80 last weight was 90)\n");
  tagNo = 5;
  result = updateWeight(handle, tagNo, true, 20, 80);
  printf("Update Weight result %d\n\n", result);

  printf("New weigh is 0\n");
  tagNo = 7;
  result = updateWeight(handle, tagNo, true, 0, 0);
  printf("Update Weight result %d\n\n", result);

  printf("End Scenario 6\n");
}

/***********************************************************************/
/*  Scenario 7:  Testing Application Placeholders                      */
/*    CNOF                                                             */
/*    CEOP                                                             */
/*    CNOF                                                             */
/*    CNOF                                                             */
/***********************************************************************/
void scenario7(hasp_handle_t handle)
{
  hasp_status_t status;
  int result = false;

  printf("Scenario 7:\n");

  printf("Invoke CNOF:\n");
  status = CNOF(handle, 1, 2, 3, 4, 5, &result);
  printf("CNOF status %u, result %i\n\n", status, result);

  printf("Invoke CEOP:\n");
  status = CEOP(handle, 1, 2, 3, 4, 5, 6, &result);
  printf("CEOP status %u, result %i\n\n", status, result);

  printf("Invoke CNOP:\n");
  status = CNOP(handle, 1, 2, 3, 4, 5, 6, &result);
  printf("CNOP status %u, result %i\n\n", status, result);

  printf("Invoke CYSO:\n");
  status = CYSO(handle, 1, 2, 3, 4, 5, 6, 7, 8, 9, &result);
  printf("CYSO status %u, result %i\n\n", status, result);

  printf("End Scenario 7\n");
}

/***********************************************************************/
/** End Scenario Functions *********************************************/
/***********************************************************************/

int main(int argc, char *argv[])
{
  hasp_handle_t handle;         /* AoC API handle */
  hasp_status_t status;

  (void)argc;                   /* unused */
  (void)argv;                   /* unused */

  getchar();

  /* login and initlialize vm engine */
  status = openAoC(&handle);
  if (status != HASP_STATUS_OK)
  {
    printf("login failed with error %u\n", status);
    exit(EXIT_FAILURE);
  }

  /* run scenarios */
  scenario1(handle);
  printf("\n\n\n");

  scenario2(handle);
  printf("\n\n\n");

  scenario3(handle);
  printf("\n\n\n");

  scenario4(handle);
  printf("\n\n\n");

  scenario5(handle);
  printf("\n\n\n");

  scenario6(handle);
  printf("\n\n\n");

  scenario7(handle);
  printf("\n\n\n");

  /* close VM and logout from HASP */
  closeAoC(handle);

  getchar();

  return EXIT_SUCCESS;

}  /* main */
