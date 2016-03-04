using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Views;

namespace BSS.MVVM.ViewModel
{
    /// <summary>
    /// A view-model for IDT verification mode.
    /// </summary>
    public class IdtReadingViewModel : IdtOperationViewModel<IdtReader>
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtReadingViewModel"/> class.
        /// </summary>
        /// <param name="idtReader">The IDT reader.</param>
        /// <param name="communicationsManager">The communication manager.</param>
        /// <param name="dialogService">The dialog service.</param>
        public IdtReadingViewModel(IdtReader idtReader, CommunicationsManager communicationsManager, IDialogService dialogService)
            : base(idtReader, communicationsManager, dialogService)
        {
            IdtOperator.TagInfoRead += HandleTagInfoRead;
        }

        #endregion Public Constructors

        #region Protected Properties

        /// <summary>
        /// Gets the reading progress step.
        /// </summary>
        /// <value>
        /// The step.
        /// </value>
        protected override sbyte Step
        {
            get { return 10; }
        }

        #endregion Protected Properties

        #region Public Methods

        /// <summary>
        /// Unregisters this instance from the Messenger class.
        /// <para>To cleanup additional resources, override this method, clean
        /// up and then call base.Cleanup().</para>
        /// </summary>
        public override void Cleanup()
        {
            IdtOperator.TagInfoRead -= HandleTagInfoRead;
            base.Cleanup();
        }

        #endregion Public Methods

        #region Private Methods

        private void HandleTagInfoRead(object sender, TagInfoEventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    IdtStatusViewModel idtStatus = GetCurrentIdtStatus();
                    idtStatus.Data = e.TagInfo;
                });
        }

        #endregion Private Methods
    }
}