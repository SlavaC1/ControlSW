unit ObjList;

{

Unit ObjList
Copyright (c) Sebastian Reichelt, 2000

Objekt-Listen für Objektorientierte Programmierung nach den Regeln des
Software Engineering


TObjectList
	TObjectList ist die Basisklasse für Objektlisten. Sie stellt alle
	Eigenschaften und Methoden für eine abstrakte Liste von Objekten zur
	Verfügung. Sie ähnelt TList, wobei statt Zeigern Objekte eingefügt
	werden können.
	TObjectList sollte nur zum akuten Abspeichern einer bestimmten Auswahl
	an Objekten benutzt werden. Für alle weiteren Aufgaben stehen die
	anderen Listen zur Verfügung.

TObjectContainer
	Jedes Objekt, das parallel zu anderen gleichartigen Objekten dynamisch
	erzeugt wird, benötigt ein übergeordnetes Objekt (Owner), das sämtliche
	dynamischen Objekte verwaltet.
	Ein solches übergeordnetes Objekt sollte ein Objekt der Klasse
	TObjectContainer oder einer davon abgeleiteten Klasse sein. Die
	dynamischen Objekte müssen dann von TContainerItem abgeleitet und beim
	Erstellen der übergeordnete Container als Parameter an den Constructor
	Create übergeben werden.
	Beim Freigeben oder Leeren (Clear) des Containers werden dann alle
	untergeordneten Objekte aus dem Speicher entfernt. Ebenso wird beim
	Freigeben eines untergeordneten Objektes dieses aus dem Container
	gelöscht.
	Um bei einem untergeordneten Objekt den Container zu wechseln, müssen
	Sie einfach der Eigenschaft Owner einen neuen Wert zuweisen. Der Owner
	kann auch NIL sein, dann handelt es sich um ein ganz normales Objekt.
	Mit PerformItemAction kann eine Integer-Konstante als Aktion an alle
	untergeordneten Objekte übergeben werden. Dort wird dann die virtuelle
	Methode PerformAction aufgerufen, die das Ereignis OnAction auslöst.
	Da diese Klasse von TCollection abgeleitet wurde, wird das Objekt in
	die Stream-Komponentenspeicherung mit einbezogen.

TConnectionList
	Um ein Objekt auf bestimmte Weise mit einem oder mehreren anderen zu
	verbinden, muß jedem der Objekte eine TConnectionList hinzugefügt werden.
	Die Verbindung zwischen zwei Objekten kann dann hergestellt werden,
	indem man die Klassenmethode ConnectLists mit den beiden Listen der
	Objekte als Parameter aufruft. Mit DisconnectLists läßt sich die
	Verbindung wieder aufheben. Alternativ dazu kann man auch die Methode
	ConnectTo bzw. DisconnectFrom einer der beiden Listen verwenden.
	Auch bei dieser Klasse gibt es eine Methode PerformItemAction, die bei
	allen verknüpften Listen ein OnAction-Ereignis auslöst (s.o.).
	Auch diese Klasse wurde in die Stream-Speicherung einbezogen.

TReferenceList
	Bei TReferenceList handelt es sich um eine spezielle Form der akuten
	Objektliste. Sie wird benutzt, wenn ein Objekt, das sich in einem
	Container befindet, genau ein Objekt eines bestimmten Typs referenzieren
	muß, dieses eine Objekt aber beliebig viele andere referenzieren kann.
	Dieses Objekt muß dann selbstverständlich an den Konstruktor des
	ContainerItem übergeben und dann in das private Symbol einer
	Eigenschaft eingetragen werden. Außerdem muß das erstellte ContainerItem
	mit der Methode Add in eine Liste des Typs TReferenceList des zu
	referenzierenden Objekts eingetragen werden. Ebenso muß es beim
	Entfernen mit Delete wieder ausgetragen werden. Das funktioniert auch
	mit akuten Listen, aber bei TReferenceList ergeben sich einige Vorteile.
	Neu ist, daß beim Entfernen eines Objekts mit Referenzliste(n) alle
	referenzierten Objekte (die ContainerItems irgendeines beliebigen
	Containers sind) gelöscht werden. Das gleiche geschieht beim Aufruf von
	Clear.
	Auch neu ist der gewohnte Einsatz von PerformItemAction bei einer
	Referenzliste.
	TReferenceList wird nicht automatisch gespeichert. Die Instanzen sollten
	daher automatisch verwaltet werden.

}

