using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BSS.MVVM.Model.BusinessLogic.States
{
    /// <summary>
    /// A base class for all states except IDT operation states.
    /// </summary>
    public abstract class IdtNonOperationState : BssState
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtNonOperationState"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        public IdtNonOperationState(ConfigurationParameters configurationParameters)
            : base(configurationParameters)
        {
        }

        #endregion Public Constructors

        #region Public Methods

        /// <summary>
        /// Determines whether transition to the specified state requires all cartridges to be removed.
        /// </summary>
        /// <returns><c>false</c>.</returns>
        public override bool IsCartridgeRemovalRequired()
        {
            return false;
        }

        /// <summary>
        /// Determines whether is hardware prepared for transition to a the specified new state.
        /// </summary>
        /// <returns><c>true</c>.</returns>
        public override bool IsHWPrepared()
        {
            return true;
        }

        /// <summary>
        /// Determines whether transition to the specified state requires Lot to be read from DB.
        /// </summary>
        /// <returns></returns>
        public override bool IsLotRequired()
        {
            return false;
        }

        /// <summary>
        /// Performs a set of actions before state activation.
        /// </summary>
        public override async Task Prepare()
        {
            await Task.Run(() => { });
        }

        /// <summary>
        /// Performs a set of actions after state deactivation.
        /// </summary>
        public override async Task Shutdown()
        {
            await Task.Run(() => { });
        }

        #endregion Public Methods
    }
}
