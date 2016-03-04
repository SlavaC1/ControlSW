using GalaSoft.MvvmLight.Threading;
using log4net;
using System;
using System.Configuration;
using System.Diagnostics;
using System.Globalization;
using System.Windows;
using System.Windows.Markup;
using System.Windows.Threading;
using AppResources = BSS.App.Properties.Resources;

namespace BSS.App
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        #region Public Constructors

        /// <summary>
        /// Initializes the <see cref="App"/> class.
        /// </summary>
        static App()
        {
            DispatcherHelper.Initialize();
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="App"/> class.
        /// </summary>
        public App()
        {
            AppDomain.CurrentDomain.UnhandledException += HandleUnhandledException;
            Dispatcher.UnhandledException += HandleUnhandledException;
            CultureInfo culture = CultureInfo.CurrentUICulture;

            try
            {
                System.Configuration.Configuration config = ConfigurationManager.OpenExeConfiguration(ConfigurationUserLevel.None);
                KeyValueConfigurationElement configElement;
                configElement = config.AppSettings.Settings["Culture"];
                if (configElement != null)
                {
                    culture = CultureInfo.GetCultureInfo(configElement.Value);
                }
            }
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
            }

            try
            {
                CultureInfo.DefaultThreadCurrentCulture = culture;
                CultureInfo.DefaultThreadCurrentUICulture = culture;

                FrameworkElement.LanguageProperty.OverrideMetadata(
                    typeof(FrameworkElement),
                    new FrameworkPropertyMetadata(
                        XmlLanguage.GetLanguage(CultureInfo.CurrentCulture.IetfLanguageTag)));
            }
            catch (Exception ex)
            {
                Trace.WriteLine(ex);
            }
        }

        #endregion Public Constructors

        #region Private Methods

        private void HandleUnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            LogException(e.ExceptionObject as Exception);
        }

        private void HandleUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            LogException(e.Exception);
            e.Handled = true;
        }

        private void LogException(Exception ex)
        {
            if (ex != null)
            {
                var log = LogManager.GetLogger("root");
                if (log != null)
                {
                    log.Error(AppResources.UnhandledException, ex);
                }

                MessageBox.Show(ex.Message, AppResources.AppName);
            }
        }

        #endregion Private Methods
    }
}