{$WEAKPACKAGEUNIT}

interface

uses
	Classes;

type
	TObjectNotifyEvent = procedure(Sender, Item: TObject) of object;

	TOwnedPersistent = class(TPersistent)
	private
		FOwner: TPersistent;
	protected
		procedure SetOwner(const Value: TPersistent); virtual;
		function GetOwner: TPersistent; override;
	public
		constructor Create(AOwner: TPersistent); virtual;
		destructor Destroy; override;
		property Owner: TPersistent read FOwner write SetOwner;
	published
	end;

	TObjectList = class(TList)
	private
		FOwner: TObject;
	protected
		function Get(Index: Integer): TObject;
		procedure Put(Index: Integer; Item: TObject);
	public
		constructor Create(AOwner: TObject);
		function Add(Item: TObject): Integer;
		procedure Insert(Index: Integer; Item: TObject);
		procedure Delete(Index: Integer); overload;
		procedure Delete(Item: TObject); overload;
		function IndexOf(Item: TObject): Integer;
		property Items[Index: Integer]: TObject read Get write Put; default;
		property Owner: TObject read FOwner;
	end;

	TContainerItem = class;

	TSubItemNotifyEvent = procedure(Sender: TObject; Item: TContainerItem) of object;

	TReferenceList = class(TObjectList)
	private
		FOnAdd: TSubItemNotifyEvent;
		FOnDelete: TSubItemNotifyEvent;
		function Get(Index: Integer): TContainerItem;
	protected
	public
		destructor Destroy; override;
		procedure Clear; override;
		procedure Add(Item: TContainerItem);
		procedure Delete(Item: TContainerItem);
		procedure PerformItemAction(Action: Integer; Obj: TObject = nil); virtual;
		function IndexOf(Item: TContainerItem): Integer;
		property Items[Index: Integer]: TContainerItem read Get; default;
		property OnAdd: TSubItemNotifyEvent read FOnAdd write FOnAdd;
		property OnDelete: TSubItemNotifyEvent read FOnDelete write FOnDelete;
	end;

	TObjectContainer = class(TOwnedCollection)
	private
		function GetContainerOwner: TPersistent;
	protected
	public
		procedure PerformItemAction(Action: Integer; Obj: TObject = nil); virtual;
		property Owner: TPersistent read GetContainerOwner;
	end;

	TActionNotifyEvent = procedure(Sender: TObject; Action: Integer; Obj: TObject) of object;

	TContainerItem = class(TCollectionItem)
	private
		FOnAction: TActionNotifyEvent;
		function GetItemOwner: TObjectContainer;
		procedure SetItemOwner(const Value: TObjectContainer);
	protected
		procedure PerformAction(Action: Integer; Obj: TObject); virtual;
	public
		property Owner: TObjectContainer read GetItemOwner write SetItemOwner;
		property OnAction: TActionNotifyEvent read FOnAction write FOnAction;
	end;

	TConnectionList = class;

	TConItem = class(TCollectionItem)
	private
		FConList: TConnectionList;
		procedure SetConList(const Value: TConnectionList);
	public
		destructor Destroy; override;
	published
		property ConList: TConnectionList read FConList write SetConList;
	end;

	TConCollection = class(TOwnedCollection)
	private
	public
		function SearchForList(List: TConnectionList): TConItem;
	end;

	TConnectionNotifyEvent = procedure(Sender: TObject; Item: TConnectionList) of object;

	TConnectionList = class(TOwnedPersistent)
	private
		FCollection: TConCollection;
		FOnConnect: TConnectionNotifyEvent;
		FOnDisconnect: TConnectionNotifyEvent;
		FOnAction: TActionNotifyEvent;
		function GetCount: Integer;
		procedure SetCollection(const Value: TConCollection);
	protected
		procedure Add(Item: TConnectionList);
		procedure Delete(Item: TConnectionList);
		procedure PerformAction(Action: Integer; Obj: TObject = nil); virtual;
	public
		class procedure ConnectLists(List1, List2: TConnectionList);
		class procedure DisconnectLists(List1, List2: TConnectionList);
		constructor Create(AOwner: TPersistent); override;
		destructor Destroy; override;
		procedure Clear;
		procedure PerformItemAction(Action: Integer; Obj: TObject); virtual;
		procedure ConnectTo(List: TConnectionList);
		procedure DisconnectFrom(List: TConnectionList);
		property Count: Integer read GetCount;
		property OnConnect: TConnectionNotifyEvent read FOnConnect write FOnConnect;
		property OnDisconnect: TConnectionNotifyEvent read FOnDisconnect write FOnDisconnect;
		property OnAction: TActionNotifyEvent read FOnAction write FOnAction;
	published
		property Collection: TConCollection read FCollection write SetCollection;
	end;

