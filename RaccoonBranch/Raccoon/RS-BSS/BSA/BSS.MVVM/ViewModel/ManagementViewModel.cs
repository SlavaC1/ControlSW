using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Properties;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Views;
using System;
using System.Collections.ObjectModel;
using System.Linq;

namespace BSS.MVVM.ViewModel
{
    public class ManagementViewModel : BSSViewModel
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ManagementViewModel"/> class.
        /// </summary>
        /// <param name="systemParameters">The system configuration parameters.</param>
        /// <param name="dialogService">The dialog service.</param>
        /// <exception cref="System.ArgumentNullException">configurationParameters</exception>
        public ManagementViewModel(ConfigurationParameters systemParameters, IDialogService dialogService)
            : base(dialogService)
        {
            if (systemParameters == null)
            {
                throw new ArgumentNullException("systemParameters");
            }

            this.ApplyParametersCommand = new RelayCommand(ApplyParameters, () => !LocalParameters.Equals(SystemParameters));
            this.ResetCommand = new RelayCommand(Reset);
            this.RevertCommand = new RelayCommand(Revert, () => !LocalParameters.Equals(SystemParameters));
            this.ChassisPrivileges = new ObservableCollection<ChassisPrivilege>();
            this.SystemParameters = systemParameters;
            this.LocalParameters = new ConfigurationParameters();
            this.LocalParameters.Load();
            SetChassisPrivileges();

            this.LocalParameters.PropertyChanged += (sender, e) =>
                {
                    ApplyParametersCommand.RaiseCanExecuteChanged();
                    RevertCommand.RaiseCanExecuteChanged();

                    if (e.PropertyName == "ChassisCount")
                    {
                        SetChassisPrivileges();
                    }
                };
        }

        #endregion Public Constructors

        #region Public Properties

        public RelayCommand ApplyParametersCommand { get; private set; }

        public string[] ChassisNames
        {
            get
            {
                return ChassisPrivileges
                    .Select((cp, i) => String.Format(Resources.ChassisNumber, i))
                    .ToArray();
            }
        }

        public ObservableCollection<ChassisPrivilege> ChassisPrivileges { get; private set; }

        public ConfigurationParameters LocalParameters { get; private set; }

        public RelayCommand ResetCommand { get; private set; }

        public RelayCommand RevertCommand { get; private set; }

        /// <summary>
        /// Gets the configuration parameters.
        /// </summary>
        /// <value>
        /// The configuration parameters.
        /// </value>
        public ConfigurationParameters SystemParameters { get; private set; }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Returns a value indicating if user can exit page.
        /// </summary>
        /// <returns>
        ///   <c>true</c> if user can exit; otherwise, <c>false</c>.
        /// </returns>
        public override bool CanExit()
        {
            ConfigureChassisPrivileges();
            if (LocalParameters.Equals(SystemParameters))
            {
                return true;
            }

            bool ok = DialogService.ShowMessage(Resources.ConfigurationParametersChanged, Resources.AppName, Resources.OK, Resources.Cancel, null).Result;
            if (ok)
            {
                ApplyParameters();
            }

            return ok;
        }

        #endregion Public Methods

        #region Private Methods

        private void ApplyParameters()
        {
            ConfigureChassisPrivileges();
            LocalParameters.Save();
            SystemParameters.Load();

            ApplyParametersCommand.RaiseCanExecuteChanged();
            RevertCommand.RaiseCanExecuteChanged();
        }

        /// <summary>
        /// Store chassis privileges list in local configuration.
        /// </summary>
        private void ConfigureChassisPrivileges()
        {
            LocalParameters.ChassisCount = (byte)ChassisPrivileges.Count;
            for (byte chassisNumber = 1; chassisNumber <= ChassisPrivileges.Count; chassisNumber++)
            {
                LocalParameters.SetChassisMaterialGroup(chassisNumber, ChassisPrivileges[chassisNumber - 1].MaterialKind);
                LocalParameters.SetChassisOperations(chassisNumber, ChassisPrivileges[chassisNumber - 1].Operation);
            }
        }

        private void Reset()
        {
            LocalParameters.RestoreDefaults();

            ApplyParametersCommand.RaiseCanExecuteChanged();
            RevertCommand.RaiseCanExecuteChanged();
        }

        private void Revert()
        {
            LocalParameters.Load();

            ApplyParametersCommand.RaiseCanExecuteChanged();
            RevertCommand.RaiseCanExecuteChanged();
        }

        /// <summary>
        /// Sets the chassis privileges list.
        /// </summary>
        private void SetChassisPrivileges()
        {
            while (ChassisPrivileges.Count > LocalParameters.ChassisCount)
            {
                ChassisPrivileges.RemoveAt(ChassisPrivileges.Count - 1);
            }

            for (byte chassisNumber = (byte)ChassisPrivileges.Count; chassisNumber < LocalParameters.ChassisCount; chassisNumber++)
            {
                ChassisPrivileges.Add(new ChassisPrivilege());
            }

            for (byte chassisNumber = 1; chassisNumber <= ChassisPrivileges.Count; chassisNumber++)
            {
                ChassisPrivileges[chassisNumber - 1].Operation = LocalParameters.GetChassisOperations(chassisNumber);
                ChassisPrivileges[chassisNumber - 1].MaterialKind = LocalParameters.GetChassisMaterialGroup(chassisNumber);
            }
        }

        #endregion Private Methods
    }
}