
#include "REDApp_utils.h"
#include <stdio.h>

#ifdef USE_DEMOMA
unsigned char vendor_code[] =
  "AzIceaqfA1hX5wS+M8cGnYh5ceevUnOZIzJBbXFD6dgf3tBkb9cvUF/Tkd/iKu2fsg9wAysY"
  "Kw7RMAsVvIp4KcXle/v1RaXrLVnNBJ2H2DmrbUMOZbQUFXe698qmJsqNpLXRA367xpZ54i8k"
  "C5DTXwDhfxWTOZrBrh5sRKHcoVLumztIQjgWh37AzmSd1bLOfUGI0xjAL9zJWO3fRaeB0NS2"
  "KlmoKaVT5Y04zZEc06waU2r6AU2Dc4uipJqJmObqKM+tfNKAS0rZr5IudRiC7pUwnmtaHRe5"
  "fgSI8M7yvypvm+13Wm4Gwd4VnYiZvSxf8ImN3ZOG9wEzfyMIlH2+rKPUVHI+igsqla0Wd9m7"
  "ZUR9vFotj1uYV0OzG7hX0+huN2E/IdgLDjbiapj1e2fKHrMmGFaIvI6xzzJIQJF9GiRZ7+0j"
  "NFLKSyzX/K3JAyFrIPObfwM+y+zAgE1sWcZ1YnuBhICyRHBhaJDKIZL8MywrEfB2yF+R3k9w"
  "FG1oN48gSLyfrfEKuB/qgNp+BeTruWUk0AwRE9XVMUuRbjpxa4YA67SKunFEgFGgUfHBeHJT"
  "ivvUl0u4Dki1UKAT973P+nXy2O0u239If/kRpNUVhMg8kpk7s8i6Arp7l/705/bLCx4kN5hH"
  "HSXIqkiG9tHdeNV8VYo5+72hgaCx3/uVoVLmtvxbOIvo120uTJbuLVTvT8KtsOlb3DxwUrwL"
  "zaEMoAQAFk6Q9bNipHxfkRQER4kR7IYTMzSoW5mxh3H9O8Ge5BqVeYMEW36q9wnOYfxOLNw6"
  "yQMf8f9sJN4KhZty02xm707S7VEfJJ1KNq7b5pP/3RjE0IKtB2gE6vAPRvRLzEohu0m7q1aU"
  "p8wAvSiqjZy7FLaTtLEApXYvLvz6PEJdj4TegCZugj7c8bIOEqLXmloZ6EgVnjQ7/ttys7VF"
  "ITB3mazzFiyQuKf4J6+b/a/Y";
#else
/* HASP vendor code for Stratasys keys */
unsigned char vendor_code[] =
  "6W2exGnhvbj9hJFqQ7fqGDHV1g4nGJcIhAEFq9bedXU+1bljiWWdh4m9BreC69oPQ7IfCzFDZtNzuy2j"
  "VTnJ5Cwowch7iUpVXnq3Cjq1rAZT3GvSKGEoO53Qyu/yx409XFGm0NwSUu6JdPiEIE97F6RxnwK203eX"
  "HH4IMIP7e2qBWUl+37Ml1pItWmtwVSWGYFYfxuP5U2Es9OxqUmJ3dLXQwzaMZms4bTzjR5KlozFMnrDm"
  "/W1YvgqId3Krddq5ozQmDMMz8pgYvVO3AKUclL0Q8NR6nGF5tLrEp1+XC6Mvll7oDIRHVou3RUJuGexP"
  "3pXsHm9FgBekoi08rwWA6TfHVPf4aqhNaWgVqDi636QIm4PyH1YIpgNrGEnO2+GKsCML3x07Vqmm2htr"
  "qm9FV6QHiR2he2pDORAXruOvVtGMivMqXaS/fgY1wyjUwyAjgZM8PP4trsNVIoMBdMFE7nyxymdWt47d"
  "NjebSdDD73OhOoZB5uK+F9rnzwpGSk3zhjS3Ozw/8n/AcWAFsmjR5+sNPixOseLmgRILj7AExbzr9j3T"
  "ndSDHT3uUdVcSDcEVnIf2fdeTUIbIt584r88ITHNvOqctyPpBpiMatnz6AMdPEG0Do7cjX2pm9fPET82"
  "7xH3INOjctcxrgw8EOFfvEyzX9S9XPnsLuNSI5DmlYvY+ICyTSb8HIaqXc7JKAdnIn5xFZJJEnyxzCUV"
  "8WvryDdRqqnlYp4qqcORkjywAaonBtLth1i4Jk4OZGSd/pquzzk1apts2QXpXJnDP/P27e5RJ002PXV3"
  "K/PBr8WoUu/weAkN/i+vmSONYchH4LuwRHDhfnV5zZukm3oIAkkF4wDJTIS6NCXwDcsPZqYgZO5Nm7Qt"
  "GEidhpWBn8yqwrxaqKhl6R8kNjlXQuh1aQSa9F/61PkvYaDVg/u5PavyfbUMIxnHcRHDUqPmiuxWNwhG"
  "NKIfhU+0vTIHQVnzRR3zuw==";
