using BSS.Contracts;
using CBS.BSP.DAL;
using CBS.BSP.HSM;
using CBS.BSP.Properties;
using System;
using System.Configuration;
using System.Globalization;
using System.IO;
using System.Linq;
using System.ServiceModel;
using System.Threading.Tasks;
using WcfInfras;

namespace CBS.BSP.TagInfoProvider
{
    /// <summary>
    /// A service for getting tag information from Oracle client and VaultIC list.
    /// </summary>
    [ServiceBehavior(IncludeExceptionDetailInFaults = DebugHelper.IncludeExceptionDetailInFaults, ConcurrencyMode = ConcurrencyMode.Single)]
    public class TagInfoProviderService : BspService, ITagInfoProvider
    {
        #region Private Fields

        /// <summary>
        /// The history folder name.
        /// </summary>
        private const string HistoryFolderName = "History";

        private const string OracleFileFormat = "AAA{0}.txt";

        /// <summary>
        /// The HSM service.
        /// </summary>
        private readonly HsmService hsmService;

        /// <summary>
        /// The current material information.
        /// </summary>
        private MaterialInfo currentMaterialInfo;

        /// <summary>
        /// The current station name.
        /// </summary>
        private string currentStationName;

        /// <summary>
        /// The VLL access layer.
        /// </summary>
        private RtDal rtDal;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="TagInfoProviderService"/> class.
        /// </summary>
        public TagInfoProviderService()
        {
            string stationName = GetStationName();
            Logger.InfoFormat(Resources.BsaSessionStarted, stationName);

            string oracleFolderBasePath = Settings.Default.OracleFolderPath;

            try
            {
                System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                KeyValueConfigurationElement configElement = config.AppSettings.Settings["OracleFilePath"];
                if (configElement != null)
                {
                    oracleFolderBasePath = configElement.Value;
                }
            }
            catch (Exception ex)
            {
                Logger.Warn(Resources.ErrorReadingConfig, ex);
            }

            OracleFolderPath = Path.Combine(oracleFolderBasePath, stationName);
            hsmService = new HsmService();
            rtDal = new RtDal();
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets the oracle file path.
        /// </summary>
        /// <value>
        /// The oracle file path.
        /// </value>
        public string OracleFolderPath { get; set; }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Asynchronously Adds the tag burning to history.
        /// </summary>
        /// <param name="tagStatistics">The tag statistics.</param>
        public async Task AddTagBurningToHistoryAsync(TagStatistics tagStatistics)
        {
            CheckUserName();

            try
            {
                await rtDal.InsertBurningActionAsync(tagStatistics, hsmService);
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }
        }

        /// <summary>
        /// Builds the tag information and sign it.
        /// </summary>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <returns></returns>
        public async Task<SignedObject<TagInfo>> BuildTagInfoAsync(byte[] serialNumber)
        {
            CheckUserName();

            Logger.InfoFormat(Resources.BuildingSignedIDD, GetStationName());
            Tag tag = await TestTagInfoAsync(serialNumber);
            TagInfo tagInfo = new TagInfo
                {
                    MaterialInfo = currentMaterialInfo,
                    SerialNumber = serialNumber,
                    CurrentMaterialWeight = currentMaterialInfo.GetTotalWeight()
                };

            byte[] pubKC = tag.PublicKey.ToArray();
            byte[] buffer = tagInfo.Encode();
            Array.Resize(ref buffer, buffer.Length + pubKC.Length);
            Array.Copy(pubKC, 0, buffer, buffer.Length - pubKC.Length, pubKC.Length);
            byte[] signature = await hsmService.SignDataAsync(buffer);
            if (signature == null)
            {
                ThrowFaultException(new InvalidOperationException(Resources.ErrorSigningData));
            }

            bool markedAsUsed = await rtDal.UpdateAndSignTagAsync(serialNumber, TagRegistration.Used, hsmService);
            if (!markedAsUsed)
            {
                ThrowFaultException(new InvalidOperationException(Resources.ErrorSigningData));
            }

            Logger.InfoFormat(
                Resources.TagInfoDetails,
                String.Format(Resources.MaterialInfoDetails,
                    tagInfo.MaterialInfo.TagStructID, tagInfo.MaterialInfo.MaterialID, tagInfo.MaterialInfo.MaterialKind,
                    tagInfo.MaterialInfo.ManufacturingTime.ToShortDateString(), tagInfo.MaterialInfo.BatchNumber,
                    tagInfo.MaterialInfo.InitialWeight, tagInfo.MaterialInfo.ExpirationDate.ToShortDateString()),
                tagInfo.SerialNumber.ToHexString(),
                tagInfo.CurrentMaterialWeight);

            return new SignedObject<TagInfo>
            {
                Object = tagInfo,
                Signature = signature,
            };
        }

        /// <summary>
        /// Ends the lot.
        /// </summary>
        public async Task EndLotAsync()
        {
            CheckUserName();

            await TestMaterialInfo();

            try
            {
                await rtDal.EndLotAsync(currentMaterialInfo.BatchNumber);
                DeleteOracleFile();
                currentMaterialInfo = null;
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }
        }

        /// <summary>
        /// Gets the tag registration as appears in VaultIC list DB.
        /// </summary>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <returns>
        /// The tag registration.
        /// </returns>
        public async Task<TagRegistration> GetTagRegistrationAsync(byte[] serialNumber)
        {
            CheckUserName();

            if (serialNumber == null)
            {
                ThrowFaultException(new ArgumentNullException("serialNumber"));
            }

            Tag tag = null;
            try
            {
                tag = await rtDal.FindTagAsync(serialNumber);
                if (tag == null)
                {
                    return TagRegistration.Unknown;
                }
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }

            //bool isValid = await hsmService.VerifySignatureAsync(tag.Encode(), tag.Signature.ToArray());
            //if (!isValid)
            //{
            //    ThrowFaultException(new InvalidOperationException(String.Format(Resources.InvalidTag, serialNumber.ToHexString())));
            //}

            return (TagRegistration)(int)tag.Status;
        }

        /// <summary>
        /// Gets the weight to decrease from the counter initial value to reflect the actual container weight.
        /// </summary>
        /// <param name="serialNumber">The tag serial number.</param>
        /// <returns>
        /// The weight to decrease from the counter initial value to reflect the actual container weight.
        /// </returns>
        public async Task<uint> GetWeightToDecreaseAsync(byte[] serialNumber)
        {
            CheckUserName();

            Tag tag = await TestTagInfoAsync(serialNumber);
            if ((TagRegistration)(int)tag.Status == TagRegistration.Used)
            {
                ThrowFaultException(new InvalidOperationException(String.Format(Resources.TagIsUsed, serialNumber.ToHexString())));
            }

            Logger.InfoFormat(Resources.SerialNumberReceived, serialNumber.ToHexString(), GetStationName());
            uint weightToDecrease = TagInfo.TagCounterInitialValue - currentMaterialInfo.GetTotalWeight();
            Logger.InfoFormat(Resources.DecrementValueSentToBSA, weightToDecrease, GetStationName());
            return weightToDecrease;
        }

        /// <summary>
        /// Starts a new Lot burning.
        /// </summary>
        /// <returns>The Lot to be burn.</returns>
        public async Task<BatchStatistics> StartLotBurningAsync()
        {
            CheckUserName();
            BatchStatistics lot = null;

            try
            {
                Logger.Info(Resources.LdfRead);
                currentMaterialInfo = await ReadMaterialInfo();
                lot = await rtDal.GetLotAsync(currentMaterialInfo.BatchNumber);
                if (lot == null)
                {
                    lot = new BatchStatistics
                    {
                        MaterialInfo = currentMaterialInfo,
                    };
                }

                Logger.InfoFormat(
                    Resources.MaterialInfoDetails,
                    currentMaterialInfo.TagStructID, currentMaterialInfo.MaterialID, currentMaterialInfo.MaterialKind,
                    currentMaterialInfo.ManufacturingTime.ToShortDateString(), currentMaterialInfo.BatchNumber,
                    currentMaterialInfo.InitialWeight, currentMaterialInfo.ExpirationDate.ToShortDateString());

                if (lot.IsNew())
                {
                    Logger.Info(Resources.StartNewLot);
                }
                else if (lot.IsOpened())
                {
                    Logger.Info(Resources.ContinueOpenedLot);
                }
                else
                {
                    Logger.Info(Resources.ContinueClosedLot);
                }

                if (lot.MaterialInfo != currentMaterialInfo)
                {
                    Logger.Warn(Resources.LdfMismatch);
                    lot.MaterialInfo = currentMaterialInfo;
                }
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }

            return lot;
        }

        /// <summary>
        /// Verifies that tag counter value equals to the expected value.
        /// </summary>
        /// <param name="counterResponse">Tag counter value.</param>
        /// <returns><c>true</c> if values are equal; otherwise, <c>false</c>.</returns>
        public async Task<bool> VerifyCounterAsync(CounterResponse counterResponse)
        {
            CheckUserName();

            await TestMaterialInfo();
            byte[] serialNumber = counterResponse.GetSerialNumber();
            bool ok = counterResponse.GetCounterValue() == currentMaterialInfo.GetTotalWeight();
            if (ok)
            {
                Tag tag = await rtDal.FindTagAsync(serialNumber);
                if (tag == null)
                {
                    ok = false;
                }
                else
                {
                    ok = await hsmService.VerifySignatureAsync(tag.PublicKey.ToArray(), counterResponse.RandomMessage, counterResponse.Signature);
                }
            }

            if (ok)
            {
                Logger.InfoFormat(Resources.CounterValueVerified, serialNumber.ToHexString());
            }
            else
            {
                Logger.ErrorFormat(Resources.CounterValueUnverified, serialNumber.ToHexString());
            }

            return ok;
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Deletes the oracle file.
        /// </summary>
        private void DeleteOracleFile()
        {
            string[] oracleFilesPath = Directory.GetFiles(OracleFolderPath, String.Format(OracleFileFormat, "*"));
            foreach (string path in oracleFilesPath)
            {
                File.Delete(path);
            }
        }

        /// <summary>
        /// Gets the name of the station connected to CBS.
        /// </summary>
        /// <returns></returns>
        private string GetStationName()
        {
            string stationHeaderContent = null;
            if (OperationContext.Current != null)
            {
                int index = OperationContext.Current.IncomingMessageHeaders.FindHeader("StationName", String.Empty);
                if (index != -1)
                {
                    stationHeaderContent = OperationContext.Current.IncomingMessageHeaders.GetHeader<string>(index);
                }
            }

            if (String.IsNullOrEmpty(stationHeaderContent))
            {
                stationHeaderContent = currentStationName ?? "BSA";
            }
            else if (currentStationName == null)
            {
                currentStationName = stationHeaderContent;
            }

            return stationHeaderContent;
        }

        /// <summary>
        /// Reads material info from Oracle file.
        /// </summary>
        private async Task<MaterialInfo> ReadMaterialInfo()
        {
            if (!Directory.Exists(OracleFolderPath))
            {
                throw new DirectoryNotFoundException(String.Format(Resources.OracleDirectoryNotFound, OracleFolderPath));
            }

            string oracleFilePath = Directory.GetFiles(OracleFolderPath, String.Format(OracleFileFormat, "*")).FirstOrDefault();
            if (oracleFilePath == null)
            {
                throw new FileNotFoundException(Resources.OracleFileNotFound);
            }

            MaterialInfo materialInfo = null;
            using (StreamReader sw = File.OpenText(oracleFilePath))
            {
                string content = await sw.ReadToEndAsync();
                string[] tokens = content.Trim().Split(
                    new char[] { '|', '\t', ' ' },
                    StringSplitOptions.RemoveEmptyEntries);

                materialInfo = new MaterialInfo
                {
                    TagStructID = UInt16.Parse(tokens[0]),
                    MaterialID = UInt16.Parse(tokens[1]),
                    MaterialKind = (MaterialKind)Enum.Parse(typeof(MaterialKind), tokens[2]),
                    ManufacturingTime = DateTime.ParseExact(tokens[3], "MM/dd/yy", CultureInfo.InvariantCulture),
                    BatchNumber = tokens[4],
                    InitialWeight = UInt16.Parse(tokens[5]),
                    ExpirationDate = DateTime.ParseExact(tokens[6], "MM/dd/yy", CultureInfo.InvariantCulture),
                };
            }

            string historyFolder = Path.Combine(OracleFolderPath, HistoryFolderName);
            if (!Directory.Exists(historyFolder))
            {
                Directory.CreateDirectory(historyFolder);
            }

            string oracleFileName = Path.GetFileName(oracleFilePath);
            File.Copy(oracleFilePath, Path.Combine(historyFolder, oracleFileName), overwrite: true);

            materialInfo.MaterialName = await rtDal.ReadMaterialNameAsync(materialInfo.MaterialID);

            return materialInfo;
        }

        /// <summary>
        /// Tests the material information.
        /// </summary>
        /// <exception cref="FaultException{ExceptionDetail}">
        /// Lot was not read.
        /// </exception>
        private async Task TestMaterialInfo()
        {
            if (currentMaterialInfo == null)
            {
                try
                {
                    currentMaterialInfo = await ReadMaterialInfo();
                }
                catch (Exception ex)
                {
                    ThrowFaultException(ex);
                }
            }
        }

        /// <summary>
        /// Tests that material info was read and tag appears in VLL and not marked as used.
        /// </summary>
        /// <param name="serialNumber">The serial number.</param>
        /// <returns></returns>
        /// <exception cref="System.FaultException{System.ServiceModel.ExceptionDetail}">
        /// Lot was not read or tag is found in VLL and marked as new.
        /// </exception>
        private async Task<Tag> TestTagInfoAsync(byte[] serialNumber)
        {
            if (serialNumber == null)
            {
                ThrowFaultException(new ArgumentNullException("serialNumber"));
            }

            Tag tag = null;
            try
            {
                tag = await rtDal.FindTagAsync(serialNumber);
                if (tag == null)
                {
                    ThrowFaultException(new InvalidOperationException(String.Format(Resources.TagNotFound, serialNumber.ToHexString())));
                }
            }
            catch (Exception ex)
            {
                ThrowFaultException(ex);
            }

            await TestMaterialInfo();
            return tag;
        }

        #endregion Private Methods

        #region IDisposable Members

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected override void Dispose(bool dispose)
        {
            if (dispose)
            {
                if (rtDal != null)
                {
                    rtDal.Dispose();
                }

                if (hsmService != null)
                {
                    hsmService.Dispose();
                }

                Logger.InfoFormat(Resources.BsaSessionEnded, GetStationName());
            }
        }

        #endregion IDisposable Members
    }
}