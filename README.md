# I2C Scan
Simple I2C utility for scanning addresses on a given bus just like i2cdetect from [i2c-tools](https://git.kernel.org/pub/scm/utils/i2c-tools/i2c-tools.git/)
but with a different approach. This utility was created from i2cdetect's inability to detect
all addresses on a given bus, in this utility we write to each register and determine if there
was a response.
