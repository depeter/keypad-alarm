using System.Threading.Tasks;
using Windows.Devices.Gpio;

namespace MatrixKeypad
{
    public static class Sounds
    {
        private static readonly int PIN = 4;
        private static GpioPin _pin;

        // tut
        public static void KeyPress()
        {
            if (_pin == null)
                _pin = GpioController.GetDefault().OpenPin(PIN);

            Task.Run(async () =>
            {
                try
                {
                    _pin.SetDriveMode(GpioPinDriveMode.Output);
                    _pin.Write(GpioPinValue.High);
                    await Task.Delay(60);
                    _pin.Write(GpioPinValue.Low);
                }
                catch (System.IO.FileLoadException)
                {
                    //ignore
                }

            });
        }

        // tut tut tut tut
        public static void Disarmed()
        {
            if (_pin == null)
                _pin = GpioController.GetDefault().OpenPin(PIN);

            Task.Run(async () =>
            {
                try
                {
                    await Task.Delay(200);
                    _pin.SetDriveMode(GpioPinDriveMode.Output);
                    _pin.Write(GpioPinValue.High);
                    await Task.Delay(60);
                    _pin.Write(GpioPinValue.Low);
                    await Task.Delay(20);
                    _pin.Write(GpioPinValue.High);
                    await Task.Delay(60);
                    _pin.Write(GpioPinValue.Low);
                    await Task.Delay(20);
                    _pin.Write(GpioPinValue.High);
                    await Task.Delay(60);
                    _pin.Write(GpioPinValue.Low);
                }
                catch (System.IO.FileLoadException)
                {
                    //ignore
                }

            });
        }

        // tut tut tuuuuuuut
        public static void Armed()
        {
            if (_pin == null)
                _pin = GpioController.GetDefault().OpenPin(PIN);

            Task.Run(async () =>
            {
                try
                {
                    await Task.Delay(400);
                    _pin.SetDriveMode(GpioPinDriveMode.Output);
                    _pin.Write(GpioPinValue.High);
                    await Task.Delay(50);
                    _pin.Write(GpioPinValue.Low);

                    await Task.Delay(200);
                    _pin.Write(GpioPinValue.High);
                    await Task.Delay(50);
                    _pin.Write(GpioPinValue.Low);
                    await Task.Delay(200);
                }
                catch (System.IO.FileLoadException)
                {
                    //ignore
                }
            });
        }
    }
}