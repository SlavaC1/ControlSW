using BSS.App.Appenders;
using BSS.App.Properties;
using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.States;
using BSS.MVVM.ViewModel;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Views;
using log4net;
using System;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;

namespace BSS.App.ViewModel
{
    /// <summary>
    /// This class contains properties that the main View can data bind to.
    /// <para>
    /// See http://www.galasoft.ch/mvvm
    /// </para>
    /// </summary>
    public class MainViewModel : ViewModelBase
    {
        #region Private Fields

        private const string IdtSrvPrefix = "Raccoon driver:";

        /// <summary>
        /// A reference to logger service.
        /// </summary>
        private static ILog log;

        /// <summary>
        /// The dialog service.
        /// </summary>
        private IDialogService dialogService;

        /// <summary>
        /// The main model.
        /// </summary>
        private MainModel mainModel;

        /// <summary>
        /// The selected view model.
        /// </summary>
        private BSSViewModel selectedViewModel;

        /// <summary>
        /// The status.
        /// </summary>
        private object status;

        /// <summary>
        /// The view models.
        /// </summary>
        private BSSViewModel[] viewModels;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="MainViewModel"/> class.
        /// </summary>
        /// <param name="dialogService">The dialog service.</param>
        /// <param name="mainModel">The main model.</param>
        /// <exception cref="System.ArgumentNullException">
        /// mainModel
        /// or
        /// dialogService
        /// </exception>
        public MainViewModel(IDialogService dialogService, MainModel mainModel)
        {
            if (mainModel == null)
            {
                throw new ArgumentNullException("mainModel");
            }

            if (dialogService == null)
            {
                throw new ArgumentNullException("dialogService");
            }

            this.mainModel = mainModel;

            this.mainModel.SetLogMessageMethod(WriteMessageToLog);
            this.mainModel.IdtCorrector.UserConfirmationPredicate = AskUser;
            this.mainModel.StatesManager.UserConfirmationPredicate = AskUser;
            this.mainModel.StatesManager.DisplayMessageDel = DisplayMessage;
            this.mainModel.StatesManager.CurrentStateChanged += HandleCurrentStateChanged;

            this.dialogService = dialogService;

            SelectModeCommand = new RelayCommand<BssStateID>(
                TransitState,
                (stateID) =>
                {
                    BssState state = this.mainModel.StatesManager.GetState(stateID);
                    if (!state.IsAccessible)
                    {
                        return false;
                    }

                    return !CurrentSystemState.IsExitAllowed();
                });
            ExitCommand = new RelayCommand(
                ExitState,
                () =>
                {
                    return CurrentSystemState.IsExitAllowed();
                });

            viewModels = new BSSViewModel[Enum.GetValues(typeof(BssStateID)).Length];
            viewModels[(int)BssStateID.Idle] = new IdtIdleViewModel(dialogService);
            viewModels[(int)BssStateID.Burning] = new IdtBurningViewModel(mainModel.IdtBurner, mainModel.CommunicationsManager, dialogService);
            viewModels[(int)BssStateID.Correction] = new IdtCorrectionViewModel(mainModel.IdtCorrector, mainModel.CommunicationsManager, dialogService);
            viewModels[(int)BssStateID.Verification] = new IdtReadingViewModel(mainModel.IdtReader, mainModel.CommunicationsManager, dialogService);
            viewModels[(int)BssStateID.Report] = new ReportingViewModel(mainModel.LotHistoryProvider, mainModel.ConfigurationParameters, dialogService);
            viewModels[(int)BssStateID.Management] = new ManagementViewModel(mainModel.ConfigurationParameters, dialogService);

            CommunicationViewModel = new CommunicationViewModel(mainModel.CommunicationsManager, mainModel.ConfigurationParameters.Domain, dialogService);
            LotViewModel = new LotViewModel(mainModel.IdtBurner);
            BurningPaneViewModel = new BurningPaneViewModel(mainModel.IdtBurner, mainModel.StatesManager);

            LogViewModel = new LogViewModel();
            InitLogger();

            RegisterMessages();
            selectedViewModel = ViewModels.FirstOrDefault();
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the burning statistics view model.
        /// </summary>
        /// <value>
        /// The burning statistics view model.
        /// </value>
        public BurningPaneViewModel BurningPaneViewModel
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the communication view model.
        /// </summary>
        /// <value>
        /// The communication view model.
        /// </value>
        public CommunicationViewModel CommunicationViewModel
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the current system state.
        /// </summary>
        /// <value>
        /// The current system state.
        /// </value>
        public BssState CurrentSystemState
        {
            get
            {
                return mainModel.StatesManager.CurrentState;
            }
        }

        public RelayCommand ExitCommand
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the log view model.
        /// </summary>
        /// <value>
        /// The log view model.
        /// </value>
        public LogViewModel LogViewModel
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the Lot view model.
        /// </summary>
        /// <value>
        /// The Lot view model.
        /// </value>
        public LotViewModel LotViewModel
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets the previous system state.
        /// </summary>
        /// <value>
        /// The previous system state.
        /// </value>
        public BssState PreviousSystemState
        {
            get
            {
                return mainModel.StatesManager.PreviousState;
            }
        }

        /// <summary>
        /// Gets or sets the selected view model.
        /// </summary>
        /// <value>
        /// The selected view model.
        /// </value>
        public BSSViewModel SelectedViewModel
        {
            get
            {
                return selectedViewModel;
            }
            set
            {
                if (selectedViewModel != value)
                {
                    if (selectedViewModel != null)
                    {
                        selectedViewModel.Deactivate();
                    }

                    selectedViewModel = value;
                    if (selectedViewModel != null)
                    {
                        selectedViewModel.Activate();
                    }

                    RaisePropertyChanged(() => SelectedViewModel);
                }
            }
        }

        /// <summary>
        /// Gets the select mode command.
        /// </summary>
        /// <value>
        /// The select mode command.
        /// </value>
        public RelayCommand<BssStateID> SelectModeCommand
        {
            get;
            private set;
        }

        /// <summary>
        /// Gets or sets the status.
        /// </summary>
        /// <value>
        /// The status.
        /// </value>
        public object Status
        {
            get
            {
                return status;
            }
            set
            {
                status = value;
                RaisePropertyChanged(() => Status);
            }
        }

        /// <summary>
        /// Gets the view models.
        /// </summary>
        /// <value>
        /// The view models.
        /// </value>
        public BSSViewModel[] ViewModels
        {
            get
            {
                return viewModels;
            }
            private set
            {
                viewModels = value;
                RaisePropertyChanged(() => ViewModels);
            }
        }

        #endregion Public Properties

        #region Private Properties

        /// <summary>
        /// Gets the log.
        /// </summary>
        /// <value>
        /// The log.
        /// </value>
        private ILog Log
        {
            get
            {
                if (log == null)
                {
                    log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
                    log4net.Core.LogImpl logImpl = log as log4net.Core.LogImpl;
                    if (logImpl != null)
                    {
                        LogViewModelAppender logViewModelAppender =
                            logImpl.Logger.Repository.GetAppenders().OfType<LogViewModelAppender>().FirstOrDefault();

                        if (logViewModelAppender != null)
                        {
                            logViewModelAppender.Target = LogViewModel.Log;
                        }
                    }
                }

                return log;
            }
        }

        #endregion Private Properties

        #region Public Methods

        /// <summary>
        /// Unregisters this instance from the Messenger class.
        /// <para>To cleanup additional resources, override this method, clean
        /// up and then call base.Cleanup().</para>
        /// </summary>
        public override void Cleanup()
        {
            mainModel.StopAll();

            mainModel.SetLogMessageMethod(null);

            foreach (var viewModel in viewModels)
            {
                if (viewModel != null)
                {
                    viewModel.Cleanup();
                }
            }

            BurningPaneViewModel.Cleanup();
            LogViewModel.Cleanup();
            LotViewModel.Cleanup();
            LogManager.Shutdown();

            base.Cleanup();
        }

        /// <summary>
        /// Sets main view-model to its initial state.
        /// </summary>
        /// <returns></returns>
        public async Task Start()
        {
            if (selectedViewModel != null)
            {
                selectedViewModel.Activate();
            }

            await mainModel.CommunicationsManager.ConnectAll();
        }

        #endregion Public Methods

        #region Private Methods

        /// <summary>
        /// Converts message from IdtSrv module:
        /// remove identification prefix and converts cartridge number from range [0..count-1] to [1..count].
        /// </summary>
        /// <param name="idtSrvMessage">The idt SRV message.</param>
        /// <returns></returns>
        private static string ConvertIdtSrvMessage(string idtSrvMessage, out Level logLevel)
        {
            // Remove module name:
            string message = idtSrvMessage.Remove(0, IdtSrvPrefix.Length);

            // Remove log level:
            logLevel = Level.Info;
            int messageStartIndex = message.IndexOf(": ") + 1;
            if (messageStartIndex > 0)
            {
                Enum.TryParse(message.Substring(0, messageStartIndex - 1), ignoreCase: true, result: out logLevel);
                messageStartIndex++;
            }
            message = message.Substring(messageStartIndex);

            // Fix cartridge number offset:
            int cartridgeNumStartIndex = message.IndexOf('#') + 1;
            if (cartridgeNumStartIndex > 0)
            {
                int length = 0;
                while (cartridgeNumStartIndex + length < message.Length &&
                    message[cartridgeNumStartIndex + length] >= '0' &&
                    message[cartridgeNumStartIndex + length] <= '9')
                {
                    length++;
                }

                byte cartridgeNum;
                bool ok = Byte.TryParse(message.Substring(cartridgeNumStartIndex, length), out cartridgeNum);
                if (ok)
                {
                    message = message.Remove(cartridgeNumStartIndex, length);
                    message = message.Insert(cartridgeNumStartIndex, (cartridgeNum + 1).ToString());
                }
            }

            return message;
        }

        private bool AskUser(string message)
        {
            Task<bool> showMessageTask = dialogService.ShowMessage(message, Resources.AppName, Resources.Yes, Resources.No, null);
            showMessageTask.Wait();
            return showMessageTask.Result;
        }

        private void DisplayMessage(string message)
        {
            Task displayMessageTask = dialogService.ShowMessage(message, Resources.AppName);
            displayMessageTask.Wait();
        }

        private async void ExitState()
        {
            bool canExit = SelectedViewModel.CanExit();
            if (!canExit)
            {
                return;
            }

            BssState previousState = PreviousSystemState;
            do
            {
                try
                {
                    await mainModel.StatesManager.ExitState();
                }
                catch (Exception ex)
                {
                    MessengerUtils.SendException(ex);
                }
            } while (CurrentSystemState != previousState);
        }

        private void HandleCurrentStateChanged(object sender, EventArgs e)
        {
            DispatcherHelper.CheckBeginInvokeOnUI(() =>
            {
                RaisePropertyChanged(() => CurrentSystemState);
                SelectModeCommand.RaiseCanExecuteChanged();
                ExitCommand.RaiseCanExecuteChanged();
                CommunicationViewModel.AllowConnection = (CurrentSystemState.StateID == BssStateID.Management);
                SelectedViewModel = viewModels[(int)mainModel.StatesManager.CurrentState.StateID];
            });
        }

        private void InitLogger()
        {
            log = LogManager.GetLogger(MethodBase.GetCurrentMethod().DeclaringType);
            if (log != null)
            {
                LogViewModelAppender logViewModelAppender =
                    log.Logger.Repository.GetAppenders().OfType<LogViewModelAppender>().FirstOrDefault();

                if (logViewModelAppender != null)
                {
                    logViewModelAppender.Target = LogViewModel.Log;
                }
            }
        }

        private void RegisterMessage<TMessage>(Action<TMessage> action)
        {
            MessengerInstance.Register<TMessage>(this, this.mainModel, action);
        }

        private void RegisterMessages()
        {
            RegisterMessage<ExceptionMessage>(
                (message) =>
                {
                    Log.Error(message.Exception.Message, message.Exception);
                    dialogService.ShowError(message.Exception, Resources.AppName, Resources.OK, null);
                });

            RegisterMessage<LogMessage>(
                (message) =>
                {
                    switch (message.LogLevel)
                    {
                        case Level.Debug:
                            Log.Debug(message.Message);
                            break;

                        case Level.Info:
                            Log.Info(message.Message);
                            break;

                        case Level.Error:
                            Log.Error(message.Message);
                            break;

                        default:
                            break;
                    }
                });
        }

        /// <summary>
        /// Transits the system state.
        /// </summary>
        /// <param name="newStateID">The new state.</param>
        private async void TransitState(BssStateID newStateID)
        {
            bool canExit = SelectedViewModel.CanExit();
            if (!canExit)
            {
                return;
            }

            bool actionRequired = false;
            do
            {
                try
                {
                    await mainModel.StatesManager.Transit(newStateID);
                }
                catch (Exception ex)
                {
                    InvalidStateTransitionException istex = ex as InvalidStateTransitionException;
                    actionRequired = istex != null && istex.ActionRequired;
                    MessengerUtils.SendException(ex);
                }
            } while (CurrentSystemState.StateID != newStateID && actionRequired);

            RaisePropertyChanged(() => CurrentSystemState);
        }

        /// <summary>
        /// Writes the message to log.
        /// </summary>
        /// <param name="message">The message.</param>
        private void WriteMessageToLog(string message)
        {
            if (message != null &&
                message.StartsWith(IdtSrvPrefix))
            {
                Level logLevel;
                message = ConvertIdtSrvMessage(message, out logLevel);
                if (logLevel >= Level.Info)
                {
                    // Report progress:
                    IdtOperationViewModel selectedIdtOperationViewModel = SelectedViewModel as IdtOperationViewModel;
                    if (selectedIdtOperationViewModel != null)
                    {
                        selectedIdtOperationViewModel.ReportProgress();

                        // log verification steps in debug level:
                        if (selectedIdtOperationViewModel == viewModels[(int)BssStateID.Verification])
                        {
                            Log.Debug(message);
                        }
                    }
                }
            }

            Status = message;
        }

        #endregion Private Methods
    }
}