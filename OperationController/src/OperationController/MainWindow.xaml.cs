﻿using System.Diagnostics;
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
using System.Windows.Threading;
using System.Threading;
using OperationController.Data;

namespace OperationController.DisplayManage
{
    public partial class MainWindow : Window
    {
        //---------------------------------------------------------------------------------------
        private nFrameworkConnector nf = null; // 통신 nFrameWork 생성

        private short setPosMode = 0;  // 클릭한 버튼에 따른 플래그 (공중위협 시작 좌표, 공중위협 목적 좌표, 대공유도탄 좌표, 모의 시작, 모의 종료)

        // 입력한 좌표 저장 변수 (공중위협, 대공유도탄)
        private double fixedAirThreatStartPosX = 0.0; // 마우스로 선택한 공중위협 시작 좌표 위도
        private double fixedAirThreatStartPosY = 0.0; // 마우스로 선택한 공중위협 시작 좌표 경도
        private double fixedAirThreatEndPosX = 0.0; // 마우스로 선택한 공중위협 종료 좌표 위도
        private double fixedAirThreatEndPosY = 0.0; // 마우스로 선택한 공중위협 종료 좌표 경도
        private double fixedMSLStartPosX = 0.0; // 마우스로 선택한 대공유도탄 좌표 위도
        private double fixedMSLStartPosY = 0.0; // 마우스로 선택한 대공유도탄 좌표 경도

        // 고정된 입력값 변수 (속도, 반경)
        private int MSLRadius = 600; // 대공유도탄 반경
        private float fixedAirThreatSpeed = 0.0f; // 입력한 공중위협 속도
        private float fixedMSLSpeed = 0.0f; // 입력한 대공유도탄 속도

        private int MSLLaunch = 0;

        // 현재 좌표 저장 변수 (공중위협, 대공유도탄)
        private double currentAirThreatPosX = 0.0; // 공중위협 모의기에서 수신한 공중위협 현재 좌표 위도
        private double currentAirThreatPosY = 0.0; // 공중위협 모의기에서 수신한 공중위협 현재 좌표 경도
        private double currentMSLPosX = 0.0; // 대공유도탄 모의기에서 수신한 대공유도탄 현재 좌표 위도
        private double currentMSLPosY = 0.0; // 대공유도탄 모의기에서 수신한 대공유도탄 현재 좌표 경도

        // 공중위협, 목적지 연결하는 선 확인 플래그
        private int airThreatStartflg = 0; // 공중위협 시작 좌표 입력되었는지 확인 플래그
        private int airThreatEndflg = 0; // 공중위협 목적 좌표 입력되었는지 확인 플래그
        private int MSLStartflg = 0; // 대공유도탄 좌표 입력되었는지 확인 플래그
     //   private double ATangle = 0.0; // 공중위협 방향
     //   private double MSLangle = 0.0; // 대공유도탄 방향
        Line line = new Line(); // 공중위협 시작에서 목적까지 경로를 그리는 선

        //공중위협, 미사일, 목적지, 반경 최초 선언
        Ellipse ellipse = new Ellipse();
        System.Windows.Controls.Image imgControl4 = null; // 공중위협 시작 이미지
        System.Windows.Controls.Image imgControl5 = null; // 공중위협 목적 이미지
        System.Windows.Controls.Image imgControl6 = null; // 대공유도탄 이미지
        RotateTransform ATrotateTransform = new RotateTransform(); // 공중위협 이미지 회전 담당
        RotateTransform MSLrotateTransform = new RotateTransform(); // 대공유도탄 이미지 회전 담당

        private SimulationStatusInfo statusInfo;

        private int crash = 0; // 공중위협 요격 확인 플래그
        //---------------------------------------------------------------------------------------


