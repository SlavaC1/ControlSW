#ifndef IO_BUFFER_HEADER
#define IO_BUFFER_HEADER

#ifdef __cplusplus
extern "C" {
#endif

typedef struct vm_io_buff_context
{
   hasp_s32_t      cmdSize;
   hasp_s32_t      rspOff;
   hasp_s8_t *     cmdBuff;
   hasp_s8_t *     rspBuff;
}VM_IO_BUFFER_T, *PVM_IO_BUFFER_T;

void put_bool(PVM_IO_BUFFER_T piobuffer, int data);
void put_byte(PVM_IO_BUFFER_T piobuffer, hasp_s8_t data);
void put_short(PVM_IO_BUFFER_T piobuffer, hasp_s16_t data);
void put_wchar(PVM_IO_BUFFER_T piobuffer, wchar_t data);
void put_int(PVM_IO_BUFFER_T piobuffer, hasp_s32_t data);

void put_byte_array(PVM_IO_BUFFER_T piobuffer, hasp_s8_t* data, hasp_u8_t length);
void put_bool_array(PVM_IO_BUFFER_T piobuffer, int *data, hasp_u8_t length);
void put_short_array(PVM_IO_BUFFER_T piobuffer, hasp_s16_t* data, hasp_u8_t length);
void put_wchar_arrary(PVM_IO_BUFFER_T piobuffer, wchar_t* data, hasp_u8_t length);
void put_hasp_int_array(PVM_IO_BUFFER_T piobuffer, hasp_s32_t* data, hasp_u8_t length);

int get_boolean(PVM_IO_BUFFER_T piobuffer);
hasp_s16_t get_short(PVM_IO_BUFFER_T piobuffer);
wchar_t get_wchar(PVM_IO_BUFFER_T piobuffer);
hasp_s32_t get_int(PVM_IO_BUFFER_T piobuffer);

hasp_s32_t get_byte_array(PVM_IO_BUFFER_T piobuffer, hasp_s8_t* data, hasp_u8_t length);
hasp_s32_t get_boolean_array(PVM_IO_BUFFER_T piobuffer, int *data, hasp_u8_t length);
hasp_s32_t get_short_array(PVM_IO_BUFFER_T piobuffer, hasp_s16_t* data, hasp_u8_t length);
hasp_s32_t get_wchar_array(PVM_IO_BUFFER_T piobuffer, wchar_t* data, hasp_u8_t length);
hasp_s32_t get_hasp_int_array(PVM_IO_BUFFER_T piobuffer, hasp_s32_t* data, hasp_u8_t length);

#ifdef __cplusplus
}
#endif

#endif
