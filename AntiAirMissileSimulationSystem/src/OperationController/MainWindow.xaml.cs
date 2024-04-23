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
using static System.Net.Mime.MediaTypeNames;

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
            nf.SendSimulationStatusInfoMsg();
            nf.SendScenarioInfoMsg(10, 11, 12, 101, 102, 5, 51, 52, 15);
        }

        internal void UpdateAirThreatInfo(AirThreatInfo info)
        {
            Console.WriteLine(info.ToString());
        }

        internal void UpdateAntiAirMissileInfo(AntiAirMissileInfo info)
        {
            Console.WriteLine(info.ToString());
        }

        internal void UpdateSimulationStatusInfo(SimulationStatusInfo info)
        {
            Console.WriteLine(info.ToString());
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

        private double currentAirThreatPosX = 0.0;
        private double currentAirThreatPosY = 0.0;

        private double currentMSLPosX = 0.0;
        private double currentMSLPosY = 0.0;

        //공중위협, 목적지 연결하는 선 확인 flg
        private int airThreatStartflg = 0;
        private int airThreatEndflg = 0;
        private double ATangle; //공중위협 방향
        Line line = new Line();

        //공중위협, 미사일, 목적지, 반경 최초 선언
        System.Windows.Controls.Image imgControl4 = null;
        System.Windows.Controls.Image imgControl5 = null;
        System.Windows.Controls.Image imgControl6 = null;
        Ellipse ellipse = new Ellipse();

        /// 함수
        // 공중위협 시작 좌표, 공중위협 시작 좌표, 대공유도탄 좌표 설정 버튼 3개를 각각 클릭시 현재 선택한 좌표 설정 모드 변경하는 함수
        private void SetPosModeClick(object sender, RoutedEventArgs e)
        {
            if (sender == ATStartPosSetBTN) // 클릭한 버튼이 공중위협 시작 좌표 설정 버튼인 경우
            {
                setPosMode = 1; // 좌표 설정 모드를 공중위협 시작 좌표 설정 모드로 변경
            }
            else if (sender == ATEndPosSetBTN) // 클릭한 버튼이 공중위협 목적 좌표 설정 버튼인 경우
            {
                setPosMode = 2; // 좌표 설정 모드를 공중위협 목적 좌표 설정 모드로 변경
            }
            else if (sender == MSLPosSetBTN) // 클릭한 버튼이 대공유도탄 좌표 설정 버튼인 경우
            {
                setPosMode = 3; // 좌표 설정 모드를 대공유도탄 좌표 설정 모드로 변경
            }
            else if (sender == Start) // 클릭한 버튼이 모의시작 버튼인 경우
            {
                // 미사일 반경과 공중위협 경로가 겹치는지 판단
                if (!Intersect(ellipse, line))
                {
                    EventLog.Text += "경고: 공중위협 경로와 미사일 반경이 겹치지 않습니다.\n";
                }
                SimulationStart_Click(sender, e);
            }
            else if (sender == Stop)
            {
                SimulationEnd_Click(sender, e);
            }
        }

        //미사일 반경과 공중위협 경로가 겹치는지 판단
        private bool Intersect(Ellipse circle, Line line)
        {
            // 원의 중심 좌표
            double circleCenterX = Canvas.GetLeft(circle) + circle.Width / 2;
            double circleCenterY = Canvas.GetTop(circle) + circle.Height / 2;

            // 직선의 시작점과 끝점 좌표
            double lineX1 = line.X1;
            double lineY1 = line.Y1;
            double lineX2 = line.X2;
            double lineY2 = line.Y2;

            // 직선의 기울기
            double lineSlope = (lineY2 - lineY1) / (lineX2 - lineX1);

            // 공중위협 각도 ATangle 구하기
            double radians = Math.Atan(-lineSlope);
            double degrees = 90 - radians * (180 / Math.PI);
            if(fixedAirThreatStartPosX < fixedAirThreatEndPosX)
                ATangle = degrees;
            else
                ATangle = 180 + degrees;

            // 직선의 y절편
            double lineIntercept = lineY1 - lineSlope * lineX1;

            // 원의 중심에서 직선까지의 거리
            double distance = Math.Abs((lineSlope * circleCenterX - circleCenterY + lineIntercept) / Math.Sqrt(lineSlope * lineSlope + 1));

            // 거리가 반지름보다 작거나 같으면 겹침
            return distance <= circle.Width / 2;
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
                setPosMode += 3; ///< 좌표 설정 모드 상태 변경
            }
            else if (setPosMode == 2) // 공중위협 목적 좌표 설정 버튼을 클릭한 상태
            {
                fixedAirThreatEndPosX = x; ///< 클릭한 좌표의 X를 공중위협 목적 위도로 저장
                fixedAirThreatEndPosY = y; ///< 클릭한 좌표의 Y를 공중위협 목적 경도로 저장
                setPosMode += 3; ///< 좌표 설정 모드 상태 변경
            }
            else if (setPosMode == 3) // 대공유도탄 좌표 설정 버튼을 클릭한 상태
            {
                fixedMSLStartPosX = x; ///< 클릭한 좌표의 X를 대공유도탄 위도로 저장
                fixedMSLStartPosY = y; ///< 클릭한 좌표의 Y를 대공유도탄 경도로 저장
                setPosMode += 3; ///< 좌표 설정 모드 상태 변경
            }
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
                ATStartPosX.Content = $"{fixedAirThreatStartPosX:F3}";
                ATStartPosY.Content = $"{fixedAirThreatStartPosY:F3}";
                mousePositionTextBox.Visibility = Visibility.Hidden;

                airThreatStartflg = 1;
                if (airThreatEndflg == 1)
                    ConnectLine();

                if (imgControl4 != null)
                    myCanvas.Children.Remove(imgControl4);

                //대공위협 이미지 클릭 위치에 추가하기
                BitmapImage image4 = new BitmapImage(new Uri(@"./map/plane.jpg", UriKind.Relative));
                imgControl4 = new System.Windows.Controls.Image();
                imgControl4.Source = image4;
                imgControl4.Width = 50;
                imgControl4.Height = 50;
                Canvas.SetLeft(imgControl4, fixedAirThreatStartPosX - (imgControl4.Width / 2.0));
                Canvas.SetTop(imgControl4, fixedAirThreatStartPosY - (imgControl4.Height / 2.0));
                myCanvas.Children.Add(imgControl4);
            }
            else if (setPosMode == 5) // 설정할 공중위협 목적 좌표를 클릭한 경우
            {
                ATEndPosX.Content = $"{fixedAirThreatEndPosX:F3}";
                ATEndPosY.Content = $"{fixedAirThreatEndPosY:F3}";
                mousePositionTextBox.Visibility = Visibility.Hidden;

                airThreatEndflg = 1;
                if (airThreatStartflg == 1)
                    ConnectLine();

                if (imgControl5 != null)
                    myCanvas.Children.Remove(imgControl5);

                //대공위협 목적지 이미지 클릭 위치에 추가하기
                BitmapImage image5 = new BitmapImage(new Uri(@"./map/ATend.jpg", UriKind.Relative));
                imgControl5 = new System.Windows.Controls.Image();
                imgControl5.Source = image5;
                imgControl5.Width = 20;
                imgControl5.Height = 20;
                Canvas.SetLeft(imgControl5, fixedAirThreatEndPosX - (imgControl5.Width / 2.0));
                Canvas.SetTop(imgControl5, fixedAirThreatEndPosY - (imgControl5.Height / 2.0));
                myCanvas.Children.Add(imgControl5);
            }
            else if (setPosMode == 6) // 설정할 대공유도탄 좌표를 클릭한 경우
            {
                MSLStartPosX.Content = $"{fixedMSLStartPosX:F3}";
                MSLStartPosY.Content = $"{fixedMSLStartPosY:F3}";
                mousePositionTextBox.Visibility = Visibility.Hidden;

                if (imgControl6 != null)
                    myCanvas.Children.Remove(imgControl6);

                //미사일 이미지 클릭 위치에 추가하기
                BitmapImage image6 = new BitmapImage(new Uri(@"./map/missile.jpg", UriKind.Relative));
                imgControl6 = new System.Windows.Controls.Image();
                imgControl6.Source = image6;
                imgControl6.Width = 100;
                imgControl6.Height = 100;
                Canvas.SetLeft(imgControl6, fixedMSLStartPosX - (imgControl6.Width / 2.0));
                Canvas.SetTop(imgControl6, fixedMSLStartPosY - (imgControl6.Height / 2.0));
                myCanvas.Children.Add(imgControl6);

                if (ellipse != null)
                    myCanvas.Children.Remove(ellipse);

                //미사일 반경 이미지 클릭 위치에 추가하기
                ellipse.Stroke = Brushes.Red;
                ellipse.StrokeThickness = 2;
                ellipse.Fill = new SolidColorBrush(Color.FromArgb(50, 255, 0, 0)); ;
                ellipse.Width = 600;
                ellipse.Height = 600;
                Canvas.SetLeft(ellipse, fixedMSLStartPosX - (ellipse.Width / 2.0));
                Canvas.SetTop(ellipse, fixedMSLStartPosY - (ellipse.Height / 2.0));
                myCanvas.ClipToBounds = true;
                myCanvas.Children.Add(ellipse);
            }
        }

        private void ConnectLine()
        {
            // 이전에 그려진 선이 있으면 제거
            if (line != null)
                myCanvas.Children.Remove(line);

            // 새로운 선 그리기
            line.Stroke = Brushes.Red; // 선의 색상
            line.StrokeThickness = 2; // 선의 두께
            line.X1 = fixedAirThreatStartPosX; // 시작점 x 좌표
            line.Y1 = fixedAirThreatStartPosY; // 시작점 y 좌표
            line.X2 = fixedAirThreatEndPosX; // 시작점 x 좌표
            line.Y2 = fixedAirThreatEndPosY; // 시작점 y 좌표
            myCanvas.Children.Add(line);
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
                if (sender == AirThreatSpeedInput)
                {
                    string inputValue = AirThreatSpeedInput.Text;
                    if (IsNumeric(inputValue))
                        fixedAirThreatSpeed = (Double.Parse(inputValue));
                    else;
                }
                else if (sender == MSLSpeedInput)
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

        private void SimulationStart_Click(object sender, RoutedEventArgs e)
        {
            // 시작클릭시 공중위협 모의기, 대공유도탄 모의기에 데이터 설정값 Publisher
            EventLog.Text += "시나리오 시작.\n";
            // 회전 변환을 생성합니다.
            RotateTransform rotateTransform = new RotateTransform();
            rotateTransform.Angle = ATangle; // 회전 각도 설정
            // 이미지의 중심을 회전 중심으로 지정합니다.
            imgControl4.RenderTransformOrigin = new Point(0.5, 0.5);
            imgControl4.RenderTransform = rotateTransform;
            ATCurrentPosX.Content = $"{fixedAirThreatStartPosX:F3}";
            ATCurrentPosY.Content = $"{fixedAirThreatStartPosY:F3}";
            if (0 <= ATangle && ATangle <= 90)
                ATCurrentDIR.Content = $"{90 - ATangle:F0}" + "°";
            else
                ATCurrentDIR.Content = $"{450 - ATangle:F0}" + "°";
            Canvas.SetLeft


            MSLCurrentPosX.Content = $"{fixedMSLStartPosX:F3}";
            MSLCurrentPosY.Content = $"{fixedMSLStartPosY:F3}";
        }

        private void SimulationEnd_Click(object sender, RoutedEventArgs e)
        {
            // 모든 시나리오 데이터 설정값 초기화
            EventLog.Text += "시나리오 종료.\n";
            airThreatStartflg = 0;
            airThreatEndflg = 0;
            setPosMode = 0;
            ATangle = 0;
            ATStartPosX.Content = "NO DATA";
            ATStartPosY.Content = "NO DATA";
            ATEndPosX.Content = "NO DATA";
            ATEndPosY.Content = "NO DATA";
            AirThreatSpeedInput.Text = "0";
            MSLStartPosX.Content = "NO DATA";
            MSLStartPosY.Content = "NO DATA";
            MSLSpeedInput.Text = "0";
            ATCurrentPosX.Content = "NO DATA";
            ATCurrentPosY.Content = "NO DATA";
            ATCurrentSpeed.Content = "NO DATA";
            ATCurrentDIR.Content = "NO DATA";
            MSLCurrentPosX.Content = "NO DATA";
            MSLCurrentPosY.Content = "NO DATA";
            MLSCurrentSpeed.Content = "NO DATA";
            MLSCurrentDIR.Content = "NO DATA";
            myCanvas.Children.Clear();
        }
        //---------------------------------------------------------------
    }
}