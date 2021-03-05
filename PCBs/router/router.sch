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
L Device:CP1 C2
U 1 1 60145EEE
P 3000 1450
F 0 "C2" H 3115 1496 50  0000 L CNN
F 1 "CP1" H 3115 1405 50  0000 L CNN
F 2 "Capacitor_THT:CP_Radial_D5.0mm_P2.00mm" H 3000 1450 50  0001 C CNN
F 3 "~" H 3000 1450 50  0001 C CNN
	1    3000 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	1500 1300 1600 1300
Wire Wire Line
	1800 1300 2000 1300
Connection ~ 1800 1300
Wire Wire Line
	1500 1600 1650 1600
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
	4850 2800 4850 4250
Connection ~ 4850 4250
Wire Wire Line
	4850 4550 4850 4250
Wire Wire Line
	4650 4250 4850 4250
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
L Device:C_Small C3
U 1 1 60150E8B
P 5950 3200
F 0 "C3" V 5721 3200 50  0000 C CNN
F 1 "C_Small" V 5812 3200 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm" H 5950 3200 50  0001 C CNN
F 3 "~" H 5950 3200 50  0001 C CNN
	1    5950 3200
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C4
U 1 1 60151642
P 5950 3800
F 0 "C4" V 5721 3800 50  0000 C CNN
F 1 "C_Small" V 5812 3800 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm" H 5950 3800 50  0001 C CNN
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
L Device:R R1
U 1 1 60152F6A
P 5000 4250
F 0 "R1" V 4793 4250 50  0000 C CNN
F 1 "R" V 4884 4250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 4930 4250 50  0001 C CNN
F 3 "~" H 5000 4250 50  0001 C CNN
	1    5000 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	5150 4250 5150 1300
$Comp
L Switch:SW_MEC_5E SW1
U 1 1 60155D78
P 5450 4650
F 0 "SW1" H 5450 5035 50  0000 C CNN
F 1 "SW_MEC_5E" H 5450 4944 50  0000 C CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 5450 4950 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 5450 4950 50  0001 C CNN
	1    5450 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5250 4550 4850 4550
Wire Wire Line
	3000 1600 3000 5450
Connection ~ 5150 1300
Wire Wire Line
	6500 4750 5100 4750
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
L Device:C_Small C5
U 1 1 6015BA65
P 6500 2800
F 0 "C5" V 6271 2800 50  0000 C CNN
F 1 "C_Small" V 6362 2800 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D4.3mm_W1.9mm_P5.00mm" H 6500 2800 50  0001 C CNN
F 3 "~" H 6500 2800 50  0001 C CNN
	1    6500 2800
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J10
U 1 1 6015A1A1
P 6800 3000
F 0 "J10" H 6772 2974 50  0000 R CNN
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
NoConn ~ 5650 4550
NoConn ~ 6600 3200
$Comp
L power:PWR_FLAG #FLG01
U 1 1 61287C82
P 1500 1300
F 0 "#FLG01" H 1500 1375 50  0001 C CNN
F 1 "PWR_FLAG" H 1500 1473 50  0000 C CNN
F 2 "" H 1500 1300 50  0001 C CNN
F 3 "~" H 1500 1300 50  0001 C CNN
	1    1500 1300
	1    0    0    -1  
$EndComp
Connection ~ 1500 1300
$Comp
L power:PWR_FLAG #FLG02
U 1 1 612AF933
P 1500 1600
F 0 "#FLG02" H 1500 1675 50  0001 C CNN
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
	6600 3100 6350 3100
Wire Wire Line
	6350 3100 6350 1300
Wire Wire Line
	2350 1600 3000 1600
Wire Wire Line
	5150 1300 6200 1300
Wire Wire Line
	3000 1300 3450 1300
$Comp
L MCU_Microchip_ATmega:ATmega328P-PU U3
U 1 1 602149F6
P 10000 3950
F 0 "U3" H 9356 3996 50  0000 R CNN
F 1 "ATmega328P-PU" H 9356 3905 50  0000 R CNN
F 2 "Package_DIP:DIP-28_W7.62mm_Socket" H 10000 3950 50  0001 C CIN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328_P%20AVR%20MCU%20with%20picoPower%20Technology%20Data%20Sheet%2040001984A.pdf" H 10000 3950 50  0001 C CNN
	1    10000 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	9400 1300 9400 2750
