EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
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
L Regulator_Linear:LM7805_TO220 U1
U 1 1 6013BFC2
P 2350 1300
F 0 "U1" V 2304 1404 50  0000 L CNN
F 1 "LM7805_TO220" V 2395 1404 50  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-220-3_Vertical" H 2350 1525 50  0001 C CIN
F 3 "https://www.onsemi.cn/PowerSolutions/document/MC7800-D.PDF" H 2350 1250 50  0001 C CNN
	1    2350 1300
	1    0    0    -1  
$EndComp
$Comp
L power:+9V #PWR01
U 1 1 6013AF2D
P 1500 1300
F 0 "#PWR01" H 1500 1150 50  0001 C CNN
F 1 "+9V" V 1515 1428 50  0000 L CNN
F 2 "" H 1500 1300 50  0001 C CNN
F 3 "" H 1500 1300 50  0001 C CNN
	1    1500 1300
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 6013B7FB
P 1500 1600
F 0 "#PWR02" H 1500 1350 50  0001 C CNN
F 1 "GND" V 1505 1472 50  0000 R CNN
F 2 "" H 1500 1600 50  0001 C CNN
F 3 "" H 1500 1600 50  0001 C CNN
	1    1500 1600
	0    1    1    0   
$EndComp
$Comp
L Device:CP1 C1
U 1 1 60144756
P 1800 1450
F 0 "C1" H 1915 1496 50  0000 L CNN
F 1 "CP1" H 1915 1405 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.00mm" H 1800 1450 50  0001 C CNN
F 3 "~" H 1800 1450 50  0001 C CNN
	1    1800 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:CP1 C3
U 1 1 60145EEE
P 3000 1450
F 0 "C3" H 3115 1496 50  0000 L CNN
F 1 "CP1" H 3115 1405 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.00mm" H 3000 1450 50  0001 C CNN
F 3 "~" H 3000 1450 50  0001 C CNN
	1    3000 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 1300 1600 1300
Wire Wire Line
	1800 1300 2050 1300
Connection ~ 1800 1300
Wire Wire Line
	1500 1600 1650 1600
Wire Wire Line
	1800 1600 2350 1600
Connection ~ 1800 1600
Wire Wire Line
	2650 1300 3000 1300
Connection ~ 2350 1600
$Comp
L MCU_Microchip_ATmega:ATmega328P-PU U2
U 1 1 601486A6
P 4050 3950
F 0 "U2" H 3406 3996 50  0000 R CNN
F 1 "ATmega328P-PU" H 3406 3905 50  0000 R CNN
F 2 "Package_DIP:DIP-28_W7.62mm_Socket" H 4050 3950 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 4050 3950 50  0001 C CNN
	1    4050 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 5450 4050 5450
Connection ~ 3000 1600
Wire Wire Line
	3000 1300 3250 1300
Wire Wire Line
	3450 1300 3450 2750
Connection ~ 3000 1300
Wire Wire Line
	3450 1300 4050 1300
Wire Wire Line
	4050 1300 4050 2450
Connection ~ 3450 1300
Wire Wire Line
	4050 1300 4150 1300
Connection ~ 4050 1300
Wire Wire Line
	4150 1300 4150 2450
Connection ~ 4150 1300
Connection ~ 4050 5450
Wire Wire Line
	4750 4450 4650 4450
Wire Wire Line
	4750 4350 4750 4450
Wire Wire Line
	4750 4550 4650 4550
Wire Wire Line
	4750 4750 4750 4550
Wire Wire Line
	4850 2800 4850 4250
Connection ~ 4850 4250
Wire Wire Line
	4850 4550 4850 4250
Wire Wire Line
	4650 4250 4850 4250
Connection ~ 5250 5450
Wire Wire Line
	5250 5450 4050 5450
Wire Wire Line
	6050 5450 5250 5450
Connection ~ 6050 5450
$Comp
L Device:Crystal Y1
U 1 1 6014DF9B
P 5500 3400
F 0 "Y1" V 5454 3531 50  0000 L CNN
F 1 "Crystal" V 5545 3531 50  0000 L CNN
F 2 "SamacSys_Parts:HC_49US__AT49_PTH" H 5500 3400 50  0001 C CNN
F 3 "~" H 5500 3400 50  0001 C CNN
	1    5500 3400
	0    1    1    0   
$EndComp
Wire Wire Line
	4650 3350 5350 3350
Wire Wire Line
	5350 3350 5500 3250
Wire Wire Line
	4650 3450 5350 3450
Wire Wire Line
	5350 3450 5500 3550
$Comp
L Device:C_Small C4
U 1 1 60150E8B
P 5950 3200
F 0 "C4" V 5721 3200 50  0000 C CNN
F 1 "C_Small" V 5812 3200 50  0000 C CNN
F 2 "SamacSys_Parts:S220K25SL0N63L6R" H 5950 3200 50  0001 C CNN
F 3 "~" H 5950 3200 50  0001 C CNN
	1    5950 3200
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C5
U 1 1 60151642
P 5950 3800
F 0 "C5" V 5721 3800 50  0000 C CNN
F 1 "C_Small" V 5812 3800 50  0000 C CNN
F 2 "SamacSys_Parts:S220K25SL0N63L6R" H 5950 3800 50  0001 C CNN
F 3 "~" H 5950 3800 50  0001 C CNN
	1    5950 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	5500 3250 5500 3200
Wire Wire Line
	5500 3200 5850 3200
Connection ~ 5500 3250
Wire Wire Line
	5500 3550 5500 3800
Wire Wire Line
	5500 3800 5850 3800
Connection ~ 5500 3550
Wire Wire Line
	6050 3200 6050 3800
Wire Wire Line
	6050 3800 6050 5450
Connection ~ 6050 3800
$Comp
L Device:R R8
U 1 1 60152F6A
P 5000 4250
F 0 "R8" V 4793 4250 50  0000 C CNN
F 1 "R" V 4884 4250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4930 4250 50  0001 C CNN
F 3 "~" H 5000 4250 50  0001 C CNN
	1    5000 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	5150 4250 5150 1300
$Comp
L Switch:SW_MEC_5E SW9
U 1 1 60155D78
P 5450 4650
F 0 "SW9" H 5450 5035 50  0000 C CNN
F 1 "SW_MEC_5E" H 5450 4944 50  0000 C CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 5450 4950 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 5450 4950 50  0001 C CNN
	1    5450 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 4550 4850 4550
