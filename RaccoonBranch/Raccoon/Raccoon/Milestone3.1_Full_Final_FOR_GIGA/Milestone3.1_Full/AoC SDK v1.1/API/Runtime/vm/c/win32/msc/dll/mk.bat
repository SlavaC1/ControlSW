@rem
@rem Build hasp_demo.exe and hasp_update.exe using hasp_windows_demo.dll
@rem hasp_windows_demo.lib is an import library.
@rem
@rem Static linkage with libhasp_windows_demo.lib is recommended.
@rem See 'lib' directory.
@rem
@rem The naming conventions for the 32 and 64 bit Windows DLLs are:
@rem
@rem    hasp_windows_<vendorid>.dll
@rem    hasp_windows_x64_<vendorid>.dll
@rem
@rem Please stick to this convention to avoid problems.
@rem

cl -MT -W3 -D_CRT_SECURE_NO_DEPRECATE ..\..\..\hasp_vm_demo_k283.c ..\..\..\hasp_vm_api.c ..\..\..\hasp_io_buffer.c -Fehasp_vm_demo.exe hasp_windows_demo.lib

@if exist hasp_vm_demo.obj del hasp_vm_demo.obj
@if exist hasp_io_buffer.obj del hasp_io_buffer.obj


@echo.
@dir *.exe