Wire Wire Line
	9400 1300 10000 1300
Wire Wire Line
	10000 1300 10000 2450
Wire Wire Line
	10000 1300 10100 1300
Connection ~ 10000 1300
Wire Wire Line
	10100 1300 10100 2450
Connection ~ 10100 1300
Wire Wire Line
	10700 4450 10600 4450
Wire Wire Line
	10700 4350 10700 4450
Wire Wire Line
	10700 4550 10600 4550
Wire Wire Line
	10700 4750 10700 4550
Wire Wire Line
	10800 2800 10800 4250
Connection ~ 10800 4250
Wire Wire Line
	10800 4550 10800 4250
Wire Wire Line
	10600 4250 10800 4250
Connection ~ 12000 5450
$Comp
L Device:Crystal Y2
U 1 1 60214A12
P 11450 3400
F 0 "Y2" V 11404 3531 50  0000 L CNN
F 1 "Crystal" V 11495 3531 50  0000 L CNN
F 2 "SamacSys_Parts:HC_49US__AT49_PTH" H 11450 3400 50  0001 C CNN
F 3 "~" H 11450 3400 50  0001 C CNN
	1    11450 3400
	0    1    1    0   
$EndComp
Wire Wire Line
	10600 3350 11300 3350
Wire Wire Line
	11300 3350 11450 3250
Wire Wire Line
	10600 3450 11300 3450
Wire Wire Line
	11300 3450 11450 3550
$Comp
L Device:C_Small C6
U 1 1 60214A1C
P 11900 3200
F 0 "C6" V 11671 3200 50  0000 C CNN
F 1 "C_Small" V 11762 3200 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm" H 11900 3200 50  0001 C CNN
F 3 "~" H 11900 3200 50  0001 C CNN
	1    11900 3200
	0    1    1    0   
$EndComp
$Comp
L Device:C_Small C7
U 1 1 60214A22
P 11900 3800
F 0 "C7" V 11671 3800 50  0000 C CNN
F 1 "C_Small" V 11762 3800 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D5.0mm_W2.5mm_P5.00mm" H 11900 3800 50  0001 C CNN
F 3 "~" H 11900 3800 50  0001 C CNN
	1    11900 3800
	0    1    1    0   
$EndComp
Wire Wire Line
	11450 3250 11450 3200
Wire Wire Line
	11450 3200 11800 3200
Connection ~ 11450 3250
Wire Wire Line
	11450 3550 11450 3800
Wire Wire Line
	11450 3800 11800 3800
Connection ~ 11450 3550
Wire Wire Line
	12000 3200 12000 3800
Wire Wire Line
	12000 3800 12000 5450
Connection ~ 12000 3800
$Comp
L Device:R R3
U 1 1 60214A31
P 10950 4250
F 0 "R3" V 10743 4250 50  0000 C CNN
F 1 "R" V 10834 4250 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 10880 4250 50  0001 C CNN
F 3 "~" H 10950 4250 50  0001 C CNN
	1    10950 4250
	0    1    1    0   
$EndComp
Wire Wire Line
	11100 4250 11100 1300
$Comp
L Switch:SW_MEC_5E SW2
U 1 1 60214A38
P 11400 4650
F 0 "SW2" H 11400 5035 50  0000 C CNN
F 1 "SW_MEC_5E" H 11400 4944 50  0000 C CNN
F 2 "Button_Switch_THT:SW_TH_Tactile_Omron_B3F-10xx" H 11400 4950 50  0001 C CNN
F 3 "http://www.apem.com/int/index.php?controller=attachment&id_attachment=1371" H 11400 4950 50  0001 C CNN
	1    11400 4650
	1    0    0    -1  
$EndComp
Wire Wire Line
	11200 4550 10800 4550
Connection ~ 11100 1300
Wire Wire Line
	12450 4750 10700 4750
Wire Wire Line
	12350 2800 10800 2800
Wire Wire Line
	12350 4350 10700 4350
Wire Wire Line
	12350 3000 12350 4350
Wire Wire Line
	12550 3000 12350 3000
Wire Wire Line
	12450 2900 12450 4750
Wire Wire Line
	12550 2900 12450 2900
