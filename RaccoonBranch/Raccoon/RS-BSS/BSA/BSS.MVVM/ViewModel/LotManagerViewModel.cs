using BSS.MVVM.Model.BusinessLogic.LotManagement;
using BSS.MVVM.Model.Entities;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Threading;
using System;

namespace BSS.MVVM.ViewModel
{
    public class LotManagerViewModel : ViewModelBase
    {
        #region Private Fields

        /// <summary>
        /// Holds a copy of the current material information as read from Oracle DB.
        /// </summary>
        private MaterialInfo currentMaterialInfo;

        /// <summary>
        /// A reference to the Lot manager.
        /// </summary>
        private LotManager lotManager;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="LotManagerViewModel"/> class.
        /// </summary>
        /// <param name="lotManager">The lot manager.</param>
        /// <exception cref="System.ArgumentNullException">lotManager</exception>
        public LotManagerViewModel(LotManager lotManager)
        {
            if (lotManager == null)
            {
                throw new ArgumentNullException("lotManager");
            }

            this.lotManager = lotManager;
            this.lotManager.CurrentMaterialInfoChanged += HandleCurrentMaterialInfoChanged;
            currentMaterialInfo = GetCurrentMaterialInfo();
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
            private set
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
            lotManager.CurrentMaterialInfoChanged -= HandleCurrentMaterialInfoChanged;
            base.Cleanup();
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Sets the current material information.
        /// </summary>
        private MaterialInfo GetCurrentMaterialInfo()
        {
            MaterialInfo currentMaterialInfo = lotManager.CurrentMaterialInfo;
            if (currentMaterialInfo == null)
            {
                return null;
            }

            return currentMaterialInfo.Clone() as MaterialInfo;
        }

        /// <summary>
        /// Sets the current material information.
        /// </summary>
        /// <param name="sender">The sender.</param>
        /// <param name="e">The <see cref="EventArgs"/> instance containing the event data.</param>
        private void HandleCurrentMaterialInfoChanged(object sender, EventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                CurrentMaterialInfo = GetCurrentMaterialInfo();
            });
        }

        #endregion Private Methods
    }
}