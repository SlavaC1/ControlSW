using System;

namespace BSS.MVVM.Model.BusinessLogic.IdtSrv
{
    /// <summary>
    /// Provides arguments for <see cref="E:InPlaceStatusChanged"/> event.
    /// </summary>
    [Serializable]
    public class InPlaceStatusChangedEventArgs : EventArgs
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="InPlaceStatusChangedEventArgs"/> class.
        /// </summary>
        /// <param name="oldInPlaceStatus">The old in-place status.</param>
        /// <param name="newInPlaceStatus">The new in-place status.</param>
        public InPlaceStatusChangedEventArgs(int oldInPlaceStatus, int newInPlaceStatus)
        {
            OldInPlaceStatus = oldInPlaceStatus;
            NewInPlaceStatus = newInPlaceStatus;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the new in-place status.
        /// </summary>
        /// <value>
        /// The new in-place status.
        /// </value>
        public int NewInPlaceStatus { get; private set; }

        /// <summary>
        /// Gets the old in-place status.
        /// </summary>
        /// <value>
        /// The old in-place status.
        /// </value>
        public int OldInPlaceStatus { get; private set; }

        #endregion Public Properties
    }
}