$Comp
L Device:C_Small C8
U 1 1 60214A48
P 12450 2800
F 0 "C8" V 12221 2800 50  0000 C CNN
F 1 "C_Small" V 12312 2800 50  0000 C CNN
F 2 "Capacitor_THT:C_Disc_D4.3mm_W1.9mm_P5.00mm" H 12450 2800 50  0001 C CNN
F 3 "~" H 12450 2800 50  0001 C CNN
	1    12450 2800
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J15
U 1 1 60214A4E
P 12750 3000
F 0 "J15" H 12722 2974 50  0000 R CNN
F 1 "Conn_01x06_Male" H 12722 2883 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 12750 3000 50  0001 C CNN
F 3 "~" H 12750 3000 50  0001 C CNN
	1    12750 3000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	12550 3300 12550 5450
Wire Wire Line
	12000 5450 12550 5450
NoConn ~ 11600 4550
NoConn ~ 12550 3200
Wire Wire Line
	10100 1300 11100 1300
Wire Wire Line
	12550 3100 12300 3100
Wire Wire Line
	12300 3100 12300 1300
Wire Wire Line
	11100 1300 12300 1300
Wire Wire Line
	6350 1300 9400 1300
Connection ~ 6350 1300
Connection ~ 9400 1300
Wire Wire Line
	6600 5450 10000 5450
Connection ~ 6600 5450
Connection ~ 10000 5450
$Comp
L Device:LED D1
U 1 1 60221C04
P 6650 900
F 0 "D1" H 6643 645 50  0000 C CNN
F 1 "LED" H 6643 736 50  0000 C CNN
F 2 "LED_THT:LED_D3.0mm" H 6650 900 50  0001 C CNN
F 3 "~" H 6650 900 50  0001 C CNN
	1    6650 900 
	-1   0    0    1   
$EndComp
$Comp
L Device:R R2
U 1 1 6022B14D
P 6350 900
F 0 "R2" V 6143 900 50  0000 C CNN
F 1 "R" V 6234 900 50  0000 C CNN
F 2 "Resistor_THT:R_Axial_DIN0207_L6.3mm_D2.5mm_P7.62mm_Horizontal" V 6280 900 50  0001 C CNN
F 3 "~" H 6350 900 50  0001 C CNN
	1    6350 900 
	0    1    1    0   
$EndComp
Wire Wire Line
	6200 1300 6200 900 
Connection ~ 6200 1300
Wire Wire Line
	6200 1300 6350 1300
Wire Wire Line
	6800 900  6800 1150
Wire Wire Line
	6800 1150 3300 1150
Wire Wire Line
	3300 1150 3300 1600
Wire Wire Line
	3300 1600 3000 1600
$Comp
L Connector:Conn_01x04_Male J7
U 1 1 6023356C
P 5250 950
F 0 "J7" H 5222 832 50  0000 R CNN
F 1 "Conn_01x04_Male" H 5222 923 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 5250 950 50  0001 C CNN
F 3 "~" H 5250 950 50  0001 C CNN
	1    5250 950 
	-1   0    0    1   
$EndComp
Wire Wire Line
	5050 850  4950 850 
Wire Wire Line
	4950 850  4950 3750
Wire Wire Line
	4950 3750 4650 3750
Wire Wire Line
	5050 950  4750 950 
Wire Wire Line
	4750 950  4750 3650
Wire Wire Line
	4750 3650 4650 3650
Wire Wire Line
	10000 5450 11600 5450
Wire Wire Line
	4050 5450 5650 5450
Wire Wire Line
	5650 4650 5650 5450
Connection ~ 5650 5450
Wire Wire Line
	5650 5450 6050 5450
Wire Wire Line
	11600 4650 11600 5450
Connection ~ 11600 5450
Wire Wire Line
	11600 5450 12000 5450
NoConn ~ 11200 4650
NoConn ~ 5250 4650
Wire Wire Line
	10600 4150 11150 4150
Wire Wire Line
	11150 4150 11150 5550
Wire Wire Line
	11150 5550 6700 5550
Wire Wire Line
	6700 5550 6700 4150
Wire Wire Line
	6700 4150 4650 4150
Wire Wire Line
	4650 4050 6800 4050
Wire Wire Line
	6800 4050 6800 5650
Wire Wire Line
	6800 5650 11250 5650
Wire Wire Line
	11250 5650 11250 4050
Wire Wire Line
	11250 4050 10600 4050
