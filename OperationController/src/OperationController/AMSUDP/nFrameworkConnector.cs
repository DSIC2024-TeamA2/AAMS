﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Interop;
using nframework.nom;
using OperationController.DisplayManage;
using WpfApplication;

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
        private MainWindow mainWindow;

        public nFrameworkConnector(MainWindow window)
        {
            mainWindow = window;
            PresentationSource presentationSource = PresentationSource.FromVisual(mainWindow);
            HwndSource source = presentationSource as HwndSource;
            source.AddHook(WndProc);
            GUIConnObj = CreateGUIConn();
            DoPlugIn(GUIConnObj);
            SetHandle(GUIConnObj, GetWindowHandle());
        }

        //윈도우 핸들 얻기
        private IntPtr GetWindowHandle()
        {
            return new WindowInteropHelper(mainWindow).Handle;
        }

        //WndProc 함수 : C++로부터 수신한 NOM data 처리
        private IntPtr WndProc(IntPtr hwnd, Int32 msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            if (msg == WM_SEND_DATA)
            {
                // if need be, write your code
            }
            else if (msg == UM_ReflectedNOM)
            {
                NOMInfo nomInfo = new NOMInfo();
                nomInfo = (NOMInfo)Marshal.PtrToStructure(wParam, typeof(NOMInfo));
                byte[] msgBuffer = new byte[nomInfo.MsgLen];
                Marshal.Copy(lParam, msgBuffer, 0, nomInfo.MsgLen);

                NOMParser parser = new NOMParser();
                parser.nomFilePath = "GUI_NOM.xml";
                parser.parse();

                if (nomInfo.MsgID == 9992)
                {
                    NMessage icdMsg = parser.getMessageObject("AirThreatInfo");
                    NOM icdNOM = icdMsg.createNOMInstance();
                    icdNOM.deserialize(msgBuffer, nomInfo.MsgLen);
                    AirThreatInfo info = new AirThreatInfo();
                    Position pos = new Position();
                    info.CurrentTime = icdNOM.getValue("currentTime").toInt();
                    pos.Latitude = icdNOM.getValue("currentLatitude").toDouble();
                    pos.Longitude = icdNOM.getValue("currentLongitude").toDouble();
                    info.CurrentPosition = pos;
                    info.CurrentSpeed = icdNOM.getValue("currentSpeed").toFloat();
                    info.CurrentAngle = icdNOM.getValue("currentAngle").toFloat();
                    mainWindow.UpdateAirThreatInfo(info);
                }
                // AntiAirMissileInfo
                else if (nomInfo.MsgID == 9993)
                {
                    NMessage icdMsg = parser.getMessageObject("AntiAirMissileInfo");
                    NOM icdNOM = icdMsg.createNOMInstance();
                    icdNOM.deserialize(msgBuffer, nomInfo.MsgLen);
                    AntiAirMissileInfo info = new AntiAirMissileInfo();
                    Position pos = new Position();
                    info.CurrentTime = icdNOM.getValue("currentTime").toInt();
                    pos.Latitude = icdNOM.getValue("currentLatitude").toDouble();
                    pos.Longitude = icdNOM.getValue("currentLongitude").toDouble();
                    info.CurrentPosition = pos;
                    info.CurrentSpeed = icdNOM.getValue("currentSpeed").toFloat();
                    info.CurrentAngle = icdNOM.getValue("currentAngle").toFloat();
                    mainWindow.UpdateAntiAirMissileInfo(info);
                }
                // SimulationStatusInfo
                else if (nomInfo.MsgID == 9994)
                {
                    NMessage icdMsg = parser.getMessageObject("SimulationStatusInfo");
                    NOM icdNOM = icdMsg.createNOMInstance();
                    icdNOM.deserialize(msgBuffer, nomInfo.MsgLen);
                    int status = icdNOM.getValue("status").toInt();

                    SimulationStatusInfo info = SimulationStatusInfoExtensions.ValueOf(status);
                    mainWindow.UpdateSimulationStatusInfo(info);
                }
            }
            else
            {
                handled = false;
            }
            return IntPtr.Zero;
        }

        public void SendSimulationStatusInfoMsg(SimulationStatusInfo simulationStatusInfo)
        {
            NOMParser parser = new NOMParser();
            parser.nomFilePath = "GUI_NOM.xml";
            parser.parse();

            NMessage icdMsg = parser.getMessageObject("SimulationStatusInfo");
            NOM startNOM = icdMsg.createNOMInstance();
            NEnum enumType = new NEnum();
            enumType.enumValue = simulationStatusInfo.ToInt();
            startNOM.setValue("status", enumType);
            int byteSize = 0;
            byte[] nomBytes = startNOM.serialize(out byteSize);

            NOMInfo nomInfo = new NOMInfo();
            nomInfo.MsgName = "SimulationStatusInfo";
            nomInfo.MsgID = 9994;
            nomInfo.MsgLen = (int)startNOM.length;

            IntPtr ptr = Marshal.AllocHGlobal(nomInfo.MsgLen);

            Marshal.Copy(nomBytes, 0, ptr, nomInfo.MsgLen);
            SendMsg(GUIConnObj, nomInfo, ptr);
        }

        public void SendScenarioInfoMsg(int startTime, double airThreatStartLatitude, double airThreatStartLongitude,
            double airThreatEndLatitude, double airThreatEndLongitude, float airThreatSpeed,
            double antiAirMissileLatitude, double antiAirMissileLongitude, float antiAirMissileSpeed)
        {
            NOMParser parser = new NOMParser();
            parser.nomFilePath = "GUI_NOM.xml";
            parser.parse();

            NMessage icdMsg = parser.getMessageObject("ScenarioInfo");
            NOM startNOM = icdMsg.createNOMInstance();
            startNOM.setValue("startTime", new NInteger(startTime));
            startNOM.setValue("airThreatStartLatitude", new NDouble(airThreatStartLatitude));
            startNOM.setValue("airThreatStartLongitude", new NDouble(airThreatStartLongitude));
            startNOM.setValue("airThreatEndLatitude", new NDouble(airThreatEndLatitude));
            startNOM.setValue("airThreatEndLongitude", new NDouble(airThreatEndLongitude));
            startNOM.setValue("airThreatSpeed", new NFloat(airThreatSpeed));
            startNOM.setValue("antiAirMissileLatitude", new NDouble(antiAirMissileLatitude));
            startNOM.setValue("antiAirMissileLongitude", new NDouble(antiAirMissileLongitude));
            startNOM.setValue("antiAirMissileSpeed", new NFloat(antiAirMissileSpeed));
            int byteSize = 0;
            byte[] nomBytes = startNOM.serialize(out byteSize);

            NOMInfo nomInfo = new NOMInfo();
            nomInfo.MsgName = "ScenarioInfo";
            nomInfo.MsgID = 9991;
            nomInfo.MsgLen = (int)startNOM.length;

            IntPtr ptr = Marshal.AllocHGlobal(nomInfo.MsgLen);

            Marshal.Copy(nomBytes, 0, ptr, nomInfo.MsgLen);
            SendMsg(GUIConnObj, nomInfo, ptr);
        }

        public void Close()
        {
            DoPlugOut(GUIConnObj);
            DeleteGUIConn(GUIConnObj);
            GUIConnObj = IntPtr.Zero;
        }
    }
}
