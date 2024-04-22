using OperationController.Data;
using System.Diagnostics;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Input;
using System.Windows.Media.Imaging;
//using static System.Net.Mime.MediaTypeNames;

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

        //---------------------------------------------------------------
        // 공중위협 시작 좌표 출력 및 지도에서 클릭시 고정 출력값 설정
        private short fixAirThreatPox = 0;

        private double fixedAirThreatStartPosX = 0.0;
        private double fixedAirThreatStartPosY = 0.0;

        private double fixedAirThreatEndPosX = 0.0;
        private double fixedAirThreatEndPosY = 0.0;

        private double fixedMSLStartPosX = 0.0;
        private double fixedMSLStartPosY = 0.0;

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
            }
            else if (fixAirThreatPox == 2)
            {
                ATEndPosX.Content = $"{relativeX:F3}";
                ATEndPosY.Content = $"{relativeY:F3}";
            }
            else if (fixAirThreatPox == 3)
            {
                MSLStartPosX.Content = $"{relativeX:F3}";
                MSLStartPosY.Content = $"{relativeY:F3}";
            }
            else if (fixAirThreatPox == 4)
            {
                ATStartPosX.Content = $"{fixedAirThreatStartPosX:F3}";
                ATStartPosY.Content = $"{fixedAirThreatStartPosY:F3}";
            }
            else if (fixAirThreatPox == 5)
            {
                ATEndPosX.Content = $"{fixedAirThreatEndPosX:F3}";
                ATEndPosY.Content = $"{fixedAirThreatEndPosY:F3}";
            }
            else if (fixAirThreatPox == 6)
            {
                MSLStartPosX.Content = $"{fixedMSLStartPosX:F3}";
                MSLStartPosY.Content = $"{fixedMSLStartPosY:F3}";
            }
        }
        private void InputTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            // Check if the Enter key is pressed
            if (e.Key == Key.Enter)
            {
                // Get the value from the TextBox
                string inputValue = AirThreatSpeedInput.Text;

                // Do something with the input value, for example, display it in a MessageBox
                MessageBox.Show($"Input value: {inputValue}");

                // Optionally, you can clear the TextBox after submission
                AirThreatSpeedInput.Clear();
            }
        }
        //---------------------------------------------------------------

        //---------------------------------------------------------------
        private void MSLStartSetting_Click(object sender, RoutedEventArgs e)
        {
            fixAirThreatPox = 3;
        }
        //---------------------------------------------------------------

        //---------------------------------------------------------------
        private void SimulationStart_Click(object sender, RoutedEventArgs e)
        {
            // 시작클릭시 공중위협 모의기, 대공유도탄 모의기에 데이터 설정값 Publisher
            Image ATimg = new Image();
            ATimg.Source = new BitmapImage(new Uri(@"../map/plane.jpg", UriKind.Relative));
            int x = 100;
            int y = 100;
            ATimg.Width = 50;
            ATimg.Height = 50;
            Canvas.SetLeft(ATimg, x - (ATimg.Width / 2.0));
            Canvas.SetBottom(ATimg, y - (ATimg.Height / 2.0));
            myCanvas.Children.Add(ATimg);
        }

        private void SimulationEnd_Click(object sender, RoutedEventArgs e)
        {
            // 모든 시나리오 데이터 설정값 초기화
        }
        //---------------------------------------------------------------
    }
}