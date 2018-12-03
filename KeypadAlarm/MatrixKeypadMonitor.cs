using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Devices.Gpio;
using Windows.UI.Xaml;

namespace MatrixKeypad
{
    /// <summary>
    /// Class that handles input from a 4x4 matrix keypad
    /// Usage Example:
    ///    // Send in a list of the GPIO pins that are used from line 1 to line 8
    ///    var matrixPad = new MatrixKeypadMonitor(new List<int> { 16, 20, 21, 5, 6, 13, 19, 26 });
    ///    // Subscribe to an event that is triggered when a keypress happens
    ///    if (matrixPad.SetupSuccessful)
    ///    {
    ///      matrixPad.FoundADigitEvent += FoundDigit;
    ///    }
    ///    // Do something when a key is pressed 
    ///    public void FoundDigit(object sender, string digit)
    ///    {
    ///       Debug.WriteLine(string.Format("Found {0}", digit));
    ///    }
    /// </summary>
    public class MatrixKeypadMonitor
    {
        #region Variables
        public bool SetupSuccessful { get; set; }
        public string SetupMessage { get; set; }
        public bool VerboseMode => true;

        private static GpioController _gpio = null;
        private readonly List<int> _pins;
        private readonly List<GpioPin> _rows = new List<GpioPin>();
        private readonly List<GpioPin> _cols = new List<GpioPin>();

        private readonly string[,] _keypad = {
            {"1", "2", "3", "A"},
            {"4", "5", "6", "B"},
            {"7", "8", "9", "C"},
            {"*", "0", "#", "D"}
        };

        #endregion

        #region Initialization
        public MatrixKeypadMonitor(List<int> pins)
        {
            SetupSuccessful = false;
            // validate inputs
            if (pins != null && pins.Count == 8)
            {
                _pins = pins;
                // Set up the GPIO controls
                if (InitializeGpio(_pins))
                {
                    // Start Listening
                    Loop();
                    SetupSuccessful = true;
                }
                else
                {
                    SetupMessage = "GPIO Initialization failed! " + SetupMessage;
                    if (VerboseMode) Debug.WriteLine(SetupMessage);
                }
            }
            else
            {
                SetupMessage = "Please supply a list of 8 GPIO pin numbers.";
                if (VerboseMode) Debug.WriteLine(SetupMessage);
            }
        }

        private bool InitializeGpio(IReadOnlyList<int> pins)
        {
            try
            {
                // Initialize GPIO controller
                _gpio = GpioController.GetDefault();
                if (_gpio != null)
                {
                    // Initialize Column GPIO Pins
                    _cols.Add(_gpio.OpenPin(pins[0]));
                    _cols.Add(_gpio.OpenPin(pins[1]));
                    _cols.Add(_gpio.OpenPin(pins[2]));
                    _cols.Add(_gpio.OpenPin(pins[3]));

                    // Set the columns up for output
                    foreach (var c in _cols)
                    {
                        c.SetDriveMode(GpioPinDriveMode.Output);
                        c.Write(GpioPinValue.High);
                    }

                    // Initialize Row GPIO Pins
                    var r1Pin = _gpio.OpenPin(pins[4]);
                    var r2Pin = _gpio.OpenPin(pins[5]);
                    var r3Pin = _gpio.OpenPin(pins[6]);
                    var r4Pin = _gpio.OpenPin(pins[7]);
                    
                    // Add to Row Array
                    _rows.Add(r1Pin);
                    _rows.Add(r2Pin);
                    _rows.Add(r3Pin);
                    _rows.Add(r4Pin);

                    // Set the rows up for input
                    foreach (var r in _rows)
                    {
                        r.SetDriveMode(GpioPinDriveMode.Output);
                        r.Write(GpioPinValue.Low);
                    }

                    // Set the rows up for input
                    foreach (var r in _rows)
                    {
                        r.SetDriveMode(GpioPinDriveMode.InputPullUp);
                    }
                    return true;
                }
                SetupMessage = "GPIO Controller not found!";
                if (VerboseMode) Debug.WriteLine(SetupMessage);
                return false;
            }
            catch (Exception ex)
            {
                SetupMessage = ex.Message;
                if (VerboseMode) Debug.WriteLine(SetupMessage);
                return false;
            }
        }

        #endregion

        #region Main Processes
        private void Loop()
        {
            Task.Run(async () =>
            {
                var prevDigit = "";
                DateTime prevDigitEnteredAt = DateTime.Now;

                while (true)
                {
                    foreach (var col in _cols)
                    {
                        col.Write(GpioPinValue.Low);

                        foreach (var row in _rows)
                        {
                            if (row.Read() == GpioPinValue.Low)
                            {
                                var rowIndex = _pins.IndexOf(row.PinNumber, 4, 4) - 4;
                                var colIndex = _pins.IndexOf(col.PinNumber, 0, 4);

                                var ch = _keypad[rowIndex, colIndex];
                                if (prevDigit == ch && Math.Abs((DateTime.Now - prevDigitEnteredAt).TotalMilliseconds) < 200)
                                    break;

                                prevDigitEnteredAt = DateTime.Now;
                                prevDigit = ch;
                                FoundADigit(ch);
                            }
                        }

                        col.Write(GpioPinValue.High);
                    }
                    await Task.Delay(20);
                }
            });
        }

        #endregion

        #region Digit Found Message Delegate
        /// <summary>
        /// Digit Found Delegate
        /// </summary>
        /// <param name="sender">The sender</param>
        /// <param name="digit">The digit that was pressed</param>
        public delegate void FoundADigitPublisher(object sender, string digit);

        /// <summary>
        /// Event occurs when a digit is found and published
        /// </summary>
        public event FoundADigitPublisher FoundADigitEvent;

        /// <summary>
        /// Sends message to subscribers telling them a digit was pressed
        /// </summary>
        /// <param name="digit">The digit</param>
        public void FoundADigit(string digit)
        {
            if (VerboseMode) Debug.WriteLine("Found character " + digit);
            //// If there are any subscribers then call them
            FoundADigitEvent?.Invoke(this, digit);
        }
        #endregion
    }
}