implementation

{ TObjectList }

function TObjectList.Add(Item: TObject): Integer;
begin
	Result := inherited Add (Pointer (Item));
end;

procedure TObjectList.Delete(Index: Integer);
begin
	inherited Delete (Index);
end;

procedure TObjectList.Delete(Item: TObject);
begin
	inherited Delete (IndexOf (Item));
end;

function TObjectList.Get(Index: Integer): TObject;
begin
	Result := TObject (inherited Get (Index));
end;

function TObjectList.IndexOf(Item: TObject): Integer;
begin
	Result := inherited IndexOf (Pointer (Item));
end;

procedure TObjectList.Insert(Index: Integer; Item: TObject);
begin
	inherited Insert (Index, Pointer (Item));
end;

procedure TObjectList.Put(Index: Integer; Item: TObject);
begin
	inherited Put (Index, Pointer (Item));
end;

constructor TObjectList.Create(AOwner: TObject);
begin
	inherited Create;
	FOwner := AOwner;
end;

{ TReferenceList }

procedure TReferenceList.Add(Item: TContainerItem);
begin
	if IndexOf (Item) < 0 then begin
		if Assigned (FOnAdd) then
			FOnAdd (Self, Item);
		inherited Add (Item);
	end;
end;

procedure TReferenceList.Clear;
var
	I: Integer;
begin
	for I := Count - 1 downto 0 do
		Items[I].Free;
	inherited;
end;

procedure TReferenceList.Delete(Item: TContainerItem);
begin
	if Assigned (FOnDelete) then
		FOnDelete (Self, Item);
	inherited Delete (Item);
end;

destructor TReferenceList.Destroy;
begin
	Clear;
	inherited;
end;

function TReferenceList.Get(Index: Integer): TContainerItem;
begin
	Result := TContainerItem (inherited Get (Index));
end;

function TReferenceList.IndexOf(Item: TContainerItem): Integer;
begin
	Result := inherited IndexOf (Item);
end;

procedure TReferenceList.PerformItemAction(Action: Integer; Obj: TObject);
var
	I: Integer;
begin
	for I := Count - 1 downto 0 do
		Items[I].PerformAction(Action, Obj);
end;

{ TContainerItem }

function TContainerItem.GetItemOwner: TObjectContainer;
begin
	Result := TObjectContainer (Collection);
end;

procedure TContainerItem.PerformAction(Action: Integer; Obj: TObject);
begin
	if Assigned (FOnAction) then
		FOnAction (Self, Action, Obj);
end;

procedure TContainerItem.SetItemOwner(const Value: TObjectContainer);
begin
	Collection := Value;
end;

