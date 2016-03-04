using BSS.Contracts.Properties;
using System;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;

namespace BSS.Contracts
{
    public static class DateTimeExtension
    {
        #region Public Methods

        /// <summary>
        /// Converts a <see cref="System.DateTime"/> object to unix time.
        /// </summary>
        /// <param name="time">The <see cref="System.DateTime"/> object.</param>
        /// <returns>The seconds elapsed since 01-Jan-1970.</returns>
        public static uint ToUnixTime(this DateTime time)
        {
            return Convert.ToUInt32((time - new DateTime(1970, 1, 1)).TotalSeconds);
        }

        #endregion Public Methods
    }

    /// <summary>
    /// Holds properties defining material information.
    /// </summary>
    [DataContract]
    public class MaterialInfo : InternalObservableObject, IEquatable<MaterialInfo>, ICloneable, IDataErrorInfo, IExtensibleDataObject
    {
        #region Private Fields

        /// <summary>
        /// The batch number length.
        /// </summary>
        private static readonly byte BatchNumberLength = 10;

        /// <summary>
        /// The batch number
        /// </summary>
        private string batchNumber;

        /// <summary>
        /// The expiration date.
        /// </summary>
        private DateTime expirationDate;

        /// <summary>
        /// The extra weight.
        /// </summary>
        private ushort extraWeight;

        /// <summary>
        /// The initial weight.
        /// </summary>
        private ushort initialWeight;

        /// <summary>
        /// The manufacturing time.
        /// </summary>
        private DateTime manufacturingTime;

        /// <summary>
        /// The material type.
        /// </summary>
        private ushort materialID;

        /// <summary>
        /// The material kind.
        /// </summary>
        private MaterialKind materialKind;

        /// <summary>
        /// The material name.
        /// </summary>
        private string materialName;

