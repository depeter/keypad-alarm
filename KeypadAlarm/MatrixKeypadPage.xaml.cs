using System.Collections.Generic;
using System.Diagnostics;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace MatrixKeypad
{
    /// <summary>
    /// Example app for getting input from Matrix Keypad
    /// </summary>
    public sealed partial class MatrixKeypadPage : Page
    {
        private string _inputText = string.Empty;
        private const string PINCODE = "3369";
        private const string ARM_PINCODE = "###";

        public MatrixKeypadPage()
        {
            this.InitializeComponent();

            // List is the GPIO pins that are used from line 1 to line 8
            var matrixPad = new MatrixKeypadMonitor(new List<int> { 19, 13, 6, 5, 21, 20, 16, 26 });

            // Subscribe to an event that is triggered when a keypress happens
            if (matrixPad.SetupSuccessful)
            {
                matrixPad.FoundADigitEvent += FoundDigit;
            }
            else
            {
                Debug.WriteLine(matrixPad.SetupMessage);
            }

            UdpServer.Start();
        }

        public void FoundDigit(object sender, string digit)
        {
            _inputText += digit;
            if (_inputText.Length > 10) _inputText = _inputText.Substring(_inputText.Length - 10, 10);

            Sounds.KeyPress();

            if (_inputText.EndsWith(ARM_PINCODE))
            {
                Alarm.Arm();
                _inputText = string.Empty;
            }

            if (Alarm.Mode == AlarmMode.Armed && _inputText.EndsWith(PINCODE + "#"))
            {
                Alarm.Disarm();
                _inputText = string.Empty;
            }
        }

        private void EnableAlarm_OnClick(object sender, RoutedEventArgs e)
        {
            Alarm.Arm();
        }

        private void DisableAlarm_OnClick(object sender, RoutedEventArgs e)
        {
            Alarm.Disarm();
        }
    }
}
