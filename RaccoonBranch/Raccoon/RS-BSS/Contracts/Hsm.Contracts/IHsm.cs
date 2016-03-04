using System.ServiceModel;
using System.Threading.Tasks;

namespace Hsm.Contracts
{
    /// <summary>
    /// Defines cryptography methods.
    /// </summary>
    [ServiceContract]
    public interface IHsm
    {
        /// <summary>
        /// Gets Stratasys public key.
        /// </summary>
        /// <returns></returns>
        byte[] GetPublicKey();

        /// <summary>
        /// Asynchronously gets Stratasys public key.
        /// </summary>
        /// <returns></returns>
        [OperationContract]
        Task<byte[]> GetPublicKeyAsync();

        /// <summary>
        /// Signs a message using Stratasys private key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <returns>The message signed by Stratasys private key.</returns>
        byte[] SignData(byte[] message);

        /// <summary>
        /// Asynchronously signs a message using Stratasys private key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <returns>The message signed by Stratasys private key.</returns>
        [OperationContract]
        Task<byte[]> SignDataAsync(byte[] message);

        /// <summary>
        /// Verifies signature equals to the message signed by Stratasys private key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        bool VerifySignature(byte[] message, byte[] signature);

        /// <summary>
        /// Verifies signature of a message by a given public key.
        /// </summary>
        /// <param name="publicKey">The public key.</param>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        bool VerifySignature(byte[] publicKey, byte[] message, byte[] signature);

        /// <summary>
        /// Asynchronously verifies signature equals to the message signed by Stratasys private key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        [OperationContract]
        Task<bool> VerifySignatureAsync(byte[] message, byte[] signature);

        /// <summary>
        /// Asynchronously signature of a message by a given public key.
        /// </summary>
        /// <param name="publicKey">The public key.</param>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        [OperationContract(Name = "VerifySignatureWithPublicKeyAsync")]
        Task<bool> VerifySignatureAsync(byte[] publicKey, byte[] message, byte[] signature);
    }
}