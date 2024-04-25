﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;

namespace OperationController.DisplayManage
{
    /// <summary>
    /// DialogWindow.xaml에 대한 상호 작용 논리
    /// </summary>
    public partial class DialogWindow : Window
    {
        private static readonly string successImagePath = "../map/SUCCESS.jpg";
        private static readonly string failImagePath = "../map/FAIL.jpg";
        private static DialogWindow successWindow;
        private static DialogWindow failWindow;

        public static DialogWindow GetSuccessDialog()
        {
            if (successWindow == null)
        {
            successWindow = new DialogWindow(successImagePath, "격추 성공");
        }
            return successWindow;
        }
        public static DialogWindow GetFailDialog()
        {
            if (failWindow == null)
            {
                failWindow = new DialogWindow(failImagePath, "격추 실패");
            }
            return failWindow;
        }

        private DialogWindow(string imagePath, string message)
        {
            InitializeComponent();

            BitmapImage bitmap = new BitmapImage();
            bitmap.BeginInit();
            bitmap.UriSource = new Uri(imagePath, UriKind.Relative);
            bitmap.EndInit();
            DialogImage.Source = bitmap;

            DialogText.Text = message;
        }

        private void OnCloseButtonClick(object sender, RoutedEventArgs e)
        {
            this.Close();
        }
    }
}
