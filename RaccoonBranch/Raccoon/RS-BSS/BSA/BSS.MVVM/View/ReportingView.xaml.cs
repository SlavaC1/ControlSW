using BSS.MVVM.ViewModel;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace BSS.MVVM.View
{
    /// <summary>
    /// Interaction logic for ReportingView.xaml
    /// </summary>
    public partial class ReportingView : UserControl
    {
        public ReportingView()
        {
            InitializeComponent();
        }

        private void UserControl_Loaded(object sender, RoutedEventArgs e)
        {
            ReportingViewModel vm = this.DataContext as ReportingViewModel;
            if (vm != null)
            {
                vm.IsDirty = true;
                vm.RefreshCommand.Execute(String.Empty);
            }

            brdBurningActionsStatistics.Measure(new Size(Double.PositiveInfinity, Double.PositiveInfinity));
            topRow.MinHeight = brdBurningActionsStatistics.DesiredSize.Height;
        }
    }
}
