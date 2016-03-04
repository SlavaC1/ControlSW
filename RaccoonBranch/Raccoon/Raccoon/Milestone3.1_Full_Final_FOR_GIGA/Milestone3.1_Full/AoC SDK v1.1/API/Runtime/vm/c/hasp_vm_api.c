#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hasp_api.h"

static hasp_u32_t g_vm_id = 0;

hasp_status_t HASP_CALLCONV hasp_vm_init(
                           hasp_handle_t      handle,
                           hasp_fileid_t      vm_id)
{

   hasp_status_t status;
   hasp_u32_t method_id, input_size, output_size;
   hasp_s32_t method_result;
   unsigned char input[256] = { 0 };
   unsigned char output[256] = { 0 };

   input_size = 0;
   output_size = sizeof(output);
   g_vm_id = vm_id;

   method_id = vm_id << 16 | 0xFFFF;//resource id: method id, if method is 0xFFFF, it mean it is vm_init command
   status = hasp_vm_dyn_execute(handle, NULL, 0, method_id, input, input_size, output, &output_size, &method_result);

   return status;
}

hasp_status_t HASP_CALLCONV hasp_vm_invoke(
                              hasp_handle_t       handle,
                              hasp_u16_t          method_id,
                              void *              iobuffer,
                              hasp_size_t *       length,
                              hasp_s32_t *        vm_returncode)
{

   hasp_status_t status;
   hasp_u32_t c_method_id, input_size, output_size;
   unsigned char input[256] = { 0 };
   unsigned char output[256] = { 0 };

   if (iobuffer == NULL || length == NULL || vm_returncode == NULL)  {
      return HASP_INVALID_PARAMETER;
   }

   output_size = sizeof(output);
   input_size = *length;
   memcpy(input, iobuffer, *length);
   c_method_id = (g_vm_id << 16) | method_id;//resource id: method id
   status = hasp_vm_dyn_execute(handle, NULL, 0, c_method_id, input, input_size, output, &output_size, vm_returncode);

   if (status == 0)   {
      if (output_size > 0) {
         memcpy(iobuffer, output, output_size);
      }

      *length = output_size;
   }
   else
   {
      if (output_size > 1) {
         /* The first byte is length, but we must zero terminate the array */
         if (output_size >= (*length))  {
           return HASP_INSUF_MEM;
         }
         else
         {
            output[output_size] = 0;
            memcpy(iobuffer, output + 1, output_size);
         }
      }
   }

   return status;
}

hasp_status_t HASP_CALLCONV hasp_vm_close(hasp_handle_t handle)
{
   g_vm_id = 0;
   return 0;
}
