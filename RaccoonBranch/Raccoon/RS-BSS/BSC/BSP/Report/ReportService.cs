using BSS.Contracts;
using CBS.BSP.DAL;
using CBS.BSP.HSM;
using System;
using System.ServiceModel;
using System.Threading.Tasks;
using WcfInfras;

namespace CBS.BSP.Report
{
    /// <summary>
    /// A service for retrieving data for report.
    /// </summary>
    [ServiceBehavior(IncludeExceptionDetailInFaults = DebugHelper.IncludeExceptionDetailInFaults)]
    public class ReportService : BspService, IReport
    {
        #region Private Fields

        /// <summary>
        /// The HSM service.
        /// </summary>
        private HsmService hsmService;

        /// <summary>
        /// The VLL access layer.
        /// </summary>
        private RtDal rtDal;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ReportService"/> class.
        /// </summary>
        public ReportService()
        {
            rtDal = new RtDal();
            hsmService = new HsmService();
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Asynchronously gets the name of the material.
        /// </summary>
        /// <param name="materialID">The material identifier.</param>
        /// <returns></returns>
        public async Task<string> GetMaterialNameAsync(ushort materialID)
        {
            string materialName = null;

            try
            {
                materialName = await rtDal.ReadMaterialNameAsync(materialID);
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }

            return materialName;
        }

        /// <summary>
        /// Reads all batch numbers from RTDB.
        /// </summary>
        /// <returns></returns>
        public async Task<string[]> ReadAllBatchNumbersAsync()
        {
            string[] batchNumbers = null;

            try
            {
                batchNumbers = await rtDal.ReadBatchNumbersAsync();
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }

            return batchNumbers;
        }

        /// <summary>
        /// Reads the batch history from RTDB.
        /// </summary>
        /// <param name="batchNumber">The batch number.</param>
        /// <returns></returns>
        public async Task<BurningActionsStatistics> ReadBatchHistoryAsync(string batchNumber)
        {
            BurningActionsStatistics batchHistory = null;

            try
            {
                batchHistory = await rtDal.ReadBatchHistoryAsync(batchNumber, hsmService);
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }

            return batchHistory;
        }
        #endregion Public Methods

        #region IDisposable Members

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected override void Dispose(bool dispose)
        {
            if (dispose)
            {
                if (hsmService != null)
                {
                    hsmService.Dispose();
                }

                if (rtDal != null)
                {
                    rtDal.Dispose();
                }
            }
        }

        #endregion IDisposable Members
    }
}