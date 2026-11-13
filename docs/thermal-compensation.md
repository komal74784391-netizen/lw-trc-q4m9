# Thermal compensation notes

The TR-4 uses a 10 kOhm B3950 thermistor in a ratiometric bridge. The firmware
linearises the sensor with a 5-point table stored in EEPROM; the default table is
valid for the production sensor lot.

Because the pass element is linear (LT3080), the board does not emit switching
noise, which matters for crystal-oven measurements. Keep the heater return path
separate from the analogue ground until the star point under the ADC.
