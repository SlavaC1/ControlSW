
all: objs

objs:
	cd src
!if $d(DLL)
!  if $d(MT)
	make -f ..\bcb60mt.mak all_dynamic
!  else
	make -f ..\bcb60.mak all_dynamic
!  endif
!else
!  if $d(MT)
	make -f ..\bcb60mt.mak all_static
!  else
	make -f ..\bcb60.mak all_static
!  endif
!endif

clean:
	del /sxyzq lib