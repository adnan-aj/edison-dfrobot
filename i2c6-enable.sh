#!/bin/bash

# From Intel Edison Kit for Arduino Hardware Guide,
# Dec 2015, Doc Num 331191-005 Page 29

# To configure IO18 and IO19 for I2C connectivity, do the following:
# 1. Refer to Table 2 for the GPIO numbers. According to Table 2,
#    the GPIO numbers for IO18 and IO19 are 28 and 27, respectively.
# 2. According to Table 4, GPIO 204 must be set to 1 to select GPIO/I2C,
#    and GPIO 28 pin-mux must be set to ‘mode1’ to select I2C for IO18.
# 3. According to Table 4, GPIO 205 must be set to 1 to select GPIO/I2C,
#    and GPIO 27 pin-mux must be set to ‘mode1’ to select I2C for IO19.
# 4. GPIO 14 and GPIO 165 are also connected to the I2C signals,
#    and should be configured as high-impedance inputs when I2C is in use
#    on these pins, to prevent them driving a signal on the I2C bus.
# 5. According to Table 7, GPIO 236 must be set to 0 to disable the output
#    direction for GPIO 14, and GPIO 237 must be set to 0 to disable the
#    output direction for GPIO 165.
# 6. According to Table 7, GPIO 212 and 213 must be set as high-impedance
#    inputs to disable the pullup resistors for IO18 and IO19, respectively.
# 7. According to Table 6, the TRI_STATE_ALL signal is controlled by GPIO 214.

# echo 28 > /sys/class/gpio/export
# echo 27 > /sys/class/gpio/export
# echo 204 > /sys/class/gpio/export
# echo 205 > /sys/class/gpio/export
# echo 236 > /sys/class/gpio/export
# echo 237 > /sys/class/gpio/export
# echo 14 > /sys/class/gpio/export
# echo 165 > /sys/class/gpio/export
# echo 212 > /sys/class/gpio/export
# echo 213 > /sys/class/gpio/export
# echo 214 > /sys/class/gpio/export
# echo low > /sys/class/gpio/gpio214/direction
# echo high > /sys/class/gpio/gpio204/direction
# echo high > /sys/class/gpio/gpio205/direction
# echo in > /sys/class/gpio/gpio14/direction
# echo in > /sys/class/gpio/gpio165/direction
# echo low > /sys/class/gpio/gpio236/direction
# echo low > /sys/class/gpio/gpio237/direction
# echo in > /sys/class/gpio/gpio212/direction
# echo in > /sys/class/gpio/gpio213/direction
# echo mode1 > /sys/kernel/debug/gpio_debug/gpio28/current_pinmux
# echo mode1 > /sys/kernel/debug/gpio_debug/gpio27/current_pinmux
# echo high > /sys/class/gpio/gpio214/direction


## For my DIY expansion board with no PCA95xx i2c IO expanders, only 
# the below statements are needed.
echo 28 > /sys/class/gpio/export
echo 27 > /sys/class/gpio/export
echo 14 > /sys/class/gpio/export
echo 165 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio14/direction
echo in > /sys/class/gpio/gpio165/direction
echo mode1 > /sys/kernel/debug/gpio_debug/gpio28/current_pinmux
echo mode1 > /sys/kernel/debug/gpio_debug/gpio27/current_pinmux
