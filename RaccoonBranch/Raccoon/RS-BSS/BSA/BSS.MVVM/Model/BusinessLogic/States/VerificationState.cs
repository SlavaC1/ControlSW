using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Properties;
using System;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// Represents the verification system state.
    /// </summary>
    public class VerificationState : IdtOperationState<IdtReader>
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="VerificationState"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="idtReader">The IDT reader.</param>
        /// <param name="communicationsManager">The communications manager.</param>
        public VerificationState(ConfigurationParameters configurationParameters, IdtReader idtReader, CommunicationsManager communicationsManager)
            : base(configurationParameters, idtReader, communicationsManager)
        {
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets a value indicating whether this state is accessible.
        /// </summary>
        /// <value>
        /// <c>true</c> if this state is accessible; otherwise, <c>false</c>.
        /// </value>
        public override bool IsAccessible
        {
            get { return StationType == BssStation.Standalone; }
        }

        /// <summary>
        /// Gets the state ID.
        /// </summary>
        /// <value>
        /// The state ID.
        /// </value>
        public override BssStateID StateID
        {
            get { return BssStateID.Verification; }
        }

        /// <summary>
        /// Gets the name of the state.
        /// </summary>
        /// <value>
        /// The name of the state.
        /// </value>
        public override string StateName
        {
            get { return Resources.VerificationState; }
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
                (newState.StateID == BssStateID.Idle || newState.StateID == BssStateID.Burning || newState.StateID == BssStateID.Correction);
        }

        /// <summary>
        /// Determines whether transition to the specified state can be undone.
        /// </summary>
        /// <returns><c>true</c>.</returns>
        public override bool IsExitAllowed()
        {
            return ConfigurationParameters.StationType == BssStation.Standalone;
        }

        /// <summary>
        /// Determines whether transition to the specified state requires Lot to be read from DB.
        /// </summary>
        /// <returns><c>false</c>.</returns>
        public override bool IsLotRequired()
        {
            return false;
        }

        #endregion Public Methods
    }
}