Wire Wire Line
	5250 4650 5250 5450
$Comp
L LED:NeoPixel_THT D6
U 1 1 601711EF
P 2300 2300
F 0 "D6" H 2644 2346 50  0000 L CNN
F 1 "NeoPixel_THT" H 2644 2255 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 2350 2000 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 2400 1925 50  0001 L TNN
	1    2300 2300
	0    -1   1    0   
$EndComp
Wire Wire Line
	2300 2600 2300 2950
$Comp
L Device:CP1 C2
U 1 1 601A99EB
P 2300 1800
F 0 "C2" V 2048 1800 50  0000 C CNN
F 1 "CP1" V 2139 1800 50  0000 C CNN
F 2 "Capacitor_THT:CP_Radial_D10.0mm_P5.00mm" H 2300 1800 50  0001 C CNN
F 3 "~" H 2300 1800 50  0001 C CNN
	1    2300 1800
	0    -1   1    0   
$EndComp
Wire Wire Line
	2000 2300 2000 1800
Wire Wire Line
	2000 1800 2150 1800
Wire Wire Line
	2600 2300 2600 1800
Wire Wire Line
	2600 1800 2450 1800
Connection ~ 2600 2300
Connection ~ 2000 2300
Connection ~ 1700 4200
Wire Wire Line
	1700 4200 1700 3250
Connection ~ 1700 5100
Wire Wire Line
	1700 5100 1700 4200
Connection ~ 1700 6000
Wire Wire Line
	1700 6000 1700 5100
Connection ~ 1700 6950
Wire Wire Line
	1700 6950 1700 6000
Connection ~ 2000 6950
Wire Wire Line
	2000 6950 1700 6950
Connection ~ 2000 6000
Wire Wire Line
	2000 6000 2000 6950
Connection ~ 2000 5100
Wire Wire Line
	2000 5100 2000 6000
Connection ~ 2000 4200
Wire Wire Line
	2000 4200 2000 5100
Connection ~ 2000 3250
Wire Wire Line
	2000 3250 2000 4200
Wire Wire Line
	2000 2300 2000 3250
Connection ~ 2600 6000
Connection ~ 2600 5100
Wire Wire Line
	2600 5100 2600 6000
Connection ~ 1100 4200
Wire Wire Line
	1100 4200 1100 3250
Connection ~ 1100 5100
Wire Wire Line
	1100 5100 1100 4200
Connection ~ 1100 6000
Wire Wire Line
	1100 6000 1100 5100
Connection ~ 1100 6950
Wire Wire Line
	1100 6950 1100 6000
Connection ~ 2600 6950
Wire Wire Line
	1100 7450 1100 6950
Wire Wire Line
	2600 7450 1100 7450
Wire Wire Line
	2600 6950 2600 7450
Wire Wire Line
	2600 6000 2600 6950
Connection ~ 2600 4200
Wire Wire Line
	2600 4200 2600 5100
Connection ~ 2600 3250
Wire Wire Line
	2600 3250 2600 4200
Wire Wire Line
	2600 2300 2600 3250
Wire Wire Line
	1400 3900 1400 3550
Wire Wire Line
	1400 4800 1400 4500
Wire Wire Line
	1400 5700 1400 5400
Wire Wire Line
	1400 6650 1400 6300
Wire Wire Line
	2300 7250 1400 7250
Wire Wire Line
	2300 6300 2300 6650
Wire Wire Line
	2300 5400 2300 5700
Wire Wire Line
	2300 4500 2300 4800
Wire Wire Line
	2300 3550 2300 3900
$Comp
L LED:NeoPixel_THT D1
U 1 1 601867BB
P 1400 3250
F 0 "D1" V 1446 2906 50  0000 R CNN
F 1 "NeoPixel_THT" V 1355 2906 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 1450 2950 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 1500 2875 50  0001 L TNN
	1    1400 3250
	0    1    -1   0   
$EndComp
$Comp
L LED:NeoPixel_THT D2
U 1 1 601860EE
P 1400 4200
F 0 "D2" V 1446 3856 50  0000 R CNN
F 1 "NeoPixel_THT" V 1355 3856 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 1450 3900 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 1500 3825 50  0001 L TNN
	1    1400 4200
	0    1    -1   0   
$EndComp
$Comp
L LED:NeoPixel_THT D3
U 1 1 60185AF7
P 1400 5100
F 0 "D3" V 1446 4756 50  0000 R CNN
F 1 "NeoPixel_THT" V 1355 4756 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 1450 4800 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 1500 4725 50  0001 L TNN
	1    1400 5100
	0    1    -1   0   
$EndComp
$Comp
L LED:NeoPixel_THT D4
U 1 1 60185020
P 1400 6000
F 0 "D4" V 1446 5656 50  0000 R CNN
F 1 "NeoPixel_THT" V 1355 5656 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 1450 5700 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 1500 5625 50  0001 L TNN
	1    1400 6000
	0    1    -1   0   
$EndComp
$Comp
L LED:NeoPixel_THT D5
U 1 1 60184903
P 1400 6950
F 0 "D5" V 1446 6606 50  0000 R CNN
F 1 "NeoPixel_THT" V 1355 6606 50  0000 R CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 1450 6650 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 1500 6575 50  0001 L TNN
	1    1400 6950
	0    1    -1   0   
$EndComp
$Comp
L LED:NeoPixel_THT D11
U 1 1 60174D68
P 2300 6950
F 0 "D11" H 2644 6996 50  0000 L CNN
F 1 "NeoPixel_THT" H 2644 6905 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 2350 6650 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 2400 6575 50  0001 L TNN
	1    2300 6950
	0    -1   1    0   
$EndComp
$Comp
L LED:NeoPixel_THT D10
U 1 1 601748CC
P 2300 6000
F 0 "D10" H 2644 6046 50  0000 L CNN
F 1 "NeoPixel_THT" H 2644 5955 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 2350 5700 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 2400 5625 50  0001 L TNN
	1    2300 6000
	0    -1   1    0   
$EndComp
$Comp
L LED:NeoPixel_THT D9
U 1 1 601743AB
P 2300 5100
F 0 "D9" H 2644 5146 50  0000 L CNN
F 1 "NeoPixel_THT" H 2644 5055 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 2350 4800 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 2400 4725 50  0001 L TNN
	1    2300 5100
	0    -1   1    0   
