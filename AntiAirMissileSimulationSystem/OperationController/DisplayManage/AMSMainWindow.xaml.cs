using OperationController.Data;
using System.Diagnostics;
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

namespace OperationController.DisplayManage
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class AMSMainWindow : Window
    {
        public AMSMainWindow()
        {
            InitializeComponent();
            EventLog.Text += "\n";
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            EventLog.Text += AMSConfiguration.GetInstance().Port() + "\n";
            EventLog.Text += AMSConfiguration.GetInstance().UpdateDuration() + "\n";
        }
    }
}