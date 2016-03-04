using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.Plc;
using BSS.MVVM.Properties;
using RaccoonCLI;
using System;
using System.Threading.Tasks;
using WcfInfras.Client;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Responsible for reading material info from IDT when cartridge is inserted.
    /// </summary>
    public class IdtReader : IdtOperator
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtReader"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="inPlaceManager">The in place manager.</param>
        /// <param name="materialMonitor">The material monitor.</param>
        /// <param name="plcWrapper">The PLC wrapper.</param>
        public IdtReader(ConfigurationParameters configurationParameters, InPlaceManager inPlaceManager, MaterialMonitorWrapper materialMonitor, IPlc plcWrapper)
            : base(configurationParameters, inPlaceManager, materialMonitor, plcWrapper)
        {
        }

        #endregion Public Constructors

        #region Public Events

        /// <summary>
        /// Occurs when material information is read.
        /// </summary>
        public event EventHandler<TagInfoEventArgs> TagInfoRead;

        #endregion Public Events

        #region Protected Methods

        /// <summary>
        /// Gets the actual IDT operation to be executed on cartrdge.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <returns></returns>
        protected override IdtOperation GetActualOperation(byte cartridgeNumber)
        {
            IdtOperationPrivileges operationPrivileges = configurationParameters.GetChassisOperations(cartridgeNumber);
            if ((operationPrivileges & IdtOperationPrivileges.V) == IdtOperationPrivileges.V)
            {
                return IdtOperation.Verification;
            }

            return IdtOperation.Idle;
        }

        /// <summary>
        /// Gets the name of the IDT operation.
        /// </summary>
        /// <returns></returns>
        protected override string GetOperationName()
        {
            return Resources.VerificationState;
        }

        /// <summary>
        /// Raises the <see cref="E:MaterialInfoRead"/> event.
        /// </summary>
        /// <param name="e">The <see cref="TagInfoEventArgs"/> instance containing the event data.</param>
        protected virtual void OnTagInfoRead(TagInfoEventArgs e)
        {
            EventHandler<TagInfoEventArgs> temp = TagInfoRead;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Reads material info from cartridge specified by its number.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        protected override async Task<bool> Operate(byte cartridgeNumber)
        {
            await UpdateTrafficLight(cartridgeNumber, inserted: true).ConfigureAwait(continueOnCapturedContext: false);

            TagInfo tagInfo = await ReadAndValidate(cartridgeNumber);
            bool ok = tagInfo != null && !tagInfo.HasError;
            await TurnTrafficLightsOn(cartridgeNumber, ok).ConfigureAwait(continueOnCapturedContext: false);

            if (tagInfo != null)
            {
                OnTagInfoRead(new TagInfoEventArgs(cartridgeNumber, tagInfo));
            }

            return ok;
        }

        /// <summary>
        /// Reads tag information from a specified cartridge and validates it.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="tagInfo">The tag information.</param>
        /// <returns>The tag information.</returns>
        protected async virtual Task<TagInfo> ReadAndValidate(byte cartridgeNumber)
        {
            MessengerUtils.SendInfoMessage(String.Format(Resources.ReadingInProcess, cartridgeNumber));
            TagInfo tagInfo = null;
            bool ok;

            try
            {
                tagInfo = await AuthenticateCartridge(cartridgeNumber);
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
            finally
            {
                ok = tagInfo != null && !tagInfo.HasError;
                if (ok)
                {
                    MessengerUtils.SendInfoMessage(String.Format(Resources.ReadingEndedSuccessfully, cartridgeNumber));
                }
                else
                {
                    MessengerUtils.SendErrorMessage(String.Format(Resources.ReadingFailed, cartridgeNumber));
                }
            }

            return tagInfo;
        }

        #endregion Protected Methods

        #region ReadAndValidate Steps

        /// <summary>
        /// Authenticates cartridge identification data.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="tagInfo">The tag information if authnetication is ok; otherwise, an empty tag information.</param>
        /// <returns><c>true</c> if cartridge was authenticated; otherwies, <c>false</c>.</returns>
        protected async Task<TagInfo> AuthenticateCartridge(byte cartridgeNumber)
        {
            MaterialInfo materialInfo = new MaterialInfo();
            TagInfo tagInfo = new TagInfo
            {
                MaterialInfo = materialInfo
            };

            byte[] idd;
            uint currentMaterialWeight;
            int err = materialMonitor.AuthenticateCartridge(cartridgeNumber, out idd, out currentMaterialWeight);
            tagInfo.CurrentMaterialWeight = currentMaterialWeight;

            if (err != 0)
            {
                tagInfo.MaterialInfo = null;
                tagInfo.Error = IdtErrorInterpreter.Interpret(err);

                byte[] serialNumber;
                err = materialMonitor.GetDeviceSerialNumber(cartridgeNumber, out serialNumber);
                if (err == 0)
                {
                    tagInfo.SerialNumber = serialNumber;
                }
            }
            else
            {
                try
                {
                    tagInfo.Decode(idd);
                    using (ProxyInvoker<IReport> reportProxyInvoker = new ProxyInvoker<IReport>("ReportEndPoint"))
                    {
                        reportProxyInvoker.CreateProxy();
                        Task<string> GetMaterialNameAsyncTask = null;
                        await reportProxyInvoker.InvokeAsync((reportProxy) =>
                            {
                                GetMaterialNameAsyncTask = reportProxy.GetMaterialNameAsync(materialInfo.MaterialID);
                                return GetMaterialNameAsyncTask;
                            });

                        if (GetMaterialNameAsyncTask.Status != TaskStatus.Faulted)
                        {
                            tagInfo.MaterialInfo.MaterialName = await GetMaterialNameAsyncTask;
                        }
                    }
                }
                catch (Exception ex)
                {
                    tagInfo.Error = ex.Message;
                }
            }

            if (!String.IsNullOrEmpty(tagInfo.Error))
            {
                MessengerUtils.SendErrorMessage(tagInfo.Error);
            }

            return tagInfo;
        }

        #endregion ReadAndValidate Steps
    }
}