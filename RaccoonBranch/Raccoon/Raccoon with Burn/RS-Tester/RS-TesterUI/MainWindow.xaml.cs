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

using RSTesterUI.ViewModels;

namespace RSTesterUI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            RSTesterViewModel rsTesterViewModel = TryFindResource("VM") as RSTesterViewModel;
            if (rsTesterViewModel != null)
            {
                rsTesterViewModel.Log.CollectionChanged += HandleLogChanged;
            }
        }

        void HandleLogChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (lvLog.Items.Count > 0)
            {
                lvLog.ScrollIntoView(lvLog.Items[lvLog.Items.Count - 1]);
            }
        }
    }
}
