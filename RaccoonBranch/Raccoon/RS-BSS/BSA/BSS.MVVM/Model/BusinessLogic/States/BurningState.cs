using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Properties;
using System;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// Represents the burning system state.
    /// </summary>
    public class BurningState : IdtOperationState<IdtBurner>
    {
        #region Public Constructors

        public BurningState(ConfigurationParameters configurationParameters, IdtBurner idtBurner, CommunicationsManager communicationsManager)
            : base(configurationParameters, idtBurner, communicationsManager)
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
            get { return BssStateID.Burning; }
        }

        /// <summary>
        /// Gets the name of the state.
        /// </summary>
        /// <value>
        /// The name of the state.
        /// </value>
        public override string StateName
        {
            get { return Resources.BurningState; }
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

        private void HandleIdtBurnerSessionEnded(object sender, EventArgs e)
        {
            OnTerminated(EventArgs.Empty);
        }

        #endregion Private Methods
    }
}