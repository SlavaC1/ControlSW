using System;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// Specifies the types of BSS system states.
    /// </summary>
    public enum BssStateID
    {
        /// <summary>
        /// The idle state.
        /// </summary>
        Idle,

        /// <summary>
        /// The burning state.
        /// </summary>
        Burning,

        /// <summary>
        /// The correction state.
        /// </summary>
        Correction,

        /// <summary>
        /// The verification state.
        /// </summary>
        Verification,

        /// <summary>
        /// The report state.
        /// </summary>
        Report,

        /// <summary>
        /// The management state.
        /// </summary>
        Management,
    }

    /// <summary>
    /// A base class for all RSBSS system states.
    /// </summary>
    public abstract class BssState
    {
        #region Protected Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BssState"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <exception cref="ArgumentNullException">configurationParameters</exception>
        protected BssState(ConfigurationParameters configurationParameters)
        {
            if (configurationParameters == null)
            {
                throw new ArgumentNullException("configurationParameters");
            }

            this.ConfigurationParameters = configurationParameters;
        }

        #endregion Protected Constructors

        #region Public Properties

        /// <summary>
        /// Gets a value indicating whether this state is accessible.
        /// </summary>
        /// <value>
        /// <c>true</c> if this state is accessible; otherwise, <c>false</c>.
        /// </value>
        public virtual bool IsAccessible
        {
            get
            {
                return true;
            }
        }

        /// <summary>
        /// When overriden in derived class, gets the state ID.
        /// </summary>
        /// <value>
        /// The state ID.
        /// </value>
        public abstract BssStateID StateID { get; }

        /// <summary>
        /// When overriden in derived class, gets the name of the state.
        /// </summary>
        /// <value>
        /// The name of the state.
        /// </value>
        public abstract string StateName { get; }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets the configuration parameters.
        /// </summary>
        /// <value>
        /// The configuration parameters.
        /// </value>
        protected ConfigurationParameters ConfigurationParameters
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets or sets the type of the station.
        /// </summary>
        /// <value>
        /// The type of the station.
        /// </value>
        protected BssStation StationType
        {
            get
            {
                return ConfigurationParameters.StationType;
            }
        }

        #endregion Protected Properties

        #region Public Methods

        /// <summary>
        /// When overriden in derived class, determines whether system can switch to the specified state.
        /// </summary>
        /// <param name="newState">The new state.</param>
        /// <returns></returns>
        public abstract bool CanSwitchState(BssState newState);

        /// <summary>
        /// When overriden in derived class, determines whether transition to the specified state requires all cartridges to be removed.
        /// </summary>
        /// <returns></returns>
        public abstract bool IsCartridgeRemovalRequired();

        /// <summary>
        /// When overriden in derived class, determines whether transition to the specified state can be undone.
        /// </summary>
        /// <returns></returns>
        public abstract bool IsExitAllowed();

        /// <summary>
        /// When overriden in derived class, determines whether is hardware prepared for transition to a the specified new state.
        /// </summary>
        /// <returns><c>true</c> if hardware is prepared; otherwise, <c>false</c>.</returns>
        public abstract bool IsHWPrepared();

        /// <summary>
        /// When overriden in derived class, determines whether transition to the specified state requires Lot to be read from DB.
        /// </summary>
        /// <returns></returns>
        public abstract bool IsLotRequired();

        /// <summary>
        /// When overriden in derived class, performs a set of actions before state activation.
        /// </summary>
        public abstract Task Prepare();

        /// <summary>
        /// When overriden in derived class, performs a set of actions after state deactivation.
        /// </summary>
        public abstract Task Shutdown();

        #endregion Public Methods
    }
}