$Comp
L Connector:Conn_01x06_Male J1
U 1 1 604A5208
P 2200 8000
F 0 "J1" V 2262 8244 50  0000 L CNN
F 1 "Conn_01x06_Male" V 2353 8244 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 2200 8000 50  0001 C CNN
F 3 "~" H 2200 8000 50  0001 C CNN
	1    2200 8000
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J8
U 1 1 604B6EBA
P 6100 8000
F 0 "J8" V 6162 8244 50  0000 L CNN
F 1 "Conn_01x06_Male" V 6253 8244 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 6100 8000 50  0001 C CNN
F 3 "~" H 6100 8000 50  0001 C CNN
	1    6100 8000
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J2
U 1 1 604C2AD5
P 2200 8350
F 0 "J2" V 2262 8594 50  0000 L CNN
F 1 "Conn_01x06_Male" V 2353 8594 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 2200 8350 50  0001 C CNN
F 3 "~" H 2200 8350 50  0001 C CNN
	1    2200 8350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2000 8150 2050 8150
Wire Wire Line
	2050 8150 2050 7800
Wire Wire Line
	2050 7800 2000 7800
Wire Wire Line
	2100 8150 2150 8150
Wire Wire Line
	2150 8150 2150 7800
Wire Wire Line
	2150 7800 2100 7800
Wire Wire Line
	2200 8150 2250 8150
Wire Wire Line
	2250 8150 2250 7800
Wire Wire Line
	2250 7800 2200 7800
Wire Wire Line
	2300 8150 2350 8150
Wire Wire Line
	2350 8150 2350 7800
Wire Wire Line
	2350 7800 2300 7800
Wire Wire Line
	2400 8150 2450 8150
Wire Wire Line
	2450 8150 2450 7800
Wire Wire Line
	2450 7800 2400 7800
Wire Wire Line
	2500 8150 2550 8150
Wire Wire Line
	2550 8150 2550 7800
Wire Wire Line
	2550 7800 2500 7800
Wire Wire Line
	5900 8150 5950 8150
Wire Wire Line
	5950 8150 5950 7800
Wire Wire Line
	5950 7800 5900 7800
Wire Wire Line
	6000 8150 6050 8150
Wire Wire Line
	6050 8150 6050 7800
Wire Wire Line
	6050 7800 6000 7800
Wire Wire Line
	6100 8150 6150 8150
Wire Wire Line
	6150 8150 6150 7800
Wire Wire Line
	6150 7800 6100 7800
Wire Wire Line
	6200 8150 6250 8150
Wire Wire Line
	6250 8150 6250 7800
Wire Wire Line
	6250 7800 6200 7800
Wire Wire Line
	6300 8150 6350 8150
Wire Wire Line
	6350 8150 6350 7800
Wire Wire Line
	6350 7800 6300 7800
Wire Wire Line
	6400 8150 6450 8150
Wire Wire Line
	6450 8150 6450 7800
Wire Wire Line
	6450 7800 6400 7800
Wire Wire Line
	2200 7800 2200 7750
Wire Wire Line
	2200 7750 2300 7750
Wire Wire Line
	2300 7750 2300 7800
Connection ~ 2200 7800
Connection ~ 2300 7800
Wire Wire Line
	2000 8150 2000 8100
Wire Wire Line
	2000 8100 2500 8100
Wire Wire Line
	2500 8100 2500 8150
Connection ~ 2000 8150
Connection ~ 2500 8150
Wire Wire Line
	5900 8150 5900 8100
Wire Wire Line
	5900 8100 6400 8100
Wire Wire Line
	6400 8100 6400 8150
Wire Wire Line
	6100 7800 6100 7750
Wire Wire Line
	6100 7750 6200 7750
Wire Wire Line
	6200 7750 6200 7800
Connection ~ 6100 7800
Connection ~ 6200 7800
$Comp
L Connector:Conn_01x06_Male J3
U 1 1 605A6780
P 3500 8000
F 0 "J3" V 3562 8244 50  0000 L CNN
F 1 "Conn_01x06_Male" V 3653 8244 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 3500 8000 50  0001 C CNN
F 3 "~" H 3500 8000 50  0001 C CNN
	1    3500 8000
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J4
U 1 1 605A67B4
P 3500 8350
F 0 "J4" V 3562 8594 50  0000 L CNN
F 1 "Conn_01x06_Male" V 3653 8594 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 3500 8350 50  0001 C CNN
F 3 "~" H 3500 8350 50  0001 C CNN
	1    3500 8350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 8150 3350 8150
