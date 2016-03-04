/*
  In App.xaml:
  <Application.Resources>
      <vm:ViewModelLocatorTemplate xmlns:vm="clr-namespace:BSS.App.ViewModel"
                                   x:Key="Locator" />
  </Application.Resources>
  
  In the View:
  DataContext="{Binding Source={StaticResource Locator}, Path=ViewModelName}"
*/

using BSS.App.Properties;
using BSS.App.Services;
using BSS.MVVM.Model.BusinessLogic;
using BSS.MVVM.Model.BusinessLogic.IdtSrv;
using BSS.MVVM.ViewModel;
using GalaSoft.MvvmLight;
using GalaSoft.MvvmLight.Ioc;
using GalaSoft.MvvmLight.Views;
using Microsoft.Practices.ServiceLocation;
using System;
using System.Threading;
using System.Threading.Tasks;

//Here is the once-per-application setup information
[assembly: log4net.Config.XmlConfigurator(Watch = true)]

namespace BSS.App.ViewModel
{
    /// <summary>
    /// This class contains static references to all the view models in the
    /// application and provides an entry point for the bindings.
    /// <para>
    /// See http://www.galasoft.ch/mvvm
    /// </para>
    /// </summary>
    public class ViewModelLocator
    {
        /// <summary>
        /// The main view-model.
        /// </summary>
        private MainViewModel main;

        /// <summary>
        /// Initializes the <see cref="ViewModelLocator"/> class.
        /// </summary>
        static ViewModelLocator()
        {
            ServiceLocator.SetLocatorProvider(() => SimpleIoc.Default);

            SimpleIoc.Default.Register<IDialogService, ModalDialogService>(true);
            SimpleIoc.Default.Register<MainModel>(() =>
                {
                    try
                    {
                        return new MainModel();
                    }
                    catch (Exception ex)
                    {
                        IDialogService dialogService = SimpleIoc.Default.GetInstance<IDialogService>();
                        dialogService.ShowError(ex, Resources.AppName, Resources.OK, null);
                        App.Current.MainWindow.Close();
                        return null;
                    }
                });

            SimpleIoc.Default.Register<MainViewModel>();
        }

        /// <summary>
        /// Gets the Main property.
        /// </summary>
        [System.Diagnostics.CodeAnalysis.SuppressMessage("Microsoft.Performance",
            "CA1822:MarkMembersAsStatic",
            Justification = "This non-static member is needed for data binding purposes.")]
        public MainViewModel Main
        {
            get
            {
                if (main == null)
                {
                    main = ServiceLocator.Current.GetInstance<MainViewModel>();
                    main.Start().ConfigureAwait(continueOnCapturedContext: false);
                }

                return main;
            }
        }

        /// <summary>
        /// Cleans up all the resources.
        /// </summary>
        public static void Cleanup()
        {
            ServiceLocator.Current.GetInstance<MainViewModel>().Cleanup();
        }
    }
}