$EndComp
$Comp
L LED:NeoPixel_THT D8
U 1 1 60173F7A
P 2300 4200
F 0 "D8" H 2644 4246 50  0000 L CNN
F 1 "NeoPixel_THT" H 2644 4155 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 2350 3900 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 2400 3825 50  0001 L TNN
	1    2300 4200
	0    -1   1    0   
$EndComp
$Comp
L LED:NeoPixel_THT D7
U 1 1 60173A68
P 2300 3250
F 0 "D7" H 2644 3296 50  0000 L CNN
F 1 "NeoPixel_THT" H 2644 3205 50  0000 L CNN
F 2 "LED_THT:LED_D5.0mm-4_RGB" H 2350 2950 50  0001 L TNN
F 3 "https://www.adafruit.com/product/1938" H 2400 2875 50  0001 L TNN
	1    2300 3250
	0    -1   1    0   
$EndComp
$Comp
L Device:R R5
U 1 1 6024FB4C
P 4950 5150
F 0 "R5" V 4743 5150 50  0000 C CNN
F 1 "R" V 4834 5150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4880 5150 50  0001 C CNN
F 3 "~" H 4950 5150 50  0001 C CNN
	1    4950 5150
	0    1    1    0   
$EndComp
Wire Wire Line
	4650 5150 4800 5150
Wire Wire Line
	5100 5150 5100 5650
Wire Wire Line
	2800 5650 2800 2000
Wire Wire Line
	2800 2000 2300 2000
Wire Wire Line
	2600 1800 2600 1600
Wire Wire Line
	2350 1600 2600 1600
Connection ~ 2600 1800
Connection ~ 2600 1600
Wire Wire Line
	2600 1600 3000 1600
Wire Wire Line
	2000 1800 1200 1800
Wire Wire Line
	1200 1800 1200 1100
Wire Wire Line
	1200 1100 3250 1100
Wire Wire Line
	3250 1100 3250 1300
Connection ~ 2000 1800
Connection ~ 3250 1300
Wire Wire Line
	3250 1300 3450 1300
Wire Wire Line
	3000 1600 3000 5450
Wire Wire Line
	5100 5650 2800 5650
Wire Wire Line
	2750 5700 2750 7100
Wire Wire Line
	6650 5850 6650 5450
Wire Wire Line
	6300 1300 5150 1300
Connection ~ 5150 1300
$Comp
L 74xx:74HC165 U4
U 1 1 60329ADD
P 8100 5150
F 0 "U4" H 8100 6231 50  0000 C CNN
F 1 "74HC165" H 8100 6140 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 8100 5150 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74HC_HCT165.pdf" H 8100 5150 50  0001 C CNN
	1    8100 5150
	1    0    0    1   
$EndComp
Wire Wire Line
	6500 4750 4750 4750
Wire Wire Line
	6400 2800 4850 2800
Wire Wire Line
	6400 4350 4750 4350
Wire Wire Line
	6400 3000 6400 4350
Wire Wire Line
	6600 3000 6400 3000
Wire Wire Line
	6500 2900 6500 4750
Wire Wire Line
	6600 2900 6500 2900
$Comp
L Device:C_Small C6
U 1 1 6015BA65
P 6500 2800
F 0 "C6" V 6271 2800 50  0000 C CNN
F 1 "C_Small" V 6362 2800 50  0000 C CNN
F 2 "SamacSys_Parts:K104K15X7RF53H5" H 6500 2800 50  0001 C CNN
F 3 "~" H 6500 2800 50  0001 C CNN
	1    6500 2800
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J2
U 1 1 6015A1A1
P 6800 3000
F 0 "J2" H 6772 2974 50  0000 R CNN
F 1 "Conn_01x06_Male" H 6772 2883 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 6800 3000 50  0001 C CNN
F 3 "~" H 6800 3000 50  0001 C CNN
	1    6800 3000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6600 3300 6600 5450
Wire Wire Line
	6050 5450 6600 5450
Connection ~ 6600 5450
Wire Wire Line
	6600 5450 6650 5450
Wire Wire Line
	8100 4150 8550 4150
Wire Wire Line
	8550 4150 8550 1600
Wire Wire Line
	8550 1600 8100 1600
Connection ~ 8100 1600
Wire Wire Line
	8700 3850 8700 1300
Wire Wire Line
	8700 1300 6350 1300
Connection ~ 6300 1300
Wire Wire Line
	8100 6050 8700 6050
Wire Wire Line
	8700 6050 8700 3850
Connection ~ 8700 3850
Wire Wire Line
	7500 2350 7500 4550
Wire Wire Line
	7500 4550 7600 4550
Wire Wire Line
	7400 2550 7400 4750
Wire Wire Line
	7400 4750 7600 4750
Wire Wire Line
	7550 2250 7550 4450
Wire Wire Line
	7550 4450 7600 4450
Wire Wire Line
	7600 4100 8650 4100
Wire Wire Line
	8650 4100 8650 5750
Wire Wire Line
	8650 5750 8600 5750
Wire Wire Line
	7300 2750 7300 1600
Connection ~ 7300 1600
Wire Wire Line
	7300 1600 8100 1600
Wire Wire Line
	6950 7750 6950 3350
Wire Wire Line
	6900 3450 6900 7700
Wire Wire Line
	7600 5650 6450 5650
Wire Wire Line
	6450 5650 6450 6050
Wire Wire Line
	6450 6050 6250 6050
Wire Wire Line
	7600 5550 6400 5550
Wire Wire Line
	6400 5550 6400 6000
Wire Wire Line
	6400 6000 5500 6000
Wire Wire Line
	7600 5450 7250 5450
Wire Wire Line
	7250 5450 7250 5500
Wire Wire Line
	7250 5500 4700 5500
Wire Wire Line
	5350 5350 5350 5550
Wire Wire Line
	5350 5550 3850 5550
Wire Wire Line
	5350 5350 7600 5350
Wire Wire Line
	7600 5250 4650 5250
Wire Wire Line
	4650 5250 4650 5500
Wire Wire Line
	4650 5500 3000 5500
Wire Wire Line
	7600 5150 7600 5250
Wire Wire Line
	7600 6450 6850 6450
Wire Wire Line
	6850 6450 6850 7250
Connection ~ 7600 5250
Wire Wire Line
	7600 5250 7600 5350
Connection ~ 7600 5350
Wire Wire Line
	7600 5350 7600 5450
