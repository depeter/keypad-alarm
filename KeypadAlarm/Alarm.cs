using System.Diagnostics;
using System.Runtime.InteropServices.WindowsRuntime;

namespace MatrixKeypad
{
    public static class Alarm
    {
        public static AlarmMode Mode { get; private set; }

        static Alarm()
        {
            Mode = AlarmMode.Disarmed;
        }

        public static void Arm(bool updateLoxone = true)
        {
            Mode = AlarmMode.Armed;

            if (updateLoxone)
                UdpClient.Send("ArmAlarm");

            Led.On();
            Sounds.Armed();
            Debug.WriteLine("Alarm Armed!");
        }

        public static void Disarm(bool updateLoxone = true)
        {
            Mode = AlarmMode.Disarmed;

            if (updateLoxone)
                UdpClient.Send("DisarmAlarm");

            Led.Off();
            Sounds.Disarmed();
            Debug.WriteLine("Alarm Disarmed!");
        }
    }
}