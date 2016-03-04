using System;
using System.Collections.Generic;
using System.Linq;
using System.ServiceModel;
using System.Text;
using System.Threading.Tasks;

namespace BSS.Contracts
{
    [ServiceContract]
    public interface IReport
    {
        /// <summary>
        /// Asynchronously reads all batch numbers from RTDB.
        /// </summary>
        /// <returns></returns>
        [OperationContract]
        Task<string[]> ReadAllBatchNumbersAsync();

        /// <summary>
        /// Asynchronously reads the batch history from RTDB.
        /// </summary>
        /// <param name="batchNumber">The batch number.</param>
        /// <returns></returns>
        [OperationContract]
        Task<BurningActionsStatistics> ReadBatchHistoryAsync(string batchNumber);

        /// <summary>
        /// Asynchronously gets the name of the material.
        /// </summary>
        /// <param name="materialID">The material identifier.</param>
        /// <returns></returns>
        [OperationContract]
        Task<string> GetMaterialNameAsync(ushort materialID);
    }
}
