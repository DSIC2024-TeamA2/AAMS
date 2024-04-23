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

        /// 변수
        private short setPosMode = 0;  ///< 좌표설정모드(공중위협 시작 좌표, 공중위협 목적 좌표, 대공유도탄 좌표)

        private double fixedAirThreatStartPosX = 0.0; ///< 마우스로 선택한 공중위협 시작 좌표 위도
        private double fixedAirThreatStartPosY = 0.0; ///< 마우스로 선택한 공중위협 시작 좌표 경도

        private double fixedAirThreatEndPosX = 0.0; ///< 마우스로 선택한 공중위협 종료 좌표 위도
        private double fixedAirThreatEndPosY = 0.0; ///< 마우스로 선택한 공중위협 종료 좌표 경도

        private double fixedMSLStartPosX = 0.0; ///< 마우스로 선택한 대공유도탄 좌표 위도
        private double fixedMSLStartPosY = 0.0; ///< 마우스로 선택한 대공유도탄 좌표 경도

        // 아직 미사용 //
        private double fixedAirThreatSpeed = 0.0; ///< 입력한 공중위협 속도
        private double fixedMSLSpeed = 0.0; ///< 입력한 대공유도탄 속도
        ///

        /// 함수
        // 공중위협 시작 좌표, 공중위협 시작 좌표, 대공유도탄 좌표 설정 버튼 3개를 각각 클릭시 현재 선택한 좌표 설정 모드 변경하는 함수
        private void SetPosModeClick(object sender, RoutedEventArgs e)
        {
            if(sender == ATStartPosSetBTN) // 클릭한 버튼이 공중위협 시작 좌표 설정 버튼인 경우
            {
                setPosMode = 1; // 좌표 설정 모드를 공중위협 시작 좌표 설정 모드로 변경
            }
            else if(sender == ATEndPosSetBTN) // 클릭한 버튼이 공중위협 목적 좌표 설정 버튼인 경우
            {
                setPosMode = 2; // 좌표 설정 모드를 공중위협 목적 좌표 설정 모드로 변경
            }
            else if (sender == MSLPosSetBTN) // 클릭한 버튼이 대공유도탄 좌표 설정 버튼인 경우
            {
                setPosMode = 3; // 좌표 설정 모드를 대공유도탄 좌표 설정 모드로 변경
            }
        }
        
        // 지도 위에서 마우스로 좌표를 선택해 클릭했을 때, 해당 좌표값 저장하고 설정 출력창에 출력하는 함수
        private void ClickPosOnMap(object sender, MouseButtonEventArgs e)
        {
            Point currentMousePosXY = e.GetPosition(myCanvas);  ///< 지도(myCanvas) 좌표계에서 마우스의 X, Y 좌표값
            double relativeX = currentMousePosXY.X; ///< 마우스 X 좌표값
            double relativeY = currentMousePosXY.Y; ///< 마우스 Y 좌표값

            // 클릭한 좌표값을 설정 출력창에 출력하는 함수 호출
            PrintSetPos(relativeX, relativeY);
        }

        // 클릭한 좌표값을 설정 출력창에 출력하는 함수
        private void PrintSetPos(double x, double y)
        {
            if (setPosMode == 1) // 공중위협 시작 좌표 설정 버튼을 클릭한 상태
            {
                fixedAirThreatStartPosX = x; ///< 클릭한 좌표의 X를 공중위협 시작 위도로 저장
                fixedAirThreatStartPosY = y; ///< 클릭한 좌표의 Y를 공중위협 시작 경도로 저장
            }
            else if (setPosMode == 2) // 공중위협 목적 좌표 설정 버튼을 클릭한 상태
            {
                fixedAirThreatEndPosX = x; ///< 클릭한 좌표의 X를 공중위협 목적 위도로 저장
                fixedAirThreatEndPosY = y; ///< 클릭한 좌표의 Y를 공중위협 목적 경도로 저장
            }
            else if (setPosMode == 3) // 대공유도탄 좌표 설정 버튼을 클릭한 상태
            {
                fixedMSLStartPosX = x; ///< 클릭한 좌표의 X를 대공유도탄 위도로 저장
                fixedMSLStartPosY = y; ///< 클릭한 좌표의 Y를 대공유도탄 경도로 저장
            }

            setPosMode += 3; ///< 좌표 설정 모드 상태 변경
        }

        // 지도 위에 이동중인 현재 마우스 좌표값을 출력하고 클릭시 설정한 좌표값을 출력하는 함수
        private void CurrentMousePosOnMap(object sender, System.Windows.Input.MouseEventArgs e)
        {
            Point currentMousePosXY = e.GetPosition(myCanvas);  ///< 지도(myCanvas) 좌표계에서 마우스의 X, Y 좌표값
            double relativeX = currentMousePosXY.X; ///< 마우스 X 좌표값
            double relativeY = currentMousePosXY.Y; ///< 마우스 Y 좌표값

            if (setPosMode == 1) // 공중위협 시작 좌표 설정 모드인 경우
            {
                ATStartPosX.Content = $"{relativeX:F3}"; ///< 현재 이동중인 마우스 좌표 X
                ATStartPosY.Content = $"{relativeY:F3}"; ///< 현재 이동중인 마우스 좌표 Y
                // 이동 중인 마우스 옆에 현재 좌표값 출력창 생성하는 함수 호출
                PrintPosXY(relativeX, relativeY);

            }
            else if (setPosMode == 2) // 공중위협 목적 좌표 설정 모드인 경우
            {
                ATEndPosX.Content = $"{relativeX:F3}"; ///< 현재 이동중인 마우스 좌표 X
                ATEndPosY.Content = $"{relativeY:F3}"; ///< 현재 이동중인 마우스 좌표 Y
                // 이동 중인 마우스 옆에 현재 좌표값 출력창 생성하는 함수 호출
                PrintPosXY(relativeX, relativeY);

            }
            else if (setPosMode == 3) // 대공유도탄 좌표 설정 모드인 경우
            {
                MSLStartPosX.Content = $"{relativeX:F3}"; ///< 현재 이동중인 마우스 좌표 X
                MSLStartPosY.Content = $"{relativeY:F3}"; ///< 현재 이동중인 마우스 좌표 Y
                // 이동 중인 마우스 옆에 현재 좌표값 출력창 생성하는 함수 호출
                PrintPosXY(relativeX, relativeY);

            }
            else if (setPosMode == 4) // 설정할 공중위협 시작 좌표를 클릭한 경우
            {
                ATStartPosX.Content = $"{fixedAirThreatStartPosX:F3}"; ///< 클릭한 마우스 좌표 X
                ATStartPosY.Content = $"{fixedAirThreatStartPosY:F3}"; ///< 클릭한 마우스 좌표 Y
                mousePositionTextBox.Visibility = Visibility.Hidden; ///< 좌표를 선택해 클릭하였으니 마우스 옆 출력창을 안 보이도록 설정
            }
            else if (setPosMode == 5) // 설정할 공중위협 목적 좌표를 클릭한 경우
            {
                ATEndPosX.Content = $"{fixedAirThreatEndPosX:F3}"; ///< 클릭한 마우스 좌표 X
                ATEndPosY.Content = $"{fixedAirThreatEndPosY:F3}"; ///< 클릭한 마우스 좌표 Y
                mousePositionTextBox.Visibility = Visibility.Hidden; ///< 좌표를 선택해 클릭하였으니 마우스 옆 출력창을 안 보이도록 설정
            }
            else if (setPosMode == 6) // 설정할 대공유도탄 좌표를 클릭한 경우
            {
                MSLStartPosX.Content = $"{fixedMSLStartPosX:F3}"; ///< 클릭한 마우스 좌표 X
                MSLStartPosY.Content = $"{fixedMSLStartPosY:F3}"; ///< 클릭한 마우스 좌표 Y
                mousePositionTextBox.Visibility = Visibility.Hidden; ///< 좌표를 선택해 클릭하였으니 마우스 옆 출력창을 안 보이도록 설정
            }
        }

        // 움직이는 마우스 바로 옆에 현재 마우스의 좌표 출력하는 생성하는 함수
        private void PrintPosXY(double x, double y)
        {
            // 좌표 선택을 위해 지도 위에 마우스 커서를 올리면 커서 바로 옆에 현재 가리키는 위도, 경도를 출력하는 기능
            // 현재 마우스 커서가 가리키는 좌표를 출력하는 사각형 형태의 출력창을 마우스 커서를 기준으로 얼만큼 떨어진 곳에 출력할지 설정
            mousePositionTextBox.Margin = new Thickness(x + 16, y + 16, 0, 0);
            // 현재 마우스 커서가 가리키는 좌표를 출력창에 출력하는 기능
            mousePositionTextBox.Text = $"위도 {x:F3}\n경도 {y:F3}";
            // 좌표 선택하는 중에만 지도 위에 현재 마우스 커서가 가리키는 좌표를 출력하는 출력창을 보이게 하는 기능
            mousePositionTextBox.Visibility = Visibility.Visible;
        }

        // 속도 입력 창에서 값을 입력한 후 엔터 입력으로 저장하는 함수
        private void SetSpeedEnterKeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter) // 엔터를 입력한 경우
            {
                if(sender == AirThreatSpeedInput)
                {
                    string inputValue = AirThreatSpeedInput.Text;
                    if (IsNumeric(inputValue))
                        fixedAirThreatSpeed = (Double.Parse(inputValue));
                    else;
                }
                else if(sender == MSLSpeedInput)
                {
                    string inputValue = MSLSpeedInput.Text;
                    if (IsNumeric(inputValue))
                        fixedMSLSpeed = (Double.Parse(inputValue));
                    else;
                }
            }
        }
        private bool IsNumeric(string value)
        {
            return double.TryParse(value, out _);
        }
        //---------------------------------------------------------------

        //박우석 수정: 모의시작을 누르면 공중위협 이미지 출력하는 코드. 추후 해당 코드를 setPosMode == 4 코드와 결합해야함
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
