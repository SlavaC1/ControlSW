using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.Model.BusinessLogic.Messages;
using BSS.MVVM.Model.BusinessLogic.Plc;
using BSS.MVVM.Model.BusinessLogic.Report;
using BSS.MVVM.Model.BusinessLogic.States;
using GalaSoft.MvvmLight.Messaging;
using RaccoonCLI;
using System;
using System.Linq;

namespace BSS.MVVM.Model.BusinessLogic
{
    /// <summary>
    /// A container of all models.
    /// </summary>
    public class MainModel : IDisposable
    {
        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="MainModel"/> class.
        /// </summary>
        public MainModel()
        {
            Messenger.Default.Register<ExceptionMessage>(this, SendExceptionMessage);
            Messenger.Default.Register<LogMessage>(this, SendMessage);

            ConfigurationParameters = new ConfigurationParameters();
            ConfigurationParameters.Load();

            LotHistoryProvider = new LotHistoryProvider();
            MaterialMonitor = new MaterialMonitorWrapper();
            PlcWrapper = PlcWrapperFactory.CreatePlcWrapper(ConfigurationParameters);
            InPlaceManager = new InPlaceManager(MaterialMonitor, ConfigurationParameters);

            CommunicationsManager = new CommunicationsManager(InPlaceManager, PlcWrapper);
            IdtBurner = new IdtBurner(ConfigurationParameters, InPlaceManager, MaterialMonitor, PlcWrapper);
            IdtCorrector = new IdtCorrector(ConfigurationParameters, InPlaceManager, MaterialMonitor, PlcWrapper);
            IdtReader = new IdtReader(ConfigurationParameters, InPlaceManager, MaterialMonitor, PlcWrapper);

            StatesManager = new StatesManager(this);
        }

        #endregion Public Constructors

        #region Public Properties

        /// <summary>
        /// Gets the communications manager.
        /// </summary>
        /// <value>
        /// The communications manager.
        /// </value>
        public CommunicationsManager CommunicationsManager { get; private set; }

        /// <summary>
        /// Gets the configuration parameters.
        /// </summary>
        /// <value>
        /// The configuration parameters.
        /// </value>
        public ConfigurationParameters ConfigurationParameters { get; private set; }

        /// <summary>
        /// Gets the IDT burner.
        /// </summary>
        /// <value>
        /// The IDT burner.
        /// </value>
        public IdtBurner IdtBurner { get; private set; }

        /// <summary>
        /// Gets the IDT corrector.
        /// </summary>
        /// <value>
        /// The IDT corrector.
        /// </value>
        public IdtCorrector IdtCorrector { get; private set; }

        /// <summary>
        /// Gets the IDT verifier.
        /// </summary>
        /// <value>
        /// The IDT verifier.
        /// </value>
        public IdtReader IdtReader { get; private set; }

        /// <summary>
        /// Gets the in-place manager.
        /// </summary>
        /// <value>
        /// The in-place manager.
        /// </value>
        public InPlaceManager InPlaceManager { get; private set; }

        /// <summary>
        /// Gets the lot history provider.
        /// </summary>
        /// <value>
        /// The lot history provider.
        /// </value>
        public LotHistoryProvider LotHistoryProvider { get; private set; }

        /// <summary>
        /// Gets the material monitor.
        /// </summary>
        /// <value>
        /// The material monitor.
        /// </value>
        public MaterialMonitorWrapper MaterialMonitor { get; private set; }

        /// <summary>
        /// Gets the PLC wrapper.
        /// </summary>
        /// <value>
        /// The PLC wrapper.
        /// </value>
        public IPlc PlcWrapper { get; private set; }

        /// <summary>
        /// Gets the states manager.
        /// </summary>
        /// <value>
        /// The states manager.
        /// </value>
        public StatesManager StatesManager { get; private set; }

        #endregion Public Properties

        #region Public Methods

        /// <summary>
        /// Sets the log message method.
        /// </summary>
        /// <param name="logMessageMethod">The log message method.</param>
        public void SetLogMessageMethod(Action<string> logMessageMethod)
        {
            MaterialMonitorWrapper.SetLogMessageMethod(logMessageMethod);
        }

        /// <summary>
        /// Stops all active operations.
        /// </summary>
        public void StopAll()
        {
            InPlaceManager.StopGetInPlaceStatus();
            IdtBurner.StopAll();
            IdtCorrector.StopAll();
            IdtReader.StopAll();
            PlcWrapper.Reset();
        }

        #endregion Public Methods

        #region Private Methods

        private void SendExceptionMessage(ExceptionMessage message)
        {
            Messenger.Default.Send(message, this);
        }

        private void SendMessage(LogMessage message)
        {
            if (ConfigurationParameters.ExecutionMode == ExecutionMode.Debug ||
                message.LogLevel >= Messages.Level.Info)
            {
                Messenger.Default.Send(message, this);
            }
        }

        #endregion Private Methods

        #region IDisposable Members

        /// <summary>
        /// Performs application-defined tasks associated with freeing, releasing, or resetting unmanaged resources.
        /// </summary>
        public void Dispose()
        {
            Dispose(dispose: true);
        }

        /// <summary>
        /// Releases unmanaged and - optionally - managed resources.
        /// </summary>
        /// <param name="dispose"><c>true</c> to release both managed and unmanaged resources; <c>false</c> to release only unmanaged resources.</param>
        protected virtual void Dispose(bool dispose)
        {
            if (dispose)
            {
                var disposables = GetType().GetProperties()
                    .Select(pi => pi.GetValue(this, null))
                    .Union(GetType().GetFields()
                        .Select(fi => fi.GetValue(this)))
                    .OfType<IDisposable>()
                    .ToArray();

                Array.ForEach(disposables, i => { i.Dispose(); });

                Messenger.Default.Unregister(this);
            }
        }

        #endregion IDisposable Members
    }
}