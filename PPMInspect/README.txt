
TODO:
  - PPM ISR: refactor code
  - PPM read analog: adjust factor
  - Analog read ISR
  - Config Screen: display current voltage



  123456789012345678901
1|PPM  sync 12 Channels|
2|Frame  22000 22000 us|
3|Pulse    300   300 us|
4|Pulse HIGH           |
5|Level    0.4V   10.2V|
6|Frames     3523626345|
7|Bad                27|
8|Overrun             0|

1|CH01 1500 [1000,2000]|
2|CH02 1500 [1000,2000]|
3|CH03 1500 [1000,2000]|
4|CH04 1500 [1000,2000]|
5|CH05 1500 [1000,2000]|
6|CH06 1500 [1000,2000]|
7|CH07 1500 [1000,2000]|
8|CH08 1500 [1000,2000]|


  123456789012345678901
1|PPM Analyze 0.1.10   |
2|Start                |
3|Min Sync    3000 usec|
4|Volt adjust         0|
5|                     |
6|                     |
7|                     |
8|                     |


Arduino Pro Micro
LCD SCK   3
LCD SDA   2
Button 1  14
Button 2  15
Button 3  16
Analog In
PPM In

Arduino Nano
LCD SCK   A5 (Port PC5 SCL)
LCD SDA   A4 (Port PC4 SDA)
Button 1   5 (Port PD5 PCINT21 PCI2)
Button 2   6 (Port PD6 PCINT22 PCI2)
Button 3   7 (Port PD7 PCINT23 PCI2)
Analog In A3 (Port PC3)
PPM In    A2 (Port PC2 PCINT10 PCI1)