Connection ~ 7600 5450
Wire Wire Line
	7600 5450 7600 5550
Connection ~ 7600 5550
Wire Wire Line
	7600 5550 7600 5650
Connection ~ 7600 5650
Wire Wire Line
	7600 5650 7600 5750
Connection ~ 7600 5750
Wire Wire Line
	7600 5750 7600 6450
Wire Wire Line
	7600 5050 7500 5050
Wire Wire Line
	7500 5050 7500 6350
Wire Wire Line
	7600 4950 7400 4950
Wire Wire Line
	7400 4950 7400 6300
Wire Wire Line
	4650 2950 5000 2950
Wire Wire Line
	5000 2950 5000 2550
Wire Wire Line
	5000 2550 7400 2550
Connection ~ 7400 2550
Wire Wire Line
	4650 3250 4750 3250
Wire Wire Line
	4750 3250 4750 2350
Wire Wire Line
	4750 2350 7500 2350
Connection ~ 7500 2350
Connection ~ 6150 5700
Wire Wire Line
	6150 5700 6300 5700
Connection ~ 6650 5850
Connection ~ 2900 5700
Wire Wire Line
	2900 5700 2750 5700
Connection ~ 3750 5700
Connection ~ 4600 5700
Wire Wire Line
	3750 5700 2900 5700
Wire Wire Line
	4600 5700 3750 5700
Connection ~ 5400 5700
Wire Wire Line
	5400 5700 4600 5700
Wire Wire Line
	6150 5700 5400 5700
Connection ~ 5900 5850
Connection ~ 5100 5850
Wire Wire Line
	5900 5850 6650 5850
Wire Wire Line
	5100 5850 5900 5850
Connection ~ 4250 5850
Wire Wire Line
	4250 5850 5100 5850
Wire Wire Line
	3400 5850 4250 5850
Connection ~ 4700 7100
Wire Wire Line
	4700 7550 4700 7100
Wire Wire Line
	6700 7550 4700 7550
Wire Wire Line
	6700 6300 6700 7550
Wire Wire Line
	7400 6300 6700 6300
Connection ~ 5500 7100
Wire Wire Line
	5500 7300 5500 7100
Wire Wire Line
	6750 7300 5500 7300
Wire Wire Line
	6750 6350 6750 7300
Wire Wire Line
	7500 6350 6750 6350
Connection ~ 6250 7100
Wire Wire Line
	6250 7250 6250 7100
Wire Wire Line
	6850 7250 6250 7250
Connection ~ 3000 6150
Wire Wire Line
	3000 5500 3000 6150
Connection ~ 3850 6150
Wire Wire Line
	3850 5550 3850 6150
Connection ~ 4700 6150
Wire Wire Line
	4700 5500 4700 6150
Connection ~ 5500 6150
Wire Wire Line
	5500 6000 5500 6150
Connection ~ 6250 6150
Wire Wire Line
	6250 6050 6250 6150
Connection ~ 3850 7100
Wire Wire Line
	3850 7700 3850 7100
Wire Wire Line
	6900 7700 3850 7700
Connection ~ 3000 7100
Wire Wire Line
	3000 7750 6950 7750
Wire Wire Line
	3000 7100 3000 7750
Connection ~ 5400 7500
Wire Wire Line
	6150 7500 6150 7100
Wire Wire Line
	5400 7500 6150 7500
Connection ~ 4600 7500
Wire Wire Line
	5400 7500 5400 7100
Wire Wire Line
	4600 7500 5400 7500
Connection ~ 3750 7500
Wire Wire Line
	4600 7500 4600 7100
Wire Wire Line
	3750 7500 4600 7500
Connection ~ 2900 7100
Wire Wire Line
	3750 7500 3750 7100
Wire Wire Line
	2900 7500 3750 7500
Wire Wire Line
	2900 7100 2900 7500
Wire Wire Line
	2750 7100 2900 7100
Wire Wire Line
	2900 5700 2900 6150
Wire Wire Line
	3750 5700 3750 6150
Wire Wire Line
	4600 5700 4600 6150
Wire Wire Line
	5400 5700 5400 6150
Wire Wire Line
	6150 6150 6150 5700
Connection ~ 4250 7400
Wire Wire Line
	3400 7400 3400 7100
Wire Wire Line
	4250 7400 3400 7400
Connection ~ 5100 7400
Wire Wire Line
	4250 7400 4250 7100
Wire Wire Line
	5100 7400 4250 7400
Connection ~ 5900 7400
Wire Wire Line
	5100 7400 5100 7100
Wire Wire Line
	5900 7400 5100 7400
Connection ~ 6650 7100
Connection ~ 6650 6150
Wire Wire Line
	5900 7400 5900 7100
Wire Wire Line
	6650 7400 5900 7400
Wire Wire Line
	6650 7100 6650 7400
Wire Wire Line
	6650 6150 6650 7100
Wire Wire Line
	6650 5850 6650 6150
Wire Wire Line
	5900 5850 5900 6150
Wire Wire Line
	5100 5850 5100 6150
Wire Wire Line
	4250 5850 4250 6150
Wire Wire Line
	3400 6150 3400 5850
Wire Wire Line
	5500 7100 5600 7100
$Comp
L Device:R R10
U 1 1 602B45BF
P 5750 7100
F 0 "R10" V 5543 7100 50  0000 C CNN
F 1 "R" V 5634 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 5680 7100 50  0001 C CNN
F 3 "~" H 5750 7100 50  0001 C CNN
	1    5750 7100
	0    1    -1   0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW8
U 1 1 602B45B5
P 5400 6900
F 0 "SW8" V 5354 7148 50  0000 L CNN
F 1 "SW_MEC_5E" V 5445 7148 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 5400 7200 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 5400 7200 50  0001 C CNN
	1    5400 6900
	0    1    -1   0   
$EndComp
Wire Wire Line
	3850 7100 3950 7100
$Comp
L Device:R R4
U 1 1 602B45AA
P 4100 7100
F 0 "R4" V 3893 7100 50  0000 C CNN
F 1 "R" V 3984 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4030 7100 50  0001 C CNN
F 3 "~" H 4100 7100 50  0001 C CNN
	1    4100 7100
	0    1    -1   0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW4
