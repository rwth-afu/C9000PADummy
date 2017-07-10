# C9000PADummy
PA Dummy for Ericsson C9000

This project helps to run a Ericsson C9000 Compact Paging Transmitter without the High Power Amplifier. It implements three fake Power Amplifier I2C-Slaves, which are normally located inside the PA. The used hardware is an Arduino Nano running the software in this repo. It is based on https://github.com/cirthix/SoftIIC  Thanks to the programmer, very good job!

# Software Installation
Just programm the compiled hex file into the arduino. Maybe there is a commando line possibility, too, so the use of Ardunio Stuido is not mandatory.

# Configuration
Besides default I2C Slave answers to the C9000, there is one polling command to read the output power of the PA. This answer has to match with the C9000 setting entered via the C9000 user interface. The last value configured is stored in the ATMega's EEPROM. On first boot, a default value is set and the ATMega afterwards remembers, that is has run already for the first time.

To change the output power setting, just send an 8-bit Byte via the UART with speed 38400 Baud. The Ardunio will take this raw value for its answer towards the C9000 and store it into its EEPROM.

# Wiring
- Connect the Line A4 to SDA and A5 to SCL of the C9000.
- Connect the USB to the already present RasPi having the https://github.com/rwth-afu/RasPagerC9000 PCB on top.
- UniPager https://github.com/rwth-afu/unipager will soon be able to configure the Arduino automatically

# Use and license
The self written parts are subject to the https://creativecommons.org/licenses/by-nc-sa/3.0/de/ license.
