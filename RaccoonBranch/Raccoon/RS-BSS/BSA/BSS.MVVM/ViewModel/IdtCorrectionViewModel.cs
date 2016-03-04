using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using GalaSoft.MvvmLight.Views;

namespace BSS.MVVM.ViewModel
{
    public class IdtCorrectionViewModel : IdtBurningViewModel
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="IdtCorrectionViewModel"/> class.
        /// </summary>
        /// <param name="idtCorrector">The idt corrector.</param>
        /// <param name="communicationsManager">The communications manager.</param>
        /// <param name="dialogService">The dialog service.</param>
        public IdtCorrectionViewModel(IdtCorrector idtCorrector, CommunicationsManager communicationsManager, IDialogService dialogService)
            : base(idtCorrector, communicationsManager, dialogService)
        {
        }

        #endregion Public Constructors

        #region Protected Properties

        /// <summary>
        /// Gets the correction progress step.
        /// </summary>
        /// <value>
        /// The step.
        /// </value>
        protected override sbyte Step
        {
            get
            {
                return 3;
            }
        }

        #endregion Protected Properties
    }
}