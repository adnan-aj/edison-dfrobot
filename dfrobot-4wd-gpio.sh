#!/bin/bash

echo mode0 > /sys/kernel/debug/gpio_debug/gpio48/current_pinmux
echo mode0 > /sys/kernel/debug/gpio_debug/gpio49/current_pinmux
echo mode0 > /sys/kernel/debug/gpio_debug/gpio128/current_pinmux
echo mode0 > /sys/kernel/debug/gpio_debug/gpio129/current_pinmux

test -e /sys/class/gpio/gpio48 || echo 48 > /sys/class/gpio/export
echo "Running iwconfig to fix wifi after exporting gpio48"
iwconfig  wlan0 > /dev/null

test -e /sys/class/gpio/gpio49 || echo 49 > /sys/class/gpio/export
test -e /sys/class/gpio/gpio128 || echo 128 > /sys/class/gpio/export
test -e /sys/class/gpio/gpio129 || echo 129 > /sys/class/gpio/export

echo out > /sys/class/gpio/gpio48/direction
echo out > /sys/class/gpio/gpio49/direction
echo out > /sys/class/gpio/gpio128/direction
echo out > /sys/class/gpio/gpio129/direction

echo 0 > /sys/class/gpio/gpio48/active_low
echo 1 > /sys/class/gpio/gpio49/active_low
echo 0 > /sys/class/gpio/gpio128/active_low
echo 1 > /sys/class/gpio/gpio129/active_low

echo 0 > /sys/class/gpio/gpio48/value
echo 0 > /sys/class/gpio/gpio49/value
echo 0 > /sys/class/gpio/gpio128/value
echo 0 > /sys/class/gpio/gpio129/value

chown :i2c /sys/class/gpio/gpio48/value
chmod g+w  /sys/class/gpio/gpio48/value
chown :i2c /sys/class/gpio/gpio49/value
chmod g+w  /sys/class/gpio/gpio49/value
chown :i2c /sys/class/gpio/gpio128/value
chmod g+w  /sys/class/gpio/gpio128/value
chown :i2c /sys/class/gpio/gpio129/value
chmod g+w  /sys/class/gpio/gpio129/value