        /// <summary>
        /// The tag structure identifier.
        /// </summary>
        private ushort tagStructID;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="MaterialInfo"/> class.
        /// </summary>
        public MaterialInfo()
        {
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the batch number.
        /// </summary>
        /// <value>
        /// The batch number.
        /// </value>
        [DataMember(Order = 5)]
        public string BatchNumber
        {
            get
            {
                return batchNumber;
            }
            set
            {
                batchNumber = value;
                RaisePropertyChanged(() => BatchNumber);
            }
        }

        /// <summary>
        /// Gets or sets the expiration date.
        /// </summary>
        /// <value>
        /// The expiration date.
        /// </value>
        [DataMember(Order = 8)]
        public DateTime ExpirationDate
        {
            get
            {
                return expirationDate;
            }
            set
            {
                expirationDate = value;
                RaisePropertyChanged(() => ExpirationDate);
            }
        }

        /// <summary>
        /// Gets or sets the extra weight in grams.
        /// </summary>
        /// <value>
        /// The extra weight.
        /// </value>
        [DataMember(Order = 9)]
        public ushort ExtraWeight
        {
            get
            {
                return extraWeight;
            }
            set
            {
                extraWeight = value;
                RaisePropertyChanged(() => ExtraWeight);
            }
        }

        /// <summary>
        /// Gets or sets the initial weight in grams.
        /// </summary>
        /// <value>
        /// The initial weight.
        /// </value>
        [DataMember(Order = 7)]
        public ushort InitialWeight
        {
            get
            {
                return initialWeight;
            }
            set
            {
                initialWeight = value;
                RaisePropertyChanged(() => InitialWeight);
            }
        }

        /// <summary>
        /// Gets or sets the manufacturing time.
        /// </summary>
        /// <value>
        /// The manufacturing time.
        /// </value>
        [DataMember(Order = 6)]
        public DateTime ManufacturingTime
        {
            get
            {
                return manufacturingTime;
            }
            set
            {
                manufacturingTime = value;
                RaisePropertyChanged(() => ManufacturingTime);
            }
        }

        /// <summary>
        /// Gets or sets the type of the material.
        /// </summary>
        /// <value>
        /// The type of the material.
        /// </value>
        [DataMember(Order = 2)]
        public ushort MaterialID
        {
            get
            {
                return materialID;
            }
            set
            {
                materialID = value;
                RaisePropertyChanged(() => MaterialID);
            }
        }

        /// <summary>
        /// Gets or sets the kind of the material.
        /// </summary>
        /// <value>
        /// The kind of the material.
        /// </value>
        [DataMember(Order = 4)]
        public MaterialKind MaterialKind
        {
            get
            {
                return materialKind;
            }
            set
            {
                materialKind = value;
                RaisePropertyChanged(() => MaterialKind);
            }
        }

        /// <summary>
        /// Gets or sets the name of the material.
        /// </summary>
        /// <value>
        /// The name of the material.
        /// </value>
        [DataMember(Order = 3)]
        public string MaterialName
        {
            get
            {
                return materialName;
            }
            set
            {
                materialName = value;
                RaisePropertyChanged(() => MaterialName);
            }
        }

        /// <summary>
        /// Gets or sets the tag structure identifier.
        /// </summary>
        /// <value>
        /// The tag structure identifier.
        /// </value>
        [DataMember(Order = 1)]
        public ushort TagStructID
        {
            get
            {
                return tagStructID;
            }
            set
            {
                tagStructID = value;
                RaisePropertyChanged(() => TagStructID);
            }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Deserializes instance from a binary buffer.
        /// </summary>
        /// <returns></returns>
        /// <param name="buffer">The buffer.</param>
        public int Decode(byte[] buffer)
        {
            using (BinaryReader br = new BinaryReader(new MemoryStream(buffer)))
            {
                TagStructID = br.ReadUInt16();
                MaterialID = br.ReadUInt16();
                // material kind and material name are not decoded
                ManufacturingTime = new DateTime(1970, 1, 1).AddSeconds(br.ReadUInt32());
                byte[] batchNumberBytes = br.ReadBytes(BatchNumberLength);
                BatchNumber = Encoding.ASCII.GetString(batchNumberBytes).TrimEnd('\0');
                InitialWeight = br.ReadUInt16();
                ExpirationDate = new DateTime(1970, 1, 1).AddSeconds(br.ReadUInt32());
                ExtraWeight = br.ReadUInt16();

                return (int)br.BaseStream.Position;
            }
        }

        /// <summary>
        /// Serializes instance to a binary buffer.
        /// </summary>
        /// <returns>The buffer.</returns>
        public byte[] Encode()
        {
            using (BinaryWriter bw = new BinaryWriter(new MemoryStream()))
            {
                bw.Write(TagStructID);
                bw.Write((ushort)MaterialID);
                // material kind and material name are not encoded
                bw.Write(ManufacturingTime.ToUnixTime());

                byte[] batchNumberBytes = (BatchNumber == null)
                    ? new byte[0]
                    : Encoding.ASCII.GetBytes(BatchNumber);
                Array.Resize(ref batchNumberBytes, BatchNumberLength);
                bw.Write(batchNumberBytes);

                bw.Write(InitialWeight);
                bw.Write(ExpirationDate.ToUnixTime());
                bw.Write(ExtraWeight);

                return ((MemoryStream)bw.BaseStream).ToArray();
            }
        }

        /// <summary>
        /// Gets the total weight in milligrams.
        /// </summary>
        /// <returns></returns>
        public uint GetTotalWeight()
        {
            return Convert.ToUInt32(1000 * InitialWeight + ExtraWeight);
        }

        #endregion Public Methods

        #region IDataErrorInfo Members

        /// <summary>
        /// Gets an error message indicating what is wrong with this object.
        /// </summary>
        public string Error
        {
            get
            {
                return this.GetType().GetProperties()
                    .Select(pi => this[pi.Name])
                    .FirstOrDefault(err => err != null);
            }
        }

        /// <summary>
        /// Gets the error message for the property with the given name.
        /// </summary>
        /// <param name="columnName">Name of the column.</param>
        /// <returns></returns>
        public string this[string columnName]
        {
            get
            {
                if (columnName == "ExpirationDate")
                {
                    if (ExpirationDate.Date < DateTime.Now.Date)
                    {
                        return Resources.MaterialExpired;
                    }
                }

                return null;
            }
        }

        #endregion IDataErrorInfo Members

        #region IEquatable Members

        /// <summary>
        /// Determines whether two specified instances of <see cref="T:BSS.Contracts.MaterialInfo"/> are inequal.
        /// </summary>
        /// <param name="x">The first object to compare.</param>
        /// <param name="y">The second object to compare.</param>
        /// <returns>
        /// <c>true</c> if x and y represent a different material information; otherwise, <c>false.</c>
        /// </returns>
        public static bool operator !=(MaterialInfo x, MaterialInfo y)
        {
            return !(x == y);
        }

        /// <summary>
        /// Determines whether two specified instances of <see cref="T:BSS.Contracts.MaterialInfo"/> are equal.
        /// </summary>
        /// <param name="x">The first object to compare.</param>
        /// <param name="y">The second object to compare.</param>
        /// <returns>
        /// <c>true</c> if x and y represent the same material information; otherwise, <c>false</c>.
        /// </returns>
        public static bool operator ==(MaterialInfo x, MaterialInfo y)
        {
            return
                 Object.ReferenceEquals(x, null) && Object.ReferenceEquals(y, null) ||
                !Object.ReferenceEquals(x, null) && !Object.ReferenceEquals(y, null) && x.Equals(y);
        }

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        /// <param name="other">An object to compare with this object.</param>
        /// <returns>
        /// true if the current object is equal to the <paramref name="other" /> parameter; otherwise, false.
        /// </returns>
        public bool Equals(MaterialInfo other)
        {
            return other != null &&
                expirationDate.Equals(other.expirationDate) &&
                extraWeight.Equals(other.extraWeight) &&
                initialWeight.Equals(initialWeight) &&
                manufacturingTime.Equals(other.manufacturingTime) &&
                ((batchNumber == null) ? (other.batchNumber == null) : batchNumber.Equals(other.batchNumber)) &&
                materialID.Equals(other.materialID) &&
                materialKind.Equals(other.materialKind) &&
                ((materialName == null) ? (other.materialName == null) : materialName.Equals(other.materialName)) &&
                tagStructID.Equals(tagStructID);
        }

        /// <summary>
        /// Indicates whether the current object is equal to another object of the same type.
        /// </summary>
        /// <param name="other">An object to compare with this object.</param>
        /// <returns>
        /// true if the current object is equal to the <paramref name="other" /> parameter; otherwise, false.
        /// </returns>
        public override bool Equals(object other)
        {
            return Equals(other as MaterialInfo);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
        /// </returns>
        public override int GetHashCode()
        {
            int batchNumberHashCode = 0;
            if (batchNumber != null)
            {
                batchNumberHashCode = batchNumber.GetHashCode();
            }

            int materialNameHashCode = 0;
            if (materialName != null)
            {
                materialNameHashCode = materialName.GetHashCode();
            }

            return expirationDate.GetHashCode() ^
                extraWeight.GetHashCode() ^
                initialWeight.GetHashCode() ^
                manufacturingTime.GetHashCode() ^
                batchNumberHashCode ^
                materialID.GetHashCode() ^
                materialKind.GetHashCode() ^
                materialName.GetHashCode() ^
                tagStructID.GetHashCode();
        }

        #endregion IEquatable Members

        #region IClonable Members

        /// <summary>
        /// Creates a new object that is a copy of the current instance.
        /// </summary>
        /// <returns>
        /// A new object that is a copy of this instance.
        /// </returns>
        public object Clone()
        {
            MaterialInfo cloned = new MaterialInfo();
            cloned.tagStructID = tagStructID;
            cloned.materialID = materialID;
            cloned.materialName = materialName;
            cloned.materialKind = materialKind;
            cloned.manufacturingTime = manufacturingTime;
            cloned.batchNumber = batchNumber;
            cloned.initialWeight = initialWeight;
            cloned.expirationDate = expirationDate;
            return cloned;
        }

        #endregion IClonable Members

        #region IExtensibleDataObject Members

        /// <summary>
        /// Gets or sets the structure that contains extra data.
        /// </summary>
        public ExtensionDataObject ExtensionData
        {
            get;
            set;
        }

        #endregion IExtensibleDataObject Members
    }
}