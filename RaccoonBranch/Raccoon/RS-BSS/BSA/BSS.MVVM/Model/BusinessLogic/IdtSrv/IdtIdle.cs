using BSS.PlcWrapper;
using RaccoonCLI;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// An IDT operator that does nothing.
    /// </summary>
    public class IdtIdle : IdtOperator
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtIdle"/> class.
        /// </summary>
        /// <param name="configurationParameters">The configuration parameters.</param>
        /// <param name="inPlaceManager">The in place manager.</param>
        /// <param name="materialMonitor">The material monitor.</param>
        /// <param name="plcWrapper">The PLC wrapper.</param>
        public IdtIdle(ConfigurationParameters configurationParameters, InPlaceManager inPlaceManager, MaterialMonitorWrapper materialMonitor, IPlc plcWrapper)
            : base(configurationParameters, inPlaceManager, materialMonitor, plcWrapper)
        {
            EnableOperation = true;
        }

        #endregion Public Constructors

        #region Protected Properties

        /// <summary>
        /// Gets the type of the operation.
        /// </summary>
        /// <value>
        /// The type of the operation.
        /// </value>
        protected override BssState OperationType
        {
            get
            {
                return BssState.Idle;
            }
        }

        #endregion Protected Properties

        #region Protected Methods

        /// <summary>
        /// Does nothing.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        protected override void Operate(byte cartridgeNumber)
        {
            // do nothing
        }

        #endregion Protected Methods
    }
}