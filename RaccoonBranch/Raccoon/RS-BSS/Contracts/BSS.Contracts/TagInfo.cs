using BSS.Contracts.Properties;
using System;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.Serialization;

namespace BSS.Contracts
{
    public static class SerialNumberExtension
    {
        #region Public Methods

        /// <summary>
        /// Converts hexadecimal string to a byte array.
        /// </summary>
        /// <param name="hexString">The hexadecimal string.</param>
        /// <returns></returns>
        /// <exception cref="System.ArgumentNullException">hexString</exception>
        /// <exception cref="System.ArgumentException">hexString length is not even.</exception>
        public static byte[] ToByteArray(this string hexString)
        {
            if (hexString == null)
            {
                throw new ArgumentNullException("hexString");
            }

            if (hexString.Length % 2 != 0)
            {
                throw new ArgumentException(Resources.HexStringError, "hexString");
            }

            return Enumerable.Range(0, hexString.Length)
                .Where(x => x % 2 == 0)
                .Select(x => System.Convert.ToByte(hexString.Substring(x, 2), 16))
                .ToArray();
        }

        /// <summary>
        /// Converts a byte array to a hexadecimal string.
        /// </summary>
        /// <param name="byteArray">The byte array.</param>
        /// <returns></returns>
        /// <exception cref="System.ArgumentNullException">byteArray</exception>
        public static string ToHexString(this byte[] byteArray)
        {
            if (byteArray == null)
            {
                throw new ArgumentNullException("byteArray");
            }

            return BitConverter.ToString(byteArray).Replace("-", String.Empty);
        }

        #endregion Public Methods
    }

    /// <summary>
    /// Holds properties for tag information.
    /// </summary>
    [DataContract]
    public class TagInfo : InternalObservableObject, IEquatable<TagInfo>, ICloneable, IDataErrorInfo, IExtensibleDataObject
    {
        #region Private Fields

        /// <summary>
        /// The current material weight.
        /// </summary>
        private uint currentMaterialWeight;

        /// <summary>
        /// An error message indicating what is wrong with this object.
        /// </summary>
        private string error;

        /// <summary>
        /// The material information
        /// </summary>
        private MaterialInfo materialInfo;

        /// <summary>
        /// The serial number
        /// </summary>
        private byte[] serialNumber;

        #endregion Private Fields

        #region Public Fields

        /// <summary>
        /// The tag counter initial value.
        /// </summary>
        public const uint TagCounterInitialValue = (uint)200e6;

        #endregion Public Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="TagInfo"/> class.
        /// </summary>
        public TagInfo()
        {
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the current material weight.
        /// </summary>
        /// <value>
        /// The current material weight.
        /// </value>
        [DataMember(Order = 3)]
        public uint CurrentMaterialWeight
        {
            get
            {
                return currentMaterialWeight;
            }
            set
            {
                currentMaterialWeight = value;
                RaisePropertyChanged(() => CurrentMaterialWeight);
            }
        }

        /// <summary>
        /// Gets a value indicating whether this instance has error.
        /// </summary>
        /// <value>
        ///   <c>true</c> if this instance has error; otherwise, <c>false</c>.
        /// </value>
        public bool HasError
        {
            get
            {
                return !String.IsNullOrEmpty(Error);
            }
        }

        /// <summary>
        /// Gets or sets the material information.
        /// </summary>
        /// <value>
        /// The material information.
        /// </value>
        [DataMember(Order = 1)]
        public MaterialInfo MaterialInfo
        {
            get
            {
                return materialInfo;
            }
            set
            {
                materialInfo = value;
                RaisePropertyChanged(() => MaterialInfo);
            }
        }

        /// <summary>
        /// Gets or sets the serial number.
        /// </summary>
        /// <value>
        /// The serial number.
        /// </value>
        [DataMember(Order = 2)]
        public byte[] SerialNumber
        {
            get
            {
                return serialNumber;
            }
            set
            {
                serialNumber = value;
                RaisePropertyChanged(() => SerialNumber);
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
            int bytesRead = 0;
            if (materialInfo != null)
            {
                bytesRead = materialInfo.Decode(buffer);
            }

            using (BinaryReader br = new BinaryReader(new MemoryStream(buffer, bytesRead, buffer.Length - bytesRead)))
            {
                byte serialNumberLength = br.ReadByte();
                serialNumber = br.ReadBytes(serialNumberLength);
                return bytesRead + serialNumberLength;
            }

            // current material weight wasn't deserialized on purpose
        }

        /// <summary>
        /// Serializes instance to a binary buffer.
        /// </summary>
        /// <returns>The buffer.</returns>
        public byte[] Encode()
        {
            byte[] buffer;
            if (materialInfo == null)
            {
                buffer = new byte[0];
            }
            else
            {
                buffer = materialInfo.Encode();
            }

            int length = buffer.Length;
            if (serialNumber != null)
            {
                Array.Resize(ref buffer, buffer.Length + 32);
                using (BinaryWriter bw = new BinaryWriter(new MemoryStream(buffer, length, buffer.Length - length)))
                {
                    bw.Write((byte)serialNumber.Length);
                    bw.Write(serialNumber);
                    length += (int)bw.BaseStream.Position;
                }
            }

            // current material weight wasn't serialized on purpose.
            Array.Resize(ref buffer, length);
            return buffer;
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
                if (!String.IsNullOrEmpty(error))
                {
                    return error;
                }

                if (MaterialInfo.Error != null)
                {
                    return MaterialInfo.Error;
                }

                return this.GetType().GetProperties()
                    .Select(pi => this[pi.Name])
                    .FirstOrDefault(err => err != null);
            }
            set
            {
                error = value;
                RaisePropertyChanged(() => Error);
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
                if (columnName == "CurrentMaterialWeight")
                {
                    if (CurrentMaterialWeight == 0)
                    {
                        return Resources.EmptyCartridge;
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
        public static bool operator !=(TagInfo x, TagInfo y)
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
        public static bool operator ==(TagInfo x, TagInfo y)
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
        public bool Equals(TagInfo other)
        {
            return other != null &&
                ((serialNumber == null) ? (other.serialNumber == null) : serialNumber.Equals(other.serialNumber)) &&
                ((materialInfo == null) ? (other.materialInfo == null) : materialInfo.Equals(other.materialInfo));
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
            return Equals(other as TagInfo);
        }

        /// <summary>
        /// Returns a hash code for this instance.
        /// </summary>
        /// <returns>
        /// A hash code for this instance, suitable for use in hashing algorithms and data structures like a hash table.
        /// </returns>
        public override int GetHashCode()
        {
            int hashCode = 0;
            if (SerialNumber != null)
            {
                hashCode = serialNumber.GetHashCode();
            }

            if (materialInfo != null)
            {
                hashCode ^= materialInfo.GetHashCode();
            }

            return hashCode;
        }

        #endregion IEquatable Members

        #region IClonable Members

        /// <summary>
        /// Creates a new object that is a copy of the current instance.
        /// </summary>
        /// <returns>
        /// A new object that is a copy of this instance.
        /// </returns>
        /// <exception cref="System.NotImplementedException"></exception>
        public object Clone()
        {
            TagInfo cloned = new TagInfo
                {
                    MaterialInfo = (materialInfo == null) ? null : (MaterialInfo)materialInfo.Clone(),
                    CurrentMaterialWeight = currentMaterialWeight,
                    SerialNumber = serialNumber,
                };

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