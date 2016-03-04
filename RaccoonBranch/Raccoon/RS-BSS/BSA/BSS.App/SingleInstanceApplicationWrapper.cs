using Microsoft.VisualBasic.ApplicationServices;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace BSS.App
{
    /// <summary>
    /// An application wrapper to make this application a single-instance.
    /// </summary>
    public class SingleInstanceApplicationWrapper : WindowsFormsApplicationBase
    {
        #region Private Fields

        /// <summary>
        /// The application.
        /// </summary>
        private App app;

        #endregion Private Fields

        #region Public Constructors

        /// <summary>
        /// Initializes a new instance of the <see cref="SingleInstanceApplicationWrapper"/> class.
        /// </summary>
        public SingleInstanceApplicationWrapper()
        {
            IsSingleInstance = true;
        }

        #endregion Public Constructors

        #region Protected Methods

        /// <summary>
        /// Raises the <see cref="E:Startup" /> event.
        /// </summary>
        /// <param name="e">The <see cref="StartupEventArgs"/> instance containing the event data.</param>
        /// <returns></returns>
        protected override bool OnStartup(StartupEventArgs e)
        {
            Process currentProcess = Process.GetCurrentProcess();
            Process[] appProcesses = Process.GetProcessesByName(currentProcess.ProcessName);
            foreach (var process in appProcesses)
            {
                if (process.Id != currentProcess.Id)
                {
                    process.Kill();
                }
            }

            app = new App();
            app.InitializeComponent();
            app.Run();
            return false;
        }

        /// <summary>
        /// Activates and maximizes window when a subsequent instance of a single-instance application starts.
        /// </summary>
        /// <param name="eventArgs"><see cref="T:Microsoft.VisualBasic.ApplicationServices.StartupNextInstanceEventArgs" />. Contains the command-line arguments of the subsequent application instance and indicates whether the first application instance should be brought to the foreground upon exiting the exception handler.</param>
        protected override void OnStartupNextInstance(StartupNextInstanceEventArgs eventArgs)
        {
            app.MainWindow.Activate();
            if (app.MainWindow.WindowState == System.Windows.WindowState.Minimized)
            {
                app.MainWindow.WindowState = System.Windows.WindowState.Maximized;
            }
        }

        #endregion Protected Methods
    }

    public class Startup
    {
        #region Public Methods

        [STAThread]
        public static void Main(string[] args)
        {
            SingleInstanceApplicationWrapper wrapper = new SingleInstanceApplicationWrapper();
            wrapper.Run(args);
        }

        #endregion Public Methods
    }
}
