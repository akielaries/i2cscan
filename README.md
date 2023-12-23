# I2C Scan
Simple I2C utility for scanning addresses on a given bus just like i2cdetect from [i2c-tools](https://git.kernel.org/pub/scm/utils/i2c-tools/i2c-tools.git/)
but with a different approach. This utility was created from i2cdetect's inability to detect
all addresses on a given bus, in this utility we write to each address on a given bus to determine
if is alive therefore available for communication.

# Usage
```bash
$ i2cscan -h

Available options:
   -h help
   -v version
   -l lists all available I2C busses
   -f fetch all busses & available addresses
   -b [BUS] specify I2C bus
   -a scan all addresses
   -r [FIRST-LAST] scan addresses between FIRST & LAST (int)
   ** For MCUs **
   -mmcu= [MCU]
   Supported MCUs :
       - atmega328
       - atmega2560
   -dev [DEVICE]
```

* Scan I2C bus `/dev/i2c-1` for any available addresses:
  ```bash
  $ i2cscan -b 1 -a
  Scanning all addresses 0 to 127
  Found device at I2C address 0x28
  Found device at I2C address 0x29
  Found device at I2C address 0x53
  Found device at I2C address 0x68
  Found device at I2C address 0x76
  ```
* Scan I2C bus `/dev/i2c-1` for addresses between 0x28(40) - 0x2a(42):
  ```bash
  $ i2cscan -b 1 -r 40-42
  Scanning all addresses 40 to 42
  Found device at I2C address 0x28
  Found device at I2C address 0x29
  ```

# Installation
## Linux Dependencies
* gcc

## AVR dependencies
* avr-libc
* gcc-avr
* binutils-avr
* avrdude

```
# install
$ sudo make install
# uninstall
$ sudo make uninstall
```
