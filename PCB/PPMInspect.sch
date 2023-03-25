EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Diode:ZPDxx D1
U 1 1 63D63C15
P 7900 4700
F 0 "D1" H 7900 4917 50  0000 C CNN
F 1 "ZPD4.7" H 7900 4826 50  0000 C CNN
F 2 "Diode_THT:D_DO-35_SOD27_P10.16mm_Horizontal" H 7900 4525 50  0001 C CNN
F 3 "http://diotec.com/tl_files/diotec/files/pdf/datasheets/zpd1" H 7900 4700 50  0001 C CNN
	1    7900 4700
	0    1    1    0   
$EndComp
$Comp
L Device:R R2
U 1 1 63D642E9
P 7500 4200
F 0 "R2" H 7570 4246 50  0000 L CNN
F 1 "100k" H 7570 4155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7430 4200 50  0001 C CNN
F 3 "~" H 7500 4200 50  0001 C CNN
	1    7500 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 63D64C41
P 7500 4700
F 0 "R3" H 7570 4746 50  0000 L CNN
F 1 "47k" H 7570 4655 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7430 4700 50  0001 C CNN
F 3 "~" H 7500 4700 50  0001 C CNN
	1    7500 4700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 63D65000
P 7900 4200
F 0 "R1" H 7970 4246 50  0000 L CNN
F 1 "47k" H 7970 4155 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 7830 4200 50  0001 C CNN
F 3 "~" H 7900 4200 50  0001 C CNN
	1    7900 4200
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C1
U 1 1 63D65AA3
P 6750 2300
F 0 "C1" H 6868 2346 50  0000 L CNN
F 1 "CP" H 6868 2255 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D6.3mm_P2.50mm" H 6788 2150 50  0001 C CNN
F 3 "~" H 6750 2300 50  0001 C CNN
	1    6750 2300
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW1
U 1 1 63D66BBC
P 4150 3250
F 0 "SW1" H 4150 3535 50  0000 C CNN
F 1 "UP/NEXT" H 4150 3444 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4150 3450 50  0001 C CNN
F 3 "~" H 4150 3450 50  0001 C CNN
	1    4150 3250
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 63D671E3
P 4150 3750
F 0 "SW2" H 4150 4035 50  0000 C CNN
F 1 "ENTER" H 4150 3944 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4150 3950 50  0001 C CNN
F 3 "~" H 4150 3950 50  0001 C CNN
	1    4150 3750
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_Push SW3
U 1 1 63D67BBE
P 4150 4250
F 0 "SW3" H 4150 4535 50  0000 C CNN
F 1 "DOWN/RST" H 4150 4444 50  0000 C CNN
F 2 "Button_Switch_THT:SW_PUSH_6mm" H 4150 4450 50  0001 C CNN
F 3 "~" H 4150 4450 50  0001 C CNN
	1    4150 4250
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x03 J1
U 1 1 63D69CF2
P 2950 1750
F 0 "J1" H 2868 2067 50  0000 C CNN
F 1 "Vcc" H 2868 1976 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x03_P2.54mm_Horizontal" H 2950 1750 50  0001 C CNN
F 3 "~" H 2950 1750 50  0001 C CNN
	1    2950 1750
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 63D6A3D1
P 7950 2700
F 0 "J2" H 7900 3050 50  0000 L CNN
F 1 "LCD I2C" H 7850 2950 50  0000 L CNN
F 2 "Connector_PinSocket_2.54mm:PinSocket_1x04_P2.54mm_Vertical" H 7950 2700 50  0001 C CNN
F 3 "~" H 7950 2700 50  0001 C CNN
	1    7950 2700
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J3
U 1 1 63D6A9F9
P 4450 1300
F 0 "J3" V 4350 1400 50  0000 L CNN
F 1 "Pwr Switch" V 4450 1400 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Horizontal" H 4450 1300 50  0001 C CNN
F 3 "~" H 4450 1300 50  0001 C CNN
	1    4450 1300
	0    1    -1   0   
