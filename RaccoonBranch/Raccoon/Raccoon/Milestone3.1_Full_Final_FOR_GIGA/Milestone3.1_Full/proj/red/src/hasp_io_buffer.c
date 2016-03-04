
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <windows.h>
#include "hasp_api.h"
#include "hasp_io_buffer.h"

void put_bool(PVM_IO_BUFFER_T piobuffer, int data) {
   if (data /* true */)  {
      piobuffer->cmdBuff[piobuffer->cmdSize++] = 1;
   }
   else {
      piobuffer->cmdBuff[piobuffer->cmdSize++] = 0;
   }
}

void put_byte(PVM_IO_BUFFER_T piobuffer, hasp_s8_t data) {
   piobuffer->cmdBuff[piobuffer->cmdSize++] = data;
}

void put_short(PVM_IO_BUFFER_T piobuffer, hasp_s16_t data) {
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t) (data & 0xff);
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t) ((data >> 8) & 0xff);
}

void put_wchar(PVM_IO_BUFFER_T piobuffer, wchar_t data) {
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t)data;
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t)((data >> 8) & 0xff);
}

void put_int(PVM_IO_BUFFER_T piobuffer, hasp_s32_t data) {
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t)data;
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t)((data >> 8) & 0xff);
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t)((data >> 16) & 0xff);
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_s8_t)((data >> 24) & 0xff);
}

void put_byte_array(PVM_IO_BUFFER_T piobuffer, hasp_s8_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   if( data == NULL )
   {
      piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)0;
      return;
   }
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)length;
   for( i = 0; i < length; i++ ) put_byte(piobuffer, data[i]);
}

void put_bool_array(PVM_IO_BUFFER_T piobuffer, int *data, hasp_u8_t length) {

   hasp_s32_t i;

   if( data == NULL )
   {
      piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)0;
      return;
   }
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)length;
   for( i = 0; i < length; i++ ) put_bool(piobuffer, data[i]);
}


void put_short_array(PVM_IO_BUFFER_T piobuffer, hasp_s16_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   if( data == NULL )
   {
      piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)0;
      return;
   }
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)(length << 1);
   for( i = 0; i < length; i++ ) put_short(piobuffer, data[i]);
}

void put_wchar_arrary(PVM_IO_BUFFER_T piobuffer, wchar_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   if( data == NULL )
   {
      piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)0;
      return;
   }
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)(length << 1);
   for( i = 0; i < length; i++ ) put_wchar(piobuffer, data[i]);
}

void put_hasp_int_array(PVM_IO_BUFFER_T piobuffer, hasp_s32_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   if( data == NULL )
   {
      piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)0;
      return;
   }
   piobuffer->cmdBuff[piobuffer->cmdSize++] = (hasp_u8_t)(length << 2);
   for( i = 0; i < length; i++ ) put_int(piobuffer, data[i]);
}


hasp_s32_t getUByte(PVM_IO_BUFFER_T piobuffer) {
   return piobuffer->rspBuff[piobuffer->rspOff++] & 0xFF;
}

hasp_s8_t getByte(PVM_IO_BUFFER_T piobuffer) {
   return piobuffer->rspBuff[piobuffer->rspOff++];
}


int get_boolean(PVM_IO_BUFFER_T piobuffer) {
   return (int)(piobuffer->rspBuff[piobuffer->rspOff++] != 0);
}

hasp_s16_t get_short(PVM_IO_BUFFER_T piobuffer) {
   return (hasp_s16_t)(getUByte(piobuffer) | (hasp_s16_t)(getUByte(piobuffer) << 8) );
}

wchar_t get_wchar(PVM_IO_BUFFER_T piobuffer) {
   return (wchar_t)(getUByte(piobuffer) | (wchar_t)(getUByte(piobuffer) << 8) );
}

hasp_s32_t get_int(PVM_IO_BUFFER_T piobuffer) {
   return (hasp_s32_t)(getUByte(piobuffer) | (hasp_s32_t)(getUByte(piobuffer) << 8) |
      (hasp_s32_t)(getUByte(piobuffer) << 16) | (hasp_s32_t)(getUByte(piobuffer) << 24) );
}

hasp_s32_t get_byte_array(PVM_IO_BUFFER_T piobuffer, hasp_s8_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   hasp_s32_t n = getUByte(piobuffer);
   if (n < length) {
      return 0;
   }

   for( i = 0; i < n; i++ ) data[i] = getByte(piobuffer);
   return n;
}


hasp_s32_t get_boolean_array(PVM_IO_BUFFER_T piobuffer, int *data, hasp_u8_t length) {
   hasp_s32_t i;

   hasp_s32_t n = getUByte(piobuffer);
   if (n < length) {
      return 0;
   }

   for(i = 0; i < n; i++ ) data[i] = get_boolean(piobuffer);
   return n;
}

hasp_s32_t get_short_array(PVM_IO_BUFFER_T piobuffer, hasp_s16_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   hasp_s32_t n = getUByte(piobuffer) >> 1;
   if (n < length) {
      return 0;
   }

   for( i = 0; i < n; i++ ) data[i] = get_short(piobuffer);
   return n;
}


hasp_s32_t get_wchar_array(PVM_IO_BUFFER_T piobuffer, wchar_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   hasp_s32_t n = getUByte(piobuffer) >> 1;
   if (n < length) {
      return 0;
   }

   for( i = 0; i < n; i++ ) data[i] = get_wchar(piobuffer);
   return n;
}


hasp_s32_t get_hasp_int_array(PVM_IO_BUFFER_T piobuffer, hasp_s32_t* data, hasp_u8_t length) {
   hasp_s32_t i;

   hasp_s32_t n = getUByte(piobuffer) >> 2;
   if (n < length) {
      return 0;
   }

   for( i = 0; i < n; i++ ) data[i] = get_int(piobuffer);
   return n;
}
