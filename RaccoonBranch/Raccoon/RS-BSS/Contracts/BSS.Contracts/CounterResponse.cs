using System;
using System.IO;
using System.Runtime.Serialization;

namespace BSS.Contracts
{
    /// <summary>
    /// Holds response of tag secured counter.
    /// </summary>
    [DataContract]
    public class CounterResponse
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="CounterResponse"/> class.
        /// </summary>
        public CounterResponse(byte[] randomMessage, byte[] signature)
        {
            if (randomMessage == null)
            {
                throw new ArgumentNullException("randomMessage");
            }

            if (randomMessage.Length < 28)
            {
                throw new ArgumentException("Random message must contain at least 28 bytes.", "randomMessage");
            }

            if (signature == null)
            {
                throw new ArgumentNullException("signature");
            }

            RandomMessage = randomMessage;
            Signature = signature;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the random message.
        /// </summary>
        /// <value>
        /// The random message.
        /// </value>
        [DataMember]
        public byte[] RandomMessage { get; private set; }

        /// <summary>
        /// Gets or sets the signature.
        /// </summary>
        /// <value>
        /// The signature.
        /// </value>
        [DataMember]
        public byte[] Signature { get; private set; }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Gets the counter value.
        /// </summary>
        /// <returns></returns>
        public uint GetCounterValue()
        {
            using (BinaryReader br = new BinaryReader(new MemoryStream(RandomMessage)))
            {
                br.BaseStream.Seek(-sizeof(uint), SeekOrigin.End);
                return br.ReadUInt32();
            }
        }

        /// <summary>
        /// Gets the serial number.
        /// </summary>
        /// <returns></returns>
        public byte[] GetSerialNumber()
        {
            using (BinaryReader br = new BinaryReader(new MemoryStream(RandomMessage)))
            {
                br.BaseStream.Seek(8, SeekOrigin.Begin);
                return br.ReadBytes(8);
            }
        }

        #endregion Public Methods
    }
}