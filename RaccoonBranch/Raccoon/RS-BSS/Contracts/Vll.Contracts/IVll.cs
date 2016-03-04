using System.Collections.Generic;
using System.ServiceModel;
using System.Threading.Tasks;

namespace CBS.VLL.Contracts
{
    /// <summary>
    /// Defines methods to populate VaultIC tags list.
    /// </summary>
    [ServiceContract(CallbackContract = typeof(IVllCallback))]
    public interface IVll
    {
        /// <summary>
        /// Loads the content of the VaultIC tags list into DB.
        /// </summary>
        /// <param name="content">The content.</param>
        [OperationContract]
        Task LoadVaultICListFileAsync(string name, string content);
    }

    /// <summary>
    /// Defines callback methods of VaultIC tags list loading.
    /// </summary>
    public interface IVllCallback
    {
        /// <summary>
        /// Reports progress of loading process.
        /// </summary>
        /// <param name="tagSerialNumber">The serial number of the loaded tag.</param>
        /// <param name="index">The tag index.</param>
        /// <param name="tagsCount">Total number of tags.</param>
        [OperationContract(IsOneWay = true)]
        void ReportProgress(string tagSerialNumber, int index, int tagsCount);

        /// <summary>
        /// Called when VaultIC tags list loading is completed.
        /// </summary>
        /// <param name="message">The message.</param>
        [OperationContract(IsOneWay = true)]
        void OnLoadingCompleted(int listCount, int numberOfTagsToLoad, int newTagsCount, int usedTagsCount);
    }
}