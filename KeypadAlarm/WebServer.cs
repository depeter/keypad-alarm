using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using Windows.Networking.Sockets;
using Windows.Storage.Streams;

namespace MatrixKeypad
{
    public static class WebServer
    {
        public static async void Start(int port)
        {
            var listener = new StreamSocketListener();
            await listener.BindServiceNameAsync(port.ToString());
            Debug.WriteLine("Webserver bound to port: " + port);
            listener.ConnectionReceived += async (s, e) =>
            {
                Debug.WriteLine("Got connection");
                using (var input = e.Socket.InputStream)
                {
                    var buffer = new Windows.Storage.Streams.Buffer(2);
                    await input.ReadAsync(buffer, buffer.Capacity, InputStreamOptions.Partial);
                }

                using (var output = e.Socket.OutputStream)
                {
                    using (var response = output.AsStreamForWrite())
                    {
                        response.Write(Encoding.ASCII.GetBytes(Alarm.Mode == AlarmMode.Armed ? "1" : "0"), 0, 1);
                    }
                }
            };
        }
    }
}