Wire Wire Line
	3350 8150 3350 7800
Wire Wire Line
	3350 7800 3300 7800
Wire Wire Line
	3400 8150 3450 8150
Wire Wire Line
	3450 8150 3450 7800
Wire Wire Line
	3450 7800 3400 7800
Wire Wire Line
	3500 8150 3550 8150
Wire Wire Line
	3550 8150 3550 7800
Wire Wire Line
	3550 7800 3500 7800
Wire Wire Line
	3600 8150 3650 8150
Wire Wire Line
	3650 8150 3650 7800
Wire Wire Line
	3650 7800 3600 7800
Wire Wire Line
	3700 8150 3750 8150
Wire Wire Line
	3750 8150 3750 7800
Wire Wire Line
	3750 7800 3700 7800
Wire Wire Line
	3800 8150 3850 8150
Wire Wire Line
	3850 8150 3850 7800
Wire Wire Line
	3850 7800 3800 7800
Wire Wire Line
	3500 7800 3500 7750
Wire Wire Line
	3500 7750 3600 7750
Wire Wire Line
	3600 7750 3600 7800
Connection ~ 3500 7800
Connection ~ 3600 7800
Wire Wire Line
	3300 8150 3300 8100
Wire Wire Line
	3300 8100 3800 8100
Wire Wire Line
	3800 8100 3800 8150
Connection ~ 3300 8150
Connection ~ 3800 8150
$Comp
L Connector:Conn_01x06_Male J5
U 1 1 605B6C36
P 4800 8000
F 0 "J5" V 4862 8244 50  0000 L CNN
F 1 "Conn_01x06_Male" V 4953 8244 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 4800 8000 50  0001 C CNN
F 3 "~" H 4800 8000 50  0001 C CNN
	1    4800 8000
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J6
U 1 1 605B6C6A
P 4800 8350
F 0 "J6" V 4862 8594 50  0000 L CNN
F 1 "Conn_01x06_Male" V 4953 8594 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 4800 8350 50  0001 C CNN
F 3 "~" H 4800 8350 50  0001 C CNN
	1    4800 8350
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4600 8150 4650 8150
Wire Wire Line
	4650 8150 4650 7800
Wire Wire Line
	4650 7800 4600 7800
Wire Wire Line
	4700 8150 4750 8150
Wire Wire Line
	4750 8150 4750 7800
Wire Wire Line
	4750 7800 4700 7800
Wire Wire Line
	4800 8150 4850 8150
Wire Wire Line
	4850 8150 4850 7800
Wire Wire Line
	4850 7800 4800 7800
Wire Wire Line
	4900 8150 4950 8150
Wire Wire Line
	4950 8150 4950 7800
Wire Wire Line
	4950 7800 4900 7800
Wire Wire Line
	5000 8150 5050 8150
Wire Wire Line
	5050 8150 5050 7800
Wire Wire Line
	5050 7800 5000 7800
Wire Wire Line
	5100 8150 5150 8150
Wire Wire Line
	5150 8150 5150 7800
Wire Wire Line
	5150 7800 5100 7800
Wire Wire Line
	4800 7800 4800 7750
Wire Wire Line
	4800 7750 4900 7750
Wire Wire Line
	4900 7750 4900 7800
Connection ~ 4800 7800
Connection ~ 4900 7800
Wire Wire Line
	4600 8150 4600 8100
Wire Wire Line
	4600 8100 5100 8100
Wire Wire Line
	5100 8100 5100 8150
Connection ~ 4600 8150
Connection ~ 5100 8150
$Comp
L Connector:Conn_01x06_Male J11
U 1 1 605CAA18
P 7450 8000
F 0 "J11" V 7512 8244 50  0000 L CNN
F 1 "Conn_01x06_Male" V 7603 8244 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 7450 8000 50  0001 C CNN
F 3 "~" H 7450 8000 50  0001 C CNN
	1    7450 8000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	7250 8150 7300 8150
Wire Wire Line
	7300 8150 7300 7800
Wire Wire Line
	7300 7800 7250 7800
Wire Wire Line
	7350 8150 7400 8150
