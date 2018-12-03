using System;
using System.Diagnostics;
using System.IO;
using Windows.Networking;
using Windows.Networking.Sockets;

namespace MatrixKeypad
{
    public static class UdpServer
    {
        private const string PORT = "7001";
        private const string LOXONE_IP = "192.168.1.163";

        private static DatagramSocket _listener;

        public static async void Start()
        {
            _listener = new DatagramSocket();
            _listener.MessageReceived += ListenerSocketOnMessageReceived;
            await _listener.BindServiceNameAsync(PORT);
        }

        private static void ListenerSocketOnMessageReceived(DatagramSocket sender, DatagramSocketMessageReceivedEventArgs args)
        {
            Debug.WriteLine("UDP Message received");
            if (args.RemoteAddress.ToString() == LOXONE_IP)
            {
                var message = new StreamReader(args.GetDataStream().AsStreamForRead()).ReadToEnd();
                Debug.WriteLine("UDP Message received: " + message + " from " + args.RemoteAddress);

                if (message == "AlarmArmed" && Alarm.Mode != AlarmMode.Armed)
                    Alarm.Arm(false);

                if (message == "AlarmDisarmed" && Alarm.Mode != AlarmMode.Disarmed)
                    Alarm.Disarm(false);
            }
        }
    }
}