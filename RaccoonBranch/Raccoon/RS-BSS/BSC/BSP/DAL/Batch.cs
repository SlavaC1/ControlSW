using BSS.Contracts;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CBS.BSP.DAL
{
    public partial class Batch
    {
        /// <summary>
        /// Encodes this instance.
        /// </summary>
        /// <returns></returns>
        public byte[] Encode()
        {
            using (BinaryWriter bw = new BinaryWriter(new MemoryStream()))
            {
                bw.Write(BatchNumber);
                bw.Write(IdtVersion);
                bw.Write(MaterialID);
                bw.Write(MaterialKind);
                bw.Write(ManufacturingDate.ToUnixTime());
                bw.Write(ExpirationDate.ToUnixTime());
                bw.Write(InitialWeight);
                if (ExtraWeight != null)
                {
                    bw.Write(ExtraWeight.Value);
                }
                if (BurnStartTime != null)
                {
                    bw.Write(BurnStartTime.Value.ToUnixTime());
                }
                if (BurnEndTime != null)
                {
                    bw.Write(BurnEndTime.Value.Date.ToBinary());
                }

                return ((MemoryStream)bw.BaseStream).ToArray();
            }
        }
    }
}