U 1 1 602B45A0
P 3750 6900
F 0 "SW4" V 3704 7148 50  0000 L CNN
F 1 "SW_MEC_5E" V 3795 7148 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 3750 7200 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 3750 7200 50  0001 C CNN
	1    3750 6900
	0    1    -1   0   
$EndComp
Wire Wire Line
	4700 7100 4800 7100
$Comp
L Device:R R7
U 1 1 602B4595
P 4950 7100
F 0 "R7" V 4743 7100 50  0000 C CNN
F 1 "R" V 4834 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4880 7100 50  0001 C CNN
F 3 "~" H 4950 7100 50  0001 C CNN
	1    4950 7100
	0    1    -1   0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW6
U 1 1 602B458B
P 4600 6900
F 0 "SW6" V 4554 7148 50  0000 L CNN
F 1 "SW_MEC_5E" V 4645 7148 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 4600 7200 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 4600 7200 50  0001 C CNN
	1    4600 6900
	0    1    -1   0   
$EndComp
Wire Wire Line
	6250 7100 6350 7100
$Comp
L Device:R R12
U 1 1 602B4580
P 6500 7100
F 0 "R12" V 6293 7100 50  0000 C CNN
F 1 "R" V 6384 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 6430 7100 50  0001 C CNN
F 3 "~" H 6500 7100 50  0001 C CNN
	1    6500 7100
	0    1    -1   0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW11
U 1 1 602B4576
P 6150 6900
F 0 "SW11" V 6104 7148 50  0000 L CNN
F 1 "SW_MEC_5E" V 6195 7148 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 6150 7200 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 6150 7200 50  0001 C CNN
	1    6150 6900
	0    1    -1   0   
$EndComp
Wire Wire Line
	3000 7100 3100 7100
$Comp
L Device:R R2
U 1 1 602B456B
P 3250 7100
F 0 "R2" V 3043 7100 50  0000 C CNN
F 1 "R" V 3134 7100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3180 7100 50  0001 C CNN
F 3 "~" H 3250 7100 50  0001 C CNN
	1    3250 7100
	0    1    -1   0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW2
U 1 1 602B4561
P 2900 6900
F 0 "SW2" V 2854 7148 50  0000 L CNN
F 1 "SW_MEC_5E" V 2945 7148 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 2900 7200 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 2900 7200 50  0001 C CNN
	1    2900 6900
	0    1    -1   0   
$EndComp
Wire Wire Line
	5500 6150 5600 6150
$Comp
L Device:R R9
U 1 1 602AC47C
P 5750 6150
F 0 "R9" V 5543 6150 50  0000 C CNN
F 1 "R" V 5634 6150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 5680 6150 50  0001 C CNN
F 3 "~" H 5750 6150 50  0001 C CNN
	1    5750 6150
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW7
U 1 1 602AC472
P 5400 6350
F 0 "SW7" V 5354 6598 50  0000 L CNN
F 1 "SW_MEC_5E" V 5445 6598 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 5400 6650 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 5400 6650 50  0001 C CNN
	1    5400 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	3850 6150 3950 6150
$Comp
L Device:R R3
U 1 1 602A8CD9
P 4100 6150
F 0 "R3" V 3893 6150 50  0000 C CNN
F 1 "R" V 3984 6150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4030 6150 50  0001 C CNN
F 3 "~" H 4100 6150 50  0001 C CNN
	1    4100 6150
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW3
U 1 1 602A8CB5
P 3750 6350
F 0 "SW3" V 3704 6598 50  0000 L CNN
F 1 "SW_MEC_5E" V 3795 6598 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 3750 6650 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 3750 6650 50  0001 C CNN
	1    3750 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 6150 4800 6150
$Comp
L Device:R R6
U 1 1 602A4449
P 4950 6150
F 0 "R6" V 4743 6150 50  0000 C CNN
F 1 "R" V 4834 6150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4880 6150 50  0001 C CNN
F 3 "~" H 4950 6150 50  0001 C CNN
	1    4950 6150
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW5
U 1 1 602A443F
P 4600 6350
F 0 "SW5" V 4554 6598 50  0000 L CNN
F 1 "SW_MEC_5E" V 4645 6598 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 4600 6650 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 4600 6650 50  0001 C CNN
	1    4600 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	6250 6150 6350 6150
$Comp
L Device:R R11
U 1 1 6029F74E
P 6500 6150
F 0 "R11" V 6293 6150 50  0000 C CNN
F 1 "R" V 6384 6150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 6430 6150 50  0001 C CNN
F 3 "~" H 6500 6150 50  0001 C CNN
	1    6500 6150
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW10
U 1 1 6029F72A
P 6150 6350
F 0 "SW10" V 6104 6598 50  0000 L CNN
F 1 "SW_MEC_5E" V 6195 6598 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 6150 6650 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 6150 6650 50  0001 C CNN
	1    6150 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 6150 3100 6150
$Comp
L Device:R R1
U 1 1 60296E84
P 3250 6150
F 0 "R1" V 3043 6150 50  0000 C CNN
F 1 "R" V 3134 6150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 3180 6150 50  0001 C CNN
F 3 "~" H 3250 6150 50  0001 C CNN
	1    3250 6150
	0    1    1    0   
$EndComp
$Comp
L Switch:SW_MEC_5E SW1
U 1 1 60293AE8
P 2900 6350
F 0 "SW1" V 2854 6598 50  0000 L CNN
F 1 "SW_MEC_5E" V 2945 6598 50  0000 L CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 2900 6650 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 2900 6650 50  0001 C CNN
	1    2900 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	7550 2250 5400 2250
Wire Wire Line
	5400 2250 5400 3050
Wire Wire Line
	5400 3050 4650 3050
Connection ~ 7550 2250
Wire Wire Line
	8650 3550 8650 1950
Wire Wire Line
	8600 3550 8650 3550
Wire Wire Line
	8100 1600 8100 1950
Wire Wire Line
	7600 2750 7300 2750
Wire Wire Line
	7600 2350 7500 2350
Wire Wire Line
	8100 3850 8700 3850
Wire Wire Line
	7600 2250 7550 2250
Wire Wire Line
	6950 3350 7600 3350
Wire Wire Line
	7600 3450 6900 3450
Wire Wire Line
	7600 3550 7600 4100
Wire Wire Line
	7600 2550 7400 2550
Wire Wire Line
	5450 3150 4650 3150
Wire Wire Line
	5450 1950 5450 3150
