﻿#pragma warning disable 1591
//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated by a tool.
//     Runtime Version:4.0.30319.18444
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace CBS.BSP.DAL
{
	using System.Data.Linq;
	using System.Data.Linq.Mapping;
	using System.Data;
	using System.Collections.Generic;
	using System.Reflection;
	using System.Linq;
	using System.Linq.Expressions;
	using System.ComponentModel;
	using System;
	
	
	[global::System.Data.Linq.Mapping.DatabaseAttribute(Name="RTDB")]
	public partial class RaccoonTagsDataContext : System.Data.Linq.DataContext
	{
		
		private static System.Data.Linq.Mapping.MappingSource mappingSource = new AttributeMappingSource();
		
    #region Extensibility Method Definitions
    partial void OnCreated();
    partial void InsertBatch(Batch instance);
    partial void UpdateBatch(Batch instance);
    partial void DeleteBatch(Batch instance);
    partial void InsertTag(Tag instance);
    partial void UpdateTag(Tag instance);
    partial void DeleteTag(Tag instance);
    partial void InsertBurningAction(BurningAction instance);
    partial void UpdateBurningAction(BurningAction instance);
    partial void DeleteBurningAction(BurningAction instance);
    partial void InsertMaterial(Material instance);
    partial void UpdateMaterial(Material instance);
    partial void DeleteMaterial(Material instance);
    #endregion
		
		public RaccoonTagsDataContext() : 
				base(global::CBS.BSP.Properties.Settings.Default.RTDBConnectionString, mappingSource)
		{
			OnCreated();
		}
		
		public RaccoonTagsDataContext(string connection) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public RaccoonTagsDataContext(System.Data.IDbConnection connection) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public RaccoonTagsDataContext(string connection, System.Data.Linq.Mapping.MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public RaccoonTagsDataContext(System.Data.IDbConnection connection, System.Data.Linq.Mapping.MappingSource mappingSource) : 
				base(connection, mappingSource)
		{
			OnCreated();
		}
		
		public System.Data.Linq.Table<Batch> Batches
		{
			get
			{
				return this.GetTable<Batch>();
			}
		}
		
		public System.Data.Linq.Table<Tag> Tags
		{
			get
			{
				return this.GetTable<Tag>();
			}
		}
		
		public System.Data.Linq.Table<BurningAction> BurningActions
		{
			get
			{
				return this.GetTable<BurningAction>();
			}
		}
		
		public System.Data.Linq.Table<Material> Materials
		{
			get
			{
				return this.GetTable<Material>();
			}
		}
		
		[global::System.Data.Linq.Mapping.FunctionAttribute(Name="dbo.spBurnedTagsCount")]
		public ISingleResult<spBurnedTagsCountResult> spBurnedTagsCount([global::System.Data.Linq.Mapping.ParameterAttribute(DbType="NChar(10)")] string batchNumber)
		{
			IExecuteResult result = this.ExecuteMethodCall(this, ((MethodInfo)(MethodInfo.GetCurrentMethod())), batchNumber);
			return ((ISingleResult<spBurnedTagsCountResult>)(result.ReturnValue));
		}
		
		[global::System.Data.Linq.Mapping.FunctionAttribute(Name="dbo.spHasReferenceTags")]
		public ISingleResult<spHasReferenceTagsResult> spHasReferenceTags([global::System.Data.Linq.Mapping.ParameterAttribute(DbType="NChar(10)")] string batchNumber)
		{
			IExecuteResult result = this.ExecuteMethodCall(this, ((MethodInfo)(MethodInfo.GetCurrentMethod())), batchNumber);
			return ((ISingleResult<spHasReferenceTagsResult>)(result.ReturnValue));
		}
		
		[global::System.Data.Linq.Mapping.FunctionAttribute(Name="dbo.spBurningAttemptsCount")]
		public ISingleResult<spBurningAttemptsCountResult> spBurningAttemptsCount([global::System.Data.Linq.Mapping.ParameterAttribute(DbType="NChar(10)")] string batchNumber)
		{
			IExecuteResult result = this.ExecuteMethodCall(this, ((MethodInfo)(MethodInfo.GetCurrentMethod())), batchNumber);
			return ((ISingleResult<spBurningAttemptsCountResult>)(result.ReturnValue));
		}
		
		[global::System.Data.Linq.Mapping.FunctionAttribute(Name="dbo.spFailuresCount")]
		public ISingleResult<spFailuresCountResult> spFailuresCount([global::System.Data.Linq.Mapping.ParameterAttribute(DbType="NChar(10)")] string batchNumber)
		{
			IExecuteResult result = this.ExecuteMethodCall(this, ((MethodInfo)(MethodInfo.GetCurrentMethod())), batchNumber);
			return ((ISingleResult<spFailuresCountResult>)(result.ReturnValue));
		}
	}
	
	[global::System.Data.Linq.Mapping.TableAttribute(Name="dbo.Batches")]
	public partial class Batch : INotifyPropertyChanging, INotifyPropertyChanged
	{
		
		private static PropertyChangingEventArgs emptyChangingEventArgs = new PropertyChangingEventArgs(String.Empty);
		
		private string _BatchNumber;
		
		private short _IdtVersion;
		
		private short _MaterialID;
		
		private byte _MaterialKind;
		
		private System.DateTime _ManufacturingDate;
		
		private System.DateTime _ExpirationDate;
		
		private short _InitialWeight;
		
		private System.Nullable<short> _ExtraWeight;
		
		private System.Nullable<System.DateTime> _BurnStartTime;
		
		private System.Nullable<System.DateTime> _BurnEndTime;
		
		private System.Data.Linq.Binary _Signature;
		
		private EntitySet<BurningAction> _BurningActions;
		
		private EntityRef<Material> _Material;
		
    #region Extensibility Method Definitions
    partial void OnLoaded();
    partial void OnValidate(System.Data.Linq.ChangeAction action);
    partial void OnCreated();
    partial void OnBatchNumberChanging(string value);
    partial void OnBatchNumberChanged();
    partial void OnIdtVersionChanging(short value);
    partial void OnIdtVersionChanged();
    partial void OnMaterialIDChanging(short value);
    partial void OnMaterialIDChanged();
    partial void OnMaterialKindChanging(byte value);
    partial void OnMaterialKindChanged();
    partial void OnManufacturingDateChanging(System.DateTime value);
    partial void OnManufacturingDateChanged();
    partial void OnExpirationDateChanging(System.DateTime value);
    partial void OnExpirationDateChanged();
    partial void OnInitialWeightChanging(short value);
    partial void OnInitialWeightChanged();
    partial void OnExtraWeightChanging(System.Nullable<short> value);
    partial void OnExtraWeightChanged();
    partial void OnBurnStartTimeChanging(System.Nullable<System.DateTime> value);
    partial void OnBurnStartTimeChanged();
    partial void OnBurnEndTimeChanging(System.Nullable<System.DateTime> value);
    partial void OnBurnEndTimeChanged();
    partial void OnSignatureChanging(System.Data.Linq.Binary value);
    partial void OnSignatureChanged();
    #endregion
		
		public Batch()
		{
			this._BurningActions = new EntitySet<BurningAction>(new Action<BurningAction>(this.attach_BurningActions), new Action<BurningAction>(this.detach_BurningActions));
			this._Material = default(EntityRef<Material>);
			OnCreated();
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_BatchNumber", DbType="NVarChar(10) NOT NULL", CanBeNull=false, IsPrimaryKey=true)]
		public string BatchNumber
		{
			get
			{
				return this._BatchNumber;
			}
			set
			{
				if ((this._BatchNumber != value))
				{
					this.OnBatchNumberChanging(value);
					this.SendPropertyChanging();
					this._BatchNumber = value;
					this.SendPropertyChanged("BatchNumber");
					this.OnBatchNumberChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_IdtVersion", DbType="SmallInt NOT NULL")]
		public short IdtVersion
		{
			get
			{
				return this._IdtVersion;
			}
			set
			{
				if ((this._IdtVersion != value))
				{
					this.OnIdtVersionChanging(value);
					this.SendPropertyChanging();
					this._IdtVersion = value;
					this.SendPropertyChanged("IdtVersion");
					this.OnIdtVersionChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_MaterialID", DbType="SmallInt NOT NULL")]
		public short MaterialID
		{
			get
			{
				return this._MaterialID;
			}
			set
			{
				if ((this._MaterialID != value))
				{
					if (this._Material.HasLoadedOrAssignedValue)
					{
						throw new System.Data.Linq.ForeignKeyReferenceAlreadyHasValueException();
					}
					this.OnMaterialIDChanging(value);
					this.SendPropertyChanging();
					this._MaterialID = value;
					this.SendPropertyChanged("MaterialID");
					this.OnMaterialIDChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_MaterialKind", DbType="TinyInt NOT NULL")]
		public byte MaterialKind
		{
			get
			{
				return this._MaterialKind;
			}
			set
			{
				if ((this._MaterialKind != value))
				{
					this.OnMaterialKindChanging(value);
					this.SendPropertyChanging();
					this._MaterialKind = value;
					this.SendPropertyChanged("MaterialKind");
					this.OnMaterialKindChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_ManufacturingDate", DbType="Date NOT NULL")]
		public System.DateTime ManufacturingDate
		{
			get
			{
				return this._ManufacturingDate;
			}
			set
			{
				if ((this._ManufacturingDate != value))
				{
					this.OnManufacturingDateChanging(value);
					this.SendPropertyChanging();
					this._ManufacturingDate = value;
					this.SendPropertyChanged("ManufacturingDate");
					this.OnManufacturingDateChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_ExpirationDate", DbType="Date NOT NULL")]
		public System.DateTime ExpirationDate
		{
			get
			{
				return this._ExpirationDate;
			}
			set
			{
				if ((this._ExpirationDate != value))
				{
					this.OnExpirationDateChanging(value);
					this.SendPropertyChanging();
					this._ExpirationDate = value;
					this.SendPropertyChanged("ExpirationDate");
					this.OnExpirationDateChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_InitialWeight", DbType="SmallInt NOT NULL")]
		public short InitialWeight
		{
			get
			{
				return this._InitialWeight;
			}
			set
			{
				if ((this._InitialWeight != value))
				{
					this.OnInitialWeightChanging(value);
					this.SendPropertyChanging();
					this._InitialWeight = value;
					this.SendPropertyChanged("InitialWeight");
					this.OnInitialWeightChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_ExtraWeight", DbType="SmallInt")]
		public System.Nullable<short> ExtraWeight
		{
			get
			{
				return this._ExtraWeight;
			}
			set
			{
				if ((this._ExtraWeight != value))
				{
					this.OnExtraWeightChanging(value);
					this.SendPropertyChanging();
					this._ExtraWeight = value;
					this.SendPropertyChanged("ExtraWeight");
					this.OnExtraWeightChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_BurnStartTime", DbType="DateTime")]
		public System.Nullable<System.DateTime> BurnStartTime
		{
			get
			{
				return this._BurnStartTime;
			}
			set
			{
				if ((this._BurnStartTime != value))
				{
					this.OnBurnStartTimeChanging(value);
					this.SendPropertyChanging();
					this._BurnStartTime = value;
					this.SendPropertyChanged("BurnStartTime");
					this.OnBurnStartTimeChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_BurnEndTime", DbType="DateTime")]
		public System.Nullable<System.DateTime> BurnEndTime
		{
			get
			{
				return this._BurnEndTime;
			}
			set
			{
				if ((this._BurnEndTime != value))
				{
					this.OnBurnEndTimeChanging(value);
					this.SendPropertyChanging();
					this._BurnEndTime = value;
					this.SendPropertyChanged("BurnEndTime");
					this.OnBurnEndTimeChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_Signature", DbType="VarBinary(MAX) NOT NULL", CanBeNull=false, UpdateCheck=UpdateCheck.Never)]
		public System.Data.Linq.Binary Signature
		{
			get
			{
				return this._Signature;
			}
			set
			{
				if ((this._Signature != value))
				{
					this.OnSignatureChanging(value);
					this.SendPropertyChanging();
					this._Signature = value;
					this.SendPropertyChanged("Signature");
					this.OnSignatureChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.AssociationAttribute(Name="Batch_BurningAction", Storage="_BurningActions", ThisKey="BatchNumber", OtherKey="BatchNumber")]
		public EntitySet<BurningAction> BurningActions
		{
			get
			{
				return this._BurningActions;
			}
			set
			{
				this._BurningActions.Assign(value);
			}
		}
		
		[global::System.Data.Linq.Mapping.AssociationAttribute(Name="Material_Batch", Storage="_Material", ThisKey="MaterialID", OtherKey="ID", IsForeignKey=true)]
		public Material Material
		{
			get
			{
				return this._Material.Entity;
			}
			set
			{
				Material previousValue = this._Material.Entity;
				if (((previousValue != value) 
							|| (this._Material.HasLoadedOrAssignedValue == false)))
				{
					this.SendPropertyChanging();
					if ((previousValue != null))
					{
						this._Material.Entity = null;
						previousValue.Batches.Remove(this);
					}
					this._Material.Entity = value;
					if ((value != null))
					{
						value.Batches.Add(this);
						this._MaterialID = value.ID;
					}
					else
					{
						this._MaterialID = default(short);
					}
					this.SendPropertyChanged("Material");
				}
			}
		}
		
		public event PropertyChangingEventHandler PropertyChanging;
		
		public event PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			if ((this.PropertyChanging != null))
			{
				this.PropertyChanging(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(String propertyName)
		{
			if ((this.PropertyChanged != null))
			{
				this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
		
		private void attach_BurningActions(BurningAction entity)
		{
			this.SendPropertyChanging();
			entity.Batch = this;
		}
		
		private void detach_BurningActions(BurningAction entity)
		{
			this.SendPropertyChanging();
			entity.Batch = null;
		}
	}
	
	[global::System.Data.Linq.Mapping.TableAttribute(Name="dbo.Tags")]
	public partial class Tag : INotifyPropertyChanging, INotifyPropertyChanged
	{
		
		private static PropertyChangingEventArgs emptyChangingEventArgs = new PropertyChangingEventArgs(String.Empty);
		
		private System.Data.Linq.Binary _SerialNumber;
		
		private System.DateTime _Date;
		
		private System.Data.Linq.Binary _PublicKey;
		
		private byte _Status;
		
		private System.Nullable<System.DateTime> _LastUpdateTime;
		
		private System.Data.Linq.Binary _Signature;
		
		private EntitySet<BurningAction> _BurningActions;
		
    #region Extensibility Method Definitions
    partial void OnLoaded();
    partial void OnValidate(System.Data.Linq.ChangeAction action);
    partial void OnCreated();
    partial void OnSerialNumberChanging(System.Data.Linq.Binary value);
    partial void OnSerialNumberChanged();
    partial void OnDateChanging(System.DateTime value);
    partial void OnDateChanged();
    partial void OnPublicKeyChanging(System.Data.Linq.Binary value);
    partial void OnPublicKeyChanged();
    partial void OnStatusChanging(byte value);
    partial void OnStatusChanged();
    partial void OnLastUpdateTimeChanging(System.Nullable<System.DateTime> value);
    partial void OnLastUpdateTimeChanged();
    partial void OnSignatureChanging(System.Data.Linq.Binary value);
    partial void OnSignatureChanged();
    #endregion
		
		public Tag()
		{
			this._BurningActions = new EntitySet<BurningAction>(new Action<BurningAction>(this.attach_BurningActions), new Action<BurningAction>(this.detach_BurningActions));
			OnCreated();
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_SerialNumber", DbType="Binary(8) NOT NULL", CanBeNull=false, IsPrimaryKey=true, UpdateCheck=UpdateCheck.Never)]
		public System.Data.Linq.Binary SerialNumber
		{
			get
			{
				return this._SerialNumber;
			}
			set
			{
				if ((this._SerialNumber != value))
				{
					this.OnSerialNumberChanging(value);
					this.SendPropertyChanging();
					this._SerialNumber = value;
					this.SendPropertyChanged("SerialNumber");
					this.OnSerialNumberChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_Date", DbType="Date NOT NULL")]
		public System.DateTime Date
		{
			get
			{
				return this._Date;
			}
			set
			{
				if ((this._Date != value))
				{
					this.OnDateChanging(value);
					this.SendPropertyChanging();
					this._Date = value;
					this.SendPropertyChanged("Date");
					this.OnDateChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_PublicKey", DbType="VarBinary(MAX) NOT NULL", CanBeNull=false, UpdateCheck=UpdateCheck.Never)]
		public System.Data.Linq.Binary PublicKey
		{
			get
			{
				return this._PublicKey;
			}
			set
			{
				if ((this._PublicKey != value))
				{
					this.OnPublicKeyChanging(value);
					this.SendPropertyChanging();
					this._PublicKey = value;
					this.SendPropertyChanged("PublicKey");
					this.OnPublicKeyChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_Status", DbType="TinyInt NOT NULL")]
		public byte Status
		{
			get
			{
				return this._Status;
			}
			set
			{
				if ((this._Status != value))
				{
					this.OnStatusChanging(value);
					this.SendPropertyChanging();
					this._Status = value;
					this.SendPropertyChanged("Status");
					this.OnStatusChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_LastUpdateTime", DbType="DateTime")]
		public System.Nullable<System.DateTime> LastUpdateTime
		{
			get
			{
				return this._LastUpdateTime;
			}
			set
			{
				if ((this._LastUpdateTime != value))
				{
					this.OnLastUpdateTimeChanging(value);
					this.SendPropertyChanging();
					this._LastUpdateTime = value;
					this.SendPropertyChanged("LastUpdateTime");
					this.OnLastUpdateTimeChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_Signature", DbType="VarBinary(MAX) NOT NULL", CanBeNull=false, UpdateCheck=UpdateCheck.Never)]
		public System.Data.Linq.Binary Signature
		{
			get
			{
				return this._Signature;
			}
			set
			{
				if ((this._Signature != value))
				{
					this.OnSignatureChanging(value);
					this.SendPropertyChanging();
					this._Signature = value;
					this.SendPropertyChanged("Signature");
					this.OnSignatureChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.AssociationAttribute(Name="Tag_BurningAction", Storage="_BurningActions", ThisKey="SerialNumber", OtherKey="SerialNumber")]
		public EntitySet<BurningAction> BurningActions
		{
			get
			{
				return this._BurningActions;
			}
			set
			{
				this._BurningActions.Assign(value);
			}
		}
		
		public event PropertyChangingEventHandler PropertyChanging;
		
		public event PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			if ((this.PropertyChanging != null))
			{
				this.PropertyChanging(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(String propertyName)
		{
			if ((this.PropertyChanged != null))
			{
				this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
		
		private void attach_BurningActions(BurningAction entity)
		{
			this.SendPropertyChanging();
			entity.Tag = this;
		}
		
		private void detach_BurningActions(BurningAction entity)
		{
			this.SendPropertyChanging();
			entity.Tag = null;
		}
	}
	
	[global::System.Data.Linq.Mapping.TableAttribute(Name="dbo.BurningActions")]
	public partial class BurningAction : INotifyPropertyChanging, INotifyPropertyChanged
	{
		
		private static PropertyChangingEventArgs emptyChangingEventArgs = new PropertyChangingEventArgs(String.Empty);
		
		private int _Id;
		
		private System.Data.Linq.Binary _SerialNumber;
		
		private string _BatchNumber;
		
		private string _StationName;
		
		private System.DateTime _BurningTime;
		
		private bool _IsReference;
		
		private string _ErrorMessage;
		
		private System.Data.Linq.Binary _Signature;
		
		private EntityRef<Batch> _Batch;
		
		private EntityRef<Tag> _Tag;
		
    #region Extensibility Method Definitions
    partial void OnLoaded();
    partial void OnValidate(System.Data.Linq.ChangeAction action);
    partial void OnCreated();
    partial void OnIdChanging(int value);
    partial void OnIdChanged();
    partial void OnSerialNumberChanging(System.Data.Linq.Binary value);
    partial void OnSerialNumberChanged();
    partial void OnBatchNumberChanging(string value);
    partial void OnBatchNumberChanged();
    partial void OnStationNameChanging(string value);
    partial void OnStationNameChanged();
    partial void OnBurningTimeChanging(System.DateTime value);
    partial void OnBurningTimeChanged();
    partial void OnIsReferenceChanging(bool value);
    partial void OnIsReferenceChanged();
    partial void OnErrorMessageChanging(string value);
    partial void OnErrorMessageChanged();
    partial void OnSignatureChanging(System.Data.Linq.Binary value);
    partial void OnSignatureChanged();
    #endregion
		
		public BurningAction()
		{
			this._Batch = default(EntityRef<Batch>);
			this._Tag = default(EntityRef<Tag>);
			OnCreated();
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_Id", DbType="Int NOT NULL", IsPrimaryKey=true)]
		public int Id
		{
			get
			{
				return this._Id;
			}
			set
			{
				if ((this._Id != value))
				{
					this.OnIdChanging(value);
					this.SendPropertyChanging();
					this._Id = value;
					this.SendPropertyChanged("Id");
					this.OnIdChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_SerialNumber", DbType="Binary(8) NOT NULL", CanBeNull=false, UpdateCheck=UpdateCheck.Never)]
		public System.Data.Linq.Binary SerialNumber
		{
			get
			{
				return this._SerialNumber;
			}
			set
			{
				if ((this._SerialNumber != value))
				{
					if (this._Tag.HasLoadedOrAssignedValue)
					{
						throw new System.Data.Linq.ForeignKeyReferenceAlreadyHasValueException();
					}
					this.OnSerialNumberChanging(value);
					this.SendPropertyChanging();
					this._SerialNumber = value;
					this.SendPropertyChanged("SerialNumber");
					this.OnSerialNumberChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_BatchNumber", DbType="NVarChar(10) NOT NULL", CanBeNull=false)]
		public string BatchNumber
		{
			get
			{
				return this._BatchNumber;
			}
			set
			{
				if ((this._BatchNumber != value))
				{
					if (this._Batch.HasLoadedOrAssignedValue)
					{
						throw new System.Data.Linq.ForeignKeyReferenceAlreadyHasValueException();
					}
					this.OnBatchNumberChanging(value);
					this.SendPropertyChanging();
					this._BatchNumber = value;
					this.SendPropertyChanged("BatchNumber");
					this.OnBatchNumberChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_StationName", DbType="NVarChar(32) NOT NULL", CanBeNull=false)]
		public string StationName
		{
			get
			{
				return this._StationName;
			}
			set
			{
				if ((this._StationName != value))
				{
					this.OnStationNameChanging(value);
					this.SendPropertyChanging();
					this._StationName = value;
					this.SendPropertyChanged("StationName");
					this.OnStationNameChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_BurningTime", DbType="DateTime NOT NULL")]
		public System.DateTime BurningTime
		{
			get
			{
				return this._BurningTime;
			}
			set
			{
				if ((this._BurningTime != value))
				{
					this.OnBurningTimeChanging(value);
					this.SendPropertyChanging();
					this._BurningTime = value;
					this.SendPropertyChanged("BurningTime");
					this.OnBurningTimeChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_IsReference", DbType="Bit NOT NULL")]
		public bool IsReference
		{
			get
			{
				return this._IsReference;
			}
			set
			{
				if ((this._IsReference != value))
				{
					this.OnIsReferenceChanging(value);
					this.SendPropertyChanging();
					this._IsReference = value;
					this.SendPropertyChanged("IsReference");
					this.OnIsReferenceChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_ErrorMessage", DbType="NVarChar(50)")]
		public string ErrorMessage
		{
			get
			{
				return this._ErrorMessage;
			}
			set
			{
				if ((this._ErrorMessage != value))
				{
					this.OnErrorMessageChanging(value);
					this.SendPropertyChanging();
					this._ErrorMessage = value;
					this.SendPropertyChanged("ErrorMessage");
					this.OnErrorMessageChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_Signature", DbType="VarBinary(MAX) NOT NULL", CanBeNull=false, UpdateCheck=UpdateCheck.Never)]
		public System.Data.Linq.Binary Signature
		{
			get
			{
				return this._Signature;
			}
			set
			{
				if ((this._Signature != value))
				{
					this.OnSignatureChanging(value);
					this.SendPropertyChanging();
					this._Signature = value;
					this.SendPropertyChanged("Signature");
					this.OnSignatureChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.AssociationAttribute(Name="Batch_BurningAction", Storage="_Batch", ThisKey="BatchNumber", OtherKey="BatchNumber", IsForeignKey=true, DeleteOnNull=true, DeleteRule="CASCADE")]
		public Batch Batch
		{
			get
			{
				return this._Batch.Entity;
			}
			set
			{
				Batch previousValue = this._Batch.Entity;
				if (((previousValue != value) 
							|| (this._Batch.HasLoadedOrAssignedValue == false)))
				{
					this.SendPropertyChanging();
					if ((previousValue != null))
					{
						this._Batch.Entity = null;
						previousValue.BurningActions.Remove(this);
					}
					this._Batch.Entity = value;
					if ((value != null))
					{
						value.BurningActions.Add(this);
						this._BatchNumber = value.BatchNumber;
					}
					else
					{
						this._BatchNumber = default(string);
					}
					this.SendPropertyChanged("Batch");
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.AssociationAttribute(Name="Tag_BurningAction", Storage="_Tag", ThisKey="SerialNumber", OtherKey="SerialNumber", IsForeignKey=true, DeleteOnNull=true, DeleteRule="CASCADE")]
		public Tag Tag
		{
			get
			{
				return this._Tag.Entity;
			}
			set
			{
				Tag previousValue = this._Tag.Entity;
				if (((previousValue != value) 
							|| (this._Tag.HasLoadedOrAssignedValue == false)))
				{
					this.SendPropertyChanging();
					if ((previousValue != null))
					{
						this._Tag.Entity = null;
						previousValue.BurningActions.Remove(this);
					}
					this._Tag.Entity = value;
					if ((value != null))
					{
						value.BurningActions.Add(this);
						this._SerialNumber = value.SerialNumber;
					}
					else
					{
						this._SerialNumber = default(System.Data.Linq.Binary);
					}
					this.SendPropertyChanged("Tag");
				}
			}
		}
		
		public event PropertyChangingEventHandler PropertyChanging;
		
		public event PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			if ((this.PropertyChanging != null))
			{
				this.PropertyChanging(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(String propertyName)
		{
			if ((this.PropertyChanged != null))
			{
				this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
	}
	
	[global::System.Data.Linq.Mapping.TableAttribute(Name="dbo.Materials")]
	public partial class Material : INotifyPropertyChanging, INotifyPropertyChanged
	{
		
		private static PropertyChangingEventArgs emptyChangingEventArgs = new PropertyChangingEventArgs(String.Empty);
		
		private short _ID;
		
		private string _Name;
		
		private EntitySet<Batch> _Batches;
		
    #region Extensibility Method Definitions
    partial void OnLoaded();
    partial void OnValidate(System.Data.Linq.ChangeAction action);
    partial void OnCreated();
    partial void OnIDChanging(short value);
    partial void OnIDChanged();
    partial void OnNameChanging(string value);
    partial void OnNameChanged();
    #endregion
		
		public Material()
		{
			this._Batches = new EntitySet<Batch>(new Action<Batch>(this.attach_Batches), new Action<Batch>(this.detach_Batches));
			OnCreated();
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_ID", DbType="SmallInt NOT NULL", IsPrimaryKey=true)]
		public short ID
		{
			get
			{
				return this._ID;
			}
			set
			{
				if ((this._ID != value))
				{
					this.OnIDChanging(value);
					this.SendPropertyChanging();
					this._ID = value;
					this.SendPropertyChanged("ID");
					this.OnIDChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Storage="_Name", DbType="NVarChar(32) NOT NULL", CanBeNull=false)]
		public string Name
		{
			get
			{
				return this._Name;
			}
			set
			{
				if ((this._Name != value))
				{
					this.OnNameChanging(value);
					this.SendPropertyChanging();
					this._Name = value;
					this.SendPropertyChanged("Name");
					this.OnNameChanged();
				}
			}
		}
		
		[global::System.Data.Linq.Mapping.AssociationAttribute(Name="Material_Batch", Storage="_Batches", ThisKey="ID", OtherKey="MaterialID")]
		public EntitySet<Batch> Batches
		{
			get
			{
				return this._Batches;
			}
			set
			{
				this._Batches.Assign(value);
			}
		}
		
		public event PropertyChangingEventHandler PropertyChanging;
		
		public event PropertyChangedEventHandler PropertyChanged;
		
		protected virtual void SendPropertyChanging()
		{
			if ((this.PropertyChanging != null))
			{
				this.PropertyChanging(this, emptyChangingEventArgs);
			}
		}
		
		protected virtual void SendPropertyChanged(String propertyName)
		{
			if ((this.PropertyChanged != null))
			{
				this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
			}
		}
		
		private void attach_Batches(Batch entity)
		{
			this.SendPropertyChanging();
			entity.Material = this;
		}
		
		private void detach_Batches(Batch entity)
		{
			this.SendPropertyChanging();
			entity.Material = null;
		}
	}
	
	public partial class spBurnedTagsCountResult
	{
		
		private System.Nullable<int> _Column1;
		
		public spBurnedTagsCountResult()
		{
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Name="", Storage="_Column1", DbType="Int")]
		public System.Nullable<int> Column1
		{
			get
			{
				return this._Column1;
			}
			set
			{
				if ((this._Column1 != value))
				{
					this._Column1 = value;
				}
			}
		}
	}
	
	public partial class spHasReferenceTagsResult
	{
		
		private System.Nullable<bool> _Column1;
		
		public spHasReferenceTagsResult()
		{
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Name="", Storage="_Column1", DbType="Bit")]
		public System.Nullable<bool> Column1
		{
			get
			{
				return this._Column1;
			}
			set
			{
				if ((this._Column1 != value))
				{
					this._Column1 = value;
				}
			}
		}
	}
	
	public partial class spBurningAttemptsCountResult
	{
		
		private System.Nullable<int> _Column1;
		
		public spBurningAttemptsCountResult()
		{
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Name="", Storage="_Column1", DbType="Int")]
		public System.Nullable<int> Column1
		{
			get
			{
				return this._Column1;
			}
			set
			{
				if ((this._Column1 != value))
				{
					this._Column1 = value;
				}
			}
		}
	}
	
	public partial class spFailuresCountResult
	{
		
		private System.Nullable<int> _Column1;
		
		public spFailuresCountResult()
		{
		}
		
		[global::System.Data.Linq.Mapping.ColumnAttribute(Name="", Storage="_Column1", DbType="Int")]
		public System.Nullable<int> Column1
		{
			get
			{
				return this._Column1;
			}
			set
			{
				if ((this._Column1 != value))
				{
					this._Column1 = value;
				}
			}
		}
	}
}
#pragma warning restore 1591