Wire Wire Line
	7400 8150 7400 7800
Wire Wire Line
	7400 7800 7350 7800
Wire Wire Line
	7450 8150 7500 8150
Wire Wire Line
	7500 8150 7500 7800
Wire Wire Line
	7500 7800 7450 7800
Wire Wire Line
	7550 8150 7600 8150
Wire Wire Line
	7600 8150 7600 7800
Wire Wire Line
	7600 7800 7550 7800
Wire Wire Line
	7650 8150 7700 8150
Wire Wire Line
	7700 8150 7700 7800
Wire Wire Line
	7700 7800 7650 7800
Wire Wire Line
	7750 8150 7800 8150
Wire Wire Line
	7800 8150 7800 7800
Wire Wire Line
	7800 7800 7750 7800
Wire Wire Line
	7250 8150 7250 8100
Wire Wire Line
	7250 8100 7750 8100
Wire Wire Line
	7750 8100 7750 8150
Wire Wire Line
	7450 7800 7450 7750
Wire Wire Line
	7450 7750 7550 7750
Wire Wire Line
	7550 7750 7550 7800
Connection ~ 7450 7800
Connection ~ 7550 7800
$Comp
L Connector:Conn_01x06_Male J13
U 1 1 605DFCA9
P 8800 8000
F 0 "J13" V 8862 8244 50  0000 L CNN
F 1 "Conn_01x06_Male" V 8953 8244 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 8800 8000 50  0001 C CNN
F 3 "~" H 8800 8000 50  0001 C CNN
	1    8800 8000
	0    -1   -1   0   
$EndComp
Wire Wire Line
	8600 8150 8650 8150
Wire Wire Line
	8650 8150 8650 7800
Wire Wire Line
	8650 7800 8600 7800
Wire Wire Line
	8700 8150 8750 8150
Wire Wire Line
	8750 8150 8750 7800
Wire Wire Line
	8750 7800 8700 7800
Wire Wire Line
	8800 8150 8850 8150
Wire Wire Line
	8850 8150 8850 7800
Wire Wire Line
	8850 7800 8800 7800
Wire Wire Line
	8900 8150 8950 8150
Wire Wire Line
	8950 8150 8950 7800
Wire Wire Line
	8950 7800 8900 7800
Wire Wire Line
	9000 8150 9050 8150
Wire Wire Line
	9050 8150 9050 7800
Wire Wire Line
	9050 7800 9000 7800
Wire Wire Line
	9100 8150 9150 8150
Wire Wire Line
	9150 8150 9150 7800
Wire Wire Line
	9150 7800 9100 7800
Wire Wire Line
	8600 8150 8600 8100
Wire Wire Line
	8600 8100 9100 8100
Wire Wire Line
	9100 8100 9100 8150
Wire Wire Line
	8800 7800 8800 7750
Wire Wire Line
	8800 7750 8900 7750
Wire Wire Line
	8900 7750 8900 7800
Connection ~ 8800 7800
Connection ~ 8900 7800
Wire Wire Line
	2500 7800 2500 7750
Wire Wire Line
	2500 7750 3300 7750
Wire Wire Line
	3300 7750 3300 7800
Connection ~ 2500 7800
Connection ~ 3300 7800
Wire Wire Line
	3800 7800 3800 7750
Wire Wire Line
	3800 7750 4600 7750
Wire Wire Line
	4600 7750 4600 7800
Connection ~ 3800 7800
Connection ~ 4600 7800
Wire Wire Line
	5100 7800 5100 7750
Wire Wire Line
	5100 7750 5900 7750
Wire Wire Line
	5900 7750 5900 7800
Connection ~ 5100 7800
Connection ~ 5900 7800
Wire Wire Line
	6400 7800 6400 7750
Wire Wire Line
	6400 7750 7250 7750
Wire Wire Line
	7250 7750 7250 7800
Connection ~ 6400 7800
Connection ~ 7250 7800
Wire Wire Line
	7750 7800 7750 7750
Wire Wire Line
	7750 7750 8600 7750
Wire Wire Line
	8600 7750 8600 7800
Connection ~ 7750 7800
Connection ~ 8600 7800
Wire Wire Line
	9100 7800 9100 7700
Wire Wire Line
	9100 7700 2000 7700
Wire Wire Line
	2000 7700 2000 7800
