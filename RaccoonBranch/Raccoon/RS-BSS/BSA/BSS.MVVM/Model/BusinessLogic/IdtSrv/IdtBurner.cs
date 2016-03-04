using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.Plc;
using BSS.MVVM.Properties;
using Hsm.Contracts;
using RaccoonCLI;
using System;
using System.IO;
using System.Threading.Tasks;
using WcfInfras.Client;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Responsible for burning material info into IDT when cartridge is inserted.
    /// </summary>
    public class IdtBurner : IdtReader
    {
        #region Private Fields

        /// <summary>
        /// The VLL file name.
        /// </summary>
        private const string VllFileName = "VLL.txt";

        /// <summary>
        /// A proxy invoker for tag info provider.
        /// </summary>
        private ProxyInvoker<ITagInfoProvider> bsProxyInvoker;

        private ushort currentSessionID;

        private bool terminateOperation;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtBurner"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="inPlaceManager">The in place manager.</param>
        /// <param name="materialMonitor">The material monitor.</param>
        /// <param name="plcWrapper">The PLC wrapper.</param>
        /// <exception cref="System.ArgumentNullException">burningDBManager</exception>
        public IdtBurner(ConfigurationParameters configurationParameters, InPlaceManager inPlaceManager, MaterialMonitorWrapper materialMonitor, IPlc plcWrapper)
            : base(configurationParameters, inPlaceManager, materialMonitor, plcWrapper)
        {
            LotStatistics = new BatchStatistics();
        }

        #endregion Public Constructors

        #region Public Events

        /// <summary>
        /// Occurs when Lot ended.
        /// </summary>
        public event EventHandler LotEnded;

        /// <summary>
        /// Occurs when Lot started.
        /// </summary>
        public event EventHandler LotStarted;

        /// <summary>
        /// Occurs when session ended.
        /// </summary>
        public event EventHandler SessionEnded;

        /// <summary>
        /// Occurs when session started.
        /// </summary>
        public event EventHandler SessionStarted;

        #endregion Public Events

        #region Public Properties

        /// <summary>
        /// Gets or sets a value indicating whether this burning is a reference burning.
        /// </summary>
        /// <value>
        /// <c>true</c> if this burning is a reference burning; otherwise, <c>false</c>.
        /// </value>
        public bool IsReference
        {
            get;
            set;
        }

        /// <summary>
        /// Gets the lot statistics.
        /// </summary>
        /// <value>
        /// The lot statistics.
        /// </value>
        public BatchStatistics LotStatistics
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets a value indicating whether reset tags.
        /// </summary>
        /// <value>
        ///   <c>true</c> if reset tags; otherwise, <c>false</c>.
        /// </value>
        public bool ResetTags
        {
            get
            {
                return configurationParameters.ResetTags;
            }
        }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets a value indicating whether terminate session if operation fails.
        /// </summary>
        /// <value>
        /// <c>true</c> if terminate session if operation fails; otherwise, <c>false</c>.
        /// </value>
        protected override bool TerminateOperation
        {
            get
            {
                return terminateOperation;
            }
            set
            {
                terminateOperation = value;
            }
        }

        #endregion Protected Properties

        #region Public Methods

        /// <summary>
        /// Connects to tag info provider and reads current batch number.
        /// </summary>
        /// <returns>The current batch Lot.</returns>
        public async Task<BatchStatistics> ConnectAndReadCurrentLot()
        {
            if (bsProxyInvoker == null)
            {
                bsProxyInvoker = ProxyInvokerUtils.CreateProxyInvoker<ITagInfoProvider>("TagInfoProviderEndPoint");
            }

            try
            {
                bsProxyInvoker.SetHeader("StationName", String.Empty, configurationParameters.StationName);
                bsProxyInvoker.CreateProxy();

                Task<BatchStatistics> StartLotBurningTask = null;
                await bsProxyInvoker.InvokeAsync(
                    (tagInfoProvider) =>
                    {
                        StartLotBurningTask = tagInfoProvider.StartLotBurningAsync();
                        return StartLotBurningTask;
                    });

                if (StartLotBurningTask.Status == TaskStatus.Faulted)
                {
                    return null;
                }

                return await StartLotBurningTask;
            }
            finally
            {
                bsProxyInvoker.RemoveProxy();
            }
        }

        /// <summary>
        /// Ends Lot burning.
        /// </summary>
        /// <returns></returns>
        public async Task<bool> EndLot()
        {
            bool lotEnded;
            if (ResetTags)
            {
                lotEnded = await SignVll();
            }
            else
            {
                Task EndLotTask = null;
                await bsProxyInvoker.InvokeAsync(tagInfoProvider =>
                    {
                        EndLotTask = tagInfoProvider.EndLotAsync();
                        return EndLotTask;
                    });

                if (EndLotTask.Status == TaskStatus.Faulted)
                {
                    return false;
                }

                await EndLotTask;
                lotEnded = String.IsNullOrEmpty(bsProxyInvoker.LastError);
            }

            if (lotEnded)
            {
                MessengerUtils.SendInfoMessage(Resources.EndOfLot);
                OnLotEnded(EventArgs.Empty);
            }

            return lotEnded;
        }

        /// <summary>
        /// Ends burning session.
        /// </summary>
        public void EndSession()
        {
            if (this.LotStatistics.CurrentSessionID != null)
            {
                this.LotStatistics.CurrentSessionID = null;
                this.LotStatistics.SessionBurnedCartridgesCount = 0;
                this.LotStatistics.SessionBurningAttemptCount = 0;
                currentSessionID++;
                OnSessionEnded(EventArgs.Empty);
            }
        }

        /// <summary>
        /// Starts Lot burning.
        /// </summary>
        /// <param name="lot">The lot.</param>
        /// <exception cref="System.ArgumentNullException">lot</exception>
        public void StartLot(BatchStatistics lot)
        {
            if (lot == null)
            {
                throw new ArgumentNullException("lot");
            }

            LotStatistics.MaterialInfo = lot.MaterialInfo;
            LotStatistics.BurnStartTime = lot.BurnStartTime ?? DateTime.Now;
            LotStatistics.BurnEndTime = lot.BurnEndTime;
            LotStatistics.LotBurnedCartridgesCount = lot.LotBurnedCartridgesCount;
            LotStatistics.LotBurningAttemptsCount = lot.LotBurningAttemptsCount;
            LotStatistics.LotFailuresCount = lot.LotFailuresCount;
            LotStatistics.HasReferenceTags = lot.HasReferenceTags;

            OnLotStarted(EventArgs.Empty);
        }

        /// <summary>
        /// Starts the burning session.
        /// </summary>
        public void StartSession()
        {
            if (LotStatistics.CurrentSessionID != currentSessionID)
            {
                LotStatistics.CurrentSessionID = currentSessionID;
                this.LotStatistics.SessionBurnedCartridgesCount = 0;
                this.LotStatistics.SessionBurningAttemptCount = 0;
                OnSessionStarted(EventArgs.Empty);
            }
        }

        #endregion Public Methods

        #region Protected Methods

        /// <summary>
        /// Burns material info into cartridge specified by cartridge number.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="tagInfo">The tag information burned on cartridge.</param>
        /// <returns>The tag information burned on cartridge.</returns>
        protected async virtual Task<TagInfo> Burn(byte cartridgeNumber)
        {
            MessengerUtils.SendInfoMessage(String.Format(Resources.BurningInProcess, cartridgeNumber));

            TagInfo tagInfo = null;
            bool completed = false;
            byte[] serialNumber;
            try
            {
                serialNumber = GetTagSerialNumber(cartridgeNumber);
                bool ok = (serialNumber != null);
                if (!ok)
                {
                    return tagInfo;
                }

                ok = await IsTagRegistrationValid(serialNumber);
                if (!ok)
                {
                    return tagInfo;
                }

                uint? weightToDecrease = await GetWeightToDecrease(serialNumber);
                if (!ok)
                {
                    return tagInfo;
                }

                CounterResponse counterResponse = InitTagCounter(cartridgeNumber, weightToDecrease.Value);
                if (counterResponse == null)
                {
                    return tagInfo;
                }

                Task<bool> VerifyTagCounterTask = Task<bool>.Run(() =>
                    {
                        return VerifyTagCounterResponse(counterResponse);
                    });

                Task<SignedObject<TagInfo>> GetSignedTagInfoTask = Task<SignedObject<TagInfo>>.Run(() =>
                    {
                        return GetSignedTagInfo(serialNumber);
                    });

                await Task.WhenAll(VerifyTagCounterTask, GetSignedTagInfoTask);

                ok = VerifyTagCounterTask.Result;
                if (!ok)
                {
                    return tagInfo;
                }

                SignedObject<TagInfo> signedTagInfo = GetSignedTagInfoTask.Result;
                ok = (signedTagInfo != null);
                if (!ok)
                {
                    tagInfo = new TagInfo
                    {
                        MaterialInfo = LotStatistics.MaterialInfo,
                        SerialNumber = serialNumber,
                        CurrentMaterialWeight = counterResponse.GetCounterValue(),
                        Error = bsProxyInvoker.LastError,
                    };
                    return tagInfo;
                }

                tagInfo = signedTagInfo.Object;
                ok = BurnSignedTagInfo(cartridgeNumber, signedTagInfo);
                if (!ok)
                {
                    return tagInfo;
                }

                completed = true;
                OnTagInfoRead(new TagInfoEventArgs(cartridgeNumber, tagInfo));
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
            finally
            {
                if (completed)
                {
                    MessengerUtils.SendInfoMessage(String.Format(Resources.BurningEndedSuccessfully, cartridgeNumber));
                }
                else
                {
                    MessengerUtils.SendErrorMessage(String.Format(Resources.BurningFailed, cartridgeNumber));
                }
            }

            return tagInfo;
        }

        /// <summary>
        /// Gets the actual IDT operation to be executed on cartrdge.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <returns></returns>
        protected override IdtOperation GetActualOperation(byte cartridgeNumber)
        {
            IdtOperationPrivileges operationPrivileges = configurationParameters.GetChassisOperations(cartridgeNumber);
            if (configurationParameters.StationType == BssStation.Inline)
            {
                if (operationPrivileges == IdtOperationPrivileges.V)
                {
                    return IdtOperation.Verification;
                }

                return IdtOperation.Burning;
            }

            if ((operationPrivileges & IdtOperationPrivileges.B) == IdtOperationPrivileges.B)
            {
                return IdtOperation.Burning;
            }

            return IdtOperation.Idle;
        }

        /// <summary>
        /// Gets the name of the IDT operation.
        /// </summary>
        /// <returns></returns>
        protected override string GetOperationName()
        {
            return Resources.BurningState;
        }

        /// <summary>
        /// Raises the <see cref="E:LotEnded" /> event.
        /// </summary>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        protected virtual void OnLotEnded(EventArgs e)
        {
            EventHandler temp = LotEnded;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:LotStarted" /> event.
        /// </summary>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        protected virtual void OnLotStarted(EventArgs e)
        {
            EventHandler temp = LotStarted;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:SessionEnded" /> event.
        /// </summary>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        protected virtual void OnSessionEnded(EventArgs e)
        {
            EventHandler temp = SessionEnded;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:SessionStarted" /> event.
        /// </summary>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        protected virtual void OnSessionStarted(EventArgs e)
        {
            EventHandler temp = SessionStarted;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Burns material info into cartridge specified by its number.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <exception cref="System.InvalidOperationException">No material info was read.</exception>
        protected override async Task<bool> Operate(byte cartridgeNumber)
        {
            if (configurationParameters.StationType == BssStation.Inline)
            {
                IdtOperationPrivileges allowedOperation = configurationParameters.GetChassisOperations(cartridgeNumber);
                if (allowedOperation == IdtOperationPrivileges.V)
                {
                    return await base.Operate(cartridgeNumber);
                }
            }

            ValidateChassisMaterialType(cartridgeNumber, LotStatistics.MaterialInfo);
            await UpdateTrafficLight(cartridgeNumber, inserted: true).ConfigureAwait(continueOnCapturedContext: false);

            TerminateOperation = false;
            bool ok = false;
            if (ResetTags)
            {
                ok = ResetCartridge(cartridgeNumber);
                UpdateCounters(ok);
            }
            else
            {
                try
                {
                    TagInfo tagInfo = null;
                    bsProxyInvoker.CreateProxy();
                    tagInfo = await Burn(cartridgeNumber);
                    ok = tagInfo != null && !tagInfo.HasError;
                    if (ok)
                    {
                        OnOperationEnded(new IdtOperationEventArgs(IdtOperation.Burning, cartridgeNumber));

                        OnOperationStarted(new IdtOperationEventArgs(IdtOperation.Verification, cartridgeNumber));
                        TagInfo ti = await ReadAndValidate(cartridgeNumber);
                        ok = ti != null && !ti.HasError;
                        tagInfo.Error = ti.Error;

                        OnOperationEnded(new IdtOperationEventArgs(IdtOperation.Verification, cartridgeNumber));
                    }

                    if (tagInfo != null &&
                        tagInfo.MaterialInfo != null)
                    {
                        bool archiveOk = await AddBurningActionToHistory(tagInfo);
                        TerminateOperation = !archiveOk;
                    }
                }
                finally
                {
                    MessengerUtils.SendInfoMessage(String.Format(Resources.BurningEnded, cartridgeNumber));
                    bsProxyInvoker.RemoveProxy();
                }
            }

            await TurnTrafficLightsOn(cartridgeNumber, ok).ConfigureAwait(continueOnCapturedContext: false);
            return ok;
        }

        /// <summary>
        /// Erases cartridge information.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <returns><c>true</c> if operation succeeded; otherwise, <c>false</c>.</returns>
        protected bool ResetCartridge(byte cartridgeNumber)
        {
            MessengerUtils.SendInfoMessage(String.Format(Resources.BurningInProcess, cartridgeNumber));

            bool ok = false;
            try
            {
                byte[] pubKC = null;
                int err = materialMonitor.ResetCartridge(cartridgeNumber, configurationParameters.DevicePassword);
                if (err != 0)
                {
                    MessengerUtils.SendErrorMessage(IdtErrorInterpreter.Interpret(err));
                    return ok;
                }

                byte[] serialNumber = GetTagSerialNumber(cartridgeNumber);
                if (serialNumber == null)
                {
                    return ok;
                }

                err = materialMonitor.ReadTagPublicKey(cartridgeNumber, out pubKC);
                if (err != 0)
                {
                    MessengerUtils.SendErrorMessage(IdtErrorInterpreter.Interpret(err));
                    return ok;
                }

                using (StreamWriter sw = new StreamWriter(VllFileName, append: true))
                {
                    sw.WriteLine(String.Join(",", DateTime.Now.ToShortDateString(), serialNumber.ToHexString(), pubKC.ToHexString()));
                }

                ok = true;
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
            finally
            {
                if (ok)
                {
                    MessengerUtils.SendInfoMessage(String.Format(Resources.BurningEndedSuccessfully, cartridgeNumber));
                }
                else
                {
                    MessengerUtils.SendErrorMessage(String.Format(Resources.BurningFailed, cartridgeNumber));
                }
            }

            return ok;
        }

        /// <summary>
        /// Stops active IDT operations on removed cartridges and turn off traffic lights.
        /// </summary>
        /// <param name="removedCartridges">Numbers of the removed cartridges.</param>
        /// <returns></returns>
        protected override async Task StopIdtOperations(byte[] removedCartridges)
        {
            await base.StopIdtOperations(removedCartridges);
            if (inPlaceManager.InPlaceStatus == 0 &&
                TerminateOperation)
            {
                EndSession();
            }
        }

        #endregion Protected Methods

        #region Burning Steps

        protected async Task<bool> AddBurningActionToHistory(TagInfo tagInfo)
        {
            MessengerUtils.SendDebugMessage(Resources.AddBurningActionToHistory);

            bool burningOk = String.IsNullOrEmpty(tagInfo.Error);

            LotStatistics.MaterialInfo = tagInfo.MaterialInfo;

            TagStatistics tagStatistics = new TagStatistics(tagInfo);
            tagStatistics.BatchStatistics = LotStatistics;
            tagStatistics.StationName = configurationParameters.StationName;
            tagStatistics.BurningTime = DateTime.Now;
            tagStatistics.LastError = tagInfo.Error;
            tagStatistics.IsReference = IsReference;

            Task AddTagBurningToHistoryTask = null;
            await bsProxyInvoker.InvokeAsync(tagInfoProvider =>
                {
                    AddTagBurningToHistoryTask = tagInfoProvider.AddTagBurningToHistoryAsync(tagStatistics);
                    return AddTagBurningToHistoryTask;
                });

            bool archiveOk = AddTagBurningToHistoryTask.Status != TaskStatus.Faulted;
            if (archiveOk)
            {
                await AddTagBurningToHistoryTask;
            }

            UpdateCounters(success: burningOk && archiveOk);
            return archiveOk;
        }

        /// <summary>
        /// Burns the signed tag information.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="signedTagInfo">The signed tag information.</param>
        /// <returns><c>true</c> if signed tag information was burned successfully; otherwise, <c>false</c>.</returns>
        protected bool BurnSignedTagInfo(byte cartridgeNumber, SignedObject<TagInfo> signedTagInfo)
        {
            MessengerUtils.SendDebugMessage(Resources.BurnSignedTagInfo);

            byte[] buffer = signedTagInfo.Object.Encode();
            int err = materialMonitor.BurnSignedIdd(cartridgeNumber, buffer, signedTagInfo.Signature);
            if (err != 0)
            {
                signedTagInfo.Object.Error = IdtErrorInterpreter.Interpret(err);
                MessengerUtils.SendErrorMessage(signedTagInfo.Object.Error);
                return false;
            }

            return true;
        }

        /// <summary>
        /// Gets the signed tag information.
        /// </summary>
        /// <param name="serialNumber">The serial number.</param>
        /// <returns>The signed tag information.</returns>
        protected async Task<SignedObject<TagInfo>> GetSignedTagInfo(byte[] serialNumber)
        {
            MessengerUtils.SendDebugMessage(Resources.GetSignedTagInfo);

            Task<SignedObject<TagInfo>> BuildTagInfoTask = null;
            await bsProxyInvoker.InvokeAsync(
                tagInfoProvider =>
                {
                    BuildTagInfoTask = tagInfoProvider.BuildTagInfoAsync(serialNumber);
                    return BuildTagInfoTask;
                });

            if (BuildTagInfoTask.Status == TaskStatus.Faulted)
            {
                return null;
            }

            return await BuildTagInfoTask;
        }

        /// <summary>
        /// Gets the tag serial number.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <returns>The tag serial number.</returns>
        protected virtual byte[] GetTagSerialNumber(byte cartridgeNumber)
        {
            MessengerUtils.SendDebugMessage(Resources.GetTagSerialNumber);

            byte[] serialNumber;
            int err = materialMonitor.GetDeviceSerialNumber(cartridgeNumber, out serialNumber);
            if (err != 0)
            {
                MessengerUtils.SendErrorMessage(IdtErrorInterpreter.Interpret(err));
                return null;
            }

            return serialNumber;
        }

        /// <summary>
        /// Gets the weight to decrease from tag counter initial value.
        /// </summary>
        /// <param name="serialNumber">The serial number.</param>
        /// <returns>The weight to decrease if operation succeeded; otherwise, <c>null</c>.</returns>
        protected async virtual Task<uint?> GetWeightToDecrease(byte[] serialNumber)
        {
            MessengerUtils.SendDebugMessage(Resources.GetTagSerialNumber);

            Task<uint> GetWeightToDecreaseTask = null;
            await bsProxyInvoker.InvokeAsync(
                tagInfoProvider =>
                {
                    GetWeightToDecreaseTask = tagInfoProvider.GetWeightToDecreaseAsync(serialNumber);
                    return GetWeightToDecreaseTask;
                });

            if (GetWeightToDecreaseTask.Status == TaskStatus.Faulted)
            {
                return null;
            }

            return await GetWeightToDecreaseTask;
        }

        /// <summary>
        /// Initializes the tag counter.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="weightToDecrease">The weight to decrease.</param>
        /// <returns>New counter value if operation succeeded; <c>null</c> if failed.</returns>
        protected CounterResponse InitTagCounter(byte cartridgeNumber, uint weightToDecrease)
        {
            MessengerUtils.SendDebugMessage(Resources.InitTagCounter);

            byte[] counterResponse, counterSignature;
            int err = materialMonitor.UpdateConsumption(cartridgeNumber, weightToDecrease, out counterResponse, out counterSignature);
            if (err != 0)
            {
                MessengerUtils.SendErrorMessage(IdtErrorInterpreter.Interpret(err));
                return null;
            }            

            return new CounterResponse(counterResponse, counterSignature);
        }

        /// <summary>
        /// Determines whether tag registration is valid.
        /// </summary>
        /// <param name="serialNumber">The serial number.</param>
        /// <returns><c>true</c> if tag registration is valid for burning; otherwise, <c>false</c>.</returns>
        protected async Task<bool> IsTagRegistrationValid(byte[] serialNumber)
        {
            MessengerUtils.SendDebugMessage(Resources.IsTagRegistrationValid);

            Task<TagRegistration> GetTagRegistrationTask = null;
            await bsProxyInvoker.InvokeAsync(
                tagInfoProvider =>
                {
                    GetTagRegistrationTask = tagInfoProvider.GetTagRegistrationAsync(serialNumber);
                    return GetTagRegistrationTask;
                });

            if (GetTagRegistrationTask.Status == TaskStatus.Faulted)
            {
                return false;
            }

            TagRegistration tagRegistration = await GetTagRegistrationTask;
            bool isValid = IsTagRegistrationValid(tagRegistration);
            if (!isValid)
            {
                string message;
                switch (tagRegistration)
                {
                    case TagRegistration.Unknown:
                        message = Resources.TagNotFound;
                        break;

                    case TagRegistration.Used:
                        message = Resources.TagUsed;
                        break;

                    default:
                        message = null;
                        break;
                }

                if (message != null)
                {
                    MessengerUtils.SendErrorMessage(String.Format(message, serialNumber.ToHexString()));
                }

                return false;
            }

            return true;
        }

        /// <summary>
        /// Determines whether tag registration is valid.
        /// </summary>
        /// <param name="TagRegistration">The tag registration value.</param>
        /// <returns><c>true</c> if tag registration is new or read; otherwise, <c>false</c>.</returns>
        protected virtual bool IsTagRegistrationValid(TagRegistration TagRegistration)
        {
            return TagRegistration == TagRegistration.New;
        }

        /// <summary>
        /// Signs the tags list using HSM.
        /// </summary>
        /// <returns></returns>
        protected async Task<bool> SignVll()
        {
            MessengerUtils.SendDebugMessage(Resources.SignVll);

            try
            {
                string fileContent;
                using (StreamReader sr = new StreamReader(VllFileName))
                {
                    fileContent = sr.ReadToEnd();
                }

                using (StreamWriter sw = new StreamWriter(VllFileName, append: true))
                {
                    byte[] signature = null;
                    byte[] message = new byte[(fileContent.Length - Environment.NewLine.Length) * sizeof(char)];
                    BinaryWriter bw = new BinaryWriter(new MemoryStream(message));
                    bw.Write(fileContent.ToCharArray(), 0, fileContent.Length - Environment.NewLine.Length);
                    using (ProxyInvoker<IHsm> hsmInvoker = new ProxyInvoker<IHsm>("HsmEndPoint"))
                    {
                        hsmInvoker.CreateProxy();
                        Task<byte[]> SignDataTask = null;
                        bool isConnected = await hsmInvoker.InvokeAsync((hsm) =>
                            {
                                SignDataTask = hsm.SignDataAsync(message);
                                return SignDataTask;
                            });

                        if (SignDataTask.Status != TaskStatus.Faulted)
                        {
                            signature = await SignDataTask.ConfigureAwait(continueOnCapturedContext: false);
                        }
                    }

                    if (signature == null)
                    {
                        MessengerUtils.SendException(new InvalidOperationException(Resources.SignatureError));
                        return false;
                    }

                    sw.WriteLine(String.Join(",", DateTime.Now.ToShortDateString(), "Signature", signature.ToHexString()));
                }

                return true;
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
                return false;
            }
        }

        /// <summary>
        /// Checks that insertion of container into chassis is allowed.
        /// </summary>
        /// <param name="chassisNumber">The chassis number.</param>
        /// <param name="materialInfo">The material information.</param>
        protected virtual void ValidateChassisMaterialType(byte chassisNumber, MaterialInfo materialInfo)
        {
            if (materialInfo == null)
            {
                throw new ArgumentNullException("materialInfo");
            }

            MaterialKind allowedGroup = configurationParameters.GetChassisMaterialGroup(chassisNumber);
            if ((allowedGroup & materialInfo.MaterialKind) != materialInfo.MaterialKind)
            {
                string localizedMaterialKind = String.Empty;
                if (materialInfo.MaterialKind == MaterialKind.M)
                {
                    localizedMaterialKind = Resources.Model;
                }
                else if (materialInfo.MaterialKind == MaterialKind.S)
                {
                    localizedMaterialKind = Resources.Support;
                }

                throw new InvalidOperationException(String.Format(Resources.MaterialTypeError, localizedMaterialKind, chassisNumber));
            }
        }

        /// <summary>
        /// Verifies that tag counter value equals to the expected value and counter signature is valid.
        /// </summary>
        /// <param name="counterResponse">Tag counter response.</param>
        /// <returns><c>true</c> if values are equal and validated; otherwise, <c>false</c>.</returns>
        protected async Task<bool> VerifyTagCounterResponse(CounterResponse counterResponse)
        {
            MessengerUtils.SendDebugMessage(Resources.SendingCounterForVerification);

            Task<bool> VerifyTagCounterResponseTask = null;
            await bsProxyInvoker.InvokeAsync(
                tagInfoProvider =>
                {
                    VerifyTagCounterResponseTask = tagInfoProvider.VerifyCounterAsync(counterResponse);
                    return VerifyTagCounterResponseTask;
                });

            if (VerifyTagCounterResponseTask.Status == TaskStatus.Faulted)
            {
                return false;
            }

            return await VerifyTagCounterResponseTask;
        }

        #endregion Burning Steps

        #region Private Methods

        private void UpdateCounters(bool success)
        {
            LotStatistics.HasReferenceTags |= IsReference;
            LotStatistics.LotBurningAttemptsCount++;
            LotStatistics.SessionBurningAttemptCount++;

            if (success)
            {
                LotStatistics.SessionBurnedCartridgesCount++;
                LotStatistics.LotBurnedCartridgesCount++;
            }
            else
            {
                LotStatistics.LotFailuresCount++;
            }
        }

        #endregion Private Methods
    }
}