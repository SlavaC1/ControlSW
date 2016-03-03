Information regarding the BCB6STLFix project
============================================

1. The BDB6STL Project is needed in order to fix a BCB6 bug concerning STL.

2. To understand the bug, read the "bug report from borland Quality Central.htm" found in this folder.

3. This project creates the STL .lib + .dll files. To find out how, read: "Public beta STLport 4_5 for Borland C++ Builder 6.htm" also found in this folder.

4. The actual "bug fix" was adding following line to _config.h: 
   #    define _STLP_WIN32THREADS

5. Another small change was applied to the bcb60mt.mak and bcb60.mak (the change is described in: "Public beta STLport 4_5 for Borland C++ Builder 6.htm" found in this folder.)

6. Note that all of the files in this directory are already changed as required in order to build the fixed stl libararies.

7. In order to build the libraries, run cmd and type: (we only use multi-threaded libraries)

    * To make multi-threaded static STLport library: make -DMT
    * To make multi-threaded dynamic STLport library: make -DMT -DDLL

8. The resulting libraries will be in \BCB6StlFix\lib

9. The resulting .DLL libraries should be copied into \Bin directory of the BCB6 installation:
  C:\DevTools\Borland\CBuilder6\Bin

  DLL File names are:
   * stlpmt_stldebug45.dll
   * stlpmt45.dll


10. The resulting .Lib libraries should be copied into \Lib directory of the BCB6 installation:
  C:\DevTools\Borland\CBuilder6\Lib

  LIB File names are:
   * stlpmt.lib
   * stlpmt_stldebug.lib
   * stlpmt_stldebugi.lib
   * stlpmti.lib









