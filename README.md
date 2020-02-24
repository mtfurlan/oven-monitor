# Oven Monitor
d1 mini + oled + k type thermocouple to track oven temperature

## Features
* Displays oven temperature if over 70C
* Reports temperature over mqtt every 60 seconds or on a 5 degree change

## Code features
* rolling average for ADC
* The rest is thermostat code

## BOM
* adafruit AD8495 Breakout: https://www.adafruit.com/product/1778
* K type thermocouple
* d1 mini
* oled display i2c SSD1306 128X64
