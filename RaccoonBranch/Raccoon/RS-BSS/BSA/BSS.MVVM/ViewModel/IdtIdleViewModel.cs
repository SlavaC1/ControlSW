using GalaSoft.MvvmLight.Views;

namespace BSS.MVVM.ViewModel
{
    /// <summary>
    /// A view-model for IDT idle mode.
    /// </summary>
    public class IdtIdleViewModel : BSSViewModel
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtIdleViewModel"/> class.
        /// </summary>
        /// <param name="dialogService">The dialog service.</param>
        public IdtIdleViewModel(IDialogService dialogService)
            : base(dialogService)
        {
            IdtStatus = new IdtStatusViewModel[0];
        }

        #endregion Public Constructors

        #region Public Properties

        public IdtStatusViewModel[] IdtStatus
        {
            get;
            private set;
        }

        #endregion Public Properties
    }
}