$EndComp
$Comp
L Connector_Generic:Conn_01x02 J4
U 1 1 63D6AEE1
P 7950 3350
F 0 "J4" H 7900 3600 50  0000 L CNN
F 1 "Data In" H 7850 3500 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Horizontal" H 7950 3350 50  0001 C CNN
F 3 "~" H 7950 3350 50  0001 C CNN
	1    7950 3350
	1    0    0    -1  
$EndComp
$Comp
L Device:R R4
U 1 1 63D6B6B6
P 6350 2150
F 0 "R4" H 6420 2196 50  0000 L CNN
F 1 "100k" H 6420 2105 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6280 2150 50  0001 C CNN
F 3 "~" H 6350 2150 50  0001 C CNN
	1    6350 2150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R5
U 1 1 63D6BB28
P 6350 2600
F 0 "R5" H 6420 2646 50  0000 L CNN
F 1 "47k" H 6420 2555 50  0000 L CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P10.16mm_Horizontal" V 6280 2600 50  0001 C CNN
F 3 "~" H 6350 2600 50  0001 C CNN
	1    6350 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	3150 1750 4350 1750
Wire Wire Line
	5500 1750 5500 2000
Wire Wire Line
	3150 1850 3300 1850
Wire Wire Line
	3300 1850 3300 4950
Wire Wire Line
	3300 4950 3800 4950
Wire Wire Line
	5600 4950 5600 4650
Wire Wire Line
	5600 4950 5700 4950
Wire Wire Line
	5700 4950 5700 4650
Connection ~ 5600 4950
Wire Wire Line
	3300 1850 3300 1650
Wire Wire Line
	3300 1650 3150 1650
Connection ~ 3300 1850
Text Notes 8050 2650 0    50   ~ 0
Vcc
Text Notes 8050 2750 0    50   ~ 0
GND
Text Notes 8050 2850 0    50   ~ 0
SCK
Text Notes 8050 2950 0    50   ~ 0
SDA
Text Notes 2650 1900 0    50   ~ 0
GND
Text Notes 2650 1800 0    50   ~ 0
Vin
Text Notes 2650 1700 0    50   ~ 0
GND
Text Notes 8050 3500 0    50   ~ 0
GND
Text Notes 8050 3400 0    50   ~ 0
PPM in
Wire Wire Line
	7300 2700 7750 2700
Wire Wire Line
	7750 3450 7300 3450
Wire Wire Line
	5800 2650 5800 1750
Wire Wire Line
	7300 1750 7300 2600
Wire Wire Line
	7300 2600 7750 2600
Wire Wire Line
	7100 4050 7100 2900
Wire Wire Line
	7100 2900 7750 2900
Wire Wire Line
	7150 4150 7150 2800
Wire Wire Line
	7150 2800 7750 2800
Connection ~ 7300 3450
Wire Wire Line
	7300 3450 7300 2700
Wire Wire Line
	7300 3450 7300 4950
Wire Wire Line
	7300 4950 7500 4950
Wire Wire Line
	7500 4950 7500 4850
Connection ~ 7300 4950
Wire Wire Line
	7500 4950 7900 4950
Wire Wire Line
	7900 4950 7900 4850
Connection ~ 7500 4950
Wire Wire Line
	7900 4550 7900 4500
Wire Wire Line
	7500 4550 7500 4400
Wire Wire Line
	7500 4050 7500 3950
Wire Wire Line
	7500 3350 7750 3350
Wire Wire Line
	7500 3950 7900 3950
Wire Wire Line
	7900 3950 7900 4050
Connection ~ 7500 3950
Wire Wire Line
	7500 3950 7500 3350
Wire Wire Line
	6950 3950 6950 4400
Wire Wire Line
	6950 4400 7500 4400
Connection ~ 7500 4400
Wire Wire Line
	7500 4400 7500 4350
