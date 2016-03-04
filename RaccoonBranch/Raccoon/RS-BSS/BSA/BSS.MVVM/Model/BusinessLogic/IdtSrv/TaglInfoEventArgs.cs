using BSS.Contracts;
using System;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Provides arguments for <see cref="E:TagInfoRead"/> event.
    /// </summary>
    public class TagInfoEventArgs : EventArgs
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="TagInfoEventArgs"/> class.
        /// </summary>
        /// <param name="cartridgeNumber">The cartridge number.</param>
        /// <param name="tagInfo">The tag information.</param>
        public TagInfoEventArgs(byte cartridgeNumber, TagInfo tagInfo)
        {
            CartridgeNumber = cartridgeNumber;
            TagInfo = tagInfo;
        }

        #endregion Public Constructors

        #region Public Properties

        #region Public Properties

        /// <summary>
        /// Gets the cartridge number.
        /// </summary>
        /// <value>
        /// The cartridge number.
        /// </value>
        public byte CartridgeNumber { get; private set; }

        #endregion Public Properties

        /// <summary>
        /// Gets the tag information.
        /// </summary>
        /// <value>
        /// The tag information.
        /// </value>
        public TagInfo TagInfo { get; private set; }

        #endregion Public Properties
    }
}