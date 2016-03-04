using GalaSoft.MvvmLight;
using System.Collections.ObjectModel;

namespace BSS.MVVM.ViewModel
{
    public class LogViewModel : ViewModelBase
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="LogViewModel"/> class.
        /// </summary>
        public LogViewModel()
        {
            this.Log = new ObservableCollection<object>();
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the log.
        /// </summary>
        /// <value>
        /// The log.
        /// </value>
        public ObservableCollection<object> Log
        {
            get;
            private set;
        }

        #endregion Public Properties
    }
}