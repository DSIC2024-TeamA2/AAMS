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

        //공중위협, 목적지 연결하는 선 확인 flg
        private int airThreatStartflg = 0;
        private int airThreatEndflg = 0;
        Line line = new Line();

        //공중위협, 미사일, 목적지, 반경 최초 선언
        System.Windows.Controls.Image imgControl4 = null;
        System.Windows.Controls.Image imgControl5 = null;
        System.Windows.Controls.Image imgControl6 = null;
        Ellipse ellipse = new Ellipse();

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

                airThreatStartflg = 1;
                if(airThreatEndflg == 1)
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

                mousePositionTextBox.Visibility = Visibility.Hidden;
            }
            else if (fixAirThreatPox == 5)
            {
                ATEndPosX.Content = $"{fixedAirThreatEndPosX:F3}";
                ATEndPosY.Content = $"{fixedAirThreatEndPosY:F3}";

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

                mousePositionTextBox.Visibility = Visibility.Hidden;
            }
            else if (fixAirThreatPox == 6)
            {
                MSLStartPosX.Content = $"{fixedMSLStartPosX:F3}";
                MSLStartPosY.Content = $"{fixedMSLStartPosY:F3}";

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

                mousePositionTextBox.Visibility = Visibility.Hidden;
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

        private void SimulationStart_Click(object sender, RoutedEventArgs e)
        {
            // 시작클릭시 공중위협 모의기, 대공유도탄 모의기에 데이터 설정값 Publisher

            //미사일 반경과 공중위협 경로가 겹치는지 판단
            if (!Intersect(ellipse, line))
            {
                EventLog.Text += "공중위협 경로와 미사일 반경이 겹치지 않습니다.\n";
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

            // 직선의 y절편
            double lineIntercept = lineY1 - lineSlope * lineX1;

            // 원의 중심에서 직선까지의 거리
            double distance = Math.Abs((lineSlope * circleCenterX - circleCenterY + lineIntercept) / Math.Sqrt(lineSlope * lineSlope + 1));

            // 거리가 반지름보다 작거나 같으면 겹침
            return distance <= circle.Width / 2;
        }

        private void SimulationEnd_Click(object sender, RoutedEventArgs e)
        {
            // 모든 시나리오 데이터 설정값 초기화
        }
        //---------------------------------------------------------------
    }
}
