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
using System.Numerics;
using OperationController.AMSUDP;
using System;

namespace OperationController.DisplayManage
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

        //---------------------------------------------------------------
        // 공중위협 시작 좌표 출력 및 지도에서 클릭시 고정 출력값 설정
        private short fixAirThreatPox = 0;

        private double fixedAirThreatStartPosX = 0.0;
        private double fixedAirThreatStartPosY = 0.0;

        private double fixedAirThreatEndPosX = 0.0;
        private double fixedAirThreatEndPosY = 0.0;

        private double fixedMSLStartPosX = 0.0;
        private double fixedMSLStartPosY = 0.0;

        private double fixedAirThreatSpeed = 0.0;
        private double fixedMSLSpeed = 0.0;

        // 공중위협 출발지 좌표 설정 버튼 클릭 시
        // 지도 위에서 좌표 클릭하는 이벤트를 실행 시키기 위한 멤버함수
        private void ATStartSetting_Click(object sender, RoutedEventArgs e)
        {
            fixAirThreatPox = 1;
        }
        private void ATEndSetting_Click(object sender, RoutedEventArgs e)
        {
            fixAirThreatPox = 2;
        }

        private void ATPos_MouseLeftButtonDown(object sender, MouseButtonEventArgs e)
        {
            Point mousePosition = e.GetPosition(myCanvas);

            double relativeX = mousePosition.X;
            double relativeY = mousePosition.Y;

            if (fixAirThreatPox == 1)
            {
                // Store the current mouse position when the Label is clicked
                fixedAirThreatStartPosX = relativeX;
                fixedAirThreatStartPosY = relativeY;
                // Set the flag to prevent updating the label content
                fixAirThreatPox = 4;
            }
            else if (fixAirThreatPox == 2)
            {
                // Store the current mouse position when the Label is clicked
                fixedAirThreatEndPosX = relativeX;
                fixedAirThreatEndPosY = relativeY;
                // Set the flag to prevent updating the label content
                fixAirThreatPox = 5;
            }
            else if (fixAirThreatPox == 3)
            {
                // Store the current mouse position when the Label is clicked
                fixedMSLStartPosX = relativeX;
                fixedMSLStartPosY = relativeY;
                // Set the flag to prevent updating the label content
                fixAirThreatPox = 6;
            }
        }

        // 공중위협 설정한 출발지 좌표값 출력창에서 입력한 좌표값으로 변경
        private void Canvas_MouseMove(object sender, System.Windows.Input.MouseEventArgs e)
        {
            Point mousePosition = e.GetPosition(myCanvas);

            double relativeX = mousePosition.X;
            double relativeY = mousePosition.Y;

            if (fixAirThreatPox == 1)
            {
                ATStartPosX.Content = $"{relativeX:F3}";
                ATStartPosY.Content = $"{relativeY:F3}";

                // Update the position of the TextBox to follow the mouse cursor
                mousePositionTextBox.Margin = new Thickness(mousePosition.X + 16, mousePosition.Y + 16, 0, 0);

                // Update the text of the TextBox to display the current mouse position
                mousePositionTextBox.Text = $"X: {relativeX:F3}, Y: {relativeY:F3}";

                // Ensure the TextBox is visible
                mousePositionTextBox.Visibility = Visibility.Visible;

            }
            else if (fixAirThreatPox == 2)
            {
                ATEndPosX.Content = $"{relativeX:F3}";
                ATEndPosY.Content = $"{relativeY:F3}";

                // Update the position of the TextBox to follow the mouse cursor
                mousePositionTextBox.Margin = new Thickness(mousePosition.X + 16, mousePosition.Y + 16, 0, 0);

                // Update the text of the TextBox to display the current mouse position
                mousePositionTextBox.Text = $"X: {relativeX:F3}, Y: {relativeY:F3}";

                // Ensure the TextBox is visible
                mousePositionTextBox.Visibility = Visibility.Visible;

            }
            else if (fixAirThreatPox == 3)
            {
                MSLStartPosX.Content = $"{relativeX:F3}";
                MSLStartPosY.Content = $"{relativeY:F3}";

                // Update the position of the TextBox to follow the mouse cursor
                mousePositionTextBox.Margin = new Thickness(mousePosition.X + 16, mousePosition.Y + 16, 0, 0);

                // Update the text of the TextBox to display the current mouse position
                mousePositionTextBox.Text = $"X: {relativeX:F3}, Y: {relativeY:F3}";

                // Ensure the TextBox is visible
                mousePositionTextBox.Visibility = Visibility.Visible;

            }
            else if (fixAirThreatPox == 4)
            {
                ATStartPosX.Content = $"{fixedAirThreatStartPosX:F3}";
                ATStartPosY.Content = $"{fixedAirThreatStartPosY:F3}";

                mousePositionTextBox.Visibility = Visibility.Hidden;
            }
            else if (fixAirThreatPox == 5)
            {
                ATEndPosX.Content = $"{fixedAirThreatEndPosX:F3}";
                ATEndPosY.Content = $"{fixedAirThreatEndPosY:F3}";

                mousePositionTextBox.Visibility = Visibility.Hidden;
            }
            else if (fixAirThreatPox == 6)
            {
                MSLStartPosX.Content = $"{fixedMSLStartPosX:F3}";
                MSLStartPosY.Content = $"{fixedMSLStartPosY:F3}";

                mousePositionTextBox.Visibility = Visibility.Hidden;
            }
        }
        private void AirThreatSpeedTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            // Check if the Enter key is pressed
            if (e.Key == Key.Enter)
            {
                // Get the value from the TextBox
                string inputValue = AirThreatSpeedInput.Text;
                fixedAirThreatSpeed = (Double.Parse(inputValue));
            }
        }

        private void MSLSpeedTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            // Check if the Enter key is pressed
            if (e.Key == Key.Enter)
            {
                // Get the value from the TextBox
                string inputValue = MSLSpeedInput.Text;
                fixedMSLSpeed = (Double.Parse(inputValue));
            }
        }
        //---------------------------------------------------------------

        //---------------------------------------------------------------
        private void MSLStartSetting_Click(object sender, RoutedEventArgs e)
        {
            fixAirThreatPox = 3;
        }
        //---------------------------------------------------------------

        //박우석 수정: 모의시작을 누르면 공중위협 이미지 출력하는 코드. 추후 해당 코드를 fixAirThreatPox == 4 코드와 결합해야함
        private void SimulationStart_Click(object sender, RoutedEventArgs e)
        {
            string projectPath = Environment.CurrentDirectory;
            Console.WriteLine(projectPath);
            // 시작클릭시 공중위협 모의기, 대공유도탄 모의기에 데이터 설정값 Publisher
            //System.Windows.Controls.Image ATimg = new System.Windows.Controls.Image();
            //BitmapImage image = new BitmapImage(new Uri(@"C:\Users\User\Documents\workspace\AAMS\AntiAirMissileSimulationSystem\OperationController\map\plane.jpg"));
            BitmapImage image = new BitmapImage(new Uri(@"..\OperationController\map\plane.jpg", UriKind.Relative));
            System.Windows.Controls.Image imgControl = new System.Windows.Controls.Image();
            imgControl.Source = image;

            Canvas.SetLeft(imgControl, 0);
            Canvas.SetTop(imgControl, 0);

            myCanvas.Children.Add(imgControl);

            /*BitmapImage ATimgtmp = new BitmapImage(new Uri(@"../map/plane.jpg", UriKind.Relative));
            System.Windows.Controls.Image ATimg = new System.Windows.Controls.Image();
            ATimg.Source = ATimgtmp;
            *//*ATimg.Width = 50;
            ATimg.Height = 50;*//*
            int x = 700;
            int y = 500;
            *//*System.Windows.Shapes.Rectangle r = new System.Windows.Shapes.Rectangle();
            r.Width = 100;
            r.Height = 200;
            r.Fill = System.Windows.Media.Brushes.Red;*/
            /*Canvas.SetLeft(ATimg, x - (ATimg.Width / 2.0));
            Canvas.SetBottom(ATimg, y - (ATimg.Height / 2.0));*//*
            Canvas.SetLeft(ATimg, x);
            Canvas.SetBottom(ATimg, y);
            myCanvas.Children.Add(ATimg);*/
        }

        private void SimulationEnd_Click(object sender, RoutedEventArgs e)
        {
            // 모든 시나리오 데이터 설정값 초기화
        }
        //---------------------------------------------------------------
    }
}
