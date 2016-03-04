using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Properties;
using System;
using System.ComponentModel;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// Represents the correction system state.
    /// </summary>
    public class CorrectionState : IdtOperationState<IdtCorrector>
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="CorrectionState"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="idtCorrector">The idt corrector.</param>
        /// <param name="communicationsManager">The communications manager.</param>
        public CorrectionState(ConfigurationParameters configurationParameters, IdtCorrector idtCorrector, CommunicationsManager communicationsManager)
            : base(configurationParameters, idtCorrector, communicationsManager)
        {
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the state ID.
        /// </summary>
        /// <value>
        /// The state ID.
        /// </value>
        public override BssStateID StateID
        {
            get { return BssStateID.Correction; }
        }

        /// <summary>
        /// Gets the name of the state.
        /// </summary>
        /// <value>
        /// The name of the state.
        /// </value>
        public override string StateName
        {
            get { return Resources.CorrectionState; }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Determines whether system can switch to the specified state.
        /// </summary>
        /// <param name="newState">The new state.</param>
        /// <returns><c>true</c> if new state represents idle or verification states.</returns>
        public override bool CanSwitchState(BssState newState)
        {
            if (newState == null)
            {
                throw new ArgumentNullException("newState");
            }

            return newState.IsAccessible &&
                (newState.StateID == BssStateID.Idle || newState.StateID == BssStateID.Verification || newState.StateID == BssStateID.Report);
        }

        /// <summary>
        /// Determines whether transition to the specified state can be undone.
        /// </summary>
        /// <returns>
        ///   <c>false</c>.
        /// </returns>
        public override bool IsExitAllowed()
        {
            return false;
        }

        /// <summary>
        /// Determines whether transition to the specified state requires Lot to be read from DB.
        /// </summary>
        /// <returns><c>true</c>.</returns>
        public override bool IsLotRequired()
        {
            return true;
        }

        /// <summary>
        /// Performs a set of actions before state activation.
        /// </summary>
        public override async Task Prepare()
        {
            IdtOperator.StartSession();
            IdtOperator.SessionEnded += HandleIdtBurnerSessionEnded;
            await base.Prepare();
        }

        /// <summary>
        /// Performs a set of actions after state deactivation.
        /// </summary>
        public override async Task Shutdown()
        {
            await base.Shutdown();
            IdtOperator.SessionEnded -= HandleIdtBurnerSessionEnded;
            IdtOperator.EndSession();
        }

        #endregion Public Methods

        #region Private Methods

        private async void HandleIdtBurnerSessionEnded(object sender, EventArgs e)
        {
            CancelEventArgs ce = new CancelEventArgs();

            if (IdtOperator.LastOperationResult)
            {
                IdtOperator.StopListenInPlaceStatusChanges();
                OnTerminating(ce);
                if (ce.Cancel)
                {
                    IdtOperator.StartListenInPlaceStatusChanges();
                    IdtOperator.StartSession();
                }
                else
                {
                    await IdtOperator.EndLot();
                }
            }

            if (!ce.Cancel)
            {
                OnTerminated(EventArgs.Empty);
            }
        }

        #endregion Private Methods
    }
}