# PPMInspect

Ein Tool zur Analyse von PPM Signalen.  

## Features

- PPM Decoder bis 16 Kanäle mit Anzeige der Timing Werte für jeden Kanal
- Einfaches Voltmeter
- Oszilloskop mit 1kHz bis 100kHz Samplingrate
- Trigger (Freilaufend, ansteigende Flanke, abfallende Flanke)
- Zuschaltbares oversampling
- Einstellbarer Triggerlevel
- Triggerverzögerung bis 9900 Microsekunden.


Zur [Bedienungsanleitung](MANUAL.md)

## Handware

- 1 Arduino Nano
- 1 OLED Display 1.3" (SSH1106 Controller)
- 3 Taster (RAFI Kurzhubtaster RACON 8 mit Stößel 7.5mm Höhe 8.2mm)
- 1 Z-Diode 4.7V
- 3 Widerstände 1/8 W 47k
- 2 Widerstände 1/8 W 100k
- 1 Elko 100uF 16V
- Stecker nach Wahl (Gebogene Pfostenleiste)


## Arduino Nano Ports

- LCD SCK:   A5 (Port PC5 SCL)
- LCD SDA:   A4 (Port PC4 SDA)
- Taster 1:   5 (Port PD5 PCINT21 PCI2)
- Taster 2:   6 (Port PD6 PCINT22 PCI2)
- Taster 3:   7 (Port PD7 PCINT23 PCI2)
- Analog In: A3 (Port PC3)
- PPM In:     8 (Port PB0 ICP1)
- Vcc:       A0
   
## Timer

- Timer 0:     8 bit       Arduino micros() millis() delay()...
- Timer 1:    16 bit       PPM Timing (See PPM.cpp)

## Verzeichnisse

CAD - dxf und FreeCAD Model für ein Gehäuse  
doc - Zur Dokumentation  
PCB - KiCAD Schaltplan und Platinenlayout  
PPMGenerate_DELAY - PPM Test Generator mit Verzögerungsschleife  
PPMGenerate_ISR - PPM Test Generator über Interrupts  
PPMGenerate_PWM - PPM Test Generator über Hardware-PWM  
PPMInspect - Der Source Code  