$Comp
L SamacSys_Parts:HDSP-A22C DS1
U 1 1 604FC12D
P 12750 2700
F 0 "DS1" H 14050 2965 50  0000 C CNN
F 1 "HDSP-A22C" H 14050 2874 50  0000 C CNN
F 2 "SamacSys_Parts:DIPS1524W51P254L2520H725Q18N" H 15200 2800 50  0001 L CNN
F 3 "https://docs.broadcom.com/docs/HDSP-A2xC-DS" H 15200 2700 50  0001 L CNN
F 4 "BROADCOM - HDSP-A22C - Alphanumeric LED Display, Red, 20 mA, 2, 13.7 mm, 1.9 V" H 15200 2600 50  0001 L CNN "Description"
F 5 "7.25" H 15200 2500 50  0001 L CNN "Height"
F 6 "Avago Technologies" H 15200 2400 50  0001 L CNN "Manufacturer_Name"
F 7 "HDSP-A22C" H 15200 2300 50  0001 L CNN "Manufacturer_Part_Number"
F 8 "630-HDSP-A22C" H 15200 2200 50  0001 L CNN "Mouser Part Number"
F 9 "https://www.mouser.co.uk/ProductDetail/Broadcom-Avago/HDSP-A22C/?qs=mhB89zxvawkE7eTk4oWrWQ%3D%3D" H 15200 2100 50  0001 L CNN "Mouser Price/Stock"
F 10 "" H 15200 2000 50  0001 L CNN "Arrow Part Number"
F 11 "" H 15200 1900 50  0001 L CNN "Arrow Price/Stock"
	1    12750 2700
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC595 U5
U 1 1 60546D19
P 9850 1800
F 0 "U5" H 9850 2581 50  0000 C CNN
F 1 "74HC595" H 9850 2490 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 9850 1800 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 9850 1800 50  0001 C CNN
	1    9850 1800
	1    0    0    -1  
$EndComp
$Comp
L 74xx:74HC595 U6
U 1 1 6057D370
P 9850 3450
F 0 "U6" H 9850 4231 50  0000 C CNN
F 1 "74HC595" H 9850 4140 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 9850 3450 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn74hc595.pdf" H 9850 3450 50  0001 C CNN
	1    9850 3450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R13
U 1 1 605CB36A
P 11750 1000
F 0 "R13" V 11543 1000 50  0000 C CNN
F 1 "R" V 11634 1000 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 1000 50  0001 C CNN
F 3 "~" H 11750 1000 50  0001 C CNN
	1    11750 1000
	0    1    1    0   
$EndComp
$Comp
L Device:R R14
U 1 1 605F9B91
P 11750 1300
F 0 "R14" V 11543 1300 50  0000 C CNN
F 1 "R" V 11634 1300 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 1300 50  0001 C CNN
F 3 "~" H 11750 1300 50  0001 C CNN
	1    11750 1300
	0    1    1    0   
$EndComp
$Comp
L Device:R R15
U 1 1 605F9EA9
P 11750 1600
F 0 "R15" V 11543 1600 50  0000 C CNN
F 1 "R" V 11634 1600 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 1600 50  0001 C CNN
F 3 "~" H 11750 1600 50  0001 C CNN
	1    11750 1600
	0    1    1    0   
$EndComp
$Comp
L Device:R R16
U 1 1 605FA187
P 11750 1900
F 0 "R16" V 11543 1900 50  0000 C CNN
F 1 "R" V 11634 1900 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 1900 50  0001 C CNN
F 3 "~" H 11750 1900 50  0001 C CNN
	1    11750 1900
	0    1    1    0   
$EndComp
$Comp
L Device:R R17
U 1 1 605FA463
P 11750 2250
F 0 "R17" V 11543 2250 50  0000 C CNN
F 1 "R" V 11634 2250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 2250 50  0001 C CNN
F 3 "~" H 11750 2250 50  0001 C CNN
	1    11750 2250
	0    1    1    0   
$EndComp
$Comp
L Device:R R18
U 1 1 605FA479
P 11750 2550
F 0 "R18" V 11543 2550 50  0000 C CNN
F 1 "R" V 11634 2550 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 2550 50  0001 C CNN
F 3 "~" H 11750 2550 50  0001 C CNN
	1    11750 2550
	0    1    1    0   
$EndComp
$Comp
L Device:R R19
U 1 1 605FA483
P 11750 2850
F 0 "R19" V 11543 2850 50  0000 C CNN
F 1 "R" V 11634 2850 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 2850 50  0001 C CNN
F 3 "~" H 11750 2850 50  0001 C CNN
	1    11750 2850
	0    1    1    0   
$EndComp
$Comp
L Device:R R20
U 1 1 605FA48D
P 11750 3150
F 0 "R20" V 11543 3150 50  0000 C CNN
F 1 "R" V 11634 3150 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 3150 50  0001 C CNN
F 3 "~" H 11750 3150 50  0001 C CNN
	1    11750 3150
	0    1    1    0   
$EndComp
$Comp
L Device:R R21
U 1 1 6060A7F7
P 11750 3500
F 0 "R21" V 11543 3500 50  0000 C CNN
F 1 "R" V 11634 3500 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 3500 50  0001 C CNN
F 3 "~" H 11750 3500 50  0001 C CNN
	1    11750 3500
	0    1    1    0   
$EndComp
$Comp
L Device:R R22
U 1 1 6060A801
P 11750 3800
F 0 "R22" V 11543 3800 50  0000 C CNN
F 1 "R" V 11634 3800 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 3800 50  0001 C CNN
F 3 "~" H 11750 3800 50  0001 C CNN
	1    11750 3800
	0    1    1    0   
$EndComp
$Comp
L Device:R R23
U 1 1 6060A80B
P 11750 4100
F 0 "R23" V 11543 4100 50  0000 C CNN
F 1 "R" V 11634 4100 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 4100 50  0001 C CNN
F 3 "~" H 11750 4100 50  0001 C CNN
	1    11750 4100
	0    1    1    0   
$EndComp
$Comp
L Device:R R24
U 1 1 6060A815
P 11750 4400
F 0 "R24" V 11543 4400 50  0000 C CNN
F 1 "R" V 11634 4400 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 4400 50  0001 C CNN
F 3 "~" H 11750 4400 50  0001 C CNN
	1    11750 4400
	0    1    1    0   
