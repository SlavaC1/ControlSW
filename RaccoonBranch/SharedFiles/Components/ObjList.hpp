// CodeGear C++Builder
// Copyright (c) 1995, 2007 by CodeGear
// All rights reserved

// (DO NOT EDIT: machine generated header) 'Objlist.pas' rev: 11.00

#ifndef ObjlistHPP
#define ObjlistHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <Sysinit.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Objlist
{
//-- type declarations -------------------------------------------------------
typedef void __fastcall (__closure *TObjectNotifyEvent)(System::TObject* Sender, System::TObject* Item);

class DELPHICLASS TOwnedPersistent;
class PASCALIMPLEMENTATION TOwnedPersistent : public Classes::TPersistent 
{
	typedef Classes::TPersistent inherited;
	
private:
	Classes::TPersistent* FOwner;
	
protected:
	virtual void __fastcall SetOwner(const Classes::TPersistent* Value);
	DYNAMIC Classes::TPersistent* __fastcall GetOwner(void);
	
public:
	__fastcall virtual TOwnedPersistent(Classes::TPersistent* AOwner);
	__fastcall virtual ~TOwnedPersistent(void);
	__property Classes::TPersistent* Owner = {read=FOwner, write=SetOwner};
};


class DELPHICLASS TObjectList;
class PASCALIMPLEMENTATION TObjectList : public Classes::TList 
{
	typedef Classes::TList inherited;
	
public:
	System::TObject* operator[](int Index) { return Items[Index]; }
	
private:
	System::TObject* FOwner;
	
protected:
	HIDESBASE System::TObject* __fastcall Get(int Index);
	HIDESBASE void __fastcall Put(int Index, System::TObject* Item);
	
public:
	__fastcall TObjectList(System::TObject* AOwner);
	HIDESBASE int __fastcall Add(System::TObject* Item);
	HIDESBASE void __fastcall Insert(int Index, System::TObject* Item);
	HIDESBASE void __fastcall Delete(int Index)/* overload */;
	HIDESBASE void __fastcall Delete(System::TObject* Item)/* overload */;
	HIDESBASE int __fastcall IndexOf(System::TObject* Item);
	__property System::TObject* Items[int Index] = {read=Get, write=Put/*, default*/};
	__property System::TObject* Owner = {read=FOwner};
public:
	#pragma option push -w-inl
	/* TList.Destroy */ inline __fastcall virtual ~TObjectList(void) { }
	#pragma option pop
	
};


class DELPHICLASS TContainerItem;
typedef void __fastcall (__closure *TSubItemNotifyEvent)(System::TObject* Sender, TContainerItem* Item);

class DELPHICLASS TReferenceList;
class PASCALIMPLEMENTATION TReferenceList : public TObjectList 
{
	typedef TObjectList inherited;
	
public:
	TContainerItem* operator[](int Index) { return Items[Index]; }
	
private:
	TSubItemNotifyEvent FOnAdd;
	TSubItemNotifyEvent FOnDelete;
	HIDESBASE TContainerItem* __fastcall Get(int Index);
	
public:
	__fastcall virtual ~TReferenceList(void);
	virtual void __fastcall Clear(void);
	HIDESBASE void __fastcall Add(TContainerItem* Item);
	HIDESBASE void __fastcall Delete(TContainerItem* Item);
	virtual void __fastcall PerformItemAction(int Action, System::TObject* Obj = (System::TObject*)(0x0));
	HIDESBASE int __fastcall IndexOf(TContainerItem* Item);
	__property TContainerItem* Items[int Index] = {read=Get/*, default*/};
	__property TSubItemNotifyEvent OnAdd = {read=FOnAdd, write=FOnAdd};
	__property TSubItemNotifyEvent OnDelete = {read=FOnDelete, write=FOnDelete};
public:
	#pragma option push -w-inl
	/* TObjectList.Create */ inline __fastcall TReferenceList(System::TObject* AOwner) : TObjectList(AOwner) { }
	#pragma option pop
	
};


class DELPHICLASS TObjectContainer;
class PASCALIMPLEMENTATION TObjectContainer : public Classes::TOwnedCollection 
{
	typedef Classes::TOwnedCollection inherited;
	
private:
	Classes::TPersistent* __fastcall GetContainerOwner(void);
	
public:
	virtual void __fastcall PerformItemAction(int Action, System::TObject* Obj = (System::TObject*)(0x0));
	__property Classes::TPersistent* Owner = {read=GetContainerOwner};
public:
	#pragma option push -w-inl
	/* TOwnedCollection.Create */ inline __fastcall TObjectContainer(Classes::TPersistent* AOwner, TMetaClass* ItemClass) : Classes::TOwnedCollection(AOwner, ItemClass) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TCollection.Destroy */ inline __fastcall virtual ~TObjectContainer(void) { }
	#pragma option pop
	
};


typedef void __fastcall (__closure *TActionNotifyEvent)(System::TObject* Sender, int Action, System::TObject* Obj);

class PASCALIMPLEMENTATION TContainerItem : public Classes::TCollectionItem 
{
	typedef Classes::TCollectionItem inherited;
	
private:
	TActionNotifyEvent FOnAction;
	TObjectContainer* __fastcall GetItemOwner(void);
	void __fastcall SetItemOwner(const TObjectContainer* Value);
	
protected:
	virtual void __fastcall PerformAction(int Action, System::TObject* Obj);
	
public:
	__property TObjectContainer* Owner = {read=GetItemOwner, write=SetItemOwner};
	__property TActionNotifyEvent OnAction = {read=FOnAction, write=FOnAction};
public:
	#pragma option push -w-inl
	/* TCollectionItem.Create */ inline __fastcall virtual TContainerItem(Classes::TCollection* Collection) : Classes::TCollectionItem(Collection) { }
	#pragma option pop
	#pragma option push -w-inl
	/* TCollectionItem.Destroy */ inline __fastcall virtual ~TContainerItem(void) { }
	#pragma option pop
	
};


class DELPHICLASS TConItem;
class DELPHICLASS TConnectionList;
class DELPHICLASS TConCollection;
class PASCALIMPLEMENTATION TConCollection : public Classes::TOwnedCollection 
{
	typedef Classes::TOwnedCollection inherited;
	
public:
	TConItem* __fastcall SearchForList(TConnectionList* List);
public:
	#pragma option push -w-inl
	/* TOwnedCollection.Create */ inline __fastcall TConCollection(Classes::TPersistent* AOwner, TMetaClass* ItemClass) : Classes::TOwnedCollection(AOwner, ItemClass) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TCollection.Destroy */ inline __fastcall virtual ~TConCollection(void) { }
	#pragma option pop
	
};


typedef void __fastcall (__closure *TConnectionNotifyEvent)(System::TObject* Sender, TConnectionList* Item);

class PASCALIMPLEMENTATION TConnectionList : public TOwnedPersistent 
{
	typedef TOwnedPersistent inherited;
	
private:
	TConCollection* FCollection;
	TConnectionNotifyEvent FOnConnect;
	TConnectionNotifyEvent FOnDisconnect;
	TActionNotifyEvent FOnAction;
	int __fastcall GetCount(void);
	void __fastcall SetCollection(const TConCollection* Value);
	
protected:
	void __fastcall Add(TConnectionList* Item);
	void __fastcall Delete(TConnectionList* Item);
	virtual void __fastcall PerformAction(int Action, System::TObject* Obj = (System::TObject*)(0x0));
	
public:
	/*         class method */ static void __fastcall ConnectLists(TMetaClass* vmt, TConnectionList* List1, TConnectionList* List2);
	/*         class method */ static void __fastcall DisconnectLists(TMetaClass* vmt, TConnectionList* List1, TConnectionList* List2);
	__fastcall virtual TConnectionList(Classes::TPersistent* AOwner);
	__fastcall virtual ~TConnectionList(void);
	void __fastcall Clear(void);
	virtual void __fastcall PerformItemAction(int Action, System::TObject* Obj);
	void __fastcall ConnectTo(TConnectionList* List);
	void __fastcall DisconnectFrom(TConnectionList* List);
	__property int Count = {read=GetCount, nodefault};
	__property TConnectionNotifyEvent OnConnect = {read=FOnConnect, write=FOnConnect};
	__property TConnectionNotifyEvent OnDisconnect = {read=FOnDisconnect, write=FOnDisconnect};
	__property TActionNotifyEvent OnAction = {read=FOnAction, write=FOnAction};
	
__published:
	__property TConCollection* Collection = {read=FCollection, write=SetCollection};
};


class PASCALIMPLEMENTATION TConItem : public Classes::TCollectionItem 
{
	typedef Classes::TCollectionItem inherited;
	
private:
	TConnectionList* FConList;
	void __fastcall SetConList(const TConnectionList* Value);
	
public:
	__fastcall virtual ~TConItem(void);
	
__published:
	__property TConnectionList* ConList = {read=FConList, write=SetConList};
public:
	#pragma option push -w-inl
	/* TCollectionItem.Create */ inline __fastcall virtual TConItem(Classes::TCollection* Collection) : Classes::TCollectionItem(Collection) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------

}	/* namespace Objlist */
using namespace Objlist;
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// Objlist