Wire Wire Line
	7900 4500 6950 4500
Connection ~ 7900 4500
Wire Wire Line
	7900 4500 7900 4350
Wire Wire Line
	6750 2150 6750 1750
Connection ~ 6750 1750
Wire Wire Line
	6750 1750 7300 1750
Wire Wire Line
	6750 2450 6750 2750
Connection ~ 6750 4950
Wire Wire Line
	6750 4950 7300 4950
Wire Wire Line
	5700 4950 6750 4950
Connection ~ 5700 4950
Wire Wire Line
	6100 4150 7150 4150
Wire Wire Line
	6100 4050 7100 4050
Wire Wire Line
	6100 3950 6950 3950
Wire Wire Line
	5800 1750 6750 1750
Wire Wire Line
	6350 2000 5500 2000
Connection ~ 5500 2000
Wire Wire Line
	5500 2000 5500 2650
Wire Wire Line
	6350 2750 6750 2750
Connection ~ 6750 2750
Wire Wire Line
	6750 2750 6750 4950
Wire Wire Line
	6350 2300 6350 2350
Wire Wire Line
	6350 2350 6200 2350
Wire Wire Line
	6200 2350 6200 3650
Wire Wire Line
	6200 3650 6100 3650
Connection ~ 6350 2350
Wire Wire Line
	6350 2350 6350 2450
NoConn ~ 6100 3050
NoConn ~ 6100 3150
NoConn ~ 6100 3450
NoConn ~ 6100 3750
NoConn ~ 6100 4250
NoConn ~ 6100 4350
NoConn ~ 5700 2650
Wire Wire Line
	4350 3750 4600 3750
Wire Wire Line
	4600 3750 4600 3650
Wire Wire Line
	4600 3650 5100 3650
Wire Wire Line
	4700 3250 4700 3550
Wire Wire Line
	4700 3550 5100 3550
Wire Wire Line
	4350 4250 4700 4250
Wire Wire Line
	4700 4250 4700 3750
Wire Wire Line
	4700 3750 5100 3750
Wire Wire Line
	3950 3250 3800 3250
Wire Wire Line
	3800 3250 3800 3750
Connection ~ 3800 4950
Wire Wire Line
	3800 4950 5600 4950
Wire Wire Line
	3950 3750 3800 3750
Connection ~ 3800 3750
Wire Wire Line
	3800 3750 3800 4250
Wire Wire Line
	3950 4250 3800 4250
Connection ~ 3800 4250
Wire Wire Line
	3800 4250 3800 4950
NoConn ~ 5100 3050
NoConn ~ 5100 3150
NoConn ~ 5100 3250
NoConn ~ 5100 3350
NoConn ~ 5100 3450
NoConn ~ 5100 4350
NoConn ~ 5100 4250
NoConn ~ 5100 4150
NoConn ~ 5100 4050
NoConn ~ 5100 3950
Wire Wire Line
	4350 1500 4350 1750
Wire Wire Line
	4450 1500 4450 1750
Wire Wire Line
	4450 1750 5500 1750
Wire Wire Line
	4350 3250 4700 3250
NoConn ~ 6100 3850
$Comp
L MCU_Module:Arduino_Nano_v3.x A1
U 1 1 63D62F77
P 5600 3650
F 0 "A1" H 5450 2700 50  0000 C CNN
F 1 "Arduino_Nano_v3.x" H 6100 2700 50  0000 C CNN
F 2 "Module:Arduino_Nano" H 5600 3650 50  0001 C CIN
F 3 "http://www.mouser.com/pdfdocs/Gravitech_Arduino_Nano3_0.pdf" H 5600 3650 50  0001 C CNN
	1    5600 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 4500 6950 4750
Wire Wire Line
	6950 4750 4800 4750
Wire Wire Line
	4800 4750 4800 3850
Wire Wire Line
	4800 3850 5100 3850
$EndSCHEMATC
