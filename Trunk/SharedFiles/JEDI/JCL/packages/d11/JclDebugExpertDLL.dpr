Library JclDebugExpertDLL;
{
-----------------------------------------------------------------------------
     DO NOT EDIT THIS FILE, IT IS GENERATED BY THE PACKAGE GENERATOR
            ALWAYS EDIT THE RELATED XML FILE (JclDebugExpertDLL-L.xml)

     Last generated: 27-03-2011  20:46:02 UTC
-----------------------------------------------------------------------------
}

{$R *.res}
{$ALIGN 8}
{$ASSERTIONS ON}
{$BOOLEVAL OFF}
{$DEBUGINFO OFF}
{$EXTENDEDSYNTAX ON}
{$IMPORTEDDATA ON}
{$IOCHECKS ON}
{$LOCALSYMBOLS OFF}
{$LONGSTRINGS ON}
{$OPENSTRINGS ON}
{$OPTIMIZATION ON}
{$OVERFLOWCHECKS OFF}
{$RANGECHECKS OFF}
{$REFERENCEINFO OFF}
{$SAFEDIVIDE OFF}
{$STACKFRAMES OFF}
{$TYPEDADDRESS OFF}
{$VARSTRINGCHECKS ON}
{$WRITEABLECONST ON}
{$MINENUMSIZE 1}
{$IMAGEBASE $58150000}
{$DESCRIPTION 'JCL Debug IDE extension'}
{$LIBSUFFIX '110'}
{$IMPLICITBUILD OFF}

{$DEFINE BCB}
{$DEFINE WIN32}
{$DEFINE CONDITIONALEXPRESSIONS}
{$DEFINE VER180}
{$DEFINE VER185}
{$DEFINE RELEASE}

uses
  ToolsAPI,
  JclDebugIdeResult in '..\..\experts\debug\converter\JclDebugIdeResult.pas' {JclDebugResultForm},
  JclDebugIdeImpl in '..\..\experts\debug\converter\JclDebugIdeImpl.pas' ,
  JclDebugIdeConfigFrame in '..\..\experts\debug\converter\JclDebugIdeConfigFrame.pas' {JclDebugIdeConfigFrame: TFrame}
  ;

exports
  JCLWizardInit name WizardEntryPoint;

end.
