using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Model.BusinessLogic.Plc;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Views;
using System;
using System.Collections.ObjectModel;
using System.Threading.Tasks;

namespace BSS.MVVM.ViewModel
{
    /// <summary>
    /// Holds common functionality for all view-models of IDT modes.
    /// </summary>
    public abstract class IdtOperationViewModel : BSSViewModel
    {
        #region Protected Fields

        protected CommunicationsManager communicationsManager;

        protected byte? currentCartridgeNumber;

        /// <summary>
        /// <c>true</c> if IDT operator is busy.
        /// </summary>
        private bool isBusy;

        #endregion Protected Fields

        #region Protected Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtOperationViewModel{T}"/> class.
        /// </summary>
        /// <param name="idtOperator">The IDT operator.</param>
        /// <param name="communicationsManager">The communication manager.</param>
        /// <param name="dialogService">The dialog service.</param>
        /// <exception cref="System.ArgumentNullException">idtOperator</exception>
        protected IdtOperationViewModel(IdtOperator idtOperator, CommunicationsManager communicationsManager, IDialogService dialogService)
            : base(dialogService)
        {
            if (idtOperator == null)
            {
                throw new ArgumentNullException("idtOperator");
            }

            this.IdtStatus = new ObservableCollection<IdtStatusViewModel>();

            this.communicationsManager = communicationsManager;
            this.communicationsManager.CommunicationChanged += HandleCommunicationChanged;

            this.IdtOperator = idtOperator;
            this.IdtOperator.OperationStarted += HandleOperationStarted;
            this.IdtOperator.OperationEnded += HandleOperationEnded;
            this.IdtOperator.TrafficLightColorChanged += HandleTrafficLightColorChanged;
        }

        #endregion Protected Constructors

        #region Public Properties

        /// <summary>
        /// Gets the status of all IDTs.
        /// </summary>
        /// <value>
        /// The status of all IDTs.
        /// </value>
        public ObservableCollection<IdtStatusViewModel> IdtStatus
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets or sets a value indicating whether IDT operator is busy.
        /// </summary>
        /// <value>
        ///   <c>true</c> if IDT operator is busy.
        /// </value>
        public bool IsBusy
        {
            get
            {
                return isBusy;
            }
            protected set
            {
                isBusy = value;
                RaisePropertyChanged(() => IsBusy);
            }
        }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets the IDT operator.
        /// </summary>
        protected IdtOperator IdtOperator
        {
            get;
            set;
        }

        /// <summary>
        /// When overriden in derived class, gets the operation progress step.
        /// </summary>
        /// <value>
        /// The step.
        /// </value>
        protected abstract sbyte Step { get; }

        #endregion Protected Properties

        #region Public Methods

        /// <summary>
        /// Starts periodically getting in-place status.
        /// </summary>
        public override void Activate()
        {
            base.Activate();
            bool isConnected = communicationsManager.GetDeviceConnectionStatus(BssPeripheral.FCB);
            if (isConnected)
            {
                PrepareForGettingInPlaceStatus().ConfigureAwait(continueOnCapturedContext: false);
            }
        }

        /// <summary>
        /// Unregisters this instance from the Messenger class.
        /// <para>To cleanup additional resources, override this method, clean
        /// up and then call base.Cleanup().</para>
        /// </summary>
        public override void Cleanup()
        {
            this.IdtOperator.OperationStarted -= HandleOperationStarted;
            this.IdtOperator.OperationEnded -= HandleOperationEnded;
            this.IdtOperator.TrafficLightColorChanged -= HandleTrafficLightColorChanged;

            this.communicationsManager.CommunicationChanged -= HandleCommunicationChanged;

            base.Cleanup();
        }

        /// <summary>
        /// Progresses operation status.
        /// </summary>
        public void ReportProgress()
        {
            if (IsBusy &&
                currentCartridgeNumber.HasValue)
            {
                IdtStatusViewModel idtStatus = GetCurrentIdtStatus();
                if (idtStatus.TrafficLightColor == TrafficLightColor.Yellow)
                {
                    idtStatus.Progress += Step;
                }
            }
        }

