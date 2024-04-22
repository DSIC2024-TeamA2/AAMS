using OperationController.DisplayManage;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;

namespace OperationController.AMSUDP
{
    public class nFrameworkConnector
    {
        [DllImport("GUIConnectord.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static IntPtr CreateGUIConn();
        [DllImport("GUIConnectord.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void DeleteGUIConn(IntPtr GUIConn);
        [DllImport("GUIConnectord.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void SetHandle(IntPtr GUIConn, IntPtr hwnd);
        [DllImport("GUIConnectord.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void SendMsg(IntPtr GUIConn, NOMInfo nomInfo, IntPtr nomBytes);
        [DllImport("GUIConnectord.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void DoPlugIn(IntPtr GUIConn);
        [DllImport("GUIConnectord.dll", CallingConvention = CallingConvention.Cdecl)]
        extern public static void DoPlugOut(IntPtr GUIConn);
        public IntPtr GUIConnObj;
        public const Int32 WM_USER = 0x0400;
        public const Int32 WM_SEND_DATA = WM_USER + 0x01;
        public const Int32 UM_ReflectedNOM = WM_USER + 0x02;
        private Window mainWindow;

        public nFrameworkConnector(AMSMainWindow window)
        {
            Console.WriteLine("nFrameworkConnector(Window window) called");
            mainWindow = window;
            HwndSource source = PresentationSource.FromVisual(mainWindow) as HwndSource;
            source.AddHook(WndProc);
            GUIConnObj = CreateGUIConn();
            DoPlugIn(GUIConnObj);
            SetHandle(GUIConnObj, GetWindowHandle(mainWindow));
            Console.WriteLine("SetHandle(GUIConnObj, GetWindowHandle(mainWindow)) called");
        }

        //윈도우 핸들 얻기
        private IntPtr GetWindowHandle()
        {
            return new WindowInteropHelper(mainWindow).Handle;
        }

        //WndProc 함수 : C++로부터 수신한 NOM data 처리
        private IntPtr WndProc(IntPtr hwnd, Int32 msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            Console.WriteLine("WndProc called");
            if (msg == WM_SEND_DATA)
            {
                // if need be, write your code
            }
            else if (msg == UM_ReflectedNOM)
            {
                NOMInfo nomInfo = new NOMInfo();
                byte[] msgBuffer = new byte[nomInfo.MsgLen];
                Marshal.Copy(lParam, msgBuffer, 0, nomInfo.MsgLen);

                // Parsing

                nomInfo = (NOMInfo)Marshal.PtrToStructure(wParam, typeof(NOMInfo));
            //    listBox.Items.Add(nomInfo.MsgName.ToString());
                Console.WriteLine("listBox.Items.Add(nomInfo.MsgName.ToString()) called");
                Console.WriteLine("nomInfo: " + nomInfo.MsgID);
            }
            else
            {
                handled = false;
            }
            return IntPtr.Zero;
        }

        public void SendMsg()
        {
            Console.WriteLine("SendMsg called");
            NOMParser parser = new NOMParser();
            parser.nomFilePath = "GUI_NOM.xml";
            parser.parse();

            NMessage icdMsg = parser.getMessageObject("GUI_Start");
            NOM startNOM = icdMsg.createNOMInstance();

            int byteSize = 0;
            byte[] nomBytes = startNOM.serialize(out byteSize);

            NOMInfo nomInfo = new NOMInfo();
            nomInfo.MsgName = "GUI_Start";
            nomInfo.MsgID = 1001;
            nomInfo.MsgLen = (int)startNOM.length;

            IntPtr ptr = Marshal.AllocHGlobal(nomInfo.MsgLen);

            Marshal.Copy(nomBytes, 0, ptr, nomInfo.MsgLen);
            SendMsg(GUIConnObj, nomInfo, ptr);
            Console.WriteLine("SendMsg(GUIConnObj, nomInfo, ptr) called");
            Console.WriteLine("nomInfo: " + nomInfo.MsgID);
        }
        public void Close()
        {
            DoPlugOut(GUIConnObj);
            DeleteGUIConn(GUIConnObj);
            GUIConnObj = IntPtr.Zero;
        }
    }
}
