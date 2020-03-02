EESchema Schematic File Version 4
LIBS:oven-monitor-cache
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
L MCU_Module:WeMos_D1_mini U1
U 1 1 5E59EEAB
P 2450 2850
F 0 "U1" H 1900 2500 50  0000 C CNN
F 1 "WeMos_D1_mini" H 1850 2600 50  0000 C CNN
F 2 "Module:WEMOS_D1_mini_light" H 2450 1700 50  0001 C CNN
F 3 "https://wiki.wemos.cc/products:d1:d1_mini#documentation" H 600 1700 50  0001 C CNN
	1    2450 2850
	1    0    0    -1  
$EndComp
$Comp
L adafruit-breakouts:LIS3DH-adafruit A2
U 1 1 5E5B9391
P 3750 3500
F 0 "A2" H 4025 4017 50  0000 C CNN
F 1 "LIS3DH-adafruit" H 4025 3926 50  0000 C CNN
F 2 "adafruit-breakouts:LIS3DH-adafruit" H 3750 3500 50  0001 C CNN
F 3 "~" H 3750 3500 50  0001 C CNN
	1    3750 3500
	1    0    0    -1  
$EndComp
$Comp
L adafruit-breakouts:AD8495-adafruit A1
U 1 1 5E5BAF00
P 3750 2200
F 0 "A1" H 4080 2192 50  0000 L CNN
F 1 "AD8495-adafruit" H 4080 2101 50  0000 L CNN
F 2 "adafruit-breakouts:AD8495-adafruit" H 3750 2200 50  0001 C CNN
F 3 "~" H 3750 2200 50  0001 C CNN
	1    3750 2200
	1    0    0    -1  
$EndComp
$Comp
L adafruit-breakouts:i2c_oled J1
U 1 1 5E5CE016
P 3750 2700
F 0 "J1" H 4230 2692 50  0000 L CNN
F 1 "i2c_oled" H 4230 2601 50  0000 L CNN
F 2 "adafruit-breakouts:i2c_oled" H 3750 2700 50  0001 C CNN
F 3 "~" H 3750 2700 50  0001 C CNN
	1    3750 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 2200 3000 2200
Wire Wire Line
	3000 2200 3000 2350
Wire Wire Line
	3000 2350 2850 2350
Wire Wire Line
	3300 2800 3300 2550
Wire Wire Line
	3300 2550 2850 2550
Wire Wire Line
	3300 2800 3550 2800
Wire Wire Line
	3550 2900 3200 2900
Wire Wire Line
	3200 2900 3200 2650
Wire Wire Line
	3200 2650 2850 2650
Wire Wire Line
	3550 3500 3300 3500
Wire Wire Line
	3300 3500 3300 2800
Connection ~ 3300 2800
Wire Wire Line
	3550 3600 3200 3600
Wire Wire Line
	3200 3600 3200 2900
Connection ~ 3200 2900
Wire Wire Line
	3100 1950 2550 1950
Wire Wire Line
	2550 1950 2550 2050
Wire Wire Line
	3550 2700 3100 2700
Connection ~ 3100 2700
Wire Wire Line
	3100 2700 3100 2400
Wire Wire Line
	3550 2400 3100 2400
Connection ~ 3100 2400
Wire Wire Line
	3100 2400 3100 1950
Wire Wire Line
	3100 3300 3550 3300
Wire Wire Line
	3100 2700 3100 3300
$Comp
L power:GND #PWR0101
U 1 1 5E5D3F2F
P 2450 3650
F 0 "#PWR0101" H 2450 3400 50  0001 C CNN
F 1 "GND" H 2455 3477 50  0000 C CNN
F 2 "" H 2450 3650 50  0001 C CNN
F 3 "" H 2450 3650 50  0001 C CNN
	1    2450 3650
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0102
U 1 1 5E5D4F31
P 3400 3850
F 0 "#PWR0102" H 3400 3600 50  0001 C CNN
F 1 "GND" H 3405 3677 50  0000 C CNN
F 2 "" H 3400 3850 50  0001 C CNN
F 3 "" H 3400 3850 50  0001 C CNN
	1    3400 3850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3550 3400 3400 3400
Wire Wire Line
	3400 3400 3400 3850
Wire Wire Line
	3400 3400 3400 2600
Wire Wire Line
	3400 2600 3550 2600
Connection ~ 3400 3400
Wire Wire Line
	3400 2600 3400 2300
Wire Wire Line
	3400 2300 3550 2300
Connection ~ 3400 2600
$EndSCHEMATC
