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
using OperationController.AMSUDP;
using System;

namespace TCCSimulator
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private nFrameworkConnector nf = null;
        public MainWindow()
        {
            Console.WriteLine("MainWindow called");
            InitializeComponent();
            EventLog.Text += "\n";
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (nf == null)
            {
                nf = new nFrameworkConnector(this);
                Console.WriteLine("nf = new() called");
            }
            nf.SendMsg();
        }
    }
}