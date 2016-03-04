using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.Plc;
using BSS.MVVM.Properties;
using RaccoonCLI;
using System;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    public enum IdtOperation
    {
        Idle,
        Verification,
        Burning,
        Correction
    }

    [Flags]
    public enum IdtOperationPrivileges
    {
        /// <summary>
        /// No operation.
        /// </summary>
        None = 0,

        /// <summary>
        /// Verification.
        /// </summary>
        V = 1,

        /// <summary>
        /// Burning.
        /// </summary>
        B = 2,

        /// <summary>
        /// Burning and verification.
        /// </summary>
        BV = B | V,
    }

    /// <summary>
    /// Resposible to operate IDT when cartridge is inserted.
    /// </summary>
    public abstract class IdtOperator : IDisposable
    {
        #region Protected Fields

        /// <summary>
        /// A reference to the states manager.
        /// </summary>
        protected ConfigurationParameters configurationParameters;

        /// <summary>
        /// A dispatcher for IDT operations.
        /// </summary>
        protected ActionsDispatcher<byte> idtOperationDispatcher;

        /// <summary>
        /// A reference to the in-place manager.
        /// </summary>
        protected InPlaceManager inPlaceManager;

        /// <summary>
        /// A reference to the material monitor wrapper.
        /// </summary>
        protected MaterialMonitorWrapper materialMonitor;

        /// <summary>
        /// A reference to the PLC wrapper.
        /// </summary>
        protected IPlc plcWrapper;

        #endregion Protected Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtOperator"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="inPlaceManager">The in place manager.</param>
        /// <param name="materialMonitor">The material monitor.</param>
        /// <param name="plcWrapper">The PLC wrapper.</param>
        /// <exception cref="System.ArgumentNullException">
        /// configurationParameters
        /// or
        /// inPlaceManager
        /// or
        /// materialMonitor
        /// or
        /// plcWrapper
        /// </exception>
        public IdtOperator(ConfigurationParameters configurationParameters, InPlaceManager inPlaceManager, MaterialMonitorWrapper materialMonitor, IPlc plcWrapper)
        {
            if (configurationParameters == null)
            {
                throw new ArgumentNullException("configurationParameters");
            }

            if (inPlaceManager == null)
            {
                throw new ArgumentNullException("inPlaceManager");
            }

            if (materialMonitor == null)
            {
                throw new ArgumentNullException("materialMonitor");
            }

            if (plcWrapper == null)
            {
                throw new ArgumentNullException("plcWrapper");
            }

            this.configurationParameters = configurationParameters;
            this.inPlaceManager = inPlaceManager;
            this.materialMonitor = materialMonitor;
            this.plcWrapper = plcWrapper;

            idtOperationDispatcher = new ActionsDispatcher<byte>();
        }

        #endregion Public Constructors

        #region Public Events

        /// <summary>
        /// Occurs when operation ended.
        /// </summary>
        public event EventHandler<IdtOperationEventArgs> OperationEnded;

        /// <summary>
        /// Occurs when operation started.
        /// </summary>
        public event EventHandler<IdtOperationEventArgs> OperationStarted;

        /// <summary>
        /// Occurs when traffic light color is changed.
        /// </summary>
        public event EventHandler<TrafficLightColorChangedEventArgs> TrafficLightColorChanged;

        #endregion Public Events

        #region Public Properties

        /// <summary>
        /// Gets or sets the cartridges count.
        /// </summary>
        /// <value>
        /// The cartridges count.
        /// </value>
        public byte CartridgesCount
        {
            get
            {
                return inPlaceManager.CartridgesCount;
            }
        }

        /// <summary>
        /// Gets a value indicating whether last operation succeeded or failed.
        /// </summary>
        /// <value>
        ///   <c>true</c> if last operation succeeded; otherwise, <c>false</c>.
        /// </value>
        public bool LastOperationResult { get; private set; }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets a value indicating whether terminate session if operation fails.
        /// </summary>
        /// <value>
        ///   <c>true</c> if terminate session if operation fails; otherwise, <c>false</c>.
        /// </value>
        protected virtual bool TerminateOperation
        {
            get
            {
                return false;
            }
            set
            {
            }
        }

        #endregion Protected Properties

        #region Public Methods

        /// <summary>
        /// Gets the in-place status.
        /// </summary>
        /// <value>
        /// The in-place status.
        /// </value>
        public async Task<TrafficLightColor[]> GetTrafficLightColors()
        {
            TrafficLightColor[] trafficLightColors = new TrafficLightColor[CartridgesCount];

            try
            {
                for (byte i = 0; i < trafficLightColors.Length; i++)
                {
                    trafficLightColors[i] = await plcWrapper.GetTrafficLightColor(Convert.ToByte(i + 1))
                        .ConfigureAwait(continueOnCapturedContext: false);
                }
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }

            return trafficLightColors;
        }

        /// <summary>
        /// Returns a value indicating whether IDT is currently being operated.
        /// </summary>
        public bool IsBusy()
        {
            return idtOperationDispatcher.IsBusy();
        }

        /// <summary>
        /// Starts listening to changes of in-place status.
        /// </summary>
        public void StartListenInPlaceStatusChanges()
        {
            inPlaceManager.InPlaceStatusChanged += HandleInPlaceStatusChanged;
        }

        /// <summary>
        /// Stops all active operations.
        /// </summary>
        public void StopAll()
        {
            idtOperationDispatcher.Clear();
            StopListenInPlaceStatusChanges();
        }

        /// <summary>
        /// Starts listening to changes of in-place status.
        /// </summary>
        public void StopListenInPlaceStatusChanges()
        {
            inPlaceManager.InPlaceStatusChanged -= HandleInPlaceStatusChanged;
            try
            {
                if (plcWrapper.IsReady())
                {
                    plcWrapper.Reset();
                }
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
        }

        #endregion Public Methods

        #region Protected Methods

        /// <summary>
        /// Gets the changes of in-place status.
        /// </summary>
        /// <param name="oldInPlaceStatus">The old in-place status.</param>
        /// <param name="newInPlaceStatus">The new in-place status.</param>
        /// <param name="insertedCartridges">The inserted cartridges.</param>
        /// <param name="removedCartridges">The removed cartridges.</param>
        protected void GetInPlaceChanges(int oldInPlaceStatus, int newInPlaceStatus, out byte[] insertedCartridges, out byte[] removedCartridges)
        {
            List<byte> insertedCartridgesList = new List<byte>(), removedCartridgesList = new List<byte>();

            int mask = 1;
            for (byte b = 1; b <= CartridgesCount; b++)
            {
                int diff = (newInPlaceStatus & mask) - (oldInPlaceStatus & mask);
                if (diff > 0)
                {
                    insertedCartridgesList.Add(b);
                }
                else if (diff < 0)
                {
                    removedCartridgesList.Add(b);
                }

                mask <<= 1;
            }

            insertedCartridges = insertedCartridgesList.ToArray();
            removedCartridges = removedCartridgesList.ToArray();
        }

        /// <summary>
        /// Gets the actual IDT operation to be executed on cartrdge.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <returns></returns>
        protected abstract IdtOperation GetActualOperation(byte cartridgeNumber);

        /// <summary>
        /// Gets the name of the IDT operation.
        /// </summary>
        /// <returns></returns>
        protected abstract string GetOperationName();

        /// <summary>
        /// Raises the <see cref="E:OperationEnded"/> event.
        /// </summary>
        /// <param name="e">The <see cref="BSS.MVVM.Model.BusinessLogic.IdtSrv.IdtOperationEventArgs"/> instance containing the event data.</param>
        protected virtual void OnOperationEnded(IdtOperationEventArgs e)
        {
            EventHandler<IdtOperationEventArgs> temp = OperationEnded;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:OperationStarted"/> event.
        /// </summary>
        /// <param name="e">The <see cref="BSS.MVVM.Model.BusinessLogic.IdtSrv.IdtOperationEventArgs"/> instance containing the event data.</param>
        protected virtual void OnOperationStarted(IdtOperationEventArgs e)
        {
            EventHandler<IdtOperationEventArgs> temp = OperationStarted;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// Raises the <see cref="E:TrafficLightColorChanged"/> event.
        /// </summary>
        /// <param name="e">The <see cref="TrafficLightColorChangedEventArgs"/> instance containing the event data.</param>
        protected virtual void OnTrafficLightColorChanged(TrafficLightColorChangedEventArgs e)
        {
            EventHandler<TrafficLightColorChangedEventArgs> temp = TrafficLightColorChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        /// <summary>
        /// When overriden in a derived class, operates on a cartridge specified by its number.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        protected abstract Task<bool> Operate(byte cartridgeNumber);

        /// <summary>
        /// Operates on a cartridge specified by its number and notifies before and after operation.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        protected void OperateAndNotify(byte cartridgeNumber)
        {
            IdtOperation actualOperation = GetActualOperation(cartridgeNumber);
            IdtOperationEventArgs e = new IdtOperationEventArgs(actualOperation, cartridgeNumber);
            OnOperationStarted(e);
            inPlaceManager.StopGetInPlaceStatus();

            LastOperationResult = false;
            try
            {
                if (actualOperation != IdtOperation.Idle)
                {
                    Task<bool> operateTask = Operate(cartridgeNumber);
                    operateTask.ConfigureAwait(continueOnCapturedContext: false);
                    LastOperationResult = operateTask.Result;
                }
                else
                {
                    string operationText = GetOperationName();
                    MessengerUtils.SendErrorMessage(
                        String.Format(Resources.OperationNotAllowedOnCartridge,
                        operationText,
                        cartridgeNumber));
                }
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }

            inPlaceManager.StartGetInPlaceStatus();
            OnOperationEnded(e);
        }

        /// <summary>
        /// Starts IDT operation on inserted cartridges.
        /// </summary>
        /// <param name="insertedCartridges">Numbers of the inserted cartridges.</param>
        /// <exception cref="ArgumentNullException">insertedCartridges</exception>
        protected virtual void StartIdtOperations(byte[] insertedCartridges)
        {
            if (insertedCartridges == null)
            {
                throw new ArgumentNullException("insertedCartridges");
            }

            foreach (byte insertedCartridgeNum in insertedCartridges)
            {
                idtOperationDispatcher.Dispatch(OperateAndNotify, insertedCartridgeNum);
            }
        }

        /// <summary>
        /// Stops active IDT operations on removed cartridges and turn off traffic lights.
        /// </summary>
        /// <param name="removedCartridges">Numbers of the removed cartridges.</param>
        /// <returns></returns>
        /// <exception cref="ArgumentNullException">removedCartridges</exception>
        protected virtual async Task StopIdtOperations(byte[] removedCartridges)
        {
            if (removedCartridges == null)
            {
                throw new ArgumentNullException("removedCartridges");
            }

            foreach (byte removedCartridgeNum in removedCartridges)
            {
                idtOperationDispatcher.CancelDispatch(OperateAndNotify, removedCartridgeNum);
                await UpdateTrafficLight(removedCartridgeNum, inserted: false);
            }
        }

        /// <summary>
        /// Turns the green or red lights on according to ok flag.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="ok">if set to <c>true</c> turn on green light; otherwise, turn on red light.</param>
        protected async Task TurnTrafficLightsOn(byte cartridgeNumber, bool ok)
        {
            TrafficLightColor color = (ok) ? TrafficLightColor.Green : TrafficLightColor.Red;
            try
            {
                await plcWrapper.SetTrafficLightColor(cartridgeNumber, color).ConfigureAwait(continueOnCapturedContext: false);
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }

            OnTrafficLightColorChanged(new TrafficLightColorChangedEventArgs(cartridgeNumber, color));
        }

        #endregion Protected Methods

        #region Private Methods

        /// <summary>
        /// Updates the traffic light.
        /// </summary>
        /// <param name="trafficLightNumber">The traffic light number.</param>
        /// <param name="inserted">if set to <c>true</c>, turn light on.</param>
        protected async Task UpdateTrafficLight(byte trafficLightNumber, bool inserted)
        {
            TrafficLightColor color;
            string message;
            if (inserted)
            {
                color = TrafficLightColor.Yellow;
                message = String.Format(Resources.CartridgeXInserted, trafficLightNumber);
            }
            else
            {
                color = TrafficLightColor.Off;
                message = String.Format(Resources.CartridgeXRemoved, trafficLightNumber);
            }

            MessengerUtils.SendInfoMessage(message);
            try
            {
                await plcWrapper.SetTrafficLightColor(trafficLightNumber, color).ConfigureAwait(continueOnCapturedContext: false);
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }

            OnTrafficLightColorChanged(new TrafficLightColorChangedEventArgs(trafficLightNumber, color));
        }

        /// <summary>
        /// Handles the <see cref="E:BSS.MVVM.Model.BusinessLogic.IdtSrv.InPlaceManager.InPlaceStatusChanged"/> event:
        /// updates traffic lights and dispatches IDT operations.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="InPlaceStatusChangedEventArgs"/> instance containing the event data.</param>
        private async void HandleInPlaceStatusChanged(object sender, InPlaceStatusChangedEventArgs e)
        {
            byte[] insertedCartridges, removedCartridges;
            GetInPlaceChanges(e.OldInPlaceStatus, e.NewInPlaceStatus, out insertedCartridges, out removedCartridges);
            StartIdtOperations(insertedCartridges);
            await StopIdtOperations(removedCartridges);
        }

        #endregion Private Methods

        #region IDisposable Members

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(dispose: true);
        }

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected virtual void Dispose(bool dispose)
        {
            if (dispose)
            {
                StopAll();
            }
        }

        #endregion IDisposable Members
    }
}