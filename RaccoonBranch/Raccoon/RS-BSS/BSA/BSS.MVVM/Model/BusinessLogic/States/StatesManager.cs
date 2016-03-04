using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Properties;
using System;
using System.ComponentModel;
using System.Runtime.Serialization;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// This exception is thrown when trying to move between states that are not connected in states diagram.
    /// </summary>
    [Serializable]
    public class InvalidStateTransitionException : Exception
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="InvalidStateTransitionException"/> class.
        /// </summary>
        public InvalidStateTransitionException()
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="InvalidStateTransitionException"/> class.
        /// </summary>
        /// <param name="message">The message that describes the error.</param>
        public InvalidStateTransitionException(string message)
            : base(message)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="InvalidStateTransitionException"/> class.
        /// </summary>
        /// <param name="message">The error message that explains the reason for the exception.</param>
        /// <param name="inner">The exception that is the cause of the current exception, or a null reference if no inner exception is specified.</param>
        public InvalidStateTransitionException(string message, Exception inner)
            : base(message, inner)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="InvalidStateTransitionException"/> class.
        /// </summary>
        /// <param name="message">The message that describes the error.</param>
        /// <param name="sourceState">The source state.</param>
        /// <param name="targetState">The target state.</param>
        public InvalidStateTransitionException(string message, bool actionRequired, BssState sourceState, BssState targetState)
            : base(message)
        {
            this.ActionRequired = actionRequired;
            this.SourceState = sourceState;
            this.TargetState = targetState;
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="InvalidStateTransitionException"/> class.
        /// </summary>
        /// <param name="message">The error message that explains the reason for the exception.</param>
        /// <param name="inner">The exception that is the cause of the current exception, or a null reference if no inner exception is specified.</param>
        /// <param name="sourceState">The source state.</param>
        /// <param name="targetState">The target state.</param>
        public InvalidStateTransitionException(string message, Exception inner, bool actionRequired, BssState sourceState, BssState targetState)
            : base(message, inner)
        {
            this.ActionRequired = actionRequired;
            this.SourceState = sourceState;
            this.TargetState = targetState;
        }

        #endregion Public Constructors

        #region Protected Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="InvalidStateTransitionException"/> class.
        /// </summary>
        /// <param name="info">The <see cref="T:System.Runtime.Serialization.SerializationInfo" /> that holds the serialized object data about the exception being thrown.</param>
        /// <param name="context">The <see cref="T:System.Runtime.Serialization.StreamingContext" /> that contains contextual information about the source or destination.</param>
        protected InvalidStateTransitionException(SerializationInfo info, StreamingContext context)
            : base(info, context)
        {
            info.AddValue("ActionRequired", ActionRequired);
            info.AddValue("SourceState", SourceState);
            info.AddValue("TargetState", TargetState);
        }

        #endregion Protected Constructors

        #region Public Properties

        /// <summary>
        /// Gets a value indicating whether action is required.
        /// </summary>
        /// <value>
        ///   <c>true</c> if action is required; otherwise, <c>false</c>.
        /// </value>
        public bool ActionRequired { get; private set; }

        /// <summary>
        /// Gets the source state.
        /// </summary>
        /// <value>
        /// The source state.
        /// </value>
        public BssState SourceState { get; private set; }

        /// <summary>
        /// Gets the target state.
        /// </summary>
        /// <value>
        /// The target state.
        /// </value>
        public BssState TargetState { get; private set; }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// When overridden in a derived class, sets the <see cref="T:System.Runtime.Serialization.SerializationInfo" /> with information about the exception.
        /// </summary>
        /// <param name="info">The <see cref="T:System.Runtime.Serialization.SerializationInfo" /> that holds the serialized object data about the exception being thrown.</param>
        /// <param name="context">The <see cref="T:System.Runtime.Serialization.StreamingContext" /> that contains contextual information about the source or destination.</param>
        /// <PermissionSet>
        ///   <IPermission class="System.Security.Permissions.FileIOPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Read="*AllFiles*" PathDiscovery="*AllFiles*" />
        ///   <IPermission class="System.Security.Permissions.SecurityPermission, mscorlib, Version=2.0.3600.0, Culture=neutral, PublicKeyToken=b77a5c561934e089" version="1" Flags="SerializationFormatter" />
        /// </PermissionSet>
        public override void GetObjectData(SerializationInfo info, StreamingContext context)
        {
            base.GetObjectData(info, context);
            ActionRequired = info.GetBoolean("ActionRequired");
            SourceState = (BssState)info.GetValue("SourceState", typeof(BssState));
            TargetState = (BssState)info.GetValue("TargetState", typeof(BssState));
        }

        #endregion Public Methods
    }

    /// <summary>
    /// Holds the current system states and the logic of moving between states.
    /// </summary>
    public class StatesManager : IDisposable
    {
        #region Private Fields

        /// <summary>
        /// Holds all system states.
        /// </summary>
        private readonly BssState[] allStates;

        /// <summary>
        /// A reference to the main model.
        /// </summary>
        private readonly MainModel mainModel;

        /// <summary>
        /// The current system state.
        /// </summary>
        private BssState currentState;

        /// <summary>
        /// The display message delegate.
        /// </summary>
        private Action<string> displayMessageDel;

        /// <summary>
        /// <c>true</c> to user confirmation.
        /// </summary>
        private bool skipUserConfirmation;

        /// <summary>
        /// The user confirmation predicate.
        /// </summary>
        private Predicate<string> userConfirmationPredicate;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="StatesManager"/> class.
        /// </summary>
        /// <param name="mainModel.BurningDBManager">The burning DB manager.</param>
        /// <param name="mainModel.InPlaceManager">The in place manager.</param>
        /// <exception cref="System.ArgumentNullException">
        /// mainModel
        /// </exception>
        public StatesManager(MainModel mainModel)
        {
            if (mainModel == null)
            {
                throw new ArgumentNullException("mainModel");
            }

            this.mainModel = mainModel;
            this.mainModel.CommunicationsManager.CommunicationChanged += HandleCommunicationChanged;
            this.displayMessageDel = (str) => { };
            this.userConfirmationPredicate = (str) => { return true; };

            allStates = new BssState[Enum.GetValues(typeof(BssStateID)).Length];
            allStates[(int)BssStateID.Idle] = new IdleState(mainModel.ConfigurationParameters);
            allStates[(int)BssStateID.Burning] = new BurningState(mainModel.ConfigurationParameters, mainModel.IdtBurner, mainModel.CommunicationsManager);
            allStates[(int)BssStateID.Correction] = new CorrectionState(mainModel.ConfigurationParameters, mainModel.IdtCorrector, mainModel.CommunicationsManager);
            allStates[(int)BssStateID.Verification] = new VerificationState(mainModel.ConfigurationParameters, mainModel.IdtReader, mainModel.CommunicationsManager);
            allStates[(int)BssStateID.Report] = new ReportState(mainModel.ConfigurationParameters, mainModel.CommunicationsManager);
            allStates[(int)BssStateID.Management] = new ManagementState(mainModel.ConfigurationParameters);

            CorrectionState correctionState = allStates[(int)BssStateID.Correction] as CorrectionState;
            if (correctionState != null)
            {
                correctionState.Terminating += HandleStateTerminating;
                correctionState.Terminated += HandleStateTerminated;
            }

            currentState = PreviousState = allStates[(int)BssStateID.Idle];
        }

        #endregion Public Constructors

        #region Public Events

        /// <summary>
        /// Occurs when current state is changed.
        /// </summary>
        public event EventHandler CurrentStateChanged;

        #endregion Public Events

        #region Public Properties

        /// <summary>
        /// Gets the current system state.
        /// </summary>
        /// <value>
        /// The current system state.
        /// </value>
        public BssState CurrentState
        {
            get
            {
                return currentState;
            }
            private set
            {
                currentState = value;
                OnCurrentStateChanged(EventArgs.Empty);
            }
        }

        /// <summary>
        /// Gets or sets the display message delgate.
        /// </summary>
        /// <value>
        /// The display message delegate.
        /// </value>
        public Action<string> DisplayMessageDel
        {
            get
            {
                return displayMessageDel;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                displayMessageDel = value;
            }
        }

        /// <summary>
        /// Gets a value indicating whether system shall burn cartridges as references.
        /// </summary>
        /// <value>
        /// <c>true</c> if system shall burn cartridges as references; otherwise, <c>false</c>.
        /// </value>
        public bool IsInReferenceMode
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the previous system state.
        /// </summary>
        /// <value>
        /// The previous system state.
        /// </value>
        public BssState PreviousState
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the station type.
        /// </summary>
        /// <value>
        /// The station type.
        /// </value>
        public BssStation StationType
        {
            get
            {
                return mainModel.ConfigurationParameters.StationType;
            }
        }

        /// <summary>
        /// Gets or sets the user confirmation predicate.
        /// </summary>
        /// <value>
        /// The user confirmation predicate.
        /// </value>
        public Predicate<string> UserConfirmationPredicate
        {
            get
            {
                return userConfirmationPredicate;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                userConfirmationPredicate = value;
            }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Ends the lot.
        /// </summary>
        public async Task EndLot()
        {
            if (!mainModel.ConfigurationParameters.ResetTags)
            {
                string warningMessage = null;
                if (mainModel.IdtBurner.LotStatistics.LotBurnedCartridgesCount == 0)
                {
                    warningMessage = Resources.NoCartridgesInLot;
                }
                else if (!mainModel.IdtBurner.LotStatistics.HasReferenceTags)
                {
                    warningMessage = Resources.NoReferenceCartridgesInLot;
                }

                if (warningMessage != null)
                {
                    bool confirm = UserConfirmationPredicate(warningMessage);
                    if (!confirm)
                    {
                        return;
                    }
                }
            }

            bool ok = await mainModel.IdtBurner.EndLot();
            if (ok)
            {
                await Transit(BssStateID.Idle);
                skipUserConfirmation = mainModel.ConfigurationParameters.ResetTags;
            }
        }

        /// <summary>
        /// Ends the session.
        /// </summary>
        public async Task EndSession()
        {
            mainModel.IdtBurner.EndSession();
            await Transit(BssStateID.Idle);
        }

        /// <summary>
        /// Transits back to previous state.
        /// </summary>
        public async Task ExitState()
        {
            bool canExit = CurrentState.IsExitAllowed();
            if (!canExit)
            {
                throw new InvalidOperationException(String.Format(Resources.CannotExitMessage, CurrentState.StateName));
            }

            await Transit(PreviousState);
        }

        /// <summary>
        /// Gets the state by specified ID.
        /// </summary>
        /// <param name="bssStateID">The BSS state identifier.</param>
        /// <returns></returns>
        public BssState GetState(BssStateID bssStateID)
        {
            return allStates[(int)bssStateID];
        }

        /// <summary>
        /// Toggles the reference state.
        /// </summary>
        /// <exception cref="BSS.MVVM.Model.BusinessLogic.InvalidStateTransitionException">
        /// chassis are not empty.
        /// </exception>
        public void ToggleReferenceState()
        {
            if (mainModel.IdtBurner.LotStatistics.LotBurnedCartridgesCount == 0)
            {
                throw new InvalidStateTransitionException(Resources.NoCartridgeBurned);
            }

            bool areCartridgesInsertedUnintentionally = AreCartridgesInsertedUnintentionally(CurrentState);
            if (areCartridgesInsertedUnintentionally)
            {
                throw new InvalidStateTransitionException(Resources.NoCartridgeLeft, actionRequired: true, sourceState: CurrentState, targetState: CurrentState);
            }

            IsInReferenceMode = !IsInReferenceMode;
            mainModel.IdtBurner.IsReference = IsInReferenceMode;
        }

        /// <summary>
        /// Transits system into a new state.
        /// </summary>
        /// <param name="newStateID">The new state ID.</param>
        /// <exception cref="BSS.MVVM.Model.BusinessLogic.InvalidStateTransitionException">
        /// new state is not reachable from current state <para>-or-</para>
        /// new state is Burning, but Lot was not read from DB <para>-or-</para>
        /// new state is Burning or Verification, but chassis are not empty.
        /// </exception>
        public async Task Transit(BssStateID newStateID)
        {
            await Transit(allStates[(int)newStateID]);
        }

        #endregion Public Methods

        #region Protected Methods

        /// <summary>
        /// Called when current state is changed.
        /// </summary>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        protected virtual void OnCurrentStateChanged(EventArgs e)
        {
            EventHandler temp = CurrentStateChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        #endregion Protected Methods

        #region Private Methods

        /// <summary>
        /// Gets a value indicating if cartridges are inserted when they are not supposed to.
        /// </summary>
        /// <param name="state">The state.</param>
        /// <returns><c>true</c> if cartridges are inserted when they are not supposed to; otherwise, <c>false</c>.</returns>
        private bool AreCartridgesInsertedUnintentionally(BssState state)
        {
            return StationType == BssStation.Standalone &&
                state.IsCartridgeRemovalRequired() &&
                mainModel.InPlaceManager.HasCartridges;
        }

        private void HandleCommunicationChanged(object sender, CommunicationChangedEventArgs e)
        {
            if (e.Device == BssPeripheral.HSM && e.IsConnected)
            {
                this.skipUserConfirmation = mainModel.ConfigurationParameters.ResetTags;
            }
        }

        /// <summary>
        /// Handles the Terminated event:
        /// transits to idle state.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private async void HandleStateTerminated(object sender, EventArgs e)
        {
            CorrectionState correctionState = sender as CorrectionState;
            if (correctionState != null)
            {
                if (!correctionState.IdtOperator.LastOperationResult)
                {
                    DisplayMessageDel(Resources.CorrectionFailed);
                }

                await Transit(BssStateID.Idle);
            }
        }

        /// <summary>
        /// Handles the Terminating event:
        /// asks user if continue correcting.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="CancelEventArgs"/> instance containing the event data.</param>
        private void HandleStateTerminating(object sender, CancelEventArgs e)
        {
            CorrectionState correctionState = sender as CorrectionState;
            if (correctionState != null)
            {
                e.Cancel = UserConfirmationPredicate(Resources.ContinueCorrection);
            }
        }

        /// <summary>
        /// Starts lot before transition into a state which requires lot.
        /// </summary>
        /// <param name="newState">The new state.</param>
        /// <returns></returns>
        /// <exception cref="BSS.MVVM.Model.BusinessLogic.States.InvalidStateTransitionException">
        /// </exception>
        private async Task<bool> StartLot(BssState newState)
        {
            IdtOperationState idtOperationState = newState as IdtOperationState;
            if (idtOperationState == null)
            {
                return false;
            }

            IdtBurner idtBurner = idtOperationState.IdtOperator as IdtBurner;
            var currentLot = await idtBurner.ConnectAndReadCurrentLot();
            if (currentLot == null)
            {
                return false;
            }

            if (currentLot.MaterialInfo != null &&
                !String.IsNullOrEmpty(currentLot.MaterialInfo.Error))
            {
                throw new InvalidStateTransitionException(
                    String.Format(Resources.CannotTransitMessageForReason, CurrentState.StateName, newState.StateName, currentLot.MaterialInfo.Error),
                    actionRequired: false,
                    sourceState: CurrentState,
                    targetState: newState);
            }

            if (!skipUserConfirmation)
            {
                string message;
                if (currentLot.IsNew())
                {
                    // lot is new
                    message = Resources.NewLot;
                }
                else if (currentLot.IsClosed())
                {
                    // lot ended
                    message = Resources.EndOfLotReported;
                }
                else
                {
                    // lot started, not ended
                    message = Resources.LotIsAlreadyOpened;
                }

                bool confirm = UserConfirmationPredicate(message + Environment.NewLine + Resources.ProceedMessage);
                if (!confirm)
                {
                    MessengerUtils.SendErrorMessage(String.Format(
                        Resources.CannotTransitMessageForReason,
                        CurrentState.StateName, newState.StateName, Resources.UserCancelled));

                    return false;
                }
            }

            skipUserConfirmation = true;
            idtBurner.StartLot(currentLot);
            return true;
        }

        /// <summary>
        /// Transits system into a new state.
        /// </summary>
        /// <param name="newState">The new state.</param>
        /// <exception cref="BSS.MVVM.Model.BusinessLogic.InvalidStateTransitionException">
        /// new state is not reachable from current state <para>-or-</para>
        /// new state is Burning, but Lot was not read from DB <para>-or-</para>
        /// new state is Burning or Verification, but chassis are not empty.
        /// </exception>
        private async Task Transit(BssState newState)
        {
            if (newState == CurrentState)
            {
                return;
            }

            bool canTransit = CurrentState.CanSwitchState(newState);
            if (!canTransit)
            {
                throw new InvalidStateTransitionException(
                    String.Format(Resources.CannotTransitMessage, CurrentState.StateName, newState.StateName),
                    actionRequired: false,
                    sourceState: CurrentState,
                    targetState: newState);
            }

            if (CurrentState.StateID == BssStateID.Idle)
            {
                bool isHWPrepared = newState.IsHWPrepared();
                if (!isHWPrepared)
                {
                    throw new InvalidStateTransitionException(
                        String.Format(Resources.CannotTransitMessageForReason, CurrentState.StateName, newState.StateName, Resources.HWNotReady),
                        actionRequired: false,
                        sourceState: CurrentState,
                        targetState: newState);
                }
            }

            if (newState.IsLotRequired())
            {
                bool confirm = await StartLot(newState);
                if (!confirm)
                {
                    return;
                }
            }

            bool fail = AreCartridgesInsertedUnintentionally(newState);
            if (fail)
            {
                throw new InvalidStateTransitionException(
                    String.Format(Resources.CannotTransitMessageForReason, CurrentState.StateName, newState.StateName, Resources.NoCartridgeLeft),
                    actionRequired: true,
                    sourceState: CurrentState,
                    targetState: newState);
            }

            await CurrentState.Shutdown();
            PreviousState = CurrentState;
            CurrentState = newState;
            await CurrentState.Prepare();

            MessengerUtils.SendInfoMessage(String.Format(Resources.TransitionSucceeded, PreviousState.StateName, CurrentState.StateName));
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
                this.mainModel.CommunicationsManager.CommunicationChanged -= HandleCommunicationChanged;
            }
        }

        #endregion IDisposable Members
    }
}