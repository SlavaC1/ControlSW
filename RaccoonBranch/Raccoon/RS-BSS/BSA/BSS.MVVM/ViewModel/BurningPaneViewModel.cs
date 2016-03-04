using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.States;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using System;
using System.Threading.Tasks;

namespace BSS.MVVM.ViewModel
{
    public class BurningPaneViewModel : ViewModelBase
    {
        #region Private Fields

        /// <summary>
        /// A reference to the idt burner.
        /// </summary>
        private IdtBurner idtBurner;

        /// <summary>
        /// A reference to the states manager.
        /// </summary>
        private StatesManager statesManager;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BurningPaneViewModel"/> class.
        /// </summary>
        /// <param name="burningDBManager">The burning DB manager.</param>
        /// <exception cref="System.ArgumentNullException">burningDBManager</exception>
        public BurningPaneViewModel(IdtBurner idtBurner, StatesManager statesManager)
        {
            if (idtBurner == null)
            {
                throw new ArgumentNullException("idtBurner");
            }

            if (statesManager == null)
            {
                throw new ArgumentNullException("statesManager");
            }

            this.idtBurner = idtBurner;
            this.statesManager = statesManager;

            EndSessionCommand = new RelayCommand(EndSession);
            EndLotCommand = new RelayCommand(EndLot);
            ToggleReferenceStateCommand = new RelayCommand(ToggleReferenceState);
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the end lot command.
        /// </summary>
        /// <value>
        /// The end lot command.
        /// </value>
        public RelayCommand EndLotCommand { get; private set; }

        /// <summary>
        /// Gets the end session command.
        /// </summary>
        /// <value>
        /// The end session command.
        /// </value>
        public RelayCommand EndSessionCommand { get; private set; }

        /// <summary>
        /// Gets a value indicating whether system shall burn cartridges as references.
        /// </summary>
        /// <value>
        /// <c>true</c> if system shall burn cartridges as references; otherwise, <c>false</c>.
        /// </value>
        public bool IsInReferenceMode
        {
            get
            {
                return statesManager.IsInReferenceMode;
            }
        }

        /// <summary>
        /// Gets the Lot statistics.
        /// </summary>
        /// <value>
        /// The Lot statistics.
        /// </value>
        public BatchStatistics LotStatistics
        {
            get
            {
                return idtBurner.LotStatistics;
            }
        }

        /// <summary>
        /// Gets the toggle reference state command.
        /// </summary>
        /// <value>
        /// The toggle reference state command.
        /// </value>
        public RelayCommand ToggleReferenceStateCommand { get; private set; }

        #endregion Public Properties

        #region Private Methods

        /// <summary>
        /// Ends the lot.
        /// </summary>
        private async void EndLot()
        {
            try
            {
                await statesManager.EndLot();
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
        }

        /// <summary>
        /// Ends the session.
        /// </summary>
        private async void EndSession()
        {
            try
            {
                await statesManager.EndSession();
            }
            catch (Exception ex)
            {
                MessengerUtils.SendException(ex);
            }
        }

        /// <summary>
        /// Toggles the reference state.
        /// </summary>
        private void ToggleReferenceState()
        {
            bool actionRequired;
            do
            {
                try
                {
                    statesManager.ToggleReferenceState();
                    actionRequired = false;
                }
                catch (Exception ex)
                {
                    InvalidStateTransitionException istex = ex as InvalidStateTransitionException;
                    actionRequired = istex != null && istex.ActionRequired;
                    MessengerUtils.SendException(ex);
                }
            } while (actionRequired);

            RaisePropertyChanged(() => IsInReferenceMode);
        }

        #endregion Private Methods
    }
}