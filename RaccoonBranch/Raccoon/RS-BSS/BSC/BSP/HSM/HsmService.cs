using BCS.HsmClient;
using BSS.Contracts;
using CBS.BSP.Properties;
using Hsm.Contracts;
using System;
using System.Configuration;
using System.IO;
using System.Security.Cryptography;
using System.ServiceModel;
using System.Text;
using System.Threading.Tasks;
using WcfInfras;

namespace CBS.BSP.HSM
{
    /// <summary>
    /// A service for invoking HSM client.
    /// </summary>
    [ServiceBehavior(InstanceContextMode = InstanceContextMode.PerCall, IncludeExceptionDetailInFaults = DebugHelper.IncludeExceptionDetailInFaults)]
    public class HsmService : BspService, IHsm
    {
        #region Private Fields

        /// <summary>
        /// The HSM password decryption IV.
        /// </summary>
        private static readonly byte[] IV = { 0x75, 0x24, 0x7b, 0x6a, 0x55, 0x60, 0xfe, 0xa6, 0x4c, 0x4c, 0x94, 0x40, 0x92, 0x6d, 0xe1, 0x44 };

        /// <summary>
        /// The HSM password decryption key.
        /// </summary>
        private static readonly byte[] Key = { 0x32, 0xf7, 0xa5, 0x07, 0x96, 0x27, 0x2c, 0x77, 0xed, 0xb3, 0x8f, 0x4c, 0x5b, 0x2b, 0xe2, 0xf7, 0x00, 0xb4, 0xec, 0x82, 0x04, 0xaf, 0xd2, 0x18, 0xb9, 0x15, 0xdb, 0x8c, 0xa9, 0x93, 0x1f, 0xad };

        /// <summary>
        /// The HSM client wrapper.
        /// </summary>
        private HsmClientWrapper hsmClientWrapper;

        /// <summary>
        /// The HSM partition number.
        /// </summary>
        private int hsmPartitionNumber;

        /// <summary>
        /// The password to login to HSM partition.
        /// </summary>
        private string hsmPassword;

        /// <summary>
        /// The HSM library path.
        /// </summary>
        private string hsmPath;