$EndComp
$Comp
L Device:R R25
U 1 1 6060A81F
P 11750 4750
F 0 "R25" V 11543 4750 50  0000 C CNN
F 1 "R" V 11634 4750 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 4750 50  0001 C CNN
F 3 "~" H 11750 4750 50  0001 C CNN
	1    11750 4750
	0    1    1    0   
$EndComp
$Comp
L Device:R R26
U 1 1 6060A829
P 11750 5050
F 0 "R26" V 11543 5050 50  0000 C CNN
F 1 "R" V 11634 5050 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 5050 50  0001 C CNN
F 3 "~" H 11750 5050 50  0001 C CNN
	1    11750 5050
	0    1    1    0   
$EndComp
$Comp
L Device:R R27
U 1 1 6060A833
P 11750 5350
F 0 "R27" V 11543 5350 50  0000 C CNN
F 1 "R" V 11634 5350 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P2.54mm_Vertical" V 11680 5350 50  0001 C CNN
F 3 "~" H 11750 5350 50  0001 C CNN
	1    11750 5350
	0    1    1    0   
$EndComp
Wire Wire Line
	15350 3500 15350 5350
Wire Wire Line
	15350 5350 11900 5350
Wire Wire Line
	15350 3300 15450 3300
Wire Wire Line
	15450 3300 15450 5050
Wire Wire Line
	15450 5050 11900 5050
Wire Wire Line
	15350 3200 15550 3200
Wire Wire Line
	15550 3200 15550 4750
Wire Wire Line
	15550 4750 11900 4750
Wire Wire Line
	15350 3100 15650 3100
Wire Wire Line
	15650 3100 15650 4400
Wire Wire Line
	15650 4400 11900 4400
Wire Wire Line
	15350 3000 15750 3000
Wire Wire Line
	15750 3000 15750 4100
Wire Wire Line
	15750 4100 11900 4100
Wire Wire Line
	15350 2800 15850 2800
Wire Wire Line
	15850 2800 15850 3800
Wire Wire Line
	15850 3800 11900 3800
Wire Wire Line
	15350 2700 15950 2700
Wire Wire Line
	15950 2700 15950 3700
Wire Wire Line
	15950 3700 11900 3700
Wire Wire Line
	11900 3700 11900 3500
Wire Wire Line
	12750 3500 12000 3500
Wire Wire Line
	12000 3500 12000 3150
Wire Wire Line
	12000 3150 11900 3150
Wire Wire Line
	12750 3400 12100 3400
Wire Wire Line
	12100 3400 12100 2850
Wire Wire Line
	12100 2850 11900 2850
Wire Wire Line
	12750 3300 12200 3300
Wire Wire Line
	12200 3300 12200 2550
Wire Wire Line
	12200 2550 11900 2550
Wire Wire Line
	12750 3200 12300 3200
Wire Wire Line
	12300 3200 12300 2250
Wire Wire Line
	12300 2250 11900 2250
Wire Wire Line
	12750 3100 12400 3100
Wire Wire Line
	12400 3100 12400 1900
Wire Wire Line
	12400 1900 11900 1900
Wire Wire Line
	12750 3000 12500 3000
Wire Wire Line
	12500 3000 12500 1600
Wire Wire Line
	12500 1600 11900 1600
Wire Wire Line
	12750 2800 12600 2800
Wire Wire Line
	12600 2800 12600 1300
Wire Wire Line
	12600 1300 11900 1300
Wire Wire Line
	12750 2700 12750 1000
Wire Wire Line
	12750 1000 11900 1000
Wire Wire Line
	10250 1500 10350 1500
Wire Wire Line
	10350 1500 10350 5350
Wire Wire Line
	10350 5350 11600 5350
Wire Wire Line
	10250 1400 10450 1400
Wire Wire Line
	10450 1400 10450 5050
Wire Wire Line
	10450 5050 11600 5050
Wire Wire Line
	10250 2000 10550 2000
Wire Wire Line
	10550 2000 10550 4750
Wire Wire Line
	10550 4750 11600 4750
Wire Wire Line
	10250 3250 10650 3250
Wire Wire Line
	10650 3250 10650 4400
Wire Wire Line
	10650 4400 11600 4400
Wire Wire Line
	10250 3150 10750 3150
Wire Wire Line
	10750 3150 10750 4100
Wire Wire Line
	10750 4100 11600 4100
Wire Wire Line
	10250 3050 10850 3050
Wire Wire Line
	10850 3050 10850 3800
Wire Wire Line
	10850 3800 11600 3800
Wire Wire Line
	10250 1900 10950 1900
Wire Wire Line
	10950 1900 10950 3500
Wire Wire Line
	10950 3500 11600 3500
Wire Wire Line
	10250 1600 11050 1600
Wire Wire Line
	11050 1600 11050 3150
Wire Wire Line
	11050 3150 11600 3150
Wire Wire Line
	10250 3650 11150 3650
Wire Wire Line
	11150 3650 11150 2850
Wire Wire Line
	11150 2850 11600 2850
Wire Wire Line
	10250 1700 11150 1700
Wire Wire Line
	11150 1700 11150 2550
Wire Wire Line
	11150 2550 11600 2550
Wire Wire Line
	10250 2100 11250 2100
Wire Wire Line
	11250 2100 11250 2250
Wire Wire Line
	11250 2250 11600 2250
Wire Wire Line
	10250 3550 11350 3550
Wire Wire Line
	11350 3550 11350 1900
Wire Wire Line
	11350 1900 11600 1900
Wire Wire Line
	10250 3450 11450 3450
Wire Wire Line
	11450 3450 11450 1600
Wire Wire Line
	11450 1600 11600 1600
Wire Wire Line
	10250 3350 11550 3350
Wire Wire Line
	11550 3350 11550 1300
Wire Wire Line
	11550 1300 11600 1300
Wire Wire Line
	10250 1800 11250 1800
Wire Wire Line
	11250 1800 11250 1000
Wire Wire Line
	11250 1000 11600 1000
Wire Wire Line
	9450 3050 9450 2700
Wire Wire Line
	9450 2700 10250 2700
Wire Wire Line
	10250 2700 10250 2300
Wire Wire Line
	9450 1600 9350 1600
Wire Wire Line
	9350 3250 9450 3250
Wire Wire Line
	9450 3550 9250 3550
Wire Wire Line
	9250 3550 9250 1900
Wire Wire Line
	9250 1900 9450 1900
Wire Wire Line
	4650 3650 4900 3650
