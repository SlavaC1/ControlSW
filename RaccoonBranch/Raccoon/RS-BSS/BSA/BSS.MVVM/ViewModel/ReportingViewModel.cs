using BSS.Contracts;
using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.Report;
using BSS.MVVM.Properties;
using GalaSoft.MvvmLight.Command;
using GalaSoft.MvvmLight.Threading;
using GalaSoft.MvvmLight.Views;
using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Data;

namespace BSS.MVVM.ViewModel
{
    public enum BatchKey
    {
        Name,
        Tag
    }

    /// <summary>
    /// The reporting view model.
    /// </summary>
    public class ReportingViewModel : BSSViewModel
    {
        #region Private Fields

        private ObservableCollection<string> allBatches;

        private BatchKey batchKey;

        private BurningActionsStatistics burningActionsStatistics;

        private ConfigurationParameters configurationParameters;

        private string executingActionName;

        private bool isDirty;

        private LotHistoryProvider lotHistoryProvider;

        private string serialNumber;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="ReportingViewModel"/> class.
        /// </summary>
        /// <param name="lotHistoryProvider"></param>
        /// <param name="configurationParameters"></param>
        /// <param name="dialogService">The dialog service.</param>
        public ReportingViewModel(LotHistoryProvider lotHistoryProvider, ConfigurationParameters configurationParameters, IDialogService dialogService)
            : base(dialogService)
        {
            this.lotHistoryProvider = lotHistoryProvider;
            this.configurationParameters = configurationParameters;

            serialNumber = String.Empty;
            batchKey = BatchKey.Name;

            allBatches = new ObservableCollection<string>();
            AllBatchesView = new ListCollectionView(allBatches);
            ExportToExcelCommand = new RelayCommand(ExportToExcel, () => { return BurningActionsStatistics != null && executingActionName == null; });
            GetReportCommand = new RelayCommand(FillReport, () => { return AllBatchesView.CurrentItem != null && executingActionName == null; });
            RefreshCommand = new RelayCommand(RefreshBatchesList);
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets a view of all batches in DB.
        /// </summary>
        /// <value>
        /// All batches view.
        /// </value>
        public ListCollectionView AllBatchesView { get; private set; }

        public BatchKey BatchKey
        {
            get
            {
                return batchKey;
            }
            set
            {
                batchKey = value;
                RaisePropertyChanged(() => BatchKey);
            }
        }

        /// <summary>
        /// Gets or sets the burning actions statistics.
        /// </summary>
        /// <value>
        /// The burning actions statistics.
        /// </value>
        /// <exception cref="System.ArgumentNullException">value</exception>
        public BurningActionsStatistics BurningActionsStatistics
        {
            get
            {
                return burningActionsStatistics;
            }
            set
            {
                burningActionsStatistics = value;
                RaisePropertyChanged(() => BurningActionsStatistics);
            }
        }

        public string ExecutingActionName
        {
            get
            {
                return executingActionName;
            }
            set
            {
                executingActionName = value;
                RaisePropertyChanged(() => ExecutingActionName);
                GetReportCommand.RaiseCanExecuteChanged();
                ExportToExcelCommand.RaiseCanExecuteChanged();
            }
        }

        public RelayCommand ExportToExcelCommand { get; private set; }

        public RelayCommand GetReportCommand { get; private set; }

        public bool IsDirty
        {
            get
            {
                return isDirty;
            }
            set
            {
                isDirty = value;
                RaisePropertyChanged(() => IsDirty);
            }
        }

        public RelayCommand RefreshCommand { get; private set; }

        /// <summary>
        /// Gets or sets the batch number to query.
        /// </summary>
        /// <value>
        /// The batch number.
        /// </value>
        public string SerialNumber
        {
            get
            {
                return serialNumber;
            }
            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value");
                }

                serialNumber = value;
                RaisePropertyChanged(() => SerialNumber);
            }
        }

        #endregion Public Properties

        #region Private Methods

        /// <summary>
        /// Exports report table to excel.
        /// </summary>
        private async void ExportToExcel()
        {
            try
            {
                ExecutingActionName = Resources.ExportReportToExcel;
                Task<bool> exportTask;
                if (String.Equals(configurationParameters.ExportFileFormat, "csv", StringComparison.InvariantCultureIgnoreCase))
                {
                    exportTask = lotHistoryProvider.ExportToCsv(BurningActionsStatistics);
                }
                else
                {
                    exportTask = lotHistoryProvider.ExportToExcel(BurningActionsStatistics);
                }

                await exportTask.ConfigureAwait(continueOnCapturedContext: false);
            }
            finally
            {
                DispatcherHelper.CheckBeginInvokeOnUI(() =>
                    {
                        ExecutingActionName = null;
                    });
            }
        }

        /// <summary>
        /// Fills report table.
        /// </summary>
        private async void FillReport()
        {
            try
            {
                ExecutingActionName = Resources.FillingReport;
                BurningActionsStatistics = await lotHistoryProvider.ReadBatchHistory(AllBatchesView.CurrentItem as string);
            }
            finally
            {
                DispatcherHelper.CheckBeginInvokeOnUI(() =>
                {
                    ExecutingActionName = null;
                });
            }
        }

        /// <summary>
        /// Refreshes the batches list.
        /// </summary>
        private async void RefreshBatchesList()
        {
            if (IsDirty)
            {
                allBatches.Clear();
                string[] batchNumbers = await lotHistoryProvider.ReadAllBatchNumbers();
                foreach (var batchNumber in batchNumbers)
                {
                    allBatches.Add(batchNumber);
                }

                IsDirty = false;
            }

            if (batchKey == BatchKey.Name)
            {
                AllBatchesView.Filter = (o => SerialNumber != null & o != null && o.ToString().Contains(SerialNumber));
            }
            else if (batchKey == BatchKey.Tag)
            {
                var readBatchHistoryTasks = allBatches
                    .Select(batchNumber => Task<BurningActionsStatistics>.Run(async () =>
                        {
                            return await lotHistoryProvider.ReadBatchHistory(batchNumber).ConfigureAwait(false);
                        }))
                    .ToArray();

                Task.WaitAll(readBatchHistoryTasks);
                var readBatchHistoryTasksMapping = readBatchHistoryTasks
                    .Where(task => task.Result != null)
                    .ToDictionary(task => task.Result.MaterialInfo.BatchNumber);

                AllBatchesView.Filter = (o) =>
                    {
                        if (o == null)
                        {
                            return false;
                        }

                        Task<BurningActionsStatistics> readBatchHistoryTask;
                        bool found = readBatchHistoryTasksMapping.TryGetValue(o.ToString(), out readBatchHistoryTask);
                        if (!found)
                        {
                            return false;
                        }

                        return readBatchHistoryTask != null && readBatchHistoryTask.Result.Tags
                            .Any(ts => ts.TagInfo.SerialNumber.ToHexString().Contains(SerialNumber));
                    };
            }

            AllBatchesView.Refresh();
            AllBatchesView.MoveCurrentToFirst();
            GetReportCommand.RaiseCanExecuteChanged();
        }

        #endregion Private Methods
    }
}