        //---------------------------------------------------------------------------------------
        // 실행시 가장 첫번째로 프로그램 메인창을 UI 요소를 초기화하는 함수
        public MainWindow()
        {
            // UI 요소들을 초기화
            InitializeComponent();

            // 이벤트 출력창 제목 아래에 이벤트 메시지 출력하기 위한 줄바꿈
            EventLog.AppendText("\n");
            EventLog.ScrollToEnd();

            // 프로그램 메인창을 컴퓨터 화면의 상단 중앙 위치에 고정
            double screenWidth = System.Windows.SystemParameters.PrimaryScreenWidth;
            double screenHeight = System.Windows.SystemParameters.PrimaryScreenHeight;
            double windowWidth = this.Width;
            double windowHeight = this.Height;
            this.Left = (screenWidth / 2) - (windowWidth / 2);
            this.Top = 0;
            MSLRadius = AMSConfiguration.GetInstance().DetectionRadius * 2;
            this.Closed += OnWindowClosed;
        }
        private void OnWindowClosed(object sender, EventArgs e)
        {
            GetNFrameworkConnector().Close();
            Thread.Sleep(1000);
            Console.WriteLine("창이 닫혔습니다.");
        }

        // 통신 nFrameWork 연결하는 함수
        private nFrameworkConnector GetNFrameworkConnector()
        {
            if (nf == null)
            {
                nf = new nFrameworkConnector(this);
            }
            return nf;
        }


        internal void UpdateAirThreatInfo(AirThreatInfo info)
        {
            currentAirThreatPosX = info.CurrentPosition.Latitude;
            currentAirThreatPosY = info.CurrentPosition.Longitude;

            Canvas.SetLeft(imgControl4, currentAirThreatPosX - (imgControl4.Width / 2.0));
            Canvas.SetTop(imgControl4, currentAirThreatPosY - (imgControl4.Height / 2.0));
            ATCurrentPosX.Content = $"{currentAirThreatPosX:F1}";
            ATCurrentPosY.Content = $"{currentAirThreatPosY:F1}";

            if (statusInfo != SimulationStatusInfo.SUCCESS && statusInfo != SimulationStatusInfo.FAIL)
            {
                EventLog.AppendText(info.ToString() + "\n");
                EventLog.ScrollToEnd();
            }

            // 공중위협을 목적지 방향으로 회전
            ATrotateTransform.Angle = 90 + info.CurrentAngle; // 회전 각도 설정
            // 공중위협 이미지의 중심을 회전 중심으로 지정
            imgControl4.RenderTransformOrigin = new Point(0.5, 0.5);
            imgControl4.RenderTransform = ATrotateTransform;
            // 공중위협 방향 현재좌표로 입력
            ATCurrentDIR.Content = $"{info.CurrentAngle:F1}" + "°";
        }
        
        internal void UpdateAntiAirMissileInfo(AntiAirMissileInfo info)
        {
            currentMSLPosX = info.CurrentPosition.Latitude;
            currentMSLPosY = info.CurrentPosition.Longitude;

            Canvas.SetLeft(imgControl6, currentMSLPosX - (imgControl6.Width / 2.0));
            Canvas.SetTop(imgControl6, currentMSLPosY - (imgControl6.Height / 2.0));
            MSLCurrentPosX.Content = $"{currentMSLPosX:F1}";
            MSLCurrentPosY.Content = $"{currentMSLPosY:F1}";
            if (AT2MSLDistance() <= AMSConfiguration.GetInstance().DetectionRadius)
                MSLLaunch = 1;
            //대공유도탄 이미지 회전
            if (MSLLaunch == 1)
            {
                //   MSLangleCalc();
                MSLrotateTransform.Angle = 90 + info.CurrentAngle;
                // 대공유도탄 이미지의 중심을 회전 중심으로 지정
                imgControl6.RenderTransformOrigin = new Point(0.5, 0.5);
                imgControl6.RenderTransform = MSLrotateTransform;
                // 대공유도탄 각도 출력
                MSLCurrentDIR.Content = $"{info.CurrentAngle:F1}" + "°";
            }
                EventLog.AppendText(info.ToString() + "\n");
                EventLog.ScrollToEnd();
        }

        internal void UpdateSimulationStatusInfo(SimulationStatusInfo info)
        {
            statusInfo = info;
            EventLog.AppendText(info.ToString() + "\n");
            EventLog.ScrollToEnd();

            if (info == SimulationStatusInfo.SUCCESS)
            {
                DialogWindow.GetSuccessDialog().Show();
            }
            else if (info == SimulationStatusInfo.FAIL)
            {
                DialogWindow.GetFailDialog().Show();
            }
        }

