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
L Connector:Conn_01x06_Male J3
U 1 1 60A03B1F
P 4000 3000
F 0 "J3" V 4062 3244 50  0000 L CNN
F 1 "Conn_01x06_Male" V 4153 3244 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 4000 3000 50  0001 C CNN
F 3 "~" H 4000 3000 50  0001 C CNN
	1    4000 3000
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x06_Male J2
U 1 1 60A0AB53
P 3900 4050
F 0 "J2" V 4054 3662 50  0000 R CNN
F 1 "Conn_01x06_Male" V 3963 3662 50  0000 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Horizontal" H 3900 4050 50  0001 C CNN
F 3 "~" H 3900 4050 50  0001 C CNN
	1    3900 4050
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Barrel_Jack_Switch J1
U 1 1 60A0DF8A
P 3150 3550
F 0 "J1" H 3207 3867 50  0000 C CNN
F 1 "Barrel_Jack_Switch" H 3207 3776 50  0000 C CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 3200 3510 50  0001 C CNN
F 3 "~" H 3200 3510 50  0001 C CNN
	1    3150 3550
	1    0    0    -1  
$EndComp
$Comp
L Connector:Barrel_Jack_Switch J4
U 1 1 60A0F330
P 4900 3550
F 0 "J4" H 4670 3500 50  0000 R CNN
F 1 "Barrel_Jack_Switch" H 4670 3591 50  0000 R CNN
F 2 "Connector_BarrelJack:BarrelJack_Horizontal" H 4950 3510 50  0001 C CNN
F 3 "~" H 4950 3510 50  0001 C CNN
	1    4900 3550
	-1   0    0    1   
$EndComp
Wire Wire Line
	3450 3450 3700 3450
Wire Wire Line
	3700 3450 3700 3200
Wire Wire Line
	4200 3200 4200 3450
Wire Wire Line
	4200 3450 3700 3450
Connection ~ 3700 3450
Wire Wire Line
	3700 3450 3700 3750
Wire Wire Line
	4200 3850 4200 3750
Wire Wire Line
	4200 3750 3700 3750
Connection ~ 3700 3750
Wire Wire Line
	3700 3750 3700 3850
Wire Wire Line
	3900 3200 4000 3200
Connection ~ 3900 3200
Wire Wire Line
	3900 3850 4000 3850
Connection ~ 3900 3850
Wire Wire Line
	4200 3750 4600 3750
Wire Wire Line
	4600 3750 4600 3650
Connection ~ 4200 3750
$Comp
L power:+9V #PWR01
U 1 1 60A13509
P 3450 3100
F 0 "#PWR01" H 3450 2950 50  0001 C CNN
F 1 "+9V" H 3465 3273 50  0000 C CNN
F 2 "" H 3450 3100 50  0001 C CNN
F 3 "" H 3450 3100 50  0001 C CNN
	1    3450 3100
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR02
U 1 1 60A13A84
P 3450 3950
F 0 "#PWR02" H 3450 3700 50  0001 C CNN
F 1 "GND" H 3455 3777 50  0000 C CNN
F 2 "" H 3450 3950 50  0001 C CNN
F 3 "" H 3450 3950 50  0001 C CNN
	1    3450 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 3650 3450 3950
Wire Wire Line
	3450 3450 3450 3100
Connection ~ 3450 3450
NoConn ~ 3800 3200
NoConn ~ 4100 3200
NoConn ~ 4100 3850
NoConn ~ 3800 3850
Wire Wire Line
	3900 3200 3900 3550
Wire Wire Line
	3450 3650 3900 3650
Connection ~ 3450 3650
Connection ~ 3900 3650
Wire Wire Line
	3900 3650 3900 3850
Wire Wire Line
	4600 3450 4350 3450
Wire Wire Line
	4350 3450 4350 3550
Wire Wire Line
	4350 3550 3900 3550
Connection ~ 3900 3550
Wire Wire Line
	3900 3550 3900 3650
NoConn ~ 3450 3550
NoConn ~ 4600 3550
$Comp
L power:PWR_FLAG #FLG0101
U 1 1 60A174CA
P 3150 3100
F 0 "#FLG0101" H 3150 3175 50  0001 C CNN
F 1 "PWR_FLAG" H 3150 3273 50  0000 C CNN
F 2 "" H 3150 3100 50  0001 C CNN
F 3 "~" H 3150 3100 50  0001 C CNN
	1    3150 3100
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG0102
U 1 1 60A177DF
P 3150 3950
F 0 "#FLG0102" H 3150 4025 50  0001 C CNN
F 1 "PWR_FLAG" H 3150 4123 50  0000 C CNN
F 2 "" H 3150 3950 50  0001 C CNN
F 3 "~" H 3150 3950 50  0001 C CNN
	1    3150 3950
	-1   0    0    1   
$EndComp
Wire Wire Line
	3150 3100 3450 3100
Connection ~ 3450 3100
Wire Wire Line
	3150 3950 3450 3950
Connection ~ 3450 3950
$Comp
L Mechanical:MountingHole H1
U 1 1 60A1D452
P 2250 3050
F 0 "H1" H 2350 3096 50  0000 L CNN
F 1 "MountingHole" H 2350 3005 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2250 3050 50  0001 C CNN
F 3 "~" H 2250 3050 50  0001 C CNN
	1    2250 3050
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 60A1D83B
P 2250 3250
F 0 "H2" H 2350 3296 50  0000 L CNN
F 1 "MountingHole" H 2350 3205 50  0000 L CNN
F 2 "MountingHole:MountingHole_3.2mm_M3" H 2250 3250 50  0001 C CNN
F 3 "~" H 2250 3250 50  0001 C CNN
	1    2250 3250
	1    0    0    -1  
$EndComp
$EndSCHEMATC
