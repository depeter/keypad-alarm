using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Devices.Gpio;

namespace MatrixKeypad
{
    public static class Led
    {
        private const int PIN = 18;
        private static GpioPin _pin;

        public static void On()
        {
            if (_pin == null)
                _pin = GpioController.GetDefault().OpenPin(PIN);

            Debug.WriteLine("Turning on LED");
            Task.Run(async () =>
            {
                try
                {
                    _pin.Write(GpioPinValue.High);
                    _pin.SetDriveMode(GpioPinDriveMode.Output);
                    Debug.WriteLine("Led is ON");
                    await Task.Delay(100);
                }
                catch (Exception e)
                {
                    Debug.WriteLine("Can't turn led ON: " + e);
                }
            });
        }

        public static void Off()
        {
            if (_pin == null)
                _pin = GpioController.GetDefault().OpenPin(PIN);

            Debug.WriteLine("Turning off LED");
            Task.Run(async () =>
            {
                try
                {
                    _pin.Write(GpioPinValue.Low);
                    _pin.SetDriveMode(GpioPinDriveMode.Output);
                    await Task.Delay(100);
                }
                catch (Exception e)
                {
                    Debug.WriteLine("Can't turn led OFF: " + e);
                }
            });
        }
    }
}