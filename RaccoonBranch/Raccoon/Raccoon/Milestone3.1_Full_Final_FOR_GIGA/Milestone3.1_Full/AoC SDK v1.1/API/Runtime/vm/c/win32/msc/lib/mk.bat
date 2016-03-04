@rem
@rem Build hasp_demo.exe and hasp_update.exe using libhasp_windows_demo.lib
@rem

cl -MT -W3 -D_CRT_SECURE_NO_DEPRECATE ..\..\..\hasp_vm_demo_exception.c ..\..\..\hasp_vm_api.c ..\..\..\hasp_io_buffer.c -Fehasp_demo_exception.exe libhasp_windows_demo.lib
cl -MT -W3 -D_CRT_SECURE_NO_DEPRECATE ..\..\..\hasp_vm_demo_k283.c ..\..\..\hasp_vm_api.c ..\..\..\hasp_io_buffer.c -Fehasp_demo_k283.exe libhasp_windows_demo.lib
cl -MT -W3 -D_CRT_SECURE_NO_DEPRECATE ..\..\..\hasp_vm_demo_md5.c ..\..\..\hasp_vm_api.c ..\..\..\hasp_io_buffer.c -Fehasp_demo_md5.exe libhasp_windows_demo.lib
cl -MT -W3 -D_CRT_SECURE_NO_DEPRECATE ..\..\..\hasp_vm_demo_rw.c ..\..\..\hasp_vm_api.c ..\..\..\hasp_io_buffer.c -Fehasp_demo_rw.exe libhasp_windows_demo.lib


@if exist hasp_vm_demo.obj del hasp_vm_demo.obj
@if exist hasp_io_buffer.obj del hasp_io_buffer.obj


@echo.
@dir *.exe