        /// <summary>
        /// <c>true</c> to use HSM simulator.
        /// </summary>
        private bool useHsmSimulator;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="HsmService"/> class.
        /// </summary>
        public HsmService()
        {
            bool isHsmClientInitialized = hsmClientWrapper != null && hsmClientWrapper.IsInitialized();
            if (!isHsmClientInitialized)
            {
                SetConfigurationParameters();
            }

            if (hsmClientWrapper == null)
            {
                hsmClientWrapper = HsmClientWrapper.GetInstance();
                bool libraryLoaded = hsmClientWrapper.LoadHsmLibrary(hsmPath);
                Logger.Info(Resources.LoadingHsm);
                if (libraryLoaded)
                {
                    Logger.Info(Resources.HsmLoadedSuccessfully);
                }
                else
                {
                    Logger.Error(String.Format(Resources.HsmLibNotFound, hsmPath));
                }
            }
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Gets Stratasys public key.
        /// </summary>
        /// <returns></returns>
        public byte[] GetPublicKey()
        {
            CheckUserName();

            bool initialized = TryInit();
            if (!initialized)
            {
                ThrowFaultException(new InvalidOperationException(Resources.FailedInitializingHsmClient));
            }

            Logger.Debug(Resources.GettingPublicKey);

            byte[] pubKS;
            ulong err = hsmClientWrapper.GetPublicKey(out pubKS);
            if (err == 0 && pubKS != null)
            {
                Logger.DebugFormat(Resources.PublicKey, pubKS.ToHexString());
            }
            else
            {
                Logger.Error(hsmClientWrapper.GetError() + " " + String.Format(Resources.ErrorCode, err));
                ThrowFaultException(new InvalidOperationException(Resources.ErrorGettingPublicKey));
            }

            return pubKS;
        }

        /// <summary>
        /// Asynchronously gets Stratasys public key.
        /// </summary>
        /// <returns></returns>
        public async Task<byte[]> GetPublicKeyAsync()
        {
            CheckUserName();
            return await Task<byte[]>.Run(() =>
                {
                    return GetPublicKey();
                });
        }

        /// <summary>
        /// Signs a message using Stratasys private key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <returns>
        /// The message signed by Stratasys private key.
        /// </returns>
        public byte[] SignData(byte[] message)
        {
            bool initialized = TryInit();
            if (!initialized)
            {
                ThrowFaultException(new InvalidOperationException(Resources.FailedInitializingHsmClient));
            }

            Logger.DebugFormat(Resources.SigningMessage, message.ToHexString());

            byte[] signature;
            ulong err = hsmClientWrapper.SignData(message, out signature);
            if (err == 0 && signature != null)
            {
                Logger.DebugFormat(Resources.Signature, signature.ToHexString());
            }
            else
            {
                Logger.Error(hsmClientWrapper.GetError() + " " + String.Format(Resources.ErrorCode, err));
            }

            return signature;
        }

        /// <summary>
        /// Asynchronously signs a message using Stratasys private key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <returns>The message signed by Stratasys private key.</returns>
        public async Task<byte[]> SignDataAsync(byte[] message)
        {
            return await Task<byte[]>.Run(() =>
                {
                    return SignData(message);
                });
        }

        /// <summary>
        /// Verifies signature of a message by Stratasys public key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        public bool VerifySignature(byte[] message, byte[] signature)
        {
            bool initialized = TryInit();
            if (!initialized)
            {
                ThrowFaultException(new InvalidOperationException(Resources.FailedInitializingHsmClient));
            }

            Logger.DebugFormat(Resources.VerfyingMessage, message.ToHexString(), signature.ToHexString());

            ulong err = hsmClientWrapper.VerifySignature(message, signature);
            if (err != 0)
            {
                Logger.Error(hsmClientWrapper.GetError() + " " + String.Format(Resources.ErrorCode, err));
            }

            return err == 0;
        }

        /// <summary>
        /// Verifies signature of a message by a given public key.
        /// </summary>
        /// <param name="publicKey">The public key.</param>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        public bool VerifySignature(byte[] publicKey, byte[] message, byte[] signature)
        {
            bool initialized = TryInit();
            if (!initialized)
            {
                ThrowFaultException(new InvalidOperationException(Resources.FailedInitializingHsmClient));
            }

            Logger.DebugFormat(Resources.VerfyingMessage, message.ToHexString(), signature.ToHexString());

            ulong err = hsmClientWrapper.VerifySignature(publicKey, message, signature);
            if (err != 0)
            {
                Logger.Error(hsmClientWrapper.GetError() + " " + String.Format(Resources.ErrorCode, err));
            }

            return err == 0;
        }

        /// <summary>
        /// Asynchronously signature of a message by Stratasys public key.
        /// </summary>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        public async Task<bool> VerifySignatureAsync(byte[] message, byte[] signature)
        {
            return await Task<bool>.Run(() =>
                {
                    return VerifySignature(message, signature);
                });
        }

        /// <summary>
        /// Asynchronously signature of a message by a given public key.
        /// </summary>
        /// <param name="publicKey">The public key.</param>
        /// <param name="message">The message.</param>
        /// <param name="signature">The signature.</param>
        /// <returns><c>true</c> if signature is valid; otherwise, <c>false</c>.</returns>
        public async Task<bool> VerifySignatureAsync(byte[] publicKey, byte[] message, byte[] signature)
        {
            return await Task<bool>.Run(() =>
            {
                return VerifySignature(publicKey, message, signature);
            });
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Decrypts the encrypted HSM password.
        /// </summary>
        /// <param name="encryptedPassword">The encrypted HSM password.</param>
        /// <returns>The HSM password decrypted.</returns>
        private string DecryptPassword(string encryptedPassword)
        {
            try
            {
                byte[] passwordData = encryptedPassword.ToByteArray();
                MemoryStream msDecrypt = new MemoryStream(passwordData);

                using (CryptoStream csDecrypt = new CryptoStream(msDecrypt, new RijndaelManaged().CreateDecryptor(Key, IV), CryptoStreamMode.Read))
                {
                    byte[] fromEncrypt = new byte[passwordData.Length];
                    csDecrypt.Read(fromEncrypt, 0, fromEncrypt.Length);
                    return ASCIIEncoding.Default.GetString(fromEncrypt).TrimEnd('\0');
                }
            }
            catch (Exception ex)
            {
                Logger.Error(Resources.CannotDecryptHsmPassword, ex);
                return null;
            }
        }

        /// <summary>
        /// Sets the configuration parameters.
        /// </summary>
        private void SetConfigurationParameters()
        {
            hsmPassword = String.Empty;

            try
            {
                Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                KeyValueConfigurationElement configElement = config.AppSettings.Settings["UseHsmSimulator"];
                if (configElement != null)
                {
                    Boolean.TryParse(configElement.Value, out useHsmSimulator);
                }

                configElement = config.AppSettings.Settings["HsmPath"];
                if (configElement != null)
                {
                    hsmPath = configElement.Value;
                }

                configElement = config.AppSettings.Settings["HsmPartition"];
                if (configElement != null)
                {
                    int newHsmPartitionNumber;
                    if (Int32.TryParse(configElement.Value, out newHsmPartitionNumber))
                    {
                        hsmPartitionNumber = newHsmPartitionNumber;
                    }
                }

                configElement = config.AppSettings.Settings["HsmPassword"];
                if (configElement != null)
                {
                    hsmPassword = DecryptPassword(configElement.Value);
                }
            }
            catch (Exception ex)
            {
                Logger.Warn(Resources.ErrorReadingConfig, ex);
            }
        }

        /// <summary>
        /// Tries to initialize HSM client.
        /// </summary>
        /// <returns></returns>
        private bool TryInit()
        {
            bool initialized = hsmClientWrapper.IsInitialized();
            if (initialized)
            {
                return true;
            }

            Logger.Info(Resources.InitializingHsmClient);
            ulong err = hsmClientWrapper.Initialize(useHsmSimulator, hsmPartitionNumber, hsmPassword);
            Logger.Info("partition: " + hsmPartitionNumber + ", use sim: " + useHsmSimulator);
            initialized = (err == 0);
            if (initialized)
            {
                Logger.Info(Resources.HsmClientInitializedSuccessfully);
                
                // log the public key as Hex string
                byte[] pubKey;
                hsmClientWrapper.GetPublicKey(out pubKey);
                Logger.Info("PubKey: " + pubKey.ToHexString());                
            }
            else
            {
                Logger.Error(hsmClientWrapper.GetError() + " " + String.Format(Resources.ErrorCode, err));
            }

            return initialized;
        }

        #endregion Private Methods
    }
}