        #endregion Public Methods

        #region Protected Methods

        protected IdtStatusViewModel GetCurrentIdtStatus()
        {
            return IdtStatus[currentCartridgeNumber.Value - 1];
        }

        #endregion Protected Methods

        #region Private Methods

        private void HandleCommunicationChanged(object sender, CommunicationChangedEventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    if (e.Device == BssPeripheral.FCB && e.IsConnected)
                    {
                        if (Activated)
                        {
                            Activate();
                        }
                    }
                });
        }

        private void HandleOperationEnded(object sender, IdtOperationEventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    IdtStatusViewModel idtStatus = GetCurrentIdtStatus();
                    idtStatus.Progress = 100;
                    IsBusy = false;
                });
        }

        private void HandleOperationStarted(object sender, IdtOperationEventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    if (Activated)
                    {
                        IsBusy = true;
                        currentCartridgeNumber = e.CartridgeNumber;
                        IdtStatusViewModel idtStatus = GetCurrentIdtStatus();
                        idtStatus.Progress = 0;
                        idtStatus.Operation = e.Operation;
                    }
                });
        }

        /// <summary>
        /// Updates traffic light status.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="TrafficLightColorChangedEventArgs"/> instance containing the event data.</param>
        private void HandleTrafficLightColorChanged(object sender, TrafficLightColorChangedEventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    if (!Activated)
                    {
                        return;
                    }

                    IdtStatusViewModel idtStatus = IdtStatus[e.Index - 1];
                    idtStatus.TrafficLightColor = e.Color;
                    if (e.Color != TrafficLightColor.Yellow)
                    {
                        idtStatus.Operation = IdtOperation.Idle;
                        idtStatus.Progress = -1;
                        if (e.Color == TrafficLightColor.Off)
                        {
                            idtStatus.Data = null;
                        }
                    }
                });
        }

        /// <summary>
        /// Initializes the traffic lights status.
        /// </summary>
        private async Task PrepareForGettingInPlaceStatus()
        {
            for (int i = IdtStatus.Count; i < IdtOperator.CartridgesCount; i++)
            {
                IdtStatus.Add(new IdtStatusViewModel { CartridgeNumber = (byte)(i + 1) });
            }

            while (IdtStatus.Count > IdtOperator.CartridgesCount)
            {
                IdtStatus.RemoveAt(IdtStatus.Count - 1);
            }

            TrafficLightColor[] trafficLightColors = await IdtOperator.GetTrafficLightColors();
            for (int i = 0; i < trafficLightColors.Length; i++)
            {
                IdtStatus[i].TrafficLightColor = trafficLightColors[i];
            }
        }

        #endregion Private Methods
    }

    /// <summary>
    /// Holds common functionality for all view-models of IDT modes.
    /// </summary>
    /// <typeparam name="T"></typeparam>
    public abstract class IdtOperationViewModel<T> : IdtOperationViewModel
        where T : IdtOperator
    {
        #region Protected Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtOperationViewModel{T}"/> class.
        /// </summary>
        /// <param name="idtOperator">The IDT operator.</param>
        /// <param name="communicationsManager">The communication manager.</param>
        /// <param name="dialogService">The dialog service.</param>
        protected IdtOperationViewModel(IdtOperator idtOperator, CommunicationsManager communicationsManager, IDialogService dialogService)
            : base(idtOperator, communicationsManager, dialogService)
        {
        }

        #endregion Protected Constructors

        #region Protected Properties

        /// <summary>
        /// Gets the IDT operator.
        /// </summary>
        protected new T IdtOperator
        {
            get
            {
                return base.IdtOperator as T;
            }
            private set
            {
                base.IdtOperator = value;
            }
        }

        #endregion Protected Properties
    }
}