        /// 함수
        // 공중위협 시작 좌표, 공중위협 시작 좌표, 대공유도탄 좌표 설정 버튼 3개를 각각 클릭시 현재 선택한 좌표 설정 모드 변경하는 함수
        private void SetPosModeClick(object sender, RoutedEventArgs e)
        {
            if (sender == ATStartPosSetBTN) // 클릭한 버튼이 공중위협 시작 좌표 설정 버튼인 경우
            {
                setPosMode = 1; // 좌표 설정 모드를 공중위협 시작 좌표 설정 모드로 변경
                verticalLine.Visibility = Visibility.Visible;
                horizontalLine.Visibility = Visibility.Visible;
            }
            else if (sender == ATEndPosSetBTN) // 클릭한 버튼이 공중위협 목적 좌표 설정 버튼인 경우
            {
                setPosMode = 2; // 좌표 설정 모드를 공중위협 목적 좌표 설정 모드로 변경
                verticalLine.Visibility = Visibility.Visible;
                horizontalLine.Visibility = Visibility.Visible;
            }
            else if (sender == MSLPosSetBTN) // 클릭한 버튼이 대공유도탄 좌표 설정 버튼인 경우
            {
                setPosMode = 3; // 좌표 설정 모드를 대공유도탄 좌표 설정 모드로 변경
                verticalLine.Visibility = Visibility.Visible;
                horizontalLine.Visibility = Visibility.Visible;
            }
            else if (sender == Start) // 클릭한 버튼이 모의시작 버튼인 경우
            {
                if (airThreatStartflg == 0 || airThreatEndflg == 0 || MSLStartflg == 0)
                {
                    EventLog.AppendText("위험: 공중위협 시작/목표, 대공유도탄의 위치를 확인해주세요.\n");
                    EventLog.ScrollToEnd();
                    return;
                }
                // 미사일 반경과 공중위협 경로가 겹치는지 판단
                if (!Intersect(ellipse, line))
                {
                    EventLog.AppendText("경고: 공중위협 경로와 대공유도탄 반경이 겹치지 않습니다.\n");
                    EventLog.ScrollToEnd();
                }

                //공중위협 속도 입력
                string inputValue = AirThreatSpeedInput.Text;
                if (IsNumeric(inputValue))
                {
                    fixedAirThreatSpeed = (float.Parse(inputValue));
                    if (fixedAirThreatSpeed <= 0)
                    {
                        EventLog.AppendText("입력한 공중위협 속도가 0입니다." + "\n");
                        EventLog.ScrollToEnd();
                        return;
                    }
                }
                else
                {
                    EventLog.AppendText("입력한 공중위협 속도 \"" + inputValue + "\"는 숫자가 아닙니다." + "\n");
                    EventLog.ScrollToEnd();
                    return;
                }

                //대공유도탄 속도 입력
                inputValue = MSLSpeedInput.Text;
                if (IsNumeric(inputValue))
                {
                    fixedMSLSpeed = (float.Parse(inputValue));
                    if (fixedMSLSpeed <= 0)
                    {
                        EventLog.AppendText("입력한 대공유도탄 속도가 0입니다." + "\n");
                        EventLog.ScrollToEnd();
                        return;
                    }
                    if (fixedMSLSpeed <= fixedAirThreatSpeed)
                    {
                        EventLog.AppendText("입력한 대공유도탄 속도가 공중위협 속도과 같거나 느립니다.\n대공 유도탄 속도를 더 높게 입력해주세요\n");
                        EventLog.ScrollToEnd();
                        return;
                    }
                }
                else
                {
                    EventLog.AppendText("입력한 대공유도탄 속도 \"" + inputValue + "\"는 숫자가 아닙니다." + "\n");
                    EventLog.ScrollToEnd();
                    return;
                }

                SimulationStart_Click(sender, e);
                GetNFrameworkConnector().SendScenarioInfoMsg(0, fixedAirThreatStartPosX, fixedAirThreatStartPosY, fixedAirThreatEndPosX, fixedAirThreatEndPosY,
                    fixedAirThreatSpeed, fixedMSLStartPosX, fixedMSLStartPosY, fixedMSLSpeed);
                statusInfo = SimulationStatusInfo.DETECTEING;
                GetNFrameworkConnector().SendSimulationStatusInfoMsg(SimulationStatusInfo.DETECTEING);

                //stopwatch.Start();
                //timer.Start();
            }
            else if (sender == Stop)
            {
                SimulationEnd_Click(sender, e);
                statusInfo = SimulationStatusInfo.IDLE;
                GetNFrameworkConnector().SendSimulationStatusInfoMsg(SimulationStatusInfo.IDLE);
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

            // 직선의 y절편
            double lineIntercept = lineY1 - lineSlope * lineX1;

            // 원의 중심에서 직선까지의 거리
            double distance = Math.Abs((lineSlope * circleCenterX - circleCenterY + lineIntercept) / Math.Sqrt(lineSlope * lineSlope + 1));

            // 거리가 반지름보다 작거나 같으면 겹침
            return distance <= circle.Width / 2;
        }

        private void CurrentMousePosDotLinePrint(Point mousePos)
        {
            verticalLine.X1 = mousePos.X;
            verticalLine.X2 = mousePos.X;

            horizontalLine.Y1 = mousePos.Y;
            horizontalLine.Y2 = mousePos.Y;
        }

        private void HideMouseInfo()
        {
            mousePositionTextBox.Visibility = Visibility.Hidden;

            verticalLine.X1 = 0;
            verticalLine.X2 = 0;

            horizontalLine.Y1 = 0;
            horizontalLine.Y2 = 0;

            verticalLine.Visibility = Visibility.Hidden;
            horizontalLine.Visibility = Visibility.Hidden;
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
                setPosMode = 4; ///< 좌표 설정 모드 상태 변경
            }
            else if (setPosMode == 2) // 공중위협 목적 좌표 설정 버튼을 클릭한 상태
            {
                fixedAirThreatEndPosX = x; ///< 클릭한 좌표의 X를 공중위협 목적 위도로 저장
                fixedAirThreatEndPosY = y; ///< 클릭한 좌표의 Y를 공중위협 목적 경도로 저장
                setPosMode = 5; ///< 좌표 설정 모드 상태 변경
            }
            else if (setPosMode == 3) // 대공유도탄 좌표 설정 버튼을 클릭한 상태
            {
                fixedMSLStartPosX = x; ///< 클릭한 좌표의 X를 대공유도탄 위도로 저장
                fixedMSLStartPosY = y; ///< 클릭한 좌표의 Y를 대공유도탄 경도로 저장
                setPosMode = 6; ///< 좌표 설정 모드 상태 변경
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
                ATStartPosX.Content = $"{relativeX:F1}"; ///< 현재 이동중인 마우스 좌표 X
                ATStartPosY.Content = $"{relativeY:F1}"; ///< 현재 이동중인 마우스 좌표 Y
                // 이동 중인 마우스 옆에 현재 좌표값 출력창 생성하는 함수 호출
                PrintPosXY(relativeX, relativeY);

                verticalLine.Stroke = Brushes.Red;
                horizontalLine.Stroke = Brushes.Red;
                CurrentMousePosDotLinePrint(currentMousePosXY);

            }
            else if (setPosMode == 2) // 공중위협 목적 좌표 설정 모드인 경우
            {
                ATEndPosX.Content = $"{relativeX:F1}"; ///< 현재 이동중인 마우스 좌표 X
                ATEndPosY.Content = $"{relativeY:F1}"; ///< 현재 이동중인 마우스 좌표 Y
                // 이동 중인 마우스 옆에 현재 좌표값 출력창 생성하는 함수 호출
                PrintPosXY(relativeX, relativeY);

                verticalLine.Stroke = Brushes.Red;
                horizontalLine.Stroke = Brushes.Red;
                CurrentMousePosDotLinePrint(currentMousePosXY);

            }
            else if (setPosMode == 3) // 대공유도탄 좌표 설정 모드인 경우
            {
                MSLStartPosX.Content = $"{relativeX:F1}"; ///< 현재 이동중인 마우스 좌표 X
                MSLStartPosY.Content = $"{relativeY:F1}"; ///< 현재 이동중인 마우스 좌표 Y
                // 이동 중인 마우스 옆에 현재 좌표값 출력창 생성하는 함수 호출
                PrintPosXY(relativeX, relativeY);

                verticalLine.Stroke = Brushes.Green;
                horizontalLine.Stroke = Brushes.Green;
                CurrentMousePosDotLinePrint(currentMousePosXY);

            }
            else if (setPosMode == 4) // 설정할 공중위협 시작 좌표를 클릭한 경우
            {
                ATStartPosX.Content = $"{fixedAirThreatStartPosX:F1}";
                ATStartPosY.Content = $"{fixedAirThreatStartPosY:F1}";
                HideMouseInfo();

                airThreatStartflg = 1;
                if (airThreatEndflg == 1)
                    ConnectLine();

                if (imgControl4 != null)
                    myCanvas.Children.Remove(imgControl4);

                //공중위협 이미지 클릭 위치에 추가하기
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
                ATEndPosX.Content = $"{fixedAirThreatEndPosX:F1}";
                ATEndPosY.Content = $"{fixedAirThreatEndPosY:F1}";
                HideMouseInfo();

                airThreatEndflg = 1;
                if (airThreatStartflg == 1)
                    ConnectLine();

                if (imgControl5 != null)
                    myCanvas.Children.Remove(imgControl5);

                //공중위협 목적지 이미지 클릭 위치에 추가하기
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
                MSLStartPosX.Content = $"{fixedMSLStartPosX:F1}";
                MSLStartPosY.Content = $"{fixedMSLStartPosY:F1}";
                HideMouseInfo();

                MSLStartflg = 1;
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
                ellipse.Stroke = Brushes.Green;
                ellipse.StrokeThickness = 2;
                ellipse.Fill = new SolidColorBrush(Color.FromArgb(80, 0, 255, 0)); ;
                ellipse.Width = MSLRadius;
                ellipse.Height = MSLRadius;
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
            mousePositionTextBox.Text = $"위도 {x:F1}\n경도 {y:F1}";
            // 좌표 선택하는 중에만 지도 위에 현재 마우스 커서가 가리키는 좌표를 출력하는 출력창을 보이게 하는 기능
            mousePositionTextBox.Visibility = Visibility.Visible;
        }

        private bool IsNumeric(string value)
        {
            return double.TryParse(value, out _);
        }
        //---------------------------------------------------------------

        private void SimulationStart_Click(object sender, RoutedEventArgs e)
        {
            // 시작클릭시 공중위협 모의기, 대공유도탄 모의기에 데이터 설정값 Publisher
            EventLog.AppendText("시나리오 시작.\n");
            EventLog.ScrollToEnd();
            setPosMode = 0;

            //공중위협, 대공유도탄 설정 버튼 비활성화
            ATStartPosSetBTN.IsEnabled = false;
            ATEndPosSetBTN.IsEnabled = false;
            AirThreatSpeedInput.IsEnabled = false;
            MSLPosSetBTN.IsEnabled = false;
            MSLSpeedInput.IsEnabled = false;
            Start.IsEnabled = false;

            //최초 입력 값 현재 값으로 입력
            currentAirThreatPosX = fixedAirThreatStartPosX;
            currentAirThreatPosY = fixedAirThreatStartPosY;
            currentMSLPosX = fixedMSLStartPosX;
            currentMSLPosY = fixedMSLStartPosY;

            //불변 값인 공중위협, 대공유도탄 속도 출력
            ATCurrentSpeed.Content = fixedAirThreatSpeed + " Km/h";
            MSLCurrentSpeed.Content = fixedMSLSpeed + " Km/h";

            //최초 입력 값 현재좌표 화면 출력 (while에서 출력하므로 일단 주석)
            ATCurrentPosX.Content = $"{fixedAirThreatStartPosX:F1}";
            ATCurrentPosY.Content = $"{fixedAirThreatStartPosY:F1}";
            MSLCurrentPosX.Content = $"{fixedMSLStartPosX:F1}";
            MSLCurrentPosY.Content = $"{fixedMSLStartPosY:F1}";


            //★공중모의기로 대공유도탄, 공중위협 좌표 송신


            //★대공모의기로 대공유도탄, 공중위협 좌표 송신

            //★받아야하는 값: 공중좌표 대공좌표 대공각도 충돌여부
            //currentAirThreatPos, currentMSLPos, MSLangle, crash
            //while (true)
            //{
            //    //★충돌 여부 수신
            //    if (crash == 1)
            //        break;

            //    //★공중위협 좌표 수신

            //    //공중위협 이미지 배치, 값 출력
            //    //Canvas.SetLeft(imgControl4, currentAirThreatPosX - (imgControl4.Width / 2.0));
            //    //Canvas.SetTop(imgControl4, currentAirThreatPosY - (imgControl4.Height / 2.0));
            //    //ATCurrentPosX.Content = $"{currentAirThreatPosX:F1}";
            //    //ATCurrentPosY.Content = $"{currentAirThreatPosY:F1}";

            //    //★대공유도탄 좌표,각도 수신

            //    //대공유도탄 이미지 배치, 값 출력
            //    //Canvas.SetLeft(imgControl6, currentMSLPosX - (imgControl6.Width / 2.0));
            //    //Canvas.SetTop(imgControl6, currentMSLPosY - (imgControl6.Height / 2.0));
            //    //MSLCurrentPosX.Content = $"{currentMSLPosX:F1}";
            //    //MSLCurrentPosY.Content = $"{currentMSLPosY:F1}";

            //    ////대공유도탄 이미지 회전
            //    //MSLrotateTransform.Angle = MSLangle;
            //    //// 대공유도탄 이미지의 중심을 회전 중심으로 지정
            //    //imgControl6.RenderTransformOrigin = new Point(0.5, 0.5);
            //    //imgControl6.RenderTransform = MSLrotateTransform;
            //    //// 대공유도탄 각도 출력
            //    //if (0 <= MSLangle && MSLangle <= 90)
            //    //    MSLCurrentDIR.Content = $"{90 - MSLangle:F0}" + "°";
            //    //else
            //    //    MSLCurrentDIR.Content = $"{450 - MSLangle:F0}" + "°";
            //    //crash = 1;
            //}

            /*//거리 연산
            double distance = AT2MSLDistance();
            if (distance <= MSLRadius) //대공유도탄 발사 명령
                MSLLaunch = 1;

            //충돌 여부 확인 (값 지정 필요)
            if (distance <= 2)
            {
                return;
            }*/
        }
        private double AT2MSLDistance()
        {
            return Math.Sqrt(Math.Pow(currentAirThreatPosX - currentMSLPosX, 2) + Math.Pow(currentAirThreatPosY - currentMSLPosY, 2));
        }

        private void SimulationEnd_Click(object sender, RoutedEventArgs e)
        {
            // 모든 시나리오 데이터 설정값 초기화
            ATStartPosSetBTN.IsEnabled = true;
            ATEndPosSetBTN.IsEnabled = true;
            AirThreatSpeedInput.IsEnabled = true;
            MSLPosSetBTN.IsEnabled = true;
            MSLSpeedInput.IsEnabled = true;
            Start.IsEnabled = true;
            EventLog.AppendText("시나리오 종료.\n");
            EventLog.ScrollToEnd();
            airThreatStartflg = 0;
            airThreatEndflg = 0;
            MSLStartflg = 0;
            setPosMode = 0;
            ATStartPosX.Content = "NO DATA";
            ATStartPosY.Content = "NO DATA";
            ATEndPosX.Content = "NO DATA";
            ATEndPosY.Content = "NO DATA";
            MSLStartPosX.Content = "NO DATA";
            MSLStartPosY.Content = "NO DATA";
            ATCurrentPosX.Content = "NO DATA";
            ATCurrentPosY.Content = "NO DATA";
            ATCurrentSpeed.Content = "NO DATA";
            ATCurrentDIR.Content = "NO DATA";
            MSLCurrentPosX.Content = "NO DATA";
            MSLCurrentPosY.Content = "NO DATA";
            MSLCurrentSpeed.Content = "NO DATA";
            MSLCurrentDIR.Content = "NO DATA";

            myCanvas.Children.Remove(imgControl4);
            myCanvas.Children.Remove(imgControl5);
            myCanvas.Children.Remove(imgControl6);
            myCanvas.Children.Remove(line);
            myCanvas.Children.Remove(ellipse);
        }
        //---------------------------------------------------------------
    }
}