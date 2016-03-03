// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Atompythonengine.pas' rev: 11.00

#ifndef AtompythonengineHPP
#define AtompythonengineHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Variants.hpp>	// Pascal unit
#include <Pythonengine.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Atompythonengine
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TAtomPythonEngine;
class PASCALIMPLEMENTATION TAtomPythonEngine : public Pythonengine::TPythonEngine 
{
	typedef Pythonengine::TPythonEngine inherited;
	
public:
	virtual Variant __fastcall PyObjectAsVariant(Pythonengine::PPyObject obj);
public:
	#pragma option push -w-inl
	/* TPythonEngine.Create */ inline __fastcall virtual TAtomPythonEngine(Classes::TComponent* AOwner) : Pythonengine::TPythonEngine(AOwner) { }
	#pragma option pop
	#pragma option push -w-inl
	/* TPythonEngine.Destroy */ inline __fastcall virtual ~TAtomPythonEngine(void) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Atompythonengine */
using namespace Atompythonengine;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Atompythonengine
