using BSS.MVVM.Properties;
using System;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// Represents the report system state.
    /// </summary>
    public class ReportState : IdtNonOperationState
    {
        #region Private Fields

        private CommunicationsManager communicationsManager;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ReportState"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="communicationsManager">The communications manager.</param>
        public ReportState(ConfigurationParameters configurationParameters, CommunicationsManager communicationsManager)
            : base(configurationParameters)
        {
            if (communicationsManager == null)
            {
                throw new ArgumentNullException("communicationsManager");
            }

            this.communicationsManager = communicationsManager;
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
            get { return BssStateID.Report; }
        }

        /// <summary>
        /// Gets the name of the state.
        /// </summary>
        /// <value>
        /// The name of the state.
        /// </value>
        public override string StateName
        {
            get { return Resources.ReportState; }
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

            return newState.IsAccessible;
        }

        /// <summary>
        /// Determines whether transition to the specified state can be undone.
        /// </summary>
        /// <returns><c>false</c>.</returns>
        public override bool IsExitAllowed()
        {
            return true;
        }

        /// <summary>
        /// Determines whether is hardware prepared for transition to a the specified new state.
        /// </summary>
        /// <returns><c>true</c>.</returns>
        public override bool IsHWPrepared()
        {
            return communicationsManager.GetDeviceConnectionStatus(BssPeripheral.DB);
        }

        #endregion Public Methods
    }
}