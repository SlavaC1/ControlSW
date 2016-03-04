using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Serialization;
using System.Text;

namespace BSS.Contracts
{
    [DataContract]
    public class SignedObject<T>
    {
        [DataMember]
        public T Object { get; set; }

        [DataMember]
        public byte[] Signature { get; set; }
    }
}
