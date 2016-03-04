using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using GalaSoft.MvvmLight.Views;

namespace BSS.MVVM.ViewModel
{
    /// <summary>
    /// A view-model for IDT burning mode.
    /// </summary>
    public class IdtBurningViewModel : IdtReadingViewModel
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtBurningViewModel"/> class.
        /// </summary>
        /// <param name="idtBurner">The IDT burner.</param>
        /// <param name="communicationsManager">The communication manager.</param>
        /// <param name="dialogService">The dialog service.</param>
        public IdtBurningViewModel(IdtBurner idtBurner, CommunicationsManager communicationsManager, IDialogService dialogService)
            : base(idtBurner, communicationsManager, dialogService)
        {
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets a value indicating whether reset tags.
        /// </summary>
        /// <value>
        ///   <c>true</c> if reset tags; otherwise, <c>false</c>.
        /// </value>
        public bool ResetTags
        {
            get
            {
                return ((IdtBurner)IdtOperator).ResetTags;
            }
        }

        #endregion Public Properties

        #region Protected Properties

        /// <summary>
        /// Gets the burning progress step.
        /// </summary>
        /// <value>
        /// The step.
        /// </value>
        protected override sbyte Step
        {
            get { return 5; }
        }

        #endregion Protected Properties
    }
}