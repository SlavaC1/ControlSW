/********************************************************************
*                        Objet Geometries LTD.                     *
*                        ---------------------                     *
* Project: Generic                                                 *
* Module: Automatic wizards generator                              *
* Module Description: Wizard pages implementation for python.      *
*                                                                  *
* Compilation: Standard C++.                                       *
*                                                                  *
* Author: Ran Peleg.                                               *
* Start date: 30/06/2003                                           *
* Last upate: 03/07/2003                                           *
********************************************************************/

#include "PythonWizardPages.h"
#include "PythonAutoWizard.h"
#include "QPythonIntegrator.h"
#include "QPythonUtils.h"

// Wizard message page
void CPythonMessageWizardPage::RefreshPythonAttributes()
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());
	Title = GetAttrFromPyObjAsString(PyPageObj,"Title");
	SubTitle = GetAttrFromPyObjAsString(PyPageObj,"SubTitle");
}


// Wizard status page
void CPythonStatusWizardPage::RefreshPythonAttributes(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());

	SubTitle = GetAttrFromPyObjAsString(PyPageObj,"SubTitle");
	StatusMessage = GetAttrFromPyObjAsString(PyPageObj,"StatusMessage");
}


// Wizard progress page
void CPythonProgressWizardPage::RefreshPythonAttributes(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());

	SubTitle = GetAttrFromPyObjAsString(PyPageObj,"SubTitle");
	Min = GetAttrFromPyObjAsInt(PyPageObj,"Min");
	Max = GetAttrFromPyObjAsInt(PyPageObj,"Max");
	Progress = GetAttrFromPyObjAsInt(PyPageObj,"Progress");
}


// Wizard checkboxes page
void CPythonCheckBoxWizardPage::RefreshPythonAttributes(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1()); // todo -oNobody -cNone: suspicious reinterpret cast

	SubTitle = GetAttrFromPyObjAsString(PyPageObj,"SubTitle");
	SetChecksMask(GetAttrFromPyObjAsInt(PyPageObj,"ChecksMask"));
    SetDisclosureDefaultState(GetAttrFromPyObjAsInt(PyPageObj,"DisclosureDefaultState"));
    SetNumOfDisclosedElements(GetAttrFromPyObjAsInt(PyPageObj,"NumOfDisclosedElements"));
	GetAttrFromPyObjAsStringList(PyPageObj,"Strings",Strings);
    SetBoldIndex(GetAttrFromPyObjAsInt(PyPageObj,"BoldIndex"));
}


bool CPythonCheckBoxWizardPage::NotifyEvent(int Param1,int Param2)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());

	int ChecksMask = GetChecksMask();

	// Update bit mask  // todo -oNobody -cNone: redundant?
	if(Param2)
	ChecksMask |= 1 << Param1;
	else
	ChecksMask &= ~(1 << Param1);

	SetAttrOfPyObjAsInt(PyPageObj,"ChecksMask",ChecksMask);

	return CCheckBoxWizardPage::NotifyEvent(Param1,Param2);
}

// Wizard radiogroup page
void CPythonRadioGroupWizardPage::RefreshPythonAttributes(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());

	SubTitle = GetAttrFromPyObjAsString(PyPageObj,"SubTitle");
	DefaultOption = GetAttrFromPyObjAsInt(PyPageObj,"DefaultOption");
    SetDisabledMask(GetAttrFromPyObjAsInt(PyPageObj,"DisabledMask"));
	GetAttrFromPyObjAsStringList(PyPageObj,"Strings",Strings);
}


bool CPythonRadioGroupWizardPage::NotifyEvent(int Param1,int Param2)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());

	SetAttrOfPyObjAsInt(PyPageObj,"SelectedOption",Param1);

	return CRadioGroupWizardPage::NotifyEvent(Param1,Param2);
}

// Wizard progress/status page
void CPythonProgressStatusWizardPage::RefreshPythonAttributes(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());

	SubTitle = GetAttrFromPyObjAsString(PyPageObj,"SubTitle");
	Min = GetAttrFromPyObjAsInt(PyPageObj,"Min");
	Max = GetAttrFromPyObjAsInt(PyPageObj,"Max");
	Progress = GetAttrFromPyObjAsInt(PyPageObj,"Progress");
	StatusMessage = GetAttrFromPyObjAsString(PyPageObj,"StatusMessage");
	DefaultMode = (TProgressStatusMode)GetAttrFromPyObjAsInt(PyPageObj,"DefaultMode");
	CurrentMode = (TProgressStatusMode)GetAttrFromPyObjAsInt(PyPageObj,"CurrentMode");
}


// Wizard Generic page
void CPythonGenericCustomWizardPage::RefreshPythonAttributes(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());

	SubTitle = GetAttrFromPyObjAsString(PyPageObj,"SubTitle");
}