#endif

/********************************************************************************/
/** Basic AoC service methods ***************************************************/
/********************************************************************************/

/*******************************************************************/
/* openAoC(hasp_handle_t *handle)                                  */
/* Logins to HASP key and initlializes vm engine                   */
/* handle - returns a handle to logged in session on the HASP key  */
/*******************************************************************/
hasp_status_t openAoC(hasp_handle_t *handle)
{
  hasp_handle_t h;
  hasp_status_t status;

  /* const char *scope = "<haspscope><hasp type='HASP-HL'><license_manager hostname='localhost'/></hasp></haspscope>"; */

  /* only use Sentinel HL keys starting with firmware 4.50 */
  const char *scope = "<haspscope><hasp type='HASP-HL'><hasp cmp='gt' version='4.50'><license_manager hostname='localhost'/></hasp></hasp></haspscope>";

  /* use runtime-less API only; current LMS is not capable of new static VM function */
  status = hasp_login(-4, 0, NULL);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("openAoC hasp_login(-4) failed with status: %d\n", status);
#endif

    return status;
  }

  /* login to HASP key and get session handle, status should be 0 on success */
  status = hasp_login_scope(0, scope, vendor_code, &h);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("openAoC hasp_login_scope() failed with status: %d\n", status);
#endif

    return status;
  }

  /* initlialize VM engine */
  status = hasp_vm_init(h, REDAppResourceId);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("openAoC hasp_vm_init() failed with status: %d\n", status);
#endif

    hasp_logout(h);
    return status;
  }

  /* pass back handle */
  *handle = h;

  return HASP_STATUS_OK;

} /* openAoC */

/*******************************************************************/
/* closeAoC(hasp_handle_t handle)                                  */
/* frees vm engine                                                 */
/* handle - the handle recieved from login process                 */
/*******************************************************************/
void closeAoC(hasp_handle_t handle)
{
  /* hasp_vm_close must be called to free vm engine for other processes or threads usage */
  hasp_vm_close(handle);

  /* hasp_logout closes established session and releases allocated memory */
  hasp_logout(handle);

} /* closeAoC */

/********************************************************************************/
/** End Basic AoC service methods ***********************************************/
/********************************************************************************/

/********************************************************************************/
/** REDApp wrappers *************************************************************/
/********************************************************************************/

hasp_status_t verifyTag(hasp_handle_t handle, uint8_t tagNo, int isActive, const uint8_t *certificate, unsigned char certificateLen, uint8_t *random, int *result)
{
  hasp_status_t status;              /* AoC API return code */
  unsigned char iobuffer[256];       /* AoC API input/output buffer */
  VM_IO_BUFFER_T iobuffer_cxt;       /* AoC API input/output structure */
  hasp_size_t iolength = 0;          /* AoC API input/output buffer length */
  hasp_u32_t vm_returncode = 0;      /* AoC API return value */

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the verification fails */
  *result = 0 /* false */;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_byte(&iobuffer_cxt, tagNo);
  put_bool(&iobuffer_cxt, isActive);
  put_byte_array(&iobuffer_cxt, (uint8_t *)certificate, certificateLen);
  put_byte_array(&iobuffer_cxt, random, 8);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_verifyTag, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(verifyTag) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  if (vm_returncode == 1)
  {
    /* fetch random output parameter */
    get_byte_array(&iobuffer_cxt, random, 8);
  }

  *result = vm_returncode;

  return status;

} /* verifyTag */

hasp_status_t verifyChallenge(hasp_handle_t handle, uint8_t tagNo, int isActive, const uint8_t *signedChallenge, unsigned char signedChallengeLen, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the verification fails */
  *result = 0 /* false */;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_byte(&iobuffer_cxt, tagNo);
  put_bool(&iobuffer_cxt, isActive);
  put_byte_array(&iobuffer_cxt, (uint8_t *)signedChallenge, signedChallengeLen);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_verifyChallenge, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(verifyChallenge) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  *result = vm_returncode;

  return status;

} /* verifyChallenge */

hasp_status_t removeTag(hasp_handle_t handle, uint8_t tagNo, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the tag removal fails */
  *result = 0;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_byte(&iobuffer_cxt, tagNo);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_removeTag, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(removeTag) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  *result = vm_returncode;

  return status;

} /* removeTag */

