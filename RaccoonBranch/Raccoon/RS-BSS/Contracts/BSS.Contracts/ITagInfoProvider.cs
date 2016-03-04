using System.ServiceModel;
using System.Threading.Tasks;

namespace BSS.Contracts
{
    /// <summary>
    /// A service contract to provide tag confidential information.
    /// </summary>
    [ServiceContract(SessionMode = SessionMode.Required)]
    public interface ITagInfoProvider
    {
        /// <summary>
        /// Starts a new Lot burning.
        /// </summary>
        /// <returns>The Lot to be burn.</returns>
        [OperationContract]
        Task<BatchStatistics> StartLotBurningAsync();

        /// <summary>
        /// Gets the tag registration as appears in VaultIC list DB.
        /// </summary>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <returns>The tag registration.</returns>
        [OperationContract]
        Task<TagRegistration> GetTagRegistrationAsync(byte[] serialNumber);

        /// <summary>
        /// Gets the weight to decrease from the counter initial value to reflect the actual container weight.
        /// </summary>
        /// <returns>
        /// The weight to decrease from the counter initial value to reflect the actual container weight.
        /// </returns>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <exception cref="Syste.InvalidOperationException">Tag is not new.</exception>
        [OperationContract]
        Task<uint> GetWeightToDecreaseAsync(byte[] serialNumber);

        /// <summary>
        /// Verifies that tag counter value equals to the expected value and counter signature is valid.
        /// </summary>
        /// <param name="counterResponse">Tag counter response.</param>
        /// <returns><c>true</c> if values are equal and validated; otherwise, <c>false</c>.</returns>
        [OperationContract]
        Task<bool> VerifyCounterAsync(CounterResponse counterResponse);

        /// <summary>
        /// Builds the tag information and sign it.
        /// </summary>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <returns></returns>
        [OperationContract]
        Task<SignedObject<TagInfo>> BuildTagInfoAsync(byte[] serialNumber);

        /// <summary>
        /// Asynchronously adds the tag burning to history.
        /// </summary>
        /// <param name="tagStatistics">The tag statistics.</param>
        /// <returns></returns>
        [OperationContract]
        Task AddTagBurningToHistoryAsync(TagStatistics tagStatistics);

        /// <summary>
        /// Asynchronously ends the lot.
        /// </summary>
        [OperationContract(IsTerminating = true)]
        Task EndLotAsync();
    }
}