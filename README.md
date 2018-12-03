## Matrix 4x4 Keypad Alarm
This project connects a 4x4 keypad module to a raspberry pi.

The setup is as follows:
![setup image](https://raw.githubusercontent.com/depeter/keypad-alarm/master/setup.jpg "setup image")

### Keypad setup
| Leypad PIN (Left to Right) | Raspberry Pi 3 PIN |
|----------------------------|--------------------|
| PIN 1 (Col 1)              | GPIO 19 (pin 35)   |
| PIN 2 (Col 2)              | GPIO 13 (pin 33)   |
| PIN 3 (Col 3)              | GPIO 6  (pin 31)   |
| PIN 4 (Col 4)              | GPIO 5  (pin 29)   |
| PIN 5 (Row 1)              | GPIO 21 (pin 40)   |
| PIN 6 (Row 2)              | GPIO 20 (pin 38)   |
| PIN 7 (Row 3)              | GPIO 16 (pin 36)   |
| PIN 8 (Row 4)              | GPIO 26 (pin 37)   |

### Tweet setup
| Tweeter PIN | Raspberry Pi 3 PIN |
|-------------|--------------------|
| +           | GPIO 4 (pin 7)     |
| -           | Ground (pin 6)     |

### Loxone communication
In Loxone I have setup UDP inputs and outputs to keep the Central Alarm block in sync with my setup.