using RSTesterCLI;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Linq;
using System.Net;
using System.Text;
using System.Windows.Threading;

namespace RSTesterUI.ViewModels
{
    class RSTesterViewModel : INotifyPropertyChanged
    {
        #region Fields

        private Dispatcher _uiDispatcher;

        private RSTesterUICLI _rsTesterUICLI;

        private bool _connected;

        private string _endPoint;

        private ObservableCollection<FileDetails> _testNames;

        private ObservableCollection<string> _log;

        #endregion

        #region Constructor

        public RSTesterViewModel()
        {
            _uiDispatcher = Dispatcher.CurrentDispatcher;

            _rsTesterUICLI = new RSTesterUICLI();
            _rsTesterUICLI.LogArrived += _rsTesterUICLI_LogArrived;

            EndPoint = "tcp://localhost:5555";
            TestNames = new ObservableCollection<FileDetails>();
            Log = new ObservableCollection<string>();

            ConnectCommand = new Command(Connect) { Text = "_Connect" };
            SelectAllCommand = new Command(SelectAll);
            RunTestsCommand = new AsynchronousCommand(RunTests, CanExecuteRunTests()) { Text = "_Run Tests..." };
            RunTestsCommand.Executed += RunTestsCommand_Executed;
        }

        void RunTestsCommand_Executed(object sender, CommandEventArgs args)
        {
            _uiDispatcher.BeginInvoke(
                new Action<bool>((canExecute) => RunTestsCommand.CanExecute = canExecute),
                CanExecuteRunTests());
        }

        void _rsTesterUICLI_LogArrived(object sender, LogEventArgs e)
        {
            _uiDispatcher.BeginInvoke(
                new Action<string>((logEvent) => Log.Add(logEvent.Replace(Environment.NewLine, String.Empty).Replace("\n", String.Empty))),
                e.GetLogEvent());
        }

        #endregion

        #region Properties

        public bool Connected
        {
            get
            {
                return _connected;
            }
            set
            {
                _connected = value;
                if (!_connected)
                {
                    TestNames.Clear();
                }

                ConnectCommand.Text = (_connected) ? "_Disconnect" : "_Connect";
                OnPropertyChanged(this, new PropertyChangedEventArgs("Connected"));
            }
        }

        public string EndPoint
        {
            get
            {
                return _endPoint;
            }
            set
            {
                _endPoint = value;
                OnPropertyChanged(this, new PropertyChangedEventArgs("EndPoint"));
            }
        }

        public ObservableCollection<FileDetails> TestNames
        {
            get
            {
                return _testNames;
            }
            private set
            {
                if (_testNames != null)
                {
                    _testNames.CollectionChanged -= HandleTestNamesCollectionChanged;
                }

                _testNames = value;
                if (_testNames != null)
                {
                    _testNames.CollectionChanged += HandleTestNamesCollectionChanged;
                }

                OnPropertyChanged(this, new PropertyChangedEventArgs("TestNames"));
            }
        }

        private void HandleTestNamesCollectionChanged(object sender, NotifyCollectionChangedEventArgs e)
        {
            SelectAllCommand.CanExecute = TestNames.Any();
            RunTestsCommand.CanExecute = CanExecuteRunTests();
            if (e.OldItems != null)
            {
                foreach (FileDetails item in e.OldItems)
                {
                    item.PropertyChanged -= HandleTestFileDetailsPropertyChanged;
                }
            }

            if (e.NewItems != null)
            {
                foreach (FileDetails item in e.NewItems)
                {
                    item.PropertyChanged += HandleTestFileDetailsPropertyChanged;
                }
            }
        }

        void HandleTestFileDetailsPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Selected")
            {
                RunTestsCommand.CanExecute = CanExecuteRunTests();
            }
        }

        public ObservableCollection<string> Log
        {
            get
            {
                return _log;
            }
            private set
            {
                _log = value;
                OnPropertyChanged(this, new PropertyChangedEventArgs("Log"));
            }
        }

        #endregion

        #region Commands

        public Command ConnectCommand { get; private set; }

        public Command SelectAllCommand { get; private set; }

        public AsynchronousCommand RunTestsCommand { get; private set; }

        #endregion

        #region Events

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion

        #region Command Executions

        private void Connect()
        {
            if (!Connected)
            {
                TestNames.Clear();

                Connected = _rsTesterUICLI.Connect(EndPoint);
                if (Connected)
                {
                    StringBuilder sb = new StringBuilder();
                    _rsTesterUICLI.GetTestFiles(sb);
                    string[] fileNames = sb.ToString().Split(new string[] { Environment.NewLine }, StringSplitOptions.RemoveEmptyEntries);
                    foreach (string fileName in fileNames)
                    {
                        TestNames.Add(new FileDetails { Name = fileName, Selected = true });
                    }
                }
            }
            else
            {
                _rsTesterUICLI.Disconnect();
                Connected = false;
            }
        }

        private void SelectAll(object selectParameter)
        {
            bool select = Convert.ToBoolean(selectParameter.ToString());
            foreach (var item in TestNames)
            {
                item.Selected = select;
            }
        }

        private void RunTests()
        {
            _uiDispatcher.BeginInvoke(
                new Action<bool>((canExecute) => RunTestsCommand.CanExecute = canExecute),
                CanExecuteRunTests());

            var fileNames = TestNames
                .Where(fd => fd.Selected)
                .Select(fd => fd.Name);

            _rsTesterUICLI.RunTests(fileNames);
        }

        private bool CanExecuteRunTests()
        {
            return
                Connected &&
                !RunTestsCommand.IsExecuting &&
                TestNames.Any(fd => fd.Selected);
        }

        #endregion

        #region Event Raisers

        protected virtual void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            PropertyChangedEventHandler temp = PropertyChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }

        #endregion
    }

    class FileDetails : INotifyPropertyChanged
    {
        private string _name;
        public string Name
        {
            get
            {
                return _name;
            }
            set
            {
                _name = value;
                OnPropertyChanged(this, new PropertyChangedEventArgs("Name"));
            }
        }

        private bool _selected;
        public bool Selected
        {
            get
            {
                return _selected;
            }
            set
            {
                _selected = value;
                OnPropertyChanged(this, new PropertyChangedEventArgs("Selected"));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected virtual void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            PropertyChangedEventHandler temp = PropertyChanged;
            if (temp != null)
            {
                temp(this, e);
            }
        }
    }
}