Wire Wire Line
	4900 3650 4900 1850
Wire Wire Line
	4900 1850 9250 1850
Wire Wire Line
	9250 1850 9250 1900
Connection ~ 9250 1900
Wire Wire Line
	4650 3750 5200 3750
Wire Wire Line
	5200 3750 5200 1700
Wire Wire Line
	5200 1700 9350 1700
Wire Wire Line
	9350 1600 9350 1700
Connection ~ 9350 1700
Wire Wire Line
	9350 1700 9350 3250
Wire Wire Line
	4650 3850 5250 3850
Wire Wire Line
	5250 3850 5250 1400
Wire Wire Line
	5250 1400 9450 1400
Wire Wire Line
	4650 3950 8950 3950
Wire Wire Line
	8950 3950 8950 5550
Wire Wire Line
	8950 5550 15800 5550
Wire Wire Line
	15800 5550 15800 2900
Wire Wire Line
	15800 2900 15350 2900
Wire Wire Line
	4650 4050 8850 4050
Wire Wire Line
	8850 4050 8850 5950
Wire Wire Line
	8850 5950 15400 5950
Wire Wire Line
	15400 5950 15400 3400
Wire Wire Line
	15400 3400 15350 3400
$Comp
L Connector:Conn_01x04_Female J1
U 1 1 60A25252
P 5250 850
F 0 "J1" H 5278 826 50  0000 L CNN
F 1 "Conn_01x04_Female" H 5278 735 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 5250 850 50  0001 C CNN
F 3 "~" H 5250 850 50  0001 C CNN
	1    5250 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9850 2850 9150 2850
Wire Wire Line
	9150 2850 9150 1300
Wire Wire Line
	9150 1200 9850 1200
Wire Wire Line
	9850 4150 9050 4150
Wire Wire Line
	9050 4150 9050 2500
Wire Wire Line
	9050 2500 9850 2500
Wire Wire Line
	8700 1300 9150 1300
Connection ~ 8700 1300
Connection ~ 9150 1300
Wire Wire Line
	9150 1300 9150 1200
Wire Wire Line
	9050 2500 9050 1600
Wire Wire Line
	9050 1600 8550 1600
Connection ~ 9050 2500
Connection ~ 8550 1600
Connection ~ 7600 2750
Wire Wire Line
	7600 2850 7600 2750
Connection ~ 7600 2850
Wire Wire Line
	7600 2950 7600 2850
Connection ~ 7600 2950
Wire Wire Line
	7600 3050 7600 2950
Connection ~ 7600 3050
Wire Wire Line
	7600 3150 7600 3050
Wire Wire Line
	7600 3250 7600 3150
Connection ~ 7600 3150
$Comp
L 74xx:74HC165 U3
U 1 1 6035C222
P 8100 2950
F 0 "U3" H 8100 4031 50  0000 C CNN
F 1 "74HC165" H 8100 3940 50  0000 C CNN
F 2 "Package_DIP:DIP-16_W7.62mm" H 8100 2950 50  0001 C CNN
F 3 "https://assets.nexperia.com/documents/data-sheet/74HC_HCT165.pdf" H 8100 2950 50  0001 C CNN
	1    8100 2950
	1    0    0    1   
$EndComp
Wire Wire Line
	5450 1950 8650 1950
Wire Wire Line
	4650 2750 4650 850 
Wire Wire Line
	4650 850  5050 850 
Wire Wire Line
	4650 2850 4700 2850
Wire Wire Line
	4700 2850 4700 950 
Wire Wire Line
	4700 950  5050 950 
NoConn ~ 12750 2900
NoConn ~ 9450 1700
NoConn ~ 9450 2000
NoConn ~ 6250 6550
NoConn ~ 6150 6550
NoConn ~ 6150 6700
NoConn ~ 6250 6700
NoConn ~ 5500 6550
NoConn ~ 5400 6550
NoConn ~ 5400 6700
NoConn ~ 5500 6700
NoConn ~ 4700 6550
NoConn ~ 4600 6550
NoConn ~ 4600 6700
NoConn ~ 4700 6700
NoConn ~ 2900 6550
NoConn ~ 3000 6550
NoConn ~ 3000 6700
NoConn ~ 2900 6700
NoConn ~ 3750 6700
NoConn ~ 3850 6700
NoConn ~ 3850 6550
NoConn ~ 3750 6550
NoConn ~ 5650 4550
NoConn ~ 5650 4650
NoConn ~ 9450 3350
NoConn ~ 9450 3650
NoConn ~ 10250 3750
NoConn ~ 10250 3950
NoConn ~ 8600 3450
NoConn ~ 1400 2950
NoConn ~ 4650 4150
NoConn ~ 4650 4650
NoConn ~ 4650 4750
NoConn ~ 4650 4850
NoConn ~ 4650 4950
NoConn ~ 4650 5050
NoConn ~ 8600 5650
NoConn ~ 6600 3200
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 61287C82
P 1500 1300
F 0 "#FLG0101" H 1500 1375 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 1473 50  0000 C CNN
F 2 "" H 1500 1300 50  0001 C CNN
F 3 "~" H 1500 1300 50  0001 C CNN
	1    1500 1300
	1    0    0    -1  
$EndComp
Connection ~ 1500 1300
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 612AF933
P 1500 1600
F 0 "#FLG0102" H 1500 1675 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 1773 50  0000 C CNN
F 2 "" H 1500 1600 50  0001 C CNN
F 3 "~" H 1500 1600 50  0001 C CNN
	1    1500 1600
	1    0    0    -1  
$EndComp
Connection ~ 1500 1600
Wire Wire Line
	4150 1300 5150 1300
Wire Wire Line
	3000 1600 7300 1600
Wire Wire Line
	1600 1300 1600 750 
Wire Wire Line
	1600 750  5050 750 
Connection ~ 1600 1300
Wire Wire Line
	1600 1300 1800 1300
Wire Wire Line
	1650 1600 1650 1050
Wire Wire Line
	1650 1050 5050 1050
Connection ~ 1650 1600
Wire Wire Line
	1650 1600 1800 1600
Wire Wire Line
	6300 5700 6300 1300
Wire Wire Line
	6600 3100 6350 3100
Wire Wire Line
	6350 3100 6350 1300
Connection ~ 6350 1300
Wire Wire Line
	6350 1300 6300 1300
$EndSCHEMATC
