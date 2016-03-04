using System;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Provides data for IDT operation events.
    /// </summary>
    public class IdtOperationEventArgs : EventArgs
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtOperationEventArgs"/> class.
        /// </summary>
        /// <param name="operation">The IDT operation.</param>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        public IdtOperationEventArgs(IdtOperation operation, byte cartridgeNumber)
        {
            Operation = operation;
            CartridgeNumber = cartridgeNumber;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the IDT action.
        /// </summary>
        /// <value>
        /// The IDT action.
        /// </value>
        public IdtOperation Operation { get; private set; }

        /// <summary>
        /// Gets the cartridge number.
        /// </summary>
        /// <value>
        /// The cartridge number.
        /// </value>
        public byte CartridgeNumber { get; private set; }

        #endregion Public Properties
    }
}