/****************************************************************************
*
* Demo program for Sentinel LDK licensing services
*
*
* Copyright (C) 2011, SafeNet, Inc. All rights reserved.
*
*
* Sentinel LDK DEMOMA key with features 1 and 42 required
*
****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>

#include "hasp_api.h"
#include "hasp_vcode.h"       /* contains HASP DEMOMA vendor code */
#include "hasp_io_buffer.h"

#define VM_RESOURCE_ID  1
#define VM_METHOD_ID    6


/****************************************************************************
 * helper function: dumps a given block of data, in hex
 */

#define DUMP_COLS_PER_ROW     16
#define DUMP_SPACE_COLS        8

void dump_hex(unsigned char *data, unsigned int datalen, char *margin)
{
    unsigned int i;

    for (i = 0; i < datalen; i++)
    {
        if (((i % DUMP_SPACE_COLS) == 0) && (i != 0))
            printf(" ");

        if ((i % DUMP_COLS_PER_ROW) == 0)
        {
            if (i != 0)
                printf("\n");

            if (margin != NULL)
                printf("%s", margin);
        }

        /* dump character in hex */
        printf("%02X ", data[i]);
    }
} /* dump_hex */

/****************************************************************************
 * helper function: dumps a given block of data, in ascii
 */

void dump_ascii(unsigned char *data, unsigned int datalen, char *margin)
{
    unsigned int i;

    for (i = 0; i < datalen; i++)
    {
        if (((i % DUMP_SPACE_COLS) == 0) && (i != 0))
            printf(" ");

        if ((i % DUMP_COLS_PER_ROW) == 0)
        {
            if (i != 0)
                printf("\n");

            if (margin != NULL)
                printf("%s", margin);
        }

        /* dump printable character in ascii */
        printf("%c", ((data[i] > 31) && (data[i] < 128)) ? data[i] : '.');
    }
} /* dump_ascii */

/****************************************************************************
 * helper function: dumps a given block of data, in hex and ascii
 */

void dump(unsigned char *data, unsigned int datalen, char *margin)
{
    unsigned int i, icols;

    for (i = 0; i < datalen; )
    {
        icols = datalen - i;

        if (icols > DUMP_COLS_PER_ROW)
            icols = DUMP_COLS_PER_ROW;

        dump_hex(&data[i], icols, margin);
        dump_ascii(&data[i], icols, "  ");
        printf("\n");

        i += icols;
    }
} /* dump */

/***************************************************************************/

void wait4key(char *hint)
{
    fflush(stdout);
    fflush(stdin);
    printf("\npress ENTER %s\n", hint);
    while (getchar() == EOF)
        ;
}

/***************************************************************************/

int main(void)
{
   hasp_status_t   status;
   hasp_handle_t   handle;
   hasp_u32_t      vm_returncode = 0;
   hasp_size_t     iolength = 0;
   unsigned char   iobuffer[256] = { 0 };
   char            RWResultBuffer[64] ={ 0 };
   VM_IO_BUFFER_T  iobuffer_cxt = { 0 }; 
   const char* scope = 
                        "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                        "<haspscope>"
                        "    <hasp type=\"HASP-HL\" >"
                        "        <license_manager hostname=\"localhost\" />"
                        "    </hasp>"
                        "</haspscope>";
						

   printf("A simple demo program for the Sentinel LDK licensing functions\n");
   printf("Copyright (C) SafeNet, Inc. All rights reserved.\n\n");

   /*
   Access Direct Mode (-4 mode) [TEST-ONLY]
   This mode force the API to always access directly DriverLess keys even if the local LM is recent enough to provide such support.
   To enable this special mode, you must use the undocumented call hasp_login(-4, 0, 0) before any other hasp functions. 
   */
   hasp_login(-4, 0, 0);
   
   /************************************************************************
   * hasp_login_scope
   *   establishes a context for HASP services
   *   allows specification of several restrictions
   */

   printf("restricting the license to 'local':\n");

   status = hasp_login_scope(HASP_DEFAULT_FID,
                    scope,
                    (hasp_vendor_code_t *)vendor_code,
                    &handle);
   switch (status)
   {
   case HASP_STATUS_OK:
      printf("OK\n");
      break;

   case HASP_FEATURE_NOT_FOUND:
      printf("login to default feature failed\n");
      break;

   case HASP_CONTAINER_NOT_FOUND:
      printf("no sentinel key with vendor code DEMOMA found\n");
      break;

   case HASP_OLD_DRIVER:
      printf("outdated driver version installed\n");
      break;

   case HASP_NO_DRIVER:
      printf("sentinel driver not installed\n");
      break;

   case HASP_INV_VCODE:
      printf("invalid vendor code\n");
      break;

   case HASP_INV_SCOPE:
      printf("invalid XML scope\n");
      break;

   default:
      printf("login to default feature failed with status %d\n", status);
   }
   if (status) {
      exit(-1);
   }


   /************************************************************************
   * Initlialize vm engine 
   */
   status = hasp_vm_init(handle, VM_RESOURCE_ID);
   if (status) {
      hasp_logout(handle);
      printf("hasp_vm_init failed: %d!\n", status);
      exit(-1);
   }
   else
   {
      printf("hasp_vm_init succeeded!\n");
   }       

   /************************************************************************
   /* Prepare input buffer and to invoke the method of vm resource */
   iobuffer_cxt.cmdBuff = iobuffer;
   iobuffer_cxt.rspBuff = iobuffer;
   iolength = iobuffer_cxt.cmdSize;

   printf(" vm/apps/ExceptClass.init : ()V\n");
   status = hasp_vm_invoke(handle,
      0,//method id      
      iobuffer, 
      &iolength,
      &vm_returncode);
   if (status) {
      hasp_logout(handle);
      printf("hasp_vm_invoke failed: %d!\n", status);
      exit(-1);
   }
   else
   {
      printf("hasp_vm_invoke succeeded!\n");
   }

   printf(" vm/apps/ExceptClass.getException : (I)V\n");

   put_int(&iobuffer_cxt, 9);
   iolength = 128;
   vm_returncode = 0;
   status = hasp_vm_invoke(handle,
      1,//method id      
      iobuffer, 
      &iolength,
      &vm_returncode);
   if (status) {
      printf("hasp_vm_invoke failed: %d!\nexception code: %d\n", status, vm_returncode);
	  printf("exception info is  :%s\n", iobuffer);
   }
   else
   {
      printf("hasp_vm_invoke succeeded!\n");
   }


   /************************************************************************
   /* hasp_vm_close must be called to free vm engine for other processes or threads usage*/
   status = hasp_vm_close(handle);
   if (status) {
      hasp_logout(handle);
      printf("hasp_vm_close failed: %d!\n", status);
      exit(-1);
   }
   else
   {
      printf("hasp_vm_close succeeded!\n");
   }

   /************************************************************************
   * hasp_logout
   *   closes established session and releases allocated memory
   */
   printf("\nlogout                           : ");
   status = hasp_logout(handle);

   switch (status)
   {
   case HASP_STATUS_OK:
      printf("OK\n");
      break;

   case HASP_INV_HND:
      printf("failed: handle not active\n");
      break;

   default:
      printf("failed\n");
   }
  
   /***********************************************************************/

   wait4key("to close the sample");

   return 0;

} /* main */