// Set argument value (override)
void CPythonGenericCustomWizardPage::SetArg(const QString Arg,int ArgNum)
{
	CGenericCustomWizardPage::SetArg(Arg,ArgNum);

	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());
	TPythonEngine *PyEng = GetPythonEngine();

	CPyAutoDecRef ListObj(PyEng->PyObject_GetAttr(PyPageObj,PyEng->PyString_FromString("Args")));

	if(ListObj == NULL)
	throw EPythonAutoWizard("\"Args\" attribute could not be found");

	// Check if the return type is a list
	if(!PyEng->PyList_Check(ListObj))
	throw EPythonAutoWizard("Object \"Args\" is not a list");

	PyEng->PyList_SetItem(ListObj, ArgNum, PyEng->PyString_FromString(const_cast<char *>(Arg.c_str())));
}

// Get argument value (override)
QString CPythonGenericCustomWizardPage::GetArg(int ArgNum)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());
	TPythonEngine *PyEng = GetPythonEngine();

	CPyAutoDecRef ListObj(PyEng->PyObject_GetAttr(PyPageObj,PyEng->PyString_FromString("Args")));

	if(ListObj == NULL)
	throw EPythonAutoWizard("\"Args\" attribute could not be found");

	// Check if the return type is a list
	if(!PyEng->PyList_Check(ListObj))
	throw EPythonAutoWizard("Object \"Args\" is not a list");

	int ItemNum = PyEng->PyList_Size(ListObj);

	for(int i = 0; i < std::min(ItemNum,MAX_GENERIC_CUSTOM_PAGE_ARGS); i++)
	{
		PPyObject ItemObj = PyEng->PyList_GetItem(ListObj,i);

		AnsiString AnsiStr = PyEng->PyObjectAsString(ItemObj);
		SetArg(AnsiStr.c_str(),i);
	}

	return CGenericCustomWizardPage::GetArg(ArgNum);
}

TWizardPageType CPythonGenericCustomWizardPage::GetPageType(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());
	return (TWizardPageType)GetAttrFromPyObjAsInt(PyPageObj,"Type");
}

// Param1 indicates the field number
bool CPythonDataEntryWizardPage::NotifyEvent(int Param1,int Param2)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());
	TPythonEngine *PyEng = GetPythonEngine();

	CPyAutoDecRef ListObj(PyEng->PyObject_GetAttr(PyPageObj,PyEng->PyString_FromString("FieldsValues")));

	if(ListObj == NULL)
	throw EPythonAutoWizard("\"FieldsValues\" attribute could not be found");

	// Check if the return type is a list
	if(!PyEng->PyList_Check(ListObj))
	throw EPythonAutoWizard("Object \"FieldsValues\" is not a list");

	int ItemNum = PyEng->PyList_Size(ListObj);

	for(int i = 0; i < std::min(ItemNum,MAX_DATA_ENTRY_FIELDS); i++)
	PyEng->PyList_SetItem(ListObj,i,PyEng->PyString_FromString(const_cast<char *>(FieldsValues[i].c_str())));

	return CDataEntryWizardPage::NotifyEvent(Param1,Param2);
}

//  CPythonDataEntryWizardPage
void CPythonDataEntryWizardPage::RefreshPythonAttributes(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());
	TPythonEngine *PyEng = GetPythonEngine();

	GetAttrFromPyObjAsStringList(PyPageObj,"Strings",Strings);
	GetAttrFromPyObjAsStringList(PyPageObj,"FieldsValues",FieldsValues);

	CPyAutoDecRef ListObj(PyEng->PyObject_GetAttr(PyPageObj,PyEng->PyString_FromString("FieldsTypes")));

	if(ListObj == NULL)
	throw EPythonAutoWizard("\"FieldsTypes\" attribute could not be found");

	// Check if the return type is a list
	if(!PyEng->PyList_Check(ListObj))
	throw EPythonAutoWizard("Object \"FieldsTypes\" is not a list");

	int ItemNum = PyEng->PyList_Size(ListObj);

	for(int i = 0; i < std::min(ItemNum,MAX_DATA_ENTRY_FIELDS); i++)
	{
		PPyObject ItemObj = PyEng->PyList_GetItem(ListObj,i);
		FieldsTypes[i] = (TDataEntryFieldType)PyEng->PyInt_AsLong(ItemObj);
	}
}

TWizardPageType CPythonDataEntryWizardPage::GetPageType(void)
{
	PPyObject PyPageObj = reinterpret_cast<PPyObject>(GetTag1());
	return (TWizardPageType)GetAttrFromPyObjAsInt(PyPageObj,"Type"); // todo -oNobody -cNone: prefer c++ cast
}


