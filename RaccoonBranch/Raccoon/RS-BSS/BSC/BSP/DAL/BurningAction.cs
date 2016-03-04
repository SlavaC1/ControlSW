using BSS.Contracts;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CBS.BSP.DAL
{
    public partial class BurningAction
    {
        #region Public Methods

        /// <summary>
        /// Encodes this instance.
        /// </summary>
        /// <returns></returns>
        public byte[] Encode()
        {
            using (BinaryWriter bw = new BinaryWriter(new MemoryStream()))
            {
                bw.Write(Id);
                bw.Write(SerialNumber.ToArray());
                bw.Write(BatchNumber.Trim());
                bw.Write(StationName.Trim());
                bw.Write(BurningTime.ToUnixTime());
                bw.Write(IsReference);
                if (ErrorMessage != null)
                {
                    bw.Write(ErrorMessage.Trim());
                }

                return ((MemoryStream)bw.BaseStream).ToArray();
            }
        }

        #endregion Public Methods
    }
}
