using CBS.BSP;
using CBS.BSP.HSM;
using CBS.BSP.Report;
using CBS.BSP.TagInfoProvider;
using CBS.BSP.VLL;
using CBS.BspHostService.Properties;
using log4net;
using System.Collections.Generic;
using System.ServiceModel;
using System.ServiceProcess;

namespace CBS.BspHostService
{
    /// <summary>
    /// A windows service of burning station proxy.
    /// </summary>
    public partial class BspHost : ServiceBase
    {
        #region Private Fields

        /// <summary>
        /// The WCF service hosts.
        /// </summary>
        private List<ServiceHost> wcfServiceHosts;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="BspHost"/> class.
        /// </summary>
        public BspHost()
        {
            InitializeComponent();
            wcfServiceHosts = new List<ServiceHost>();
        }

        #endregion Public Constructors

        #region Protected Methods

        /// <summary>
        /// Executes when a Start command is sent to the service by the Service Control Manager (SCM) or when the operating system starts (for a service that starts automatically). Specifies actions to take when the service starts.
        /// </summary>
        /// <param name="args">Data passed by the start command.</param>
        protected override void OnStart(string[] args)
        {
            var log = LogManager.GetLogger("root");
            log.Info(Resources.BspServicesHostStarted);

            CloseAllServiceHosts();
            OpenAllServiceHosts();
        }

        /// <summary>
        /// Executes when a Stop command is sent to the service by the Service Control Manager (SCM). Specifies actions to take when a service stops running.
        /// </summary>
        protected override void OnStop()
        {
            CloseAllServiceHosts();

            var log = LogManager.GetLogger("root");
            log.Info(Resources.BspServicesHostStopped);

            LogManager.Shutdown();
        }

        #endregion Protected Methods

        #region Private Methods

        /// <summary>
        /// Closes all service hosts.
        /// </summary>
        private void CloseAllServiceHosts()
        {
            foreach (var serviceHost in wcfServiceHosts)
            {
                if (serviceHost != null)
                {
                    serviceHost.Close();
                }
            }

            wcfServiceHosts.Clear();
        }

        /// <summary>
        /// Opens all service hosts.
        /// </summary>
        private void OpenAllServiceHosts()
        {
            wcfServiceHosts.Clear();
            wcfServiceHosts.AddRange(new[]
            {
                new ServiceHost(typeof(HsmService)),
                new ServiceHost(typeof(TagInfoProviderService)),
                new ServiceHost(typeof(ReportService)),
                new ServiceHost(typeof(VllService))
            });

            foreach (var serviceHost in wcfServiceHosts)
            {
                serviceHost.Open();
            }
        }

        #endregion Private Methods
    }
}