hasp_status_t updateConsumption(hasp_handle_t handle, uint8_t tagNo, int isActive, int consumption, uint8_t *random, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;
  int returnValue = 0; /* false */

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the consumption update fails */
  *result = 0 /* false */;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_byte(&iobuffer_cxt, tagNo);
  put_bool(&iobuffer_cxt, isActive);
  put_int(&iobuffer_cxt, consumption);
  put_byte_array(&iobuffer_cxt, random, 8);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_updateConsumption, iobuffer, &iolength, &vm_returncode);

  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(updateConsumption) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  returnValue = (vm_returncode != 0);

  if (returnValue)
  {
    /* fetch random output parameter */
    get_byte_array(&iobuffer_cxt, random, 8);
  }

  *result = returnValue;

  return status;

} /* updateConsumption */

hasp_status_t verifyWeight(hasp_handle_t handle, uint8_t tagNo, int isActive, const uint8_t *signedWeight, unsigned char signedWeightLen, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the verification fails */
  *result = 0 /* false */;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_byte(&iobuffer_cxt, tagNo);
  put_bool(&iobuffer_cxt, isActive);
  put_byte_array(&iobuffer_cxt, (uint8_t *)signedWeight, signedWeightLen);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_verifyWeight, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(verifyWeight) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  *result = (vm_returncode != 0);

  return status;

} /* verifyWeight */

hasp_status_t CNOF(hasp_handle_t handle, int SW, int SR, int LHO1200, int SRX, int IEF, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the operation fails */
  *result = -1;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_int(&iobuffer_cxt, SW);
  put_int(&iobuffer_cxt, SR);
  put_int(&iobuffer_cxt, LHO1200);
  put_int(&iobuffer_cxt, SRX);
  put_int(&iobuffer_cxt, IEF);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_CNOF, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(CNOF) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  *result = vm_returncode;

  return status;

} /* CNOF */

hasp_status_t CEOP(hasp_handle_t handle, int SOP, int SW, int SR, int LHO1200, int AF, int IEF, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the operation fails */
  *result = -1;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_int(&iobuffer_cxt, SOP);
  put_int(&iobuffer_cxt, SW);
  put_int(&iobuffer_cxt, SR);
  put_int(&iobuffer_cxt, LHO1200);
  put_int(&iobuffer_cxt, AF);
  put_int(&iobuffer_cxt, IEF);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_CEOP, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(CEOP) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  *result = vm_returncode;

  return status;

} /* CEOP */

hasp_status_t CNOP(hasp_handle_t handle, int SH, int SST, int SYO, int HPW, int SPEO, int CH, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the operation fails */
  *result = -1;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_int(&iobuffer_cxt, SH);
  put_int(&iobuffer_cxt, SST);
  put_int(&iobuffer_cxt, SYO);
  put_int(&iobuffer_cxt, HPW);
  put_int(&iobuffer_cxt, SPEO);
  put_int(&iobuffer_cxt, CH);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_CNOP, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(CYSO) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

   return status;
  }

  *result = vm_returncode;

  return status;

} /* CNOP */

hasp_status_t CYSO(hasp_handle_t handle, int SH, int SST, int SYO, int HPW, int NGIP, int NOP, int SPEO, int YINOP, int YSINOP, int *result)
{
  hasp_status_t status;
  unsigned char iobuffer[256];
  VM_IO_BUFFER_T iobuffer_cxt;
  hasp_size_t iolength = 0;
  hasp_u32_t vm_returncode = 0;

  /* initialize arrays and structs */
  memset(iobuffer, 0, sizeof(iobuffer));
  memset(&iobuffer_cxt, 0, sizeof(iobuffer_cxt));

  /* assume that the operation fails */
  *result = -1;

  /* prepare Aoc API input buffer */
  iobuffer_cxt.cmdBuff = iobuffer;
  iobuffer_cxt.rspBuff = iobuffer;

  put_int(&iobuffer_cxt, SH);
  put_int(&iobuffer_cxt, SST);
  put_int(&iobuffer_cxt, SYO);
  put_int(&iobuffer_cxt, HPW);
  put_int(&iobuffer_cxt, NGIP);
  put_int(&iobuffer_cxt, NOP);
  put_int(&iobuffer_cxt, SPEO);
  put_int(&iobuffer_cxt, YINOP);
  put_int(&iobuffer_cxt, YSINOP);

  /* iolength = iobuffer_cxt.cmdSize; */
  iolength = sizeof(iobuffer);

  /* invoke the method of VM resource */
  status = hasp_vm_invoke(handle, REDAppMethodId_CYSO, iobuffer, &iolength, &vm_returncode);
  if (status != HASP_STATUS_OK)
  {
#ifdef DEBUG
    printf("hasp_vm_invoke(CYSO) failed with status: %d\n", status);

    if (status == HASP_VM_METHOD_EXCEPTION_OCCUR)
      printf("exception code is %d, exception info is: %s\n", vm_returncode, iobuffer);
#endif

    return status;
  }

  *result = vm_returncode;

  return status;

} /* CYSO */

/********************************************************************************/
/** END REDApp wrappers *********************************************************/
/********************************************************************************/
