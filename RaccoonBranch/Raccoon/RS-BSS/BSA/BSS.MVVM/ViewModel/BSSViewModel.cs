using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Views;
using System;

namespace BSS.MVVM.ViewModel
{
    /// <summary>
    /// Holds common functionality for all view-models.
    /// </summary>
    public abstract class BSSViewModel : ViewModelBase
    {
        #region Private Fields

        /// <summary>
        /// <c>true</c> if activated; otherwise, <c>false</c>.
        /// </summary>
        private bool activated;

        private string status;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BSSViewModel"/> class.
        /// </summary>
        /// <param name="dialogService">The dialog service.</param>
        /// <exception cref="System.ArgumentNullException">
        /// dialogService
        /// </exception>
        public BSSViewModel(IDialogService dialogService)
        {
            if (dialogService == null)
            {
                throw new ArgumentNullException("dialogService");
            }

            DialogService = dialogService;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets or sets a value indicating whether this <see cref="BSSViewModel"/> is activated.
        /// </summary>
        /// <value>
        ///   <c>true</c> if activated; otherwise, <c>false</c>.
        /// </value>
        public bool Activated
        {
            get
            {
                return activated;
            }
            set
            {
                activated = value;
                RaisePropertyChanged(() => Activated);
            }
        }

        /// <summary>
        /// Gets the status.
        /// </summary>
        /// <value>
        /// The status.
        /// </value>
        public string Status
        {
            get
            {
                return status;
            }
            protected set
            {
                status = value;
                RaisePropertyChanged(() => Status);
            }
        }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets the dialog service.
        /// </summary>
        /// <value>
        /// The dialog service.
        /// </value>
        protected IDialogService DialogService { get; private set; }

        #endregion Protected Properties

        #region Public Methods

        /// <summary>
        /// When overriden in derived class, performs startup operations.
        /// </summary>
        public virtual void Activate()
        {
            Activated = true;
        }

        /// <summary>
        /// When overriden in derived class, returns a value indicating if user can exit page.
        /// </summary>
        /// <returns><c>true</c> if user can exit; otherwise, <c>false</c>.</returns>
        public virtual bool CanExit()
        {
            return true;
        }

        /// <summary>
        /// Unregisters this instance from the Messenger class.
        /// <para>To cleanup additional resources, override this method, clean
        /// up and then call base.Cleanup().</para>
        /// </summary>
        public override void Cleanup()
        {
            Deactivate();
            base.Cleanup();
        }

        /// <summary>
        /// When overriden in derived class, performs shutdown operations.
        /// </summary>
        public virtual void Deactivate()
        {
            Activated = false;
        }

        #endregion Public Methods
    }
}