using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Threading;
using System;

namespace BSS.MVVM.ViewModel
{
    public class LotViewModel : ViewModelBase
    {
        #region Private Fields

        /// <summary>
        /// A reference to the IDT burner.
        /// </summary>
        private IdtBurner idtBurner;

        /// <summary>
        /// The current material information.
        /// </summary>
        private MaterialInfo currentMaterialInfo;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="LotViewModel"/> class.
        /// </summary>
        /// <param name="idtBurner">The burning DB manager.</param>
        /// <exception cref="System.ArgumentNullException">idtBurner</exception>
        public LotViewModel(IdtBurner idtBurner)
        {
            if (idtBurner == null)
            {
                throw new ArgumentNullException("idtBurner");
            }

            this.idtBurner = idtBurner;
            this.idtBurner.LotStarted += HandleLotStarted;
            this.idtBurner.LotEnded += HandleLotEnded;
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the current lot.
        /// </summary>
        /// <value>
        /// The current lot.
        /// </value>
        public MaterialInfo CurrentMaterialInfo
        {
            get
            {
                return currentMaterialInfo;
            }
            set
            {
                currentMaterialInfo = value;
                RaisePropertyChanged(() => CurrentMaterialInfo);
            }
        }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Unregisters this instance from the Messenger class.
        /// <para>To cleanup additional resources, override this method, clean
        /// up and then call base.Cleanup().</para>
        /// </summary>
        public override void Cleanup()
        {
            idtBurner.LotEnded -= HandleLotEnded;
            idtBurner.LotStarted -= HandleLotStarted;
            base.Cleanup();
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Handles the lot started event.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        /// <exception cref="System.NotImplementedException"></exception>
        private void HandleLotStarted(object sender, EventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                CurrentMaterialInfo = idtBurner.LotStatistics.MaterialInfo;
            });
        }

        /// <summary>
        /// Handles the lot started event.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        /// <exception cref="System.NotImplementedException"></exception>
        private void HandleLotEnded(object sender, EventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                CurrentMaterialInfo = null;
            });
        }

        #endregion Private Methods
    }
}