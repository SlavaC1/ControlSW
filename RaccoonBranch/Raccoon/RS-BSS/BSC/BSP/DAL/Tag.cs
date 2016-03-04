using BSS.Contracts;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CBS.BSP.DAL
{
    public partial class Tag
    {
        /// <summary>
        /// Encodes this instance.
        /// </summary>
        /// <returns></returns>
        public byte[] Encode()
        {
            using (BinaryWriter bw = new BinaryWriter(new MemoryStream()))
            {
                bw.Write(SerialNumber.ToArray());
                bw.Write(Date.ToUnixTime());
                bw.Write(PublicKey.ToArray());
                bw.Write(Status);
                if (LastUpdateTime != null)
                {
                    bw.Write(LastUpdateTime.Value.ToUnixTime());
                }

                return ((MemoryStream)bw.BaseStream).ToArray();
            }
        }
    }
}
