using BSS.MVVM.Properties;
using System;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// Represents the management system state.
    /// </summary>
    public class ManagementState : IdtNonOperationState
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ReportState"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        public ManagementState(ConfigurationParameters configurationParameters)
            : base(configurationParameters)
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
            get { return BssStateID.Management; }
        }

        /// <summary>
        /// Gets the name of the state.
        /// </summary>
        /// <value>
        /// The name of the state.
        /// </value>
        public override string StateName
        {
            get { return Resources.MaintenanceState; }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Determines whether system can switch to the specified state.
        /// </summary>
        /// <param name="newState">The new state.</param>
        /// <returns><c>true</c>.</returns>
        public override bool CanSwitchState(BssState newState)
        {
            if (newState == null)
            {
                throw new ArgumentNullException("newState");
            }

            return newState.IsAccessible && newState.StateID == BssStateID.Idle;
        }

        /// <summary>
        /// Determines whether transition to the specified state can be undone.
        /// </summary>
        /// <returns><c>false</c>.</returns>
        public override bool IsExitAllowed()
        {
            return true;
        }

        #endregion Public Methods
    }
}