Connection ~ 9100 7800
Connection ~ 2000 7800
Wire Wire Line
	2300 8150 2300 8050
Wire Wire Line
	2300 8050 3500 8050
Wire Wire Line
	3500 8050 3500 8150
Connection ~ 2300 8150
Connection ~ 3500 8150
Wire Wire Line
	3600 8150 3600 8050
Wire Wire Line
	3600 8050 4800 8050
Wire Wire Line
	4800 8050 4800 8150
Connection ~ 3600 8150
Connection ~ 4800 8150
Wire Wire Line
	4900 8150 4900 8050
Wire Wire Line
	4900 8050 6100 8050
Wire Wire Line
	6100 8050 6100 8150
Connection ~ 4900 8150
Wire Wire Line
	6200 8150 6200 8050
Wire Wire Line
	6200 8050 7450 8050
Wire Wire Line
	7450 8050 7450 8150
Wire Wire Line
	7550 8150 7550 8050
Wire Wire Line
	7550 8050 8800 8050
Wire Wire Line
	8800 8050 8800 8150
Wire Wire Line
	8900 8150 8900 8050
Wire Wire Line
	8900 8050 9200 8050
Wire Wire Line
	9200 8050 9200 8400
Wire Wire Line
	9200 8400 1900 8400
Wire Wire Line
	1900 8400 1900 8050
Wire Wire Line
	1900 8050 2200 8050
Wire Wire Line
	2200 8050 2200 8150
Connection ~ 2200 8150
Wire Wire Line
	2000 1300 2000 7700
Connection ~ 2000 1300
Wire Wire Line
	2000 1300 2050 1300
Connection ~ 2000 7700
Wire Wire Line
	1900 8050 1900 1600
Wire Wire Line
	1800 1600 1900 1600
Connection ~ 1900 8050
Connection ~ 1900 1600
Wire Wire Line
	1900 1600 2350 1600
Wire Wire Line
	4800 4550 4800 4650
Wire Wire Line
	4800 4650 5100 4650
Wire Wire Line
	5100 4650 5100 4750
Wire Wire Line
	4650 4550 4800 4550
Wire Wire Line
	4650 4750 4750 4750
Wire Wire Line
	4750 4750 4750 6600
Wire Wire Line
	4750 6600 2100 6600
Wire Wire Line
	2100 6600 2100 7800
Connection ~ 2100 7800
Wire Wire Line
	4650 4650 4750 4650
Wire Wire Line
	4750 4650 4750 4700
Wire Wire Line
	4750 4700 4800 4700
Wire Wire Line
	4800 4700 4800 6650
Wire Wire Line
	4800 6650 2400 6650
Wire Wire Line
	2400 6650 2400 7800
Connection ~ 2400 7800
Wire Wire Line
	4650 4850 4950 4850
Wire Wire Line
	4950 4850 4950 6800
Wire Wire Line
	4950 6800 3700 6800
Wire Wire Line
	3700 6800 3700 7800
Connection ~ 3700 7800
Wire Wire Line
	4650 4950 4900 4950
Wire Wire Line
	4900 4950 4900 6750
Wire Wire Line
	4900 6750 3400 6750
Wire Wire Line
	3400 6750 3400 7800
Connection ~ 3400 7800
Wire Wire Line
	4650 5050 5100 5050
Wire Wire Line
	5100 5050 5100 6950
Wire Wire Line
	5100 6950 5000 6950
Wire Wire Line
	5000 6950 5000 7800
Connection ~ 5000 7800
Wire Wire Line
	4650 5150 5050 5150
Wire Wire Line
	5050 5150 5050 6900
Wire Wire Line
	4700 6900 4700 7800
Wire Wire Line
	4700 6900 5050 6900
Connection ~ 4700 7800
Wire Wire Line
	4650 2750 5300 2750
Wire Wire Line
	5300 2750 5300 2250
Wire Wire Line
	5300 2250 8350 2250
Wire Wire Line
	8350 2250 8350 7000
Wire Wire Line
	8350 7000 6300 7000
Wire Wire Line
	6300 7000 6300 7800
Connection ~ 6300 7800
Wire Wire Line
	4650 2850 5350 2850
Wire Wire Line
	5350 2850 5350 2300
Wire Wire Line
	5350 2300 8300 2300
Wire Wire Line
	8300 2300 8300 6950