{ TConnectionList }

procedure TConnectionList.Add(Item: TConnectionList);
begin
	if not Assigned (FCollection.SearchForList (Item)) then begin
		with TConItem(FCollection.Add) do
			ConList := Item;
	end;
end;

procedure TConnectionList.Clear;
var
	I: Integer;
begin
	for I := Count - 1 downto 0 do
		DisconnectFrom (TConItem(FCollection.Items[I]).ConList);
	FCollection.Clear;
end;

class procedure TConnectionList.ConnectLists(List1, List2: TConnectionList);
begin
	List1.Add (List2);
	List2.Add (List1);
end;

procedure TConnectionList.ConnectTo(List: TConnectionList);
begin
	ConnectLists (Self, List);
end;

constructor TConnectionList.Create(AOwner: TPersistent);
begin
	inherited;
	FCollection := TConCollection.Create (Self, TConItem);
end;

procedure TConnectionList.Delete(Item: TConnectionList);
var
	I: TConItem;
begin
	I := FCollection.SearchForList (Item);
	if Assigned (I) then
		I.Free;
end;

destructor TConnectionList.Destroy;
begin
	Clear;
	FCollection.Free;
	inherited;
end;

procedure TConnectionList.DisconnectFrom(List: TConnectionList);
begin
	DisconnectLists (Self, List);
end;

class procedure TConnectionList.DisconnectLists(List1, List2: TConnectionList);
begin
	List1.Delete (List2);
	List2.Delete (List1);
end;

function TConnectionList.GetCount: Integer;
begin
	Result := FCollection.Count;
end;

procedure TConnectionList.PerformAction(Action: Integer; Obj: TObject);
begin
	if Assigned (FOnAction) then
		FOnAction (Self, Action, Obj);
end;

procedure TConnectionList.PerformItemAction(Action: Integer; Obj: TObject);
var
	I: Integer;
begin
	for I := Count - 1 downto 0 do
		TConItem(FCollection.Items[I]).ConList.PerformAction(Action, Obj);
end;

procedure TConnectionList.SetCollection(const Value: TConCollection);
begin
	FCollection.Assign (Value);
end;

{ TObjectContainer }

function TObjectContainer.GetContainerOwner: TPersistent;
begin
	Result := GetOwner;
end;

procedure TObjectContainer.PerformItemAction(Action: Integer; Obj: TObject);
var
	I: Integer;
begin
	for I := Count - 1 downto 0 do
		TContainerItem(Items[I]).PerformAction(Action, Obj);
end;

{ TOwnedPersistent }

constructor TOwnedPersistent.Create(AOwner: TPersistent);
begin
	inherited Create;
	SetOwner (AOwner);
end;

destructor TOwnedPersistent.Destroy;
begin
	SetOwner (nil);
	inherited;
end;

function TOwnedPersistent.GetOwner: TPersistent;
begin
	Result := FOwner;
end;

procedure TOwnedPersistent.SetOwner(const Value: TPersistent);
begin
	FOwner := Value;
end;

{ TConItem }

destructor TConItem.Destroy;
begin
	SetConList (nil);
	inherited;
end;

procedure TConItem.SetConList(const Value: TConnectionList);
begin
	if Assigned (FConList) and Assigned (FConList.FOnDisconnect) then
		FConList.FOnDisconnect (FConList, TConnectionList(TConCollection(Collection).GetOwner));
	FConList := Value;
	if Assigned (FConList) and Assigned (FConList.FOnConnect) then
		FConList.FOnConnect (FConList, TConnectionList(TConCollection(Collection).GetOwner));
end;

{ TConCollection }

function TConCollection.SearchForList(List: TConnectionList): TConItem;
var
	I:   Integer;
	Res: TConItem;
begin
	Result := nil;
	for I := 0 to Count - 1 do begin
		Res := TConItem (Items [I]);
		if Res.ConList = List then begin
			Result := Res;
			Break;
		end;
	end;
end;

end.
