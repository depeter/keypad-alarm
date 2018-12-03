using System;
using Windows.Networking;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace MatrixKeypad
{
    public static class UdpClient
    {
        private const string PORT = "7000";
        private const string LOXONE_IP = "192.168.1.163";

        public static async void Send(string message)
        {
            var listenerSocket = new DatagramSocket();
            var outputStream = await listenerSocket.GetOutputStreamAsync(new HostName(LOXONE_IP), PORT);

            using (DataWriter writer = new DataWriter(outputStream))
            {
                writer.WriteString(message);
                await writer.StoreAsync();
            }
        }
    }
}