Wire Wire Line
	8300 6950 6000 6950
Wire Wire Line
	6000 6950 6000 7800
Connection ~ 6000 7800
Wire Wire Line
	4650 2950 5450 2950
Wire Wire Line
	5450 2950 5450 2400
Wire Wire Line
	5450 2400 8500 2400
Wire Wire Line
	8500 7150 7650 7150
Wire Wire Line
	7650 7150 7650 7800
Wire Wire Line
	8500 2400 8500 7150
Connection ~ 7650 7800
Wire Wire Line
	7350 7800 7350 7100
Wire Wire Line
	7350 7100 8450 7100
Wire Wire Line
	8450 7100 8450 2450
Wire Wire Line
	8450 2450 5500 2450
Wire Wire Line
	5500 2450 5500 3050
Wire Wire Line
	5500 3050 4650 3050
Connection ~ 7350 7800
Wire Wire Line
	4650 3150 5600 3150
Wire Wire Line
	5600 3150 5600 2550
Wire Wire Line
	5600 2550 8650 2550
Wire Wire Line
	8650 2550 8650 7150
Wire Wire Line
	8650 7150 9000 7150
Wire Wire Line
	9000 7150 9000 7800
Connection ~ 9000 7800
Wire Wire Line
	4650 3250 5450 3250
Wire Wire Line
	5450 3250 5450 3100
Wire Wire Line
	5450 3100 5650 3100
Wire Wire Line
	5650 3100 5650 2600
Wire Wire Line
	5650 2600 8600 2600
Wire Wire Line
	8600 2600 8600 7200
Wire Wire Line
	8600 7200 8700 7200
Wire Wire Line
	8700 7200 8700 7800
Connection ~ 8700 7800
$Comp
L Connector:Conn_01x06_Male J9
U 1 1 604668E3
P 6100 8350
F 0 "J9" V 6162 8594 50  0000 L CNN
F 1 "Conn_01x06_Male" V 6253 8594 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 6100 8350 50  0001 C CNN
F 3 "~" H 6100 8350 50  0001 C CNN
	1    6100 8350
	0    -1   -1   0   
$EndComp
Connection ~ 5900 8150
Connection ~ 6100 8150
Connection ~ 6200 8150
Connection ~ 6400 8150
$Comp
L Connector:Conn_01x06_Male J12
U 1 1 60467525
P 7450 8350
F 0 "J12" V 7512 8594 50  0000 L CNN
F 1 "Conn_01x06_Male" V 7603 8594 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 7450 8350 50  0001 C CNN
F 3 "~" H 7450 8350 50  0001 C CNN
	1    7450 8350
	0    -1   -1   0   
$EndComp
Connection ~ 7250 8150
Connection ~ 7450 8150
Connection ~ 7550 8150
Connection ~ 7750 8150
$Comp
L Connector:Conn_01x06_Male J14
U 1 1 604683D9
P 8800 8350
F 0 "J14" V 8862 8594 50  0000 L CNN
F 1 "Conn_01x06_Male" V 8953 8594 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 8800 8350 50  0001 C CNN
F 3 "~" H 8800 8350 50  0001 C CNN
	1    8800 8350
	0    -1   -1   0   
$EndComp
Connection ~ 8600 8150
Connection ~ 8800 8150
Connection ~ 8900 8150
Connection ~ 9100 8150
$Comp
L Mechanical:MountingHole H1
U 1 1 6054FFE1
P 1300 2650
F 0 "H1" H 1400 2696 50  0000 L CNN
F 1 "MountingHole" H 1400 2605 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1300 2650 50  0001 C CNN
F 3 "~" H 1300 2650 50  0001 C CNN
	1    1300 2650
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 605508DE
P 1300 3150
F 0 "H2" H 1400 3196 50  0000 L CNN
F 1 "MountingHole" H 1400 3105 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1300 3150 50  0001 C CNN
F 3 "~" H 1300 3150 50  0001 C CNN
	1    1300 3150
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 60550AD6
P 1300 3800
F 0 "H3" H 1400 3846 50  0000 L CNN
F 1 "MountingHole" H 1400 3755 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 1300 3800 50  0001 C CNN
F 3 "~" H 1300 3800 50  0001 C CNN
	1    1300 3800
	1    0    0    -1  
$EndComp
$EndSCHEMATC
